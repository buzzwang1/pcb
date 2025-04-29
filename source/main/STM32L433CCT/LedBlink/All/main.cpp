
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;


LED<GPIOB_BASE, 12> lcLedRed1;              //  GND
LED<GPIOB_BASE, 13> lcLedRed2;              //  GND
LED<GPIOB_BASE, 14> lcLedRed3;              //  3,3V
LED<GPIOB_BASE, 15> lcLedRed4;              //  Reset
LED<GPIOA_BASE,  8> lcLedRed5;              LED<GPIOB_BASE, 11> lcLedRed25;
LED<GPIOA_BASE,  9> lcLedRed6;              LED<GPIOB_BASE, 10> lcLedRed26;
LED<GPIOA_BASE, 10> lcLedRed7;              LED<GPIOB_BASE,  1> lcLedRed27;
LED<GPIOA_BASE, 11> lcLedRed8;              LED<GPIOB_BASE,  0> lcLedRed28;
LED<GPIOA_BASE, 12> lcLedRed9;              LED<GPIOA_BASE,  7> lcLedRed29;
LED<GPIOA_BASE, 15> lcLedRed10;             LED<GPIOA_BASE,  6> lcLedRed30;
LED<GPIOB_BASE,  3> lcLedRed11;             LED<GPIOA_BASE,  5> lcLedRed31;
LED<GPIOB_BASE,  4> lcLedRed12;             LED<GPIOA_BASE,  4> lcLedRed32;
LED<GPIOB_BASE,  5> lcLedRed13;             LED<GPIOA_BASE,  3> lcLedRed33;
LED<GPIOB_BASE,  6> lcLedRed14;             LED<GPIOA_BASE,  2> lcLedRed34;
LED<GPIOB_BASE,  7> lcLedRed15;             LED<GPIOA_BASE,  1> lcLedRed35;
LED<GPIOB_BASE,  8> lcLedRed16;             LED<GPIOA_BASE,  0> lcLedRed36;
LED<GPIOB_BASE,  9> lcLedRed17;             LED<GPIOC_BASE, 15> lcLedRed37;
/* 5V */                                    LED<GPIOC_BASE, 14> lcLedRed38;
/* GND */                                   LED<GPIOC_BASE, 13> lcLedRed39;
/* 3,3V */                                  // VB



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




void MAIN_vTick1000msLp(void)
{
  
  lcLedRed1.Toggle();              //  GND
  lcLedRed2.Toggle();              //  GND
  lcLedRed3.Toggle();              //  3,3V
  lcLedRed4.Toggle();              //  Reset
  lcLedRed5.Toggle();              lcLedRed25.Toggle();
  lcLedRed6.Toggle();              lcLedRed26.Toggle();
  lcLedRed7.Toggle();              lcLedRed27.Toggle();
  lcLedRed8.Toggle();              lcLedRed28.Toggle();
  lcLedRed9.Toggle();              lcLedRed29.Toggle();
  lcLedRed10.Toggle();             lcLedRed30.Toggle();
  lcLedRed11.Toggle();             lcLedRed31.Toggle();
  lcLedRed12.Toggle();             lcLedRed32.Toggle();
  lcLedRed13.Toggle();             lcLedRed33.Toggle();
  lcLedRed14.Toggle();             lcLedRed34.Toggle();
  lcLedRed15.Toggle();             lcLedRed35.Toggle();
  lcLedRed16.Toggle();             lcLedRed36.Toggle();
  lcLedRed17.Toggle();             lcLedRed37.Toggle();
  /* 5V */                         lcLedRed38.Toggle();
  /* GND */                        lcLedRed39.Toggle();
  /* 3,3V */                       // VB
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
    __asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                                       |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                                       |RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }
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
  //SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

