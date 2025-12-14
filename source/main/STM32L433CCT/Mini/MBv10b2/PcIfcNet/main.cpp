
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz


__IO uint32_t TimingDelay = 0;
LED<GPIOB_BASE, 9> mcLed;
LED<GPIOA_BASE, 6> mcLedBar;
cGpPin lcS1(GPIOC_BASE, 13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin lcS_3V3(GPIOA_BASE, 10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);


// BotNet
cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBotNet gcBn(&mcMyBotNetCfg);

tcUart<USART2_BASE, GPIOA_BASE, 2, GPIOA_BASE, 3> mcComPort2(38400, GPIO_AF7_USART2, 16, 16);
cBotNet_LinkBotCom         mcUpLinkBotCom(&mcComPort2);

cNRF905Master              mcNRF905Master(0x00010110, 0x00010100);
cBotNet_DownLinknRf905Net  gcDownLink(&mcNRF905Master, &gcBn);
bool mbConnected = False;


void NMI_Handler(void)
{
  while (1)
  {
  }
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
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
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

class cBnMsgHandlerApp : public cBotNet_MsgSysProcess
{
public:
  cBnMsgHandlerApp()
    : cBotNet_MsgSysProcess(&gcBn)
  {
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    // Überprüfen,obes eine Session Start/Stop Nachricht ist
    //
    //                 0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
    // Wait & Start:  11 | DH DL | FE | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || DA DA E0 00 80 : Versucht mit DH.DL:DA.DA zu verbinden
    // Disconnect:    11 | DH DL | FF | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || DA DA E0 00 80
    u8* lu8RxPayload = lcMsg.mpu8Data;
    if (lcMsg.u8Len() == 25)
    {
      // Info Nachricht ?
      // Nachricht vom PC
      if ((lu8RxPayload[24] == 0x80) &&
           (lu8RxPayload[0] == 0x11))
      {
        //Start
        if ((lu8RxPayload[3] == 0xFE) && (mbConnected == False))
        {
          gcDownLink.mu16SessionDAdr  = (lu8RxPayload[ 1] << 8) + lu8RxPayload[2];
          gcDownLink.mu16SessionBnAdr = (lu8RxPayload[20] << 8) + lu8RxPayload[21];

          gcBn.mcAdr.Set((lu8RxPayload[22] << 8) + lu8RxPayload[23]);
          gcBn.mcDeviceID.Set((lu8RxPayload[4] << 8) + lu8RxPayload[5]);

          gcDownLink.mbSessionStart = True;
          gcDownLink.mbSessionStop  = False;
          //lcLed2.On();
          //mcTextPort.vAddText((const char8*)"Wait for Connection\r\n");
          //mcDisplayTimer.vReset();
          return True;
        }

        //Stop
        if (lcMsg[3] == 0xFF)
        {
          gcDownLink.mbSessionStop = True;
          //mcTextPort.vAddText((const char8*)"Abort Connection\r\n");
          //mcDisplayTimer.vReset();
          //lcLed2.Off();
          return True;
        }
      }
    }
    return False;
  }
};

cBnMsgHandlerApp gcBnMsgHandlerApp;

void EXTI15_10_IRQHandler(void)
{
  // Adress Match nRF905
  if (__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_13) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_13);
    mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvUsartExtiP1);
  }
}

void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC2;
  mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvDmaRxTc);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC3;
  mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvDmaTxTc);
}

void TIM7_IRQHandler(void)
{
  if (TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvUsartTimer);
  }
}

//---------------------------------- U2 --------------------------------

void USART2_IRQHandler(void)
{
  mcComPort2.vIRQHandler();
}

void MAIN_vTick1msLp(void)
{
  gcBn.vProcess(1000);
  mcNRF905Master.vTick1ms();
}

