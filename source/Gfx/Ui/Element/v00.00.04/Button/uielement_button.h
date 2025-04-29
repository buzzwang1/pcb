#ifndef __UI_ELEMENT_BUTTON_H__
#define __UI_ELEMENT_BUTTON_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "cStringTools.h"


class cUiElement_Button: public cUiElement
{
  public:

    cRFont*      mpcRFont;
    char*        mChText;
    GfxInt       miGfxTextLength;
    uint32       mui32MaxCharacterCount;
    uint32       mui32TextCol;
    uint8        isPressed;


    cUiElement_Button(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
      uint32 lui32MaxCharacterCount,
      cRFont* lpcRFont)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChText = new char[mui32MaxCharacterCount + 1];
      mpcRFont = lpcRFont;
      vSetText("BTN");
      mui32TextCol = 0xFFFFFF;

      isPressed = 0;

      mstStatus.DragAble = 0;
      mstStatus.Sizeable = 0;
      mstStatus.Container = 0;
      menType = cUiElement::tenType::nButton;
    }

    ~cUiElement_Button()
    {
      delete[]mChText;
    }

    void vSetText(const char* lChText)
    {
      tstGfxPos2D lstStrLen;
      cStrTools::szStrnCpy(mChText, lChText, mui32MaxCharacterCount);

      mpcRFont->mcText.From(mChText);
      mpcRFont->i8StringLen(&lstStrLen);
      miGfxTextLength = lstStrLen.x;
    }

    virtual void OnPaint() override
    {
      mpcRFont->mui32Col = mui32TextCol;
      if (isPressed)
      {
        mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2 + 1, 3 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, cUiElement::mcRefScreen);
      }
      else
      {
        mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, cUiElement::mcRefScreen);
      }
    }

    bool virtual bAdd(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight, cUiElement *lcEle) override
    {
      UNUSED(liGfxRefX);
      UNUSED(liGfxRefY);
      UNUSED(liGfxWidth);
      UNUSED(liGfxHeight);
      UNUSED(lcEle);
      return False; // False, d.h. es können keine Childelemente angelegt werden
    }

    virtual void OnPressStart() {cUiElement::vParentCallback(this, cUiElement::tenEvent::nBtnPressStart); } // Button wurde gedrückt
    virtual void OnPressEnd()   {cUiElement::vParentCallback(this, cUiElement::tenEvent::nBtnPressEnd); } // Button wurde losgelassen

    virtual void OnCursorPressStart(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY) override
    {
      UNUSED(li16CurX);
      UNUSED(li16CurY);
      UNUSED(li16OldX);
      UNUSED(li16OldY);
      if (mstStatus.CursorIn)
      {
        isPressed = True;
        OnPressStart();
      }
    }

    virtual void OnCursorPressEnd(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY, u8 lu8Duration) override
    {
      UNUSED(li16CurX);
      UNUSED(li16CurY);
      UNUSED(li16OldX);
      UNUSED(li16OldY);
      UNUSED(lu8Duration);
      if (isPressed)
      {
        isPressed = False;
        if (mstStatus.CursorIn)
        {
          OnPressEnd();
        }
      }
    }
};



#endif  //__UI_ELEMENT_BUTTON_H__
