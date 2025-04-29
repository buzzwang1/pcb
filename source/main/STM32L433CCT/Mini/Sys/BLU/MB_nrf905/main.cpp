
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;
LED<GPIOB_BASE, 9> mcLed;
cGpPin lcS1(GPIOC_BASE, 13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
u32   mu32SpopCounter;


// BotNet
cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBotNet mcBn(&mcMyBotNetCfg);

cNRF905                   mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905      mcSideLinkRf(&mcNRF905);
cBotNet_UpLinknRf905Net   mcSideLink(&mcSideLinkRf, &mcBn, 0);


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



void EXTI15_10_IRQHandler(void)
{
  // Adress Match nRF905
  if (__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_13) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_13);
    mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}

void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC2;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC3;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
}

void TIM7_IRQHandler(void)
{
  if (TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}

void MAIN_vTick1msHp(void)
{
  mcBn.vTickHp1ms();
}



void MAIN_vTick1msLp(void)
{
  mcBn.vProcess(1000);

  if (mcBn.mcSpop.isEnable()) mu32SpopCounter = 1000 * 60 * 2; // 2 min

  if (mu32SpopCounter > 10)
  {
    mu32SpopCounter--;
  }
  else
  {
    mu32SpopCounter = 0;
    // 2min no Spop, then reset
    cBnSpop_vJumpToApp(0);
  }
}

void MAIN_vTick100msLp(void)
{
  mcLed.Toggle();
  cBnSpop_vResetWdog();
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  //u8 lu8t;

  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
  cBnSpop_vResetWdog();

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

  mcBn.bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
  mcSideLink.vSetTiming(15 * 1000, 50); // 15s Ping Interval, 50ms warten auf eine Session nach Ping.
  
  mu32SpopCounter = 1000 * 60 * 2;

  //{
  //  u8 lszData[16];
  //
  //  cMemTools::vMemSet((u8*)lszData, 0, 16);
  //  lszData[0] = 'B';
  //  lszData[1] = 'L';
  //  lszData[2] = 'U';
  //
  //  mcSideLink.vSetStatus(0x11, (u8*)lszData);
  //}

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp               /*1ms_LP*/,
                NULL   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL /*1s_LP*/);
  cBnSpop_vResetWdog();
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  if (RomConst_stDevice_Info->u8PartitionCnt < 3)
  {
    // RomConst Error
    mcBn.mcAdr.Set(0x1EEE);
  }

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
  cBnSpop_vResetWdog();
  SystemInit();
  HAL_Init();
  SystemClock_Config_HSE(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

