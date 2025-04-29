#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"
#include "RomConst.h"

//System
#include "ClockInfo.h"

//Data
#include "cRingBufT.h"
#include "cBnLinkUsartMpHd.h"
#include "cBotnet.h"
#include "cUsart_MpHd.h"
#include "cBnSpop.h"

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


#ifdef __cplusplus
}
#endif

#endif  //MAIN
