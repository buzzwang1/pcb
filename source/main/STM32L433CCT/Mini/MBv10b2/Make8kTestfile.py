RomConst = [0xFF] * 8*1024


liIdx = 0
while (liIdx < (len(RomConst) / 2)):
    RomConst[liIdx * 2 + 0] = liIdx >> 8
    RomConst[liIdx * 2 + 1] = liIdx & 0xFF
    liIdx += 1
    
newFileByteArray = bytearray(RomConst)
newFile = open("8kTestFile.bin", "wb")
newFile.write(newFileByteArray)
newFile.close()

print("Done")










