set mypath=%cd%
cd ..\..\..\..\..\..\..
call elf2bin.bat *STM\L4\STM32L433CCT\SolBx\01\App.Debug_App* first

cd %mypath%

copy ..\..\..\..\..\..\..\bin\STM\L4\STM32L433CCT\SolBx\01\App\Debug_App\bin\STM32L433CCT_SolBx_01_App.elf.bin Debug_App_Img_0x0802C000.bin


call bl_zip Debug_App_Img_0x0802C000.bin