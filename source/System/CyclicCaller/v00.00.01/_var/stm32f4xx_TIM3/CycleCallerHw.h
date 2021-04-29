
#ifndef __CYCLIC_CALLER_HW_H__
#define __CYCLIC_CALLER_HW_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "TypeDef.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "ClockInfo.h"

typedef void(*CycCallTimer_Callback)(void);

extern void TIM3_IRQHandler(void);
extern void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_500us_HP);

#ifdef __cplusplus
  }
#endif

#endif // __CYCLIC_CALLER_HW_H__
