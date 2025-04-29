Test CPU:
5s CPU und dann 5s CPU aus (WFI, wait for interrupt, mode name sleep)

HSE, PLL, 64MHz: 3.3V direkt an Core
29,0mA  
11,0mA  (WFI)

Nach Datenblatt
 - Running from HSE crystal clock 8 MHz, code executing from Flash
   - Supply current in Run mode from VDD supply
     - 64 MHz 54.8 mA(Peripherals enabled)   25.4mA (Peripherals disabled)
   - Supply current in Sleep mode from VDD supply
     - 64 MHz 39.7 mA(Peripherals enabled)    6.3mA (Peripherals disabled)

Timer2 49,1µA/MHz * 64Mhz = 3,1mA


HSI, kein PLL, 8MHz: 3.3V direkt an Core
5.75mA  
2.1mA   (WFI)