#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
//#include "cI2c.h"
#include "cNrf905.h"

//Data
#include "cRingBufT.h"
//#include "cBnLinkI2c.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortIap.h"
#include "cBotnetSwitch.h"

//System
#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32f30x.h"
//#include "_stm32f30x_exti.h"
//#include "_stm32f30x_adc.h"
//#include "_stm32f30x_dac.h"
#include "stm32f30x_gpio.h"
//#include "_stm32f30x_syscfg.h"
//#include "_stm32f30x_i2c.h"
//#include "_stm32f30x_iwdg.h"
//#include "_stm32f30x_opamp.h"
//#include "_stm32f30x_pwr.h"
#include "stm32f30x_rcc.h"
//#include "stm32f30x_rtc.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_usart.h"
//#include "stm32f30x_wwdg.h"
#include "stm32f30x_misc.h"


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
extern void TIM4_IRQHandler(void);

extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void TIM3_IRQHandler(void);


extern void DMA1_Channel2_IRQHandler(void); // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // SPI1 TX



extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);



#ifdef __cplusplus
}
#endif

#endif  //MAIN
