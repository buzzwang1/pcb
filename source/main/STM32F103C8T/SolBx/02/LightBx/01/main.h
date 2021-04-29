#ifndef __MAIN_H__
#define __MAIN_H__

//Data
#include "cStrT.h"

// Driver
#include "tGPPin.h"
#include "LED.h"
#include "cssd1306.h"
#include "ci2c.h"


//System
#include "ClockInfo.h"
#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
//#include "_stm32f30x_syscfg.h"
//#include "_stm32f30x_i2c.h"
//#include "_stm32f30x_iwdg.h"
//#include "_stm32f30x_opamp.h"
//#include "_stm32f30x_pwr.h"
#include "stm32f10x_rcc.h"
//#include "stm32f30x_rtc.h"
//#include "stm32f30x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
//#include "stm32f30x_wwdg.h"
#include "stm32f10x_misc.h"


//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"



extern void SysTick_Handler(void);

extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);


extern void DMA1_Channel4_IRQHandler(void); // I2C2 TX
extern void DMA1_Channel5_IRQHandler(void); // I2C2 RX
extern void DMA1_Channel6_IRQHandler(void); // I2C1 TX
extern void DMA1_Channel7_IRQHandler(void); // I2C1 RX

#ifdef __cplusplus
}
#endif

#endif  //MAIN
