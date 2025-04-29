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
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

#define PE3_Pin             GPIO_PIN_3
#define PE3_GPIO_Port       GPIOE
#define KEY_Pin             GPIO_PIN_13
#define KEY_GPIO_Port       GPIOC
#define LCD_CS_Pin          GPIO_PIN_11
#define LCD_CS_GPIO_Port    GPIOE
#define LCD_WR_RS_Pin       GPIO_PIN_13
#define LCD_WR_RS_GPIO_Port GPIOE
#define F_CS_Pin            GPIO_PIN_6
#define F_CS_GPIO_Port      GPIOD


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

#ifdef __cplusplus
}
#endif

#endif  //MAIN
