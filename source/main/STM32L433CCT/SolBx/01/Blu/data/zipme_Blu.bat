set mypath=%cd%
cd ..\..\..\..\..\..\..
call elf2bin.bat *STM\L4\STM32L433CCT\SolBx\01\Blu.RamRelease* first

cd %mypath%

copy ..\..\..\..\..\..\..\bin\STM\L4\STM32L433CCT\SolBx\01\Blu\RamRelease\bin\STM32L433CCT_SolBx_01_Blu.elf.bin RamBlu.bin

call bl_zip RamBlu.bin