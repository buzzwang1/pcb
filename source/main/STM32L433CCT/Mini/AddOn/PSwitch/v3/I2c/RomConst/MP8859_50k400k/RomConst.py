import numpy as np
import struct

RomConst = [0x00] * 1024




                          
                          
#         Name               0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15
Calib = ["PowerInVout;    4255;   4944;   5949;   6951;   7965;   8970;   9959;  10982;  11984;  12955;  13996;  14996;  15967;  17123;  18147;  19169",
         "PowerInDac;     4095;   3840;   3584;   3328;   3072;   2816;   2560;   2304;   2048;   1792;   1536;   1280;   1024;    768;    512;    256",
         "Table3;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0",
         "Table4;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0",
         "Table5;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0",
         "Table6;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0",
         "Table7;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0",
         "Table8;            0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0;      0"]


class cElementEntry:
    def __init__(self, liAdr, lszType, laValue, lszName, lszComment = "", liSize = 0):
        self.miAdr = liAdr
        self.maValue = laValue
        self.mszType = lszType
        self.mszName = lszName
        self.mszComment = lszComment
        self.miSize = liSize

    def ToString(self) -> str:
        lszRet = ""
        if (self.mszComment).strip() != "":
            lszComment = ":"
        else:
            lszComment = ""
        if (self.mszType == "u8*"):   lszRet = ((("#define u8GetRomConst"  + self.mszName+"()").ljust(44) +" (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + "       + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u8"):    lszRet = ((("#define u8GetRomConst"  + self.mszName+"()").ljust(44) +" (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + "      + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16"):   lszRet = ((("#define u16GetRomConst" + self.mszName+"()").ljust(44) +" (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "     + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u32"):   lszRet = ((("#define u32GetRomConst" + self.mszName+"()").ljust(44) +" (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + "     + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "float"): lszRet = ((("#define fGetRomConst"   + self.mszName+"()").ljust(44) +" (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + "   + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "rsz"):   lszRet = ((("#define rszGetRomConst" + self.mszName+"()").ljust(44) +" ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + "        + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16*"):  lszRet = ((("#define u16GetRomConst" + self.mszName+"()").ljust(44) +" ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "       + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        return lszRet


def SetRes_ArmCm4(Adr, lu8Cnt, lszName = "", lszComment = "", llstList = None):
    while (lu8Cnt):
        RomConst[Adr] = 0xFF
        Adr    = Adr + 1;
        lu8Cnt = lu8Cnt - 1;

    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u8*", 0xFF, lszName, lszComment)
        llstList.append(lcEntry)
    return Adr

def Setu8_ArmCm4(Adr, lu8Value, lszName = "", lszComment = "", llstList = None):
    lu8Value = lu8Value & 0xFF
    RomConst[Adr]     = lu8Value & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u8", lu8Value, lszName, lszComment)
        llstList.append(lcEntry)
    return Adr + 1

def Setu16_ArmCm4(Adr, lu16Value, lszName = "", lszComment = "", llstList = None):
    lu16Value = lu16Value & 0xFFFF
    RomConst[Adr]     = lu16Value & 0xFF
    RomConst[Adr + 1] = (lu16Value >> 8) & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u16", lu16Value, lszName, lszComment)
        llstList.append(lcEntry)
    return Adr + 2

def Setu32_ArmCm4(Adr, lu32Value, lszName = "", lszComment = "", llstList = None):
    lu32Value = lu32Value & 0xFFFFFFFF
    RomConst[Adr]     = lu32Value & 0xFF
    RomConst[Adr + 1] = (lu32Value >> 8) & 0xFF
    RomConst[Adr + 2] = (lu32Value >> 16) & 0xFF
    RomConst[Adr + 3] = (lu32Value >> 24) & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u32", lu32Value, lszName, lszComment)
        llstList.append(lcEntry)
    return Adr + 4

def SetFloat_ArmCm4(Adr, lfValue, lszName = "", lszComment = "", llstList = None):
    a = np.float32(lfValue)
    hex_str=struct.pack('<f', a)
    RomConst[Adr]     = hex_str[0]
    RomConst[Adr + 1] = hex_str[1]
    RomConst[Adr + 2] = hex_str[2]
    RomConst[Adr + 3] = hex_str[3]
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "float", lfValue, lszName, lszComment)
        llstList.append(lcEntry)
    return Adr + 4

def SetString(Adr, lszString, liMaxSize, lszName = "", lszComment = "", llstList = None):
    liChrIdx = 0
    while (liChrIdx < liMaxSize) and (liChrIdx < len(lszString)):
        RomConst[Adr+liChrIdx] = ord(lszString[liChrIdx])
        liChrIdx += 1

    while (liChrIdx < liMaxSize):
        RomConst[Adr + liChrIdx] = 0
        liChrIdx += 1
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "rsz", lszString, lszName, lszComment, liMaxSize)
        llstList.append(lcEntry)
    return Adr + liMaxSize

