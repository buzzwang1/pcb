#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg1((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcI2c1_SCL_BnUl(GPIOB_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c1_SDA_BnUl(GPIOB_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C1_BnSlave(I2C1, &mcI2c1_SCL_BnUl, &mcI2c1_SDA_BnUl, DMA1, DMA1_Channel7, DMA1_Channel6, 0, 0, u16GetRomConstBaudUpLink() * 1000),
    mcBn(&mcMyBotNetCfg1),
    mcUpLink(&mcI2C1_BnSlave)
{
}

void cSysPkgCom::vInit1()
{
  //Überprüfen, ob ein externes EEPROM in der Partitionstabelle konfiguriert ist
  const tstMemPart_Partition* lpPartition;
  for (u8 lu8MemIdx = 0; lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt; lu8MemIdx++)
  {
    RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);
    if ((lpPartition->ui16Sort == RomConst_Sort_ExtEep) ||
        (lpPartition->ui16Sort == RomConst_Sort_IntEep))
    {
      mcBn.mcSpop.mcEep = &mcSys.mcBoard.mcEep;
      break;
    }
  }

  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);
}

void cSysPkgCom::vInit2()
{
}

bool cSysPkgCom::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  if (mcI2C1_BnSlave.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2c1Bn";
    lbRet = True;
  }


  return lbRet;
}


bool cSysPkgCom::isReadyForSleep(cStr& lcStatus)
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  // Warten bis SPOP fertig ist  
  if (mcBn.mcSpop.isBusy())
  {
    lszStrBuf.Setf((rsz)"Spop");
    if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
    lcStatus += lszStrBuf;
  }

  return lbRet;
}

void cSysPkgCom::vTick1msHp()
{
  mcBn.vTickHp1ms();
}

void cSysPkgCom::vTick1msLp(void)
{
  mcBn.vProcess(1000);
}

void cSysPkgCom::vTick10msLp(void)
{
}

void cSysPkgCom::vTick100msLp(void)
{
}

void cSysPkgCom::vTick1000msLp(void)
{
}


void I2C1_EV_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcCom.mcI2C1_BnSlave.I2C_EV_IRQHandler();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[0].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[0].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void I2C1_ER_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcCom.mcI2C1_BnSlave.I2C_ER_IRQHandler();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[1].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[1].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}




