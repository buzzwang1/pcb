#!/usr/bin/python

import os
import sys

# Write a pointer to the tail into the header timestamp field and modifies the checksum in the tail


def patch(lszFileSource, lszFileDest):

    lbaUInt32 = bytearray(4)
    ChkSum    = 1

    with open(lszFileSource, 'r+b') as myFile:
        myFileData = myFile.read()
        myOrigSize = len(myFileData)
        #Checksumme
        ChkSum = (ChkSum + sum(myFileData)) & 0xFFFFFFFF

        print("File:     " + lszFileSource)
        print("ChkSum:   " + str(ChkSum)  + " 0x" + hex(ChkSum))
        print("Filesize: " + str(myOrigSize) + " 0x" + hex(myOrigSize))

    with open(lszFileDest, 'r+b') as myFile:
        myFileData = myFile.read()
        lTailPtr = len(myFileData) - 8

        lbaUInt32[0] = (ChkSum >>  0) & 0xFF
        lbaUInt32[1] = (ChkSum >>  8) & 0xFF
        lbaUInt32[2] = (ChkSum >> 16) & 0xFF
        lbaUInt32[3] = (ChkSum >> 24) & 0xFF

        #Overwrite given checksum
        myFile.seek(lTailPtr)
        myFile.write(lbaUInt32)

        print("File:     " + lszFileDest)
        print("Filesize: " + str(lTailPtr)  + " 0x" + hex(lTailPtr))

        lbaUInt32[0] = (lTailPtr >>  0) & 0xFF
        lbaUInt32[1] = (lTailPtr >>  8) & 0xFF
        lbaUInt32[2] = (lTailPtr >> 16) & 0xFF
        lbaUInt32[3] = (lTailPtr >> 24) & 0xFF

        myFile.seek(4)
        myFile.write(lbaUInt32)


isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ

if not isRunningInPyCharm:

    lszFileSource =  sys.argv[1]
    lszFileDest   =  sys.argv[2]
    patch(lszFileSource, lszFileDest)
else:
    # Test mode
    if __name__ == "__main__":
        print("!! Testmode !!")
        lszFileSource = "U:\\E2\\Projekte\\SW\\Code\\Cmake_PC_Gzip\\source\\main\\PC\\Bn\\Test01\\data\\STM32F303CCT_Disc_Firmware.elf.bin"
        lszFileDest = "U:\\E2\\Projekte\\SW\\Code\\Cmake_PC_Gzip\\source\\main\\PC\\Bn\\Test01\\data\\STM32F303CCT_Disc_Firmware.elf.bin.gz"
        patch(lszFileSource, lszFileDest)




