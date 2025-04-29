#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
//#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif


extern void PreMain(void);

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "samd10.h"
#include "drivers.h"
//#include "gd32f1x0.h"
//#include "_stm32f30x_exti.h"
//#include "gd32f1x0_gpio.h"
//#include "_stm32f30x_syscfg.h"
//#include "_stm32f30x_i2c.h"
//#include "gd32f1x0_fwdgt.h"
//#include "_stm32f30x_opamp.h"
//#include "_stm32f30x_pwr.h"
//#include "gd32f1x0_rcu.h"
//#include "stm32f30x_rtc.h"
//#include "stm32f30x_spi.h"
//#include "gd32f1x0_timer.h"
//#include "stm32f30x_usart.h"
//#include "stm32f30x_wwdg.h"
//#include "gd32f1x0_misc.h"


extern void SysTick_Handler(void);
//extern void TIMER2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
