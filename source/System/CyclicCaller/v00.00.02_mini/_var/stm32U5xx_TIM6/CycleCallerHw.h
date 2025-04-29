
#ifndef __CYCLIC_CALLER_HW_H__
#define __CYCLIC_CALLER_HW_H__

#include "TypeDef.h"
#include "stm32u5xx_hal.h"
#include "ClockInfo.h"

#ifdef __cplusplus
  extern "C" {
#endif

extern void CycCall_vMain_Hp_1ms(void);

extern void TIM6_IRQHandler(void);

extern void CycCallHw_vInit();
#define CycCallHw_u16GetCnt() (TIM6->CNT) 

#ifdef __cplusplus
  }
#endif

#endif // __CYCLIC_CALLER_HW_H__
