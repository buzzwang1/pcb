#ifndef __UI_ELEMENT_BOX3DF_H__
#define __UI_ELEMENT_BOX3DF_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "math.h"
#include "uielement_box3df_viewer_base.h"
#include "Texture32.h"
#include "MemTools.h"


class cUiElement_Box3Df : public cUiElement
{
public:
  uint8                        mui8Play;

  cWorld_3Df*                  mcpWorld;
  cWorld_3Df_ViewerWorld_Base* mcpViewer;

  cUiElement_Box3Df(cMsgBox *lcMsgBox, cScreen *lpcScreen, cWorld_3Df* lcWorld, cWorld_3Df_ViewerWorld_Base* lcViewer)
    : cUiElement(lcMsgBox, lpcScreen) 
  {
     mui8Play = 1;
     mcpWorld  = lcWorld;
     mcpViewer = lcViewer;
     menType = cUiElement::tenType::nBox3Df;
  }

  ~cUiElement_Box3Df()
  {
  }

  virtual void OnLoaded() override
  {
    mcpViewer->World2Viewer();
  }


  virtual void vRepaint() override
  {
    cUiElement::vRepaint();
    mcpViewer->World2Viewer();
  }

  virtual void OnTimer(u32 lu32Timediff_ms) override
  {
    UNUSED(lu32Timediff_ms);
    if (mui8Play)
    {
      mcpViewer->World2Viewer();
    }
    vRepaint();
  }

  virtual void OnPaint() override
  {
    mcpViewer->Show(miGfxWidth, miGfxHeight, mcRefScreen, False);
  }

  virtual void vMsgHdl(cMsg* lpcMsg) override
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
            if (mstStatus.CursorIn)
            {
              mui8Play = !mui8Play;
            }
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

};

#endif  //__UI_ELEMENT_BOX3DF_H__
