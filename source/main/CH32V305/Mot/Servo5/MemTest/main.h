#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

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


#ifdef __cplusplus
}
#endif

#endif  //MAIN
