#!/usr/bin/python

import os
import sys

# Write a pointer to the tail into the header timestamp field


def patch(lszFilename):

    lbaTailPtr = bytearray(4)

    with open(lszFilename, 'r+b') as myFile:
        myFileData = myFile.read()
        lTailPtr = len(myFileData) - 8

        print("File:     " + lszFilename)
        print("Filesize: " + str(lTailPtr))

        lbaTailPtr[0] = (lTailPtr >>  0) & 0xFF
        lbaTailPtr[1] = (lTailPtr >>  8) & 0xFF
        lbaTailPtr[2] = (lTailPtr >> 16) & 0xFF
        lbaTailPtr[3] = (lTailPtr >> 24) & 0xFF

        myFile.seek(4)
        myFile.write(lbaTailPtr)


isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ

if not isRunningInPyCharm:
    lszFile = sys.argv[1]
    patch(lszFile)
else:
    # Test mode
    if __name__ == "__main__":
        print("!! Testmode !!")
        lszFile = "U:\\E2\\Projekte\\SW\\Code\\Cmake_PC_Gzip\\source\\main\\PC\\Bn\\Test01\\data\\STM32F303CCT_Disc_Firmware.elf.bin.gz"
        patch(lszFile)




