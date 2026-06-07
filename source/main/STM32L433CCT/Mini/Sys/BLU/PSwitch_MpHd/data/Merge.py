RomConst = [0xFF] * 32*1024


def LoadFile(liAdr, lszFilename):
    loData = open(lszFilename, "rb").read()

    for lbData in loData:
        RomConst[liAdr] = lbData
        liAdr += 1

LoadFile( 0 * 1024, "Bl.bin")
LoadFile( 8 * 1024, "RamBlu.bin.gz.bin")


newFileByteArray = bytearray(RomConst)
newFile = open("MergeBLBLU.bin", "wb")
newFile.write(newFileByteArray)


print("Merge Bl.bin + RamBlu.bin => MergeBLBLU.bin")

newFile.close()








