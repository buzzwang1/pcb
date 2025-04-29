#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "CycleCaller.h"
#include "ClockInfo.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_conf.h"
    

extern void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

extern void MainSystemInit(void);



//extern void TIMER2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
