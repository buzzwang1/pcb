#ifndef __cI2C_TIM_H__
#define __cI2C_TIM_H__

#include "TypeDef.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_misc.h"
#include "ClockInfo.h"

class cBotNet_DownLinkI2c_Timer
{
  public:

  cBotNet_DownLinkI2c_Timer()
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

  void vInit(void)
  {
    TIM_TimeBaseInitTypeDef lstTim;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);

    cClockInfo::Update();


    // The main block of the programmable advanced-control timer is a 16-bit upcounter with its related auto-reload register. The counter clock can be divided by a prescaler.
    TIM_TimeBaseStructInit(&lstTim);
    lstTim.TIM_Prescaler = (cClockInfo::mstClocks.PCLK2_Frequency / 1000000) - 1;  // 1us
    lstTim.TIM_CounterMode = TIM_CounterMode_Up;
    lstTim.TIM_Period = -1;

    lstTim.TIM_ClockDivision = TIM_CKD_DIV1;
    lstTim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM15, &lstTim);
    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag

    /* Enable the TIM15 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable the TIM15 Interrupt Request
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);

  }
};





#endif // __cI2C_TIM_H__
