#ifndef __UI_ELEMENT_BASE_H__
#define __UI_ELEMENT_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap.h"
#include "screen.h"
#include "paint.h"
#include "Msg.h"
#include "Cursor.h"


#define UI_INVALID        GFX_INVALID
#define UI_FAIL           GFX_FAIL
#define UI_OK             GFX_OK
#define UI_INLINE         GFX_INLINE


class cUiElement
{
  public:

    enum tenEvent
    {
      // Button
      nBtnPressStart,
      nBtnPressEnd,

      // Window
      nWndClose,
      nWndSizeToggleMaximize,

      // Switch
      nSwitchStateChanged
    };


    enum tenType
    {
      nBase,
      nBox3D,
      nBox3Df,
      nButton,
      nfGraphYBox,
      niGraphXYBox,
      niGraphYBox,
      nRoot,
      nVSplit,
      nHSplit,
      nTextBox,
      nWindow,
      nSwitch,
      nSwitchPressOnly,
      nSwitchGroup
    };


    // Die Elemente werden in einem Baum verkn¸pft/verbunden
    cUiElement  *mpcParent;
    cUiElement  *mpcFirstChild;
    cUiElement  *mpcNext;
    
    // Absolute Koordinaten des Zeichen-Bereichs
    //
    //    +(Refx/RefY)
    //              (x1/y1)------+
    //              |            |
    //              +------(x2/y2)
    cScreen*    mcRefScreen;
    cScreenArea mcPaintArea;

    // Grˆﬂe
    //       +----Width---+
    //       |            |
    //       |          Height
    //       |            |
    //       +------------+
    //
    GfxInt miGfxWidth;
    GfxInt miGfxHeight;

    // Zwischenspeicher um Originalgrˆﬂe f¸r Maximierung zu speichern
    GfxInt miGfxRefX_Save;
    GfxInt miGfxRefY_Save;
    GfxInt miGfxWidth_Save;
    GfxInt miGfxHeight_Save;

    // F¸rs Schubsen/Swipen
    GfxInt miGfxSpeedX;
    GfxInt miGfxSpeedY;

    struct tstStatus
    {
      /* User Einstellungen */
      u32 DragAble   : 1; // Element kann verschoben werden
      u32 PaintBg    : 1; // Hintergrund zeichnen
      u32 PaintFrame : 1; // Rahmen zeichnen 
      u32 Visible    : 1; // Zeichnen oder nicht
      u32 Enable     : 1; // TDB (Aktiviert oder nicht. Es wird gezeichnet, aber es kann nichts gemacht werden)
      u32 Sizeable   : 1; // Grˆﬂe ver‰nderbar
      u32 Container  : 1; // Kann Elemente aufnehmen

      // ird vom Element selbst gesetzt
      u32 Maximized      : 1; // Zeigt an, dass das Element maximiert ist
      u32 Repaint        : 1; // Inhalt/grˆﬂe/position hat sich und muss neu gezeichnet werden

      /* Vom Root-Element gesetzt */
      u32 Selected       : 1; // Es wurde ausgew‰hlt
      u32 SelectedDrag   : 1; // Es wurde ausgew‰hlt und Cursor ist in einem Bereich, um es zu verschieben   (z.B. Titelbereich bei Fenster)
      u32 SelectedResize : 1; // Es wurde ausgew‰hlt und Cursor ist in einem Bereich, um die Grˆﬂe zu ‰ndern
      u32 CursorIn       : 1; // Cursor ist in dem Element
      u32 Overlapped     : 1; // Zeigt an, dass das Element wird von einem Nachbarelement verdeckt ist. Wird vom Root gesetzt
      u32 Hidden         : 1; // Zeigt an, dass das Element wird von einem Nachbarelement komplett verdeckt ist. Wird vom Root gesetzt
                              // Muss nach einer Grˆﬂen‰nderungen, Verschiebung, Hinzugef¸gt oder Entfernt gemacht werden 
      u32 HiddenByChild  : 1; // Zeigt an, dass das Element wird von einem Child komplett verdeckt ist. vom Root gesetzt
                              // Muss nach einer Grˆﬂen‰nderungen, Verschiebung, Hinzugef¸gt oder Entfernt gemacht werden 
    };

    tstStatus mstStatus;

    uint32 mu32BaseCol;
    uint32 mu32FrameCol;


    tenType   menType;

    cMsgBox *mcMsgBox;

    cUiElement(cMsgBox *lcMsgBox, cScreen *lpcScreen)
    {
      mcMsgBox = lcMsgBox;
      mcRefScreen  = lpcScreen;
      vInit();
    }

