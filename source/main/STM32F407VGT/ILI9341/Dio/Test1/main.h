#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f4xx.h"
//#include "_stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"
//#include "_stm32f4xx_syscfg.h"
//#include "_stm32f4xx_i2c.h"
//#include "_stm32f4xx_iwdg.h"
//#include "_stm32f4xx_opamp.h"
//#include "_stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_rtc.h"
//#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
//#include "stm32f4xx_usart.h"
//#include "stm32f4xx_wwdg.h"
#include "misc.h"

#include "LED.h"
#include "tGPPin.h"
#include "ili9341.h"
#include "screen.h"


//System
#include "CycleCaller.h"


extern void TIM2_IRQHandler(void);
extern void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
