
#ifndef __DIFFTIMER_HW_H__
#define __DIFFTIMER_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "stm32u5xx_hal.h"
#include "ClockInfo.h"

class cDiffTimerHw
{
public:
  // Tim2 32Bit Timer. Set to 1us
  static void InitTimer(u32 lu32InitCnt = 0)
  {
    TIM_HandleTypeDef TimHandle = {};

    __HAL_RCC_TIM2_CLK_ENABLE();

    cClockInfo::Update();

    // The main block of the programmable advanced-control timer is a 16-bit upcounter with its related auto-reload register. The counter clock can be divided by a prescaler.

    TimHandle.Instance = TIM2;
    TimHandle.Init.Period = -1; // 1ms
    TimHandle.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) - 1;
    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM2->CNT = lu32InitCnt;
    TIM2->CR1 |= TIM_CR1_CEN;
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
