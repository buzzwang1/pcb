#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

#include "tGPPin.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f30x.h"
//#include "_stm32f30x_exti.h"
#include "stm32f30x_gpio.h"
//#include "_stm32f30x_syscfg.h"
//#include "_stm32f30x_i2c.h"
//#include "_stm32f30x_iwdg.h"
//#include "_stm32f30x_opamp.h"
//#include "_stm32f30x_pwr.h"
#include "stm32f30x_rcc.h"
//#include "stm32f30x_rtc.h"
//#include "stm32f30x_spi.h"
#include "stm32f30x_tim.h"
//#include "stm32f30x_usart.h"
//#include "stm32f30x_wwdg.h"
#include "stm32f30x_misc.h"


extern void TIM2_IRQHandler(void);
extern void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
