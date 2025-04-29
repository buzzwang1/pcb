#ifndef __UI_ELEMENT_BUTTON_H__
#define __UI_ELEMENT_BUTTON_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "cStringTools.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cUiElement_Button: public cUiElement
{
  public:

    cRFont*      mpcRFont;
    char*        mChText;
    GfxInt       miGfxTextLength;
    uint32       mui32MaxCharacterCount;
    uint32       mui32TextCol;
    uint8        isPressed;

    cScreenArea  mcPaintAreaSave;


    cUiElement_Button(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
      uint32 lui32MaxCharacterCount,
      cRFont* lpcRFont)
      : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChText = new char[mui32MaxCharacterCount + 1];
      mpcRFont = lpcRFont;
      vSetText("BTN");
      mui32TextCol = 0xFFFFFF;

      isPressed = 0;

      mstStatus.DragAble = 0;
      mstStatus.Sizeable = 0;

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

    virtual void vPaint(cScreen* lpcScreen)
    {
      if (isVisible())
      {
        cUiElement::vPaint(lpcScreen);

        mpcRFont->mui32Col = mui32TextCol;
        if (isPressed)
        {
          mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2 + 1, 3 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, lpcScreen);
        }
        else
        {
          mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, lpcScreen);
        }
      }
    }

    virtual void vPressStart()
    {
    }

    virtual void vPressEnd()
    {
    }


    virtual void vMsgHdl(cMsg* lpcMsg)
    {
      switch ((cMsg::tenMsgIDs)lpcMsg->mID)
      {
      case cMsg::tenMsgIDs::nCursor:
      {
        cCursor::tenMsgEvent lenCursorEvent;

        lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

        switch (lenCursorEvent)
        {
          case cCursor::tenMsgEvent::nPressStart:
          {
            int16 li16CurX;
            int16 li16CurY;

            int16* lpi16Data = (int16*)lpcMsg->mui8Data;

            li16CurX = lpi16Data[0];
            li16CurY = lpi16Data[1];

            if (mcPaintArea.isInAbs((GfxInt)li16CurX, (GfxInt)li16CurY))
            {
              if (lenCursorEvent == cCursor::tenMsgEvent::nPressStart)
              {
                isPressed = 1;
                vPressStart();
              }
            }
          }
          break;
          case cCursor::tenMsgEvent::nPressEnd:
          case cCursor::tenMsgEvent::nPressEndShort:
          case cCursor::tenMsgEvent::nPressEndLong:
          {
            isPressed = 0; 
            vPressEnd();
          }
          break;


          default:
            break;
        }
      }
      break;
      default:
        break;
      }
    }

};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BUTTON_H__
