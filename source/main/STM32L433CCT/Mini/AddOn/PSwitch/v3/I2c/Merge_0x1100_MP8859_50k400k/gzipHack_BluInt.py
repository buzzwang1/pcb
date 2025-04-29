#!/usr/bin/python

import os
import sys

def FileWriteu32(loFile, lu32Value):
    lbaUInt32 = bytearray(4)
    lbaUInt32[0] = (lu32Value >> 24) & 0xFF
    lbaUInt32[1] = (lu32Value >> 16) & 0xFF
    lbaUInt32[2] = (lu32Value >> 8) & 0xFF
    lbaUInt32[3] = (lu32Value >> 0) & 0xFF
    loFile.write(lbaUInt32)

def FileWriteu32Inverse(loFile, lu32Value):
    lbaUInt32 = bytearray(4)
    lbaUInt32[0] = (lu32Value >> 0) & 0xFF
    lbaUInt32[1] = (lu32Value >> 8) & 0xFF
    lbaUInt32[2] = (lu32Value >> 16) & 0xFF
    lbaUInt32[3] = (lu32Value >> 24) & 0xFF
    loFile.write(lbaUInt32)

def FileGetChecksum(lszFile, lStartAdress):
    ChkSum = 1
    with open(lszFile, 'r+b') as myFile:
        myFileData = myFile.read()
        ChkSum = (ChkSum + sum(myFileData[lStartAdress:])) & 0xFFFFFFFF
        return ChkSum

def FileGetChecksum2(lszFile, lStartAdress):
    ChkSum = 1
    with open(lszFile, 'r+b') as myFile:
        myFileData = myFile.read()
        ChkSum = (ChkSum + sum(myFileData[lStartAdress:])) & 0xFFFFFFFF

        #for i in range(lStartAdress, 4): #len(myFileData)):
            #ChkSum += myFileData[i]
            #ChkSum &= 0xFFFFFFFF
        return ChkSum

def FileGetSize(lszFile):
    with open(lszFile, 'r+b') as myFile:
        myFileData = myFile.read()
        return len(myFileData)

