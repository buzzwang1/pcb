/*
 * bl1hdr.h
 *
 *  Created on: 31.05.2016
 *      Author: glunmart
 */

#ifndef BL1HDR_H_
#define BL1HDR_H_

#include "TypeDef.h"

  #define  IAP_MM_RAM   1
  #define  IAP_MM_FLASH 2

  #define InAppProg_Platform_u16GetMM(x) IAP_MM_FLASH

  #define InAppProg_Platform_vFlash_Unlock()
  #define InAppProg_Platform_vFlash_EreasePage(lpui8DestMem)           memset(lpui8DestMem, 0xFF, BUFLEN)
  #define InAppProg_Platform_vFlash_Program(lpui8DestMem, buf, BUFLEN) memcpy(lpui8DestMem, buf, BUFLEN)
  #define InAppProg_Platform_vFlash_Lock()

#endif /* BL1HDR_H_ */
