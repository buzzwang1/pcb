#ifndef __MAIN_H__
#define __MAIN_H__

//#include "LED.h"

//System
//#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "nrf.h"


extern void MainSystemInit();

extern void SAADC_IRQHandler(void);
extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void OTG_HS_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
