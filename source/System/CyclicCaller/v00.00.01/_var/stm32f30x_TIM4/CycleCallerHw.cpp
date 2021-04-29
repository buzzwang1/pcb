
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_1ms_HP;


/* TIM4 - 16Bit Timer  */
void TIM4_IRQHandler(void)
{
  if (TIM4->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
    mptrCycCall_Callback_1ms_HP();
  }
}


void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP)
{
  TIM_TimeBaseInitTypeDef lstTim4;
  NVIC_InitTypeDef NVIC_InitStructure;

  cClockInfo  lcRCC_Clocks;

  mptrCycCall_Callback_1ms_HP = lptrCycCall_Callback_1ms_HP;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  cClockInfo::Update();

  TIM_TimeBaseStructInit(&lstTim4);
  lstTim4.TIM_Prescaler = (lcRCC_Clocks.mstClocks.PCLK2_Frequency / 1000000) - 1;  // 1us
  lstTim4.TIM_CounterMode = TIM_CounterMode_Up;
  lstTim4.TIM_Period = 1000 - 1; // 500us

  lstTim4.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTim4.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &lstTim4);
  TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag

  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the TIM4 Interrupt Request
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM4, ENABLE);
}
