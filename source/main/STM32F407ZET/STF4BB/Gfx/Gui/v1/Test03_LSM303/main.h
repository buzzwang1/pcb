#ifndef __MAIN_H__
#define __MAIN_H__


#include "LED.h"
#include "tGPPin.h"

//System
#include "ClockInfo.h"
#include "CycleCaller.h"
#include "DiffTimer.h"
#include "lcd.h"

//Graphics
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "rfont.h"
#include "paint.h"
#include "uielement.h"
#include "uieletree.h"
#include "cursor.h"
#include "msg.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"


#include "ci2c.h"
#include "LSM303DLHC_MAG.h"
#include "LSM303DLHC_ACC.h"


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f4xx.h"
//#include "_stm32f4xx_exti.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_fsmc.h"
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
#include "misc2.h"




extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);


extern void DMA1_Stream0_IRQHandler(void);
extern void DMA1_Stream1_IRQHandler(void);
extern void DMA1_Stream2_IRQHandler(void);
extern void DMA1_Stream3_IRQHandler(void);
extern void DMA1_Stream4_IRQHandler(void);
extern void DMA1_Stream5_IRQHandler(void);
extern void DMA1_Stream6_IRQHandler(void);

extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN