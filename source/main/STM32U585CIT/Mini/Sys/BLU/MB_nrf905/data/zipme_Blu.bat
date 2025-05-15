set mypath=%cd%
cd ..\..\..\..\..\..\..\..
set pcbroot=%cd%

call build.bat *STM\L4\STM32L433CCT\Mini\Sys\BL.Release* first
call build.bat *STM\L4\STM32L433CCT\Mini\Sys\BLU\nrf905.RamRelease* first

call elf2bin.bat *STM\L4\STM32L433CCT\Mini\Sys\BL.Release* first
call elf2bin.bat *STM\L4\STM32L433CCT\Mini\Sys\BLU\nrf905.RamRelease* first

cd %mypath%

copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\Sys\BL\Release\bin\STM32L433CCT_Mini_Sys_BL.elf.bin Bl.bin
copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\Sys\BLU\nrf905\RamRelease\bin\STM32L433CCT_Mini_Sys_BLU_nrf905.elf.bin RamBlu.bin

call bl_zip RamBlu.bin

python Merge.py