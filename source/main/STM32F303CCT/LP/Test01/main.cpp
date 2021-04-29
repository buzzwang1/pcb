
#include "main.h"



// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
uint8 mui8Wfi = 0;

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





void TIM2_vInit();
void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  SystemCoreClockUpdate();
  RCC_GetClocksFreq(&RCC_Clocks);


  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  TIM2_vInit();
}


/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  if(TIM2->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag

    if (!mui8Wfi)
    {
      mui8Wfi = 1;
    }
    else
    {
      mui8Wfi = 0;
    }
  }
}


void TIM2_vInit(void)
{
  /* Timer 2 - 32 Bit Timer*/
  TIM_TimeBaseInitTypeDef lstTim2;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&lstTim2);
  lstTim2.TIM_Prescaler = (SystemCoreClock / 1000) -1;  // 1ms
  lstTim2.TIM_CounterMode = TIM_CounterMode_Up;
  lstTim2.TIM_Period = 5000-1; // 500ms = 5s

  lstTim2.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTim2.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &lstTim2);
  TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the TIM2 Interrupt Request
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM2, ENABLE);
}

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    if (mui8Wfi)
    {
      __asm("wfi");
    }
  }
}

