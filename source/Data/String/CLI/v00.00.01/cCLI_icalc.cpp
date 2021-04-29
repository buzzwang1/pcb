#include "cCLI_iCalc.h"

#if 0


int8 CLI_iCalc_OpsInStr(char8 lc8Char, uint8 *lui8OpIdx);
int8 CLI_iCalc_Alu(DString8* lszZahl1, DString8* lszOP, DString8* lszZahl2, DString8* lszErgebnis);
int8 CLI_iCalc_GetStartofTerm(DString8 *lszTerm, uint8 lui8OpPos, uint8 *lui8Startpos);
int8 CLI_iCalc_GetEndofTerm(DString8* lszTerm, uint8 lui8OpPos, uint8* lui8EndPos);

#define CLI_ICALC_NoOps                       10
const char8 *CLI_iCalc_Ops[CLI_ICALC_NoOps]   = {"^","*","/","+","-","<<",">>","%","&","|"};

#define CLI_ICALC_NoFunc                      2
const char8 *CLI_iCalc_Funcs[CLI_ICALC_NoFunc] = {"hex","str"};



uint8 CLI_iCalc_SolveNoBrackets(DString8* lszSource, DString8* lszErgebnis)
{
 
  int8  li8Fehler;

  uint8 lui8Start;
  uint8 lui8IdxOp;
  uint8 lui8s;   // Start Term
  uint8 lui8z;   // Stopp Term
  uint8 lui8OpS; // Start Operator
  uint8 lui8OpE; // Stopp Operator
  
  // ALU
  DString8 lszZahl1;
  DString8 lszOP;
  DString8 lszZahl2;
  DString8 lszErg;
  DString8 lszTerm;
  DString8 lszTerm2;
  DString8 lszTerm3;
  
  
  li8Fehler = null;
  lszZahl1  = null;
  lszOP     = null;
  lszZahl2  = null;
  lszErg    = null;
  lszTerm   = null;
  lszTerm2  = null;
  lszTerm3  = null;


  if ( DStr8_Create(&lszZahl1, 0) +
       DStr8_Create(&lszOP,    0) +
       DStr8_Create(&lszZahl2, 0) +
       DStr8_Create(&lszErg,   0) +
       DStr8_Create(&lszTerm,  0) +
       DStr8_Create(&lszTerm2, 0) +
       DStr8_Create(&lszTerm3, 0))
  {
    // Fehler beim Speicher holen
    li8Fehler = 1;
  }

  if (!li8Fehler)
  {
    DStr8_Copy(&lszTerm, lszSource); 
    DStr8_Copy(lszErgebnis, lszSource); 
  }
  
  
  // Parameterüberwachung für lszTerm
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(&lszTerm) == 0) 
    {
      li8Fehler = 2;
    }
  }
  
  // Sicherheits-Prüfung
  if (!li8Fehler)
  {
    if ((DStr8_Instr(0, &lszTerm, "(") < DSTR8_FAIL) || 
        (DStr8_Instr(0, &lszTerm, ")") < DSTR8_FAIL) )
    {
      li8Fehler = 2;
      // lszFehlerStr = "Fehler bei CLI_iCalc_SolveNoBrackets: Ungültige Klammerung !"
    }
  }

  // Eigentlicher Funktionsrumpf für CLI_iCalc_SolveNoBrackets
  if (!li8Fehler)
  {
    uint8 lui8OpDone;
    uint8 lui8OpCurrent;

    lui8OpDone = 1;

    // Exponenten rechnen
    while ((!li8Fehler) && (lui8OpDone))
    {
      lui8OpDone = 0;

      // Start suchen (mit führenden Vorzeichen)
      lui8Start = 0;
      li8Fehler = CLI_iCalc_OpsInStr(DStr8_pi8Data(&lszTerm)[0], &lui8IdxOp);
      if ((!li8Fehler) && (lui8IdxOp < CLI_ICALC_NoOps))
      {
        li8Fehler = CLI_iCalc_GetEndofTerm(lszSource, 0, &lui8Start);
      }

      // Alle Operatoren durchgehen
      for (lui8OpCurrent = 0; (lui8OpCurrent < CLI_ICALC_NoOps); lui8OpCurrent++)
      {
        // Exponent enthalten ?
        lui8OpS = DStr8_Instr(lui8Start, &lszTerm, (char8*)CLI_iCalc_Ops[lui8OpCurrent]);

        // Abbruch-Bedingung
        if (lui8OpS != DSTR8_FAIL)
        {
          lui8OpDone = 1;
          lui8OpE = lui8OpS + (uint8)StrTools_uixStrLen(CLI_iCalc_Ops[lui8OpCurrent]);

          // Ergebnis ermitteln und Einsetzen
          if (!li8Fehler) li8Fehler = CLI_iCalc_GetStartofTerm(&lszTerm, (uint8)lui8OpS,       &lui8s);
          if (!li8Fehler) li8Fehler = CLI_iCalc_GetEndofTerm(&lszTerm,   (uint8)(lui8OpE - 1), &lui8z);
          
          if (!li8Fehler)
          {
            if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8s),   (uint8)(lui8OpS - lui8s),          &lszZahl1, &lszTerm);
            if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8OpS), (uint8)(lui8OpE - lui8OpS),        &lszOP,    &lszTerm);
            if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8OpE), (uint8)((lui8z  - (lui8OpE - 1))), &lszZahl2, &lszTerm);
            if (!li8Fehler) li8Fehler = CLI_iCalc_Alu(&lszZahl1, &lszOP, &lszZahl2, &lszErg);
          }

          if (!li8Fehler)
          {
            if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(0),         (uint8)(lui8s),                              &lszTerm2, &lszTerm);
            if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8z + 1), (uint8)(DStr8_ui8Len(&lszTerm) - lui8z + 1), &lszTerm3, &lszTerm);
            if (!li8Fehler) li8Fehler = DStr8_Copy(&lszTerm, &lszTerm2);
            if (!li8Fehler) li8Fehler = DStr8_Add(&lszTerm, DStr8_pi8Data(&lszErg));
            if (!li8Fehler) li8Fehler = DStr8_Add(&lszTerm, DStr8_pi8Data(&lszTerm3));
          }
          break; // for
        }
      }
    }
    DStr8_Copy(lszErgebnis, &lszTerm);
  }

  DStr8_Delete(&lszZahl1);
  DStr8_Delete(&lszOP);
  DStr8_Delete(&lszZahl2);
  DStr8_Delete(&lszErg);
  DStr8_Delete(&lszTerm);
  DStr8_Delete(&lszTerm2);
  DStr8_Delete(&lszTerm3);

  return li8Fehler;
}


