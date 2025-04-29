set mypath=%cd%
cd ..\..\..\..\..\..\
call elf2bin.bat *LedBlink\PE15Bl* first

cd %mypath%

copy ..\..\..\..\..\..\bin\STM\F3\STM32F303VC\STM32F3DISCOVERY\LedBlink\PE15Bl\Debug\cmake\bin\STM32F303CCT_LedBlink_PE15Bl.elf.bin App.bin


call bl_zip App.bin


