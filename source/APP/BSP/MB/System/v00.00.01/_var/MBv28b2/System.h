
#ifndef __BSP_MBv10b2_SYSTEM_H__
#define __BSP_MBv10b2_SYSTEM_H__

#include "CycleCaller.h"
#include "SysPkgPMon.h"
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
                        // WUF- zur�cksetzt.
  cSysPkgPMon  mcPMon;
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
      mcPMon(),
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

    cBnSpop_vResetWdog();
    cBnSpop_vDeactivateIWgdDuringSleep();

    mcSMan.vInit1(); // State Manager vorne wegen Wakeup handling
    mcClock.vInit1();
    mcBoard.vInit1();
    mcPMon.vInit1();

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


    mcBoard.vInit2();
    mcPMon.vInit2();
    mcClock.vInit2();
    mcSMan.vInit2();
    mcCom.vInit2();
    mcSMsg.vInit2();

    // Bei Tiefentladener Akku, selbst ausschalten
    cBnSpop_vResetWdog();

    // Im EEPROM Konstruktor wurde angegeben, keine Daten einzulesen
    // Das dauert ca. 2ms, volles EEPORM lesen w�rde da 30ms dauern
    // Damit sollten direkt die Inas drankommen
    // I2C Bausteine Initialisieren
    // Warten bis die Ina initialiert sind
    u16 lu16Timeout_500us;
    bool lbFinish;
    if (mcBoard.mcI2C2_Board.bStartNext())
    {
      lbFinish = False;
      lu16Timeout_500us = 20;
      while ((!lbFinish) && (lu16Timeout_500us))
      {
        mcBoard.mcI2C2_Board.bStartNext();
        SysTime_Delay_us(500);
        // Check die Ina. Gibt True zur�ck f�r den Fall von Error oder Ina-Initialisiert
        // Initialisierung von einem Ina dauert ca. 500us
        lbFinish = mcBoard.mcLipoMon.bStartupCheck();
      }
    }

    cBnSpop_vResetWdog();
    if (!mcBoard.mcLipoMon.bCheckInaError())
    {
      // 1x Ina alle 3 Kan�le einlesen via IC2 dauert 900us
      // Ina wurde auf 332uS Sampletime eingestellt.
      //        6x 332u = 2ms
      //     Bei 5 Durchl�ufen ca.
      //       10ms
      // In der Zeit schon mal die erste 256 vom EEPROM einlesen
      //   das sollte ca 8ms dauern
      mcBoard.mcEep.vCmdReadBlock(0, 255);
      lbFinish = False;
      lu16Timeout_500us = 24;
      while ((!lbFinish) && (lu16Timeout_500us))
      {
        mcBoard.mcI2C2_Board.bStartNext();
        SysTime_Delay_us(500);
        lbFinish = mcBoard.mcEep.isIdle();
      }

      cBnSpop_vResetWdog();
      if (mcBoard.mcEep.mStatus.IsInit == false)
      {
        // EEPROM Fehler, wahrscheinlich nicht verbaut.
        // EEPROM Ladezeit warten, damit Ina ADC genug Zeit hat.
        SysTime_Delay_us(10000);
      }

      // 1x Ina alle 3 Kan�le einlesen via IC2 dauert 900us
      cBnSpop_vResetWdog();
      mcBoard.mcLipoMon.vStartupInaFirstRead();
      lbFinish = False;
      lu16Timeout_500us = 8;
      while ((!lbFinish) && (lu16Timeout_500us))
      {
        mcBoard.mcI2C2_Board.bStartNext();
        SysTime_Delay_us(500);
        lbFinish = mcBoard.mcLipoMon.bCheckStartupInaFirstReadFinished();
      }
    }

    // Einmal durchlaufen um Batterie-Spannung zu testen
    cBnSpop_vResetWdog();
    mcBoard.mcLipoMon.vUpdateAll_10ms(True);

    // Die restlichen Daten laden
    if (mcBoard.mcEep.mStatus.IsInit)
    {
      mcBoard.mcEep.vCmdReadBlock(256, 1023);
    }

    mszErrorInfo = "";
    mcCom.isError(mszErrorInfo);
    mcBoard.isError(mszErrorInfo);
    mszErrorInfo.Trim();
    mszErrorInfo.ToString();

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
    mcPMon.vTick1msHp();
    mcSMan.vTick1msHp();

    if (mcSMan.mcPowerManager.enSysState == cPowerManager::tenSysStates::nStRun)
    {
      mcCom.vTick1msHp();
      mcSMsg.vTick1msHp();
    }

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

    mcClock.vTick1msLp();
    mcBoard.vTick1msLp();
    mcPMon.vTick1msLp();
    mcSMan.vTick1msLp();


    if (mcSMan.mcPowerManager.enSysState == cPowerManager::tenSysStates::nStRun)
    {
      mcCom.vTick1msLp();
      mcSMsg.vTick1msLp();
    }

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

    mcClock.vTick10msLp();
    mcBoard.vTick10msLp();
    mcPMon.vTick10msLp();
    mcSMan.vTick10msLp();
    mcCom.vTick10msLp();
    mcSMsg.vTick10msLp();


    ////mcLipoMon.vUpdateAll(mcOpMode.IsAuto());
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

    mcClock.vTick100msLp();
    mcBoard.vTick100msLp();
    mcPMon.vTick100msLp();
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

    mcClock.vTick1000msLp();
    mcBoard.vTick1000msLp();
    mcPMon.vTick1000msLp();
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