int CLI_iCalc_HexStr2Int(DString8* lszHexValue, uint32 *ui32Result)
{
  uint8  lui8AktPos;
  int8   li8Fertig;
  char8  lc8Zeichen;
  uint8  lui8Counter;

  *ui32Result = 0;

  lui8Counter  = 0;
  lui8AktPos = DStr8_ui8Len(lszHexValue) - 1;
  li8Fertig  = 0;

  while (!li8Fertig)
  {
    lc8Zeichen = DStr8_pi8Data(lszHexValue)[lui8AktPos];
    if (((lc8Zeichen >= 'A') && (lc8Zeichen <= 'Z')) ||
        ((lc8Zeichen >= 'a') && (lc8Zeichen <= 'f')) ||
        ((lc8Zeichen >= '0') && (lc8Zeichen <= '9')))
    {
      lui8Counter++;
    }
    else
    {
      li8Fertig = 1;
    }
    if (lui8Counter >= 8) li8Fertig = 1;
    if (!li8Fertig) lui8AktPos--;
  }

  li8Fertig  = 0;

  for (lui8AktPos++; lui8AktPos < DStr8_ui8Len(lszHexValue); lui8AktPos++)
  {
    *ui32Result <<= 4;

    lc8Zeichen = DStr8_pi8Data(lszHexValue)[lui8AktPos];
    if ((lc8Zeichen >= 'A') && (lc8Zeichen <= 'Z'))  *ui32Result += lc8Zeichen - 'A' + 10;
    else
    if ((lc8Zeichen >= 'a') && (lc8Zeichen <= 'f'))  *ui32Result += lc8Zeichen - 'a' + 10;
    else
    if ((lc8Zeichen >= '0') && (lc8Zeichen <= '9'))  *ui32Result += lc8Zeichen - '0';
  }

  return 0;
}


