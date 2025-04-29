del App.bin
del Blu.bin
del RomConst.bin


U:\E2\Projekte\SW\Code\pcb\bin\CH\V3\CH32V305\Mot\Servo5\App\Release\bin

copy ..\..\..\..\..\..\..\bin\CH\V3\CH32V305\Mot\Servo5\App\Release\bin\CH32V305_Mot_Servo5_App.elf.bin App.bin
copy ..\..\..\..\..\..\..\bin\CH\V3\CH32V305\Mot\Servo5\Blu\Release\bin\CH32V305_Mot_Servo5_Blu.elf.bin Blu.bin
copy ..\..\RomConst\RomConst_0X0801F000_0X1E10.bin RomConst.bin

python Merge.py

move Firmware.bin Firmware_0x1E10.bin

copy ..\..\RomConst\RomConst_0X0801F000_0X1E20.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1E20.bin

copy ..\..\RomConst\RomConst_0X0801F000_0X1E30.bin RomConst.bin
python Merge.py
move Firmware.bin Firmware_0x1E30.bin