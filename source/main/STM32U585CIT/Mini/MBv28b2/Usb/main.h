#ifndef MAIN_H
#define MAIN_H

#include "TypeDef.h"
#include "ZD25WQ32.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_rcc.h"
#include "stm32u5xx_ll_dac.h"
#include "stm32u5xx_ll_icache.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_crs.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_exti.h"
#include "stm32u5xx_ll_cortex.h"
#include "stm32u5xx_ll_utils.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_spi.h"

void led_indicator_pulse(void);
extern void MainSystemInit(void);

#ifdef __cplusplus
}
#endif

#endif
