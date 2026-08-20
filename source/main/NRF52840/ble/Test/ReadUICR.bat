@echo off
:: ==============================================================================
:: nRF52840 UICR Read Script - Auslesen der UICR-Register
:: ==============================================================================

:: EINSTELLUNGEN
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg
set OUTPUT_FILE=uicr_backup.bin

echo ==============================================================================
echo Lese UICR-Bereich (0x10001000 - 0x10001300) aus...
echo Ziel-Datei: %OUTPUT_FILE%
echo ==============================================================================
echo.

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "dump_image \"%OUTPUT_FILE:\=/%\ " 0x10001000 0x300" ^
  -c "reset run" ^
  -c "shutdown"

pause