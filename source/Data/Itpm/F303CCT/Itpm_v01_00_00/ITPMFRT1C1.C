#include  "ITPMFRT1c1.h"

//#ifndef ITPM_ENABLED
//  #error Enable Testing by defining ITPM_ENABLED in build options
//#endif


#ifdef ITPM_ENABLED

uint32 ITPM_u32MeasurementTimer_Read(void)
{
  return TIM_GetCounter(TIM2);
}

void ITPM_vMeasurementTimer_Reset(void)
{
  /* Tim2 is 32Bit
     Set to 1us => 4294s => 71min*/
  RCC_ClocksTypeDef lstRCC_Clocks;
  TIM_TimeBaseInitTypeDef lstTim2;

  RCC_GetClocksFreq(&lstRCC_Clocks);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&lstTim2);
  lstTim2.TIM_Prescaler = (lstRCC_Clocks.TIM2CLK_Frequency / 1000000) -1;  // 1us
  lstTim2.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &lstTim2);
  TIM_Cmd(TIM2, ENABLE);
}


#endif