int32 CLI_iCalc_Pow(int32 li32Zahl, int32 li32Expo)
{
  int32 li32Result;

  li32Result = 0;

  if(li32Expo == 0)
  {
    li32Result = 1;
  }
  else if(li32Expo > 0)
  {
    int32 li32t;

    li32Result = li32Zahl;

    for (li32t = 1; li32t < li32Expo; li32t++)
    {
      li32Result *= li32Zahl;
    }
  }

  return li32Result;
}

int8 CLI_iCalc_Func(DString8* lszFunc, DString8* lszParam, DString8* lszErgebnis)
{
  int8 li8Fehler;

  li8Fehler = 0;

  if (StrTools_i8StrCmp(DStr8_pi8Data(lszFunc), "hex") == 0)
  {
    int32 li32ZwErg;

    if (!li8Fehler) li32ZwErg = StrTools_atoi(DStr8_pi8Data(lszParam));

    if (!li8Fehler)
    {
      char8  lszZwErgStr[10];

      StrTools_i8Itoa(li32ZwErg, lszZwErgStr, 16);
      if (!li8Fehler) li8Fehler = DStr8_Set(lszErgebnis, "0x");
      if (!li8Fehler) li8Fehler = DStr8_Add(lszErgebnis, lszZwErgStr);
    }
  }
  else
  if (StrTools_i8StrCmp(DStr8_pi8Data(lszFunc), "str") == 0)
  {
    if (!li8Fehler) li8Fehler = DStr8_Copy(lszErgebnis, lszParam);
    if (!li8Fehler) li8Fehler = DStr8_Insert(0, lszErgebnis, "\"");
    if (!li8Fehler) li8Fehler = DStr8_Add(lszErgebnis, "\"");
  }

  return li8Fehler;
}

