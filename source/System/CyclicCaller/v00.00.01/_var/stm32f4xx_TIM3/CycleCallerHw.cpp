
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_500us_HP;


/* TIM3 - 16Bit Timer  */

void TIM3_IRQHandler(void)
{
  if (TIM3->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM3->SR &= ~TIM_SR_UIF; // clear UIF flag
    mptrCycCall_Callback_500us_HP();
  }
}


void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_500us_HP)
{
  TIM_TimeBaseInitTypeDef lstTIM;
  NVIC_InitTypeDef NVIC_InitStructure;

  cClockInfo  lcRCC_Clocks;

  mptrCycCall_Callback_500us_HP = lptrCycCall_Callback_500us_HP;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&lstTIM);
  lstTIM.TIM_Prescaler = (lcRCC_Clocks.mstClocks.PCLK2_Frequency / 1000000) - 1;  // 1us
  lstTIM.TIM_CounterMode = TIM_CounterMode_Up;
  lstTIM.TIM_Period = 500 - 1; // 500us

  lstTIM.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTIM.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &lstTIM);
  TIM3->SR &= ~TIM_SR_UIF; // clear UIF flag

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the TIM3 Interrupt Request
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM3, ENABLE);
}