
#ifndef __DIFFTIMER_HW_H__
#define __DIFFTIMER_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "ClockInfo.h"

class cDiffTimerHw
{
public:
  // Tim2 32Bit Timer. Set to 1us
  static void InitTimer(u32 lu32InitCnt = 0)
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

    TIM2->CNT = lu32InitCnt;
    TIM_Cmd(TIM2, ENABLE);
  };

  static u32 u32GetTimer(void)
  {
    return TIM2->CNT;
  }
};

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_HW_H__
