#ifndef __BOTNET_SPOPCFG_H__
#define __BOTNET_SPOPCFG_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "RomConst.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_pwr.h"

typedef void cBnSpop_tvCall(uint32 lui32P1, uint32 lui32P2, uint32 lui32P3, uint32 lui32P4);
typedef void cBnSpop_tvJump(void);

void   cBnSpop_vStartWdog();
void   cBnSpop_vResetWdog();
void   cBnSpop_vDeactivateIWgdDuringSleep();

uint16 cBnSpop_u16GetMemorySortByAdr(uint8* lpui8Adress);
uint16 cBnSpop_u16GetMemorySortByMemIdx(u8 lu8MemIdx);

void   cBnSpop_vFlash_Unlock();
void   cBnSpop_vFlash_Lock();
uint32 cBnSpop_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size);
void   cBnSpop_vFlash_ErasePage_Start(uint8* lpui8PageStartAdress);
void   cBnSpop_vFlash_ErasePage_Finish();
uint32 cBnSpop_vFlash_Program_Start(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);
void   cBnSpop_vFlash_Program_Finish();
bool   cBnSpop_isFlash_Busy();
bool   cBnSpop_isFlash_Error();

void   cBnSpop_vJumpToApp(uint8* lpui8Adr);
void   cBnSpop_vCall(uint8* lpui8Adr, uint32* luip32Param);

void   cBnSpop_vSystemReset();

#ifdef __cplusplus
}
#endif

#endif // __BOTNET_SPOPCFG_H__

