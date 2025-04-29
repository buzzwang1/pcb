
#ifndef __SYS_PKG_STATE_MANAGER_H__
#define __SYS_PKG_STATE_MANAGER_H__


#include "TypeDef.h"
#include "tGPPin.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

#include "cStrT.h"


class cWorkMode
{
  public:

  typedef enum
  {
    nStManuel,
    nStAuto,
  }tenState;

  tenState mMode;
  u16      mCounter;

  cWorkMode();

  void vSet(cWorkMode::tenState lMode);

  bool IsAuto();

  void vTick1000ms();

  const char* ToString();
};






class cSysPkgSMan
{
  public:

  cWorkMode              mcOpMode;

  cSysPkgSMan();

  void vInit1(void);
  void vInit2(void);

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};


#endif // __SYS_PKG_STATE_MANAGER_H__
