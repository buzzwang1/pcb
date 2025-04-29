

#ifndef _C_PAL_H
#define _C_PAL_H

#include "TYPEDEF.h"
#include "cRingBufT.h"
#include "cStrT.h"
#include "cPalCmd.h"


#define PAL_KEY_MAXLINE    128
#define PAL_KEY_EOL         13
#define PAL_KEY_BREAK      255
#define PAL_KEY_STOP       253



class cPal
{
public:

  cPalCmd* mcCmds;

  char8 mBufDataIn[PAL_KEY_MAXLINE];
  cStr  mcStrIn;

  cRingBufT<uint8, uint16>* mcStreamIn;
  cRingBufT<uint8, uint16>* mcStreamOut;

  u8 mui8CursorPos;


  cPal(cPalCmd* lcPalCmd, cRingBufT<uint8, uint16>* lcStreamIn, cRingBufT<uint8, uint16>* lcStreamOut)
    : mcStrIn(mBufDataIn, PAL_KEY_MAXLINE)
  {
    mcCmds = lcPalCmd;

    mcStreamIn = lcStreamIn;
    mcStreamOut = lcStreamOut;
    mcStrIn = "";

    mui8CursorPos = 0;
  }



  void vProcessCmd()
  {
    // Der angeschlossene Steuerrechner sendet einen Befehl, der mit einem
    // Zeilenende - Zeichen[chr(13)] abgeschlossen ist.
    //
    // Die Prozessoreinheit quittiert die Ausführung bzw.Speicherung des Befehles
    // durch das Quittierungs - Signal ‘0‘[chr(48)] oder meldet einen aufgetretenen
    // Fehler mit einem ASCII - Zeichen ungleich ‘0‘(vgl.CNC - Betriebssystem 5.0
    // Kapitel Fehlermeldungen der Prozessorkarten)

    bool lbAnswer = False;

    if (mcStrIn.Len() > 0)
    {
      if (mcStrIn[0] == '@')
      {
        mcStrIn.Remove(' ');
        // Achsenanzahl setzen
        if      (mcStrIn.Instr(0, (rsz)"@01") == 0) {mcCmds->vAchsenanzahl_setzen(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@03") == 0) {mcCmds->vAchsenanzahl_setzen(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@05") == 0) {mcCmds->vAchsenanzahl_setzen(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@07") == 0) {mcCmds->vAchsenanzahl_setzen(&mcStrIn, False); lbAnswer = True;}

        // Referenzfahrt
        else if (mcStrIn.Instr(0, (rsz)"@0r") == 0) {mcCmds->vReferenzfahrt(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@0R") == 0) {mcCmds->vReferenzfahrt(&mcStrIn, True);}

        // Referenzgeschwindigkeit setzen
        else if (mcStrIn.Instr(0, (rsz)"@0d") == 0) {mcCmds->vReferenzgeschwindigkeit_setzen(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@0D") == 0) {mcCmds->vReferenzgeschwindigkeit_setzen(&mcStrIn, True);}

        // Bewegung relativ
        else if (mcStrIn.Instr(0, (rsz)"@0a") == 0) {mcCmds->vBewegung_relativ(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@0A") == 0) {mcCmds->vBewegung_relativ(&mcStrIn, True);}

        // Bewegung zur Position(move to)
        else if (mcStrIn.Instr(0, (rsz)"@0m") == 0) {mcCmds->vBewegung_zur_Position(&mcStrIn, False); lbAnswer = True;}
        else if (mcStrIn.Instr(0, (rsz)"@0M") == 0) {mcCmds->vBewegung_zur_Position(&mcStrIn, True);}
      }

      // stop
      else if ((u8)mcStrIn[0] == 253) {mcCmds->vStop(&mcStrIn, True);}
      // break
      else if ((u8)mcStrIn[0] == 255) {mcCmds->vBreak(&mcStrIn, False); lbAnswer = True;}
    }

    if (lbAnswer) mcStreamOut->put('0');
  }

  /*inline bool isSpace(u16 lu16Value) { return mcPrinter.isSpace(lu16Value); }
  inline bool bPrompt(void) { return mcPrinter.bPrint(mcStrPrompt); }
  inline bool bPrint(cStr& lcStr) { return mcPrinter.bPrint(lcStr); }
  inline bool bPrintLn(cStr& lcStr) { return mcPrinter.bPrintLn(lcStr); }
  inline bool bPrint(const char8* lszStr) { return mcPrinter.bPrint(lszStr); }
  inline bool bPrintLn(const char8* lszStr) { return mcPrinter.bPrintLn(lszStr); }
  inline bool bPrint(char8 li8Char) { return mcPrinter.bPrint(li8Char); }*/

  inline bool isKbHit(void) { return (mcStreamIn->cnt() > 0); }
  inline char8 chGetChr(void) { return (char8)mcStreamIn->get(); }

  bool bDoProcess(void)
  {
    u16 lu16StreamInCnt;
    u8  lu8Code = 0;

    lu16StreamInCnt = mcStreamIn->cnt();

    while (lu16StreamInCnt)
    {
      lu8Code = chGetChr();
      lu16StreamInCnt--;

      switch (lu8Code)
      {
        case PAL_KEY_EOL:
          vProcessCmd();

          mui8CursorPos = 0;
          mcStrIn = "";
          return (mcStreamIn->isEmpty());
          break;

        case PAL_KEY_BREAK:
        case PAL_KEY_STOP:
          if (mui8CursorPos < PAL_KEY_MAXLINE)
          {
            mui8CursorPos++;
            mcStrIn.Add(lu8Code);
          }

          vProcessCmd();

          mui8CursorPos = 0;
          mcStrIn = "";
          return (mcStreamIn->isEmpty());

          break;


        default:
          if (mui8CursorPos < PAL_KEY_MAXLINE)
          {
            mui8CursorPos++;
            mcStrIn.Add(lu8Code);
          }
          break;
      }
    }
    return (mcStreamIn->isEmpty());
  }
};

#endif /* _C_PAL_H */
