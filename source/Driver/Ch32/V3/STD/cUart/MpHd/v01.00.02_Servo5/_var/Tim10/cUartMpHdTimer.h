#ifndef __cUSART_MPHD_TIM_H__
#define __cUSART_MPHD_TIM_H__

#include "Typedef.h"
#include "ch32v30x.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_tim.h"

#include "ClockInfo.h"

class cUsartMpHd_Slave_Timer
{
  public:

  cUsartMpHd_Slave_Timer()
  {
    vInit();
  }

  void vStart(u16 lui16Time_us)
  {
    TIM10->CNT = lui16Time_us;
    TIM_Cmd(TIM10, ENABLE);
  }

  void vStop()
  {
    TIM_ClearFlag(TIM10, TIM_FLAG_Update); 
    TIM_Cmd(TIM10, DISABLE);
  }

  void vInit(void)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    // TIMER configuration
    TIM_TimeBaseInitStructure.TIM_Prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1; // 1ms;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);
    
    // Das Gleiche wie oben, braucht aber so 150Byte weniger
    //TIMER_DMAINTEN(TIMER16) = 1;
    //TIMER_PSC(TIMER16)      = 7;   //  15 für 16Mhz und 7 für 8Mhz

    // Update Event übernimmt die Werte ins Shadow Register.
    TIM10->SWEVGR = TIM_EventSource_Update;
    TIM_ClearFlag(TIM10, TIM_FLAG_Update);

    TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE); // Hintendran, sonst wird bei Update Event direkt ein Irq ausgelöst

    /*NVIC config*/
    // Prio 0(höchste) 7(kleinste)
    NVIC_SetPriority(TIM10_UP_IRQn, (2 << 5) | (0x01 << 4));
    NVIC_EnableIRQ(TIM10_UP_IRQn);
  }
};


#endif // __cUSART_MPHD_TIM_H__
