@echo off
:: ==============================================================================
:: nRF52840 RAM Read Script - Vollstaendiger RAM Dump (256 kB)
:: ==============================================================================

:: EINSTELLUNGEN (Hier an deine Pfade anpassen!)
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg
set OUTPUT_FILE=full_ram_backup.bin

echo ==============================================================================
echo Lese RAM-Speicher (256 kB / 0x20000000 - 0x20040000) aus...
echo Ziel-Datei: %OUTPUT_FILE%
echo ==============================================================================
echo.

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "dump_image %OUTPUT_FILE% 0x20000000 0x40000" ^
  -c "reset run" ^
  -c "shutdown"

pause