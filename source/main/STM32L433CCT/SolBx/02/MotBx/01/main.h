#ifndef __MAIN_H__
#define __MAIN_H__

// Driver
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "cSSD1306.h"
//#include "LSM303DLHC_MAG.h"
//#include "LSM303DLHC_ACC.h"
#include "apds9960.h"

//Data
#include "cRingBufT.h"
#include "cBnLinkI2c.h"
//#include "cBnLinkNrf905.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortIap.h"
#include "cBotnet.h"

//System
#include "ClockInfo.h"
#include "CycleCaller.h"

// App
#include "MotCtrl.h"


#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"


//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

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
extern void TIM3_IRQHandler(void);


extern void TIM1_BRK_TIM15_IRQHandler(void);
extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
