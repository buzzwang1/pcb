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

call build.bat   *STM\L4\STM32L433CCT\Mini\AddOn\PSwitch\v3\SLed.Release_App* first
call elf2bin.bat *STM\L4\STM32L433CCT\Mini\AddOn\PSwitch\v3\SLed.Release_App* first

cd %mypath%

::APP
copy %pcbroot%\bin\STM\L4\STM32L433CCT\Mini\AddOn\PSwitch\v3\I2c\Release_App\bin\STM32L433CCT_Mini_AddOn_PSwitch_v3_SLed.elf.bin in\0x0802C000_APP.bin

:: -9 => best compression
:: -k => keep (don't delete) input files
:: -f => force overwrite of output file and compress links
:: Die App zippen
call gzip -9 -k -f in\0x0802C000_APP.bin
call python gzipHack.py in\0x0802C000_APP.bin in\0x0802C000_APP.bin.gz out\0x0802C000_APP.bin.gz.bin
