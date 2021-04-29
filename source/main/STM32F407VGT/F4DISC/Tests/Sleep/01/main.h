#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "TypeDef.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx.h"

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void EXTI0_IRQHandler(void);
extern void RTC_WKUP_IRQHandler(void);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  extern int __io_putchar(int ch);
#else
  extern int fputc(int ch, FILE *f);
#endif /* __GNUC__ */


#ifdef __cplusplus
}
#endif

#endif  //MAIN
