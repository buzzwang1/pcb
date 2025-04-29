#ifndef __MAIN_H__
#define __MAIN_H__

//#include "LED.h"
#include "RomConst.h"

//System
#include "ClockInfo.h"

//Data
//#include "cUsart_MpHd.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "ch641.h"
#include "ch641_conf.h"


#define FSTIRQ "WCH-Interrupt-fast"

extern void NMI_Handler(void)       __attribute__((interrupt(FSTIRQ)));
extern void HardFault_Handler(void) __attribute__((interrupt(FSTIRQ)));

extern void MainSystemInit(void);

extern void SysTick_Handler(void)          __attribute__((interrupt(FSTIRQ)));
extern void USART1_IRQHandler(void)        __attribute__((interrupt(FSTIRQ)));
extern void TIM2_IRQHandler(void)          __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt(FSTIRQ)));
extern void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt(FSTIRQ)));


#ifdef __cplusplus
}
#endif

#endif  //MAIN
