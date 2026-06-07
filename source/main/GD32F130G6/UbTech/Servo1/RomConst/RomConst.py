import numpy as np
import struct

RomConst = [0xFF] * 1024






#         Name               0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15
Calib = [# ADC-Werte für Temperatur für NTC 10k bei 3300mV
         #               -40°C,  -35°C,  -30°C,  -25°C,  -20°C,  -15°C,  -10°C,   -5°C,    0°C,    5°C,   10°C,   15°C,   20°C,   25°C,   30°C,    35°C,  
         "ExtTempTab1;    3898;   3839;   3767;   3680;   3577;   3458;   3322;   3169;   3002;   2823;   2634;   2439;   2243;   2048;   1859;   1678",
         #                40°C,   45°C,   50°C,   55°C,   60°C,   65°C,   70°C,   75°C,   80°C,   85°C,   90°C,   95°C,  100°C,  105°C,  110°C,   115°C,
         "ExtTempTab2;    1508;   1349;   1203;   1071;    951;    843;    748;    662;    587;    520;    462;    410;    364;    324;    289;    258",

         # ADC-Werte für interne Temperatur bei 3300mV
         # GD32F1x0 User Manual: Temperature (°C) = {(V25 – Vtemperature(digit)) / Avg_Slope} + 25.
         #    - V25: Vtemperature value at 25°C, the typical value is 1.43 V.
         #    - Avg_Slope: Average Slope for curve between Temperature vs. Vtemperature, the typical value is 4.3 mV/°C.

         #               -40°C,  -35°C,  -30°C,  -25°C,  -20°C,  -15°C,  -10°C,   -5°C,    0°C,    5°C,   10°C,   15°C,   20°C,   25°C,   30°C,   35°C,  
         "IntTempTab1;    2122;   2095;   2068;   2042;   2015;   1988;   1962;   1935;   1908;   1882;   1855;   1828;   1802;   1775;   1748;   1722",
         #                40°C,   45°C,   50°C,   55°C,   60°C,   65°C,   70°C,   75°C,   80°C,   85°C,   90°C,   95°C,  100°C,  105°C,  110°C,   115°C,
         "IntTempTab2;    1695;   1668;   1642;   1615;   1588;   1561;   1535;   1508;   1481;   1455;   1428;   1401;   1375;   1348;   1321;   1295",

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
        if (self.mszType == "u8*"):    lszRet = ((("#define u8GetRomConst"     + self.mszName+"()").ljust(44) +" (((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + "           + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u8"):     lszRet = ((("#define u8GetRomConst"     + self.mszName+"()").ljust(44) +" (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + "          + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16"):    lszRet = ((("#define u16GetRomConst"    + self.mszName+"()").ljust(44) +" (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "         + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u32"):    lszRet = ((("#define u32GetRomConst"    + self.mszName+"()").ljust(44) +" (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + "         + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "float"):  lszRet = ((("#define fGetRomConst"      + self.mszName+"()").ljust(44) +" (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + "       + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "rsz"):    lszRet = ((("#define rszGetRomConst"    + self.mszName+"()").ljust(44) +" ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + "            + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16*"):   lszRet = ((("#define u16GetRomConst"    + self.mszName+"()").ljust(44) +" ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "           + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "Fp2210"): lszRet = ((("#define Fp2210GetRomConst" + self.mszName+"()").ljust(44) +" (*((cFixPti2210*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        return lszRet

    def ToAdr(self) -> str:
        lszRet = ""
        if (self.mszType == "u8*"):    lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "u8"):     lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "u16"):    lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "u32"):    lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "float"):  lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "rsz"):    lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "u16*"):   lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
        if (self.mszType == "Fp2210"): lszRet = (("#define u8PtrRomConst" + self.mszName+"()").ljust(44) +" ((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + " + "0x{0:04X}".format(self.miAdr).upper() + "))").strip()
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

