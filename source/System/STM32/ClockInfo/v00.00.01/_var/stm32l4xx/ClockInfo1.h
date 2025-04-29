
#ifndef __ClockInfo1_H__
#define __ClockInfo1_H__

#include "stm32l4xx_ll_rcc.h"

#define RCC_ClocksTypeDef LL_RCC_ClocksTypeDef
#define RCC_GetClocksFreq LL_RCC_GetSystemClocksFreq


#define ClockInfo_Clock_Core mstClocks.HCLK_Frequency
#define ClockInfo_Clock_Sys  mstClocks.SYSCLK_Frequency
#define ClockInfo_Clock_Port mstClocks.PCLK1_Frequency


#endif // __ClockInfo1_H__
