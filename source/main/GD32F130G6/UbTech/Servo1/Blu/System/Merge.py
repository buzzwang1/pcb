RomConst = [0xFF] * 32*1024



def LoadFile(liAdr, lszFilename):
    loData = open(lszFilename, "rb").read()

    for lbData in loData:
        RomConst[liAdr] = lbData
        liAdr += 1

LoadFile( 0 * 1024, "Blu.bin")
LoadFile(10 * 1024, "App.bin")
LoadFile(31 * 1024, "RomConst.bin")


newFileByteArray = bytearray(RomConst)
newFile = open("Firmware.bin", "wb")
newFile.write(newFileByteArray)
newFile.close()








