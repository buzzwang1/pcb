#ifndef __MAIN_H__
#define __MAIN_H__


// Driver
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "cNrf905.h"


//Data
#include "cRingBufT.h"
#include "cBnSpop.h"
#include "cBnLinkI2c.h"
#include "cBnLinkUsartMpHd.h"
#include "cBnLinkVLink.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortCom.h"
#include "cBotnet.h"
#include "cClock.h"

//System
#include "ClockInfo.h"
#include "CycleCaller.h"
#ifdef PCB_PROJECTCFG_Test
  #include "DiffTimer.h"
#endif

// App
#include "BoardCtrl.h"
#include "MP8859.h"


#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"


extern void SystemClock_Config();
extern void MAIN_vInitSystem(bool bFirstCall);

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

extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);

extern void RTC_WKUP_IRQHandler(void);
extern void EXTI0_IRQHandler(void);
extern void EXTI15_10_IRQHandler(void);
extern void TIM7_IRQHandler(void);



extern void DMA1_Channel2_IRQHandler(void); // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // SPI1 TX


extern void DMA2_Channel6_IRQHandler(void); // DMA USART1 TX
extern void DMA2_Channel7_IRQHandler(void); // DMA USART1 RX
extern void TIM1_UP_TIM16_IRQHandler(void);  //Botnet USART2 Timer

extern void TIM1_BRK_TIM15_IRQHandler(void); //Botnet I2C Timer
extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);
extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif  //MAIN
