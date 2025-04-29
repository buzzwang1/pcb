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

#include "cRingBufT.h"

#include "cCnc.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_tim.h"
#include "stm32h7xx_ll_exti.h"
#include "stm32h7xx_hal_dcmi.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_i2c.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

void Error_Handler(void);

void Main_Usb_Receive(u8* Buf, u16 Len);

extern PCD_HandleTypeDef hpcd_USB_OTG_HS;


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


extern void TIM8_BRK_TIM12_IRQHandler(void);
extern void TIM8_UP_TIM13_IRQHandler(void);
extern void TIM8_TRG_COM_TIM14_IRQHandler(void);

extern void TIM15_IRQHandler(void);
extern void TIM16_IRQHandler(void);
extern void TIM17_IRQHandler(void);



extern void SPI4_IRQHandler(void);
extern void DMA1_Stream0_IRQHandler(void);
extern void DCMI_PSSI_IRQHandler(void);

extern void OTG_HS_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