    virtual ~cUiElement()
    {
    }

    void vInit()
    {
      mpcParent     = NULL;
      mpcFirstChild = NULL;
      mpcNext       = NULL;

      mstStatus.DragAble   = 1;
      mstStatus.PaintBg    = 1;
      mstStatus.PaintFrame = 1;
      mstStatus.Visible    = 1;
      mstStatus.Enable     = 1;
      mstStatus.Sizeable   = 1;
      mstStatus.Container  = 1;

      mstStatus.Selected       = 0;
      mstStatus.SelectedDrag   = 0;
      mstStatus.SelectedResize = 0;
      mstStatus.CursorIn       = 0;
      mstStatus.Hidden         = 0;
      mstStatus.HiddenByChild  = 0;
      mstStatus.Overlapped     = 0;
      mstStatus.Maximized      = 0;
      mstStatus.Repaint        = 1;

      menType      = cUiElement::tenType::nBase;
      mu32BaseCol  = 0;
      mu32FrameCol = 0x00FFFFFF;

      miGfxWidth  = 8;
      miGfxHeight = 8;
      vSetPos(0, 0);

      miGfxSpeedX = 0;
      miGfxSpeedY = 0;
    }


    // Touch oder Maus wird bewegt
    virtual void OnCursorMove(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY) { UNUSED(li16CurX); UNUSED(li16CurY); UNUSED(li16OldX); UNUSED(li16OldY); }
    // Touch oder Maus(Linksclick) wurde gedr¸ckt
    virtual void OnCursorPressStart(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY) { UNUSED(li16CurX); UNUSED(li16CurY); UNUSED(li16OldX); UNUSED(li16OldY); }
    // Touch oder Maus(Linksclick) wurde losgelassen
    virtual void OnCursorPressEnd(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY, u8 lu8Duration) { UNUSED(li16CurX); UNUSED(li16CurY); UNUSED(li16OldX); UNUSED(li16OldY); UNUSED(lu8Duration); }
    // Start Element wird verschoben
    virtual void OnCursorDragStart(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY) { UNUSED(li16CurX); UNUSED(li16CurY); UNUSED(li16OldX); UNUSED(li16OldY); }
    // Start Element wird verschoben
    virtual void OnCursorDrag(int16 li16CurX, int16 li16CurY, int16 li16OldX, int16 li16OldY) { UNUSED(li16CurX); UNUSED(li16CurY); UNUSED(li16OldX); UNUSED(li16OldY); }

    // Wird aufgerufen, wenn Grˆﬂe (z.B. nach Maximierung) oder Position sich ge‰ndert hat 
    virtual void OnUpdateSize() {}

    // Ist das Element sichtbar (auf dem Bildschirm / eingeschaltet / nicht ¸berdeckt)
    bool isVisible() { return ((mcPaintArea.mbVisible) && (mstStatus.Visible) && (mstStatus.Hidden == 0)); }

    // Wird durch das Timingevent aufgerufen
    virtual void OnTimer(u32 lu32Timediff_ms) { UNUSED(lu32Timediff_ms); }
    virtual void OnPaint() {}          // Wird aufgerufen, um den Inhalt des Element zu zeichnen
    virtual void OnLoaded() {}         // Wird aufgerufen, nachdem das Element dazugef¸gt wurde.
    virtual void OnClose() {}          // Wird aufgerufen, nachdem das Element entfernt wurde.
    virtual void OnElementChanged() {} // Wenn sich bei einem Element Grˆﬂe oder Lage ge‰ndert hat.
                                       // Damit der Root den Baum aktualisieren kann. Das sollte nur im Root genutzt werden

    /* Positions Relative zum Parent */
    void vSetSpeed(GfxInt liGfxSpeedX, GfxInt liGfxSpeedY)
    {
      miGfxSpeedX = liGfxSpeedX;
      miGfxSpeedY = liGfxSpeedY;
    }

    /* Positions Relative zum Parent */
    void vSetPos(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      mcPaintArea.miGfxRefPosx1 = mcRefScreen->miGfxRefPosx1 + liGfxRefX;
      mcPaintArea.miGfxRefPosy1 = mcRefScreen->miGfxRefPosy1 + liGfxRefY;

      if (mpcParent)
      {
        mcPaintArea.miGfxRefPosx1 += mpcParent->mcPaintArea.miGfxRefPosx1;
        mcPaintArea.miGfxRefPosy1 += mpcParent->mcPaintArea.miGfxRefPosy1;
      }
      vUpdatePaintAreaMove();
    }