def SetArrayU16(liAdr, lu16Values, lszName = "", lszComment = "", llstList = None):
    liAdr2 = liAdr

    for lu16Value in lu16Values:
        Setu16_ArmCm4(liAdr, lu16Value)
        liAdr+=2

    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(liAdr2, "u16*", lu16Values, lszName, lszComment, len(lu16Values))
        llstList.append(lcEntry)

    return liAdr


def SetHeader(lAdr, 
              lszDeviceName,      lszPlatformName,    lszPlatformInfo, lu16BnDeviceID, lu16BnAdr, 
              lu32IapRamSize,     lu8RomConstPageCnt, lu8PartitionCnt, lu8TablesCnt,   lu8HwInfo,       
              lu8RomConstVersion, llstList = None):
   
    lAdr = SetString(lAdr, lszDeviceName,   16, "DeviceName",   "", llstList);
    lAdr = SetString(lAdr, lszPlatformName, 16, "PlatformName", "", llstList);
    lAdr = SetString(lAdr, lszPlatformInfo, 32, "PlatformInfo", "", llstList);

    lAdr = Setu16_ArmCm4(lAdr, lu16BnDeviceID,     "BnDeviceID",      "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, lu16BnAdr,          "BnAdr",           "", llstList);
    lAdr = Setu32_ArmCm4(lAdr, lu32IapRamSize,     "IapRamSize",      "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  lu8RomConstPageCnt, "RomConstPageCnt", "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  lu8PartitionCnt,    "PartitionCnt",    "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  lu8TablesCnt,       "TablesCnt",       "", llstList);
    lAdr = SetRes_ArmCm4(lAdr, 1,                  "Reserve1",        "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, lu8HwInfo,          "HwInfo",          "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, lu8RomConstVersion, "RomConstVersion", "", llstList);

    return lAdr

def SetConfig(lAdr, 
              u8DateTimeSyncTimeout_s,  u8DateTimeSyncHour_s, 
              u8DateTimeValidTimeout_d,
              u16RadioPingIntervall_s,  u16RadioPingTimeoutSessionReq_ms,
              u16BaudUpLink,            u16BaudSideLink,
              u16BaudDownLink1,         u16BaudDownLink2,
              llstList = None):

    u8Reserve = 0

    lAdr = Setu8_ArmCm4(lAdr,  u8DateTimeSyncTimeout_s,          "DateTimeSyncTimeout_s",             "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  u8DateTimeSyncHour_s,             "DateTimeSyncHour_s",                "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  u8DateTimeValidTimeout_d,         "DateTimeValidTimeout_d",            "", llstList);
    lAdr = Setu8_ArmCm4(lAdr,  u8Reserve,                        "Reserve",                           "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16RadioPingIntervall_s,          "RadioPingIntervall_s",              "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16RadioPingTimeoutSessionReq_ms, "RadioPingTimeoutSessionReq_ms",     "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16BaudUpLink,                    "BaudUpLink",                        "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16BaudSideLink,                  "BaudSideLink",                      "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16BaudDownLink1,                 "BaudDownLink1",                     "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, u16BaudDownLink2,                 "BaudDownLink2",                     "", llstList);

    return lAdr


def SetPartition(lAdr, liIdx, lszName, lu16Sort, lu16Type, lu32Start_Adr, lu32Size, lu32BlockSize, llstList = None):

    lAdr = SetString(lAdr, lszName, 16, "Partition" + str(liIdx) + "Name", "", llstList)

    lAdr = Setu16_ArmCm4(lAdr, lu16Sort, "Partition" + str(liIdx) + "Sort", "", llstList);
    lAdr = Setu16_ArmCm4(lAdr, lu16Type, "Partition" + str(liIdx) + "Type", "", llstList);

    lAdr = Setu32_ArmCm4(lAdr, lu32Start_Adr, "Partition" + str(liIdx) + "BaseAdr",   "", llstList);
    lAdr = Setu32_ArmCm4(lAdr, lu32Size,      "Partition" + str(liIdx) + "Size",      "", llstList);
    lAdr = Setu32_ArmCm4(lAdr, lu32BlockSize, "Partition" + str(liIdx) + "BlockSize", "", llstList);

    return lAdr


def WriteCalib(liAdr, llstList = None):
    lszName = ""
    for lszlines in Calib:
        lszValues = lszlines.split(";")

        liIdx = 0
        lu16Values = list()
        for lszValue in lszValues:
            if (liIdx == 0):
                lszName = lszValue.strip()
            else:
                lu16Values.append(int(lszValue.strip()))
            liIdx += 1

        liAdr = SetArrayU16(liAdr, lu16Values, lszName, "", llstList)
    return liAdr

def SetPid(lAdr, lszName, lfLpIn, lfKp, lfKi, lfKiCut, lfKd, lfLpOut, llstList = None):
    lAdr = SetFloat_ArmCm4(lAdr, lfLpIn,  lszName + "LpIn",     "LP_In",  llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKp,    lszName + "Kp",       "Kp",     llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKi,    lszName + "Ki",       "Ki",     llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKiCut, lszName + "KiCut",    "KiCut",  llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKd,    lszName + "Kd",       "Kd",     llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfLpOut, lszName + "LpOut",    "LP_Out", llstList);

    return lAdr


def SetMagic(lAdr, lszMagicPattern, llstList = None):
    return SetString(lAdr, lszMagicPattern, 4, "MagicPattern", "", llstList)


def convert_byte_to_ascii(lu8Byte):
    lszStr = chr(int(lu8Byte))

    if lszStr.isprintable() == False:
        return "."
    return lszStr


def GenerateRomConst(liBaseAdr, liBnAdr):
    llstListElements = list()

    liIdx = 0
    for u8Byte in RomConst:
        RomConst[liIdx] = liIdx & 0xFF
        liIdx+=1

    lAdr = 0
    
    # BotNet Info
    #                                                                                   Bn-DAdr   Bn-Adr   Ram    Pages  Partitions Tables  HwVer  RomConst-Ver
    lAdr = SetHeader(lAdr, "MiniPsI2c", "STM32L433CCT6", "Cortex-M4,Rom 256KB,Ram 64KB", 0x41,    liBnAdr, 4*1024,   1,       7,        8,     3,       2,        llstListElements)

    # BotNet Config
    #                       u8DateTimeSyncTimeout_s u8DateTimeSyncHour_s u8DateTimeValidTimeout_d   u16RadioPingIntervall_s   u16RadioPingTimeoutSessionReq_ms  u16BaudUpLink  u16BaudSideLink  u16BaudDownLink1  u16BaudDownLink2
    #                              3min                    02h00                 2 days                     15s                          50ms                       400khz          100khz           50khz             200khz
    lAdr = SetConfig(lAdr,         3*60,                     2,                    2,                       15,                           50,                       400,            100,             50,               200,        llstListElements)


    # Botnet Link Config
    #   0: Disable
    # !=0: Enable
    lAdr = Setu8_ArmCm4(lAdr, 1, "UpLink",     "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "SideLink",   "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink1",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink2",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink3",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink4",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink5",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink6",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink7",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink8",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink9",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink10",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink11",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink12",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink13",  "", llstListElements)
    lAdr = Setu8_ArmCm4(lAdr, 1, "DownLink14",  "", llstListElements)

    # BotNet-Partitions
    #
    # Sort:
    #   0 = Internal Ram
    #   1 = External Ram
    #  16 = Internal Flash
    #  17 = External Flash
    #  32 = Internal EEP
    #  33 = External EEP
    #
    #                       liIdx,    lszName, lu16Sort,  lu16Type, lu32Start_Adr,                           lu32Size, lu32BlockSize
    lAdr = SetPartition(lAdr,   0, "Flash Bl",       16,         0,    0x08000000,                            32*1024,       0x0800, llstListElements)
    lAdr = SetPartition(lAdr,   1, "Flash APP",      16,         0,    0x08000000 +  32 * 1024,              144*1024,       0x0800, llstListElements)
    lAdr = SetPartition(lAdr,   2, "Flash IAP",      16,         0,    0x08000000 +  (32 + 144) * 1024,       76*1024,       0x0800, llstListElements)
    lAdr = SetPartition(lAdr,   3, "Flash EEP",      16,         0,    0x08000000 +  (32 + 144 + 76) * 1024,   4*1024,       0x0800, llstListElements)
    lAdr = SetPartition(lAdr,   4, "Ram",             0,         0,    0x20000000,                            48*1024,            1, llstListElements)
    lAdr = SetPartition(lAdr,   5, "BuRam",           0,         0,    0x40002850,                               4*32,            1, llstListElements)
    lAdr = SetPartition(lAdr,   6, "Dummy",           0,         0,    0x00000000,                                  0,            1, llstListElements)
    lAdr = SetPartition(lAdr,   7, "Dummy",           0,         0,    0x00000000,                                  0,            1, llstListElements)

    # RomConst End of Part 1
    lAdr = SetRes_ArmCm4(lAdr,  8, "Reserve4", "", llstListElements);

    lu32Checksum = 1
    for u8Byte in RomConst[0:lAdr]:
        lu32Checksum += int(u8Byte)
    print("Checksumme1 : " + str(lu32Checksum) + " (" + hex(lu32Checksum) + ")")

    lAdr = Setu32_ArmCm4(lAdr, lu32Checksum, "Checksum1",     "", llstListElements)
    lAdr = SetString(lAdr,     "FLKA",    4, "MagicPattern1", "", llstListElements)

    
    # Tables: Analog Calibration
    lAdr = WriteCalib(lAdr, llstListElements)


    
    lAdr = 0x02C0
    

    # Input/Output Modul
    #
    # Modultypen:
    # 0: Bridge
    # 1: Ein/Aus-Schalter
    # 2: MT3608 Boost Modul: DAC
    # 3: reserve
    # 4: MP2315 Buck: DAC
    # 5: reserve
    # 6: MP8859 Modul: Buck/Boost: I2C 
    # 7: TP4056 Modul: 1-Cell-Lipo Charger: DAC 

    # Operations Modus:
    #  0: Nichts
    #  1: SW Konstant I
    #  2: SW Konstant I mit Spannungbegrenzung
    #  3: SW Konstant U
    #  4: SW Konstant U mit Strombegrenzung
    # 11: HW Konstant I
    # 12: HW Konstant I mit Spannungbegrenzung
    # 13: HW Konstant U
    # 14: HW Konstant U mit Strombegrenzung

    # Eingangsmodul
    lAdr = SetRes_ArmCm4(lAdr,      1, "Reserve6",           "",                                                                                                                                           llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,       0, "ModulInType",        "Type des Input Moduls",                                                                                                                      llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,       0, "ModulInModeInput",   "Input Modus des Input Modules",                                                                                                              llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,      14, "ModulInModeOutput",  "Output Modus des Input Modules",                                                                                                             llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   5000, "ModulInSetVoltInput","Soll Input Spannung des Input Moduls [mV]",                                                                                                  llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,    400, "ModulInSetCurrInput","Soll Input Strom des Input Moduls [mA]",                                                                                                     llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   5000, "ModulInSetVoltOutput","Soll Output Spannung des Input Moduls [mV]",                                                                                                llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,    400, "ModulInSetCurrOutput","Soll Output Strom des Input Moduls [mA]",                                                                                                   llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   2500, "ModulInVoltMinSupply","Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]",                                                           llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,  18000, "ModulInVoltMax",      "Maximale Ausgangsspannung. [mV]",                                                                                                           llstListElements);
    lAdr = SetRes_ArmCm4(lAdr,     16, "Reserve8",             "",                                                                                                                                         llstListElements);

    # Eingangsmodul Reglereinstellungen
    lAdr = SetPid(lAdr, "ModulInInVolt",  1.000, 0.100, 0.000, 200.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulInInCurr",  1.000, 0.500, 0.000, 100.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulInOutVolt", 1.000, 0.100, 0.000, 200.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulInOutCurr", 1.000, 0.500, 0.000, 100.0, 0.000, 1.000, llstListElements)


    # Ausgangsmodul
    lAdr = SetRes_ArmCm4(lAdr,      1, "Reserve7",           "",                                                                                                                                            llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,       6, "ModulOutType",        "Type des Output Moduls",                                                                                                                     llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,       0, "ModulOutModeInput",   "Input Modus des Output Modules",                                                                                                             llstListElements);
    lAdr = Setu8_ArmCm4(lAdr,      14, "ModulOutModeOutput",  "Output Modus des Output Modules",                                                                                                            llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   5000, "ModulOutSetVoltInput","Soll Input Spannung des Output Moduls [mV]",                                                                                                 llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,    400, "ModulOutSetCurrInput","Soll Input Strom des Output Moduls [mA]",                                                                                                    llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   5000, "ModulOutSetVoltOutput","Soll Output Spannung des Output Moduls [mV]",                                                                                               llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   1000, "ModulOutSetCurrOutput","Soll Output Strom des Output Moduls [mA]",                                                                                                  llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,   2500, "ModulOutVoltMinSupply","Minimal Eingangsspannung. Minimal Spannung, damit das Modul aktiviert wird [mV]",                                                           llstListElements);
    lAdr = Setu16_ArmCm4(lAdr,  18000, "ModulOutVoltMax",      "Maximale Ausgangsspannung. [mV]",                                                                                                           llstListElements);
    lAdr = SetRes_ArmCm4(lAdr,     16, "Reserve9",             "",                                                                                                                                          llstListElements);

    # Ausgangsmodul Reglereinstellungen
    lAdr = SetPid(lAdr, "ModulOutInVolt",  1.000, 0.100, 0.000, 200.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulOutInCurr",  1.000, 0.500, 0.000, 100.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulOutOutVolt", 1.000, 0.100, 0.000, 200.0, 0.000, 1.000, llstListElements)
    lAdr = SetPid(lAdr, "ModulOutOutCurr", 1.000, 0.500, 0.000, 100.0, 0.000, 1.000, llstListElements)


    # Reserve
    lAdr = SetRes_ArmCm4(lAdr,  3 * 16, "Reserve10", "", llstListElements);
    lAdr = SetRes_ArmCm4(lAdr,       8, "Reserve11", "", llstListElements);
    
    # RomConst End of Part 2
    lu32Checksum = 1
    for u8Byte in RomConst[0:lAdr]:
        lu32Checksum += int(u8Byte)
    print("Checksumme2 : " + str(lu32Checksum) + " (" + hex(lu32Checksum) + ")")

    lAdr = Setu32_ArmCm4(lAdr, lu32Checksum, "Checksum2",     "", llstListElements)
    lAdr = SetString(lAdr,     "FLKA",    4, "MagicPattern2", "", llstListElements)

    t = 0
    print('       0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F')
    while ((t + 15) < len(RomConst)):
        print('%04x: ' % t, end='')
        lszHexStr = ''.join('%02x.' % i for i in RomConst[t:t+16])
        print(lszHexStr + ": " + ''.join('%s' % convert_byte_to_ascii(i) for i in RomConst[t:t+16]))
        t += 16


    t = 0
    print('//           0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F')
    while ((t + 15) < len(RomConst)):
        print('/*%04x:*/' % t, end='')
        lszHexStr = ''.join(' 0x%02x,' % i for i in RomConst[t:t+16])
        print(lszHexStr + " // : " + ''.join('%s' % convert_byte_to_ascii(i) for i in RomConst[t:t+16]))
        t += 16


    newFileByteArray = bytearray(RomConst)
    newFile = open("RomConst_"+"0x{0:08X}".format(liBaseAdr).upper()+"_"+"0x{0:04X}".format(liBnAdr).upper()+".bin", "wb")
    newFile.write(newFileByteArray)
    newFile.close()

    lszDefine = str()

    for lcElement in llstListElements:
        lszDefine += lcElement.ToString() + "\n";

    newFile = open("RomConst_"+"0x{0:08X}".format(liBaseAdr).upper()+"_"+"0x{0:04X}".format(liBnAdr).upper()+".txt", "w")
    newFile.write(lszDefine)
    newFile.close()




liAdr = 0x1100
liAdrAdd = 0x0100
liAdrCnt = 1

while (liAdrCnt > 0):
    GenerateRomConst(0x0803F000, liAdr)
    liAdr += liAdrAdd
    liAdrCnt -= 1




