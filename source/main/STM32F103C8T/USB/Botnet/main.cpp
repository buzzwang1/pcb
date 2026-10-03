
#include "main.h"


// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 20KB

LED<GPIOC_BASE, 13> lcLedRed;

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


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



extern "C"
{
  // Wird von TinyUSB automatisch aufgerufen, wenn die HW das Paket gesendet hat
  void tud_vendor_tx_cb(uint8_t idx, uint32_t xfer_bytes)
  {
    (void)idx;
    (void)xfer_bytes;

    // Hardware ist wieder frei für das nächste Paket
    gcUpLink.vDataWriteDone();
  }

  // Offizieller Callback im Unbuffered-Modus (CFG_TUD_VENDOR_TXRX_BUFFERED = 0)
  void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize)
  {
    (void)idx; // Verhindert Warnungen zu ungenutzten Variablen

    gcUpLink.vDataReceived(buffer, bufsize);
  }
} // extern "C"



// Erzwingt, dass der PC das Gerät neu erkennt
void usb_hardware_reset(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA12 (USB D+) kurz hart auf Low ziehen
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(50); // 50ms reichen aus

  // Pin wieder freigeben, damit die USB-Peripherie ihn übernehmen kann
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);
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


void RCC_Config_72MHz_HSE_USB48(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct  = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct  = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    __asm("nop");
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    __asm("nop");
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection    = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    __asm("nop");
  }
}

void MainSystemInit()
{
  SystemInit();
  RCC_Config_72MHz_HSE_USB48();
  SystemCoreClockUpdate();
}
