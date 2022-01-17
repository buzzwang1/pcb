#ifndef __UI_ELEMENT_FGRAPHYBOX_H__
#define __UI_ELEMENT_FGRAPHYBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "graphy.h"
#include "cStringTools.h"


class cUiElement_fGraphYBox: public cUiElement, public cfGraphY
{
  public:

    cRFont *mpcRFont;
    cfGraphY_Channel *mcGraphChn;

    cUiElement_fGraphYBox(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                          cRFont *lpcRFont, cfGraphY_Channel *lcGraphChn)
      : cUiElement(lcMsgBox, lpcScreen), cfGraphY(lpcRFont, 0, 0, lpcScreen)
    {
      mpcRFont   = lpcRFont;
      mcGraphChn = lcGraphChn;

      mstStatus.PaintBg    = 0;
      mstStatus.PaintFrame = 0;
      menType = cUiElement::tenType::nfGraphYBox;
    }

    virtual void vToggleMaximize() override
    {
      cUiElement::vToggleMaximize();

      cfGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void OnUpdateSize() override
    {
      cfGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void OnPaint() override
    {
      if (isVisible())
      {
        cfGraphY::vClear();
        cfGraphY::vPaint(mcGraphChn);
      }
    }

};


#endif  //__UI_ELEMENT_FGRAPHYBOX_H__
