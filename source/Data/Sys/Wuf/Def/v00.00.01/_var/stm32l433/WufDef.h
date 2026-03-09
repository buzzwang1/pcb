#ifndef __SYS_WUF_DEF_H__
#define __SYS_WUF_DEF_H__


#include "typedef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  u32 isWuTimerRtc : 1; // 01
  u32 isWuStandBy  : 1; // 02
  u32 isWuPin1     : 1; // 04
  u32 isWuPinRst   : 1; // 08
  u32 isWuSftRst   : 1; // 10
  u32 isWuIWdg     : 1; // 20
}tstWakeupSources;

typedef union
{
  u32              u32WakeupSources;
  tstWakeupSources stWakeupSources;
}tunWakeupSources;

extern void WufDef_vCheckWuReason(tunWakeupSources *lunWuSources);

#ifdef __cplusplus
}
#endif

#endif //__SYS_WUF_DEF_H__
