#ifndef __UI_ELEMENT_BOX3DF_H__
#define __UI_ELEMENT_BOX3DF_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "math.h"
#include "uielement_box3df_viewer_base.h"
#include "Texture32.h"
#include "MemTools.h"


#ifdef __cplusplus
 extern "C" {
#endif


class cUiElement_Box3Df : public cUiElement
{
public:
  uint8                        mui8Play;

  cWorld_3Df*                  cWorld;
  cWorld_3Df_ViewerWorld_Base* cViewer;


  cUiElement_Box3Df(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox, cWorld_3Df* lcWorld, cWorld_3Df_ViewerWorld_Base* lcViewer)
    : cUiElement(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox) 
  {
     mui8Play = 1;
     cWorld  = lcWorld;
     cViewer = lcViewer;
  }

  ~cUiElement_Box3Df()
  {
  }

  virtual void vMsgHdl(cMsg* lpcMsg)
  {
    cUiElement::vMsgHdl(lpcMsg);

    switch ((cMsg::tenMsgIDs)lpcMsg->mID)
    {
    case cMsg::tenMsgIDs::nCursor:
    {
      cCursor::tenMsgEvent lenCursorEvent;

      lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

      switch (lenCursorEvent)
      {
        case cCursor::tenMsgEvent::nPressEndShort:
        {
          mui8Play = !mui8Play;
        }
        break;

        default:
          break;
        }
    }
    break;
    default:
      break;
    }
  }

  virtual void vPaint(cScreen* lpcScreen)
  {
    if (isVisible())
    {
      cUiElement::vPaint(lpcScreen);

      cViewer->World2Viewer();
      cViewer->Show(miGfxWidth, miGfxHeight, lpcScreen, False);
    }
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_H__