def SetFp2210_ArmCm4(Adr, lfValue, lszName = "", lszComment = "", llstList = None):    
    a = np.float32(lfValue)
    hex_str=struct.pack('<f', a)

    lu32Value = int(round(lfValue * 1024))
    RomConst[Adr]     = lu32Value & 0xFF
    RomConst[Adr + 1] = (lu32Value >> 8) & 0xFF
    RomConst[Adr + 2] = (lu32Value >> 16) & 0xFF
    RomConst[Adr + 3] = (lu32Value >> 24) & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "Fp2210", lfValue, lszName, lszComment)
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
    lAdr = SetFloat_ArmCm4(lAdr, lfKp,    lszName + "Kp",       "Kp",     llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKi,    lszName + "Ki",       "Ki",     llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKiCut, lszName + "KiCut",    "KiCut",  llstList);
    lAdr = SetFloat_ArmCm4(lAdr, lfKd,    lszName + "Kd",       "Kd",     llstList);
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

    #liIdx = 0
    #for u8Byte in RomConst:
    #    RomConst[liIdx] = liIdx & 0xFF
    #    liIdx+=1

    lAdr = 0
    
    # BotNet Info
    #                     DeviceName PlatformName      PlatformInfo        Bn-DAdr   Bn-Adr   Ram    Pages  Partitions Tables  HwVer  RomConst-Ver
    lAdr = SetHeader(lAdr, "Servo1", "GD32L130G6", "CM3,Rom 32KB,Ram 4KB",  0xA1,   liBnAdr,  256,   1,       5,        5,     1,        2,        llstListElements)

    # BotNet Config
    #                       u8DateTimeSyncTimeout_s u8DateTimeSyncHour_s u8DateTimeValidTimeout_d   u16RadioPingIntervall_s   u16RadioPingTimeoutSessionReq_ms  u16BaudUpLink  u16BaudSideLink  u16BaudDownLink1  u16BaudDownLink2
    #                              3min                    02h00                 2 days                     15s                          50ms                       400khz          100khz          400khz             200khz
    lAdr = SetConfig(lAdr,         3*60,                     2,                    2,                       15,                           50,                       200,            100,            400,               200,        llstListElements)


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
    lAdr = SetPartition(lAdr,   0, "Flash Bl",       16,         0,    0x08000000,                            11*1024,       0x0400, llstListElements)
    lAdr = SetPartition(lAdr,   1, "Flash APP",      16,         0,    0x08000000 +  11 * 1024,               20*1024,       0x0400, llstListElements)
    lAdr = SetPartition(lAdr,   2, "Flash IAP",      16,         0,    0x08000000 +  11 * 1024,               20*1024,       0x0400, llstListElements)
    lAdr = SetPartition(lAdr,   3, "Flash EEP",      16,         0,    0x08000000 +  (11 + 20) * 1024,         1*1024,       0x0400, llstListElements)
    lAdr = SetPartition(lAdr,   4, "Ram",             0,         0,    0x20000000,                             4*1024,            1, llstListElements)
    lAdr = SetPartition(lAdr,   5, "Dummy",           0,         0,    0x40002850,                               4*32,            1, llstListElements)
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

    # Init-Werte für Sollwerte
    lAdr = SetFp2210_ArmCm4(lAdr, (0),        "InitPos_degree",            "InitPos_degree",           llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0),        "InitSpeed_degree_s",        "InitSpeed_degree_s",       llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1000),     "InitLimtCurrent_mA",        "InitLimtCurrent_mA",       llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (5000),     "InitLimitPower_mW",         "InitLimitPower_mW",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (5500),     "InitLimitVolt_mV",          "InitLimitVolt_mV",         llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (90),       "InitLimitIntTemp_Degree",   "InitLimitIntTemp_Degree",  llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (90),       "InitLimitExtTemp_Degree",   "InitLimitExtTemp_Degree",  llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),      "InitPwn_percent",           "InitPwn_percent",          llstListElements)

    # Input Filter
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputPos",         "LpInputPos",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputSpeed",       "LpInputSpeed",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputCurrent",     "LpInputCurrent",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputPower",       "LpInputPower",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputSupply",      "LpInputSupply",     llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputTemp1",       "LpInputTemp1",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "LpInputTemp2",       "LpInputTemp2",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.9),      "LpOutputMotor",      "LpOutputMotor",     llstListElements)

    # Input Skalierung
    # Positon Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (-280.0/4096.0), "CvrtInputPosF",     "CvrtInputPosF",     llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (-140.0),        "CvrtInputPosO",     "CvrtInputPosO",     llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, ( 140),          "CvrtInputPosC",     "CvrtInputPosC",     llstListElements)

    # Speed Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtInputSpeedF",   "CvrtInputSpeedF",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputSpeedO",   "CvrtInputSpeedO",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1000.0),        "CvrtInputSpeedC",   "CvrtInputSpeedC",    llstListElements)

    # Current Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtInputCurrentF", "CvrtInputCurrentF",  llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputCurrentO", "CvrtInputCurrentO",  llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (10000.0),       "CvrtInputCurrentC", "CvrtInputCurrentC",  llstListElements)

    # Power Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtInputPowerF",   "CvrtInputPowerF",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputPowerO",   "CvrtInputPowerO",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (65000.0),      "CvrtInputPowerC",   "CvrtInputPowerC",    llstListElements)
 
    # Supply Voltage Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, 14520.0/4096.0,  "CvrtInputSupplyF",  "CvrtInputSupplyF",   llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputSupplyO",  "CvrtInputSupplyO",   llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (20000.0),       "CvrtInputSupplyC",  "CvrtInputSupplyC",   llstListElements)
    
    # Internal Temperature Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtInputTemp1O",   "CvrtInputTemp1O",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputTemp1C",   "CvrtInputTemp1C",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (115.0),         "CvrtInputTemp1F",   "CvrtInputTemp1F",    llstListElements)
    
    # External Temperature Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtInputTemp2F",   "CvrtInputTemp2F",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtInputTemp2O",   "CvrtInputTemp2O",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (115.0),         "CvrtInputTemp2C",   "CvrtInputTemp2C",    llstListElements)

    # Output PMW zum Motor Sensor
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),           "CvrtOutputMotorF",  "CvrtOutputMotorF",   llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),           "CvrtOutputMotorO",  "CvrtOutputMotorO",   llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (100.0),         "CvrtOutputMotorC",  "CvrtOutputMotorC",   llstListElements)


    # Minimal Änderungen
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),      "MinDiffInputPos",        "MinDiffInputPos",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0),      "MinDiffInputSpeed",      "MinDiffInputSpeed",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (5.0),      "MinDiffInputCurrent",    "MinDiffInputCurrent",    llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (5.0),      "MinDiffInputPower",      "MinDiffInputPower",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (5.0),      "MinDiffInputSupply",     "MinDiffInputSupply",     llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (3.0),      "MinDiffInputTemp1",      "MinDiffInputTemp1",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (3.0),      "MinDiffInputTemp2",      "MinDiffInputTemp2",      llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (8.0),      "MinDiffOutputMotor",     "MinDiffOutputMotor",     llstListElements)

    # PID für Positionregelung
    lAdr = SetFp2210_ArmCm4(lAdr, (25.0),     "PidPosKp",          "PidPosKp",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1.0/32.0), "PidPosKi",          "PidPosKi",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (250.0),    "PidPosKd",          "PidPosKd",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (100.0),    "PidPosLimit",       "PidPosLimit",       llstListElements)

    # PID für Geschwindigkeitsregelung
    lAdr = SetFp2210_ArmCm4(lAdr, (25.0),     "PidSpeedKp",        "PidSpeedKp",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "PidSpeedKi",        "PidSpeedKi",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),      "PidSpeedKd",        "PidSpeedKd",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1000.0),   "PidSpeedLimit",     "PidSpeedLimit",     llstListElements)

    # PID für Stromregelung
    lAdr = SetFp2210_ArmCm4(lAdr, (25.0),     "PidCurKp",          "PidCurKp",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "PidCurKi",          "PidCurKi",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),      "PidCurKd",          "PidCurKd",          llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1000.0),   "PidCurLimit",       "PidCurLimit",       llstListElements)

    # PID für Leistungsregelung
    lAdr = SetFp2210_ArmCm4(lAdr, (10.0),     "PidPowerKp",        "PidPowerKp",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.2),      "PidPowerKi",        "PidPowerKi",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (0.0),      "PidPowerKd",        "PidPowerKd",        llstListElements)
    lAdr = SetFp2210_ArmCm4(lAdr, (1000.0),   "PidPowerLimit",     "PidPowerLimit",     llstListElements)


    lAdr = Setu8_ArmCm4(lAdr,      0x0F,      "LedEnable",         "LedEnable",         llstListElements)

    
    # RomConst End of Part 2
    lAdr = 0x03F8
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
        lszDefine += lcElement.ToAdr() + "\n";

    lszDefine += "\n" + "\n" + "\n";

    for lcElement in llstListElements:
        lszDefine += lcElement.ToString() + "\n";

    newFile = open("RomConst_"+"0x{0:08X}".format(liBaseAdr).upper()+"_"+"0x{0:04X}".format(liBnAdr).upper()+".txt", "w")
    newFile.write(lszDefine)
    newFile.close()




liAdr = 0x1100
liAdrAdd = 0x0100
liAdrCnt = 14

while (liAdrCnt > 0):
    GenerateRomConst(0x08007C00, liAdr)
    liAdr += liAdrAdd
    liAdrCnt -= 1




