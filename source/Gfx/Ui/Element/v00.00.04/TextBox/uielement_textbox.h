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


    cUiElement_TextBox(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                       uint32 lui32MaxCharacterCount,
                       cRFont *lpcRFont)
      : cUiElement_TextBox(lcMsgBox, lpcScreen, 
                           cUiElement_TextBox::tenTextFitMode::nNoFit,
                           lui32MaxCharacterCount,
                           lpcRFont)
      {}

    cUiElement_TextBox(cMsgBox *lcMsgBox, cScreen *lpcScreen,  
                       tenTextFitMode lenTextFitMode,
                       uint32 lui32MaxCharacterCount,
                       cRFont *lpcRFont)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChText  = new char[mui32MaxCharacterCount + 1];
      mpcRFont = lpcRFont;
      vSetText((const char*)"Text");
      mui32TextCol = 0xFFFFFF;
      menTextFitMode = lenTextFitMode;

      menType = cUiElement::tenType::nTextBox;
    }

    ~cUiElement_TextBox()
    {
      delete []mChText;
    }

    void vSetText(const char *lChText)
    {
      cStrTools::szStrnCpy(mChText, lChText, mui32MaxCharacterCount);
      vRepaint();
    }

    virtual void OnPaint() override
    {
      tstGfxPos2D lstSize;

      mpcRFont->mui32Col = mui32TextCol;
      
      //Größe (Width, Height) des Text ermitteln
      switch (menTextFitMode)
      {
        case nFitRev:      lstSize = mpcRFont->vFitText(2,      0, miGfxWidth, mChText, cUiElement::mcRefScreen, True); break;
        case nFitSmartRev: lstSize = mpcRFont->vFitSmartText(2, 0, miGfxWidth, mChText, cUiElement::mcRefScreen, True); break;
        case nNoFitRev:    lstSize = mpcRFont->i8PutStringXY(2, 0,             mChText, cUiElement::mcRefScreen, True); break;
        default:           lstSize.x = 0; lstSize.y = 0; break;
      }

      switch (menTextFitMode)
      {
        case nFitRev:      mpcRFont->vFitText(2,      cUiElement::mcRefScreen->iGfxHeight() - lstSize.y, miGfxWidth, mChText, cUiElement::mcRefScreen);break;
        case nFitSmartRev: mpcRFont->vFitSmartText(2, cUiElement::mcRefScreen->iGfxHeight() - lstSize.y, miGfxWidth, mChText, cUiElement::mcRefScreen);break;
        case nNoFitRev:    mpcRFont->i8PutStringXY(2, cUiElement::mcRefScreen->iGfxHeight() - lstSize.y,             mChText, cUiElement::mcRefScreen);break;
        case nFit:         mpcRFont->vFitText(2,      2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, miGfxWidth, mChText, cUiElement::mcRefScreen);break;
        case nFitSmart:    mpcRFont->vFitSmartText(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, miGfxWidth, mChText, cUiElement::mcRefScreen);break;
        default:           mpcRFont->i8PutStringXY(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset,             mChText, cUiElement::mcRefScreen);
      }
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_TEXTBOX_H__
