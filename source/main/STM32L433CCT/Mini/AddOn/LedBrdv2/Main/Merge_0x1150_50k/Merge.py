RomConst = [0xFF] * 256*1024


def LoadFile(liAdr, lszFilename):
    loData = open(lszFilename, "rb").read()

    print("Load '" + lszFilename + "' to " + str(liAdr) + " len " + str(len(loData)))

    for lbData in loData:
        RomConst[liAdr] = lbData
        liAdr += 1
    

print("\n\n")    

LoadFile(   0x000000, "in/0x08000000_BL.bin")
LoadFile(   0x002000, "in/0x08002000_BLU_int.bin.gz.bin")
LoadFile(   0x008000, "in/0x08008000_APP.bin")
LoadFile(   0x03F000, "in/0x0803F000_RomConst.bin")


newFileByteArray = bytearray(RomConst)
newFile = open("out/Merged.bin", "wb")
newFile.write(newFileByteArray)


print("Merge done")

newFile.close()








