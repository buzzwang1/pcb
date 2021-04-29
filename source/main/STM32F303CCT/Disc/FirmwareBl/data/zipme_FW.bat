set mypath=%cd%
cd ..\..\..\..\..\..\
call elf2bin.bat *STM32F3DISCOVERY\FirmwareBl* first

cd %mypath%

copy ..\..\..\..\..\..\bin\STM\F3\STM32F303VC\STM32F3DISCOVERY\FirmwareBl\Debug\cmake\bin\STM32F303CCT_Disc_FirmwareBl.elf.bin App.bin


call bl_zip App.bin