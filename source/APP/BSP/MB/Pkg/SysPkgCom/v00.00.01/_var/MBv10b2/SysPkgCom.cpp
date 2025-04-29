#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcI2c1_SCL_Bn(GPIOB_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c1_SDA_Bn(GPIOB_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C1_BnMaster(I2C1, &mcI2c1_SCL_Bn, &mcI2c1_SDA_Bn, DMA1, DMA1_Channel7, DMA1_Channel6, 16, u16GetRomConstBaudDownLink1() * 1000),
    mcComPort2(38400, GPIO_AF7_USART2, 16, 16),
    mcBn(&mcMyBotNetCfg),
    mcNRF905(0x00010110, 0x00010100),
    mcSideLinkRf(&mcNRF905),
    mcSideLink(&mcSideLinkRf, &mcBn, 0),
    mcSideLinkBotCom(&mcComPort2),
    mcDownLinks_0x1000_to_0x1100(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1200(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1300(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1400(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1500(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1600(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1700(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1800(&mcI2C1_BnMaster),
    mcU1TxRx(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1),
    mcMasterUartMpHdU1(USART1, cBotNet::enCnstSlaveCnt, u16GetRomConstBaudDownLink2() * 1000),
    mcDownLinks_0x1000_to_0x1900(&mcMasterUartMpHdU1),
    mcDownLinks_0x1000_to_0x1A00(&mcMasterUartMpHdU1),
    mcDownLinks_0x1000_to_0x1B00(&mcMasterUartMpHdU1),
    mcDownLinks_0x1000_to_0x1C00(&mcMasterUartMpHdU1),
    mcDownLinks_0x1000_to_0x1D00(&mcMasterUartMpHdU1),
    mcDownLinks_0x1000_to_0x1E00(&mcMasterUartMpHdU1)
  {
  }

void cSysPkgCom::vInit1()
{
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 7, 8);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

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

  #ifdef PCB_PROJECTCFG_Test
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcSideLinkBotCom, 0xE000);
  #else
    mcBn.bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
  #endif

  // I2C
  mcBn.vSetHandleHardware(1, True);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1100);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1200);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1300);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1400);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1500);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1600);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1700);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1800);

  // Usart
  mcBn.vSetHandleHardware(9, True);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1900);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1A00);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1B00);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1C00);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1D00);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1E00);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  mcSideLink.vSetTiming(15*1000, 50); // 15s Ping Interval, 50ms warten auf eine Session nach Ping.
}

void cSysPkgCom::vInit2()
{
}

bool cSysPkgCom::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  #ifdef PCB_PROJECTCFG_Test
    if (!mcSideLinkBotCom.mStatus.IsInit)
    {
      lszErrorInfo += (rsz)" ErrSideLnk";
      lbRet = True;
    }
  #else
    if (!mcSideLink.mStatus.IsInit)
    {
      lszErrorInfo += (rsz)" ErrSideLnk";
      lbRet = True;
    }
  #endif

  if (mcI2C1_BnMaster.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2cBn";
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

  // Warten solange active Funk-Verbindung besteht oder Funkverbindung gerade einen Ping durchführt.
  if (((mcSideLink.mStatus.IsOnline) || (!mcSideLink.isReadyForSleep())) &&
       (mcSideLink.mStatus.IsInit))
  {
    lbRet = False;
    if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
    lcStatus += (rsz)"SideLnk";
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

  mcSys.mcCom.mcI2C1_BnMaster.I2C_EV_IRQHandler();

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

  mcSys.mcCom.mcI2C1_BnMaster.I2C_ER_IRQHandler();

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

// ---------------------------- RF ---------------------------

  void EXTI15_10_IRQHandler(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTRFIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////    mcPA05.vSet1();
    ////  #endif
    ////#endif

    if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_13) != 0x00u)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_13);
      mcSys.mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
    }

    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTRFIRQ
    ////    mcPA05.vSet0();
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[4].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[4].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }


void DMA1_Channel2_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC2;
  mcSys.mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[5].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[5].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void DMA1_Channel3_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC3;
  mcSys.mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[6].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[6].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void TIM7_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSys.mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[7].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[7].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}


// ---------------------------- U1 ---------------------------

void DMA2_Channel6_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // USART1 TX
  DMA2_Channel6->CCR &= ~DMA_CCR_EN;
  DMA2->IFCR = DMA_FLAG_TC6;
  mcSys.mcCom.mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[8].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[8].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}


void DMA2_Channel7_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // USART1 RX
  DMA2_Channel7->CCR &= ~DMA_CCR_EN;
  DMA2->IFCR = DMA_FLAG_TC7;
  mcSys.mcCom.mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[9].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[9].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void USART1_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  if (USART1->ISR & LL_USART_ISR_TC)
  {
    USART1->ICR = LL_USART_ISR_TC;
    mcSys.mcCom.mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
  }
  if (USART1->ISR & 0xF)
  {
    USART1->ICR = LL_USART_ISR_ORE;
    mcSys.mcCom.mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
  }

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[10].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[10].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void TIM1_UP_TIM16_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSys.mcCom.mcMasterUartMpHdU1.TIM_EV_IRQHandler();
  }

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU1IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[12].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[12].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

//---------------------------------- U2 --------------------------------

void USART2_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU2IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcCom.mcComPort2.vIRQHandler();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU2IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[11].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[11].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

