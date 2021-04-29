set mypath=%cd%
cd ..\..\..\..\..\..\..\..\
call elf2bin.bat *SolBx\02\Master\01.Debug_App* first

cd %mypath%

copy ..\..\..\..\..\..\..\..\bin\STM\F3\STM32F303CCT\SolBx\02\Master\01\Debug_App\cmake\bin\STM32F303CCT_SolBx_02_Master_01.elf.bin App_0x0802C000.bin


call bl_zip App_0x0802C000.bin