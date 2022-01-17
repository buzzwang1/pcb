#ifndef __UI_ELEMENT_GRAPHYBOX_H__
#define __UI_ELEMENT_GRAPHYBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "graphy.h"
#include "cStringTools.h"


class cUiElement_iGraphYBox: public cUiElement, public ciGraphY
{
  public:

    cRFont *mpcRFont;
    ciGraphY_Channel *mcGraphChn;

    cUiElement_iGraphYBox(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                          cRFont *lpcRFont, ciGraphY_Channel *lcGraphChn)
      : cUiElement(lcMsgBox, lpcScreen), ciGraphY(lpcRFont, 0, 0, lpcScreen)
    {
      mpcRFont   = lpcRFont;
      mcGraphChn = lcGraphChn;

      mstStatus.PaintBg    = 0;
      mstStatus.PaintFrame = 0;
      menType = cUiElement::tenType::niGraphYBox;
    }

    virtual void vToggleMaximize()
    {
      cUiElement::vToggleMaximize();

      ciGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void OnUpdateSize() override
    {
      ciGraphY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }


    virtual void OnPaint() override
    {
      if (isVisible())
      { 
        ciGraphY::vClear();
        ciGraphY::vPaint(mcGraphChn);
      }
    }

};


#endif  //__UI_ELEMENT_GRAPHBOX_H__
