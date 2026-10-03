
#include "main.h"


// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 20KB

LED<GPIOB_BASE, 9> lcLedRed;

FDCAN_HandleTypeDef hfdcan1;
FDCAN_FilterTypeDef sFilterConfig;
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
uint8_t TxData0[] = { 0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
uint8_t TxData1[] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
uint8_t TxData2[] = { 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00 };
uint8_t RxData[12];

//cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);


//cBotNet_LinkUsb gcUpLink;


//cBotNet gcBn(&mcMyBotNetCfg);

//cBotNetMsgPortBtr   gcBtr(&gcBn);
//cBotNetMsgPortSpop  gcSpop(&gcBn, &gcBtr);
//cBotNetMsgPortRRpt  gcRRpt(&gcBn);
//cBotNetMsgPortMView gcMView(&gcBn);

//class cBn_MsgProcess : public cBotNet_MsgSysProcess
//{
//  public:
//    cBn_MsgProcess()
//      : cBotNet_MsgSysProcess(&gcBn)
//    {
//    }
//
//  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
//  {
//    u8* lu8RxPayload = lcMsg.GetPayload().mpu8Data;
//    switch (lcMsg.u16GetIdx())
//    {
//      // System
//      case 8: // Request message
//        switch (lu8RxPayload[0])
//        {
//          case 0: // System: Identification
//            // TX 00 | 00 | 10 | RR RR DI DI HW HW RR RR BT : RR: Reserve = 0; DI Device Idx; HW: Hardware Version; BT Board Type
//            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] == 10))
//            {
//              u8 lu8Data[12];
//
//              lu8Data[ 0] =  0; // R1
//              lu8Data[ 1] =  0; // S1
//              lu8Data[ 2] = 10; // S2
//
//              // Reserve
//              lu8Data[ 3] = 0;
//              lu8Data[ 4] = 0;
//              // HV: HW Version
//              lu8Data[ 5] = u16GetRomConstBnDeviceID() >> 8;
//              lu8Data[ 6] = u16GetRomConstBnDeviceID() & 0xFF;;
//
//              // HV: HW Version
//              lu8Data[ 7] = u16GetRomConstHwInfo() >> 8;
//              lu8Data[ 8] = u16GetRomConstHwInfo() & 0xFF;;
//              // SV: SW Version
//              lu8Data[ 9] = 0;
//              lu8Data[10] = 0;
//              // BT Board Type
//              lu8Data[11] = 0; //u8GetRomConstBoardType();
//
//              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x09, lu8Data, sizeof(lu8Data));
//
//              return True;
//            }
//            break;
//         }
//         break;
//
//      default:
//        break;
//    }
//    return False;
//  }
//};

//cBn_MsgProcess mcBn_MsgProcess;


//class cCliCmd_SetPwm: public cCliCmd
//{
//  public:
//
//  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}
//
//  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
//  {
//    char8  lszStrBuf[32];
//    cStr  lszStr(lszStrBuf, 32);
//
//    UNUSED(lcParam);
//    UNUSED(lbFirstCall);
//    UNUSED(lCallerAdr);
//
//    lszStr.Setf((const char8*)"Test\r\n");
//    lcCli->bPrintLn(lszStr);
//      return True;
//  }
//};
//
//class cCliCmd_SetPos: public cCliCmd
//{
//  public:
//
//  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}
//
//  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
//  {
//    char8  lszStrBuf[32];
//    cStr  lszStr(lszStrBuf, 32);
//
//    UNUSED(lcParam);
//    UNUSED(lbFirstCall);
//    UNUSED(lCallerAdr);
//
//    lszStr.Setf((const char8*)"Test\r\n");
//    lcCli->bPrintLn(lszStr);
//
//      return True;
//  }
//};
//
//class cCliCmd_Status: public cCliCmd
//{
//  public:
//    cCliCmd_Status():cCliCmd((const char*)"?", (const char*)"status")
//    {}
//
//    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
//    {
//      char8  lszStrBuf[32];
//      cStr  lszStr(lszStrBuf, 32);
//
//      UNUSED(lcParam);
//      UNUSED(lbFirstCall);
//      UNUSED(lCallerAdr);
//
//      lszStr.Setf((const char8*)"Test\r\n");
//      lcCli->bPrintLn(lszStr);
//
//      return True;
//    }
//};
//
//
//class cBotNetMotCli
//{
//  public:
//  cCliCmd_SetPwm  mcCliCmd_SetPwm;
//  cCliCmd_SetPos  mcCliCmd_SetPos;
//  cCliCmd_Status  mcCliCmd_Status;
//
//  cBotNetMotCli()
//  {
//    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPwm);
//    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPos);
//    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
//  }
//};
//
//cBotNetMotCli mcBnMotCli;
//
//
//cCliCmd_SetPwm    mcCliCmd_SetPwm;
//cCliCmd_SetPos    mcCliCmd_SetPos;
//cCliCmd_Status    mcCliCmd_Status;


//cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_SetPwm,  &mcCliCmd_SetPos,
//                                           &mcCliCmd_Status},  3);


// Exception Handlers (Cortex-M33)
extern "C" {
  void NMI_Handler(void) {}
  void HardFault_Handler(void) { while (1); }
  void MemManage_Handler(void) { while (1); }
  void BusFault_Handler(void) { while (1); }
  void UsageFault_Handler(void) { while (1); }
  void SVC_Handler(void) {}
  void DebugMon_Handler(void) {}
  void PendSV_Handler(void) {}
  void SysTick_Handler(void) {}
}


void MAIN_vTick1msLp(void)
{
  //gcBn.vProcess(1000);
}
u8 gu8LedCnt = 0;
u8 gu8LedCntReload = 10;

void MAIN_vTick100msLp(void)
{
  if (gu8LedCnt) gu8LedCnt--;
  else
  {
    lcLedRed.Toggle();
    gu8LedCnt = gu8LedCntReload;
  }
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
  if (hfdcan->Instance == FDCAN1)
  {
    /* USER CODE BEGIN FDCAN1_MspInit 0 */

    /* USER CODE END FDCAN1_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN1;
    PeriphClkInit.Fdcan1ClockSelection = RCC_FDCAN1CLKSOURCE_PLL1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      //Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_FDCAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_TX
    PD1     ------> FDCAN1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

/**
* @brief FDCAN MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hfdcan: FDCAN handle pointer
* @retval None
*/
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* hfdcan)
{
  if (hfdcan->Instance == FDCAN1)
  {
    /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

    /* USER CODE END FDCAN1_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_FDCAN1_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PB8     ------> FDCAN1_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1);

    /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

    /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

static u32 BufferCmp8b(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}


static void MX_FDCAN1_Init(void)
{
  hfdcan1.Instance                = FDCAN1;

  HAL_FDCAN_MspInit(&hfdcan1);

  hfdcan1.Init.FrameFormat        = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode               = FDCAN_MODE_INTERNAL_LOOPBACK; // FDCAN_MODE_EXTERNAL_LOOPBACK;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause      = ENABLE;
  hfdcan1.Init.ProtocolException  = DISABLE;

  // Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
  // Formel: Bitrate = f_FDCAN / ( (Prescaler + 1) * (TimeSeg1 + TimeSeg2 + 3) )
  // Hier: 80 MHz / ( (9 + 1) * (10 + 3 + 3) ) =
  //       80 MHz /   ( 10    *     16)        = 500 kbit/s
  hfdcan1.Init.ClockDivider         = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.NominalPrescaler     = 10;
  hfdcan1.Init.NominalSyncJumpWidth = 2;
  hfdcan1.Init.NominalTimeSeg1      = 10;               // Prop_Seg + Phase_Seg1
  hfdcan1.Init.NominalTimeSeg2      = 3;                // Phase_Seg2

  // Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
  // Formel: Bitrate = f_FDCAN / ( (Prescaler + 1) * (TimeSeg1 + TimeSeg2 + 3) )
  // Hier: 80 MHz / ( (0 + 1) * (10 + 3 + 3) ) =
  //       80 MHz /   ( 1    *     16)        = 5000 kbit/s
  hfdcan1.Init.DataPrescaler     = 1;
  hfdcan1.Init.DataSyncJumpWidth = 2;
  hfdcan1.Init.DataTimeSeg1      = 10;
  hfdcan1.Init.DataTimeSeg2      =  3;


  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 1;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    //Error_Handler();
  }
}

void MAIN_vInitSystem(void)
{
  /* SysTick end of count event each 10ms */
  HAL_Init();

  cGpPin mMX22917_S1(GPIOC_BASE, 5);
  cGpPin mMX22917_S2(GPIOC_BASE, 4);

  cGpPin mTPS62125_S1(GPIOE_BASE, 7);
  cGpPin mTPS62125_S2(GPIOE_BASE, 8);

  mMX22917_S1.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  mMX22917_S2.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  mTPS62125_S1.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  mTPS62125_S2.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);


  mTPS62125_S2.vSet1();
  mTPS62125_S1.vSet1();

  mMX22917_S1.vSet1();
  mMX22917_S2.vSet1();

  cClockInfo::Update();

  MX_FDCAN1_Init();

  /* Configure standard ID reception filter to Rx FIFO 0 */
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x444;
  sFilterConfig.FilterID2 = 0x555;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  /* Configure extended ID reception filter to Rx FIFO 1 */
  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE_NO_EIDM;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
  sFilterConfig.FilterID1 = 0x1111111;
  sFilterConfig.FilterID2 = 0x2222222;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  /* Configure global filter:
     Filter all remote frames with STD and EXT ID
     Reject non matching frames with STD ID and EXT ID */
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
    //Error_Handler();
  }

  /*##-2 Start FDCAN controller (continuous listening CAN bus) ##############*/
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    //Error_Handler();
  }

  //cBnMsgPool::vInit();

  // Add MsgSys
  //gcBtr.vAddMsgSys();
  //gcSpop.vAddMsgSys();
  //gcRRpt.vAddMsgSys();
  //gcMView.vAddMsgSys();
  //
  //mcBn_MsgProcess.vAddMsgSys();
  //
  //gcBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp    /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL               /*1s_LP*/);

  // Add Uplink, nach der hardware initialisierung
  // gcBn.bAddLink((cBotNet_LinkBase*)&gcUpLink);
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    /* Add message to Tx FIFO */
    TxHeader.Identifier = 0x444;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_12;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
    TxHeader.MessageMarker = 0x52;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData0) != HAL_OK)
    {
      //Error_Handler();
    }

    /* Add second message to Tx FIFO */
    TxHeader.Identifier = 0x1111112;
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_12;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
    TxHeader.MessageMarker = 0xCC;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData1) != HAL_OK)
    {
      //Error_Handler();
    }

    /* Add third message to Tx FIFO */
    TxHeader.Identifier = 0x1111113;
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_12;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
    TxHeader.MessageMarker = 0xDD;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData2) != HAL_OK)
    {
      __asm("nop");
    }

    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) != 1)
    {
      __asm("nop");
    }

    /* Retrieve message from Rx FIFO 0 */
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
      __asm("nop");
    }

    /* Compare payload to expected data */
    if (BufferCmp8b(TxData0, RxData, 12) != 0)
    {
      __asm("nop");
    }
    else
    {
      __asm("nop");
    }


    /* Wait transmissions complete */
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) != 3) {}

    /* Check two messages are received in Rx FIFO 1 */
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO1) != 2)
    {
      __asm("nop");
    }

    /* Retrieve message from Rx FIFO 1 */
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &RxHeader, RxData) != HAL_OK)
    {
      __asm("nop");
    }

    /* Compare payload to expected data */
    if (BufferCmp8b(TxData1, RxData, 12) != 0)
    {
      __asm("nop");
    }
    else
    {
      __asm("nop");
    }


    /* Retrieve next message from Rx FIFO 1 */
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &RxHeader, RxData) != HAL_OK)
    {
      __asm("nop");
    }

    /* Compare payload to expected data */
    if (BufferCmp8b(TxData2, RxData, 12) != 0)
    {
      __asm("nop");
    }
    else
    {
      __asm("nop");
    }

    CycCall_vIdle();
  }
}


