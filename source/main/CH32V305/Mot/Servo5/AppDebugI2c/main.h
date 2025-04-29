#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "cI2c.h"
//#include "cUsart_MpHd.h"

//Data
#include "cRingBufT.h"
#include "cBotnet.h"
#include "cUsart_MpHd.h"
#include "cBnLinkI2c.h"
//#include "cBnLinkUsartMpHd.h"
#include "App.h"

#ifdef TEST_BnLinkUsartMpHd
  #include "cTimeStampArray.h"
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_conf.h"


#define FSTIRQ "WCH-Interrupt-fast"

extern void NMI_Handler(void)       __attribute__((interrupt(FSTIRQ)));
extern void HardFault_Handler(void) __attribute__((interrupt(FSTIRQ)));

extern void MainSystemInit(void);

extern void SysTick_Handler(void)          __attribute__((interrupt(FSTIRQ)));
extern void USART3_IRQHandler(void)        __attribute__((interrupt(FSTIRQ)));
extern void TIM10_UP_IRQHandler(void)      __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt(FSTIRQ)));

extern void I2C2_EV_IRQHandler(void)        __attribute__((interrupt(FSTIRQ)));
extern void I2C2_ER_IRQHandler(void)        __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel4_IRQHandler(void)  __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel5_IRQHandler(void)  __attribute__((interrupt(FSTIRQ)));


#ifdef __cplusplus
}
#endif

#endif  //MAIN
