/*
 * bl1hdr.h
 *
 *  Created on: 31.05.2016
 *      Author: glunmart
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "Typedef.h"
#include "string.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "iap_platform.h"
#include "WufDef.h"
#include "BuRamDef.h"
#include "uzlib.h"

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

#endif /* _MAIN_H_ */
