#ifndef __MAIN_H__
#define __MAIN_H__

//Driver
#include "ili9341.h"
#include "LED.h"
#include "tGPPin.h"
#include "ov7670.h"


//System
#include "CycleCaller.h"
#include "ClockInfo.h"

//Graphics
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "rfont.h"
#include "rfont.h"
#include "paint.h"
#include "uielement.h"
#include "uieletree.h"
#include "cursor.h"
#include "msg.h"

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

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"


extern void DMA2_Stream6_IRQHandler(void);
extern void DMA2_Stream1_IRQHandler(void);
extern void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
