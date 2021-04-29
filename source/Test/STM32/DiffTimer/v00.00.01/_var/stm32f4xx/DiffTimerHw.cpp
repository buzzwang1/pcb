
#include "DiffTimerHw.h"


// Tim2 32Bit Timer. Set to 1us

void InitTimer()
{
  TIM_TimeBaseInitTypeDef lstTim;
  cClockInfo  lcRCC_Clocks;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&lstTim);
  lstTim.TIM_Prescaler = (lcRCC_Clocks.mstClocks.PCLK2_Frequency / 1000000) - 1;  // 1us
  lstTim.TIM_CounterMode = TIM_CounterMode_Up;

  lstTim.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTim.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &lstTim);
  TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag

  TIM_Cmd(TIM2, ENABLE);
};

u32 u32GetTimer(void)
{
  return TIM2->CNT;
}