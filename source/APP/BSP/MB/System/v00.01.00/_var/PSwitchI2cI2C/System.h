
#ifndef __BSP_MBv10b2_SYSTEM_H__
#define __BSP_MBv10b2_SYSTEM_H__

#include "CycleCaller.h"
////#include "SysPkgPMon.h"
#include "SysPkgCom.h"
#include "SysPkgClock.h"
#include "SysPkgSMan.h"
#include "SysPkgBoard.h"
#include "SysPkgMsg.h"

class cBnMsgProcessSystem : public cBotNet_MsgSysProcess
{
  public:
  cBnMsgProcessSystem(cBotNet* lcBotNet);

  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
};


class cSystem
{
  public:
  cClockInfo   mcClockInfo;
  cSysPkgSMan  mcSMan;  // SMan vorne, weil hier BuRam und WUF geladen werden
  cSysPkgClock mcClock; // Clock muss vor Powermanager sein, weil der Powermanager die
                        // WUF- zurücksetzt.
   ////cSysPkgPMon  mcPMon;
  cSysPkgBoard mcBoard; // Board mit DigiPower (3V3) vor I2C init, sonst error in I2C_Init.CheckPins
  cSysPkgCom   mcCom;

  cSysPkgMsg   mcSMsg;

  cBnMsgProcessSystem mcBnMsgSys;

  char8  mszErrorInfo_Buf[128];
  cStr   mszErrorInfo;

  cSystem()
    : mcClockInfo(),
      mcSMan(),
      mcClock(),
      ////mcPMon(),
      mcBoard(),
      mcCom(),

      mcSMsg(),
      mcBnMsgSys(&mcCom.mcBn),
      mszErrorInfo((const char8*)mszErrorInfo_Buf, 0, sizeof(mszErrorInfo_Buf))
  {

  }

  void vInit1()
  {
    cClockInfo::Update();
    SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
    cBnMsgPool::vInit();

    cBnSpop_vResetWdog();
    cBnSpop_vDeactivateIWgdDuringSleep();

    mcSMan.vInit1(); // State Manager vorne wegen Wakeup handling
    mcClock.vInit1();
    mcBoard.vInit1();
    ////mcPMon.vInit1();

    mcCom.vInit1();
    mcBnMsgSys.vAddMsgSys();
    mcSMsg.vInit1();
  }

  void vInit2()
  {
    cBnSpop_vResetWdog();

    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYSRUNTIME
    ////    mcPB00.vSet0();
    ////    mcTestClassTim[2].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
    ////  #endif
    ////#endif

    cBuRam::vAddLogPos(10);
    mcBoard.vInit2();
    cBuRam::vAddLogPos(11);
    ////mcPMon.vInit2();
    cBuRam::vAddLogPos(12);
    mcClock.vInit2();
    cBuRam::vAddLogPos(13);
    mcSMan.vInit2();
    cBuRam::vAddLogPos(14);
    mcCom.vInit2();
    cBuRam::vAddLogPos(15);
    mcSMsg.vInit2();

    cBnSpop_vResetWdog();

    u32 lu32Systime_Until;

    // Im EEPROM Konstruktor wurde angegeben, keine Daten einzulesen
    // Das dauert ca. 2ms, volles EEPORM lesen würde da 30ms dauern
    // Damit sollten direkt die Inas drankommen
    // I2C Bausteine Initialisieren
    // Warten bis die Ina initialiert sind

    // Bei Tiefentladener Akku, selbst ausschalten
    bool lbFinish;
    cBuRam::vAddLogPos(16);
    lbFinish = mcBoard.mcBoardCntrl.bStartupCheck();
    if (!lbFinish)
    {
      SysTime_vUpdateTime();
      lu32Systime_Until = SysTime_u32TimeGetTime_ms() + 10/*ms*/;
      while ((!lbFinish) && (SysTime_u32TimeGetTime_ms() < lu32Systime_Until))
      {
        CycCall_vIdle();
        // Check die Ina. Gibt True zurück für den Fall von Error oder Ina-Initialisiert
        // Initialisierung von einem Ina dauert ca. 500us
        lbFinish = mcBoard.mcBoardCntrl.bStartupCheck();
      }
    }


    cBnSpop_vResetWdog();
    cBuRam::vAddLogPos(17);
    // INA ist nicht zwingend verbaut auf dem PSwitch
    // Trotzdem versuchen EEPROM zu laden
    if  (True) // (!mcBoard.mcBoardCntrl.bCheckInaError())
    {
      // 1x Ina alle 3 Kanäle einlesen via IC2 dauert 900us
      // Ina wurde auf 332uS Sampletime eingestellt.
      //        6x 332u = 2ms
      //     Bei 5 Durchläufen ca.
      //       10ms
      // In der Zeit schon mal die erste 256B vom EEPROM einlesen
      //   das sollte ca 8ms dauern
      mcBoard.mcEep.vCmdReadBlock(0, 255);
      lbFinish = False;
      SysTime_vUpdateTime();
      lu32Systime_Until = SysTime_u32TimeGetTime_ms() + 12/*ms*/;
      while ((!lbFinish) && (SysTime_u32TimeGetTime_ms() < lu32Systime_Until))
      {
        CycCall_vIdle();
        lbFinish = mcBoard.mcEep.isIdle() && (mcBoard.mcEep.mControl.StartRequest == 0);
      }

      cBnSpop_vResetWdog();
      if (mcBoard.mcEep.mStatus.IsInit == false)
      {
        // EEPROM Fehler, wahrscheinlich nicht verbaut.
        // EEPROM Ladezeit warten, damit Ina ADC genug Zeit hat.
        SysTime_Delay_us(10000);
      }

      // 1x Ina alle 3 Kanäle einlesen via IC2 dauert 900us
      cBnSpop_vResetWdog();
      mcBoard.mcBoardCntrl.vStartupInaFirstRead();
      lbFinish = False;
      SysTime_vUpdateTime();
      lu32Systime_Until = SysTime_u32TimeGetTime_ms() + 4/*ms*/;
      while ((!lbFinish) && (SysTime_u32TimeGetTime_ms() < lu32Systime_Until))
      {
        CycCall_vIdle();
        lbFinish = mcBoard.mcBoardCntrl.bCheckStartupInaFirstReadFinished();
      }
    }

    // Einmal durchlaufen
    cBuRam::vAddLogPos(18);
    cBnSpop_vResetWdog();
    mcBoard.mcBoardCntrl.vUpdateAll(True);


    // Die restlichen Daten laden
    cBuRam::vAddLogPos(19);
    if (mcBoard.mcEep.mStatus.IsInit)
    {
      mcBoard.mcEep.vCmdReadBlock(256, 1023);
    }
    mszErrorInfo = "";
    mcCom.isError(mszErrorInfo);
    mcBoard.isError(mszErrorInfo);  
    mszErrorInfo.Trim();
    mszErrorInfo.ToString();
    
    #ifdef PCB_PROJECTCFG_Test
      if (mcBoard.mcBoardCntrl.mCellCnt == 0)
      {
        mcBoard.mcBoardCntrl.mcPowerOut->vEnable();
      }
    #endif
    
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYSRUNTIME
    ////    mcPB00.vSet1();
    ////    mcTestClassTim[3].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
    ////  #endif
    ////#endif
  }

