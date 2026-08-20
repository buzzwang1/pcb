@echo off
:: ==============================================================================
:: nRF52840 Mass Erase Script - Kompletten Flash-Speicher loeschen
:: ==============================================================================

:: EINSTELLUNGEN (Hier an deine Pfade anpassen!)
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg

echo ==============================================================================
echo ACHTUNG: Der gesamte Flash-Speicher des nRF52840 wird vollstaendig geloescht!
echo ==============================================================================
echo.
pause

echo Starte Komplettloeschung (Mass Erase)...
echo.

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "nrf5 mass_erase" ^
  -c "reset run" ^
  -c "shutdown"

pause