void MAIN_vTick10msLp(void)
{
  // Check Connected Status
  // Überprüfen,obes eine Session Start/Stop Nachricht ist
  //
  //                 0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
  // Wait & Start:  11 | DH DL | FE | SH.SL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || E0 00 DA DA 80
  // Disconnect:    11 | DH DL | FF | SH.SL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || E0 00 DA DA 80

  if ((!mbConnected) && (gcDownLink.IsOnline()))
  {
    mbConnected = True;
    //lcLed2.Off();
    //mcTextPort.vAddText((const char8*)"Connected\r\n");
    //mcDisplayTimer.vReset();

    // Send Connection Acknowledge
    u8 u8MsgData[] = { 0x11, (u8)(gcDownLink.mu16SessionDAdr >> 8), (u8)gcDownLink.mu16SessionDAdr, 0xFE,
                             (u8)(gcBn.mcDeviceID.Get() >> 8), (u8)gcBn.mcDeviceID.Get(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             (u8)(gcBn.mcAdr.Get() >> 8), (u8)gcBn.mcAdr.Get(), (u8)(gcDownLink.mu16SessionBnAdr >> 8), (u8)gcDownLink.mu16SessionBnAdr, 0x80 };

    u8 lu8PoolIdx;
    cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, sizeof(u8MsgData));
    if (lu8PoolIdx)
    {
      lcMsgTx.Set(u8MsgData, sizeof(u8MsgData));
      cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.Len());
      mcUpLinkBotCom.bPut(lu8PoolIdx);
      cBnMsgPool::vReleaseMsg(lu8PoolIdx);
    }
  }


  if ((mbConnected) && (!gcDownLink.IsOnline()))
  {
    mbConnected = False;
    //lcLed2.Off();
    //mcTextPort.vAddText((const char8*)"Disconnected\r\n");
    //mcDisplayTimer.vReset();

    // Send Dis-Connection Acknowledge
    u8 u8MsgData[] = { 0x11, (u8)(gcDownLink.mu16SessionDAdr >> 8), (u8)gcDownLink.mu16SessionDAdr, 0xFF,
                             (u8)(gcBn.mcDeviceID.Get() >> 8), (u8)gcBn.mcDeviceID.Get(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             (u8)(gcBn.mcAdr.Get() >> 8), (u8)gcBn.mcAdr.Get(), (u8)(gcDownLink.mu16SessionBnAdr >> 8), (u8)gcDownLink.mu16SessionBnAdr, 0x80 };

    u8 lu8PoolIdx;
    cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, sizeof(u8MsgData));
    if (lu8PoolIdx)
    {
      lcMsgTx.Set(u8MsgData, sizeof(u8MsgData));
      cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.Len());
      mcUpLinkBotCom.bPut(lu8PoolIdx);
      cBnMsgPool::vReleaseMsg(lu8PoolIdx);
    }
  }
}
void MAIN_vTick100msLp(void)
{
  if (mbConnected)
  {
    mcLed.Toggle();
    mcLedBar.Toggle();
  }
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
  if (!mbConnected)
  {
    mcLed.Toggle();
    mcLedBar.Toggle();
  }
}


void MAIN_vInitSystem(void)
{
  //u8 lu8t;

  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
  cBnMsgPool::vInit();

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  mcUpLinkBotCom.menType = cBotNet_LinkBase::tenType::enUpLink;
  gcBn.bAddLink((cBotNet_LinkBase*)&mcUpLinkBotCom);

  // Nrf905
  // AddLink initialisiert beim Slave auch direkt die HW
  // Wenn man das zu schnell nach Reset macht, dann kann sein dass der 3 nrf905 noch nicht bereit ist.
  // Device Switching Times: PWR_DWN -> ST_BY mode: 3 ms
  gcBn.bAddLink((cBotNet_LinkBase*)&gcDownLink);
  gcBnMsgHandlerApp.vAddMsgSys();

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp    /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config_HSE(void)
{
  // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct   = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  // Configure the main internal regulator output voltage
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config_HSE(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

