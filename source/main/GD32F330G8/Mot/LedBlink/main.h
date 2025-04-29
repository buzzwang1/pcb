#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "gd32f3x0.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_fwdgt.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_timer.h"
#include "gd32f3x0_misc.h"


extern void SysTick_Handler(void);
extern void TIMER2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
