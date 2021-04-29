
#include "main.h"



// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 24KB



RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> lcLedRed;

tcGPin<GPIOA_BASE, 6> lcSM1A(GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 1);
tcGPin<GPIOA_BASE, 7> lcSM1B(GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 0);
tcGPin<GPIOB_BASE, 0> lcSM1C(GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 0);
tcGPin<GPIOB_BASE, 1> lcSM1D(GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 0);


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


void MAIN_vTick10msLp(void)
{
  static uint8 t = 0;

  t++;
  t &= 3;

  switch (t)
  {
  case 0:
    lcSM1A.vSet1();
    lcSM1B.vSet0();
    lcSM1C.vSet1();
    lcSM1D.vSet0();
    break;
  case 1:
    lcSM1A.vSet0();
    lcSM1B.vSet1();
    lcSM1C.vSet1();
    lcSM1D.vSet0();
    break;
  case 2:
    lcSM1A.vSet0();
    lcSM1B.vSet1();
    lcSM1C.vSet0();
    lcSM1D.vSet1();
    break;
  case 3:
    lcSM1A.vSet1();
    lcSM1B.vSet0();
    lcSM1C.vSet0();
    lcSM1D.vSet1();
    break;
  }
}

void MAIN_vTick100msLp(void)
{
  lcLedRed.Toggle();
}

void MAIN_vTick1000msLp(void)
{
}




void TIM2_vInit();
void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();

  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
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

