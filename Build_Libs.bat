@echo off

::-------------------------------------F3 v01.11.00--------------------------------------------------------------------

if not exist .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a (
  @echo on
  call build STM/F3/HalLib/v01.11.00.Og first
  @echo.  
  @echo copy /Y .\bin\STM\F3\HalLib\v01.11.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a
  call copy /Y .\bin\STM\F3\HalLib\v01.11.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a
  @echo off
) else (
  @echo STM/F3/HalLib/v01.11.00.Og skipped
)

if not exist .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a (
  @echo on
  call build STM/F3/HalLib/v01.11.00.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\F3\HalLib\v01.11.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a
  call copy /Y .\bin\STM\F3\HalLib\v01.11.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/F3/HalLib/v01.11.00.O3 skipped
)

if not exist .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a (
  @echo on
  call build STM/F3/HalLib/v01.11.00.Os first
  @echo.  
  @echo copy /Y .\bin\STM\F3\HalLib\v01.11.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a
  call copy /Y .\bin\STM\F3\HalLib\v01.11.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\F3\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a
  @echo off
) else (
  @echo STM/F3/HalLib/v01.11.00.Os skipped
)

::-------------------------------------G4 v01.05.00--------------------------------------------------------------------

if not exist .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_og\libhal_lib_og.a (
  @echo on
  call build STM/G4/HalLib/v01.05.00.Og first
  @echo.  
  @echo copy /Y .\bin\STM\G4\HalLib\v01.05.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_og\libhal_lib_og.a
  call copy /Y .\bin\STM\G4\HalLib\v01.05.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_og\libhal_lib_og.a
  @echo off
) else (
  @echo STM/G4/HalLib/v01.05.00.Og skipped
)

if not exist .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_o3\libhal_lib_o3.a (
  @echo on
  call build STM/G4/HalLib/v01.05.00.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\G4\HalLib\v01.05.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_o3\libhal_lib_o3.a
  call copy /Y .\bin\STM\G4\HalLib\v01.05.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_o3\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/G4/HalLib/v01.05.00.O3 skipped
)

if not exist .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_os\libhal_lib_os.a (
  @echo on
  call build STM/G4/HalLib/v01.05.00.Os first
  @echo.  
  @echo copy /Y .\bin\STM\G4\HalLib\v01.05.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_os\libhal_lib_os.a
  call copy /Y .\bin\STM\G4\HalLib\v01.05.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\G4\HAL\v01.05.00\HAL\_var\lib_os\libhal_lib_os.a
  @echo off
) else (
  @echo STM/G4/HalLib/v01.05.00.Os skipped
)

::-------------------------------------L4 v01.15.01--------------------------------------------------------------------

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_8Mhz\libhal_lib_og.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_8Mhz.Og first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_8Mhz\libhal_lib_og.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_8Mhz\libhal_lib_og.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_8Mhz.Og_8Mhz skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_8Mhz\libhal_lib_o3.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_8Mhz.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_8Mhz\libhal_lib_o3.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_8Mhz\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_8Mhz.O3 skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_8Mhz\libhal_lib_os.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_8Mhz.Os first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_8Mhz\libhal_lib_os.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_8Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_8Mhz\libhal_lib_os.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_8Mhz.Os skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_24Mhz\libhal_lib_og.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_24Mhz.Og first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_24Mhz\libhal_lib_og.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_og_24Mhz\libhal_lib_og.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_24Mhz.Og_24Mhz skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_24Mhz\libhal_lib_o3.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_24Mhz.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_24Mhz\libhal_lib_o3.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_o3_24Mhz\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_24Mhz.O3 skipped
)

if not exist .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_24Mhz\libhal_lib_os.a (
  @echo on
  call build STM/L4/HalLib/v01.15.01_24Mhz.Os first
  @echo.  
  @echo copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_24Mhz\libhal_lib_os.a
  call copy /Y .\bin\STM\L4\HalLib\v01.15.01_24Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\L4\HAL\v01.15.01\HAL\_var\lib_os_24Mhz\libhal_lib_os.a
  @echo off
) else (
  @echo STM/L4/HalLib/v01.15.01_24Mhz.Os skipped
)

::-------------------------------------L4 v01.17.00--------------------------------------------------------------------

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


::-------------------------------------U5 v01.01.00-------------------------------------------

if not exist .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_og_32Mhz\libhal_lib_og.a (
  @echo on
  call build STM/U5/HalLib/v01.01.00_32Mhz.Og first
  @echo.  
  @echo copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_og_32Mhz\libhal_lib_og.a
  call copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_og_32Mhz\libhal_lib_og.a
  @echo off
) else (
  @echo STM/U5/HalLib/v01.01.00.Og_32Mhz skipped
)


if not exist .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_o3_32Mhz\libhal_lib_o3.a (
  @echo on
  call build STM/U5/HalLib/v01.01.00_32Mhz.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_o3_32Mhz\libhal_lib_o3.a
  call copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_o3_32Mhz\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/U5/HalLib/v01.01.00.O3_32Mhz skipped
)

if not exist .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_os_32Mhz\libhal_lib_os.a (
  @echo on
  call build STM/U5/HalLib/v01.01.00_32Mhz.Os first
  @echo.  
  @echo copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_os_32Mhz\libhal_lib_os.a
  call copy /Y .\bin\STM\U5\HalLib\v01.01.00_32Mhz\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\U5\HAL\v01.01.00\HAL\_var\lib_os_32Mhz\libhal_lib_os.a
  @echo off
) else (
  @echo STM/U5/HalLib/v01.01.00.Os_32Mhz skipped
)


::-------------------------------------H7B0 v01.11.00-------------------------------------------

if not exist .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a (
  @echo on
  call build STM/H7/HalLib/v01.11.00.Og first
  @echo.  
  @echo copy /Y .\bin\STM\H7\HalLib\v01.11.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a
  call copy /Y .\bin\STM\H7\HalLib\v01.11.00\Og\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_og\libhal_lib_og.a
  @echo off
) else (
  @echo STM/H7/HalLib/v01.11.00.Og skipped
)


if not exist .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a (
  @echo on
  call build STM/H7/HalLib/v01.11.00.O3 first
  @echo.  
  @echo copy /Y .\bin\STM\H7\HalLib\v01.11.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a
  call copy /Y .\bin\STM\H7\HalLib\v01.11.00\O3\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_o3\libhal_lib_o3.a
  @echo off
) else (
  @echo STM/H7/HalLib/v01.11.00.O3 skipped
)

if not exist .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a (
  @echo on
  call build STM/H7/HalLib/v01.11.00.Os first
  @echo.  
  @echo copy /Y .\bin\STM\H7\HalLib\v01.11.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a
  call copy /Y .\bin\STM\H7\HalLib\v01.11.00\Os\cmake\lib\libhal_lib.a .\source\Driver\STM32\H7\HAL\v01.11.00\HAL\_var\lib_os\libhal_lib_os.a
  @echo off
) else (
  @echo STM/H7/HalLib/v01.11.00.Os skipped
)


@echo on