    /* Positions Relative zum Parent */
    void vAddPos(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      mcPaintArea.miGfxRefPosx1 += liGfxRefX;
      mcPaintArea.miGfxRefPosy1 += liGfxRefY;

      vUpdatePaintAreaMove();

      // Alle Children auch verschieben
      cUiElement* lcChild = mpcFirstChild;
      while (lcChild) 
      {
        lcChild->vAddPos(liGfxRefX, liGfxRefY);
        lcChild = lcChild->mpcNext;
      }
    }

    /* Grˆﬂe ‰ndern */
    void vResize(GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      if (mstStatus.Maximized)
      {
        if (mpcParent)
        {
          cScreenArea lcPaintArea;
          mpcParent->vGetMaxArea(&lcPaintArea);

          mcPaintArea.miGfxRefPosx1 = lcPaintArea.miGfxRefPosx1;
          mcPaintArea.miGfxRefPosy1 = lcPaintArea.miGfxRefPosy1;
          miGfxWidth  = lcPaintArea.miGfxClipPosx2;
          miGfxHeight = lcPaintArea.miGfxClipPosy2;
        }
        else
        {
          mcPaintArea.miGfxRefPosx1 = mcRefScreen->miGfxRefPosx1;
          mcPaintArea.miGfxRefPosy1 = mcRefScreen->miGfxRefPosy1;
          miGfxWidth = mcRefScreen->iGfxWidth();
          miGfxHeight = mcRefScreen->iGfxHeight();
        }
      }
      else
      {
        miGfxWidth  += liGfxWidth;
        miGfxHeight += liGfxHeight;
      }

      if (miGfxWidth < 8)  miGfxWidth = 8;
      if (miGfxHeight < 8) miGfxHeight = 8;

      vUpdatePaintAreaResize();

      // Clippingbereich aller Children auch anpassen
      cUiElement* lcChild = mpcFirstChild;
      while (lcChild) 
      {
        lcChild->vResize(0, 0);
        lcChild = lcChild->mpcNext;
      }
    }

