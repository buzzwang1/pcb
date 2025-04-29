import numpy as np
import struct

RomConst = [0x00] * 4096

Calib = ["PowerInVout;4255;4944;5949;6951;7965;8970;9959;10982;11984;12955;13996;14996;15967;17123;18147;19169;19889",
         "PowerInDac;4095;3840;3584;3328;3072;2816;2560;2304;2048;1792;1536;1280;1024;768;512;256;0",
         "DigitAdc0;3330;3329;3327;3325;3332;3328;3321;3328;3323;3321;3323;3316;3275;3191;3116;3044;2933",
         "DigitAdc1;887;1034;1245;1456;1669;1879;2088;2303;2512;2721;2934;3144;3361;3593;3808;4025;4094",
         "DigitAdc2;902;1051;1266;1480;1697;1910;2125;2342;2555;2768;2984;3198;3417;3653;3873;4091;4094",
         "DigitAdc3;903;1052;1267;1481;1698;1912;2126;2344;2557;2770;2986;3200;3420;3657;3876;4092;4094",
         "DigitAdc4;884;1030;1241;1450;1663;1872;2081;2295;2504;2712;2924;3133;3349;3580;3795;4010;4094"]


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
        if (self.mszType == "u8"):    lszRet = ((("#define u8GetRomConst"  + self.mszName+"()").ljust(44) +" (*((u8*)(ROMCONST_PARTITION_START_ADRESS_EEP + "      + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16"):   lszRet = ((("#define u16GetRomConst" + self.mszName+"()").ljust(44) +" (*((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "     + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u32"):   lszRet = ((("#define u32GetRomConst" + self.mszName+"()").ljust(44) +" (*((u32*)(ROMCONST_PARTITION_START_ADRESS_EEP + "     + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "float"): lszRet = ((("#define fGetRomConst"   + self.mszName+"()").ljust(44) +" (*((float*)(ROMCONST_PARTITION_START_ADRESS_EEP + "   + "0x{0:04X}".format(self.miAdr).upper() + ")))").ljust(108) + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "rsz"):   lszRet = ((("#define rszGetRomConst" + self.mszName+"()").ljust(44) +" ((rsz)(ROMCONST_PARTITION_START_ADRESS_EEP + "        + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        if (self.mszType == "u16*"):  lszRet = ((("#define u16GetRomConst" + self.mszName+"()").ljust(44) +" ((u16*)(ROMCONST_PARTITION_START_ADRESS_EEP + "       + "0x{0:04X}".format(self.miAdr).upper() + "))").ljust(108)  + " // Defaultvalue " + str(self.maValue) + lszComment).ljust(132) + (" " + self.mszComment).strip()
        return lszRet


def Setu8_ArmCm4(Adr, lu8Value, lszName = "", lszComment = "", llstList = None):
    lu8Value = lu8Value & 0xFF
    RomConst[Adr]     = lu8Value & 0xFF
    RomConst[Adr + 1] = lu8Value >> 8
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u8", lu8Value, lszName, lszComment)
        llstList.append(lcEntry)

def Setu16_ArmCm4(Adr, lu16Value, lszName = "", lszComment = "", llstList = None):
    lu16Value = lu16Value & 0xFFFF
    RomConst[Adr]     = lu16Value & 0xFF
    RomConst[Adr + 1] = (lu16Value >> 8) & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u16", lu16Value, lszName, lszComment)
        llstList.append(lcEntry)

def Setu32_ArmCm4(Adr, lu32Value, lszName = "", lszComment = "", llstList = None):
    lu32Value = lu32Value & 0xFFFFFFFF
    RomConst[Adr]     = lu32Value & 0xFF
    RomConst[Adr + 1] = (lu32Value >> 8) & 0xFF
    RomConst[Adr + 2] = (lu32Value >> 16) & 0xFF
    RomConst[Adr + 3] = (lu32Value >> 24) & 0xFF
    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(Adr, "u32", lu32Value, lszName, lszComment)
        llstList.append(lcEntry)

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

def SetArrayU16(liAdr, lu16Values, lszName = "", lszComment = "", llstList = None):
    liAdr2 = liAdr

    for lu16Value in lu16Values:
        Setu16_ArmCm4(liAdr, lu16Value)
        liAdr+=2

    if ((lszName != "") and (llstList != None)):
        lcEntry = cElementEntry(liAdr2, "u16*", lu16Values, lszName, lszComment, len(lu16Values))
        llstList.append(lcEntry)


def SetHeader(lszDeviceName, lszPlatformName, lszPlatformInfo, lu16BnDeviceID, lu16BnAdr, lu8PartitionCnt, lu32IapRamSize, llstList = None):
    lAdr = 0

    SetString(lAdr, lszDeviceName,   16, "DeviceName",   "", llstList); lAdr += 16
    SetString(lAdr, lszPlatformName, 16, "PlatformName", "", llstList); lAdr += 16
    SetString(lAdr, lszPlatformInfo, 32, "PlatformInfo", "", llstList); lAdr += 32

    Setu16_ArmCm4(lAdr, lu16BnDeviceID, "BnDeviceID",    "", llstList); lAdr += 2
    Setu16_ArmCm4(lAdr, lu16BnAdr,      "BnAdr",         "", llstList); lAdr += 2
    lAdr += 4

    Setu8_ArmCm4(lAdr, lu8PartitionCnt, "PartitionCnt",  "", llstList); lAdr += 1
    lAdr += 3
    Setu32_ArmCm4(lAdr, lu32IapRamSize, "IapRamSize",    "", llstList); lAdr += 4


def PartitionIdx2Adr(liIdx):
    return 0x50 + liIdx * 32


def SetPartition(liIdx, lszName, lu16Sort, lu16Type, lu32Start_Adr, lu32Size, lu32BlockSize, llstList = None):
    lAdr = PartitionIdx2Adr(liIdx)

    SetString(lAdr, lszName, 16, "Partition" + str(liIdx) + "Name", "", llstList)
    lAdr += 16

    Setu16_ArmCm4(lAdr, lu16Sort, "Partition" + str(liIdx) + "Sort", "", llstList); lAdr += 2
    Setu16_ArmCm4(lAdr, lu16Type, "Partition" + str(liIdx) + "Type", "", llstList); lAdr += 2

    Setu32_ArmCm4(lAdr, lu32Start_Adr, "Partition" + str(liIdx) + "BaseAdr",   "", llstList); lAdr += 4
    Setu32_ArmCm4(lAdr, lu32Size,      "Partition" + str(liIdx) + "Size",      "", llstList); lAdr += 4
    Setu32_ArmCm4(lAdr, lu32BlockSize, "Partition" + str(liIdx) + "BlockSize", "", llstList); lAdr += 4


def WriteCalib(liAdr, llstList = None):
    lszName = ""
    for lszlines in Calib:
        lszValues = lszlines.split(";")
		
        liIdx = 0
        lu16Values = list()
        for lszValue in lszValues:
            if (liIdx == 0):
                lszName = lszValue
            else:
                lu16Values.append(int(lszValue))
            liIdx += 1

        SetArrayU16(liAdr, lu16Values, lszName, "", llstList)
        liAdr += 2 * len(lu16Values)
    return liAdr


def SetFooter(lAdr, lszMagicPattern, llstList = None):
    SetString(lAdr, lszMagicPattern, 4, "MagicPattern", "", llstList)


def convert_byte_to_ascii(lu8Byte):
    lszStr = chr(int(lu8Byte))

    if lszStr.isprintable() == False:
        return "."
    return lszStr


def GenerateRomConst(liBaseAdr, liAdr):
    llstListElements = list()

    liIdx = 0
    for u8Byte in RomConst:
        RomConst[liIdx] = liIdx & 0xFF
        liIdx+=1

    SetHeader("MBv4b4", "STM32L433CCT6", "Cortex-M4,Rom 256KB,Ram 64KB",  21, 0x1000, 6, 4*1024, llstListElements)

    SetPartition(0, "Flash Bl",  16, 0, 0x08000000,                            32*1024, 0x0800, llstListElements)
    SetPartition(1, "Flash APP", 16, 0, 0x08000000 +  32 * 1024,              144*1024, 0x0800, llstListElements)
    SetPartition(2, "Flash IAP", 16, 0, 0x08000000 +  (32 + 144) * 1024,       76*1024, 0x0800, llstListElements)
    SetPartition(3, "Flash EEP", 16, 0, 0x08000000 +  (32 + 144 + 76) * 1024,   4*1024, 0x0800, llstListElements)

    SetPartition(4, "Ram",        0, 0, 0x20000000,                            48*1024,      1, llstListElements)
    SetPartition(5, "CCM",        0, 0, 0x10000000,                            16*1024,      1, llstListElements)

    #Analog Calibration
    liAdress = 0x150
    liAdress = WriteCalib(liAdress, llstListElements)

    #Regler für Eingangspannung
    SetFloat_ArmCm4(liAdress, 0.010, "P_In",     "P_In",             llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Kp",       "Kp",               llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Ki",       "Ki",               llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Kd",       "Kd",               llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 0.010, "P_Out",    "P_Out",            llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Reserve1", "Reserve1",         llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Reserve2", "Reserve2",         llstListElements); liAdress+=4;
    SetFloat_ArmCm4(liAdress, 1.000, "Reserve3", "Reserve3",         llstListElements); liAdress+=4;

    # Charger Settings
    Setu16_ArmCm4(liAdress,   3000, "VoltLipoOff",        "VoltLipoOff [mV]        Tiefentladeschutz. Bei der Spannung schaltet sich der uC ab. Versorgung zur Lipo wird getrennt",                     llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   3150, "VoltSysOff",         "VoltSysOff [mV]         Unterspannungsschutz. Bei der Spannung wird der Ausgang ausgeschaltet und der uC geht in  den Stromsparmodus",       llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   3550, "MaxVoltCell",        "MaxVoltCell [mV]        Ladeschlussspannung. Bei der Spannung ist die Lipo geladen",                                                         llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   3650, "VoltBalance",        "VoltBalance [mV]        Balancespannung. Bei der Spannung wird beim Laden der Balancer der Zelle aktiviert",                                 llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   5000, "MpptVoltInput",      "MPPT_VoltInput [mV]     MPPT Eingangsspannung. Bei MPPT wird versucht die Eingangsspannung auf diesen Level zu halten.",                     llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,     25, "CurrentSave",        "CurrentSave [mA]        Sichere Ladestrom. Ladestrom bei tiefentladener Zelle oder wenn Balancer einer Zelle an ist.",                       llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,    100, "MaxCurrentBat",      "MaxCurrentBat [mA]      Ladestrombegrenzung. Maximaler Ladestrom der Akku.",                                                                 llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,    500, "MaxCurrentInput",    "MaxCurrentInput [mA]    Eingangsstrombegrenzung. Maximaler Eingangsstrom = Eingangsboostfaktor x MaxCurrentBat + Strom des Systems",         llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   2000, "VoltCellDefect",     "VoltCellDefect [mV]     Defektspannung. Bei dieser Spannung wird die Zelle als defekt erkannt und die Versorgung zur Lipo wird getrennt",    llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,  10000, "VoltMinPowerIn",     "VoltMinPowerIn [mV]     Minnimale Boostspannung",                                                                                            llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress, 4*3650, "VoltMaxPowerIn",     "VoltMaxPowerIn [mV]     Maximale Boostspannung",                                                                                             llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,    100, "VoltPowerInCharged", "VoltPowerInCharged [mV] Erhaltungsspannung. Sobald Lipo fertig geladen ist, wird diese Spannung eingestellt",                                llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   2000, "TimeOut1",           "TimeOut1 [ms]           Timeout: Ein Spannung muss diese Zeit anliegen um gultig zu werden, z.B. Ladeschlussspannung",                       llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   1000, "TimeOut2",           "TimeOut2 [ms]",                                                                                                                              llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,    500, "TimeOut3",           "TimeOut3 [ms]",                                                                                                                              llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,    100, "TimeOut4",           "TimeOut4 [ms]",                                                                                                                              llstListElements); liAdress+=2; #
    Setu16_ArmCm4(liAdress,   2500, "VoltMinSupply",      "VoltMinSupply [mV]      Minimal Eingangsspannung. Minimal Spannung, damit der Eingagsbooster aktiviert wird",                                llstListElements); liAdress+=2; #

    Setu8_ArmCm4(liAdress,       4, "CellCount",          "Anzahl Zellen der Lipo",                                                                                                                     llstListElements); liAdress+=1; # Cell Count

    SetFooter(len(RomConst) - 4,"FLKA", llstListElements)

    lu32Checksum = 1
    for u8Byte in RomConst[0:len(RomConst)-8]:
        lu32Checksum += int(u8Byte)
    print("Checksumme : " + str(lu32Checksum) + " (" + hex(lu32Checksum) + ")")

    Setu32_ArmCm4(len(RomConst) - 8, lu32Checksum, "Checksum", "", llstListElements)


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
    newFile = open("RomConst_"+"0x{0:08X}".format(liBaseAdr).upper()+"_"+"0x{0:04X}".format(liAdr).upper()+".bin", "wb")
    newFile.write(newFileByteArray)
    newFile.close()

    lszDefine = str()

    for lcElement in llstListElements:
        lszDefine += lcElement.ToString() + "\n";

    newFile = open("RomConst_"+"0x{0:08X}".format(liBaseAdr).upper()+"_"+"0x{0:04X}".format(liAdr).upper()+".txt", "w")
    newFile.write(lszDefine)
    newFile.close()




liAdr = 0x1000
liAdrAdd = 0x1000
liAdrCnt = 1

while (liAdrCnt > 0):
    GenerateRomConst(0x0803F000, liAdr)
    liAdr += liAdrAdd
    liAdrCnt -= 1




