#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
//#include "cI2c.h"

//Data
#include "cRingBufT.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBotnet.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_icache.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_crs.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_exti.h"
#include "stm32u5xx_ll_cortex.h"
#include "stm32u5xx_ll_utils.h"


extern void SystemClock_Config();
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


//extern void TIM3_IRQHandler(void);

extern void EXTI5_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void TIM7_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
