#ifndef __BOTNET_STREAM_SYS_PORT_TEXT_H__
#define __BOTNET_STREAM_SYS_PORT_TEXT_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"

//Graphics
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "rfont.h"

//Resource
#include "ResBase.h"

#include "cBnStreamSysPortBase.h"

class cBotNetStreamPort_TextPort : public cBotNetStreamPort
{
public:
  typedef enum
  {
    nNoFit = 0,
    nFit,
    nFitSmart,
    nNoFitRev,
    nFitRev,
    nFitSmartRev,
  } tenTextFitMode ;

  cRFont *mpcRFont;
  char   *mChText;
  cStr    mszText;
  uint32 mui32MaxCharacterCount;
  uint32 mui32TextCol;
  tenTextFitMode menTextFitMode;

  cBotNetStreamPort_TextPort(tenTextFitMode lenTextFitMode,
                             uint32 lui32MaxCharacterCount,
                             cRFont *lpcRFont)
  : mszText(mChText = new char[lui32MaxCharacterCount + 1], 0, lui32MaxCharacterCount)
  {
    mszName = (char*)"BnTextPort";

    mui32MaxCharacterCount = lui32MaxCharacterCount;
    mpcRFont = lpcRFont;
    mszText = "";
    mui32TextCol = 0xFFFFFF;
    menTextFitMode = lenTextFitMode;
  }

  ~cBotNetStreamPort_TextPort()
  {
    delete []mChText;
  }


  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort) + sizeof(cBotNetStreamPort_TextPort);
  }

  void vProcess(void)
  {
    // Tx data to HW port
    u16 lu16Cnt = mcRxRingBuf.cnt();
    while (lu16Cnt)
    {
      vAddSign(mcRxRingBuf.get());
      lu16Cnt--;
    }
  }

  void vAddSign(const char8 lChr)
  {
    // Wenn Textbuffer voll, dann ...
    if (mszText.Len() >= mszText.Size())
    {
      // ... versuchen, eine komplette eine Zeile zu löschen
      u16 lu16t;

      lu16t = 0;
      while ((lu16t < mszText.Len()) && (mszText[lu16t] != '\n'))
      {
        lu16t++;
      }

      if (mszText[lu16t] == '\n')
      {
        lu16t++;
      }

      if (lu16t > 0)
      {
        mszText.Cut(0, lu16t);
      }
    }
    mszText += lChr;
  }


  void vAddText(const char8* lszText)
  {
    while (*lszText != 0)
    {
      vAddSign(*lszText);
      lszText++;
    }
  }

  void vPaint(cScreen *lpcScreen)
  {
    if (lpcScreen != null)
    {
      tstGfxPos2D lstSize;

      lpcScreen->vFill(0);

      mpcRFont->mui32Col = mui32TextCol;

      switch (menTextFitMode)
      {
        case nFitRev:      lstSize = mpcRFont->vFitText(2,      0, &mszText, lpcScreen, True); break;
        case nFitSmartRev: lstSize = mpcRFont->vFitSmartText(2, 0, &mszText, lpcScreen, True); break;
        case nNoFitRev:    lstSize = mpcRFont->i8PutStringXY(2, 0, &mszText, lpcScreen, True); break;
        default:           lstSize.x = 0; lstSize.y = 0;                           break;
      }

      switch (menTextFitMode)
      {
        case nFitRev:      mpcRFont->vFitText(2,      lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nFitSmartRev: mpcRFont->vFitSmartText(2, lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nNoFitRev:    mpcRFont->i8PutStringXY(2, lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nFit:         mpcRFont->vFitText(2,      2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);break;
        case nFitSmart:    mpcRFont->vFitSmartText(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);break;
        default:           mpcRFont->i8PutStringXY(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);
      }
    }
  }

  bool bProcessCmd(cStr &lcStrCmd, cStr &lcStrParam)
  {
    (void)lcStrCmd; /* avoid warning */
    (void)lcStrParam; /* avoid warning */
    return False;
  }

  virtual void vPrintCmds()
  {};
};


#endif // __BOTNET_STREAM_SYS_PORT_Text_H__
