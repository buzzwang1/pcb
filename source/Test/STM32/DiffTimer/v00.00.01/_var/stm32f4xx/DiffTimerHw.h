
#ifndef __DIFFTIMER_HW_H__
#define __DIFFTIMER_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "ClockInfo.h"

void InitTimer(void);
u32  u32GetTimer(void);

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_HW_H__
