#ifndef __UI_ELEMENT_ROOT_H__
#define __UI_ELEMENT_ROOT_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"

class cUiElement_Root: public cUiElement
{
  public:
    uint32       mui32BgCol;
    cUiElement  *lcEleCursorIn;
    cUiElement  *lcEleSelected;
    cUiElement  *lcEleSelectedResize;

    GfxInt miGfxCursorX;
    GfxInt miGfxCursorY;

    struct tstRootStatus
    {
      u8 CheckHidden     : 1;
    };

    tstRootStatus mstRootStatus;

    cUiElement_Root(GfxInt liGfxRefX,  GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight, cMsgBox *lcMsgBox,
                    cScreen *lpcScreen, uint32 lui32BgCol)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      mui32BgCol = lui32BgCol;
      mstRootStatus.CheckHidden = 1;

      lcEleCursorIn = NULL;
      lcEleSelected = NULL;
      lcEleSelectedResize = NULL;

      miGfxCursorX = 0;
      miGfxCursorY = 0;

      miGfxWidth  = liGfxWidth;
      miGfxHeight = liGfxHeight;
      vSetPos(liGfxRefX, liGfxRefY);
      menType = cUiElement::tenType::nRoot;
    }

    bool IsAnyFullScreen()
    {
      if (mstStatus.Maximized)
      {
        return True;
      }
      else
      {
        if ((mcPaintArea.miGfxRefPosx1 <= 0) &&
            (mcPaintArea.miGfxRefPosy1 <= 0) &&
            (mcPaintArea.miGfxRefPosx1 + miGfxWidth  - 1 >= mcRefScreen->miGfxClipPosx2) &&
            (mcPaintArea.miGfxRefPosy1 + miGfxHeight - 1 >= mcRefScreen->miGfxClipPosy2)
          )
        {
          return True;
        }
      }
      return False;
    }


    void OnElementChanged() override
    {
      mstRootStatus.CheckHidden = 1;
    }


    void vDoCheckHidden(cUiElement* lcElement)
    {

      lcElement->vDoCheckHidden();

      // Rekursiv den Baum durchsuchen
      if (lcElement->mpcNext)
      {
        vDoCheckHidden(lcElement->mpcNext);
      }

      if (!lcElement->mstStatus.Hidden)
      {
        if (lcElement->mpcFirstChild)
        {
          vDoCheckHidden(lcElement->mpcFirstChild);
        }
      }
    }


    void vDoCheckHighlighted(GfxInt liGfxCursorX, GfxInt liGfxCursorY, cUiElement* lcElement)
    {
      // Rekursiv den Baum durchsuchen
      if ((lcElement->mcPaintArea.isInAbs(liGfxCursorX, liGfxCursorY)) &&
          (lcElement->isVisible()))
      {
        cUiElement* lcElementSave;
        lcElementSave = lcEleCursorIn = lcElement;

        if (lcElement->mpcNext)
        {
          vDoCheckHighlighted(liGfxCursorX, liGfxCursorY, lcElement->mpcNext);
        }

        if (lcEleCursorIn == lcElementSave)
        {
          if (lcElement->mpcFirstChild)
          {
            vDoCheckHighlighted(liGfxCursorX, liGfxCursorY, lcElement->mpcFirstChild);
          }
        }
      }
      else
      {
        if (lcElement->mpcNext)
        {
          vDoCheckHighlighted(liGfxCursorX, liGfxCursorY, lcElement->mpcNext);
        }
      }
    }

    void vToggleMaximize() override
    {
      vDoCursorMove(miGfxCursorX, miGfxCursorY, miGfxCursorX, miGfxCursorY);
    }

    void vDoCursorMove(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY)
    {
      UNUSED(li16OldX);
      UNUSED(li16OldY);

      cUiElement* lcEleCurInOld = lcEleCursorIn;

      lcEleCursorIn = NULL;
      vDoCheckHighlighted(li16CurX, li16CurY, this);

      if (lcEleCursorIn)  // Cursor ist mindestens im Root
      {
        if (lcEleCurInOld) // Wenn zuvor schnon was selektiert war
                           // dann die Elemente Un-Highlighten
                           // die nicht mehr Cursorin sind
        {
          if (lcEleCursorIn != lcEleCurInOld)
          {
            cUiElement* lcParent = lcEleCurInOld;
            while (lcParent)
            {
              if ((lcEleCursorIn->isChildOf(lcParent)) ||
                   (lcParent == this))
              {
                break;
              }
              else
              {
                lcParent->vDoHighlight(False);
              }
              lcParent = lcParent->mpcParent;
            }
          }
        }
      }
      else // Cursor ist im Hintergrund
      {
        if (lcEleCurInOld) // Wenn zuvor schnon was selektiert war
                           // dann alle Elemente Un-Highlighten
        {
          cUiElement* lcParent = lcEleCurInOld;
          while (lcParent)
          {
            if (lcParent->mstStatus.CursorIn)
            {
              lcParent->mstStatus.CursorIn = 0;
            }
            lcParent = lcParent->mpcParent;
          }
          if (lcEleCurInOld->mstStatus.Overlapped)
          {
            vSetRepaint();
          }
          else
          {
            lcEleCurInOld->vSetRepaint();
          }
        }
      }

      if (lcEleCursorIn)
      {
        lcEleCursorIn->vDoHighlight(True);
      }
    }

    void vDoCursorPressStart(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY)
    {
      UNUSED(li16CurX);
      UNUSED(li16CurY);
      UNUSED(li16OldX);
      UNUSED(li16OldY);

      cUiElement* lcElement = lcEleSelected;

      // Im Fall von Touchpad/TouchScreen, gibt es kein CursorMove
      // Von daher nochmals hier aufrufen.
      vDoCursorMove(li16CurX, li16CurY, li16OldX, li16OldY);

      if (lcEleCursorIn)
      {
        lcEleCursorIn->vBringToTop();
        mstRootStatus.CheckHidden = 1;
        lcEleSelected = lcEleCursorIn;
        lcEleSelected->mstStatus.Selected = 1;
        vSetRepaint();
        // Und alle Parents selektieren
        if (lcEleSelected->mpcParent)
        {
          cUiElement* lcParent = lcEleSelected->mpcParent;
          while (1)
          {
            lcParent->mstStatus.Selected = 1;
            if (lcParent->mpcParent == NULL)
            {
              break;
            }
            lcParent = lcParent->mpcParent;
          }
        }
      }
      else
      {
        lcEleSelected = NULL;
      }

      if (lcElement != lcEleSelected)
      {
        // Überprüfen, ob das neu gewählte Element im gleichen Baum-branch
        // zum bisherigen Element ist
        // Es müsste nur bis zum ersten gemeinsamen Parent deselektiert werden
        if ((lcEleSelected) && (lcElement))
        {

          // Überprüfen, ob das neu gewählte element child vom bisherigen ist
          if (lcEleSelected->isChildOf(lcElement))
          {
            // dann muss nicht deselektiert werden
            return;
          }

          //  oder anders herum
          if (lcElement->isChildOf(lcEleSelected))
          {
            // dann muss nur bis zum bisherigen deselektiert werden
            lcElement->mstStatus.Selected = 0;
            lcElement->vSetRepaint();
            cUiElement* lcParent = lcElement->mpcParent;
            while (lcParent)
            {
              if (lcParent == lcEleSelected)
              {
                return;
              }
              lcParent->mstStatus.Selected = 0;
              lcParent->vSetRepaint();

              lcParent = lcParent->mpcParent;
            }
          }

          //Überprüfen, ob es einen gemeinsamen Parent gibt
          //Spätestens der Root, müsste gemeinsamer Parent sein.
          cUiElement* lcParent = lcElement->mpcParent;
          while (lcParent)
          {
            if (lcEleSelected->isChildOf(lcParent))
            {
              return;
            }
            lcParent->mstStatus.Selected = 0;
            lcParent->vSetRepaint();
            lcParent = lcParent->mpcParent;
          }


        }

        if (lcElement)
        {
          lcElement->mstStatus.Selected = 0;
          vSetRepaint();
          // Und alle Parents deselektieren
          if (lcElement->mpcParent)
          {
            cUiElement* lcParent = lcElement->mpcParent;
            while (1)
            {
              lcParent->mstStatus.Selected = 0;
              if (lcParent->mpcParent == NULL)
              {
                return;
              }
              lcParent = lcParent->mpcParent;
            }
          }
        }

      }
      else
      {
        if (lcEleSelected)
        {
          lcEleSelected->vSetRepaint();
        }
      }
    }

    void vDoCursorPressEnd(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY, u8 lu8Duration)
    {
      UNUSED(li16CurX);
      UNUSED(li16CurY);
      UNUSED(li16OldX);
      UNUSED(li16OldY);
      UNUSED(lu8Duration);

      if (lcEleSelectedResize)
      {
        lcEleSelectedResize->mstStatus.SelectedDrag   = 0;
        lcEleSelectedResize->mstStatus.SelectedResize = 0;
      }

      if (lcEleSelected)
      {
        lcEleSelected->vSetRepaint();
      }
    }

    void vDoCursorDragStart(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY)
    {
      if (lcEleSelected)
      {
        i16 li16DivX;
        i16 li16DivY;

        li16DivX = li16CurX - li16OldX;
        li16DivY = li16CurY - li16OldY;

        lcEleSelectedResize = lcEleSelected;
        if (lcEleSelected->mstStatus.Maximized == 0)
        {
          if ((lcEleSelectedResize->isOnFrame(li16OldX, li16OldY)) &&
              (lcEleSelectedResize->mstStatus.Sizeable))
          {
            lcEleSelectedResize->mstStatus.SelectedResize = 1;
            lcEleSelectedResize->vResize(li16DivX, li16DivY);
          }
          else
          {
            if (lcEleSelectedResize->mstStatus.DragAble)
            {
              lcEleSelectedResize->mstStatus.SelectedDrag   = 1;
              lcEleSelectedResize->vAddPos(li16DivX, li16DivY);
              lcEleSelectedResize->vSetSpeed(li16DivX, li16DivY);
            }
          }
        }
        else
        {
          // Wenn Maximized, dann zum Parent wechseln
          if (lcEleSelectedResize->mpcParent)
          {
            cUiElement* lcParent = lcEleSelectedResize->mpcParent;
            while (1)
            {
              if (!lcParent->mstStatus.Maximized)
              {
                lcEleSelectedResize = lcParent;
                break;
              }

              if (lcParent->mpcParent == NULL)
              {
                lcEleSelectedResize = NULL;
                return;
              }
              lcParent = lcParent->mpcParent;
            }
          }

          if (lcEleSelectedResize->isOnFrame(li16OldX, li16OldY))
          {
            if (lcEleSelectedResize->mstStatus.Sizeable)
            {
              lcEleSelectedResize->mstStatus.SelectedResize = 1;
              lcEleSelectedResize->vResize(li16DivX, li16DivY);
            }
          }
        }
      }
    }


    void vDoCursorDrag(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY)
    {
      vDoCursorMove(li16CurX, li16CurY, li16OldX, li16OldY);
      if (lcEleSelectedResize)
      {
        i16 li16DivX;
        i16 li16DivY;

        li16DivX = li16CurX - li16OldX;
        li16DivY = li16CurY - li16OldY;

        if (lcEleSelectedResize->mstStatus.SelectedDrag)
        {
          lcEleSelectedResize->vAddPos(li16DivX, li16DivY);
          lcEleSelectedResize->vSetSpeed(li16DivX, li16DivY);
        }
        else
        if (lcEleSelectedResize->mstStatus.SelectedResize)
        {
          lcEleSelectedResize->vResize(li16DivX, li16DivY);
        }
      }
    }


    void vPaintBgScreen()
    {
      if (!IsAnyFullScreen())
      {
        GfxInt liGfxX1;
        GfxInt liGfxY1;
        GfxInt liGfxX2;
        GfxInt liGfxY2;

        // Um den Biggest herum zeichnen
        liGfxX1 = mcPaintArea.miGfxClipPosx1 - mcRefScreen->miGfxRefPosx1;
        liGfxY1 = mcPaintArea.miGfxClipPosy1 - mcRefScreen->miGfxRefPosy1;
        liGfxX2 = mcPaintArea.miGfxClipPosx2 - mcRefScreen->miGfxRefPosx1;
        liGfxY2 = mcPaintArea.miGfxClipPosy2 - mcRefScreen->miGfxRefPosy1;

        //Oben malen
        cPaint::vRectFull(0,           0,
                          mcRefScreen->iGfxWidth(),  liGfxY1,
                          mui32BgCol,  mcRefScreen);

        //Unten malen
        cPaint::vRectFull(0,           liGfxY2 + 1,
                          mcRefScreen->iGfxWidth(),  mcRefScreen->miGfxClipPosy2 - mcRefScreen->miGfxRefPosy1 - liGfxY2,
                          mui32BgCol, mcRefScreen);

        //Links malen
        cPaint::vRectFull(0,           liGfxY1,
                          liGfxX1,     liGfxY2 - liGfxY1 + 1,
                          mui32BgCol, mcRefScreen);

        //Rechts malen
        cPaint::vRectFull(liGfxX2,     liGfxY1,
                          mcRefScreen->miGfxClipPosx2 - mcRefScreen->miGfxRefPosx1 - liGfxX2 + 1, liGfxY2 - liGfxY1 + 1,
                          mui32BgCol, mcRefScreen);
      }
    }


    void vPaint() override
    {
      if (mstRootStatus.CheckHidden)
      {
        mstRootStatus.CheckHidden = 0;
        vDoCheckHidden(this);
      }

      if (isVisible())
      {
        if (mstStatus.Repaint)
        {
          vPaintBgScreen();
        }
        cUiElement::vPaint();
      }
    }


    void vClose(cUiElement* lcEle)
    {
      // Dieses Element ausschneiden
      if (lcEle->mpcParent)
      {
        //Das Element davor suchen und danach suchen
        cUiElement* lcBefore = NULL;
        cUiElement* lcAfter  = lcEle->mpcNext;

        cUiElement* lcSearch = lcEle->mpcParent->mpcFirstChild;

        // Das Element vor diesem suchen
        while (lcSearch != lcEle)
        {
          lcBefore = lcSearch;
          lcSearch = lcSearch->mpcNext;
        }

        //Ist es erstes Element ?
        if (lcBefore == NULL)
        {
          lcEle->mpcParent->mpcFirstChild = lcAfter;
        }
        else if (lcAfter == NULL) //Ist es letztes Element ?
        {
          lcBefore->mpcNext = NULL;
        }
        else // Es ist mitten drin
        {
          lcBefore->mpcNext = lcAfter;
        }

        lcEle->mpcParent = NULL;
        lcEle->vClose();

        vSetRepaint();
        mstRootStatus.CheckHidden = 1;
      }
    }



    virtual void vMsgHdl(cMsg *lpcMsg)
    {
      switch ((cMsg::tenMsgIDs)lpcMsg->mID)
      {
        case cMsg::tenMsgIDs::nCursor:
          {
            cCursor::tenMsgEvent lenCursorEvent;

            lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

            int16 li16CurX;
            int16 li16CurY;
            int16 li16OldX;
            int16 li16OldY;

            int16* lpi16Data = (int16*)lpcMsg->mui8Data;

            li16CurX = lpi16Data[0];
            li16CurY = lpi16Data[1];
            li16OldX = lpi16Data[2];
            li16OldY = lpi16Data[3];

            miGfxCursorX = li16CurX;
            miGfxCursorY = li16CurY;

            switch (lenCursorEvent)
            {
              case cCursor::tenMsgEvent::nMove:           vDoCursorMove(li16CurX, li16CurY, li16OldX, li16OldY);       break;
              case cCursor::tenMsgEvent::nPressStart:     vDoCursorPressStart(li16CurX, li16CurY, li16OldX, li16OldY); break;
              case cCursor::tenMsgEvent::nPressEnd:       vDoCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 0); break;
              case cCursor::tenMsgEvent::nPressEndShort:  vDoCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 1); break;
              case cCursor::tenMsgEvent::nPressEndLong:   vDoCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 2); break;

              case cCursor::tenMsgEvent::nPressDragStart: vDoCursorDragStart(li16CurX, li16CurY, li16OldX, li16OldY); break;
              case cCursor::tenMsgEvent::nDrag:           vDoCursorDrag(li16CurX, li16CurY, li16OldX, li16OldY); break;
              default: break;
            }
          }
          break;

          case cMsg::tenMsgIDs::nGfxElement:
            {
              cCursor::tenMsgEvent lenCursorEvent;
              cMsg lcMsg;

              lpcMsg->vClone(&lcMsg);

              lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

              switch (lenCursorEvent)
              {
                case cUiElement::tenEvent::nWndClose:
                  {
                    u32 lu32Data = *((u32*)lpcMsg->mui8Data);
                    vClose((cUiElement*)(lu32Data));
                  }
                  break;
                default:
                  break;
              }
            }
          break;

        case cMsg::tenMsgIDs::nTimestamp:
          {
            OnTimer(lpcMsg->mui8Data[7]);
            vDoSwipe(lpcMsg->mui8Data[7]);
          }
          break;

        default:
          break;
      }

      // An Children weiterleiten
      if (mpcFirstChild) mpcFirstChild->vMsgHdl(lpcMsg);

      // An Nachfolger weiterleiten
      if (mpcNext) mpcNext->vMsgHdl(lpcMsg);
    }
};


#endif  //__UI_ELEMENT_ROOT_H__
