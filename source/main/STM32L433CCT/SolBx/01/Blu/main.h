#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"

//Data
#include "cRingBufT.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBnStreamSysPortIap.h"
#include "cBotnet.h"
//#include "cBotnetBlu.h"

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

extern void EXTI15_10_IRQHandler(void);
extern void TIM7_IRQHandler(void);

extern void TIM3_IRQHandler(void);

extern void DMA1_Channel2_IRQHandler(void); // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // SPI1 TX



#ifdef __cplusplus
}
#endif

#endif  //MAIN
