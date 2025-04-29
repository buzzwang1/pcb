
#ifndef __CYCLIC_CALLER_HW_H__
#define __CYCLIC_CALLER_HW_H__

#include "TypeDef.h"
#include "gd32f3x0.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_timer.h"
#include "gd32f3x0_misc.h"
#include "ClockInfo.h"

#ifdef __cplusplus
  extern "C" {
#endif

typedef void(*CycCallTimer_Callback)(void);

extern void TIMER2_IRQHandler(void);
extern void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP);

#ifdef __cplusplus
  }
#endif

#endif // __CYCLIC_CALLER_HW_H__
