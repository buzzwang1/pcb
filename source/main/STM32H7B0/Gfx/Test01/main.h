#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"
#include "st7735.h"

//System
#include "CycleCaller.h"
//#include "DiffTimer.h"

//Graphics
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "rfont.h"
#include "paint.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"


extern void MainSystemInit();


extern void TIM2_IRQHandler(void);
extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

extern void SPI4_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
