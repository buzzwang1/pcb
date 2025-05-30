#ifndef __ROM_CONSTS_DEF_H__
#define __ROM_CONSTS_DEF_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"

#define rszGetRomConstDeviceName()           ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0000))           // Defaultvalue LedBoardv1
#define rszGetRomConstPlatformName()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0010))           // Defaultvalue STM32L433CCT6
#define rszGetRomConstPlatformInfo()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0020))           // Defaultvalue Cortex-M4,Rom 256KB,Ram 64KB
#define u16GetRomConstBnDeviceID()           (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0040)))       // Defaultvalue 21     
#define u16GetRomConstBnAdr()                (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0042)))       // Defaultvalue 7696   
#define u8GetRomConstPartitionCnt()          (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0048)))        // Defaultvalue 6      
#define u32GetRomConstIapRamSize()           (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004C)))       // Defaultvalue 4096   
#define rszGetRomConstPartition0Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0050))           // Defaultvalue Flash Bl
#define u16GetRomConstPartition0Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0060)))       // Defaultvalue 16     
#define u16GetRomConstPartition0Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0062)))       // Defaultvalue 0      
#define u32GetRomConstPartition0BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0064)))       // Defaultvalue 134217728
#define u32GetRomConstPartition0Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0068)))       // Defaultvalue 32768  
#define u32GetRomConstPartition0BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006C)))       // Defaultvalue 2048   
#define rszGetRomConstPartition1Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0070))           // Defaultvalue Flash APP
#define u16GetRomConstPartition1Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0080)))       // Defaultvalue 16     
#define u16GetRomConstPartition1Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0082)))       // Defaultvalue 0      
#define u32GetRomConstPartition1BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0084)))       // Defaultvalue 134250496
#define u32GetRomConstPartition1Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0088)))       // Defaultvalue 147456 
#define u32GetRomConstPartition1BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X008C)))       // Defaultvalue 2048   
#define rszGetRomConstPartition2Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0090))           // Defaultvalue Flash IAP
#define u16GetRomConstPartition2Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A0)))       // Defaultvalue 16     
#define u16GetRomConstPartition2Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A2)))       // Defaultvalue 0      
#define u32GetRomConstPartition2BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A4)))       // Defaultvalue 134397952
#define u32GetRomConstPartition2Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A8)))       // Defaultvalue 77824  
#define u32GetRomConstPartition2BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00AC)))       // Defaultvalue 2048   
#define rszGetRomConstPartition3Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00B0))           // Defaultvalue Flash EEP
#define u16GetRomConstPartition3Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C0)))       // Defaultvalue 16     
#define u16GetRomConstPartition3Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C2)))       // Defaultvalue 0      
#define u32GetRomConstPartition3BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C4)))       // Defaultvalue 134475776
#define u32GetRomConstPartition3Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C8)))       // Defaultvalue 4096   
#define u32GetRomConstPartition3BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00CC)))       // Defaultvalue 2048   
#define rszGetRomConstPartition4Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00D0))           // Defaultvalue Ram    
#define u16GetRomConstPartition4Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E0)))       // Defaultvalue 0      
#define u16GetRomConstPartition4Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E2)))       // Defaultvalue 0      
#define u32GetRomConstPartition4BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E4)))       // Defaultvalue 536870912
#define u32GetRomConstPartition4Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E8)))       // Defaultvalue 49152  
#define u32GetRomConstPartition4BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00EC)))       // Defaultvalue 1      
#define rszGetRomConstPartition5Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00F0))           // Defaultvalue CCM    
#define u16GetRomConstPartition5Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0100)))       // Defaultvalue 0      
#define u16GetRomConstPartition5Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0102)))       // Defaultvalue 0      
#define u32GetRomConstPartition5BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0104)))       // Defaultvalue 268435456
#define u32GetRomConstPartition5Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0108)))       // Defaultvalue 16384  
#define u32GetRomConstPartition5BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X010C)))       // Defaultvalue 1      
#define u16GetRomConstPowerInVout()          ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0150))          // Defaultvalue [4255, 4944, 5949, 6951, 7965, 8970, 9959, 10982, 11984, 12955, 13996, 14996, 15967, 17123, 18147, 19169, 19889]
#define u16GetRomConstPowerInDac()           ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0172))          // Defaultvalue [4095, 3840, 3584, 3328, 3072, 2816, 2560, 2304, 2048, 1792, 1536, 1280, 1024, 768, 512, 256, 0]
#define u16GetRomConstDigitAdc0()            ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0194))          // Defaultvalue [3330, 3329, 3327, 3325, 3332, 3328, 3321, 3328, 3323, 3321, 3323, 3316, 3275, 3191, 3116, 3044, 2933]
#define u16GetRomConstDigitAdc1()            ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01B6))          // Defaultvalue [887, 1034, 1245, 1456, 1669, 1879, 2088, 2303, 2512, 2721, 2934, 3144, 3361, 3593, 3808, 4025, 4094]
#define u16GetRomConstDigitAdc2()            ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01D8))          // Defaultvalue [902, 1051, 1266, 1480, 1697, 1910, 2125, 2342, 2555, 2768, 2984, 3198, 3417, 3653, 3873, 4091, 4094]
#define u16GetRomConstDigitAdc3()            ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01FA))          // Defaultvalue [903, 1052, 1267, 1481, 1698, 1912, 2126, 2344, 2557, 2770, 2986, 3200, 3420, 3657, 3876, 4092, 4094]
#define u16GetRomConstDigitAdc4()            ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X021C))          // Defaultvalue [884, 1030, 1241, 1450, 1663, 1872, 2081, 2295, 2504, 2712, 2924, 3133, 3349, 3580, 3795, 4010, 4094]
#define u8GetRomConstBoardType()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X023E)))        // Defaultvalue 0:     Type des Boards
#define rszGetRomConstMagicPattern()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0FFC))           // Defaultvalue FLKA   
#define u32GetRomConstChecksum()             (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0FF8)))       // Defaultvalue 481668 



#ifdef __cplusplus
}
#endif

#endif // __ROM_CONSTS_DEF_H__

