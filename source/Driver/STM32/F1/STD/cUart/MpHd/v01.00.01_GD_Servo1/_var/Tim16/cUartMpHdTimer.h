#ifndef __cUSART_MPHD_TIM_H__
#define __cUSART_MPHD_TIM_H__

#include "Typedef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_timer.h"

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
    // In upcounting mode, the counter counts from 0 to the auto-reload value (content of the TIMx_ARR register), then restarts from 0 and generates a counter overflow event
    TIMER_CNT(TIMER16) = 0;
    TIMER_CAR(TIMER16) = lui16Time_us;
    /* auto-reload preload enable */
    timer_enable(TIMER16);
  }

  void vStop()
  {
    timer_interrupt_flag_clear(TIMER16, TIMER_INT_FLAG_UP);
    timer_disable(TIMER16);
  }

  void vInit(void)
  {
    rcu_periph_clock_enable(RCU_TIMER16);

    /*timer_parameter_struct timer_initpara;
     
    // TIMER configuration
    timer_initpara.prescaler         = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1000 - 1; // 1ms
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER16, &timer_initpara);

    timer_interrupt_enable(TIMER16, TIMER_INT_UP);*/

    // Das Gleiche wie oben, braucht aber so 150Byte weniger
    TIMER_DMAINTEN(TIMER16) = 1;
    TIMER_PSC(TIMER16)      = 7;


    /*NVIC config*/
    nvic_irq_enable(TIMER16_IRQn, 1, 1);
  }
};


#endif // __cUSART_MPHD_TIM_H__
