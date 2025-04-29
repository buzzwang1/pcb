:: Alle Datei hierherkopieren
IF exist in (
  RMDIR "in" /S /Q
  md in  
) ELSE (
md in
)

IF exist out (
  RMDIR "out" /S /Q
  md out  
) ELSE (
  md out
)


set mypath=%cd%
cd ..\..\..\..\..\..\..\..\..
set pcbroot=%cd%


call build.bat *STM/L4/STM32L433CCT/Mini/Sys/BL.Release* first
call elf2bin.bat *STM/L4/STM32L433CCT/Mini/Sys/BL.Release* first


call build.bat *STM/L4/STM32L433CCT/Mini/Sys/BLU/MB_nrf905.RamRelease* first
call elf2bin.bat *STM/L4/STM32L433CCT/Mini/Sys/BLU/MB_nrf905.RamRelease* first


call build.bat *STM/L4/STM32L433CCT/Mini/MBv10b2/Lampe/Pflanzen/01.Release_App* first
call elf2bin.bat *STM/L4/STM32L433CCT/Mini/MBv10b2/Lampe/Pflanzen/01.Release_App* first


cd %mypath%

md in
md out

::BL
copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\Sys\BL\Release\bin\STM32L433CCT_Mini_Sys_BL.elf.bin in\0x08000000_BL.bin

::BLU
copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\Sys\BLU\MB_nrf905\RamRelease\bin\STM32L433CCT_Mini_Sys_BLU_MB_nrf905.elf.bin in\0x08002000_BLU.bin

::APP
copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\MBv10b2\Lampe\Pflanzen\01\Release_App\bin\STM32L433CCT_Mini_MBv10b2_Pflanzen_01.elf.bin in\0x08008000_APP.bin

::RomConst
cd ..\RomConst\RomConst_0X0803F000_0X1000_1s_LiPo_TP4056
call Make.Bat
cd %mypath%
copy ..\RomConst\RomConst_0X0803F000_0X1000_1s_LiPo_TP4056\RomConst_0X0803F000_0X1000.bin in\0x0803F000_RomConst.bin



:: -9 => best compression
:: -k => keep (don't delete) input files
:: -f => force overwrite of output file and compress links
:: Den Blu zippen
call gzip -9 -k -f in\0x08002000_BLU.bin
call python gzipHack_BluInt.py in\0x08002000_BLU.bin in\0x08002000_BLU.bin.gz in\0x08002000_BLU_Int.bin.gz.bin
call python gzipHack_BluExt.py in\0x08002000_BLU.bin in\0x08002000_BLU.bin.gz in\0x08002000_BLU_Ext.bin.gz.bin

call gzip -9 -k -f in\0x08008000_APP.bin
call python gzipHack_App.py in\0x08008000_APP.bin in\0x08008000_APP.bin.gz in\0x08008000_APP.bin.gz.bin


python Merge.py
copy in\0x08008000_APP.bin.gz.bin         out\0x08008000_APP.img
copy in\0x08008000_APP.bin.gz.bin.txt     out\0x08008000_APP.img.txt
copy in\0x08002000_BLU_Int.bin.gz.bin     out\0x08002000_BLU.img
copy in\0x08002000_BLU_Int.bin.gz.bin.txt out\0x08002000_BLU.img.txt

