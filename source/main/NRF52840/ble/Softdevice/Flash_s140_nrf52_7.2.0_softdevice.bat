@echo off
:: ==============================================================================
:: nRF52840 Flash Script - Letzte 4kB beschreiben (0x0FF000)
:: ==============================================================================

:: EINSTELLUNGEN (Hier an deine Pfade anpassen!)
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.11.0-WCH\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/nrf52.cfg
set FILE_PATH=s140_nrf52_7.2.0_softdevice.hex

:: Überprüfen, ob die zu flashende Datei existiert
if not exist "%FILE_PATH%" (
    echo [FEHLER] Die angegebene Datei wurde nicht gefunden: "%FILE_PATH%"
    echo Bitte passe den Pfad in dieser .bat Datei an.
    echo.
    pause
    exit /b 1
)

echo ==============================================================================
echo Starte Flash-Vorgang fuer nRF52840...
echo Ziel-Adresse: 0x000000
echo Datei:        %FILE_PATH%
echo ==============================================================================
echo.

:: OpenOCD Aufruf
:: Hinweis: Die Backslashes im Dateipfad werden für OpenOCD zu Froward-Slashes ersetzt
"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "program %FILE_PATH% verify reset exit"