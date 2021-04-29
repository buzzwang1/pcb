#ifndef __UI_ELEMENT_GRAPHYBOX_H__
#define __UI_ELEMENT_GRAPHYBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "graphy.h"
#include "cStringTools.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cUiElement_iGraphYBox: public cUiElement, public ciGraphY
{
  public:

    cRFont *mpcRFont;
    ciGraphY_Channel *mcGraphChn;

    cUiElement_iGraphYBox(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
                          cRFont *lpcRFont, ciGraphY_Channel *lcGraphChn)
      : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox), ciGraphY(lpcRFont, liGfxPosx2 - liGfxPosx1 + 1, liGfxPosy2 - liGfxPosy1 + 1)
    {
      mpcRFont   = lpcRFont;
      mcGraphChn = lcGraphChn;

      mstStatus.PaintBg    = 0;
      mstStatus.PaintFrame = 0;
    }


    virtual void vToggleMaximize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vToggleMaximize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      ciGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void vUpdateSize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vUpdateSize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      ciGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }


    virtual void vPaint(cScreen *lpcScreen)
    {
      if (isVisible())
      {
        cUiElement::vPaint(lpcScreen);

        ciGraphY::vSetScreen(lpcScreen);
        
        ciGraphY::vClear();
        ciGraphY::vPaint(mcGraphChn);

        cPaint::vRect(0,                      0,
                      cUiElement::miGfxWidth, cUiElement::miGfxHeight,
                      mu32FrameCol,           lpcScreen);
      }
    }

};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_GRAPHBOX_H__
