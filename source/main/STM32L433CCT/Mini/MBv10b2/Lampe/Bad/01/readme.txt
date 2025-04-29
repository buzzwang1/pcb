Für MBv10b2 HW (mit EEPROM)

Lampensteuerung mit Satelliten
Nur Wakeup by Bewegungsmelder und RTC
Wakeup by Kl30 wird deaktiviert (Bautteil entfernt)

Aufbau sollte sein

MBv10b2 1.0.0.0
|
| I2C, ca. 1m Leitung => Pull up 1kOhm
| 400khz
|
|
+-- PSwitch 1.1.0.0
|    |
|    | I2C, ca. 15m Leitung => Pull up 1kOhm
|    | 50khz
|    |
|    |
|    |
|    +-- PSwitch SLed Addon 1.1.1.0
|    |
|    +-- PSwitch SLed Addon 1.1.2.0
|    |
|    \-- Power Board 1.1.5.0
|       - CH1 (5V):   Deckenleuchte RGB
|       - CH2 (12V):  nc
|       - CH3 (12V):  nc
|
|
\-- Power Board 1.2.0.0
   - CH1 (12V):  nc
   - CH2 (12V):  LED-Strip Weiss
   - CH3 (12V):  Relais für 220V 