int8 CLI_iCalc_Alu(DString8* lszZahl1, DString8* lszOP, DString8* lszZahl2, DString8* lszErgebnis)
{
  int8  li8Fehler;
  int32 li32Zahl1;
  int32 li32Zahl2;
  
  // Berechnen
  li8Fehler = DStr8_Set(lszErgebnis, "0");
      
  // Parameterüberwachung für lszZahl1
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszZahl1) == 0) 
    {
      li8Fehler = 2;
    }
  }

  // Parameterüberwachung für OP
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszOP) == 0) 
    {
      li8Fehler = 2;
    }
  }

  // Parameterüberwachung für lszZahl2
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszZahl2) == 0) 
    {
      li8Fehler = 2;
    }
  }


  // Eigentlicher Funktionsrumpf für CLI_iCalc_Alu
  if (!li8Fehler)
  {
    // Ist Zahl ein String ?
    if ((DStr8_Instr(0, lszZahl1, "\"") == DSTR8_FAIL) || (DStr8_Instr(0, lszZahl2, "\"") == DSTR8_FAIL)) 
    {
      //wenn nicht das hier tun
      

      // Punkt - Komma Geschichte
      DStr8_ReplaceSign(lszZahl1, ',', '.');
      DStr8_ReplaceSign(lszZahl2, ',', '.');
      
      // Hex ?
      if (DStr8_Instr(0, lszZahl1, "0x") != DSTR8_FAIL)
      {
        CLI_iCalc_HexStr2Int(lszZahl1, (uint32*)(&li32Zahl1));
      }
      else
      {
        li32Zahl1 = StrTools_atoi(DStr8_pi8Data(lszZahl1));
      }

      if (DStr8_Instr(0, lszZahl2, "0x") != DSTR8_FAIL)
      {
        CLI_iCalc_HexStr2Int(lszZahl2, (uint32*)(&li32Zahl2));
      }
      else
      {
        li32Zahl2 = StrTools_atoi(DStr8_pi8Data(lszZahl2));
      } 
    }
    
    if (!li8Fehler)
    {
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "+") == 0) 
      {
        
        // Stringaddidition ?
        if ((DStr8_Instr(0, lszZahl1, "\"") != DSTR8_FAIL) && (DStr8_Instr(0, lszZahl2, "\"") != DSTR8_FAIL)) 
        {
          li8Fehler = DStr8_Mid(0, (uint8)(DStr8_ui8Len(lszZahl1)-1), lszErgebnis, lszZahl1); 
          if (!li8Fehler) li8Fehler = DStr8_Add(lszErgebnis,  &(DStr8_pi8Data(lszZahl2)[1]));
        }
        else
        {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 + li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));          
          }
        }

      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "-") == 0) 
      {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 - li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "*") == 0) 
      {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 * li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "^") == 0) 
      {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);
          if (!li8Fehler)
          {
            StrTools_i8Itoa(CLI_iCalc_Pow(li32Zahl1, li32Zahl2), DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "/") == 0) 
      {
         
        if (li32Zahl2 != 0)
        {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 / li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
        }
        else
        {
          DStr8_Set(lszErgebnis,"0");
          li8Fehler = 1;
          //lszFehlerStr = "Fehler bei CLI_iCalc_Alu: Division durch 0"
        }            
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), ">>") == 0) 
      {
        if (li32Zahl2 > 0) 
        {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 >> li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
        }
        else
        {
          DStr8_Set(lszErgebnis, "0");
          li8Fehler = 1;
          //lszFehlerStr = "Fehler bei CLI_iCalc_Alu: Mod mit <=0"
        }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "<<") == 0) 
      {
        if (li32Zahl2 > 0) 
        {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 << li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
        }
        else
        {
          DStr8_Set(lszErgebnis, "0");
          li8Fehler = 1;
          //lszFehlerStr = "Fehler bei CLI_iCalc_Alu: Mod mit <=0"
        }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "&") == 0) 
      {
        li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
        if (!li8Fehler)
        {
          StrTools_i8Itoa(li32Zahl1 & li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
          DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
        }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "|") == 0) 
      {          
        li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
        if (!li8Fehler)
        {
          StrTools_i8Itoa(li32Zahl1 | li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
          DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
        }
      }
      else
      if (StrTools_i8StrCmp(DStr8_pi8Data(lszOP), "%") == 0) 
      {
         
        if (li32Zahl2 > 0) 
        {
          li8Fehler = DStr8_Reuse(lszErgebnis, 20);          
          if (!li8Fehler)
          {
            StrTools_i8Itoa(li32Zahl1 % li32Zahl2, DStr8_pi8Data(lszErgebnis), 10);
            DStr8_ui8Len(lszErgebnis) = (uint8)StrTools_uixStrLen(DStr8_pi8Data(lszErgebnis));
          }
        }
        else
        {
          DStr8_Set(lszErgebnis, "0");
          li8Fehler = 1;
          //lszFehlerStr = "Fehler bei CLI_iCalc_Alu: Mod mit <=0"
        }
      }
    }
  }

  return li8Fehler;
}


