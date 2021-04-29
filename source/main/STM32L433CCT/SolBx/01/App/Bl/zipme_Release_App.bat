set mypath=%cd%
cd ..\..\..\..\..\..\..
call elf2bin.bat *STM\L4\STM32L433CCT\SolBx\01\App.Release_App* first

cd %mypath%

copy ..\..\..\..\..\..\..\bin\STM\L4\STM32L433CCT\SolBx\01\App\Release_App\bin\STM32L433CCT_SolBx_01_App.elf.bin Release_App_Img_0x0802C000.bin


call bl_zip Release_App_Img_0x0802C000.bin