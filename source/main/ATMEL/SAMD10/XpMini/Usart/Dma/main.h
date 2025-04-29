#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
//#include "CycleCaller.h"

#ifdef __cplusplus
  extern "C" {
#endif


extern void PreMain(void);

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "samd10.h"
#include "drivers.h"


extern void SysTick_Handler(void);
extern void TC2_Handler(void);
extern void DMAC_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