// Clock Setup für STM32U575 mit 32 MHz HSE (System: 160 MHz, USB: 48 MHz HSI48 mit CRS)
void SystemClock_Config_U575(void)
{
  // Enable PWR clock
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

  // Set the regulator supply output voltage
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }

  // Switch to SMPS regulator instead of LDO
  //LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  //while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  //{
  //}

  // Enable MSI oscillator
  LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4); // => 4Mhz
  LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
  LL_RCC_MSIS_Enable();
  while (LL_RCC_MSIS_IsReady() != 1)
  {
  }

  // Set FLASH latency
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  // Configure PLL clock source
  LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

  // Enable the EPOD to reach max frequency
  LL_PWR_EnableEPODBooster();
  while (LL_PWR_IsActiveFlag_BOOST() == 0)
  {
  }

  // Main PLL configuration and activation
  // 4Mhz (MSI)  * 80 / 2 => 160Mhz
  LL_RCC_PLL1_EnableDomain_SYS();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
  LL_RCC_PLL1_SetDivider(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);

  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1)
  {
  }

  // Set AHB, APB1, APB2 and APB3 prescalers
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  // Set PLL1 as System Clock Source
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  // Set systick to 1ms with frequency set to 160MHz
  LL_Init1msTick(160000000);

  // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
  LL_SetSystemCoreClock(160000000);

  // USB-Taktquelle auf HSI48 (48 MHz) festlegen
  RCC_PeriphCLKInitTypeDef PeriphClkInit;
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
  PeriphClkInit.Clk48ClockSelection  = RCC_CLK48CLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    while (1);
  }

  LL_PWR_EnableVDDUSB();

  LL_RCC_HSI48_Enable();
  while (!LL_RCC_HSI48_IsReady())
  {
  }

  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_USBFS);

  RCC_CRSInitTypeDef RCC_CRSInitStruct;

  // Clock Recovery System (CRS) für HSI48 aktivieren (Synchronisation via USB-SOF)
  __HAL_RCC_CRS_CLK_ENABLE();
  RCC_CRSInitStruct.Prescaler             = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source                = RCC_CRS_SYNC_SOURCE_USB;
  RCC_CRSInitStruct.Polarity              = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue           = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
  RCC_CRSInitStruct.ErrorLimitValue       = RCC_CRS_ERRORLIMIT_DEFAULT;
  RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;
  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}

void MainSystemInit()
{
  SystemInit();
  SystemClock_Config_U575();
  SystemCoreClockUpdate();
}
