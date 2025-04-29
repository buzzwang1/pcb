#ifndef __UI_ELEMENT_FGRAPHYBOX_H__
#define __UI_ELEMENT_FGRAPHYBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "graphy.h"
#include "cStringTools.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cUiElement_fGraphYBox: public cUiElement, public cfGraphY
{
  public:

    cRFont *mpcRFont;
    cfGraphY_Channel *mcGraphChn;

    cUiElement_fGraphYBox(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
                          cRFont *lpcRFont, cfGraphY_Channel *lcGraphChn)
      : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox), cfGraphY(lpcRFont, liGfxPosx2 - liGfxPosx1 + 1, liGfxPosy2 - liGfxPosy1 + 1)
    {
      mpcRFont   = lpcRFont;
      mcGraphChn = lcGraphChn;

      mstStatus.PaintBg    = 0;
      mstStatus.PaintFrame = 0;
    }


    virtual void vToggleMaximize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vToggleMaximize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      cfGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void vUpdateSize(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight)
    {
      cUiElement::vUpdateSize(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight);

      cfGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }


    virtual void vPaint(cScreen *lpcScreen)
    {
      if (isVisible())
      {
        cUiElement::vPaint(lpcScreen);

        cfGraphY::vSetScreen(lpcScreen);
        
        cfGraphY::vClear();
        cfGraphY::vPaint(mcGraphChn);

        cPaint::vRect(0,                      0,
                      cUiElement::miGfxWidth, cUiElement::miGfxHeight,
                      mu32FrameCol,           lpcScreen);
      }
    }

};

#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_FGRAPHYBOX_H__
