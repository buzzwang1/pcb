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

#define RomConst_Sort_IntRam     (RomConst_Sort_Ram)
#define RomConst_Sort_IntFlash   (RomConst_Sort_Flash)
#define RomConst_Sort_IntEep     (RomConst_Sort_Eep)

#define RomConst_Sort_ExtRam     (RomConst_Sort_Ram + 1)
#define RomConst_Sort_ExtFlash   (RomConst_Sort_Flash + 1)
#define RomConst_Sort_ExtEep     (RomConst_Sort_Eep + 1)


#define RomConst_Error        (0xFFFF)

#define ROMCONST_PARTITION_COUNT     RomConst_Partition_Count


#define tstRomConst_Platform_Partition tstMemPart_Partition


  typedef struct
  {
    const char8 szDevice_Name[16];                                                    // 0x0000
    const char8 szPlatform_Name[16];                                                  // 0x0010
    const char8 szPlatform_Info[32];                                                  // 0x0020
                                                                                      //
    // BotNet Info, Romconst - Info, und Partitionen-Info: 16Byte                     // 0x0040
    const u16   u16BnDeviceId;                                                        //
    const u16   u16BnNodeAdr;                                                         //
    const u32   u32IAP_RamSize;                                                       //
    const u8    u8RomConstPageCnt;                                                    //
    const u8    u8PartitionCnt;                                                       //
    const u8    u8TablesCnt;                                                          //
    const u8    u8Reserve1[1];                                                        //
    const u16   u8HwInfo;                                                             //
    const u16   u8RomConstVersion;                                                    //

    // BotNet Configuration values, Timing values, calibration values: 16 Byte   // 0x0050
    const u16   u16DateTimeRequestIntervall_s;
    const u16   u16DateTimeTimeoutRequest_s;
    const u16   u16DateTimeTimeoutValid_s;
    const u16   u16DateTimeRequestTriggerHour_h;
    const u16   u16RadioPingIntervall_s;
    const u16   u16RadioPingTimeoutSessionRequest_ms;
    const u8    u8Reserve2[4];

    // Botnet Link Configuration
    const u8    u8Reserve3[16];                                                       // 0x0060
    //
    // Reserved space for 8 Partitions                                                // 0x0070
    const tstMemPart_Partition stPartitions[8];                                       // 

    const u8    u8Reserve4[8];                                                        // 0x0170
    const u32   u32Checksum1;                                                         // 0x0178
    const u32   u32MagicPattern1;                                                     // 0x017B

                                                                                      // 

    // Reserved space for 8 LookUp-Tables                                             // 0x0180
    const u16   u16Table1[16];
    const u16   u16Table2[16];
    const u16   u16Table3[16];
    const u16   u16Table4[16];
    const u16   u16Table5[16];
    const u16   u16Table6[16];
    const u16   u16Table7[16];
    const u16   u16Table8[16];

    // Application Specific: 376 Byte                                                 // 0x0280
    const u8    u8AppSpecific[376];

    const u32   u32Checksum2;                                                         // 0x03f8
    const u32   u32MagicPattern2;                                                     // 0x03fB

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

