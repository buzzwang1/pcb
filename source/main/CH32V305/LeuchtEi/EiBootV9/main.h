#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"
#include "cI2c.h"
#include "Eep.h"

//System
#include "CycleCaller.h"
#include "ClockInfo.h"
#include "cStringtools.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_conf.h"

#include "ch32v30x_usbhs_device.h"
#include "SPI_FLASH.h"
#include "Internal_FLASH.h"
#include "SW_UDISK.h"

#include "LedCtrl.h"

extern void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void DMA1_Channel2_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));

extern void MainSystemInit(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
