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
        lImgAdrDest = 0x08000000 + 16 * 1024
        FileWriteu32(myDestFile, lImgAdrDest)

        # 28. Execution Adress, Null = no execution
        lImgAdrExe = 0x0
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

    print("File:     " + lszFileSourceOrig)
    print("ChkSum:   " + str(lOrigChecksum)  + " 0x" + hex(lOrigChecksum))
    print("Filesize: " + str(lOrigSize) + " 0x" + hex(lOrigSize))
    print("-----------------------------------------------------------------")
    print("File:     " + lszFileSourceZip)
    print("ChkSum:   " + str(lImgChecksum) + " 0x" + hex(lImgChecksum))
    print("Filesize: " + str(lImgSize) + " 0x" + hex(lImgSize))
    print("-----------------------------------------------------------------")
    print("File:     " + lszFileDest)
    print("ChkSum:   " + str(lImgChecksumAndHeader) + " 0x" + hex(lImgChecksumAndHeader))
    print("Filesize: " + str(lImgSize+32)  + " 0x" + hex(lImgSize+32) + " " + str(round((lImgSize+32)/lOrigSize*100, 1))+ "%")
    print("Dest Adr: " + str(lImgAdrDest) + " 0x" + hex(lImgAdrDest))
    print("Exe Adr:  " + str(lImgAdrExe) + " 0x" + hex(lImgAdrExe))


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
        lszFileSourceOrig = "U:\\E2\\Projekte\\SW\\Code\\pcb\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.bin"
        lszFileSourceZip  = "U:\\E2\\Projekte\\SW\\Code\\pcb\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.bin.gz"
        lszFileDest       = "U:\\E2\\Projekte\\SW\\Code\\pcb\source\\main\\STM32F303CCT\\Disc\\FirmwareBl\\data\\App.img"
        patch(lszFileSourceOrig, lszFileSourceZip, lszFileDest)




