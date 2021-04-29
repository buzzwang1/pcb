
#ifndef __CYCLIC_CALLER_HW_H__
#define __CYCLIC_CALLER_HW_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "TypeDef.h"

typedef void(*CycCallTimer_Callback)(void);

extern void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_500us_HP);

#ifdef __cplusplus
  }
#endif

#endif // __CYCLIC_CALLER_HW_H__
