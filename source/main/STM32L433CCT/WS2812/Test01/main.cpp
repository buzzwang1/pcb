
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB


// Test mit 256 LEDs in Reihe
// keine       an: ca. 300mA
// 8 weis 100% an: ca. 750mA ~56,25mA
// 8  rot 100% an: ca. 450mA ~18,75mA
// 8 grün 100% an: ca. 450mA ~18,75mA
// 8 blau 100% an: ca. 450mA ~18,75mA

// 8 weis  50% an: ca. 525mA ~28,12mA
// 8  rot  50% an: ca. 400mA ~12,5mA
// 8 grün  50% an: ca. 400mA ~12,5mA
// 8 blau  50% an: ca. 400mA ~12,5mA



__IO uint32_t TimingDelay = 0;

cClockInfo    mcClockInfo;
LED<GPIOC_BASE, 13> lcLedRed;
cGpPin mcEnable(GPIOA_BASE, 0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);

//tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcComPort(38400, GPIO_AF7_USART2, 2048, 2048);
cWs2812 mcWs2812;


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


void USART1_IRQHandler(void)
{
  lcLedRed.Toggle();
  //mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  //mcComPort.vIRQHandler();
}


void MAIN_vTick1msHp(void)
{
}

void MAIN_vTick1msLp(void)
{
}

void MAIN_vTick10msLp(void)
{
}


/*void MAIN_vTick100msLp(void)
{ 
  static u8 LedIdx = 0x0;
  //mcWs2812.vSetLed(LedIdx, 0, 0, 0);

  LedIdx&=0xFF;

  switch (LedIdx % 8)
  {
    case 0:
      mcWs2812.vSetLed(LedIdx, 128, 128, 128);
    break;
    case 1:
      mcWs2812.vSetLed(LedIdx, 128, 128, 0);
    break;
    case 2:
      mcWs2812.vSetLed(LedIdx, 128, 0, 0);
    break;
    case 3:
      mcWs2812.vSetLed(LedIdx, 0, 128, 0);
    break;
    case 4:
      mcWs2812.vSetLed(LedIdx, 0, 0, 128);
      break;
    case 5:
      mcWs2812.vSetLed(LedIdx, 128, 0, 128);
      break;
    case 6:
      mcWs2812.vSetLed(LedIdx, 128, 128, 0);
      break;
    case 7:
      mcWs2812.vSetLed(LedIdx, 0, 128, 128);
      break;
    break;
  }
  LedIdx++;
}*/


void MAIN_vTick100msLp(void)
{
  static u8 LedIdx = 0x0;
  //mcWs2812.vSetLed(LedIdx, 0, 0, 0);

  if (LedIdx >= (256 + 16)) LedIdx = 0;
  mcWs2812.vSetLed(LedIdx, 32, 32, 32);
  LedIdx++;
}


void vLedXY(u8 x, u8 y, u8 r, u8 g, u8 b)
{
  u16 Idx = y * 16 + x;
  mcWs2812.vSetLed(Idx, r, g, b);
}


void vRect(u8 x1, u8 y1, u8 x2, u8 y2, u8 r, u8 g, u8 b)
{
  u8 x, y;

  for (x = x1; x <= x2; x++)
  {
    vLedXY(x, y1, r, g, b);
    vLedXY(x, y2, r, g, b);
  }

  for (y = (y1+1); y <= (y2-1); y++)
  {
    vLedXY(x1, y, r, g, b);
    vLedXY(x2, y, r, g, b);
  }
}



/*void MAIN_vTick100msLp(void)
{

  static u8 cnt = 0;
  //mcWs2812.vSetLed(LedIdx, 0, 0, 0);

  const u8 ColA[] = {15,31,63,191,255,191,127,63,0,63,127,191,255,191,127,63 };

  cnt++;
  cnt &= 15;

  vRect(7, 7,  8,  8, ColA[(cnt + 0) & 15], 0, 0);
  vRect(6, 6,  9,  9, ColA[(cnt + 1) & 15], 0, 0);
  vRect(5, 5, 10, 10, ColA[(cnt + 2) & 15], 0, 0);
  vRect(4, 4, 11, 11, ColA[(cnt + 3) & 15], 0, 0);
  vRect(3, 3, 12, 12, ColA[(cnt + 4) & 15], 0, 0);
  vRect(2, 2, 13, 13, ColA[(cnt + 5) & 15], 0, 0);
  vRect(1, 1, 14, 14, ColA[(cnt + 6) & 15], 0, 0);
  vRect(0, 0, 15, 15, ColA[(cnt + 7) & 15], 0, 0);
}*/


void MAIN_vTick1000msLp(void)
{
  static u8 LedCnt = 0x0;
  static u8 LedIdx = 0x0;

  lcLedRed.Toggle();

  /*switch (LedCnt % 5)
  {
    case 0:
      mcWs2812.vSetLed(LedIdx+0, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+1, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+2, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+3, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+4, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+5, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+6, 128, 128, 128);
      mcWs2812.vSetLed(LedIdx+7, 128, 128, 128);
    break;
    case 1:
      mcWs2812.vSetLed(LedIdx+0, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+1, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+2, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+3, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+4, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+5, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+6, 0, 0, 0);
      mcWs2812.vSetLed(LedIdx+7, 0, 0, 0);
    break;
    case 2:
      mcWs2812.vSetLed(LedIdx+0, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+1, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+2, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+3, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+4, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+5, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+6, 128, 0, 0);
      mcWs2812.vSetLed(LedIdx+7, 128, 0, 0);
    break;
    case 3:
      mcWs2812.vSetLed(LedIdx+0, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+1, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+2, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+3, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+4, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+5, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+6, 0, 128, 0);
      mcWs2812.vSetLed(LedIdx+7, 0, 128, 0);
    break;
    case 4:
      mcWs2812.vSetLed(LedIdx+0, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+1, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+2, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+3, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+4, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+5, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+6, 0, 0, 128);
      mcWs2812.vSetLed(LedIdx+7, 0, 0, 128);
    break;
  }*/

  //mcWs2812.vSetLed(LedIdx, 0, 0, 0);
  LedCnt++;
  //LedIdx&=0xFF;
  //mcWs2812.vSetLed(LedIdx, 255, 255, 255);

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

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}



int main(void)
{
  MAIN_vInitSystem();

  //mcComPort.vSend("Start\n\r");

  while (1)
  {
    CycCall_vIdle();

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
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
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

