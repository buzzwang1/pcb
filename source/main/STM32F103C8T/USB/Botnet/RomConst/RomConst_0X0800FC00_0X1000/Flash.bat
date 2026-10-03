@echo off
:: ==============================================================================
:: STM32F103 Flash Script - Letzten 1kB beschreiben (0x0800FC00)
:: ==============================================================================

:: EINSTELLUNGEN (Hier an deine Pfade anpassen!)
set OPENOCD_PATH=C:\E2\Projekte\SW\Code\pcb_tools\openocd\0.12.0-0\bin\openocd.exe
set INTERFACE=interface/stlink.cfg
set TARGET=target/stm32f1x.cfg
set FILE_PATH=RomConst_0X0800FC00_0X1000.bin

:: Überprüfen, ob die zu flashende Datei existiert
if not exist "%FILE_PATH%" (
    echo [FEHLER] Die angegebene Datei wurde nicht gefunden: "%FILE_PATH%"
    echo Bitte passe den Pfad in dieser .bat Datei an.
    echo.
    pause
    exit /b 1
)

echo ==============================================================================
echo Starte Flash-Vorgang fuer STM32F103...
echo Ziel-Adresse: 0x0800FC00 (Letzten 1kB des 64KB Flash)
echo Datei:        %FILE_PATH%
echo ==============================================================================
echo.

:: OpenOCD Aufruf
:: Hinweis: Die Backslashes im Dateipfad werden für OpenOCD zu Forward-Slashes ersetzt
"%OPENOCD_PATH%" -f %INTERFACE% -f %TARGET% ^
  -c "init" ^
  -c "reset halt" ^
  -c "flash write_image erase %FILE_PATH% 0x0800FC00 bin" ^
  -c "verify_image %FILE_PATH% 0x0800FC00 bin" ^
  -c "reset run" ^
  -c "shutdown"