def patch(lszFileSourceOrig, lszFileSourceZip, lszFileDest):
    ChkSum    = 1
    myOrigSize = 1
    lOrigChecksum = 0
    lImgChecksum = 0
    lImgChecksumAndHeader = 0
    lOrigSize = 0
    lImgSize = 0
    lImgAdrDest = 0
    lImgAdrExe = 0

    print("\n\n")

    with open(lszFileDest, 'wb') as myDestFile:

        #  0. Magic Patter:
        #       - 0x12345678: New Block, neededs to be flashed
        #       - 0x87654321: Block, already flashed

        FileWriteu32(myDestFile, 0x12345678)

        #  4. 0xFFFFFFFF: Reserved
        FileWriteu32(myDestFile, 0xFFFFFFFF)

        #  8. Checksum Image: Starting from Img. Adress 12
        #  Placeholder: will be written later
        lImgChecksum = 0x00000000
        FileWriteu32(myDestFile, lImgChecksum)

        # 12. Checksum Data
        lOrigChecksum = FileGetChecksum(lszFileSourceOrig, 0)
        FileWriteu32(myDestFile, lOrigChecksum)

        # 16. Size Image
        lImgSize = FileGetSize(lszFileSourceZip)
        FileWriteu32(myDestFile, lImgSize + 32)

        # 20. Size Data
        lOrigSize = FileGetSize(lszFileSourceOrig)
        FileWriteu32(myDestFile, lOrigSize)

        # 24. Destination Adress, to be programmed
        # lImgAdrDest = 0x08000000 + 32 * 1024 # Flash
        lImgAdrDest = 0x20000000 # RAM
        FileWriteu32(myDestFile, lImgAdrDest)

        # 28. Execution Adress, Null = no execution
        lImgAdrExe = 0x20000000 # RAM
        FileWriteu32(myDestFile, lImgAdrExe)

        # 32. Start of Image
        with open(lszFileSourceZip, 'r+b') as myFileZip:
            myFileData = myFileZip.read()
            myDestFile.write(myFileData)

    # Overwrite the gzip Checksum by a simple checksum
    with open(lszFileDest, 'r+b') as myDestFile:
        myFileData = myDestFile.read()
        lTailPtr = len(myFileData) - 8
        myDestFile.seek(lTailPtr)
        FileWriteu32Inverse(myDestFile, lOrigChecksum)

    # Overwrite the Placeholder '8. Checksum Image:'
    with open(lszFileDest, 'r+b') as myDestFile:
        myDestFile.seek(8)
        lImgChecksum          = FileGetChecksum(lszFileDest, 32)
        lImgChecksumAndHeader = FileGetChecksum(lszFileDest, 12)
        FileWriteu32(myDestFile, lImgChecksumAndHeader)

    lImgFileChecksum = FileGetChecksum2(lszFileDest, 0)

    lszStr = ""

    text_file = open(lszFileDest + ".txt", "w")

    lszStr += "File:      " + lszFileSourceOrig + "\r\n"
    lszStr += "ChkSum:    " + str(lOrigChecksum)  + " " + hex(lOrigChecksum)  + "\r\n"
    lszStr += "Filesize:  " + str(lOrigSize) + " " + hex(lOrigSize) + "\r\n"
    lszStr += "-----------------------------------------------------------------" + "\r\n"
    lszStr += "File:      " + lszFileSourceZip + "\r\n"
    lszStr += "ChkSum:    " + str(lImgChecksum) + " " + hex(lImgChecksum) + "\r\n"
    lszStr += "Filesize:  " + str(lImgSize) + " " + hex(lImgSize) + "\r\n"
    lszStr += "-----------------------------------------------------------------" + "\r\n"
    lszStr += "File:      " + lszFileDest + "\r\n"
    lszStr += "ChkSum:    " + str(lImgChecksumAndHeader) + " " + hex(lImgChecksumAndHeader) + "\r\n"
    lszStr += "ChkSumFile:" + str(lImgFileChecksum) + " " + hex(lImgFileChecksum) + "\r\n"
    lszStr += "Filesize:  " + str(lImgSize+32)  + " " + hex(lImgSize+32) + " " + str(round((lImgSize+32)/lOrigSize*100, 1))+ "%" + "\r\n"
    lszStr += "Dest Adr:  " + str(lImgAdrDest) + " " + hex(lImgAdrDest) + "\r\n"
    lszStr += "Exe Adr:   " + str(lImgAdrExe) + " " + hex(lImgAdrExe) + "\r\n"

    print(lszStr)
    text_file.write(lszStr)

    text_file.close()

    print("-----------------------------------------------------------------")
    print("Fill up with '0'")
    # Open img file and fill up with '0' to the next 2048 border
    with open(lszFileDest, 'r+b') as myDestFile:
        myDestFile.seek(lImgSize+32)

        lBytestoFill = 2048 - ((lImgSize+32) & 2047)
        if (lBytestoFill > 0):
            lbaBytestoFill = bytearray(lBytestoFill)
            myDestFile.write(lbaBytestoFill)



isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ

if not isRunningInPyCharm:

    lszFileSourceOrig = sys.argv[1]
    lszFileSourceZip  = sys.argv[2]
    lszFileDest       = sys.argv[3]
    patch(lszFileSourceOrig, lszFileSourceZip, lszFileDest)
else:
    # Test mode
    if __name__ == "__main__":
        print("!! Testmode !!")
        lszFileSourceOrig = "U:\\E2\\Projekte\\SW\\Code\\pcb\\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.bin"
        lszFileSourceZip  = "U:\\E2\\Projekte\\SW\\Code\\pcb\\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.bin.gz"
        lszFileDest       = "U:\\E2\\Projekte\\SW\\Code\\pcb\\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.img"
        patch(lszFileSourceOrig, lszFileSourceZip, lszFileDest)




