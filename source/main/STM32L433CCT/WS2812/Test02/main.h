
#ifndef __MAIN_H
#define __MAIN_H


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

#include <stdint.h>
#include <stddef.h>

#include "ws2812.h"
#include "stripEffects.h"


extern void MainSystemInit();

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


