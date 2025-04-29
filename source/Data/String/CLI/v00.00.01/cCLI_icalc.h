
/* CLI_iCalc - Command Line Interpreter: Calculater */
/* Einfacher Taschenrechner
    - der iCalc macht nur Integer 32Bit
    - rekursiv !
    - Unterst�tzt 
         - Klammersetzung 
         - Stringaddition
         - Behehl
             - hex()
             - str()
         - Operatoren: '+','*','-','/','%','&','|','^','<<','>>'

 */


#ifndef _C_CLI_ICALC_H
#define _C_CLI_ICALC_H

  #include "TYPEDEF.h"
  #include "cStrT.h"

  /* Unterst�tze Operatoren der Priorit�t nach sortiert */
  //extern const char8 *CLI_iCalc_Ops[];
  /* Unterst�tze Funktionen. Immmer klein schreiben */
  //extern const char8 *CLI_iCalc_Funcs[];

  /* Aufruf den Taschenrechners */
  //extern int8 CLI_iCalc_DoProcess(DString8* lszTerm, DString8* lszErgebnis);

#endif
