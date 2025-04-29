#ifndef __UI_ELEMENT_BASE_H__
#define __UI_ELEMENT_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap.h"
#include "screen.h"
#include "paint.h"
#include "Msg.h"
#include "Cursor.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define UI_INVALID        GFX_INVALID
#define UI_FAIL           GFX_FAIL
#define UI_OK             GFX_OK
#define UI_INLINE         GFX_INLINE


class cUiElement
{
  public:

    enum tenMsgEvent
    {
      // Button
      nBtnPressStart,
      nBtnPressEnd,

      // Window
      nWndClose,
      nWndSizeMax,
    };


    cUiElement  *mpcParent;
    
   
    // Absolute Koordinaten des Zeichen-Bereichs
    //
    //    +(Refx/RefY)
    //              (x1/y1)------+
    //              |            |
    //              +------(x2/y2)
    cScreenArea mcPaintArea;


    // Relative Position zum Parent
    //  (Refx/RefY)
    //       +----Width---+
    //       |            |
    //       |          Height
    //       |            |
    //       +------------+
    //
    GfxInt miGfxRefX;
    GfxInt miGfxRefY;
    GfxInt miGfxWidth;
    GfxInt miGfxHeight;

    GfxInt miGfxRefX_Save;
    GfxInt miGfxRefY_Save;
    GfxInt miGfxWidth_Save;
    GfxInt miGfxHeight_Save;

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
      u32 Sizeable   : 1; // Größe veränderbar

