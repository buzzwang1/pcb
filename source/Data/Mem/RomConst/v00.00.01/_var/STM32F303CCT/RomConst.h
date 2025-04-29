#ifndef __ROM_CONSTS_H__
#define __ROM_CONSTS_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "MemPart.h"

#define RomConst_Platform_vLoadPartition(lu8PartitionIdx, lpPartition) lpPartition = (tstMemPart_Partition*)&RomConst_stDevice_Info->stPartitions[lu8PartitionIdx]

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

#define RomConst_Sort_Ram     (0)
#define RomConst_Sort_Flash   (16)
#define RomConst_Sort_Eep     (32)

#define RomConst_Error        (0xFFFF)

#define ROMCONST_PARTITION_COUNT     RomConst_Partition_Count


typedef struct
{
  char8  szName[16];
  uint16 ui16Sort;
  uint16 ui16Type;
  uint32 ui32Start;
  uint32 ui32Size;
  uint32 ui32Block_Size;
}tstRomConst_Platform_Partition;


typedef struct
{
  const char8 szDevice_Name[16];
  const char8 szPlatform_Name[16];
  const char8 szPlatform_Info[32];

  // BotNet-Info und Partitionen - Info
  u16   u16BnDeviceId;
  u16   u16BnNodeAdr;
  u8    u8Reserve1[4];
  u8    u8PartitionCnt;
  u8    u8Reserve2[3];
  u32   u32IAP_RamSize;

  // 16 Partitionen
  tstMemPart_Partition stPartitions[16];

  // End of Device Info Block
  u8    u8Reserve3[8];
  u32   u32Checksum;
  u32   u32MagicPattern;

}tstRomConst_Platform_Device_Info;

extern const tstRomConst_Platform_Device_Info* RomConst_stDevice_Info;

u8 RomConst_IsValid();

#ifdef __cplusplus
}
#endif

#endif // __ROM_CONSTS_H__

