#ifndef __UI_ELEMENT_WINDOW_H__
#define __UI_ELEMENT_WINDOW_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_button.h"
#include "rfont.h"
#include "cStringTools.h"


class cUiElement_Window_CmdMax : public cUiElement_Button
{
  public:

  cUiElement_Window_CmdMax(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
    uint32 lui32MaxCharacterCount,
    cRFont* lpcRFont)
    : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount,  lpcRFont)
  {
  }

  virtual void OnPressEnd()
  {
    cUiElement::vParentCallback(this, cUiElement::tenEvent::nWndSizeToggleMaximize); 
  }
};

class cUiElement_Window_CmdClose : public cUiElement_Button
{
  public:

  cUiElement_Window_CmdClose(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
    uint32 lui32MaxCharacterCount,
    cRFont* lpcRFont)
    : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount,  lpcRFont)
  {
  }

  virtual void OnPressEnd()
  {
    cMsg lcMsg;

    // Ein Nachricht zum Schließen schicken
    // Wird dann vom root gemacht

    lcMsg.mID    = cMsg::nGfxElement;
    lcMsg.mSubID = cUiElement::nWndClose;

    u32* lpu32Data = (u32*) lcMsg.mui8Data;
    lpu32Data[0] = (u32)mpcParent;

    mcMsgBox->vput(&lcMsg);
  }
};



class cUiElement_Window: public cUiElement
{
  public:

    cRFont *mpcRFont;
    char   *mChTitle;
    uint32 mui32MaxCharacterCount;
    uint32 mui32TextCol;
    GfxInt miGfxFontHeight;

    cUiElement                 *mcEleContainer;
    cUiElement_Window_CmdMax   *mcCmdMax;
    cUiElement_Window_CmdClose *mcCmdClose;

    cUiElement_Window(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                      uint32 lui32MaxCharacterCount,
                      cRFont *lpcRFont)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChTitle = new char[mui32MaxCharacterCount];
      mpcRFont = lpcRFont;
      miGfxFontHeight = mpcRFont->miGfxHeight + 3;

      vSetTitle("Title");
      mui32TextCol = 0xFFFFFF;
      
      mcEleContainer = (cUiElement*)                 new cUiElement(lcMsgBox, lpcScreen);
      mcCmdMax       = (cUiElement_Window_CmdMax*)   new cUiElement_Window_CmdMax(lcMsgBox, lpcScreen, 2, lpcRFont);
      mcCmdClose     = (cUiElement_Window_CmdClose*) new cUiElement_Window_CmdClose(lcMsgBox, lpcScreen, 2, lpcRFont);

      mstStatus.PaintBg = 0;
      
      mcEleContainer->mstStatus.DragAble   = 0;
      mcEleContainer->mstStatus.Sizeable   = 0;
      mcEleContainer->mstStatus.Maximized  = 1;
      mcEleContainer->mstStatus.PaintFrame = 0;
      mcEleContainer->mu32BaseCol = 0x0000FF00;

      mstStatus.PaintBg = 0;

      mcCmdMax->vSetText("O");
      mcCmdClose->vSetText("X");

      menType = cUiElement::tenType::nWindow;
    }

    ~cUiElement_Window()
    {
      delete mcEleContainer;
      delete mcCmdMax;
      delete mcCmdClose;
      delete []mChTitle;
    }
    
    virtual void OnLoaded() override
    {
      cUiElement::bAdd(1, miGfxFontHeight + 2, miGfxWidth - 2, miGfxHeight - (miGfxFontHeight + 3), mcEleContainer);
      cUiElement::bAdd(miGfxWidth - 2 * (miGfxFontHeight + 3), 1, miGfxFontHeight, miGfxFontHeight, mcCmdMax);
      cUiElement::bAdd(miGfxWidth - 1 * (miGfxFontHeight + 3), 1, miGfxFontHeight, miGfxFontHeight, mcCmdClose);
    }

    void vSetTitle(const char *lChText)
    {
      cStrTools::szStrnCpy(mChTitle, lChText, mui32MaxCharacterCount);
    }


    virtual void vGetMaxArea(cScreenArea* lcPaintArea) override
    {
      lcPaintArea->miGfxRefPosx1 = mcPaintArea.miGfxRefPosx1 + 1;
      lcPaintArea->miGfxRefPosy1 = mcPaintArea.miGfxRefPosy1 + 1 + miGfxFontHeight + 1;
      
      lcPaintArea->miGfxClipPosx1 = 0;
      lcPaintArea->miGfxClipPosy1 = 0;

      lcPaintArea->miGfxClipPosx2 = miGfxWidth - 2;
      lcPaintArea->miGfxClipPosy2 = miGfxHeight - 1 - (miGfxFontHeight + 2);
    }

    bool virtual bAdd(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight, cUiElement *lcEle) override
    {
      return mcEleContainer->bAdd(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight, lcEle);
    }

    virtual void vToggleMaximize() override
    {
      cUiElement::vToggleMaximize();

      mcCmdMax->vSetPos(miGfxWidth - 2 * (miGfxFontHeight + 3), 1);
      mcCmdClose->vSetPos(miGfxWidth - 1 * (miGfxFontHeight + 3), 1);

      if (mstStatus.Maximized)
      {
        mcCmdMax->vSetText("o");
      }
      else
      {
        mcCmdMax->vSetText("O");
      }

      cGetRoot()->vToggleMaximize();
    }

    virtual void OnUpdateSize() override
    {
      if (mstStatus.SelectedResize)
      {
        mcCmdMax->mcPaintArea.miGfxRefPosx1 = mcPaintArea.miGfxRefPosx1 + miGfxWidth - 2 * (miGfxFontHeight + 3);
        mcCmdClose->mcPaintArea.miGfxRefPosx1 = mcPaintArea.miGfxRefPosx1 + miGfxWidth - 1 * (miGfxFontHeight + 3);
      }
    }

    virtual void vParentCallback(cUiElement *lpcChild, tenEvent lenEvent) override
    {
      if ((lenEvent == cUiElement::tenEvent::nWndSizeToggleMaximize) &&
          (lpcChild == this->mcCmdMax))
      {
        this->vToggleMaximize();
      }
      else
      {
        cUiElement::vParentCallback(lpcChild, lenEvent);
      }
    }


    virtual void OnPaint()
    {
      if (isVisible())
      {
        if (mstStatus.Selected)
        {
          cPaint::vRectFull(1,              1,
                            miGfxWidth - 2, miGfxFontHeight,
                            0x0000FF, cUiElement::mcRefScreen);
        }
        else
        {
          cPaint::vRectFull(1,              1,
                            miGfxWidth - 2, miGfxFontHeight,
                            0xA0A0A0, cUiElement::mcRefScreen);
        }

        cUiElement::mcRefScreen->vLineH(0, miGfxWidth - 2, miGfxFontHeight + 1, mu32FrameCol);

        mpcRFont->mui32Col = mui32TextCol;
        mpcRFont->i8PutStringXY(2, miGfxFontHeight - mpcRFont->miGfxYOffset, mChTitle, cUiElement::mcRefScreen);
      }
    }

};

#endif  //__UI_ELEMENT_WINDOW_H__
