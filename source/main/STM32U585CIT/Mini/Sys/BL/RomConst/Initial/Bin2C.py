import binascii
filename = 'RomConst_0X0803F000_0X1000.bin.gz.bin'
with open(filename, 'rb') as f:
    content = f.read()
hexstr = ""
for x in content:
    if (hexstr != ""): hexstr = hexstr + ", "
    hexstr = hexstr + ''.join('0x%02x' %x)

print(hexstr)








