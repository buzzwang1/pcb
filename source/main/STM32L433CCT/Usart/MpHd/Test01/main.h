#ifndef __MAIN_H__
#define __MAIN_H__

//Driver
#include "LED.h"
#include "cUsart_MpHd.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_dma.h"


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



extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);

extern void TIM1_BRK_TIM15_IRQHandler(void);


extern void DMA1_Channel2_IRQHandler(void); // DMA USART3 TX // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // DMA USART3 RX // SPI1 RX
extern void DMA1_Channel4_IRQHandler(void); // DMA USART1 TX
extern void DMA1_Channel5_IRQHandler(void); // DMA USART1 RX
extern void DMA1_Channel6_IRQHandler(void); // DMA USART2 RX
extern void DMA1_Channel7_IRQHandler(void); // DMA USART2 TX
#ifdef __cplusplus
}
#endif

#endif  //MAIN