@echo off
:: ==============================================================================
:: nRF52840 Flash Read Script - Vollstaendiger Flash Dump (1 MB)
:: ==============================================================================

:: EINSTELLUNGEN (Hier an deine Pfade anpassen!)
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg
set OUTPUT_FILE=full_flash_backup.bin

echo ==============================================================================
echo Lese kompletten Flash-Speicher (1 MB / 0x00000000 - 0x00100000) aus...
echo Ziel-Datei: %OUTPUT_FILE%
echo ==============================================================================
echo.

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "dump_image %OUTPUT_FILE% 0x00000000 0x100000" ^
  -c "reset run" ^
  -c "shutdown"

pause