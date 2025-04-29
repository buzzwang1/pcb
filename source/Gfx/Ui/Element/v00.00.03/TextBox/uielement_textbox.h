#ifndef __UI_ELEMENT_TEXTBOX_H__
#define __UI_ELEMENT_TEXTBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_base.h"
#include "rfont.h"
#include "cStringTools.h"
#include "cChr8.h"

#ifdef __cplusplus
  extern "C" {
#endif

class cUiElement_TextBox: public cUiElement
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
    uint32 mui32MaxCharacterCount;
    uint32 mui32TextCol;
    tenTextFitMode menTextFitMode;


    cUiElement_TextBox(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
                       uint32 lui32MaxCharacterCount,
                       cRFont *lpcRFont)
      : cUiElement_TextBox(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, 
                           cUiElement_TextBox::tenTextFitMode::nNoFit,
                           lcMsgBox,
                           lui32MaxCharacterCount,
                           lpcRFont)
      {}

    cUiElement_TextBox(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, 
                       tenTextFitMode lenTextFitMode,
                       cMsgBox *lcMsgBox,
                       uint32 lui32MaxCharacterCount,
                       cRFont *lpcRFont)
      : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChText  = new char[mui32MaxCharacterCount + 1];
      mpcRFont = lpcRFont;
      vSetText((const char*)"Text");
      mui32TextCol = 0xFFFFFF;
      menTextFitMode = lenTextFitMode;
    }

    ~cUiElement_TextBox()
    {
      delete []mChText;
    }

    void vSetText(const char *lChText)
    {
      cStrTools::szStrnCpy(mChText, lChText, mui32MaxCharacterCount);
    }

    virtual void vPaint(cScreen *lpcScreen)
    {
      if (isVisible())
      {
        tstGfxPos2D lstSize;
        cUiElement::vPaint(lpcScreen);

        mpcRFont->mui32Col = mui32TextCol;
        
        switch (menTextFitMode)
        {
          case nFitRev:      lstSize = mpcRFont->vFitText(2,      0, mChText, lpcScreen, True); break;
          case nFitSmartRev: lstSize = mpcRFont->vFitSmartText(2, 0, mChText, lpcScreen, True); break;
          case nNoFitRev:    lstSize = mpcRFont->i8PutStringXY(2, 0, mChText, lpcScreen, True); break;
          default:           lstSize.x = 0; lstSize.y = 0;                           break;
        }

        switch (menTextFitMode)
        {
          case nFitRev:      mpcRFont->vFitText(2,      lpcScreen->iGfxHeight() - lstSize.y, mChText, lpcScreen);break;
          case nFitSmartRev: mpcRFont->vFitSmartText(2, lpcScreen->iGfxHeight() - lstSize.y, mChText, lpcScreen);break;
          case nNoFitRev:    mpcRFont->i8PutStringXY(2, lpcScreen->iGfxHeight() - lstSize.y, mChText, lpcScreen);break;
          case nFit:         mpcRFont->vFitText(2,      2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, lpcScreen);break;
          case nFitSmart:    mpcRFont->vFitSmartText(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, lpcScreen);break;
          default:           mpcRFont->i8PutStringXY(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, lpcScreen);
        }
      }
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_TEXTBOX_H__
