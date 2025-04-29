#ifndef __ROM_CONSTS_DEF_H__
#define __ROM_CONSTS_DEF_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"

#define rszGetRomConstDeviceName()           ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0000))           // Defaultvalue MBv7b2 
#define rszGetRomConstPlatformName()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0010))           // Defaultvalue STM32L433CCT6
#define rszGetRomConstPlatformInfo()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0020))           // Defaultvalue Cortex-M4,Rom 256KB,Ram 64KB
#define u16GetRomConstBnDeviceID()           (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0040)))       // Defaultvalue 21     
#define u16GetRomConstBnAdr()                (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0042)))       // Defaultvalue 4096   
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
#define u8GetRomConstChrgCellCount()         (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X023F)))        // Defaultvalue 2:     Anzahl Zellen der Lipo
#define u8GetRomConstChrgModeCharged()       (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0240)))        // Defaultvalue 1:     Verhalten, wenn Laden beendet, 0: Charger aus, 1: Erhaltungsspannung
#define u8GetRomConstChrgModeStart()         (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0241)))        // Defaultvalue 1:     Verhalten, wenn Laden gestartet wird, 0: Sobald Spannung kleiner ChrgVoltStart, 1: Sobald Eingangsspannung vorhanden
#define u16GetRomConstChrgVoltLipoOff()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0242)))       // Defaultvalue 3000:  VoltLipoOff [mV]        Tiefentladeschutz. Bei der Spannung schaltet sich der uC ab. Versorgung zur Lipo wird getrennt
#define u16GetRomConstChrgVoltSysOff()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0244)))       // Defaultvalue 3150:  VoltSysOff [mV]         Unterspannungsschutz. Bei der Spannung wird der Ausgang ausgeschaltet und der uC geht in  den Stromsparmodus
#define u16GetRomConstChrgMaxVoltCell()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0246)))       // Defaultvalue 4150:  MaxVoltCell [mV]        Ladeschlussspannung. Bei der Spannung ist die Lipo geladen
#define u16GetRomConstChrgVoltBalance()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0248)))       // Defaultvalue 4200:  VoltBalance [mV]        Balancespannung. Bei der Spannung wird beim Laden der Balancer der Zelle aktiviert.  (0 == Balancer aus)
#define u16GetRomConstChrgCurrentSave()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X024A)))       // Defaultvalue 25:    CurrentSave [mA]        Sichere Ladestrom. Ladestrom bei tiefentladener Zelle oder wenn Balancer einer Zelle an ist.
#define u16GetRomConstChrgCurrentCharged()   (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X024C)))       // Defaultvalue 25:    CurrentCharged [mA]     Laden wird beendet, sobald diese Strom unterschritten ist
#define u16GetRomConstChrgVoltStart()        (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X024E)))       // Defaultvalue 3200:  VoltStart [mV]          Spannung, bei der das Laden gestartet wird
#define u16GetRomConstChrgMaxCurrentBat()    (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0250)))       // Defaultvalue 100:   MaxCurrentBat [mA]      Ladestrombegrenzung. Maximaler Ladestrom der Akku.
#define u16GetRomConstChrgVoltCellDefect()   (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0252)))       // Defaultvalue 3200:  VoltCellDefect [mV]     Defektspannung. Bei dieser Spannung wird die Zelle als defekt erkannt und die Versorgung zur Lipo wird getrennt
#define u16GetRomConstChrgVoltPowerInCharged() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0254)))     // Defaultvalue 100:   VoltPowerInCharged [mV] Erhaltungsspannung. Sobald Lipo fertig geladen ist, wird diese Spannung eingestellt
#define u16GetRomConstChrgTimeOut1()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0256)))       // Defaultvalue 2000:  TimeOut1 [ms]           Timeout: Ein Spannung muss diese Zeit anliegen um gultig zu werden, z.B. Ladeschlussspannung
#define u16GetRomConstChrgTimeOut2()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0258)))       // Defaultvalue 1000:  TimeOut2 [ms]
#define u16GetRomConstChrgTimeOut3()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X025A)))       // Defaultvalue 500:   TimeOut3 [ms]
#define u16GetRomConstChrgTimeOut4()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X025C)))       // Defaultvalue 100:   TimeOut4 [ms]
#define u8GetRomConstModulInType()           (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X025E)))        // Defaultvalue 6:     Type des Input Moduls
#define u8GetRomConstModulInModeInput()      (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X025F)))        // Defaultvalue 0:     Input Modus des Input Modules
#define u8GetRomConstModulInModeOutput()     (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0260)))        // Defaultvalue 4:     Output Modus des Input Modules
#define u16GetRomConstModulInSetVoltInput()  (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0261)))       // Defaultvalue 5000:  Soll Input Spannung des Input Moduls [mV]
#define u16GetRomConstModulInSetCurrInput()  (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0263)))       // Defaultvalue 500:   Soll Input Strom des Input Moduls [mA]
#define u16GetRomConstModulInSetVoltOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0265)))       // Defaultvalue 5000:  Soll Output Spannung des Input Moduls [mV]
#define u16GetRomConstModulInSetCurrOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0267)))       // Defaultvalue 100:   Soll Output Strom des Input Moduls [mA]
#define u16GetRomConstModulInVoltMinSupply() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0269)))       // Defaultvalue 2500:  Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]
#define u16GetRomConstModulInVoltMax()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X026B)))       // Defaultvalue 18000: Maximale Ausgangsspannung. [mV]
#define fGetRomConstModulInInVoltLpIn()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X026D)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulInInVoltKp()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0271)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulInInVoltKi()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0275)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInInVoltKiCut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0279)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulInInVoltKd()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X027D)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInInVoltLpOut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0281)))     // Defaultvalue 1.0:   LP_Out
#define fGetRomConstModulInInCurrLpIn()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0285)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulInInCurrKp()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0289)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulInInCurrKi()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028D)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInInCurrKiCut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0291)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulInInCurrKd()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0295)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInInCurrLpOut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0299)))     // Defaultvalue 1.0:   LP_Out
#define fGetRomConstModulInOutVoltLpIn()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029D)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulInOutVoltKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02A1)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulInOutVoltKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02A5)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInOutVoltKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02A9)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulInOutVoltKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02AD)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInOutVoltLpOut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02B1)))     // Defaultvalue 1.0:   LP_Out
#define fGetRomConstModulInOutCurrLpIn()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02B5)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulInOutCurrKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02B9)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulInOutCurrKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02BD)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInOutCurrKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C1)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulInOutCurrKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C5)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInOutCurrLpOut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C9)))     // Defaultvalue 1.0:   LP_Out
#define fGetRomConstModulInReserve1()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CD)))     // Defaultvalue 1.0:   Reserve1
#define fGetRomConstModulInReserve2()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02D1)))     // Defaultvalue 1.0:   Reserve2
#define fGetRomConstModulInReserve3()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02D5)))     // Defaultvalue 1.0:   Reserve3
#define u8GetRomConstModulOutType()          (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02D9)))        // Defaultvalue 1:     Type des Output Moduls
#define u8GetRomConstModulOutModeInput()     (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02DA)))        // Defaultvalue 0:     Input Modus des Output Modules
#define u8GetRomConstModulOutModeOutput()    (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02DB)))        // Defaultvalue 0:     Output Modus des Output Modules
#define u16GetRomConstModulOutSetVoltInput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02DC)))       // Defaultvalue 5000:  Soll Input Spannung des Output Moduls [mV]
#define u16GetRomConstModulOutSetCurrInput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02DE)))       // Defaultvalue 500:   Soll Input Strom des Output Moduls [mA]
#define u16GetRomConstModulOutSetVoltOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E0)))      // Defaultvalue 5000:  Soll Output Spannung des Output Moduls [mV]
#define u16GetRomConstModulOutSetCurrOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E2)))      // Defaultvalue 100:   Soll Output Strom des Output Moduls [mA]
#define u16GetRomConstModulOutVoltMinSupply() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E4)))      // Defaultvalue 2500:  Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]
#define u16GetRomConstModulOutVoltMax()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E6)))       // Defaultvalue 18000: Maximale Ausgangsspannung. [mV]
#define fGetRomConstModulOutOutVoltLpIn()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E8)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulOutOutVoltKp()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02EC)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulOutOutVoltKi()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F0)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutOutVoltKiCut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F4)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulOutOutVoltKd()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F8)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulOutOutVoltLpOut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02FC)))     // Defaultvalue 1.0:   LP_Out
#define fGetRomConstModulOutOutCurrLpIn()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0300)))     // Defaultvalue 1.0:   LP_In
#define fGetRomConstModulOutOutCurrKp()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0304)))     // Defaultvalue 0.01:  Kp
#define fGetRomConstModulOutOutCurrKi()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0308)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutOutCurrKiCut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X030C)))     // Defaultvalue 500.0: KiCut
#define fGetRomConstModulOutOutCurrKd()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0310)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulOutOutCurrLpOut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0314)))     // Defaultvalue 1.0:   LP_Out
#define rszGetRomConstMagicPattern()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0FFC))           // Defaultvalue FLKA   
#define u32GetRomConstChecksum()             (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0FF8)))       // Defaultvalue 460792


#ifdef __cplusplus
}
#endif

#endif // __ROM_CONSTS_DEF_H__