      /* Vom System gesetzt */
      u32 Selected       : 1; // Es wurde ausgewählt
      u32 SelectedDrag   : 1; // Es wurde ausgewählt und Cursor ist in einem Bereich, um es zu verschieben   (z.B. Titelbereich bei Fenster)
      u32 SelectedResize : 1; // Es wurde ausgewählt und Cursor ist in einem Bereich, um die Größe zu ändern (z.B. Titelbereich bei Fenster)
      u32 CursorIn       : 1; // Cursor ist in
      u32 Paint          : 1; // Wird vom EleTree gesetzt, zeigt an, dass das Element komplett verdeckt ist oder nicht
      u32 Maximized      : 1; // Wird vom EleTree gesetzt, zeigt an, dass das Element maximiert ist
    };

    tstStatus mstStatus;

    uint32 mu32BaseCol;
    uint32 mu32FrameCol;

    uint16 mGfxTreeIdx;

    cMsgBox *mcMsgBox;

    cUiElement(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxWidth, GfxInt liGfxHeight, cMsgBox *lcMsgBox)
    {
      vInit(liGfxPosx1, liGfxPosy1, liGfxWidth, liGfxHeight);
      mcMsgBox = lcMsgBox;
    }

    virtual ~cUiElement()
    {
    }

    void vInit(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      mpcParent = NULL;
      mGfxTreeIdx = 0xFFFF;

      mstStatus.DragAble   = 1;
      mstStatus.PaintBg    = 1;
      mstStatus.PaintFrame = 1;
      mstStatus.Visible    = 1;
      mstStatus.Enable     = 1;
      mstStatus.Sizeable   = 1;

      mstStatus.Selected       = 0;
      mstStatus.SelectedDrag   = 0;
      mstStatus.SelectedResize = 0;
      mstStatus.CursorIn       = 0;
      mstStatus.Paint          = 1;
      mstStatus.Maximized      = 0;

      mu32BaseCol  = 0;
      mu32FrameCol = 0x00FFFFFF;

      miGfxRefX   = liGfxRefX;
      miGfxRefY   = liGfxRefY;
      miGfxWidth  = liGfxWidth;
      miGfxHeight = liGfxHeight;

      miGfxSpeedX = 0;
      miGfxSpeedY = 0;
    }

    /* Positions Relative zum Parent */
    void vSetSpeed(GfxInt liGfxSpeedX, GfxInt liGfxSpeedY)
    {
      miGfxSpeedX = liGfxSpeedX;
      miGfxSpeedY = liGfxSpeedY;
    }


    /* Positions Relative zum Parent */
    void vSetPos(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      miGfxRefX = liGfxRefX;
      miGfxRefY = liGfxRefY;

      vUpdatePaintArea();
    }

    /* Positions Relative zum Parent */
    void vAddPos(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      miGfxRefX += liGfxRefX;
      miGfxRefY += liGfxRefY;

      vUpdatePaintArea();
    }

    /* Größe ändern */
    virtual void vResize(GfxInt liGfxRefX, GfxInt liGfxRefY)
    {
      if (mstStatus.Maximized == 0)
      {
        miGfxWidth  += liGfxRefX;
        miGfxHeight += liGfxRefY;

        if (miGfxWidth < 8)  miGfxWidth = 8;
        if (miGfxHeight < 8) miGfxHeight = 8;

        vUpdatePaintArea();
      }
    }

    void vUpdateCursorMove(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
    {
      if (mcPaintArea.isInAbs(liGfxCursorX, liGfxCursorY))
      {
        mstStatus.CursorIn = 1;
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

      mcResizeArea.miGfxClipPosx1  = mcResizeArea.miGfxClipPosx2 - 8;
      mcResizeArea.miGfxClipPosy1  = mcResizeArea.miGfxClipPosy2 - 8;
      mcResizeArea.miGfxClipPosx2 += 8;
      mcResizeArea.miGfxClipPosy2 += 8;

      return mcResizeArea.isInAbs(liGfxCursorX, liGfxCursorY);
    }

    virtual void vUpdateCursorPressStart(GfxInt liGfxCursorX, GfxInt liGfxCursorY)
    {
      mstStatus.SelectedDrag   = 0;
      mstStatus.SelectedResize = 0;

      if (isOnFrame(liGfxCursorX, liGfxCursorY))
      {
        if (mstStatus.Sizeable)
        {
          mstStatus.SelectedResize = 1;
        }
      }
      if (mcPaintArea.isInAbs(liGfxCursorX, liGfxCursorY))
      {
        mstStatus.Selected = 1;

        if ((mstStatus.Maximized == 0) && (mstStatus.SelectedResize == 0))
        {
          if (mstStatus.DragAble)
          {
            mstStatus.SelectedDrag = 1;
          }
        }
      }
      else
      {
        mstStatus.Selected     = 0;
      }
    }

    virtual void vUpdateCursorPressEnd()
    {
      mstStatus.SelectedResize = 0;
    }

    /* Called from from the EleTree, when was added to the Tree */
    virtual void vAddedToTree(void* lpcEleTree)
    {
      UNUSED(lpcEleTree);
    }

    /* Called from from the EleTree, when was added new Element.
       Return the Container/Parent Element */
    virtual uint16 vGetContainerTreeIdx()
    {
      return mGfxTreeIdx;
    }

    virtual void vGetMaxArea(cScreenArea* lcPaintArea)
    {
      lcPaintArea->miGfxRefPosx1 = 0;
      lcPaintArea->miGfxRefPosy1 = 0;
      
      lcPaintArea->miGfxClipPosx1 = 0;
      lcPaintArea->miGfxClipPosy1 = 0;

      lcPaintArea->miGfxClipPosx2 = miGfxWidth  - 1;
      lcPaintArea->miGfxClipPosy2 = miGfxHeight - 1;
    }

    virtual void vToggleMaximize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      if (mstStatus.Sizeable)
      {
        if (!mstStatus.Maximized)
        {
          mstStatus.Maximized = 1;

          miGfxRefX_Save   = miGfxRefX;
          miGfxRefY_Save   = miGfxRefY;
          miGfxWidth_Save  = miGfxWidth;
          miGfxHeight_Save = miGfxHeight;

          miGfxRefX   = liGfxRefX;
          miGfxRefY   = liGfxRefY;
          miGfxWidth  = liGfxWidth;
          miGfxHeight = liGfxHeight;
        }
        else
        {
          mstStatus.Maximized = 0;

          miGfxRefX   = miGfxRefX_Save;
          miGfxRefY   = miGfxRefY_Save;
          miGfxWidth  = miGfxWidth_Save;
          miGfxHeight = miGfxHeight_Save;
        }
      }
    }

    virtual void vUpdateSize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      miGfxRefX   = liGfxRefX;
      miGfxRefY   = liGfxRefY;
      miGfxWidth  = liGfxWidth;
      miGfxHeight = liGfxHeight;
    }

    virtual void vGetPaintArea(cScreenArea* lcPaintArea)
    {
      mcPaintArea.vClone(lcPaintArea);
    }

    void vUpdatePaintArea()
    {
      // Position relativ zum Parent übernehmen
      if (mpcParent != NULL)
      {
        cScreenArea lcPaintArea;

        mpcParent->vGetPaintArea(&lcPaintArea);
        vUpdatePaintArea(&lcPaintArea);
      }
    }

    void vUpdatePaintArea(cScreenArea* lcRefArea)
    {
      // Sichtbarkeit vom Referenz übernehmen
      mcPaintArea.mbVisible = lcRefArea->mbVisible;

      // Position relativ zur Referenz übernehmen
      mcPaintArea.miGfxRefPosx1 = lcRefArea->miGfxRefPosx1 + miGfxRefX;
      mcPaintArea.miGfxRefPosy1 = lcRefArea->miGfxRefPosy1 + miGfxRefY;

      // Zeichenbereich setzen
      mcPaintArea.miGfxClipPosx1 = mcPaintArea.miGfxRefPosx1;
      mcPaintArea.miGfxClipPosy1 = mcPaintArea.miGfxRefPosy1;
      mcPaintArea.miGfxClipPosx2 = mcPaintArea.miGfxClipPosx1 + miGfxWidth  - 1;
      mcPaintArea.miGfxClipPosy2 = mcPaintArea.miGfxClipPosy1 + miGfxHeight - 1;

      /* Am Referenz-Bereich clippen */
      /*Check, ob komplett außerhalb */
      if ((mcPaintArea.miGfxClipPosx1 <= lcRefArea->miGfxClipPosx2) && (mcPaintArea.miGfxClipPosy1 <= lcRefArea->miGfxClipPosy2) &&
          (mcPaintArea.miGfxClipPosx2 >= lcRefArea->miGfxClipPosx1) && (mcPaintArea.miGfxClipPosy2 >= lcRefArea->miGfxClipPosy1))
      {
        /* Einzelne Bereiche checken */
        if (mcPaintArea.miGfxClipPosx1 < lcRefArea->miGfxClipPosx1) mcPaintArea.miGfxClipPosx1 = lcRefArea->miGfxClipPosx1;
        if (mcPaintArea.miGfxClipPosx2 > lcRefArea->miGfxClipPosx2) mcPaintArea.miGfxClipPosx2 = lcRefArea->miGfxClipPosx2;
        if (mcPaintArea.miGfxClipPosy1 < lcRefArea->miGfxClipPosy1) mcPaintArea.miGfxClipPosy1 = lcRefArea->miGfxClipPosy1;
        if (mcPaintArea.miGfxClipPosy2 > lcRefArea->miGfxClipPosy2) mcPaintArea.miGfxClipPosy2 = lcRefArea->miGfxClipPosy2;
      }
      else
      {
        mcPaintArea.mbVisible = False;
      }
    }


    bool isVisible()
    {
      return ((mcPaintArea.mbVisible) && (mstStatus.Visible) && (mstStatus.Paint));
    }


    virtual void vPaint(cScreen *lpcScreen)
    {
      if (isVisible())
      {
        if (mstStatus.CursorIn)
        {  
          cScreenArea lcPaintArea;
          
          lcPaintArea.miGfxClipPosx1 = -1 + mcPaintArea.miGfxClipPosx1;
          lcPaintArea.miGfxClipPosy1 = -1 + mcPaintArea.miGfxClipPosy1;
          lcPaintArea.miGfxClipPosx2 =  1 + mcPaintArea.miGfxClipPosx2;
          lcPaintArea.miGfxClipPosy2 =  1 + mcPaintArea.miGfxClipPosy2;
          lcPaintArea.miGfxRefPosx1  =  lcPaintArea.miGfxClipPosx1;
          lcPaintArea.miGfxRefPosy1  =  lcPaintArea.miGfxClipPosy1;
          lcPaintArea.mbVisible      =  mcPaintArea.mbVisible;

          if (lcPaintArea.miGfxClipPosx1 < lpcScreen->miGfxClipPosx1) lcPaintArea.miGfxClipPosx1 = lpcScreen->miGfxClipPosx1;
          if (lcPaintArea.miGfxClipPosy1 < lpcScreen->miGfxClipPosy1) lcPaintArea.miGfxClipPosy1 = lpcScreen->miGfxClipPosy1;
          if (lcPaintArea.miGfxClipPosx2 > lpcScreen->miGfxClipPosx2) lcPaintArea.miGfxClipPosx2 = lpcScreen->miGfxClipPosx2;
          if (lcPaintArea.miGfxClipPosy2 > lpcScreen->miGfxClipPosy2) lcPaintArea.miGfxClipPosy2 = lpcScreen->miGfxClipPosy2;

          lpcScreen->vSetArea(&lcPaintArea);


          if (mstStatus.SelectedResize)
          {            
            cPaint::vRectDashed(0,  0,
                                miGfxWidth + 2, miGfxHeight + 2,
                                mu32FrameCol, 8, lpcScreen); 
          }
          else
          {            
            cPaint::vRect(0,  0,
                          miGfxWidth + 2, miGfxHeight + 2,
                          mu32FrameCol,   lpcScreen); 
          }
        }

        /* Meinen Zeichenbereich setzen */
        lpcScreen->vSetArea(&mcPaintArea);

        if (mstStatus.PaintFrame)
        {
          if (mstStatus.SelectedResize)
          {            
            cPaint::vRectDashed(0,            0,
                                miGfxWidth,   miGfxHeight,
                                mu32FrameCol, 8, lpcScreen);
          }
          else
          {            
            cPaint::vRect(0,            0,
                          miGfxWidth,   miGfxHeight,
                          mu32FrameCol, lpcScreen);
          }

          if (mstStatus.PaintBg)
          {
            if ((miGfxWidth > 2) && (miGfxHeight > 2))
            {
              cPaint::vRectFull(1,              1,
                                miGfxWidth - 2, miGfxHeight - 2,
                                mu32BaseCol,    lpcScreen);
            }
          }
        }
        else
        {
          if (mstStatus.PaintBg)
          {
            cPaint::vRectFull(0,           0,
                              miGfxWidth,  miGfxHeight,
                              mu32BaseCol, lpcScreen);
          }
        }
      }
    }

    virtual void vMsgHdl(cMsg *lpcMsg)
    {
      UNUSED(lpcMsg);
    }

};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BASE_H__
