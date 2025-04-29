#ifndef __cUSART_MPHD_TIM_H__
#define __cUSART_MPHD_TIM_H__

#include "Typedef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"

#include "ClockInfo.h"

class cBotNet_DownLinkUsartMpHd_Timer
{
  public:

  cBotNet_DownLinkUsartMpHd_Timer()
  {
    vInit();
  }

  void vStart(u16 lui16Time_us)
  {
    // In upcounting mode, the counter counts from 0 to the auto-reload value (content of the TIMx_ARR register), then restarts from 0 and generates a counter overflow event
    TIM15->CNT = 0;
    TIM15->ARR = lui16Time_us;
    TIM15->CR1 |= TIM_CR1_CEN;
  }

  void vStop()
  {
    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM15->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }

  void vInit(void) // __attribute__((optimize("-O0")))
  {
    TIM_HandleTypeDef TimHandle = {};

    __HAL_RCC_TIM15_CLK_ENABLE();

    cClockInfo::Update();

    TIM15->DIER = 0;

    // The main block of the programmable advanced-control timer is a 16-bit upcounter with its related auto-reload register. The counter clock can be divided by a prescaler.

    TimHandle.Instance = TIM15;
    TimHandle.Init.Period            = -1; // 1ms
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) -1;
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
      /* Initialization Error */
      //Error_Handler();
    }

    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM15->DIER |= TIM_DIER_UIE;

    // Enable the TIM15 global Interrupt
    HAL_NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 7, 8);  // Höhere Prio,wegen Asynchron
    /* Enable the TIM15 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
  }
};





#endif // __cUSART_MPHD_TIM_H__
