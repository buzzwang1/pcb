#ifndef __MAIN_H__
#define __MAIN_H__

//Driver
#include "LED.h"
#include "cSSD1306.h"
#include "htu21d.h"
#include "apds9960.h"

//System
#include "CycleCaller.h"

//String
#include "cStringTools.h"

//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f30x.h"
//#include "stm32f30x_exti.h"
//#include "stm32f30x_adc.h"
 #include "stm32f30x_gpio.h"
//#include "stm32f30x_syscfg.h"
//#include "stm32f30x_i2c.h"
//#include "stm32f30x_iwdg.h"
//#include "stm32f30x_opamp.h"
//#include "stm32f30x_pwr.h"
#include "stm32f30x_rcc.h"
//#include "stm32f30x_rtc.h"
//#include "stm32f30x_spi.h"
#include "stm32f30x_tim.h"
//#include "stm32f30x_usart.h"
//#include "stm32f30x_wwdg.h"
#include "stm32f30x_misc.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

extern void DMA1_Channel1_IRQHandler(void);
extern void DMA1_Channel2_IRQHandler(void);
extern void DMA1_Channel3_IRQHandler(void);
extern void DMA1_Channel4_IRQHandler(void);
extern void DMA1_Channel5_IRQHandler(void);
extern void DMA1_Channel6_IRQHandler(void);
extern void DMA1_Channel7_IRQHandler(void);

extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN