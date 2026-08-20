del App.bin
del Blu.bin
del RomConst.bin

copy ..\..\..\..\..\..\..\bin\STM\F1\GD32F1x0\UbTech\Servo1\App\Release\bin\GD32F1x0_UbTech_Servo1_App.elf.bin App.bin
copy ..\..\..\..\..\..\..\bin\STM\F1\GD32F1x0\UbTech\Servo1\Blu\Release\bin\GD32F1x0_UbTech_Servo1_Blu.elf.bin Blu.bin

copy ..\..\RomConst\RomConst_0X08007C00_0x1100.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1100.bin

copy ..\..\RomConst\RomConst_0X08007C00_0x1200.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1200.bin

copy ..\..\RomConst\RomConst_0X08007C00_0x1300.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1300.bin