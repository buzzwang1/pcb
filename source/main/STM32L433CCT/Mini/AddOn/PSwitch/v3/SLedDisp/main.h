#ifndef __MAIN_H__
#define __MAIN_H__


#include "System.h"

//
//System
#include "ClockInfo.h"
#include "CycleCaller.h"
#include "LedCtrl.h"
#include "Eep.h"
#include "cSSD1306.h"
#include "apds9960.h"

//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

extern void SystemClock_Config();
extern void MAIN_vInitSystem(void);


extern void MainSystemInit(void);

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

extern void DMA1_Channel1_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
