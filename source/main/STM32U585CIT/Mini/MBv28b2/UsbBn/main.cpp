
#include "main.h"


// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 20KB

LED<GPIOB_BASE, 9> lcLedRed;

cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);


cBotNet_LinkUsb gcUpLink;


cBotNet gcBn(&mcMyBotNetCfg);

cBotNetMsgPortBtr   gcBtr(&gcBn);
cBotNetMsgPortSpop  gcSpop(&gcBn, &gcBtr);
cBotNetMsgPortRRpt  gcRRpt(&gcBn);
cBotNetMsgPortMView gcMView(&gcBn);

class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:
    cBn_MsgProcess()
      : cBotNet_MsgSysProcess(&gcBn)
    {
    }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    u8* lu8RxPayload = lcMsg.GetPayload().mpu8Data;
    switch (lcMsg.u16GetIdx())
    {
      // System
      case 8: // Request message
        switch (lu8RxPayload[0])
        {
          case 0: // System: Identification
            // TX 00 | 00 | 10 | RR RR DI DI HW HW RR RR BT : RR: Reserve = 0; DI Device Idx; HW: Hardware Version; BT Board Type
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] == 10))
            {
              u8 lu8Data[12];

              lu8Data[ 0] =  0; // R1
              lu8Data[ 1] =  0; // S1
              lu8Data[ 2] = 10; // S2

              // Reserve
              lu8Data[ 3] = 0;
              lu8Data[ 4] = 0;
              // HV: HW Version
              lu8Data[ 5] = u16GetRomConstBnDeviceID() >> 8;
              lu8Data[ 6] = u16GetRomConstBnDeviceID() & 0xFF;;

              // HV: HW Version
              lu8Data[ 7] = u16GetRomConstHwInfo() >> 8;
              lu8Data[ 8] = u16GetRomConstHwInfo() & 0xFF;;
              // SV: SW Version
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              // BT Board Type
              lu8Data[11] = 0; //u8GetRomConstBoardType();

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x09, lu8Data, sizeof(lu8Data));

              return True;
            }
            break;
         }
         break;

      default:
        break;
    }
    return False;
  }
};

cBn_MsgProcess mcBn_MsgProcess;


class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lszStr.Setf((const char8*)"Test\r\n");
    lcCli->bPrintLn(lszStr);
      return True;
  }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lszStr.Setf((const char8*)"Test\r\n");
    lcCli->bPrintLn(lszStr);

      return True;
  }
};

class cCliCmd_Status: public cCliCmd
{
  public:
    cCliCmd_Status():cCliCmd((const char*)"?", (const char*)"status")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lszStr.Setf((const char8*)"Test\r\n");
      lcCli->bPrintLn(lszStr);

      return True;
    }
};


class cBotNetMotCli
{
  public:
  cCliCmd_SetPwm  mcCliCmd_SetPwm;
  cCliCmd_SetPos  mcCliCmd_SetPos;
  cCliCmd_Status  mcCliCmd_Status;

  cBotNetMotCli()
  {
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPwm);
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPos);
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
  }
};

cBotNetMotCli mcBnMotCli;


cCliCmd_SetPwm    mcCliCmd_SetPwm;
cCliCmd_SetPos    mcCliCmd_SetPos;
cCliCmd_Status    mcCliCmd_Status;


cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_SetPwm,  &mcCliCmd_SetPos,
                                           &mcCliCmd_Status},  3);


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

  void tud_vendor_tx_cb(uint8_t idx, uint32_t xfer_bytes)
  {
    (void)idx;
    (void)xfer_bytes;
    gcUpLink.vDataWriteDone();
  }

  void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize)
  {
    (void)idx;
    gcUpLink.vDataReceived(buffer, bufsize);
  }
}



// USB Reset für STM32U575 PHY/Transceiver
void usb_hardware_reset(void)
{
  // Schaltet den USB PHY temporär aus und wieder ein
  HAL_PWREx_DisableVddUSB();
  HAL_Delay(50);
  HAL_PWREx_EnableVddUSB();
}

void MAIN_vTick1msLp(void)
{
  gcBn.vProcess(1000);
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

  // STM32U5: VDDUSB Stromversorgung für USB Peripherie einschalten
  HAL_PWREx_EnableVddUSB();

  cClockInfo::Update();

  cBnMsgPool::vInit();

  // Add MsgSys
  gcBtr.vAddMsgSys();
  gcSpop.vAddMsgSys();
  gcRRpt.vAddMsgSys();
  gcMView.vAddMsgSys();

  mcBn_MsgProcess.vAddMsgSys();

  gcBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp    /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL               /*1s_LP*/);

  // Blue Pill USB-Reset-Schnittstelle triggern
  usb_hardware_reset();

  board_init();
  // TinyUSB Stack initialisieren
  tusb_init();

  // Add Uplink, nach der hardware initialisierung
  gcBn.bAddLink((cBotNet_LinkBase*)&gcUpLink);
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    tud_task(); // Verarbeitet USB-Events im Hintergrund
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
