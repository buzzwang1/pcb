#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
//#include "CycleCaller.h"
#include "cStringTools.h"

#ifdef __cplusplus
extern "C" {
#endif


  extern void PreMain(void);

  /* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "samd10.h"
#include "drivers.h"

//#include "sercom.h"
//#include "sercom_interrupt.h"

//#include "usart.h"
//#include "usart_interrupt.h"

  extern void SysTick_Handler(void);
  extern void TC2_Handler(void);

  extern void SERCOM0_Handler(void);
  extern void SERCOM1_Handler(void);
  extern void SERCOM2_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
