#ifndef __cUSART_MPHD_TIM_H__
#define __cUSART_MPHD_TIM_H__

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

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
    TIM16->CNT = 0;
    TIM16->ARR = lui16Time_us;
    TIM16->CR1 |= TIM_CR1_CEN;
  }

  void vStop()
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }

  void vInit(void)
  {
    TIM_HandleTypeDef TimHandle = {};

    __HAL_RCC_TIM16_CLK_ENABLE();

    cClockInfo::Update();

    TIM16->DIER = 0;

    // The main block of the programmable advanced-control timer is a 16-bit upcounter with its related auto-reload register. The counter clock can be divided by a prescaler.

    TimHandle.Instance = TIM16;
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

    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->DIER |= TIM_DIER_UIE;

    // Enable the TIM16 global Interrupt
    HAL_NVIC_SetPriority(TIM16_IRQn, 6, 8);  // Höhere Prio,wegen Asynchron
    /* Enable the TIM16 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM16_IRQn);
  }
};





#endif // __cUSART_MPHD_TIM_H__
