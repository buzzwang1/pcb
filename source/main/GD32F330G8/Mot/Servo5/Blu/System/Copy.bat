del App.bin
del Blu.bin
del RomConst.bin

copy ..\..\..\..\..\..\..\bin\STM\F3\GD32F3x0\Mot\Servo5\App\Release\bin\GD32F3x0_Mot_Servo5_App.elf.bin App.bin
copy ..\..\..\..\..\..\..\bin\STM\F3\GD32F3x0\Mot\Servo5\Blu\Release\bin\GD32F3x0_Mot_Servo5_Blu.elf.bin Blu.bin
copy ..\..\RomConst\RomConst_0X0800FC00_0X1E10.bin RomConst.bin

python Merge.py

move Firmware.bin Firmware_0x1E10.bin

copy ..\..\RomConst\RomConst_0X08007C00_0X1E20.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1E20.bin

copy ..\..\RomConst\RomConst_0X08007C00_0X1E30.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1E30.bin