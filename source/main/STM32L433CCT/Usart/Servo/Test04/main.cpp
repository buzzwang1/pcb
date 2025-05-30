
#include "main.h"



// STM32L433
// ARM�-based Cortex�-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> lcLedRed;

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


tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort2(115200, GPIO_AF7_USART2, 64, 64);
tcUart<USART3_BASE, GPIOB_BASE, 10, GPIOB_BASE, 11> mcComPort3(115200, GPIO_AF7_USART3, 64, 64);


void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcComPort2.vIRQHandler();
}

void USART3_IRQHandler(void)
{
  mcComPort3.vIRQHandler();
}


void MAIN_vTick1000msLp(void)
{
  static u8 u8Test = 0;

  lcLedRed.Toggle();

  switch (u8Test)
  {
    case 0:
      // Status
      mcComPort3.vAdd((char8)0xFC);
      mcComPort3.vAdd((char8)0xCF);
      mcComPort3.vAdd((char8)0x1f);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x20);
      mcComPort3.vSend(0xed);
    break;

    case 1:
      // Status
      mcComPort3.vAdd((char8)0xFC);
      mcComPort3.vAdd((char8)0xCF);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x02);
      mcComPort3.vSend(0xed);
    break;
    case 2:
      // Status
      mcComPort3.vAdd((char8)0xFA);
      mcComPort3.vAdd((char8)0xAF);
      mcComPort3.vAdd((char8)0x1f);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x20);
      mcComPort3.vSend(0xed);
    break;

    case 3:
      // Status
      mcComPort3.vAdd((char8)0xFA);
      mcComPort3.vAdd((char8)0xAF);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x02);
      mcComPort3.vSend(0xed);
    break;

    case 4:
      // Status
      mcComPort3.vAdd((char8)0x55);
      mcComPort3.vAdd((char8)0x55);
      mcComPort3.vAdd((char8)0x1f);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x20);
      mcComPort3.vSend(0xed);
    break;

    case 5:
      // Status
      mcComPort3.vAdd((char8)0x55);
      mcComPort3.vAdd((char8)0x55);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x01);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x00);
      mcComPort3.vAdd((char8)0x02);
      mcComPort3.vSend(0xed);
    break;
  }

  u8Test++;
  if (u8Test >= 8) u8Test = 0;
}


void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

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

  // Uart3 auf Half-Duplex umstellen.
  LL_USART_Disable(USART3);
  LL_USART_EnableHalfDuplex(USART3);
  LL_USART_Enable(USART3);


  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

}



int main(void) 
{
  MAIN_vInitSystem();


  while (1)
  {
    CycCall_vIdle();

    while (mcComPort2.mcUartDataIn.cnt() > 0)
    {
      u8 lu8DataIn = mcComPort2.mcUartDataIn.get();
      LL_USART_SetTransferDirection(USART3, LL_USART_DIRECTION_TX); // Rx ausschalten
      mcComPort3.vSend(lu8DataIn);
      //LL_USART_SetTransferDirection(USART3, LL_USART_DIRECTION_TX_RX); // Alles einschalten
    }

    while (mcComPort3.mcUartDataIn.cnt() > 0)
    {
      u8 lu8DataIn = mcComPort3.mcUartDataIn.get();
      mcComPort2.vSend(lu8DataIn);
    }

    //__asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1|
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
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
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

