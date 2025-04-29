#ifndef __UI_ELEMENT_WINDOW_H__
#define __UI_ELEMENT_WINDOW_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_button.h"
#include "rfont.h"
#include "cStringTools.h"
#include "uieletree.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cUiElement_Window_CmdMax : public cUiElement_Button
{
  public:
  cUiElement* mcParent;

  cUiElement_Window_CmdMax(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
    uint32 lui32MaxCharacterCount,
    cRFont* lpcRFont)
    : cUiElement_Button(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox,  lui32MaxCharacterCount,  lpcRFont)
  {
    mcParent = null;
  }

  virtual void vPressEnd()
  {
    cMsg lcMsg;

    lcMsg.mID    = cMsg::nGfxElement;
    lcMsg.mSubID = cUiElement::nWndSizeMax;

    uint16 *lpi16Data = (uint16 *)lcMsg.mui8Data;

    if (mcParent != null)
    {
      lpi16Data[0] = mcParent->mGfxTreeIdx;
    }
    else
    {
      lpi16Data[0] = 0xFFFF;
    }
    
    mcMsgBox->vput(&lcMsg);
  }
};

class cUiElement_Window_CmdClose : public cUiElement_Button
{
  public:

  cUiElement* mcParent;

  cUiElement_Window_CmdClose(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
    uint32 lui32MaxCharacterCount,
    cRFont* lpcRFont)
    : cUiElement_Button(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox,  lui32MaxCharacterCount,  lpcRFont)
  {
    mcParent = null;
  }

  virtual void vPressEnd()
  {
    cMsg lcMsg;

    lcMsg.mID    = cMsg::nGfxElement;
    lcMsg.mSubID = cUiElement::nWndClose;

    uint16 *lpi16Data = (uint16 *)lcMsg.mui8Data;
    if (mcParent != null)
    {
      lpi16Data[0] = mcParent->mGfxTreeIdx;
    }
    else
    {
      lpi16Data[0] = 0xFFFF;
    }


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

    cUiElement_Window(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
                      uint32 lui32MaxCharacterCount,
                      cRFont *lpcRFont)
      : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox)
    {
      mui32MaxCharacterCount = lui32MaxCharacterCount;
      mChTitle = new char[mui32MaxCharacterCount];
      mpcRFont = lpcRFont;
      miGfxFontHeight = mpcRFont->miGfxHeight + 3;

      vSetTitle("Title");
      mui32TextCol = 0xFFFFFF;
      
      mcEleContainer = (cUiElement*)                 new cUiElement(1, miGfxFontHeight + 2, miGfxWidth - 2, miGfxHeight - (miGfxFontHeight + 3), lcMsgBox);
      mcCmdMax       = (cUiElement_Window_CmdMax*)   new cUiElement_Window_CmdMax(miGfxWidth - 2 * (miGfxFontHeight + 3), 1, miGfxFontHeight, miGfxFontHeight, lcMsgBox, 2, lpcRFont);
      mcCmdClose     = (cUiElement_Window_CmdClose*) new cUiElement_Window_CmdClose(miGfxWidth - 1 * (miGfxFontHeight + 3), 1, miGfxFontHeight, miGfxFontHeight, lcMsgBox, 2, lpcRFont);

      mstStatus.PaintBg = 0;
      
      mcEleContainer->mstStatus.DragAble   = 0;
      mcEleContainer->mstStatus.Maximized  = 1;
      mcEleContainer->mstStatus.PaintFrame = 0;

      mstStatus.PaintBg = 0;

      mcCmdMax->vSetText("O");
      mcCmdClose->vSetText("X");

      mcCmdMax->mcParent   = this;
      mcCmdClose->mcParent = this;
    }

    ~cUiElement_Window()
    {
      delete mcEleContainer;
      delete mcCmdMax;
      delete mcCmdClose;
      delete []mChTitle;
    }

    void vSetTitle(const char *lChText)
    {
      cStrTools::szStrnCpy(mChTitle, lChText, mui32MaxCharacterCount);
    }

    /* Called from from the EleTree, when was added to the Tree */
    virtual void vAddedToTree(void* lpcEleTree)
    {
      cUiEleTree* mcEleTree = (cUiEleTree*)lpcEleTree;

      mcEleTree->vAdd(this, mcCmdMax);
      mcEleTree->vAdd(this, mcCmdClose);
      
      mcEleTree->vAdd(this, mcEleContainer);
    }

