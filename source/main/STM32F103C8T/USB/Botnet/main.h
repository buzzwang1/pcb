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
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"


//extern void SysTick_Handler(void);
extern void MainSystemInit();
extern void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize);
extern void USB_LP_CAN1_RX0_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //MAIN
