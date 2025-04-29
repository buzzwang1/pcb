#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_utils.h"
#include "stm32u5xx_ll_gpio.h"
#include "stm32u5xx_ll_dma.h"

#define DMA_CLK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ALL)

/* Definition for DMA NVIC */
#define DMA_IRQn         GPDMA1_Channel13_IRQn
#define DMA_IRQHandler   GPDMA1_Channel13_IRQHandler

/* Definition of buffer sizes */
#define BUFFER1_SIZE     (8U)
#define BUFFER2_SIZE     (16U)
#define BUFFER3_SIZE     (24U)

#define DEFAULT_REGISTER_UPDATE LL_DMA_UPDATE_CTR1 | LL_DMA_UPDATE_CTR2 | \
                                LL_DMA_UPDATE_CBR1 | LL_DMA_UPDATE_CSAR | \
                                LL_DMA_UPDATE_CDAR | LL_DMA_UPDATE_CLLR


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

extern void DMA_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
