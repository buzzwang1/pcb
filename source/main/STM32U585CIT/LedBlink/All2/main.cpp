
#include "main.h"


// STM32U585
// ARM®-based Cortex®-M33 32b MCU, (160 MHz max)
// Rom 2048KB
// Ram 768KB

__IO uint32_t TimingDelay = 0;

#define BLUEPILL_BTB_100P

#ifdef BLUEPILL
// Bluepill
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
#endif

#ifdef BLUEPILL_BTB
// Bluepill
LED<GPIOB_BASE, 12> lcLedRed1;              //  GND
LED<GPIOB_BASE, 13> lcLedRed2;              //  GND
LED<GPIOB_BASE, 14> lcLedRed3;              //  3,3V
LED<GPIOB_BASE, 15> lcLedRed4;              //  Reset
LED<GPIOA_BASE, 8> lcLedRed5;               LED<GPIOB_BASE, 11> lcLedRed25;
LED<GPIOA_BASE, 9> lcLedRed6;               LED<GPIOB_BASE, 10> lcLedRed26;
LED<GPIOA_BASE, 10> lcLedRed7;              LED<GPIOB_BASE, 1> lcLedRed27;
LED<GPIOA_BASE, 11> lcLedRed8;              LED<GPIOB_BASE, 0> lcLedRed28;
LED<GPIOA_BASE, 12> lcLedRed9;              LED<GPIOA_BASE, 7> lcLedRed29;
LED<GPIOA_BASE, 15> lcLedRed10;             LED<GPIOA_BASE, 6> lcLedRed30;
LED<GPIOB_BASE, 3> lcLedRed11;              LED<GPIOA_BASE, 5> lcLedRed31;
LED<GPIOB_BASE, 4> lcLedRed12;              LED<GPIOA_BASE, 4> lcLedRed32;
LED<GPIOB_BASE, 5> lcLedRed13;              LED<GPIOA_BASE, 3> lcLedRed33;
LED<GPIOB_BASE, 6> lcLedRed14;              LED<GPIOA_BASE, 2> lcLedRed34;
LED<GPIOB_BASE, 7> lcLedRed15;              LED<GPIOA_BASE, 1> lcLedRed35;
LED<GPIOB_BASE, 8> lcLedRed16;              LED<GPIOA_BASE, 0> lcLedRed36;
LED<GPIOB_BASE, 9> lcLedRed17;              LED<GPIOB_BASE, 2> lcLedRed37;
/* 5V */                                    // NC
/* GND */                                   LED<GPIOC_BASE, 13> lcLedRed39;
/* 3,3V */                                  // VB
#endif


#ifdef BLUEPILL_BTB_100P
// Bluepill
LED<GPIOB_BASE, 12> lcLedRed1;              //  GND
LED<GPIOB_BASE, 13> lcLedRed2;              //  GND
LED<GPIOB_BASE, 14> lcLedRed3;              //  3,3V
LED<GPIOB_BASE, 15> lcLedRed4;              //  Reset
LED<GPIOA_BASE, 8> lcLedRed5;               LED<GPIOE_BASE,  8> lcLedRed25;
LED<GPIOA_BASE, 9> lcLedRed6;               LED<GPIOB_BASE, 10> lcLedRed26;
LED<GPIOA_BASE, 10> lcLedRed7;              LED<GPIOB_BASE, 1> lcLedRed27;
LED<GPIOA_BASE, 11> lcLedRed8;              LED<GPIOB_BASE, 0> lcLedRed28;
LED<GPIOA_BASE, 12> lcLedRed9;              LED<GPIOA_BASE, 7> lcLedRed29;
LED<GPIOA_BASE, 15> lcLedRed10;             LED<GPIOA_BASE, 6> lcLedRed30;
LED<GPIOB_BASE, 3> lcLedRed11;              LED<GPIOA_BASE, 5> lcLedRed31;
LED<GPIOB_BASE, 4> lcLedRed12;              LED<GPIOA_BASE, 4> lcLedRed32;
LED<GPIOB_BASE, 5> lcLedRed13;              LED<GPIOA_BASE, 3> lcLedRed33;
LED<GPIOB_BASE, 6> lcLedRed14;              LED<GPIOA_BASE, 2> lcLedRed34;
LED<GPIOB_BASE, 7> lcLedRed15;              LED<GPIOA_BASE, 1> lcLedRed35;
LED<GPIOB_BASE, 8> lcLedRed16;              LED<GPIOA_BASE, 0> lcLedRed36;
LED<GPIOB_BASE, 9> lcLedRed17;              LED<GPIOB_BASE, 2> lcLedRed37;
/* 5V */                                    // NC
/* GND */                                   LED<GPIOC_BASE, 13> lcLedRed39;
/* 3,3V */                                  // VB
#endif

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




void MAIN_vTick100msLp(void)
{
  static u8 lu8i = 0;

  GPIOA->ODR = 0;
  GPIOB->ODR = 0;
  GPIOC->ODR = 0;
  GPIOE->ODR = 0;

  lu8i++;

  switch (lu8i)
  {
    case 1:  lcLedRed1.vSet1();              /* GND */           break;
    case 2:  lcLedRed2.vSet1();              /* GND */           break;
    case 3:  lcLedRed3.vSet1();              /* 3,3V */          break;
    case 4:  lcLedRed4.vSet1();              /* Reset */         break;
    case 5:  lcLedRed5.vSet1();              lcLedRed25.vSet1(); break;
    case 6:  lcLedRed6.vSet1();              lcLedRed26.vSet1(); break;
    case 7:  lcLedRed7.vSet1();              lcLedRed27.vSet1(); break;
    case 8:  lcLedRed8.vSet1();              lcLedRed28.vSet1(); break;
    case 9:  lcLedRed9.vSet1();              lcLedRed29.vSet1(); break;
    case 10: lcLedRed10.vSet1();             lcLedRed30.vSet1(); break;
    case 11: lcLedRed11.vSet1();             lcLedRed31.vSet1(); break;
    case 12: lcLedRed12.vSet1();             lcLedRed32.vSet1(); break;
    case 13: lcLedRed13.vSet1();             lcLedRed33.vSet1(); break;
    case 14: lcLedRed14.vSet1();             lcLedRed34.vSet1(); break;
    case 15: lcLedRed15.vSet1();             lcLedRed35.vSet1(); break;
    case 16: lcLedRed16.vSet1();             lcLedRed36.vSet1(); break;
    case 17: lcLedRed17.vSet1();             lcLedRed37.vSet1(); break;
    case 18: /* 5V */                        /* nc */            break;
    case 19: /* GND */                       lcLedRed39.vSet1(); break;
    case 20: /* 3,3V */                      /* VB */            break;
    default: lu8i = 0; break;
  }
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
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL               /*1s_LP*/);

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

  uwTickPrio = 0;

  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

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

  RCC->CR |= RCC_CR_HSEON;
  RCC->BDCR |= RCC_BDCR_LSEON;
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

