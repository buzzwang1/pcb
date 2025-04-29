
#ifndef __ClockInfo1_H__
#define __ClockInfo1_H__

#include "ch32v30x.h"
#include "ch32v30x_rcc.h"

#define RCC_ClocksTypeDef RCC_ClocksTypeDef
#define RCC_GetClocksFreq RCC_GetClocksFreq


#define ClockInfo_Clock_Core mstClocks.HCLK_Frequency
#define ClockInfo_Clock_Sys  mstClocks.SYSCLK_Frequency
#define ClockInfo_Clock_Port mstClocks.PCLK1_Frequency
#define ClockInfo_Clock_PCLK1 mstClocks.PCLK1_Frequency // (APB1 peripherals/TIM2,3,4,5,6,7)
#define ClockInfo_Clock_PCLK2 mstClocks.PCLK2_Frequency // (APB2 peripherals/TIM1,8,9,10)


#endif // __ClockInfo1_H__
