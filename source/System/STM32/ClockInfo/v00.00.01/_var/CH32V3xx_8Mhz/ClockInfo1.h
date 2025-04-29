
#ifndef __ClockInfo1_H__
#define __ClockInfo1_H__

#include "ch32v30x.h"
#include "ch32v30x_rcc.h"

#define RCC_ClocksTypeDef u8
#define RCC_GetClocksFreq //LL_RCC_GetSystemClocksFreq


#define ClockInfo_Clock_Core SystemCoreClock//mstClocks.HCLK_Frequency
#define ClockInfo_Clock_Sys  8000000//mstClocks.SYSCLK_Frequency
#define ClockInfo_Clock_Port 8000000//mstClocks.PCLK1_Frequency


#endif // __ClockInfo1_H__
