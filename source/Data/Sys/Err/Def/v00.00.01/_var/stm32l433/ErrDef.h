#ifndef __SYS_ERR_DEF_H__
#define __SYS_ERR_DEF_H__


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
  u32 isHardFault  : 1;
  u32 isMemManage  : 1;
  u32 isBusFault   : 1;
  u32 isUsageFault : 1;
  u32 isSVC        : 1;
  u32 isDebugMon   : 1;
  u32 isPendSV     : 1;
}tstErrSources;

typedef union
{
  u32           u32Err;
  tstErrSources stErr;
}tunErrSources;


#ifdef __cplusplus
}
#endif

#endif //__SYS_ERR_DEF_H__
