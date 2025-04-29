#ifndef __ROM_CONSTS_H__
#define __ROM_CONSTS_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "MemPart.h"

#define RomConst_Platform_vLoadPartition(lu8PartitionIdx, lpPartition) lpPartition = (const tstMemPart_Partition*)&RomConst_stDevice_Info->stPartitions[lu8PartitionIdx]

#define ROMCONST_PARTITION_SIZE_BL  ( 32 * 1024)
#define ROMCONST_PARTITION_SIZE_APP (144 * 1024)
#define ROMCONST_PARTITION_SIZE_IMG ( 76 * 1024)
#define ROMCONST_PARTITION_SIZE_EEP (  4 * 1024)

#define ROMCONST_PARTITION_START_ADRESS_FLASH (u8*)(0x08000000)
#define ROMCONST_PARTITION_START_ADRESS_BL    (u8*)(ROMCONST_PARTITION_START_ADRESS_FLASH)
#define ROMCONST_PARTITION_START_ADRESS_BLU   (u8*)(ROMCONST_PARTITION_START_ADRESS_FLASH + (8 * 1024))
#define ROMCONST_PARTITION_START_ADRESS_APP   (u8*)(ROMCONST_PARTITION_START_ADRESS_FLASH + ROMCONST_PARTITION_SIZE_BL)
#define ROMCONST_PARTITION_START_ADRESS_IMG   (u8*)(ROMCONST_PARTITION_START_ADRESS_FLASH + ROMCONST_PARTITION_SIZE_BL + ROMCONST_PARTITION_SIZE_APP)
#define ROMCONST_PARTITION_START_ADRESS_EEP   (u8*)(ROMCONST_PARTITION_START_ADRESS_FLASH + ROMCONST_PARTITION_SIZE_BL + ROMCONST_PARTITION_SIZE_APP + ROMCONST_PARTITION_SIZE_IMG)

#define RomConst_Platform_Ram_Size   (4*1024) //(*(u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0x004C))
#define RomConst_Partition_Count     (*(u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0x0048))

#define RomConst_Sort_Ram     (MemPart_Sort_Ram)
#define RomConst_Sort_Flash   (MemPart_Sort_Flash)
#define RomConst_Sort_Eep     (MemPart_Sort_Eep)

#define RomConst_Error        (0xFFFF)

#define ROMCONST_PARTITION_COUNT     RomConst_Partition_Count


#define tstRomConst_Platform_Partition tstMemPart_Partition


typedef struct
{
  const char8 szDevice_Name[16];                     // 0x0000
  const char8 szPlatform_Name[16];                   // 0x0010
  const char8 szPlatform_Info[32];                   // 0x0020
                                                     //
  // BotNet-Info und Partitionen - Info              // 0x0040
  const u16   u16BnDeviceId;                         //
  const u16   u16BnNodeAdr;                          //
  const u8    u8Reserve1[4];                         //
  const u8    u8PartitionCnt;                        //
  const u8    u8Reserve2[3];                         //
  const u32   u32IAP_RamSize;                        //
                                                     //
  // 8 Partitionen                                   //
  const tstMemPart_Partition stPartitions[8];        // 0x0050   8*32 = 0x100
                                                     // 
                                                     // 0x0150
                                                     // 
  // End of Device Info Block                        //
  const u8    u8Reserve7[4096-8-0x0150];             // 0x03f0
  const u32   u32Checksum;                           // 
  const u32   u32MagicPattern;                       //

}tstRomConst_Platform_Device_Info;

extern const tstRomConst_Platform_Device_Info* RomConst_stDevice_Info;

u8 RomConst_IsHeaderPlausible();
u8 RomConst_IsMagicPatternOk();
u8 RomConst_IsValid();

#include "RomConstDef.h"

#ifdef __cplusplus
}
#endif

#endif // __ROM_CONSTS_H__

