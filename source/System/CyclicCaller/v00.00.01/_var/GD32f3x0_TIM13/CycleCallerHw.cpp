
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_1ms_HP;



void TIMER13_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER13, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);
    mptrCycCall_Callback_1ms_HP();
  }
}


void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP)
{
  mptrCycCall_Callback_1ms_HP = lptrCycCall_Callback_1ms_HP;

  timer_parameter_struct timer_initpara;
  rcu_periph_clock_enable(RCU_TIMER2);
  timer_deinit(TIMER2);

  /* TIMER configuration */
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);


  /*NVIC config*/
  nvic_irq_enable(TIMER13_IRQn, 1, 1);

  /* auto-reload preload enable */
  timer_auto_reload_shadow_enable(TIMER2);
  /* auto-reload preload enable */
  timer_enable(TIMER2);
}