  void vTick1msHp(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS1MSHPIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////    mcPA05.vSet1();
    ////  #endif
    ////#endif

    mcClock.vTick1msHp();
    mcBoard.vTick1msHp();
    ////mcPMon.vTick1msHp();
    mcSMan.vTick1msHp();

    mcCom.vTick1msHp();
    mcSMsg.vTick1msHp();

    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS1MSHPIRQ
    ////    mcPA05.vSet0();
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[13].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[13].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }

  void vTick1msLp(void)
  {
    static u16 lu16_1ms = 0;

    cBuRam::vAddLogPos(60);
    mcClock.vTick1msLp();
    cBuRam::vAddLogPos(61);
    mcBoard.vTick1msLp();
    cBuRam::vAddLogPos(62);
    ////mcPMon.vTick1msLp();
    mcSMan.vTick1msLp();

    cBuRam::mBuRam->u32SysTime_ms = SysTime_u32TimeGetTime_ms();

    mcCom.vTick1msLp();
    mcSMsg.vTick1msLp();

    lu16_1ms++;
    if ((lu16_1ms % 10) == 0)
    {
      vTick10msLp();

      if ((lu16_1ms % 100) == 0)
      {
        vTick100msLp();

        if ((lu16_1ms % 1000) == 0)
        {
          vTick1000msLp();
          lu16_1ms = 0;
        }
      }
    }
  }

  void vTick10msLp(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS10MSLPIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////  #endif
    ////#endif

    cBuRam::vAddLogPos(70);
    mcClock.vTick10msLp();
    cBuRam::vAddLogPos(71);
    mcBoard.vTick10msLp();
    cBuRam::vAddLogPos(72);
    ////mcPMon.vTick10msLp();
    cBuRam::vAddLogPos(73);
    mcSMan.vTick10msLp();
    mcCom.vTick10msLp();
    mcSMsg.vTick10msLp();


    ////mcBoardCntrl.vUpdateAll(mcOpMode.IsAuto());
    ////


    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS10MSLPIRQ
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[14].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[14].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }

  void vTick100msLp(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS1000MSLPIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////  #endif
    ////#endif
    cBnSpop_vResetWdog();

    cBuRam::vAddLogPos(80);
    mcClock.vTick100msLp();
    cBuRam::vAddLogPos(81);
    mcBoard.vTick100msLp();
    cBuRam::vAddLogPos(82);
    ////mcPMon.vTick100msLp();
    cBuRam::vAddLogPos(83);
    mcSMan.vTick100msLp();
    mcCom.vTick100msLp();
    mcSMsg.vTick100msLp();


    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS100MSLPIRQ
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[15].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[15].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }


  void vTick1000msLp(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS1000MSLPIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////  #endif
    ////#endif

    cBuRam::vAddLogPos(90);
    mcClock.vTick1000msLp();
    cBuRam::vAddLogPos(91);
    mcBoard.vTick1000msLp();
    cBuRam::vAddLogPos(92);
    ////mcPMon.vTick1000msLp();
    cBuRam::vAddLogPos(93);
    mcSMan.vTick1000msLp();
    mcCom.vTick1000msLp();
    mcSMsg.vTick1000msLp();


    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTSYS1000MSLPIRQ
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[16].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[16].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }
};


extern cSystem mcSys;


#endif // __BSP_MBv10b2_SYSTEM_H__
