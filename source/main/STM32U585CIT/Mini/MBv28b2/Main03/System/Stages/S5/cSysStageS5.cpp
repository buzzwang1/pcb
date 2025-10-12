#include "cSysStageS5.h"
#include "cSysStages.h"

// ------------------ cCom ----------------------------

cCom::cCom()
  : mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcI2c3_SCL_Bn(GPIOC_BASE, 0, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c3_SDA_Bn(GPIOC_BASE, 1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C3_BnMaster(I2C3, &mcI2c3_SCL_Bn, &mcI2c3_SDA_Bn, 4, 16, u16GetRomConstBaudDownLink1() * 1000, 0),
    mcComPort1(38400, GPIO_AF7_USART1, 16, 16),
    mcBn(&mcMyBotNetCfg),
    mcNRF905(0x00010110, 0x00010100),
    mcSideLinkRf(&mcNRF905),
    mcSideLink(&mcSideLinkRf, &mcBn, 1), // 1x10ms warten bis zum Start vom Ping, weil NRF905 erst in bAddedToBn initialisiert wird
    mcSideLinkBotCom(&mcComPort1),
    mcDownLinks_0x1000_to_0x1100(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1200(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1300(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1400(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1500(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1600(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1700(&mcI2C3_BnMaster),
    mcDownLinks_0x1000_to_0x1800(&mcI2C3_BnMaster),
    mcU2TxRx(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1),
    mcMasterUartMpHdU2(USART2, cBotNet::enCnstSlaveCnt, u16GetRomConstBaudDownLink2() * 1000, &mcU2TxRx, 5),
    mcDownLinks_0x1000_to_0x1900(&mcMasterUartMpHdU2),
    mcDownLinks_0x1000_to_0x1A00(&mcMasterUartMpHdU2),
    mcDownLinks_0x1000_to_0x1B00(&mcMasterUartMpHdU2),
    mcDownLinks_0x1000_to_0x1C00(&mcMasterUartMpHdU2),
    mcDownLinks_0x1000_to_0x1D00(&mcMasterUartMpHdU2),
    mcDownLinks_0x1000_to_0x1E00(&mcMasterUartMpHdU2)
{
}

void cCom::vInit()
{
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 7, 8);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  mcI2C3_BnMaster.vInitHw();

  //Überprüfen, ob ein externes EEPROM in der Partitionstabelle konfiguriert ist
  //// const tstMemPart_Partition* lpPartition;
  //// for (u8 lu8MemIdx = 0; lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt; lu8MemIdx++)
  //// {
  ////   RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);
  ////   if ((lpPartition->ui16Sort == RomConst_Sort_ExtEep) ||
  ////     (lpPartition->ui16Sort == RomConst_Sort_IntEep))
  ////   {
  ////     mcBn.mcSpop.mcEep = &mcSys.mcBoard.mcEep;
  ////     break;
  ////   }
  //// }

  #ifdef PCB_PROJECTCFG_Test
    mcBn.bAddLink((cBotNet_LinkBase*)&mcSideLinkBotCom, 0xE000);
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

  mcSideLink.vSetTiming(15 * 1000, 50); // 15s Ping Interval, 50ms warten auf eine Session nach Ping.
}



bool cCom::isError(cStr& lszErrorInfo)
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

  if (mcI2C3_BnMaster.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2cBn";
    lbRet = True;
  }

  return lbRet;
}


bool cCom::isReadyForSleep(cStr& lcStatus)
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


cSysStageReq cStage5::mcReq;

// ------------------ cStage5 init ----------------------------

cGpPin                    cStage5::mcWakeupPin(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cStatusLed<GPIOB_BASE, 9> cStage5::mcStatusLed(False);

cGpPin cStage5::mcI2c2_SCL_Board(GPIOB_BASE, 13, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin cStage5::mcI2c2_SDA_Board(GPIOB_BASE, 14, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cI2cMaster  cStage5::mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 1, 8, 400000, False);

cBoardCntr  cStage5::mcBoardCntr(&mcI2C2_Board);

osThreadId_t cStage5::mTaskBoard2Handle = 0;
bool         cStage5::mbInitBoard2Finished = False;
bool         cStage5::mbRunBoard2 = True;
bool         cStage5::mbDeInitBoard2Finished = False;



osThreadId_t cStage5::mTaskComHandle = 0;
bool         cStage5::mbInitComFinished = False;
bool         cStage5::mbRunCom = True;
bool         cStage5::mbDeInitComFinished = False;
u32          cStage5::mu32ComTime_us = 0;
cCom         cStage5::mcCom;


// ------------------- Task Board2 --------------------------------

void cStage5::vDoInitBoard2()
{
  {
    TIM_HandleTypeDef TimHandle = {};

    // 1 ms Tick Timer starten
    __HAL_RCC_TIM6_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM6_IRQn, 15, 8);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);

    TimHandle.Instance = TIM6;
    TimHandle.Init.Period = 1000 - 1; // 1ms
    TimHandle.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) - 1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TimHandle);
    HAL_TIM_Base_Start_IT(&TimHandle);
  }

  mcI2C2_Board.vInitHw();
  mcBoardCntr.vInit();

  // I2C Teilnehmer initialisieren
  for (u8 lu8t = 1; lu8t < 40; lu8t++)
  {
    mcI2C2_Board.bStartNext();
    if (mcBoardCntr.isInit()) break;
    osDelay(1);
  }
}

void cStage5::vDoRunBoard2()
{
  static u8 lu8Tick100ms = 0;

  cStage1::mcSysTick.vDoProcess();

  mcI2C2_Board.bStartNext();
  mcBoardCntr.vUpdateAll_10ms(/*isAuto*/ True);

  if (lu8Tick100ms == 100)
  {
    mcStatusLed.vTick100ms();
    lu8Tick100ms = 0;
  }

  lu8Tick100ms += 10;
  osDelay(10);
}

void cStage5::vDoDeInitBoard2()
{
  u8 liTimeout_ms = 0;
    
  while (!mcBoardCntr.isDeInit())
  {
    mcI2C2_Board.bStartNext();
    mcBoardCntr.vDeInit();
    osDelay(10);
    
    if (liTimeout_ms >= 40) break;
    liTimeout_ms += 10;
  }

  // 1 ms Tick Timer stoppen
  TIM6->CR1 &= ~(TIM_CR1_CEN);
  __HAL_RCC_TIM6_CLK_DISABLE();
}

// ------------------- Task Com --------------------------------


void cStage5::vDoInitCom()
{
  cStage5::mcCom.vInit();
  mu32ComTime_us = cSysTick::u32Get_us();
}

void cStage5::vDoRunCom()
{
  
  //void cSysPkgCom::vTick1msHp()
  //{
  //  mcBn.vTickHp1ms();
  //}
  //
  u32 lu32NewTime_us  = cSysTick::u32Get_us();
  u32 lu23DiffTime_us = cTimer32Tools::u32Diff(mu32ComTime_us, lu32NewTime_us);
  mu32ComTime_us = lu32NewTime_us;
  
  cStage5::mcCom.mcBn.vProcess(lu23DiffTime_us);

  osDelay(1);
}

void cStage5::vTickHp1ms()
{
  cStage5::mcCom.mcBn.vTickHp1ms();
}


void cStage5::vDoDeInitCom()
{
}

// --------------------------- 1ms Timer ---------------------------

void TIM6_IRQHandler(void)
{
  if (TIM6->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM6->SR &= ~TIM_SR_UIF; // clear UIF flag
    cStage5::vTickHp1ms();
  }
}


// --------------------- Board2 -----------------------------------------
// ---------------------- I2C2   ----------------------------------------

void I2C2_EV_IRQHandler(void)
{
  cStage5::mcI2C2_Board.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  cStage5::mcI2C2_Board.I2C_ER_IRQHandler();
}

// --------------------- cCom -----------------------------------------

void I2C3_EV_IRQHandler(void)
{
  cStage5::mcCom.mcI2C3_BnMaster.I2C_EV_IRQHandler();
}

void I2C3_ER_IRQHandler(void)
{
  cStage5::mcCom.mcI2C3_BnMaster.I2C_ER_IRQHandler();
}

// ---------------------------- RF ---------------------------

void EXTI5_IRQHandler(void)
{
  if (__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_5) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_5);
    cStage5::mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}



void SPI1_IRQHandler(void)
{
  if (SPI1->SR & SPI_FLAG_EOT)
  {
    SPI1->CR1 &= ~(SPI_CR1_SPE);
    SPI1->IFCR = 0xFFFF; // Clear all flags
  }

  #ifndef PCB_PROJECTCFG_Test
    if (cStage5::mcCom.mcNRF905.mSpi.mu8ModeTx == 1)
    {
      cStage5::mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
    }
    else
    {
      cStage5::mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
    }
  #endif
}

void TIM7_IRQHandler(void)
{
  if (TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    cStage5::mcCom.mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}


// ---------------------------- U1 ---------------------------

void GPDMA1_Channel5_IRQHandler(void)
{
  // USART1 TX
  GPDMA1_Channel5->CCR &= ~DMA_CCR_EN;
  GPDMA1_Channel5->CFCR = DMA_CFCR_TCF;
  cStage5::mcCom.mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTc);
}


void USART2_IRQHandler(void)
{
  if (USART2->ISR & LL_USART_ISR_TC)
  {
    USART2->ICR = LL_USART_ISR_TC;
    cStage5::mcCom.mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
  }
  if (USART2->ISR & 0xF)
  {
    USART2->ICR = LL_USART_ISR_ORE;
    cStage5::mcCom.mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
  }
}

void TIM16_IRQHandler(void)
{
  if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    cStage5::mcCom.mcMasterUartMpHdU2.TIM_EV_IRQHandler();
  }
}

//---------------------------------- U1 --------------------------------

void USART1_IRQHandler(void)
{
  cStage5::mcCom.mcComPort1.vIRQHandler();
}


