#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "cUsart_MpHd.h"

//Data
#include "cRingBufT.h"
#include "cBnLinkUsartMpHd.h"
#include "cBnStreamSysPortIap.h"
#include "cBnStreamSysPortIap.h"
#include "cBotnet.h"
#include "cUsart_MpHd.h"
#include "App.h"

#ifdef TEST_BnLinkUsartMpHd
  #include "cTimeStampArray.h"
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "gd32f1x0.h"
//#include "_stm32f30x_exti.h"
#include "gd32f1x0_gpio.h"
//#include "_stm32f30x_syscfg.h"
//#include "_stm32f30x_i2c.h"
#include "gd32f1x0_fwdgt.h"
//#include "_stm32f30x_opamp.h"
//#include "_stm32f30x_pwr.h"
#include "gd32f1x0_rcu.h"
//#include "stm32f30x_rtc.h"
//#include "stm32f30x_spi.h"
#include "gd32f1x0_timer.h"
//#include "stm32f30x_usart.h"
//#include "stm32f30x_wwdg.h"
#include "gd32f1x0_misc.h"

extern void SysTick_Handler(void);

extern void USART0_IRQHandler(void);
extern void TIMER16_IRQHandler(void);

extern void TIMER2_IRQHandler(void);

extern void DMA_Channel1_2_IRQHandler(void);
extern void DMA_Channel3_4_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN