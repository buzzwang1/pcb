#ifndef __MAIN_H__
#define __MAIN_H__

#include "LED.h"

//System
#include "CycleCaller.h"
#include "cBnLinkUsb.h"
#include "cBnMsgSysBtr.h"
#include "cBnMsgSysSpop.h"
#include "cBnMsgSysRRpt.h"
#include "cBnMsgSysMView.h"
#include "cBotnet.h"

#include "tusb.h"
#include "board_api.h"

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TypeDef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_icache.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_crs.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_cortex.h"
#include "stm32u5xx_ll_utils.h"


//extern void SysTick_Handler(void);
extern void MainSystemInit();
extern void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize);
extern void OTG_FS_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