int8 CLI_iCalc_OpsInStr(char8 lc8Char, uint8 *lui8OpIdx)
{
  for (*lui8OpIdx = 0; *lui8OpIdx < CLI_ICALC_NoOps; (*lui8OpIdx)++)
  {
    if (CLI_iCalc_Ops[*lui8OpIdx][0] == lc8Char) return 0;
  }

  return 0;
}



int8 CLI_iCalc_GetStartofTerm(DString8 *lszTerm, uint8 lui8OpPos, uint8 *lui8Startpos)
{
  int8   li8Fehler;
  uint8  lui8AktPos;
  uint8  lui8IdxOp;
  char8  lc8Zeichen;

  *lui8Startpos = 0;
  li8Fehler     = 0;

    
  // Parameterüberwachung für lszTerm
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszTerm) == 0) 
    {
      li8Fehler = 2;
    }
  }

  // Parameterüberwachung für lui8OpPos
  if (!li8Fehler)
  {
    if (lui8OpPos >= DStr8_ui8Len(lszTerm))
    {
      li8Fehler = 2;
    }
  }

  // Eigentlicher Funktionsrumpf für CLI_iCalc_GetStartofTerm
  if (!li8Fehler)
  {
    lui8AktPos = lui8OpPos;
    
    switch (DStr8_pi8Data(lszTerm)[lui8AktPos])
    {
      case 34: // == '"' ?
        while (lui8AktPos > 0)
        {
          lui8AktPos--;
          lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];
          if (lc8Zeichen == 34) break; /* while */
        }
        break;
        
      default:
        while (lui8AktPos > 0)
        {
          lui8AktPos--;

          lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];
          //* Sonderfall '-' bei z.B. -5 + 5 oder 5+ -2

          CLI_iCalc_OpsInStr(lc8Zeichen, &lui8IdxOp);

          if (lui8IdxOp < CLI_ICALC_NoOps)
          {
            // Weitersuchen bis kein Operator mehr => wegen Vorzeichen
            // Überprüfen, ob vorhergehendes Zeichen auch Operator oder Stringanfang
            while (lui8AktPos > 0)
            {
              lui8AktPos--;
              lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];
              CLI_iCalc_OpsInStr(lc8Zeichen, &lui8IdxOp);
              if (lui8IdxOp == CLI_ICALC_NoOps)
              {
                lui8AktPos += 2;
                break; /* while */
              }
            }
            break; /* while */
          }
        }
        break;
    }
    *lui8Startpos = lui8AktPos;
  }
  return 0;
}


int8 CLI_iCalc_GetEndofTerm(DString8* lszTerm, uint8 lui8OpPos, uint8* lui8EndPos)
{
  int8   li8Fehler;
  uint8  lui8IdxOp;
  uint8  lui8AktPos;
  uint8  lui8MaxPos;
  char8  lc8Zeichen;

  *lui8EndPos = 0;
  li8Fehler   = 0;


  // Parameterüberwachung für lszTerm
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszTerm) == 0) 
    {
      li8Fehler = 2;
    }
  }

  // Parameterüberwachung für lui8OpPos
  if (!li8Fehler)
  {
    if (lui8OpPos >= DStr8_ui8Len(lszTerm))
    {
      li8Fehler = 2;
    }
  }

  lui8MaxPos = DStr8_ui8Len(lszTerm) - 1;


  // Eigentlicher Funktionsrumpf für CLI_iCalc_GetStartofTerm
  if (!li8Fehler)
  {
    lui8AktPos = lui8OpPos;

    switch (DStr8_pi8Data(lszTerm)[lui8AktPos])
    {
      case 34:
        while (lui8AktPos < lui8MaxPos)
        {
          lui8AktPos++;
          lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];
          if (lc8Zeichen == 34) break;
        }
        break;

      default:
        while (lui8AktPos < lui8MaxPos)
        {
          lui8AktPos++;

          lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];

          CLI_iCalc_OpsInStr(lc8Zeichen, &lui8IdxOp);

          if (lui8IdxOp == CLI_ICALC_NoOps)
          {
            // Weitersuchen bis wieder Operator => wegen Vorzeichen
            while (lui8AktPos < lui8MaxPos)
            {
              lui8AktPos++;
              lc8Zeichen = DStr8_pi8Data(lszTerm)[lui8AktPos];
              CLI_iCalc_OpsInStr(lc8Zeichen, &lui8IdxOp);
              if (lui8IdxOp < CLI_ICALC_NoOps)
              {
                lui8AktPos--;
                break; /* while */
              }
            }
            break; /* while */
          }
        }
        break;
    }
    *lui8EndPos = lui8AktPos;
  }

  return 0;
}


