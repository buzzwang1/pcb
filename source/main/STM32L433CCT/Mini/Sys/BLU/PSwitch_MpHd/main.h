#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
//#include "cI2c.h"

//Data
#include "cRingBufT.h"
#include "cUsart_MpHd.h"
#include "cBnLinkUsartMpHd.h"

//#include "cI2C.h"
//#include "cBnLinkI2c.h"

#include "cBotnet.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"


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

extern void DMA2_Channel7_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void TIM1_BRK_TIM15_IRQHandler(void);

//extern void I2C1_EV_IRQHandler(void);
//extern void I2C1_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
