@echo off
:: ==============================================================================
:: nRF52840 Flash Erase Script - Nur 1MB Haupt-Flash loeschen (UICR bleibt erhalten)
:: ==============================================================================

:: EINSTELLUNGEN
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg

echo ==============================================================================
echo ACHTUNG: Der 1 MB Haupt-Flash des nRF52840 wird vollstaendig geloescht!
echo Hinweis: Die UICR-Register (0x10001000) bleiben DABEI ERHALTEN.
echo ==============================================================================
echo.
pause

echo Starte Loeschvorgang fuer Bank 0 (0x00000000 bis 0x00100000)...
echo.

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "flash erase_sector 0 0 last" ^
  -c "reset run" ^
  -c "shutdown"

pause