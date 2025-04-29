

#ifndef _C_CLI_H
#define _C_CLI_H

#include "TYPEDEF.h"
#include "cRingBufT.h"
#include "cStrT.h"


#define CLI_KEY_MAXLINE     32

// VT100 => ESC = Start von Sonderzeichen

#define CLI_KEY_ESCAPE      0x1B     // VT100: 27; Esc [
#define CLI_KEY_ESCAPE2     0x1B5B   // VT100: 27 91; Esc [

#define CLI_KEY_BACKSPACE   0x7F     // VT100: 127
#define CLI_KEY_RETURN      0x0D     // VT100: 13 [10]


#define CLI_KEY_ARROW_UP    0x01B5B41 // VT100: 27 91 65; ESC [ A
#define CLI_KEY_ARROW_DOWN  0x01B5B42 // VT100: 27 91 66; ESC [ B
#define CLI_KEY_ARROW_LEFT  0x01B5B44 // VT100: 27 91 68; ESC [ D
#define CLI_KEY_ARROW_RIGHT 0x01B5B43 // VT100: 27 91 67; ESC [ C

#define CLI_PROMPT_INIT ": "

class cCli;
class cCliCmdList;

class cCliCmd
{
  public:

  cStr mcCmd;
  cStr mcCmdInfo;

  cCliCmd(const char* lszCmd, const char* lszInfo)
    : mcCmd(lszCmd),
      mcCmdInfo(lszInfo)
  {}

  bool isMe(cStr* lszStr)
  {
    uint16 lui16t;

    // Erstes Space suchen
    lui16t = lszStr->IndexOf(0, ' ');

    if (lui16t == (uint16)-1)
    {
      lui16t = lszStr->Len();
    }

    return ((cStrTools::i8TextnCmp((const char8*)mcCmd.mpu8Data, (const char8*)lszStr->mpu8Data, mcCmd.muiLen) == 0) &&
            (mcCmd.muiLen == lui16t));
  }

  virtual bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) = 0;
};


class cCliCmd_Version: public cCliCmd
{
  public:
    cCliCmd_Version();
    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};

class cCliCmd_Help : public cCliCmd
{
  u16 mu16Cnt;
  u16 mu16CmdCnt;
  u16 mu16State;
  cCliCmdList *mcCmdList;

  public:
    cCliCmd_Help();
    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};


class cCliCmdList
{
  public:
    cCliCmd       **mCmdList;
    u16             mCmdListCnt;
    cCliCmdList  *mCmdListNext;

    cCliCmdList(cCliCmd **lCmdList, u16 lu16CmdListCnt)
    {
      mCmdListCnt  = lu16CmdListCnt;
      mCmdList     =  lCmdList;
      mCmdListNext = null;
    }




    virtual bool bAddCmd(cCliCmd *mCmd) {UNUSED(mCmd); return False;}

    void vAddList(cCliCmdList *lcNextList)
    {
      mCmdListNext = lcNextList;
    }
};


class cCliCmdList_Dyn: public cCliCmdList
{
  public:
    u16 mCmdListSize;

    cCliCmdList_Dyn(u16 lu16CmdListSize)
    : cCliCmdList(null, 0)
    {
      mCmdListSize = lu16CmdListSize;
      mCmdListCnt  = 0;

      mCmdList    =  new cCliCmd*[lu16CmdListSize];

      for (u8 lu8t = 0; lu8t < mCmdListSize; lu8t++)
      {
        mCmdList[lu8t] = null;
      }
    }

    ~cCliCmdList_Dyn()
    {
      delete[] mCmdList;
    }

    bool bAddCmd(cCliCmd *mCmd) override
    {
      for (u8 lu8t = 0; lu8t < mCmdListSize; lu8t++)
      {
        if (mCmdList[lu8t] == null)
        {
          mCmdListCnt++;
          mCmdList[lu8t] = mCmd;
          return True;
        }
      }
      return False;
    }
};


class cCliPrinter
{
  public:
  cRingBufT<uint8, uint16> *mcStreamOut;

  char*   mszTexttoPrint;
  u16     mu16TextLen;
  bool    mbNewLine;

  cCliPrinter(cRingBufT<uint8, uint16> *lcStreamOut)
  {
    mcStreamOut = lcStreamOut;
    mszTexttoPrint = null;
    mu16TextLen    = 0;
  }


  inline bool isSpace(u16 lu16Value)
  {
    return (mcStreamOut->space_left() > lu16Value);
  }

  inline bool isReady()
  {
     return (mu16TextLen == 0);
  }

