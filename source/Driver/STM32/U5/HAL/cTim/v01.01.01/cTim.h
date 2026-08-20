#pragma once

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

#include "ClockInfo.h"

class cTimDummy
{
public:

  void vStart(u16 lui16Time_us) { UNUSED(lui16Time_us); }
  void vStop() {}
  void vInit() {}
};

template <u32 mpstTim, const IRQn_Type lenIrq, const u8 mu8Prio1, const u8 mu8Prio2>
class tcTimBasic_1us
{
  public:

  void vStart(u16 lui16Time_us)
  {
    // In upcounting mode, the counter counts from 0 to the auto-reload value (content of the TIMx_ARR register), then restarts from 0 and generates a counter overflow event
    ((TIM_TypeDef *)mpstTim)->CNT = 0;
    ((TIM_TypeDef *)mpstTim)->ARR = lui16Time_us;
    ((TIM_TypeDef *)mpstTim)->CR1 |= TIM_CR1_CEN;
  }

  bool isDone()
  {
    return ((((TIM_TypeDef*)mpstTim)->SR & TIM_SR_UIF) == TIM_SR_UIF);
  }

  void vStop()
  {
    ((TIM_TypeDef*)mpstTim)->SR  &= ~TIM_SR_UIF; // clear UIF flag
    ((TIM_TypeDef*)mpstTim)->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }

  void vInit(void) // __attribute__((optimize("-O0")))
  {
    TIM_HandleTypeDef TimHandle = {};

    switch (mpstTim)
    {
      case TIM7_BASE:__HAL_RCC_TIM7_CLK_ENABLE(); break;
      case TIM15_BASE:__HAL_RCC_TIM15_CLK_ENABLE(); break;
      case TIM16_BASE:__HAL_RCC_TIM16_CLK_ENABLE(); break;
      case TIM17_BASE:__HAL_RCC_TIM17_CLK_ENABLE(); break;
      default: //#error "tcTimBasic_1us: Unkown Timer"
        break;
    }

    cClockInfo::Update();

    ((TIM_TypeDef*)mpstTim)->DIER = 0;

    TimHandle.Instance = (TIM_TypeDef*)mpstTim;
    TimHandle.Init.Period            = 0; // No Auto reload
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) -1;
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_DOWN;
    TimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
      /* Initialization Error */
      //Error_Handler();
    }

    ((TIM_TypeDef*)mpstTim)->SR &= ~TIM_SR_UIF; // clear UIF flag

    if (lenIrq > 0)
    {
      ((TIM_TypeDef*)mpstTim)->DIER |= TIM_DIER_UIE;

      // Enable the Timer's global Interrupt
      HAL_NVIC_SetPriority(lenIrq, mu8Prio1, mu8Prio2);  // Höhere Prio,wegen Asynchron
      HAL_NVIC_EnableIRQ(lenIrq);
    }
  }

  void vEnableIrq()
  {
    HAL_NVIC_EnableIRQ(lenIrq);
  }

  void vDisableIrq()
  {
    HAL_NVIC_DisableIRQ(lenIrq);
  }
};