    void vUpdateCursorMove(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
    {
      if (mcPaintArea.isInAbs(liGfxCursorX, liGfxCursorY))
      {
        if (mpcParent)
        {
          mstStatus.CursorIn = mpcParent->mstStatus.CursorIn;
        }
        else
        {
          mstStatus.CursorIn = 1;
        }

        // ‹berpr¸fen, ob man von einem Nachbar ¸berlappt wird
        if ((mstStatus.CursorIn) && (mpcNext))
        {
          cUiElement* lcNext = mpcNext;
          while (lcNext) 
          {
            if (lcNext->mcPaintArea.isInAbs(liGfxCursorX, liGfxCursorY))
            {
              mstStatus.CursorIn = 0;
              return;
            }
            lcNext = lcNext->mpcNext;
          }
        }

      }
      else
      {
        mstStatus.CursorIn = 0;
      }
    }

    virtual bool isOnFrame(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
    {
      cScreenArea mcResizeArea;
      
      mcPaintArea.vClone(&mcResizeArea);

      mcResizeArea.miGfxClipPosx1  = mcPaintArea.miGfxRefPosx1 + miGfxWidth  - 8;
      mcResizeArea.miGfxClipPosy1  = mcPaintArea.miGfxRefPosy1 + miGfxHeight - 8;
      mcResizeArea.miGfxClipPosx2  = mcResizeArea.miGfxClipPosx1 + 8;
      mcResizeArea.miGfxClipPosy2  = mcResizeArea.miGfxClipPosy1 + 8;

      return mcResizeArea.isInAbs(liGfxCursorX, liGfxCursorY);
    }

    virtual void vGetMaxArea(cScreenArea* lcPaintArea)
    {
      lcPaintArea->miGfxRefPosx1 = mcPaintArea.miGfxRefPosx1;
      lcPaintArea->miGfxRefPosy1 = mcPaintArea.miGfxRefPosy1;
      
      lcPaintArea->miGfxClipPosx1 = 0;
      lcPaintArea->miGfxClipPosy1 = 0;

      lcPaintArea->miGfxClipPosx2 = miGfxWidth;
      lcPaintArea->miGfxClipPosy2 = miGfxHeight;
    }

    virtual void vToggleMaximize()
    {
      if (mstStatus.Sizeable)
      {
        if (!mstStatus.Maximized)
        {
          mstStatus.Maximized = 1;

          miGfxRefX_Save = mcPaintArea.miGfxRefPosx1;
          miGfxRefY_Save = mcPaintArea.miGfxRefPosy1;

          miGfxWidth_Save  = miGfxWidth;
          miGfxHeight_Save = miGfxHeight;

          // Referenzepunkt aller Children anpassen
          cScreenArea lcPaintArea;
          if (mpcParent)
          {
            mpcParent->vGetMaxArea(&lcPaintArea);
          }
          else
          {
            lcPaintArea.miGfxRefPosx1 = mcRefScreen->miGfxRefPosx1;
            lcPaintArea.miGfxRefPosy1 = mcRefScreen->miGfxRefPosy1;
          }

          cUiElement* lcChild = mpcFirstChild;

          while (lcChild) 
          {
            lcChild->vAddPos(lcPaintArea.miGfxRefPosx1 - mcPaintArea.miGfxRefPosx1, 
                             lcPaintArea.miGfxRefPosy1 - mcPaintArea.miGfxRefPosy1);
            lcChild = lcChild->mpcNext;
          }
        }
        else
        {
          mstStatus.Maximized = 0;

          // Referenzepunkt aller Children anpassen
          cUiElement* lcChild = mpcFirstChild;

          while (lcChild) 
          {
            lcChild->vAddPos(miGfxRefX_Save - mcPaintArea.miGfxRefPosx1, 
                             miGfxRefY_Save - mcPaintArea.miGfxRefPosy1);
            lcChild = lcChild->mpcNext;
          }

          mcPaintArea.miGfxRefPosx1 = miGfxRefX_Save;
          mcPaintArea.miGfxRefPosy1 = miGfxRefY_Save;

          miGfxWidth  = miGfxWidth_Save;
          miGfxHeight = miGfxHeight_Save;
        }
      }
      vResize(0, 0);
    }

    virtual void vGetPaintArea(cScreenArea* lcPaintArea)
    {
      mcPaintArea.vClone(lcPaintArea);
    }

    // Aktualisiert, welche Elemente neu gezeichnet werden m¸ssen
    virtual void vRepaint() 
    {
      // Falls diesesn Element von irgendeinem anderen Element ¸berlappt wird
      // dann alle Element neu Zeichnen
      if (mstStatus.Overlapped)
      {
        cGetRoot()->vSetRepaint();
      }
      else
      {
        // ..ansonsten nur ab diesem Element
        vSetRepaint();
      }
    }

    // Dieses und folgenden Elemente mit (mstStatus.Refresh == 1) werden beim n‰chtsen
    // Paint ber¸cksichtigt
    void vSetRepaint()
    {
      mstStatus.Repaint = 1;

      cUiElement* lcChild = mpcFirstChild;

      while (lcChild) 
      {
        lcChild->vSetRepaint();
        lcChild = lcChild->mpcNext;
      }
    }

    void vUpdatePaintAreaMove()
    {
      // Zeichenbereich setzen
      mcPaintArea.miGfxClipPosx1 = mcPaintArea.miGfxRefPosx1;
      mcPaintArea.miGfxClipPosy1 = mcPaintArea.miGfxRefPosy1;
      mcPaintArea.miGfxClipPosx2 = mcPaintArea.miGfxClipPosx1 + miGfxWidth  - 1;
      mcPaintArea.miGfxClipPosy2 = mcPaintArea.miGfxClipPosy1 + miGfxHeight - 1;

      // Position relativ zum Parent ¸bernehmen
      if (mpcParent != NULL)
      {
        cScreenArea lcPaintArea;

        mpcParent->vGetPaintArea(&lcPaintArea);

        // Sichtbarkeit vom Referenz ¸bernehmen
        mcPaintArea.mbVisible = lcPaintArea.mbVisible;
        mstStatus.Hidden      = mpcParent->mstStatus.Hidden;

        // Am Referenz-Bereich clippen
        // Check, ob komplett auﬂerhalb 
        if ((mcPaintArea.miGfxClipPosx1 <= lcPaintArea.miGfxClipPosx2) && (mcPaintArea.miGfxClipPosy1 <= lcPaintArea.miGfxClipPosy2) &&
            (mcPaintArea.miGfxClipPosx2 >= lcPaintArea.miGfxClipPosx1) && (mcPaintArea.miGfxClipPosy2 >= lcPaintArea.miGfxClipPosy1))
        {
          // Einzelne Bereiche checken
          if (mcPaintArea.miGfxClipPosx1 < lcPaintArea.miGfxClipPosx1) mcPaintArea.miGfxClipPosx1 = lcPaintArea.miGfxClipPosx1;
          if (mcPaintArea.miGfxClipPosx2 > lcPaintArea.miGfxClipPosx2) mcPaintArea.miGfxClipPosx2 = lcPaintArea.miGfxClipPosx2;
          if (mcPaintArea.miGfxClipPosy1 < lcPaintArea.miGfxClipPosy1) mcPaintArea.miGfxClipPosy1 = lcPaintArea.miGfxClipPosy1;
          if (mcPaintArea.miGfxClipPosy2 > lcPaintArea.miGfxClipPosy2) mcPaintArea.miGfxClipPosy2 = lcPaintArea.miGfxClipPosy2;
        }
        else
        {
          mcPaintArea.mbVisible = False;
          mstStatus.Hidden         = 1;
        }

        mpcParent->vSetRepaint();
      }
      else
      {
        // Am Referenz-Bereich clippen
        // Check, ob komplett auﬂerhalb 
        if ((mcPaintArea.miGfxClipPosx1 <= mcRefScreen->miGfxClipPosx2) && (mcPaintArea.miGfxClipPosy1 <= mcRefScreen->miGfxClipPosy2) &&
            (mcPaintArea.miGfxClipPosx2 >= mcRefScreen->miGfxClipPosx1) && (mcPaintArea.miGfxClipPosy2 >= mcRefScreen->miGfxClipPosy1))
        {
          // Einzelne Bereiche checken
          if (mcPaintArea.miGfxClipPosx1 < mcRefScreen->miGfxClipPosx1) mcPaintArea.miGfxClipPosx1 = mcRefScreen->miGfxClipPosx1;
          if (mcPaintArea.miGfxClipPosx2 > mcRefScreen->miGfxClipPosx2) mcPaintArea.miGfxClipPosx2 = mcRefScreen->miGfxClipPosx2;
          if (mcPaintArea.miGfxClipPosy1 < mcRefScreen->miGfxClipPosy1) mcPaintArea.miGfxClipPosy1 = mcRefScreen->miGfxClipPosy1;
          if (mcPaintArea.miGfxClipPosy2 > mcRefScreen->miGfxClipPosy2) mcPaintArea.miGfxClipPosy2 = mcRefScreen->miGfxClipPosy2;

          mcPaintArea.mbVisible = True;
          mstStatus.Hidden      = 0;
        }
        else
        {
          mcPaintArea.mbVisible = False;
          mstStatus.Hidden      = 1;
        }
        vSetRepaint();
      }
      cGetRoot()->OnElementChanged();
      
    }

    void vUpdatePaintAreaResize()
    {
      vUpdatePaintAreaMove();
      OnUpdateSize();
    }

    void vPaintBg()
    {
      if (mstStatus.PaintBg)
      {
        if (mpcFirstChild)
        {
          // Den grˆﬂten Child suchen
          cUiElement *mpcChild;
          cUiElement *mpcBiggest;
          u32        lu32Size;

          mpcBiggest = mpcChild = mpcFirstChild;
          lu32Size = (u32)(mpcChild->mcPaintArea.iGfxWidth() * mpcChild->mcPaintArea.iGfxHeight());

          mpcChild = mpcChild->mpcNext;
          while (mpcChild)
          {
            if (((u32)(mpcChild->mcPaintArea.iGfxWidth() * mpcChild->mcPaintArea.iGfxHeight())) > lu32Size)
            {
              lu32Size = (u32)(mpcChild->mcPaintArea.iGfxWidth() * mpcChild->mcPaintArea.iGfxHeight());
              mpcBiggest = mpcChild;
            }
            mpcChild = mpcChild->mpcNext;
          }

          GfxInt liGfxX1; 
          GfxInt liGfxY1; 
          GfxInt liGfxX2; 
          GfxInt liGfxY2;

          // Um den Biggest herum zeichnen
          liGfxX1 = mpcBiggest->mcPaintArea.miGfxClipPosx1 - mcPaintArea.miGfxRefPosx1; 
          liGfxY1 = mpcBiggest->mcPaintArea.miGfxClipPosy1 - mcPaintArea.miGfxRefPosy1; 
          liGfxX2 = mpcBiggest->mcPaintArea.miGfxClipPosx2 - mcPaintArea.miGfxRefPosx1; 
          liGfxY2 = mpcBiggest->mcPaintArea.miGfxClipPosy2 - mcPaintArea.miGfxRefPosy1;

          //Oben malen
          cPaint::vRectFull(0,           0,
                            miGfxWidth,  liGfxY1,
                            mu32BaseCol,  mcRefScreen);
          
          //Unten malen
          cPaint::vRectFull(0,           liGfxY2 + 1,
                            miGfxWidth,  mcPaintArea.miGfxClipPosy2 - mcPaintArea.miGfxRefPosy1 - liGfxY2,
                            mu32BaseCol, mcRefScreen);

          //Links malen
          cPaint::vRectFull(0,           liGfxY1,
                            liGfxX1,     liGfxY2 - liGfxY1 + 1,
                            mu32BaseCol, mcRefScreen);

          //Rechts malen
          cPaint::vRectFull(liGfxX2,     liGfxY1,
                            mcPaintArea.miGfxClipPosx2 - mcPaintArea.miGfxRefPosx1 - liGfxX2 + 1, liGfxY2 - liGfxY1 + 1,
                            mu32BaseCol, mcRefScreen);
        }
        else
        {
          GfxInt liGfxRefX; 
          GfxInt liGfxRefY; 
          GfxInt liGfxWidth; 
          GfxInt liGfxHeight;

          if (mstStatus.PaintFrame)
          {
            liGfxRefX   = 1; 
            liGfxRefY   = 1; 
            liGfxWidth  = miGfxWidth - 2; 
            liGfxHeight = miGfxHeight - 2;
          }
          else
          {
            liGfxRefX   = 0; 
            liGfxRefY   = 0; 
            liGfxWidth  = miGfxWidth; 
            liGfxHeight = miGfxHeight;
          }

          cPaint::vRectFull(liGfxRefX,   liGfxRefY,
                            liGfxWidth,  liGfxHeight,
                            mu32BaseCol, mcRefScreen);
        }
      }
    }


    
    // Zeichnet Rahmen und Hintergrund
    // Parset rekursiv durch den Elementen-Baum durch
    // Setzt den Clippingbereich f¸r OnPaint
    virtual void vPaint()
    {
      cScreenArea lcPaintAreaSave;
      if (isVisible())
      {
        if (mstStatus.Repaint)
        {
          mcRefScreen->vClone(&lcPaintAreaSave);

          // Meinen Zeichenbereich setzen
          mcRefScreen->vSetArea(&mcPaintArea);

          if (mstStatus.HiddenByChild == 0)
          {
            vPaintBg();
            OnPaint();
          } 
        }
        // Children malen
        if (mpcFirstChild) mpcFirstChild->vPaint();
      }

      if (isVisible() && (mstStatus.Repaint))
      {
        if (mstStatus.PaintFrame)
        {
          if (mstStatus.SelectedResize)
          {
            cPaint::vRect(0,            0,
                          miGfxWidth,   miGfxHeight,
                          mu32BaseCol, mcRefScreen);
            cPaint::vRectDashed(0,            0,
                                miGfxWidth,   miGfxHeight,
                                mu32FrameCol, 8, mcRefScreen);
          }
          else if (mstStatus.SelectedDrag)
          { 
            cPaint::vRect(0,            0,
                          miGfxWidth,   miGfxHeight,
                          mu32BaseCol, mcRefScreen);
            cPaint::vRectDashed(0,            0,
                                miGfxWidth,   miGfxHeight,
                                mu32FrameCol, 2, mcRefScreen);
          }
          else if (mstStatus.CursorIn)
          { 
            cPaint::vRect(0,            0,
                          miGfxWidth,   miGfxHeight,
                          mu32BaseCol, mcRefScreen);
            cPaint::vRectDashed(0,            0,
                                miGfxWidth,   miGfxHeight,
                                mu32FrameCol, 1, mcRefScreen);
          }
          else
          {
            cPaint::vRect(0,            0,
                          miGfxWidth,   miGfxHeight,
                          mu32FrameCol, mcRefScreen);
          }
        }
        mcRefScreen->vSetArea(&lcPaintAreaSave);
      }

      // Nachfolger malen
      if (mpcNext) mpcNext->vPaint();
      mstStatus.Repaint = 0;
    }

    void vClose()
    {
      // An alle Slaves weiterleiten
      cUiElement* lcChild = mpcFirstChild;
      while (lcChild)
      {
        lcChild->vClose();
        lcChild = lcChild->mpcNext;
      }
      OnClose();
    }

    void vDoSwipe(u32 lu32Timediff_ms)
    {
      if (!mstStatus.SelectedDrag)
      {
        if ((miGfxSpeedX) || (miGfxSpeedY))
        {
          i16 li16Dec = lu32Timediff_ms / 10;

          vAddPos(miGfxSpeedX / li16Dec, miGfxSpeedY / li16Dec);
          vSetSpeed((miGfxSpeedX * 10) / 11, (miGfxSpeedY * 10) / 11);
        }
      }
    }


    bool virtual bAdd(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight, cUiElement *lcEle)
    {
      if (mstStatus.Container)
      {
        if (mpcFirstChild)
        {
          // Vorne anf¸gen
          lcEle->mpcNext = mpcFirstChild;
          mpcFirstChild = lcEle;
        }
        else
        {
          mpcFirstChild = lcEle;
        }

        lcEle->mpcParent = this;
        lcEle->miGfxWidth  = liGfxWidth;
        lcEle->miGfxHeight = liGfxHeight;

        if (lcEle->miGfxWidth < 8)  lcEle->miGfxWidth = 8;
        if (lcEle->miGfxHeight < 8) lcEle->miGfxHeight = 8;

        lcEle->vSetPos(liGfxRefX, liGfxRefY);

        lcEle->OnLoaded();

        return True;
      }
      return False;
    }

    void vBringToTop()
    {
      // Dieses Element nach hinten in der Liste bringen
      if (mpcParent)
      {
        cUiElement* lcFirstChild = mpcParent->mpcFirstChild;

        // Ist dieser schon hinten, dann nichts zu tun ?
        if (this->mpcNext != NULL)
        {
          cUiElement* lcBefore = lcFirstChild;

          // Bin ich das erste Child
          if (lcFirstChild == this)
          {
            // diesen Element ausschneiden...
            mpcParent->mpcFirstChild = mpcNext;
          }
          else
          {
            // Das Element vor diesem suchen
            while (lcBefore) 
            {
              if (lcBefore->mpcNext == this)
              {
                break;
              }
              lcBefore = lcBefore->mpcNext;
            }

            // diesen Element ausschneiden...
            lcBefore->mpcNext = mpcNext;
          }

          // letztes Element suchen
          while (lcBefore->mpcNext != NULL) 
          {
            lcBefore = lcBefore->mpcNext;
          }

          // ... und hinten anh‰ngen
          lcBefore->mpcNext = this;
          mpcNext = NULL;

          OnElementChanged();
        }

        mstStatus.Repaint = 1;

        mpcParent->vBringToTop();
      }
    }

    // Das aktuelle Element Highlighten mit allen Parent
    void vDoHighlight(bool lbHighLight)
    {
      if (lbHighLight)
      {
        if (!mstStatus.CursorIn)
        {
          mstStatus.CursorIn = 1;
          vRepaint();
        }

        if (mpcParent)
        {
          cUiElement* lcParent = mpcParent;
          while (1) 
          {
            lcParent->mstStatus.CursorIn = 1;
            if (lcParent->mpcParent == NULL)
            {
              return;
            }          
            lcParent = lcParent->mpcParent;
          }
        }
      }
      else
      {
        if (mstStatus.CursorIn)
        {
          mstStatus.CursorIn = 0;
          vRepaint();
        }
      }
    }

    bool isChildOf(cUiElement* lcElement)
    {
      if (mpcParent)
      {
        cUiElement* lcParent = mpcParent;
        while (lcParent) 
        {
          if (lcParent == lcElement)
          {
            return True;
          }
          lcParent = lcParent->mpcParent;
        }
      }
      return False;
    }

    cUiElement* cGetRoot()
    {
      // Ist dieser schon der root, dann nichts zu tun ?
      if (mpcParent)
      {
        cUiElement* lcParent = mpcParent;
        while (1) 
        {
          if (lcParent->mpcParent == NULL)
          {
            return lcParent;
          }
          lcParent = lcParent->mpcParent;
        }
      }
      else
      {
        return this;
      }
      return NULL;
    }

    void vDoCheckHiddenAndOverlap()
    {
      mstStatus.Hidden         = 0;
      mstStatus.HiddenByChild  = 0;
      mstStatus.Overlapped     = 0;
    }

    // ‹berpr¸ft nur nachfolgende Elemente auf dem selben Level
    void vDoCheckHidden(cUiElement* lcEle2Check)
    {
      while (lcEle2Check)
      {
        if (lcEle2Check->isVisible())
        {
          // ‹berpr¸fnung f¸r Verdeckung
          if ((lcEle2Check->mcPaintArea.miGfxClipPosx1 <= mcPaintArea.miGfxClipPosx1) &&
              (lcEle2Check->mcPaintArea.miGfxClipPosy1 <= mcPaintArea.miGfxClipPosy1) &&
              (lcEle2Check->mcPaintArea.miGfxClipPosx2 >= mcPaintArea.miGfxClipPosx2) &&
              (lcEle2Check->mcPaintArea.miGfxClipPosy2 >= mcPaintArea.miGfxClipPosy2))
          {
            mstStatus.Hidden      = 1;
            mstStatus.Overlapped  = 0;
            return;
          }
          else
          {
            if (!mstStatus.Overlapped)
            {
              // ‹berpr¸fnung f¸r ‹berlappung
              if ((mcPaintArea.isInAbs(lcEle2Check->mcPaintArea.miGfxClipPosx1, lcEle2Check->mcPaintArea.miGfxClipPosy1)) ||
                  (mcPaintArea.isInAbs(lcEle2Check->mcPaintArea.miGfxClipPosx2, lcEle2Check->mcPaintArea.miGfxClipPosy1)) ||
                  (mcPaintArea.isInAbs(lcEle2Check->mcPaintArea.miGfxClipPosx1, lcEle2Check->mcPaintArea.miGfxClipPosy2)) ||
                  (mcPaintArea.isInAbs(lcEle2Check->mcPaintArea.miGfxClipPosx2, lcEle2Check->mcPaintArea.miGfxClipPosy2)) ||
                  (lcEle2Check->mcPaintArea.isInAbs(mcPaintArea.miGfxClipPosx1, mcPaintArea.miGfxClipPosy1)) ||
                  (lcEle2Check->mcPaintArea.isInAbs(mcPaintArea.miGfxClipPosx2, mcPaintArea.miGfxClipPosy1)) ||
                  (lcEle2Check->mcPaintArea.isInAbs(mcPaintArea.miGfxClipPosx1, mcPaintArea.miGfxClipPosy2)) ||
                  (lcEle2Check->mcPaintArea.isInAbs(mcPaintArea.miGfxClipPosx2, mcPaintArea.miGfxClipPosy2))
                 )
              {
                mstStatus.Overlapped  = 1;
              }
            }
          }
        }

        lcEle2Check = lcEle2Check->mpcNext;
      }
    }

    // ‹berpr¸ft nur nachfolgende Elemente auf dem selben Level, Parents und die Children
    void vDoCheckHidden()
    {
      cUiElement* lcEle2Check; 

      mstStatus.Hidden         = 0;
      mstStatus.HiddenByChild  = 0;
      mstStatus.Overlapped     = 0;

      // ‹berpr¸fen auf gleicher Ebene
      vDoCheckHidden(mpcNext);
      // Falls es verdeckt wird, braucht man nicht mehr  weitersuchen
      if (mstStatus.Hidden) return;

      // ‹berprufen, obes durch ein der Children verdeckt wird.
      lcEle2Check = mpcFirstChild;
      while (lcEle2Check)
      {
        if ((lcEle2Check->mcPaintArea.miGfxClipPosx1 <= mcPaintArea.miGfxClipPosx1) &&
            (lcEle2Check->mcPaintArea.miGfxClipPosy1 <= mcPaintArea.miGfxClipPosy1) &&
            (lcEle2Check->mcPaintArea.miGfxClipPosx2 >= mcPaintArea.miGfxClipPosx2) &&
            (lcEle2Check->mcPaintArea.miGfxClipPosy2 >= mcPaintArea.miGfxClipPosy2))
        {
          mstStatus.HiddenByChild = 1;
          mstStatus.Overlapped    = 1;
          return;
        }
        lcEle2Check = lcEle2Check->mpcNext;
      }

      // ‹berpr¸fen, ob es durch ein anderes Element im Baum ¸berdeckt wird
      cUiElement* lcEleParent = mpcParent; 

      while (lcEleParent)
      {
        lcEle2Check = lcEleParent->mpcNext;
        vDoCheckHidden(lcEle2Check);
        // Falls es verdeckt wird, braucht man nicht mehr  weitersuchen
        if (mstStatus.Hidden) return;

        lcEleParent = lcEleParent->mpcParent;
      }
    }

    
    virtual void vParentCallback(cUiElement *lpcChild, tenEvent lenEvent)
    {
      if (mpcParent) mpcParent->vParentCallback(lpcChild, lenEvent);
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

            switch (lenCursorEvent)
            {
              case cCursor::tenMsgEvent::nMove:           OnCursorMove(li16CurX, li16CurY, li16OldX, li16OldY);       break;
              case cCursor::tenMsgEvent::nPressStart:     OnCursorPressStart(li16CurX, li16CurY, li16OldX, li16OldY); break;
              case cCursor::tenMsgEvent::nPressEnd:       OnCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 0); break;
              case cCursor::tenMsgEvent::nPressEndShort:  OnCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 1); break;
              case cCursor::tenMsgEvent::nPressEndLong:   OnCursorPressEnd(li16CurX, li16CurY, li16OldX, li16OldY, 2); break;

              case cCursor::tenMsgEvent::nPressDragStart: OnCursorDragStart(li16CurX, li16CurY, li16OldX, li16OldY); break;
              case cCursor::tenMsgEvent::nDrag:           OnCursorDrag(li16CurX, li16CurY, li16OldX, li16OldY); break;
              default: break;
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


#endif  //__UI_ELEMENT_BASE_H__