  inline bool bPrint(char8 li8Char)
  {
    if (!mcStreamOut->isFull())
    {
      mcStreamOut->put(li8Char);
      return True;
    }
    return False;
  }

  inline bool bPrint(const char8 *lszStr)
  {
    cStr lcStr(lszStr);
    return bPrint(lcStr);
  }



  inline bool bPrintLn(const char8 *lszStr)
  {
    cStr lcStr(lszStr);
    return bPrintLn(lcStr);
  }

  bool bNewLine()
  {
    if (mbNewLine)
    {
      if (isSpace(2))
      {
        mcStreamOut->put('\r');
        mcStreamOut->put('\n');
        mbNewLine = False;
        return True;
      }
      else
      {
        return False;
      }
    }
    return True;
  }

  inline bool bPrintLn(cStr &lcStr)
  {
    if (isSpace(lcStr.Len()) + 2)
    {
      mcStreamOut->put((u8*)lcStr.mpu8Data, lcStr.Len());
      mcStreamOut->put('\r');
      mcStreamOut->put('\n');
    }
    return True;
  }

  inline bool bPrint(cStr &lcStr)
  {
    if (isSpace(lcStr.Len()))
    {
      mcStreamOut->put((u8*)lcStr.mpu8Data, lcStr.Len());
    }
    return True;
  }


  bool bFeedQueue()
  {
    while (1)
    {
      if (isSpace(16))
      {
        if (mu16TextLen)
        {
          if (mu16TextLen < 16)
          {
            mcStreamOut->put((u8*)mszTexttoPrint, mu16TextLen);
            mszTexttoPrint = null;
            mu16TextLen    = 0;

            return bNewLine();
          }
          else
          {
            mcStreamOut->put((u8*)mszTexttoPrint, 16);
            mszTexttoPrint += 16;
            mu16TextLen    -= 16;
          }
        }
        else
        {
          return bNewLine();
        }
      }
      else
      {
        return False;
      }
    }
  }

  inline bool bPrintTextLn(cStr &lcStr)
  {
    mbNewLine = True;
    if (lcStr.Len())
    {
      mszTexttoPrint = (char*)lcStr.mpu8Data;
      mu16TextLen    = lcStr.Len();
      return bFeedQueue();
    }
    return bNewLine();
  }

  inline bool bPrintText(cStr &lcStr)
  {
    mbNewLine = False;
    if (lcStr.Len())
    {
      mszTexttoPrint = (char*)lcStr.mpu8Data;
      mu16TextLen    = lcStr.Len();
      return bFeedQueue();
    }
    return True;
  }

  bool bDoProcess()
  {
    if (!mcStreamOut->isEmpty()) return False;
    return bFeedQueue();
  }
};


class cCli
{
  public:

  bool      mboPrompt;

  cRingBufT<uint8, uint16> *mcStreamIn;
  cRingBufT<uint8, uint16> *mcStreamOut;

  cCliCmdList *mcCmdList;
  cCliCmd     *mCmdActive;

  char8 mBufDataIn[CLI_KEY_MAXLINE];
  cStr  mcStrIn;

  char8 mBufPrompt[10];
  cStr  mcStrPrompt;

  cCliPrinter    mcPrinter;

  void*          mpOwner;

  uint8 mui8CursorPos;

  cCli(cRingBufT<uint8, uint16> *lcStreamIn, cRingBufT<uint8, uint16> *lcStreamOut, cCliCmdList *lcCmdList, u8 lu8HistoryCnt, void* lpOwner = null)
        : mcStrIn(mBufDataIn, 0, CLI_KEY_MAXLINE), mcStrPrompt(mBufPrompt, 0, 10),
          mcPrinter(lcStreamOut)
  {
    UNUSED(lu8HistoryCnt);
    mcStreamIn  = lcStreamIn;
    mcStreamOut = lcStreamOut;
    mcStrIn     = "";

    mCmdActive    = null;
    mcCmdList     = lcCmdList;

    mcStrPrompt = CLI_PROMPT_INIT;
    mboPrompt   = True;

    mui8CursorPos = 0;

    mpOwner = lpOwner;
  }

  bool bAddCmd(cCliCmd *lcCmd)
  {
    return mcCmdList->bAddCmd(lcCmd);
  }

  bool bAddCmdList(cCliCmdList *lcCmdListToAdd)
  {
    cCliCmdList *lcCmdList;
    lcCmdList = mcCmdList;

    while (lcCmdList)
    {
      if (lcCmdList->mCmdListNext == null)
      {
        lcCmdList->mCmdListNext = lcCmdListToAdd;
        return True;
      }
      lcCmdList = lcCmdList->mCmdListNext;
    }
    return False;
  }

