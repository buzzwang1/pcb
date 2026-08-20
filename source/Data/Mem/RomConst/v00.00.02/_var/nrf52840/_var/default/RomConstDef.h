#ifndef __ROM_CONSTS_DEF_H__
#define __ROM_CONSTS_DEF_H__

#ifdef __cplusplus
  extern "C" {
#endif

#define u8PtrRomConstDeviceName()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0000))
#define u8PtrRomConstPlatformName()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0010))
#define u8PtrRomConstPlatformInfo()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0020))
#define u8PtrRomConstBnDeviceID()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0040))
#define u8PtrRomConstBnAdr()                 ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0042))
#define u8PtrRomConstIapRamSize()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0044))
#define u8PtrRomConstRomConstPageCnt()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0048))
#define u8PtrRomConstPartitionCnt()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0049))
#define u8PtrRomConstTablesCnt()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004A))
#define u8PtrRomConstReserve1()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004C))
#define u8PtrRomConstHwInfo()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004C))
#define u8PtrRomConstRomConstVersion()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004E))
#define u8PtrRomConstDateTimeSyncTimeout_s() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0050))
#define u8PtrRomConstDateTimeSyncHour_s()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0051))
#define u8PtrRomConstDateTimeValidTimeout_d() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0052))
#define u8PtrRomConstReserve()               ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0053))
#define u8PtrRomConstRadioPingIntervall_s()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0054))
#define u8PtrRomConstRadioPingTimeoutSessionReq_ms() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0056))
#define u8PtrRomConstBaudUpLink()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0058))
#define u8PtrRomConstBaudSideLink()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005A))
#define u8PtrRomConstBaudDownLink1()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005C))
#define u8PtrRomConstBaudDownLink2()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005E))
#define u8PtrRomConstUpLink()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0060))
#define u8PtrRomConstSideLink()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0061))
#define u8PtrRomConstDownLink1()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0062))
#define u8PtrRomConstDownLink2()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0063))
#define u8PtrRomConstDownLink3()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0064))
#define u8PtrRomConstDownLink4()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0065))
#define u8PtrRomConstDownLink5()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0066))
#define u8PtrRomConstDownLink6()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0067))
#define u8PtrRomConstDownLink7()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0068))
#define u8PtrRomConstDownLink8()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0069))
#define u8PtrRomConstDownLink9()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006A))
#define u8PtrRomConstDownLink10()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006B))
#define u8PtrRomConstDownLink11()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006C))
#define u8PtrRomConstDownLink12()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006D))
#define u8PtrRomConstDownLink13()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006E))
#define u8PtrRomConstDownLink14()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006F))
#define u8PtrRomConstPartition0Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0070))
#define u8PtrRomConstPartition0Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0080))
#define u8PtrRomConstPartition0Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0082))
#define u8PtrRomConstPartition0BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0084))
#define u8PtrRomConstPartition0Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0088))
#define u8PtrRomConstPartition0BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X008C))
#define u8PtrRomConstPartition1Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0090))
#define u8PtrRomConstPartition1Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A0))
#define u8PtrRomConstPartition1Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A2))
#define u8PtrRomConstPartition1BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A4))
#define u8PtrRomConstPartition1Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A8))
#define u8PtrRomConstPartition1BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00AC))
#define u8PtrRomConstPartition2Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00B0))
#define u8PtrRomConstPartition2Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C0))
#define u8PtrRomConstPartition2Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C2))
#define u8PtrRomConstPartition2BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C4))
#define u8PtrRomConstPartition2Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C8))
#define u8PtrRomConstPartition2BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00CC))
#define u8PtrRomConstPartition3Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00D0))
#define u8PtrRomConstPartition3Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E0))
#define u8PtrRomConstPartition3Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E2))
#define u8PtrRomConstPartition3BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E4))
#define u8PtrRomConstPartition3Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E8))
#define u8PtrRomConstPartition3BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00EC))
#define u8PtrRomConstPartition4Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00F0))
#define u8PtrRomConstPartition4Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0100))
#define u8PtrRomConstPartition4Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0102))
#define u8PtrRomConstPartition4BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0104))
#define u8PtrRomConstPartition4Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0108))
#define u8PtrRomConstPartition4BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X010C))
#define u8PtrRomConstPartition5Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0110))
#define u8PtrRomConstPartition5Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0120))
#define u8PtrRomConstPartition5Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0122))
#define u8PtrRomConstPartition5BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0124))
#define u8PtrRomConstPartition5Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0128))
#define u8PtrRomConstPartition5BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X012C))
#define u8PtrRomConstPartition6Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0130))
#define u8PtrRomConstPartition6Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0140))
#define u8PtrRomConstPartition6Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0142))
#define u8PtrRomConstPartition6BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0144))
#define u8PtrRomConstPartition6Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0148))
#define u8PtrRomConstPartition6BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X014C))
#define u8PtrRomConstPartition7Name()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0150))
#define u8PtrRomConstPartition7Sort()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0160))
#define u8PtrRomConstPartition7Type()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0162))
#define u8PtrRomConstPartition7BaseAdr()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0164))
#define u8PtrRomConstPartition7Size()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0168))
#define u8PtrRomConstPartition7BlockSize()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X016C))
#define u8PtrRomConstReserve4()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0178))
#define u8PtrRomConstChecksum1()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0178))
#define u8PtrRomConstMagicPattern1()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X017C))
#define u8PtrRomConstPowerInVout()           ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0180))
#define u8PtrRomConstPowerInDac()            ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01A0))
#define u8PtrRomConstTable3()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01C0))
#define u8PtrRomConstTable4()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01E0))
#define u8PtrRomConstTable5()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0200))
#define u8PtrRomConstTable6()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0220))
#define u8PtrRomConstTable7()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0240))
#define u8PtrRomConstTable8()                ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0260))
#define u8PtrRomConstBoardType()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0280))
#define u8PtrRomConstChrgCellCount()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0281))
#define u8PtrRomConstChrgModeCharged()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0282))
#define u8PtrRomConstChrgModeStart()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0283))
#define u8PtrRomConstChrgVoltLipoOff()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0284))
#define u8PtrRomConstChrgVoltSysOff()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0286))
#define u8PtrRomConstChrgMaxVoltCell()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0288))
#define u8PtrRomConstChrgVoltBalance()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028A))
#define u8PtrRomConstChrgCurrentSave()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028C))
#define u8PtrRomConstChrgCurrentCharged()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028E))
#define u8PtrRomConstChrgVoltStart()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0290))
#define u8PtrRomConstChrgMaxCurrentBat()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0292))
#define u8PtrRomConstChrgVoltCellDefect()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0294))
#define u8PtrRomConstChrgVoltPowerInCharged() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0296))
#define u8PtrRomConstChrgTimeOut1()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0298))
#define u8PtrRomConstChrgTimeOut2()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029A))
#define u8PtrRomConstChrgTimeOut3()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029C))
#define u8PtrRomConstChrgTimeOut4()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029E))
#define u8PtrRomConstReserve5()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C0))
#define u8PtrRomConstReserve6()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C1))
#define u8PtrRomConstModulInType()           ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C1))
#define u8PtrRomConstModulInModeInput()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C2))
#define u8PtrRomConstModulInModeOutput()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C3))
#define u8PtrRomConstModulInSetVoltInput()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C4))
#define u8PtrRomConstModulInSetCurrInput()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C6))
#define u8PtrRomConstModulInSetVoltOutput()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C8))
#define u8PtrRomConstModulInSetCurrOutput()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CA))
#define u8PtrRomConstModulInVoltMinSupply()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CC))
#define u8PtrRomConstModulInVoltMax()        ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CE))
#define u8PtrRomConstReserve8()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E0))
#define u8PtrRomConstModulInInVoltKp()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E0))
#define u8PtrRomConstModulInInVoltKi()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E4))
#define u8PtrRomConstModulInInVoltKiCut()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E8))
#define u8PtrRomConstModulInInVoltKd()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02EC))
#define u8PtrRomConstModulInInCurrKp()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F0))
#define u8PtrRomConstModulInInCurrKi()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F4))
#define u8PtrRomConstModulInInCurrKiCut()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F8))
#define u8PtrRomConstModulInInCurrKd()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02FC))
#define u8PtrRomConstModulInOutVoltKp()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0300))
#define u8PtrRomConstModulInOutVoltKi()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0304))
#define u8PtrRomConstModulInOutVoltKiCut()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0308))
#define u8PtrRomConstModulInOutVoltKd()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X030C))
#define u8PtrRomConstModulInOutCurrKp()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0310))
#define u8PtrRomConstModulInOutCurrKi()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0314))
#define u8PtrRomConstModulInOutCurrKiCut()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0318))
#define u8PtrRomConstModulInOutCurrKd()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X031C))
#define u8PtrRomConstReserve7()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0321))
#define u8PtrRomConstModulOutType()          ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0321))
#define u8PtrRomConstModulOutModeInput()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0322))
#define u8PtrRomConstModulOutModeOutput()    ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0323))
#define u8PtrRomConstModulOutSetVoltInput()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0324))
#define u8PtrRomConstModulOutSetCurrInput()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0326))
#define u8PtrRomConstModulOutSetVoltOutput() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0328))
#define u8PtrRomConstModulOutSetCurrOutput() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032A))
#define u8PtrRomConstModulOutVoltMinSupply() ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032C))
#define u8PtrRomConstModulOutVoltMax()       ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032E))
#define u8PtrRomConstReserve9()              ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0340))
#define u8PtrRomConstModulOutInVoltKp()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0340))
#define u8PtrRomConstModulOutInVoltKi()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0344))
#define u8PtrRomConstModulOutInVoltKiCut()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0348))
#define u8PtrRomConstModulOutInVoltKd()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X034C))
#define u8PtrRomConstModulOutInCurrKp()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0350))
#define u8PtrRomConstModulOutInCurrKi()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0354))
#define u8PtrRomConstModulOutInCurrKiCut()   ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0358))
#define u8PtrRomConstModulOutInCurrKd()      ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X035C))
#define u8PtrRomConstModulOutOutVoltKp()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0360))
#define u8PtrRomConstModulOutOutVoltKi()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0364))
#define u8PtrRomConstModulOutOutVoltKiCut()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0368))
#define u8PtrRomConstModulOutOutVoltKd()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X036C))
#define u8PtrRomConstModulOutOutCurrKp()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0370))
#define u8PtrRomConstModulOutOutCurrKi()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0374))
#define u8PtrRomConstModulOutOutCurrKiCut()  ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0378))
#define u8PtrRomConstModulOutOutCurrKd()     ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X037C))
#define u8PtrRomConstReserve10()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B0))
#define u8PtrRomConstReserve11()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B8))
#define u8PtrRomConstChecksum2()             ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B8))
#define u8PtrRomConstMagicPattern2()         ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03BC))



#define rszGetRomConstDeviceName()           ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0000))           // Defaultvalue BleTest
#define rszGetRomConstPlatformName()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0010))           // Defaultvalue NRF52840
#define rszGetRomConstPlatformInfo()         ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0020))           // Defaultvalue Cortex-M4,Rom 1024K,Ram 256K
#define u16GetRomConstBnDeviceID()           (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0040)))       // Defaultvalue 34     
#define u16GetRomConstBnAdr()                (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0042)))       // Defaultvalue 4096   
#define u32GetRomConstIapRamSize()           (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0044)))       // Defaultvalue 4096   
#define u8GetRomConstRomConstPageCnt()       (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0048)))        // Defaultvalue 1      
#define u8GetRomConstPartitionCnt()          (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0049)))        // Defaultvalue 5      
#define u8GetRomConstTablesCnt()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004A)))        // Defaultvalue 8      
#define u8GetRomConstReserve1()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004C)))         // Defaultvalue 255    
#define u16GetRomConstHwInfo()               (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004C)))       // Defaultvalue 1      
#define u16GetRomConstRomConstVersion()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X004E)))       // Defaultvalue 2      
#define u8GetRomConstDateTimeSyncTimeout_s() (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0050)))        // Defaultvalue 180    
#define u8GetRomConstDateTimeSyncHour_s()    (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0051)))        // Defaultvalue 2      
#define u8GetRomConstDateTimeValidTimeout_d() (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0052)))       // Defaultvalue 2      
#define u8GetRomConstReserve()               (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0053)))        // Defaultvalue 0      
#define u16GetRomConstRadioPingIntervall_s() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0054)))       // Defaultvalue 15     
#define u16GetRomConstRadioPingTimeoutSessionReq_ms() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0056))) // Defaultvalue 50  
#define u16GetRomConstBaudUpLink()           (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0058)))       // Defaultvalue 400    
#define u16GetRomConstBaudSideLink()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005A)))       // Defaultvalue 100    
#define u16GetRomConstBaudDownLink1()        (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005C)))       // Defaultvalue 400    
#define u16GetRomConstBaudDownLink2()        (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X005E)))       // Defaultvalue 200    
#define u8GetRomConstUpLink()                (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0060)))        // Defaultvalue 1      
#define u8GetRomConstSideLink()              (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0061)))        // Defaultvalue 1      
#define u8GetRomConstDownLink1()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0062)))        // Defaultvalue 1      
#define u8GetRomConstDownLink2()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0063)))        // Defaultvalue 1      
#define u8GetRomConstDownLink3()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0064)))        // Defaultvalue 1      
#define u8GetRomConstDownLink4()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0065)))        // Defaultvalue 1      
#define u8GetRomConstDownLink5()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0066)))        // Defaultvalue 1      
#define u8GetRomConstDownLink6()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0067)))        // Defaultvalue 1      
#define u8GetRomConstDownLink7()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0068)))        // Defaultvalue 1      
#define u8GetRomConstDownLink8()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0069)))        // Defaultvalue 1      
#define u8GetRomConstDownLink9()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006A)))        // Defaultvalue 1      
#define u8GetRomConstDownLink10()            (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006B)))        // Defaultvalue 1      
#define u8GetRomConstDownLink11()            (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006C)))        // Defaultvalue 1      
#define u8GetRomConstDownLink12()            (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006D)))        // Defaultvalue 1      
#define u8GetRomConstDownLink13()            (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006E)))        // Defaultvalue 1      
#define u8GetRomConstDownLink14()            (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X006F)))        // Defaultvalue 1      
#define rszGetRomConstPartition0Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0070))           // Defaultvalue Flash Bl
#define u16GetRomConstPartition0Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0080)))       // Defaultvalue 16     
#define u16GetRomConstPartition0Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0082)))       // Defaultvalue 0      
#define u32GetRomConstPartition0BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0084)))       // Defaultvalue 0      
#define u32GetRomConstPartition0Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0088)))       // Defaultvalue 159744 
#define u32GetRomConstPartition0BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X008C)))       // Defaultvalue 4096   
#define rszGetRomConstPartition1Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0090))           // Defaultvalue Flash APP
#define u16GetRomConstPartition1Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A0)))       // Defaultvalue 16     
#define u16GetRomConstPartition1Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A2)))       // Defaultvalue 0      
#define u32GetRomConstPartition1BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A4)))       // Defaultvalue 159744 
#define u32GetRomConstPartition1Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00A8)))       // Defaultvalue 626688 
#define u32GetRomConstPartition1BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00AC)))       // Defaultvalue 4096   
#define rszGetRomConstPartition2Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00B0))           // Defaultvalue Flash IAP
#define u16GetRomConstPartition2Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C0)))       // Defaultvalue 16     
#define u16GetRomConstPartition2Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C2)))       // Defaultvalue 0      
#define u32GetRomConstPartition2BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C4)))       // Defaultvalue 790528 
#define u32GetRomConstPartition2Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00C8)))       // Defaultvalue 258048 
#define u32GetRomConstPartition2BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00CC)))       // Defaultvalue 4096   
#define rszGetRomConstPartition3Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00D0))           // Defaultvalue Flash EEP
#define u16GetRomConstPartition3Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E0)))       // Defaultvalue 16     
#define u16GetRomConstPartition3Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E2)))       // Defaultvalue 0      
#define u32GetRomConstPartition3BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E4)))       // Defaultvalue 1044480
#define u32GetRomConstPartition3Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00E8)))       // Defaultvalue 4096   
#define u32GetRomConstPartition3BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00EC)))       // Defaultvalue 4096   
#define rszGetRomConstPartition4Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X00F0))           // Defaultvalue Ram    
#define u16GetRomConstPartition4Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0100)))       // Defaultvalue 0      
#define u16GetRomConstPartition4Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0102)))       // Defaultvalue 0      
#define u32GetRomConstPartition4BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0104)))       // Defaultvalue 536870912
#define u32GetRomConstPartition4Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0108)))       // Defaultvalue 262144 
#define u32GetRomConstPartition4BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X010C)))       // Defaultvalue 1      
#define rszGetRomConstPartition5Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0110))           // Defaultvalue Dummy  
#define u16GetRomConstPartition5Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0120)))       // Defaultvalue 0      
#define u16GetRomConstPartition5Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0122)))       // Defaultvalue 0      
#define u32GetRomConstPartition5BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0124)))       // Defaultvalue 0      
#define u32GetRomConstPartition5Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0128)))       // Defaultvalue 0      
#define u32GetRomConstPartition5BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X012C)))       // Defaultvalue 1      
#define rszGetRomConstPartition6Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0130))           // Defaultvalue Dummy  
#define u16GetRomConstPartition6Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0140)))       // Defaultvalue 0      
#define u16GetRomConstPartition6Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0142)))       // Defaultvalue 0      
#define u32GetRomConstPartition6BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0144)))       // Defaultvalue 0      
#define u32GetRomConstPartition6Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0148)))       // Defaultvalue 0      
#define u32GetRomConstPartition6BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X014C)))       // Defaultvalue 1      
#define rszGetRomConstPartition7Name()       ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0150))           // Defaultvalue Dummy  
#define u16GetRomConstPartition7Sort()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0160)))       // Defaultvalue 0      
#define u16GetRomConstPartition7Type()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0162)))       // Defaultvalue 0      
#define u32GetRomConstPartition7BaseAdr()    (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0164)))       // Defaultvalue 0      
#define u32GetRomConstPartition7Size()       (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0168)))       // Defaultvalue 0      
#define u32GetRomConstPartition7BlockSize()  (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X016C)))       // Defaultvalue 1      
#define u8GetRomConstReserve4()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0178)))         // Defaultvalue 255    
#define u32GetRomConstChecksum1()            (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0178)))       // Defaultvalue 12355  
#define rszGetRomConstMagicPattern1()        ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X017C))           // Defaultvalue FLKA   
#define u16GetRomConstPowerInVout()          ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0180))          // Defaultvalue [4255, 4944, 5949, 6951, 7965, 8970, 9959, 10982, 11984, 12955, 13996, 14996, 15967, 17123, 18147, 19169]
#define u16GetRomConstPowerInDac()           ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01A0))          // Defaultvalue [4095, 3840, 3584, 3328, 3072, 2816, 2560, 2304, 2048, 1792, 1536, 1280, 1024, 768, 512, 256]
#define u16GetRomConstTable3()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01C0))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u16GetRomConstTable4()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X01E0))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u16GetRomConstTable5()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0200))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u16GetRomConstTable6()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0220))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u16GetRomConstTable7()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0240))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u16GetRomConstTable8()               ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0260))          // Defaultvalue [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#define u8GetRomConstBoardType()             (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0280)))        // Defaultvalue 0:     Type des Boards
#define u8GetRomConstChrgCellCount()         (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0281)))        // Defaultvalue 0:     Anzahl Zellen der Lipo
#define u8GetRomConstChrgModeCharged()       (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0282)))        // Defaultvalue 0:     Verhalten, wenn Laden beendet, 0: Charger aus, 1: Erhaltungsspannung
#define u8GetRomConstChrgModeStart()         (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0283)))        // Defaultvalue 0:     Verhalten, wenn Laden gestartet wird, 0: Sobald Spannung kleiner ChrgVoltStart, 1: Sobald Eingangsspannung vorhanden
#define u16GetRomConstChrgVoltLipoOff()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0284)))       // Defaultvalue 2800:  VoltLipoOff [mV]        Tiefentladeschutz. Bei der Spannung schaltet sich der uC ab. Versorgung zur Lipo wird getrennt
#define u16GetRomConstChrgVoltSysOff()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0286)))       // Defaultvalue 3000:  VoltSysOff [mV]         Unterspannungsschutz. Bei der Spannung wird der Ausgang ausgeschaltet und der uC geht in  den Stromsparmodus
#define u16GetRomConstChrgMaxVoltCell()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0288)))       // Defaultvalue 4050:  MaxVoltCell [mV]        Ladeschlussspannung. Bei der Spannung ist die Lipo geladen
#define u16GetRomConstChrgVoltBalance()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028A)))       // Defaultvalue 4200:  VoltBalance [mV]        Balancespannung. Bei der Spannung wird beim Laden der Balancer der Zelle aktiviert.  (0 == Balancer aus)
#define u16GetRomConstChrgCurrentSave()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028C)))       // Defaultvalue 25:    CurrentSave [mA]        Sichere Ladestrom. Ladestrom bei tiefentladener Zelle oder wenn Balancer einer Zelle an ist.
#define u16GetRomConstChrgCurrentCharged()   (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X028E)))       // Defaultvalue 300:   CurrentCharged [mA]     Laden wird beendet, sobald diese Strom unterschritten ist
#define u16GetRomConstChrgVoltStart()        (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0290)))       // Defaultvalue 3600:  VoltStart [mV]          Spannung, bei der das Laden gestartet wird
#define u16GetRomConstChrgMaxCurrentBat()    (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0292)))       // Defaultvalue 100:   MaxCurrentBat [mA]      Ladestrombegrenzung. Maximaler Ladestrom der Akku.
#define u16GetRomConstChrgVoltCellDefect()   (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0294)))       // Defaultvalue 1500:  VoltCellDefect [mV]     Defektspannung. Bei dieser Spannung wird die Zelle als defekt erkannt und die Versorgung zur Lipo wird getrennt
#define u16GetRomConstChrgVoltPowerInCharged() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0296)))     // Defaultvalue 100:   VoltPowerInCharged [mV] Erhaltungsspannung. Sobald Lipo fertig geladen ist, wird diese Spannung eingestellt
#define u16GetRomConstChrgTimeOut1()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0298)))       // Defaultvalue 2000:  TimeOut1 [ms]           Timeout: Ein Spannung muss diese Zeit anliegen um gultig zu werden, z.B. Ladeschlussspannung
#define u16GetRomConstChrgTimeOut2()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029A)))       // Defaultvalue 1000:  TimeOut2 [ms]
#define u16GetRomConstChrgTimeOut3()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029C)))       // Defaultvalue 500:   TimeOut3 [ms]
#define u16GetRomConstChrgTimeOut4()         (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X029E)))       // Defaultvalue 100:   TimeOut4 [ms]
#define u8GetRomConstReserve5()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C0)))         // Defaultvalue 255    
#define u8GetRomConstReserve6()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C1)))         // Defaultvalue 255    
#define u8GetRomConstModulInType()           (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C1)))        // Defaultvalue 0:     Type des Input Moduls
#define u8GetRomConstModulInModeInput()      (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C2)))        // Defaultvalue 0:     Input Modus des Input Modules
#define u8GetRomConstModulInModeOutput()     (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C3)))        // Defaultvalue 14:    Output Modus des Input Modules
#define u16GetRomConstModulInSetVoltInput()  (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C4)))       // Defaultvalue 5000:  Soll Input Spannung des Input Moduls [mV]
#define u16GetRomConstModulInSetCurrInput()  (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C6)))       // Defaultvalue 400:   Soll Input Strom des Input Moduls [mA]
#define u16GetRomConstModulInSetVoltOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02C8)))       // Defaultvalue 5000:  Soll Output Spannung des Input Moduls [mV]
#define u16GetRomConstModulInSetCurrOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CA)))       // Defaultvalue 400:   Soll Output Strom des Input Moduls [mA]
#define u16GetRomConstModulInVoltMinSupply() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CC)))       // Defaultvalue 2500:  Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]
#define u16GetRomConstModulInVoltMax()       (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02CE)))       // Defaultvalue 18000: Maximale Ausgangsspannung. [mV]
#define u8GetRomConstReserve8()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E0)))         // Defaultvalue 255    
#define fGetRomConstModulInInVoltKp()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E0)))     // Defaultvalue 0.1:   Kp
#define fGetRomConstModulInInVoltKi()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E4)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInInVoltKiCut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02E8)))     // Defaultvalue 200.0: KiCut
#define fGetRomConstModulInInVoltKd()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02EC)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInInCurrKp()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F0)))     // Defaultvalue 0.5:   Kp
#define fGetRomConstModulInInCurrKi()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F4)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInInCurrKiCut()     (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02F8)))     // Defaultvalue 100.0: KiCut
#define fGetRomConstModulInInCurrKd()        (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X02FC)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInOutVoltKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0300)))     // Defaultvalue 0.1:   Kp
#define fGetRomConstModulInOutVoltKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0304)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInOutVoltKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0308)))     // Defaultvalue 200.0: KiCut
#define fGetRomConstModulInOutVoltKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X030C)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulInOutCurrKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0310)))     // Defaultvalue 0.5:   Kp
#define fGetRomConstModulInOutCurrKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0314)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulInOutCurrKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0318)))     // Defaultvalue 100.0: KiCut
#define fGetRomConstModulInOutCurrKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X031C)))     // Defaultvalue 0.0:   Kd
#define u8GetRomConstReserve7()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0321)))         // Defaultvalue 255    
#define u8GetRomConstModulOutType()          (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0321)))        // Defaultvalue 0:     Type des Output Moduls
#define u8GetRomConstModulOutModeInput()     (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0322)))        // Defaultvalue 0:     Input Modus des Output Modules
#define u8GetRomConstModulOutModeOutput()    (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0323)))        // Defaultvalue 14:    Output Modus des Output Modules
#define u16GetRomConstModulOutSetVoltInput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0324)))       // Defaultvalue 5000:  Soll Input Spannung des Output Moduls [mV]
#define u16GetRomConstModulOutSetCurrInput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0326)))       // Defaultvalue 400:   Soll Input Strom des Output Moduls [mA]
#define u16GetRomConstModulOutSetVoltOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0328)))      // Defaultvalue 5000:  Soll Output Spannung des Output Moduls [mV]
#define u16GetRomConstModulOutSetCurrOutput() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032A)))      // Defaultvalue 1000:  Soll Output Strom des Output Moduls [mA]
#define u16GetRomConstModulOutVoltMinSupply() (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032C)))      // Defaultvalue 2500:  Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]
#define u16GetRomConstModulOutVoltMax()      (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X032E)))       // Defaultvalue 18000: Maximale Ausgangsspannung. [mV]
#define u8GetRomConstReserve9()              (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0340)))         // Defaultvalue 255    
#define fGetRomConstModulOutInVoltKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0340)))     // Defaultvalue 0.1:   Kp
#define fGetRomConstModulOutInVoltKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0344)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutInVoltKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0348)))     // Defaultvalue 200.0: KiCut
#define fGetRomConstModulOutInVoltKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X034C)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulOutInCurrKp()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0350)))     // Defaultvalue 0.5:   Kp
#define fGetRomConstModulOutInCurrKi()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0354)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutInCurrKiCut()    (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0358)))     // Defaultvalue 100.0: KiCut
#define fGetRomConstModulOutInCurrKd()       (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X035C)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulOutOutVoltKp()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0360)))     // Defaultvalue 0.1:   Kp
#define fGetRomConstModulOutOutVoltKi()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0364)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutOutVoltKiCut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0368)))     // Defaultvalue 200.0: KiCut
#define fGetRomConstModulOutOutVoltKd()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X036C)))     // Defaultvalue 0.0:   Kd
#define fGetRomConstModulOutOutCurrKp()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0370)))     // Defaultvalue 0.5:   Kp
#define fGetRomConstModulOutOutCurrKi()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0374)))     // Defaultvalue 0.0:   Ki
#define fGetRomConstModulOutOutCurrKiCut()   (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X0378)))     // Defaultvalue 100.0: KiCut
#define fGetRomConstModulOutOutCurrKd()      (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X037C)))     // Defaultvalue 0.0:   Kd
#define u8GetRomConstReserve10()             (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B0)))         // Defaultvalue 255    
#define u8GetRomConstReserve11()             (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B8)))         // Defaultvalue 255    
#define u32GetRomConstChecksum2()            (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03B8)))       // Defaultvalue 55969  
#define rszGetRomConstMagicPattern2()        ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + 0X03BC))           // Defaultvalue FLKA   

#ifdef __cplusplus
}
#endif

#endif // __ROM_CONSTS_DEF_H__

