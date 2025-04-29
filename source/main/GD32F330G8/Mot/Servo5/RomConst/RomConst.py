
RomConst = [0x00] * 0x400 # 1024


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



def SetFooter(lAdr, lszMagicPattern, llstList = None):
    SetString(lAdr, lszMagicPattern, 4, "MagicPattern", "", llstList)


def convert_byte_to_ascii(lu8Byte):
    lszStr = chr(int(lu8Byte))

    if lszStr.isprintable() == False:
        return "."
    return lszStr

def ToFp1814(lfValue):
  return int(round(lfValue * 16384))


def GenerateRomConst(liBaseAdr, liAdr):
    llstListElements = list()

    liIdx = 0
    for u8Byte in RomConst:
        RomConst[liIdx] = liIdx & 0xFF
        liIdx+=1

    SetHeader("Servo5", "GD32F330G8", "Cortex-M4,Rom 64KB,Ram 8KB",  0, liAdr, 5, 256, llstListElements)

    SetPartition(0, "Flash Bl",   16, 0, 0x08000000,                       16*1024, 0x0400, llstListElements)
    SetPartition(1, "Flash APP",  16, 0, 0x08000000 +  16 * 1024,          47*1024, 0x0400, llstListElements)
    SetPartition(2, "Flash IAP",  16, 0, 0x08000000 +  16 * 1024,          47*1024, 0x0400, llstListElements)
    SetPartition(3, "Flash EEP",  16, 0, 0x08000000 +  (16 + 47) * 1024,    1*1024, 0x0400, llstListElements)

    SetPartition(4, "Ram",         0, 0, 0x20000000,                        8*1024,      1, llstListElements)

    liIdx = 0x150

    # Input Filter
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "LpInputPos",        "LpInputPos",        llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "LpInputCurrent",    "LpInputCurrent",    llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "LpInputSupply",     "LpInputSupply",     llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "LpInputTemp",       "LpInputTemp",       llstListElements); liIdx += 4

    # PID für Positionregelung
    Setu32_ArmCm4(liIdx, ToFp1814(7.5),      "PidPosKp",          "PidPosKp",          llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(0.015625), "PidPosKi",          "PidPosKi",          llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(250.0),    "PidPosKd",          "PidPosKd",          llstListElements); liIdx += 4
    Setu16_ArmCm4(liIdx,            100,     "PidPosLimit",       "PidPosLimit",       llstListElements); liIdx += 2
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "PidPosLpOutput",    "PidPosLpOutput",    llstListElements); liIdx += 4

    # PID für Stromregelung
    Setu32_ArmCm4(liIdx, ToFp1814(100.0),    "PidCurKp",          "PidCurKp",          llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(1.0),      "PidCurKi",          "PidCurKi",          llstListElements); liIdx += 4
    Setu32_ArmCm4(liIdx, ToFp1814(0.0),      "PidCurKd",          "PidCurKd",          llstListElements); liIdx += 4
    Setu16_ArmCm4(liIdx,           1000,     "PidCurLimit",       "PidCurLimit",       llstListElements); liIdx += 2
    Setu32_ArmCm4(liIdx, ToFp1814(0.12),     "PidCurLpOutput",    "PidCurLpOutput",    llstListElements); liIdx += 4



    Setu8_ArmCm4(liIdx,              1,      "LedEnable",         "LedEnable",         llstListElements); liIdx += 1

    print("Bytes used : " + str(liIdx + 8) + ", Frei " + str(len(RomConst) - liIdx - 8))

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




liAdr = 0x1E10
liAdrAdd = 0x0010
liAdrCnt = 14

while (liAdrCnt > 0):
    GenerateRomConst(0x0800FC00, liAdr)
    liAdr += liAdrAdd
    liAdrCnt -= 1