  void vCallCmd(cCliCmd *lcCmd, bool lbFirstCall)
  {
    uint16 lui16t;
    cStr lcStrParam;

    mcStrIn.Trim();

    // Parameter herausschneiden
    // Erstes Space suchen
    lui16t = mcStrIn.IndexOf(0, ' ');

    if (lui16t != (uint16)-1)
    {
      lcStrParam.muiSize  = mcStrIn.muiSize - lui16t;
      lcStrParam.muiLen   = mcStrIn.muiLen  - lui16t;
      lcStrParam.mpu8Data = mcStrIn.mpu8Data + lui16t;
      lcStrParam.Trim();
    }

    // Kommando Callback Funktion aufrufen
    if (lcCmd->bProzessCmd(lcStrParam, this, lbFirstCall, mpOwner))
    {
      mCmdActive = null;
      mboPrompt  = True;
      bPrintLn((const char8*)"");
    }
    else
    {
      mCmdActive = lcCmd;
    }
  }

  void vProcessCmd()
  {
    uint8          lui8t;
    cCliCmdList *lcCmdList;

    lcCmdList = mcCmdList;

    mcStrIn.Trim();
    if (mcStrIn.Len() > 0)
    {
      while (lcCmdList)
      {
        // Einfache Suche nach dem Kommando
        for (lui8t = 0; lui8t < lcCmdList->mCmdListCnt; lui8t++)
        {
          //Bekanntes Kommando ? Und steht ganz vorne (==0)
          if (lcCmdList->mCmdList[lui8t] != null)
          {
            if (lcCmdList->mCmdList[lui8t]->isMe(&mcStrIn))
            {
              vCallCmd(lcCmdList->mCmdList[lui8t], True);
              return;
            }
          }
        }

        lcCmdList = lcCmdList->mCmdListNext;
      }
    }

    bPrintLn((rsz)"???: ->'help'");
    mboPrompt = True;
  }

  inline bool isSpace(u16 lu16Value)        { return mcPrinter.isSpace(lu16Value);}
  inline bool bPrompt(void)                 { return mcPrinter.bPrint(mcStrPrompt);}
  inline bool bPrint(cStr &lcStr)           { return mcPrinter.bPrint(lcStr);}
  inline bool bPrintLn(cStr &lcStr)         { return mcPrinter.bPrintLn(lcStr);}
  inline bool bPrint(const char8 *lszStr)   { return mcPrinter.bPrint(lszStr);}
  inline bool bPrintLn(const char8 *lszStr) { return mcPrinter.bPrintLn(lszStr);}
  inline bool bPrint(char8 li8Char)         { return mcPrinter.bPrint(li8Char);}

  inline bool isKbHit(void)                 { return (mcStreamIn->cnt() > 0);}
  inline char8 chGetChr(void)               { return (char8)mcStreamIn->get();}

  bool bDoProcess(void)
  {

    if (!mcPrinter.bDoProcess()) return False;

    if (mCmdActive != null)
    {
      vCallCmd(mCmdActive, False);
    }

    if (mCmdActive == null)
    {
      if (mboPrompt)
      {
        mboPrompt = !bPrompt();
      }
      else
      {
        uint16 lui16StreamInCnt;
        static uint32 lui32Code = 0;
        static bool   lbEscMode = False;

        lui16StreamInCnt = mcStreamIn->cnt();

        while (lui16StreamInCnt)
        {
          lui32Code = chGetChr();
          lui16StreamInCnt--;

          if (!lbEscMode)
          {
            switch (lui32Code)
            {
              case CLI_KEY_RETURN: // return

                bPrintLn("");

                vProcessCmd();

                mui8CursorPos = 0;
                mcStrIn = "";
                break;

              default:
                if (mui8CursorPos < CLI_KEY_MAXLINE)
                {
                  uint8 lui8t;

                  mui8CursorPos++;
                  mcStrIn.Insert((uint8)(mui8CursorPos - 1), lui32Code & 0xFF);

                  for (lui8t = (mui8CursorPos - 1); lui8t < mcStrIn.Len(); lui8t++)
                  {
                    bPrint(mcStrIn[lui8t]);
                  }

                  for (lui8t = mui8CursorPos; lui8t < mcStrIn.Len(); lui8t++)
                  {
                    bPrint((rsz)"\b");
                  }
                }
                break;
            }
          }
        }
      }
    }
    return (!mcStreamOut->isEmpty());
  }
};

#endif /* _C_CLI_H */
