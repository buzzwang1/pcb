@echo off
:: ==============================================================================
:: nRF52840 nrf52_recover Script - Setzt die Hardware das APPROTECT-Flag zurück
:: ==============================================================================

:: EINSTELLUNGEN
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/jlink.cfg
set TARGET=target/nrf52.cfg

echo ==============================================================================
echo init, nrf52_recover
echo Setzt die Hardware das APPROTECT-Flag zurück
echo ==============================================================================
echo.


"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% -c "catch { init }; nrf52_recover; shutdown"

pause
