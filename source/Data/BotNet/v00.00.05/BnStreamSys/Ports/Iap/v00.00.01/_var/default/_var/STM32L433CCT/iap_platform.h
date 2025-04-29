#ifndef __IAP_PLATFORM_H__
#define __IAP_PLATFORM_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "MemPart.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"

#define IAP_vLoadPartition(lu8PartitionIdx, lpPartition) lpPartition = &stInAppProg_Platform_Partitions[lu8PartitionIdx]

#define IAP_PARTITION_SIZE_BL  ( 32 * 1024)
#define IAP_PARTITION_SIZE_APP (144 * 1024)
#define IAP_PARTITION_SIZE_IMG ( 76 * 1024)
#define IAP_PARTITION_SIZE_EEP (  4 * 1024)

#define IAP_PARTITION_START_ADRESS_FLASH (u8*)(0x08000000)
#define IAP_PARTITION_START_ADRESS_BL    (u8*)(IAP_PARTITION_START_ADRESS_FLASH)
#define IAP_PARTITION_START_ADRESS_BLU   (u8*)(IAP_PARTITION_START_ADRESS_FLASH + (8 * 1024))
#define IAP_PARTITION_START_ADRESS_APP   (u8*)(IAP_PARTITION_START_ADRESS_FLASH + IAP_PARTITION_START_ADRESS_BL)
#define IAP_PARTITION_START_ADRESS_IMG   (u8*)(IAP_PARTITION_START_ADRESS_FLASH + IAP_PARTITION_START_ADRESS_BL + IAP_PARTITION_START_ADRESS_APP)
#define IAP_PARTITION_START_ADRESS_EEP   (u8*)(IAP_PARTITION_START_ADRESS_FLASH + IAP_PARTITION_START_ADRESS_BL + IAP_PARTITION_START_ADRESS_APP + IAP_PARTITION_START_ADRESS_IMG)

#define InAppProg_Platform_Ram_Size   (4*1024)

#define InAppProg_Sort_Ram     (MemPart_Sort_Ram)
#define InAppProg_Sort_Flash   (MemPart_Sort_Flash)
#define InAppProg_Sort_Eep     (MemPart_Sort_Eep)

#define InAppProg_Error        (0xFFFF)

#define IAP_MM_RAM              InAppProg_Sort_Ram
#define IAP_MM_FLASH            InAppProg_Sort_Flash
#define IAP_MM_EEPROM           InAppProg_Sort_Eep
#define IAP_MM_ERROR            InAppProg_Error

const char8  InAppProg_Interfacename[] = "Mem 01.00.02";
const char8  InAppProg_Platform_Name[] = "STM32L433CCT6";
const char8  InAppProg_Platform_Info[] = "Cortex-M4,Rom 256KB,Ram 64KB";

#define IAP_PARTITION_COUNT     6

extern const tstMemPart_Partition stInAppProg_Platform_Partitions[IAP_PARTITION_COUNT];


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


#ifdef __cplusplus
}
#endif

#endif // __IAP_PLATFORM_H__

