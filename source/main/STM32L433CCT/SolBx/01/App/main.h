#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
#include "HX8347.h"
#include "cNrf905.h"
#include "ina219.h"
#include "dcf77.h"

//Data
#include "cStrT.h"
#include "cClock.h"
#include "cRingBufT.h"
//include "cBnLinkI2c.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortIap.h"
#include "cBnStreamSysPortText.h"
#include "cBotnet.h"

//System
#include "ClockInfo.h"
#include "CycleCaller.h"


//Graphics
#include "screen.h"
#include "rfont.h"
#include "paint.h"
#include "graphXY.h"

#include  "AddStuff.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"


//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

#include "MainBottom.h"
#include "MainMidPower.h"
#include "MainTop1_1_Power.h"
#include "MainTop1_2_ADC1.h"
#include "MainTop1_3_ADC2.h"
#include "MainTop1_4_ADC3.h"
#include "MainTop1_5_ADC4.h"
#include "MainTop1_6_DCF.h"
#include "MainTop1_7_Funk.h"
#include "MainTop1_Status.h"
#include "MainTop2_Konsole.h"
#include "MainTop3_Info.h"
#include "MainTop4_1_Dim.h"
#include "MainTop4_Einstellungen.h"

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

//nrf905
extern void EXTI15_10_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void DMA1_Channel2_IRQHandler(void); // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // SPI1 TX
// Display
extern void DMA1_Channel4_IRQHandler(void); // SPI2 RX
extern void DMA1_Channel5_IRQHandler(void); // SPI2 TX
// INA219
extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
// CyclicCaller
extern void TIM6_IRQHandler(void);

//Botnet I2C Timer
extern void TIM1_BRK_TIM15_IRQHandler(void); 

#ifdef __cplusplus
}
#endif

#endif  //MAIN