int8 CLI_iCalc_FindEndofTerm(DString8* lszTerm, uint8 *lui8Ergebnis)
  {

  
  uint8  li8Kl_tiefe;   // Verschachtelungstiefe der Klammern
  uint8 lui8AktPos;     // Aktuelle Parser Position im String
  int8 li8Fertig;       // äußere Schleife
  int8 lc8Chr;
  int8 lui8zA;           //  Zustand Automat
                        //  0: Init\Default
                        //  1: String
                        //  2: Klammer
  int8 li8Fehler;

  lui8zA = 0;
  li8Kl_tiefe = 0;
  lui8AktPos   = 0;
  li8Fertig    = 0;
    
  li8Fehler     = 0;
  *lui8Ergebnis = 0;
    
  // Parameterüberwachung für lszTerm
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszTerm) == 0)
    {
      li8Fehler = 2;
    }
  }

  // Eigentlicher Funktionsrumpf für FI_Calculate
  if (!li8Fehler)
  {
    while (!li8Fertig)
    {    
      lc8Chr = DStr8_pi8Data(lszTerm)[lui8AktPos];
    
      switch (lui8zA)
      {
        
        case 0: //Normal
          switch (lc8Chr)
          {
            case 34: //String anfang
              lui8zA = 1;
              break;
              
            case '(':
              li8Kl_tiefe++;
              lui8zA = 2;
              break;
              
            case ')':
              // lszFehlerStr = "Fehler bei CLI_iCalc_FindEndofTerm: Fehler in Klammerung"
              li8Fehler = 1;;
              break;
              
            case ',':
              li8Fertig = 1;
              break;
          }
          break;
                
        case 1: // String
          switch (lc8Chr)
          {
            case 34: // String Ende
              lui8zA = 0;
            break;
          }          
          break;          

        case 2: // In Klammern
         
          switch (lc8Chr)
          {
            case 34: // String anfang
              // lszFehlerStr = "Fehler bei CLI_iCalc_FindEndofTerm: String in Klammern nicht erlaubt"
              li8Fehler = 1;;
              break;              
            case '(':
              li8Kl_tiefe++;
              break;
                                          
            case ')':
              li8Kl_tiefe--;
              if (li8Kl_tiefe == 0)
              {
                lui8zA = 0;
              }
            break;
          }
          break;                          
      }
      
      if ((lui8AktPos >= DStr8_ui8Len(lszTerm)) || (li8Fehler) || (li8Fertig))
      {
        li8Fertig = 1;
      }
      else
      {
        lui8AktPos++;
      }
    }
                
    if (li8Fehler)
    {
      if (li8Kl_tiefe != 0)
      {
         //lszFehlerStr = "Fehler bei CLI_iCalc_FindEndofTerm: Fehler in Klammerung"
         li8Fehler = 1;
      }
    }
    
    if (!li8Fehler)
    {
      *lui8Ergebnis = lui8AktPos;
    }        
  }

  return li8Fehler;
}



