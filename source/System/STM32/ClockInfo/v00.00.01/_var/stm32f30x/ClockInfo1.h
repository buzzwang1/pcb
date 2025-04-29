
#ifndef __ClockInfo1_H__
#define __ClockInfo1_H__

#include "stm32f30x_rcc.h"


#define ClockInfo_Clock_Core mstClocks.HCLK_Frequency
#define ClockInfo_Clock_Sys  mstClocks.SYSCLK_Frequency
#define ClockInfo_Clock_Port mstClocks.PCLK1_Frequency

#endif // __ClockInfo1_H__
