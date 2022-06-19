@echo off

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og\libhal_lib_og.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01.Og first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og\libhal_lib_og.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og\libhal_lib_og.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01.Og skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3\libhal_lib_o3.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3\libhal_lib_o3.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01.O3 skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os\libhal_lib_os.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01.Os first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os\libhal_lib_os.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os\libhal_lib_os.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01.Os skipped
)



if not exist .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_og\libhal_lib_og.a (
  @echo on
  call build STM/L4/HalLib/v01.17.00.Og first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.17.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_og\libhal_lib_og.a
  call copy /Y .\bin\STM\L4\HalLib\v01.17.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_og\libhal_lib_og.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.17.00.Og skipped
)


if not exist .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_o3\libhal_lib_o3.a (
  @echo on
  call build STM/L4/HalLib/v01.17.00.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.17.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_o3\libhal_lib_o3.a
  call copy /Y .\bin\STM\L4\HalLib\v01.17.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_o3\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.17.00.O3 skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_os\libhal_lib_os.a (
  @echo on
  call build STM/L4/HalLib/v01.17.00.Os first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.17.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_os\libhal_lib_os.a
  call copy /Y .\bin\STM\L4\HalLib\v01.17.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.17.00\HAL\_var\lib_os\libhal_lib_os.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.17.00.Os skipped
)

@echo on

