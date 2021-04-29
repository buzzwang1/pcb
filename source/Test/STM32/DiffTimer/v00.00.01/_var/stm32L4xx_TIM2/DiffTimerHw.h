
#ifndef __DIFFTIMER_HW_H__
#define __DIFFTIMER_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Typedef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"

#include "ClockInfo.h"

  extern void InitTimer(void);
  extern u32  u32GetTimer(void);

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_HW_H__
