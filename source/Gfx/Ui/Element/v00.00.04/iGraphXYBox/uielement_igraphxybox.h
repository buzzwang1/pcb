#ifndef __UI_ELEMENT_GRAPHXYBOX_H__
#define __UI_ELEMENT_GRAPHXYBOX_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "rfont.h"
#include "graphxy.h"
#include "cStringTools.h"


class cUiElement_iGraphXYBox: public cUiElement, public ciGraphXY
{
  public:

    cRFont *mpcRFont;
    ciGraphXY_Channel *mcGraphChn;

    cUiElement_iGraphXYBox(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                           cRFont *lpcRFont, ciGraphXY_Channel *lcGraphChn)
      : cUiElement(lcMsgBox, lpcScreen), ciGraphXY(lpcRFont, 0, 0, lpcScreen)
    {
      mpcRFont   = lpcRFont;
      mcGraphChn = lcGraphChn;

      mstStatus.PaintBg    = 0;
      mstStatus.PaintFrame = 0;
      menType = cUiElement::tenType::niGraphXYBox;
    }


    virtual void vToggleMaximize() override
    {
      cUiElement::vToggleMaximize();

      ciGraphXY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }

    virtual void OnUpdateSize() override
    {
      ciGraphXY::vSetSize(cUiElement::miGfxWidth, cUiElement::miGfxHeight);
    }


    virtual void OnPaint() override
    {
      if (isVisible())
      {
        ciGraphXY::vClear();
        ciGraphXY::vPaint(mcGraphChn);
      }
    }

};


#endif  //__UI_ELEMENT_GRAPHBOX_H__
