#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg1((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcMyBotNetCfg2((rsz)"Virtual Node", 0, cBotNetAdress::GetSlaveAdr(RomConst_stDevice_Info->u16BnNodeAdr, 14)),
    mcI2c1_SCL_Bn(GPIOB_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c1_SDA_Bn(GPIOB_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C1_BnMaster(I2C1, &mcI2c1_SCL_Bn, &mcI2c1_SDA_Bn, 2, 16, u16GetRomConstBaudDownLink1() * 1000),
    mcComPort2(38400, GPIO_AF7_USART2, 16, 16),
    mcBn_0x1000(&mcMyBotNetCfg1),
    mcNRF905(0x00010110, 0x00010100),
    mcSideLinkRf(&mcNRF905),
    mcSideLink(&mcSideLinkRf, &mcBn_0x1000, 0),
    mcSideLinkBotCom(&mcComPort2),
    mcDownLinks_0x1000_to_0x1100(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1200(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1300(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1400(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1500(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1600(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1700(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1800(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1900(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1A00(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1B00(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1C00(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1D00(&mcI2C1_BnMaster),
    mcDownLinks_0x1000_to_0x1E00(),
    mcBn_0x1E00(&mcMyBotNetCfg2),
    mcUpLinks_0x1E00_to_0x1000(),
    mcU1TxRx(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1),
    mcMasterUartMpHdU1(USART1, cBotNet::enCnstSlaveCnt, u16GetRomConstBaudDownLink2() * 1000, &mcU1TxRx, 5),
    mcDownLinks_0x1E00_to_0x1E10(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E20(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E30(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E40(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E50(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E60(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E70(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E80(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1E90(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1EA0(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1EB0(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1EC0(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1ED0(&mcMasterUartMpHdU1),
    mcDownLinks_0x1E00_to_0x1EE0(&mcMasterUartMpHdU1)
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
      mcBn_0x1000.mcSpop.mcEep = &mcSys.mcBoard.mcEep;
      break;
    }
  }

  // Virtuelle Ports verbinden
  mcUpLinks_0x1E00_to_0x1000.vConnect((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1E00);

  #ifdef PCB_PROJECTCFG_Test
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcSideLinkBotCom, 0xE000);
  #else
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
  #endif

  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1100);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1200);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1300);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1400);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1500);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1600);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1700);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1800);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1900);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1A00);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1B00);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1C00);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1D00);
  mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1E00);
  mcBn_0x1000.vSetHandleHardware(1, True);
  mcBn_0x1000.vSetHandleHardware(14, True);


  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1000.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);


  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcUpLinks_0x1E00_to_0x1000);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E10);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E20);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E30);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E40);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E50);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E60);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E70);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E80);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E90);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EA0);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EB0);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EC0);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1ED0);
  mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EE0);
  mcBn_0x1E00.vSetHandleHardware(1, True);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1E00.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

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
  if (mcBn_0x1000.mcSpop.isBusy())
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
  mcBn_0x1000.vTickHp1ms();
  mcBn_0x1E00.vTickHp1ms();
}

void cSysPkgCom::vTick1msLp(void)
{
  mcBn_0x1000.vProcess(1000);
  mcBn_0x1E00.vProcess(1000);
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
  GPDMA1_Channel8->CCR &= ~DMA_CCR_EN;
  GPDMA1_Channel8->CFCR  = DMA_CFCR_TCF;
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
  GPDMA1_Channel9->CCR &= ~DMA_CCR_EN;
  GPDMA1_Channel9->CFCR = DMA_CFCR_TCF;
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
  GPDMA1_Channel5->CCR &= ~DMA_CCR_EN;
  GPDMA1_Channel5->CFCR = DMA_CFCR_TCF;
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
  GPDMA1_Channel5->CCR &= ~DMA_CCR_EN;
  GPDMA1_Channel5->CFCR = DMA_CFCR_TCF;
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

