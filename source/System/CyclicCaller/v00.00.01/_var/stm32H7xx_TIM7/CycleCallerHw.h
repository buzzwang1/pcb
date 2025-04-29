
#ifndef __CYCLIC_CALLER_HW_H__
#define __CYCLIC_CALLER_HW_H__

#include "TypeDef.h"
#include "stm32h7xx_hal.h"
#include "ClockInfo.h"

#ifdef __cplusplus
  extern "C" {
#endif

typedef void(*CycCallTimer_Callback)(void);

extern void TIM7_IRQHandler(void);
extern void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP);

#ifdef __cplusplus
  }
#endif

#endif // __CYCLIC_CALLER_HW_H__
