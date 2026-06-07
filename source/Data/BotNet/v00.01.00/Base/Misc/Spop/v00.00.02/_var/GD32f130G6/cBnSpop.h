#ifndef __BOTNET_SPOPCFG_H__
#define __BOTNET_SPOPCFG_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "RomConst.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_pmu.h"
#include "gd32f1x0_fmc.h"
#include "gd32f1x0_fwdgt.h"


typedef void cBnSpop_tvCall(uint32 lui32P1, uint32 lui32P2, uint32 lui32P3, uint32 lui32P4);
typedef void cBnSpop_tvJump(void);

void   cBnSpop_vStartWdog();
void   cBnSpop_vResetWdog();


uint16 cBnSpop_u16GetMemorySortByAdr(uint8* lpui8Adress);
uint16 cBnSpop_u16GetMemorySortByMemIdx(u8 lu8MemIdx);

void   cBnSpop_vFlash_Unlock();
void   cBnSpop_vFlash_Lock();
uint32 cBnSpop_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size);

void   cBnSpop_vFlash_ErasePage(uint8* lpui8PageStartAdress);
void   cBnSpop_vFlash_ErasePage_Start(uint8* lpui8PageStartAdress);
void   cBnSpop_vFlash_ErasePage_Finish();

uint32 cBnSpop_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);
uint32 cBnSpop_vFlash_Program_Start(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);
void   cBnSpop_vFlash_Program_Finish();
bool   cBnSpop_isFlash_Busy();
bool   cBnSpop_isFlash_Error();

void   cBnSpop_vJumpToApp(uint8* lpui8Adr);
void   cBnSpop_vCall(uint8* lpui8Adr, uint32* luip32Param);

void   cBnSpop_vSystemReset();

void cBnSpop_vWriteBackupData(uint16 lu16Idx, u32 lu32Value);
u32  cBnSpop_u32ReadBackupData(uint16 lu16Idx);


#ifdef __cplusplus
}
#endif

#endif // __BOTNET_SPOPCFG_H__

