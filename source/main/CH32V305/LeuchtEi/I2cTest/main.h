#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"
#include "cI2c.h"
#include "Eep.h"
#include "Tmp102.h"
#include "cSSD1306.h"
#include "htu21d.h"

//System
#include "CycleCaller.h"
#include "ClockInfo.h"

//String
#include "cStringTools.h"
#include "cCli.h"

//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_conf.h"

#include "LedCtrl.h"
#include "CdcCli.h"

#include "stdlib.h" // for Rand
  
//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"
    

extern void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void DMA1_Channel2_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));

extern void I2C2_EV_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
extern void I2C2_ER_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
extern void DMA1_Channel4_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
extern void DMA1_Channel5_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));


extern void MainSystemInit(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
