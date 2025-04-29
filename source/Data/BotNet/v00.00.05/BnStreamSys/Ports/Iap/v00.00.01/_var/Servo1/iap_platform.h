#ifndef __IAP_PLATFORM_H__
#define __IAP_PLATFORM_H__

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

#define IAP_vLoadPartition(lu8PartitionIdx, lpPartition) IAP_RomConst_Platform_vLoadPartition(lu8PartitionIdx, lpPartition)

#define IAP_PARTITION_SIZE_BL  ROMCONST_PARTITION_SIZE_BL
#define IAP_PARTITION_SIZE_APP ROMCONST_PARTITION_SIZE_APP
#define IAP_PARTITION_SIZE_IMG ROMCONST_PARTITION_SIZE_IMG
#define IAP_PARTITION_SIZE_EEP ROMCONST_PARTITION_SIZE_EEP
#define IAP_PARTITION_SIZE_EEP ROMCONST_PARTITION_SIZE_EEP

#define IAP_PARTITION_START_ADRESS_FLASH ROMCONST_PARTITION_START_ADRESS_FLASH
#define IAP_PARTITION_START_ADRESS_BL    ROMCONST_PARTITION_START_ADRESS_BL
#define IAP_PARTITION_START_ADRESS_BLU   ROMCONST_PARTITION_START_ADRESS_BLU
#define IAP_PARTITION_START_ADRESS_APP   ROMCONST_PARTITION_START_ADRESS_APP
#define IAP_PARTITION_START_ADRESS_IMG   ROMCONST_PARTITION_START_ADRESS_IMG
#define IAP_PARTITION_START_ADRESS_EEP   ROMCONST_PARTITION_START_ADRESS_EEP

#define InAppProg_Platform_Ram_Size   RomConst_Platform_Ram_Size

#define InAppProg_Sort_Ram     (MemPart_Sort_Ram)
#define InAppProg_Sort_Flash   (MemPart_Sort_Flash)
#define InAppProg_Sort_Eep     (MemPart_Sort_Eep)

#define InAppProg_Error        (0xFFFF)

#define IAP_PARTITION_COUNT     RomConst_Partition_Count

#define IAP_MM_RAM              RomConst_Sort_Ram
#define IAP_MM_FLASH            RomConst_Sort_Flash
#define IAP_MM_EEPROM           RomConst_Sort_Eep
#define IAP_MM_ERROR            RomConst_Error

const char8  InAppProg_Interfacename[] = "Mem 01.00.02";
#define      InAppProg_Platform_Name RomConst_stDevice_Info->szPlatform_Name
#define      InAppProg_Platform_Info RomConst_stDevice_Info->szPlatform_Info


typedef void cInAppProg_Platform_vCall(uint32 lui32P1, uint32 lui32P2, uint32 lui32P3, uint32 lui32P4);
typedef void cInAppProg_Platform_vJump(void);

void   InAppProg_Platform_vStartWdog();
void   InAppProg_Platform_vResetWdog();


uint16 InAppProg_Platform_u16GetMemorySort(uint8* lpui8Adress);
void   InAppProg_Platform_vFlash_Unlock();
void   InAppProg_Platform_vFlash_Lock();
uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size);
void   InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress);
uint32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);

void   InAppProg_Platform_vJumpToApp(uint8* lpui8Adr);
void   InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param);

void   InAppProg_Platform_vSystemReset();


void InAppProg_Platform_vWriteBackupData(uint16 lu16Idx, u32 lu32Value);
u32  InAppProg_Platform_u32ReadBackupData(uint16 lu16Idx);


#ifdef __cplusplus
}
#endif

#endif // __IAP_PLATFORM_H__

