#ifndef __MAIN_H__
#define __MAIN_H__

//Driver
#include "ili9341.h"
#include "LED.h"
#include "tGPPin.h"


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

#include "stm32f429i_discovery_sdram.h"

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

#include "MainBottom.h"

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