int8 CLI_iCalc_DoProcess(DString8* lszTerm, DString8* lszErgebnis)
{
 
  
  uint8 lui8Kl_auf_Pos;  // Position der Klammer auf
  uint8 lui8Kl_zu_Pos;   // Position der Klammer zu
  uint8 li8Kl_tiefe;     // Verschachtelungstiefe der Klammern
  uint8 lui8AktPos;      // Aktuelle Parser Position im String
  int8  li8Fertig;       // äußere Schleife
  uint8 lui8ZwErg;
  uint8 lui8Restart;     // Nach Klammer-Rechnung wieder von Vorn beginnen
  char8 lc8Chr;
  uint8 lui8zA;          // Zustand Automat
                         // 0: Init\Default
                         // 1: String
  uint8    lui8Befehl;
  DString8 lszBefehl;
  DString8 lszBefehlParam;

  DString8 lszErg;
  DString8 lszTerm2;
  DString8 lszTerm3;
  int8     li8Fehler;

  lui8zA = 0;
  li8Kl_tiefe = 0;
  lui8Kl_auf_Pos = 0;
  lui8Kl_zu_Pos = 0;
  lui8AktPos = 0;
  li8Fertig = 0;
  lui8Restart = 0;

  li8Fehler = 0;

  lszBefehl       = null;
  lszBefehlParam  = null;
  lszErg          = null;
  lszTerm2        = null;
  lszTerm3        = null;

  if (DStr8_Create(&lszBefehl, 4) +
      DStr8_Create(&lszBefehlParam, 4) +
      DStr8_Create(&lszErg, 0) +
      DStr8_Create(&lszTerm2, 0) +
      DStr8_Create(&lszTerm3, 0)) li8Fehler = 1;


  //Nur beim 1.Aufruf machen
  if (!li8Fehler)
  {
    li8Fehler = CLI_iCalc_FindEndofTerm(lszTerm, &lui8ZwErg);
    if ((!li8Fehler) && (lui8ZwErg <= DStr8_ui8Len(lszTerm)))
    {
      if (lui8ZwErg > 0)
      {
        li8Fehler = DStr8_Copy(&lszTerm2, lszTerm);
        if (!li8Fehler) DStr8_Mid(0, (uint8)(lui8ZwErg), lszTerm, &lszTerm2);
      }
    }
  }

  // Parameterüberwachung für lszTerm
  if (!li8Fehler)
  {
    if (DStr8_ui8Len(lszTerm) == 0)
    {
      li8Fehler = 2;
    }
  }

  // Eigentlicher Funktionsrumpf für FI_Calculate
  if (!li8Fehler)
  {
    while (!li8Fertig)
    {

      lc8Chr = DStr8_pi8Data(lszTerm)[lui8AktPos];

      switch (lui8zA)
      {
        case 0:
          switch (lc8Chr)
          {
            case 34: //String anfang
              if (li8Kl_tiefe == 0)
              {
                lui8zA = 1;
              }
              else
              {
                // lszFehlerStr = "Fehler bei Calculate: String in Klammern nicht erlaubt"
                li8Fehler = 1;
              }
              break;

            case '(':
              lui8Kl_auf_Pos = lui8AktPos;
              li8Kl_tiefe++;
              break;
              
            case ')':
              if (li8Kl_tiefe > 0)
              {
                lui8Kl_zu_Pos = lui8AktPos;
                li8Kl_tiefe--;
                
                if (lui8Kl_auf_Pos + 1 < lui8Kl_zu_Pos) 
                {
                  uint8 lui8BefehlIdx;
                  uint8 lui8BefehlLen;
                
                  // Auf Befehl überprüfen
                  DStr8_Set(&lszBefehl, "");
                  lui8Befehl = 0xFF;
                  DStr8_Set(&lszErg, "0");

                  for (lui8BefehlIdx = 0; lui8BefehlIdx < CLI_ICALC_NoFunc; lui8BefehlIdx++)
                  {
                    lui8BefehlLen = (uint8)StrTools_uixStrLen(CLI_iCalc_Funcs[lui8BefehlIdx]);

                    // Überprüfen, ob Befehl überhaupt davor passt
                    if (lui8Kl_auf_Pos >= lui8BefehlLen)
                    {
                      if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8Kl_auf_Pos-lui8BefehlLen), lui8BefehlLen, &lszBefehl, lszTerm);

                      if (!li8Fehler)
                      {
                        StrTools_szStrLwr(DStr8_pi8Data(&lszBefehl));

                        if (StrTools_i8StrCmp(DStr8_pi8Data(&lszBefehl), CLI_iCalc_Funcs[lui8BefehlIdx]) == 0)
                        {
                          // Befehl gefunden Index merken
                          lui8Befehl = lui8BefehlIdx;
                          break; /* for */
                        }
                      }
                    }
                  }

                  li8Fehler = DStr8_Mid((uint8)(lui8Kl_auf_Pos + 1), (uint8)(lui8Kl_zu_Pos - lui8Kl_auf_Pos - 1), &lszTerm2, lszTerm);
                  if (!li8Fehler) li8Fehler = CLI_iCalc_SolveNoBrackets(&lszTerm2, &lszErg);

                  // Ggf Function noch aufrufen
                  if (lui8Befehl != 0xFF)
                  {
                    if (!li8Fehler) li8Fehler = DStr8_Copy(&lszBefehlParam, &lszErg);
                    if (!li8Fehler) li8Fehler = CLI_iCalc_Func(&lszBefehl, &lszBefehlParam, &lszErg);
                    lui8Kl_auf_Pos = lui8Kl_auf_Pos -lui8BefehlLen;
                  }

                  // String (Vor der Klammer) + Ergebnis + String (Nach der Klammer)
                  if (!li8Fehler) li8Fehler = DStr8_Mid(0,                          (uint8)(lui8Kl_auf_Pos),            &lszTerm2, lszTerm);
                  if (!li8Fehler) li8Fehler = DStr8_Mid((uint8)(lui8Kl_zu_Pos + 1), (uint8)(DStr8_ui8Len(lszTerm) - 1), &lszTerm3, lszTerm);
                  if (!li8Fehler) li8Fehler = DStr8_Copy(lszTerm, &lszTerm2);
                  if (!li8Fehler) li8Fehler = DStr8_Add(lszTerm, DStr8_pi8Data(&lszErg));
                  if (!li8Fehler) li8Fehler = DStr8_Add(lszTerm, DStr8_pi8Data(&lszTerm3));
                }
                else
                {
                  // lszFehlerStr = "Fehler bei Calculate: Leere Klammer"
                  li8Fehler = 1;
                }

                // Wieder von vorne anfangen
                lui8Restart = 1;
              }
              else
              {
                // lszFehlerStr = "Fehler bei Calculate: Klammerzu Fehler"
                li8Fehler = 1;
              }
              break;
          }
          break;

        case 1:
          switch (lc8Chr)
          {
            case 34: //String Ende
              lui8zA = 0;
              break;
          }
          break;
      }
      

      if (li8Fehler)
      {
        li8Fertig = 1;
      }

      if (lui8Restart)
      {
        lui8Restart = 0;
        lui8AktPos  = 0;  
        lui8Kl_auf_Pos = 0;
        lui8Kl_zu_Pos  = 0;
        li8Kl_tiefe    = 0;
      }
      else
      {
        if (lui8AktPos >= DStr8_ui8Len(lszTerm))
        {
          li8Fertig = 1;
        }
        else
        {
          lui8AktPos++;
        }
      }
    }

    if (!li8Fehler)
    {
      li8Fehler = CLI_iCalc_SolveNoBrackets(lszTerm, lszErgebnis);
    }
  }

  DStr8_Delete(&lszErg);
  DStr8_Delete(&lszTerm2);
  DStr8_Delete(&lszTerm3);
  DStr8_Delete(&lszBefehl);
  DStr8_Delete(&lszBefehlParam);

  return li8Fehler;
}

#endif