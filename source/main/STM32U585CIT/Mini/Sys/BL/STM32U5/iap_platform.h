#ifndef __IAP_PLATFORM_H__
#define __IAP_PLATFORM_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "RomConst.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"


#define IAP_PARTITION_SIZE_BL  ROMCONST_PARTITION_SIZE_BL
#define IAP_PARTITION_SIZE_APP ROMCONST_PARTITION_SIZE_APP
#define IAP_PARTITION_SIZE_IMG ROMCONST_PARTITION_SIZE_IMG
#define IAP_PARTITION_SIZE_EEP ROMCONST_PARTITION_SIZE_EEP

#define IAP_PARTITION_START_ADRESS_FLASH ROMCONST_PARTITION_START_ADRESS_FLASH
#define IAP_PARTITION_START_ADRESS_BL    ROMCONST_PARTITION_START_ADRESS_BL
#define IAP_PARTITION_START_ADRESS_BLU   ROMCONST_PARTITION_START_ADRESS_BLU
#define IAP_PARTITION_START_ADRESS_APP   ROMCONST_PARTITION_START_ADRESS_APP
#define IAP_PARTITION_START_ADRESS_IMG   ROMCONST_PARTITION_START_ADRESS_IMG
#define IAP_PARTITION_START_ADRESS_EEP   ROMCONST_PARTITION_START_ADRESS_EEP

#define InAppProg_Platform_Ram_Size   RomConst_Platform_Ram_Size

#define InAppProg_Sort_Ram     (0)
#define InAppProg_Sort_Flash   (16)
#define InAppProg_Sort_Eep     (32)

#define InAppProg_Error        (0xFFFF)

#define IAP_PARTITION_COUNT     RomConst_Partition_Count

#define IAP_MM_RAM              RomConst_Sort_Ram
#define IAP_MM_FLASH            RomConst_Sort_Flash
#define IAP_MM_EEPROM           RomConst_Sort_Eep
#define IAP_MM_ERROR            RomConst_Error


typedef void cInAppProg_Platform_vCall(uint32 lui32P1, uint32 lui32P2, uint32 lui32P3, uint32 lui32P4);
typedef void cInAppProg_Platform_vJump(void);

void   InAppProg_Platform_vStartWdog();
void   InAppProg_Platform_vResetWdog();

//void   InAppProg_Platform_vLoadPartition(u8 lu8PartitionIdx, tstInAppProg_Platform_Partition* lpPartition);
uint16 InAppProg_Platform_u16GetMemorySort(uint8* lpui8Adress);
void   InAppProg_Platform_vFlash_Unlock();
void   InAppProg_Platform_vFlash_Lock();
uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size);
void   InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress);
uint32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);

void   InAppProg_Platform_vJumpToApp(uint8* lpui8Adr);
void   InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param);

void   InAppProg_Platform_vSystemReset();


#ifdef __cplusplus
}
#endif

#endif // __IAP_PLATFORM_H__

