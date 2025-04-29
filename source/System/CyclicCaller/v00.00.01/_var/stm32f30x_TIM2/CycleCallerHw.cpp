
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_1ms_HP;


/* TIM2 - 32Bit Timer  */


void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag
    mptrCycCall_Callback_1ms_HP();
  }
}


void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP)
{
  TIM_TimeBaseInitTypeDef lstTim;
  NVIC_InitTypeDef NVIC_InitStructure;

  mptrCycCall_Callback_1ms_HP = lptrCycCall_Callback_1ms_HP;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&lstTim);
  lstTim.TIM_Prescaler = (cClockInfo::mstClocks.PCLK2_Frequency / 1000000) - 1;  // 1us
  lstTim.TIM_CounterMode = TIM_CounterMode_Up;
  lstTim.TIM_Period = 500 - 1; // 500us

  lstTim.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTim.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &lstTim);
  TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag

  /* Enable the TIMx global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the TIMx Interrupt Request
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM2, ENABLE);
}