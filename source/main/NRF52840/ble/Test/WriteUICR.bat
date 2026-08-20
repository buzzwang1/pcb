@echo off
:: ==============================================================================
:: nRF52840 UICR Write Script - Schreiben der UICR-Register
:: ==============================================================================

:: EINSTELLUNGEN
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg
set OUTPUT_FILE=uicr_backup.bin

:: Überprüfen, ob die Datei existiert
if not exist "%INPUT_FILE%" (
    echo [FEHLER] Die einzulesende Datei wurde nicht gefunden: "%INPUT_FILE%"
    echo.
    pause
    exit /b 1
)

echo ==============================================================================
echo ACHTUNG: Schreibe Daten in den UICR-Bereich (0x10001000)!
echo Quelldatei: %INPUT_FILE%
echo ==============================================================================
echo.
pause

"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "halt" ^
  -c "nrf5 uicr erase" ^
  -c "flash write_bank 1 %INPUT_FILE% 0x0" ^
  -c "reset run" ^
  -c "shutdown"

pause