
#ifndef __UI_ELEMENT_TREE_H__
#define __UI_ELEMENT_TREE_H__

#include "typedef.h"
#include "cMemTreeIdx16.h"
#include "uielement_Base.h"
#include "Msg.h"

#ifdef __cplusplus
extern "C" {
#endif



class cUiEleTree : public cMemTreeIdx16
{
  public:

  uint16   mui16EleActivated;
  uint16   mui16EleSelected;
  uint8    mui8PosUptoDate;

  cUiEleTree(uint16 lui16ElementCount)
    : cMemTreeIdx16(lui16ElementCount)
  {
    mui16EleActivated = 0xFFFF;
    mui16EleSelected  = 0xFFFF;
    mui8PosUptoDate   = 0;
  }


  void vAdd(cUiElement* lcUiElelementParent, cUiElement* lcUiElement)
  {
    uint16 lui16Node;

    if (lcUiElelementParent == NULL)
    {
      //Root
      lcUiElement->mGfxTreeIdx = u16NodeAddChildBottom(0, (void*)lcUiElement);
      lcUiElement->vAddedToTree((void*) this);
    }
    else
    {
      lui16Node = lcUiElelementParent->vGetContainerTreeIdx();
      if (lui16Node != 0xFFFF)
      {
        lcUiElement->mpcParent   = ((cUiElement*)(mastTree[lui16Node].pData));
        lcUiElement->vUpdatePaintArea();
        lcUiElement->mGfxTreeIdx = u16NodeAddChildBottom(lui16Node, (void*)lcUiElement);
        lcUiElement->vAddedToTree((void*) this);
      }
    }
    mui8PosUptoDate = 0;
  }

  void vKill(uint16 lui16Node)
  {
    vTreeKillNode(lui16Node);

    mui8PosUptoDate = 0;
  }

  void vMoveRelXY(cUiElement *lcUiElement, GfxInt liGfxPosx, GfxInt liGfxPosy, GfxInt liGfxSpeedx, GfxInt liGfxSpeedy)
  {
    uint16 lui16Node;
    uint16 lui16NodeChild;
    cUiElement* lpcEle;

    lui16Node = lcUiElement->mGfxTreeIdx;
    lpcEle    = ((cUiElement*)(mastTree[lui16Node].pData));

    //Den Parent
    lpcEle->vAddPos(liGfxPosx, liGfxPosy);
    lpcEle->vSetSpeed(liGfxSpeedx, liGfxSpeedy);

    if (lpcEle->mpcParent != NULL)
    {
      lui16NodeChild = u16NodeGetFirstChild(lui16Node);
      while (lui16NodeChild != 0xFFFF)
      {
        ((cUiElement*)(mastTree[lui16NodeChild].pData))->vUpdatePaintArea();
        lui16NodeChild = u16TreeParseChild(lui16NodeChild, lui16Node);
      }
    }
    else
    {
      mui8PosUptoDate = 0;
    }
  }

  void vResize(cUiElement *lcUiElement, GfxInt liGfxDivX, GfxInt liGfxDivY)
  {
    uint16 lui16Node;
    cUiElement* lpcEle;

    lui16Node = lcUiElement->mGfxTreeIdx;
    lpcEle    = ((cUiElement*)(mastTree[lui16Node].pData));

    //Den Parent
    lpcEle->vResize(liGfxDivX, liGfxDivY);

    vUpdateMaximize(lui16Node);
  }

  void vUpdateSpeedAll(u16 lu16DiffTime)
  {
    uint16      lui16Node;
    cUiElement* lpcEle;

    lui16Node = u16NodeGetFirstChild(u16GetNodeRoot());
    while (lui16Node != 0xFFFF)
    {
      if (mastTree[lui16Node].pData != NULL)
      {
        i16 li16Dec = lu16DiffTime / 10;

        lpcEle  = ((cUiElement*)(mastTree[lui16Node].pData));

        if ((lpcEle->miGfxSpeedX) || (lpcEle->miGfxSpeedY))
        {
          vMoveRelXY(lpcEle,
                     lpcEle->miGfxSpeedX / li16Dec, lpcEle->miGfxSpeedY / li16Dec, 
                     (lpcEle->miGfxSpeedX * 7) / 8, (lpcEle->miGfxSpeedY * 7) / 8);
        }
      }

      lui16Node = u16TreeParse(lui16Node);
    }
  }

  void vDeactivateAll()
  {
    uint16 lui16Node;
    uint16 lui16NodeParent;

    if (mui16EleActivated != 0xFFFF)
    {
      lui16NodeParent = u16GetNodeRoot();

      //Alle Aktiven Fenster deaktivieren
      lui16Node = u16NodeGetFirstChild(lui16NodeParent);
      while (lui16Node != 0xFFFF)
      {
        //Falls Cursor nicht drin ist, dann alle Children überspringen
        if (((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.CursorIn == 1)
        {
          ((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.CursorIn = 0;
          lui16NodeParent = lui16Node;
          lui16Node = u16NodeGetFirstChild(lui16Node);
        }
        else
        {
          lui16Node = u16TreeParseChild(lui16Node, lui16NodeParent);
        }
      }
    }
  }

  void vActivate(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
  {
    uint16 lui16Node;
    uint16 lui16NodeParent;

    mui16EleActivated = 0xFFFF;

    //Aktives Fenster suchen
    lui16NodeParent = u16GetNodeRoot();
    lui16Node = u16NodeGetLastChild(lui16NodeParent);
    while (lui16Node != 0xFFFF)
    {
      ((cUiElement*)(mastTree[lui16Node].pData))->vUpdateCursorMove(liGfxCursorX, liGfxCursorY);

      //Falls Cursor nicht drin ist, dann alle Children überspringen
      if (((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.CursorIn == 0)
      {
        lui16Node = u16NodeParseBefore(lui16Node);
      }
      else
      {
        lui16NodeParent = lui16Node;
        mui16EleActivated = lui16Node;
        lui16Node = u16NodeGetLastChild(lui16NodeParent);
      }
    }
  }

  void vActivateAndToTop(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
  {
    uint16 lui16Node;
    uint16 lui16NodeParent;

    mui16EleActivated = 0xFFFF;

    //Aktives Fenster suchen
    lui16NodeParent = u16GetNodeRoot();
    lui16Node = u16NodeGetLastChild(lui16NodeParent);
    while (lui16Node != 0xFFFF)
    {
      ((cUiElement*)(mastTree[lui16Node].pData))->vUpdateCursorMove(liGfxCursorX, liGfxCursorY);

      //Falls Cursor nicht drin ist, dann alle Children überspringen
      if (((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.CursorIn == 0)
      {
        lui16Node = u16NodeParseBefore(lui16Node);
      }
      else
      {
        vTreeSetChildToBottom(lui16Node);
        lui16NodeParent = lui16Node;
        mui16EleActivated = lui16Node;
        lui16Node = u16NodeGetLastChild(lui16NodeParent);
      }
    }
  }

  void vDeselectAll()
  {
    uint16 lui16Node;
    uint16 lui16NodeParent;

    if (mui16EleSelected != 0xFFFF)
    {
      lui16NodeParent = u16GetNodeRoot();

      //Alle Aktiven Fenster deaktivieren
      lui16Node = u16NodeGetFirstChild(lui16NodeParent);
      while (lui16Node != 0xFFFF)
      {
        //Falls Cursor nicht drin ist, dann alle Children überspringen
        if (((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.Selected == 1)
        {
          ((cUiElement*)(mastTree[lui16Node].pData))->mstStatus.Selected = 0;
          lui16NodeParent = lui16Node;
          lui16Node = u16NodeGetFirstChild(lui16Node);
        }
        else
        {
          lui16Node = u16TreeParseChild(lui16Node, lui16NodeParent);
        }
      }
    }
    mui16EleSelected  = 0xFFFF;
  }


  // Ausgegehen von dem selektierten Element,
  // werden all Eltern-Elemente auch selektiert
  void vSelect(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
  {
    cUiElement* lpcEle;
 
    if (mui16EleActivated  != 0xFFFF)
    {
      //Aktives Fenster nehmen
      lpcEle  = ((cUiElement*)(mastTree[mui16EleActivated].pData));

      //Schauen ob der Cursor drin ist und selectieren
      lpcEle->vUpdateCursorPressStart(liGfxCursorX, liGfxCursorY);

      if (lpcEle->mstStatus.Selected)
      {
       mui16EleSelected = mui16EleActivated;

       //Alle Parents selektiern
       while (lpcEle->mpcParent != null)
       {
         lpcEle->mpcParent->mstStatus.Selected = 1;
         lpcEle = lpcEle->mpcParent;
       }

      }
    }
  }


  // Alle maximierte Element unter dem gegebenen Element
  // werden mit der Größe angepasst.
  void vUpdateMaximize(uint16 lui16NodeParent)
  {
    uint16 lui16Node;
    cUiElement* lpcEle;
    cUiElement* lpcEleParent;

    lpcEleParent = ((cUiElement*)(mastTree[lui16NodeParent].pData));
    lui16Node    = u16NodeGetFirstChild(lui16NodeParent);
    
    while (lui16Node != 0xFFFF)
    {
      lpcEle = ((cUiElement*)(mastTree[lui16Node].pData));
      
      if (lpcEle->mstStatus.Maximized == 1)
      {
        cScreenArea lpcParentArea;

        lpcEleParent->vGetMaxArea(&lpcParentArea);
        lpcEle->vUpdateSize(lpcParentArea.miGfxRefPosx1, lpcParentArea.miGfxRefPosy1, lpcParentArea.iGfxWidth(), lpcParentArea.iGfxHeight());
        lpcEleParent = lpcEle;
        lui16Node = u16NodeGetFirstChild(lui16Node);
      }
      else
      {
        lui16Node = u16NodeParse(lui16Node);
      }
    }
  }

  void vUpdateMaximize(cUiElement* lpcEle)
  {
    cUiElement* lpcEleParent;
    
    lpcEleParent = lpcEle->mpcParent;

    if (lpcEleParent != null)
    {
      vUpdateMaximize(lpcEleParent->mGfxTreeIdx);
    }
  }

  virtual void vMsgHdl(cMsg *lpcMsg, cScreenArea *lpcArea)
  {
    switch ((cMsg::tenMsgIDs)lpcMsg->mID)
    {
      case cMsg::tenMsgIDs::nCursor:
        {
          cCursor::tenMsgEvent lenCursorEvent;
          cMsg lcMsg;

          lpcMsg->vClone(&lcMsg);

          lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

          int16 li16CurX;
          int16 li16CurY;

          int16* lpi16Data = (int16*)lpcMsg->mui8Data;

          li16CurX = lpi16Data[0];
          li16CurY = lpi16Data[1];

          switch (lenCursorEvent)
          {
            case cCursor::tenMsgEvent::nMove:
              {
                vDeactivateAll();
                vActivate(li16CurX, li16CurY);
              }
              break;

            case cCursor::tenMsgEvent::nPressStart:
              {
                vDeactivateAll();
                vDeselectAll();
                vActivateAndToTop(li16CurX, li16CurY);
                vSelect(li16CurX, li16CurY);
              }
              break;

            case cCursor::tenMsgEvent::nPressEnd:
            case cCursor::tenMsgEvent::nPressEndShort:
            case cCursor::tenMsgEvent::nPressEndLong:
              {
                if (mui16EleSelected != 0xFFFF)
                {
                  cUiElement* lpcEle;
                  lpcEle = ((cUiElement*)(mastTree[mui16EleSelected].pData));
                  lpcEle->vUpdateCursorPressEnd();
                }
              }
             break;

            case cCursor::tenMsgEvent::nPressDragStart:
            case cCursor::tenMsgEvent::nDrag:
              {
                i16 li16DivX;
                i16 li16DivY;

                li16DivX = li16CurX - lpi16Data[2];
                li16DivY = li16CurY - lpi16Data[3];

                if (mui16EleActivated != 0xFFFF)
                {
                  if (((cUiElement*)(mastTree[mui16EleActivated].pData))->mstStatus.SelectedDrag)
                  {
                    cScreenArea lcArea;
                    
                    // Überprüfen, ob man noch im Parent-Bereich ist
                    if (((cUiElement*)(mastTree[mui16EleActivated].pData))->mpcParent != null)
                    {
                      ((cUiElement*)(mastTree[mui16EleActivated].pData))->mpcParent->vGetPaintArea(&lcArea);
                    }
                    else
                    {
                      lpcArea->vClone(&lcArea);
                    }

                    //Das ausgewählte Element verschieben
                    //und alle Kinder updaten
                    if (lcArea.isInAbs(li16CurX, li16CurY))
                    {
                      vMoveRelXY(((cUiElement*)(mastTree[mui16EleActivated].pData)), li16DivX, li16DivY, li16DivX, li16DivY);
                      mui8PosUptoDate = 0;
                    }
                  }
                  else
                  if (((cUiElement*)(mastTree[mui16EleActivated].pData))->mstStatus.SelectedResize)
                  {
                    cScreenArea lcArea;
                    
                    // Überprüfen, ob man noch im Parent-Bereich ist
                    if (((cUiElement*)(mastTree[mui16EleActivated].pData))->mpcParent != null)
                    {
                      ((cUiElement*)(mastTree[mui16EleActivated].pData))->mpcParent->vGetPaintArea(&lcArea);
                    }
                    else
                    {
                      lpcArea->vClone(&lcArea);
                    }

                    //Das ausgewählte Element vergrößern
                    //und alle Kinder updaten
                    if (lcArea.isInAbs(li16CurX, li16CurY))
                    {
                      vResize(((cUiElement*)(mastTree[mui16EleActivated].pData)), li16DivX, li16DivY);
                      mui8PosUptoDate = 0;
                    }
                  }
                }
              }
              break;
            default:
              break;
          }

          if (mui16EleActivated != 0xFFFF)
          {
            ((cUiElement*)(mastTree[mui16EleActivated].pData))->vMsgHdl(&lcMsg);
          }
        }
        break;
      case cMsg::tenMsgIDs::nGfxElement:
        {
          uint16 lui16Node;
          cCursor::tenMsgEvent lenCursorEvent;
          cMsg lcMsg;

          lpcMsg->vClone(&lcMsg);

          lui16Node = ((uint16*)(lpcMsg->mui8Data))[0];
          

          if (lui16Node != 0xFFFF)
          {
            lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

            switch (lenCursorEvent)
            {
              case cUiElement::tenMsgEvent::nWndSizeMax:
                {
                  cUiElement* lpcEle;
                  cUiElement* lpcEleParent;

                  lpcEle       = ((cUiElement*)(mastTree[lui16Node].pData));
                  lpcEleParent = ((cUiElement*)(mastTree[lui16Node].pData))->mpcParent;

                  mui8PosUptoDate = 0;

                  /* Zeichenbereich vom Parent übernehmen */
                  if (lpcEleParent != NULL)
                  {
                    cScreenArea lpcParentArea;

                    lpcEleParent->vGetMaxArea(&lpcParentArea);

                    lpcEle->vToggleMaximize(lpcParentArea.miGfxRefPosx1, lpcParentArea.miGfxRefPosy1, lpcParentArea.iGfxWidth(), lpcParentArea.iGfxHeight());
                  }
                  else
                  {
                    /* Element hat kein Parent */
                    lpcEle->vToggleMaximize(0, 0, lpcArea->iGfxWidth(), lpcArea->iGfxHeight());
                  }

                  vUpdateMaximize(lui16Node);
                }
                break;
              case cUiElement::tenMsgEvent::nWndClose:
                {
                  vKill(lui16Node);
                }
                break;
              default:
                break;
            }

            ((cUiElement*)(mastTree[lui16Node].pData))->vMsgHdl(&lcMsg);
          }
        }
        break;

      case cMsg::tenMsgIDs::nTimestamp:
        {
          cMsg lcMsg;

          lpcMsg->vClone(&lcMsg);

          switch ((cCursor::tenMsgEvent)lpcMsg->mSubID)
          {
            case 0: // Alle Elemente durchgehen und Geschwindigkeit anpassen
              vUpdateSpeedAll(lcMsg.mui8Data[7]);
            break;
            default:
            break;
          }
        }

      default:
        break;

    }
  }

  bool IsAnyFirstLevelFullScreen(cScreen* lpcScr)
  {
    uint16      lui16Node;
    cUiElement* lpcEle;

    lui16Node = u16NodeGetFirstChild(u16GetNodeRoot());
    while (lui16Node != 0xFFFF)
    {
      if (mastTree[lui16Node].pData != NULL)
      {
        lpcEle       = ((cUiElement*)(mastTree[lui16Node].pData));

        if (lpcEle->mstStatus.Maximized)
        {
          return True;
        }
        
        if ((lpcEle->miGfxRefX <= 0) &&
            (lpcEle->miGfxRefY <= 0) &&
            (lpcEle->miGfxRefX + lpcEle->miGfxWidth  - 1 >= lpcScr->miGfxClipPosx2) &&
            (lpcEle->miGfxRefY + lpcEle->miGfxHeight - 1 >= lpcScr->miGfxClipPosy2)
          )
        {
           return True;
        }
        
        lui16Node = u16NodeParseUnsafe(lui16Node);
      }
    }
    return False;
  }

  void vFill(u32 lu32Col, cScreen* lpcScr)
  {
    if (!IsAnyFirstLevelFullScreen(lpcScr))
    {
      lpcScr->vFill(lu32Col);
    }
  }

  void vPaint(cScreen* lpcScr)
  {
    uint16      lui16Node;
    uint16      lui16NodeNext;
    uint16      lui16NodeParent;
    cScreenArea cAreaSave(lpcScr);
    cUiElement* lpcEle;
    cUiElement* lpcEleNext;
    cUiElement* lpcEleParent;

    /* --- Zeichenbereiche aller Elemente updaten */
    if (!mui8PosUptoDate)
    {
      lui16Node = u16NodeGetFirstChild(u16GetNodeRoot());
      while (lui16Node != 0xFFFF)
      {
        if (mastTree[lui16Node].pData != NULL)
        {
          lpcEle       = ((cUiElement*)(mastTree[lui16Node].pData));
          lpcEleParent = ((cUiElement*)(mastTree[lui16Node].pData))->mpcParent;

          /* Zeichenbereich vom Parent übernehmen */
          if (lpcEleParent != NULL)
          {
            lpcEle->vUpdatePaintArea();
          }
          else
          {
            /* Element hat kein Parent */
            lpcEle->vUpdatePaintArea(lpcScr);
          }
        }
        lui16Node = u16TreeParse(lui16Node);
      }
    }

    mui8PosUptoDate = 1;

    /* --- Elemente zeichenen */

    /* Erstes Element holen
       Gezeichnet wird Top zu Bottom
       d.h. das letzte Element wird das oberste sein
    */
    lui16Node = u16NodeGetFirstChild(u16GetNodeRoot());
    while (lui16Node != 0xFFFF)
    {
      if (mastTree[lui16Node].pData != NULL)
      {
        lpcEle       = ((cUiElement*)(mastTree[lui16Node].pData));
        lpcEleParent = ((cUiElement*)(mastTree[lui16Node].pData))->mpcParent;

        lpcEle->mstStatus.Paint = 1;

        /* Absoluter Zeichenbereich vom Parent übernehmen */
        if (lpcEleParent != NULL)
        {
          lpcEle->mstStatus.Paint = lpcEleParent->mstStatus.Paint;
        }

        if (lpcEle->mstStatus.Paint)
        {
          /* Mit den nachfoldenden und darüberliegenden Elementen vergleichen,
             ob es überdeckt wird
          */
          lui16NodeParent = lui16Node;
          lui16NodeNext   = u16NodeParseUnsafe(lui16Node);

          do
          {
            while (lui16NodeNext != 0xFFFF)
            {
              lpcEleNext    = ((cUiElement*)(mastTree[lui16NodeNext].pData));
          
              if ((lpcEleNext->mcPaintArea.miGfxClipPosx1 <= lpcEle->mcPaintArea.miGfxClipPosx1) &&
                  (lpcEleNext->mcPaintArea.miGfxClipPosy1 <= lpcEle->mcPaintArea.miGfxClipPosy1) &&
                  (lpcEleNext->mcPaintArea.miGfxClipPosx2 >= lpcEle->mcPaintArea.miGfxClipPosx2) &&
                  (lpcEleNext->mcPaintArea.miGfxClipPosy2 >= lpcEle->mcPaintArea.miGfxClipPosy2))
              {
                lpcEle->mstStatus.Paint = 0;
                break;
              }
              else
              {
                lui16NodeNext = u16NodeParseUnsafe(lui16NodeNext);
              }
            }
            if (lpcEle->mstStatus.Paint)
            {
              lui16NodeParent = u16TreeGetParent(lui16NodeParent);
              lui16NodeNext   = u16NodeParseUnsafe(lui16NodeParent);
            }
            else
            {
              break;
            }
          } while (lui16NodeParent != u16GetNodeRoot());
        }

        if (lpcEle->mstStatus.Paint)
        {
          lpcScr->vSetArea(&cAreaSave);
          lpcEle->vPaint(lpcScr);
        }
      }
      lui16Node = u16TreeParse(lui16Node);
    }

    lpcScr->vSetArea(&cAreaSave);
  }

};

#ifdef __cplusplus
}
#endif

#endif //__UI_ELEMENT_TREE_H__