    /* Called from from the EleTree, when was added new Element.
       Return the Container/Parent Element */
    virtual uint16 vGetContainerTreeIdx()
    {
      if (mcEleContainer->mGfxTreeIdx == 0xFFFF) return mGfxTreeIdx;
      return mcEleContainer->mGfxTreeIdx;
    }


    virtual void vGetMaxArea(cScreenArea* lcPaintArea)
    {
      lcPaintArea->miGfxRefPosx1 = 1;
      lcPaintArea->miGfxRefPosy1 = miGfxFontHeight + 2;
      
      lcPaintArea->miGfxClipPosx1 = 0;
      lcPaintArea->miGfxClipPosy1 = 0;

      lcPaintArea->miGfxClipPosx2 = miGfxWidth  - 3;
      lcPaintArea->miGfxClipPosy2 = miGfxHeight - 2 - (miGfxFontHeight + 2);
    }

    virtual void vToggleMaximize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vToggleMaximize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      mcCmdMax->miGfxRefX = miGfxWidth - 2 * (miGfxFontHeight + 3);
      mcCmdClose->miGfxRefX = miGfxWidth - 1 * (miGfxFontHeight + 3);

      if (mstStatus.Maximized)
      {
        mcCmdMax->vSetText("o");
      }
      else
      {
        mcCmdMax->vSetText("O");
      }
    }

    virtual void vUpdateSize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vUpdateSize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      mcCmdMax->miGfxRefX   = miGfxWidth - 2 * (miGfxFontHeight + 3);
      mcCmdClose->miGfxRefX = miGfxWidth - 1 * (miGfxFontHeight + 3);
    }

    /* Größe ändern */
    virtual void vResize(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      cUiElement::vResize(liGfxRefX, liGfxRefY);

      mcCmdMax->miGfxRefX   = miGfxWidth - 2 * (miGfxFontHeight + 3);
      mcCmdClose->miGfxRefX = miGfxWidth - 1 * (miGfxFontHeight + 3);
    }


    /*
    virtual void vMsgHdl(cMsg *lpcMsg)
    {
      cUiElement::vMsgHdl(lpcMsg);
 
      switch ((cMsg::tenMsgIDs)lpcMsg->mID)
      {
        case cMsg::tenMsgIDs::nCursor:
        {
          cCursor::tenMsgEvent lenCursorEvent;

          mcEleTree->vMsgHdl(lpcMsg, &mcPaintArea);

          lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

          switch (lenCursorEvent)
          {
            case cCursor::tenMsgEvent::nDrag:
              mcEleTree->mui8PosUptoDate = 0;
            break;


            default:
              break;
          }
        }
        break;
        case cMsg::tenMsgIDs::nGfxElement:
        {
          cCursor::tenMsgEvent lenCursorEvent;

          lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

          switch (lenCursorEvent)
          {
            case cUiElement::nWndSizeMax:
              mcEleTree->mui8PosUptoDate = 0;

              //Position der Buttons anpassen
              mcCmdMax->miGfxRefX   = miGfxWidth - 2 * (miGfxFontHeight + 1);
              mcCmdClose->miGfxRefX = miGfxWidth - 1 * (miGfxFontHeight + 1);
            break;

            default:
              break;
          }
        }
        break;
        default:
          break;
      }
    }*/


    virtual void vPaint(cScreen *lpcScreen)
    {
      if (isVisible())
      {
        cUiElement::vPaint(lpcScreen);

        if (mstStatus.Selected)
        {
          cPaint::vRectFull(1,              1,
                            miGfxWidth - 2, miGfxFontHeight,
                            0x0000FF, lpcScreen);
        }
        else
        {
          cPaint::vRectFull(1,              1,
                            miGfxWidth - 2, miGfxFontHeight,
                            0xA0A0A0, lpcScreen);
        }

        lpcScreen->vLineH(0, miGfxWidth - 2, miGfxFontHeight + 1, mu32FrameCol);

        mpcRFont->mui32Col = mui32TextCol;
        mpcRFont->i8PutStringXY(2, miGfxFontHeight - mpcRFont->miGfxYOffset, mChTitle, lpcScreen);
      }
    }

};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_WINDOW_H__
