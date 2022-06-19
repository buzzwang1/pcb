#ifndef __UI_ELEMENT_SWITCH_H__
#define __UI_ELEMENT_SWITCH_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"
#include "uielement_Button.h"


class cUiElement_Switch: public cUiElement_Button
{
  public:

    u8 mu8State;

    cUiElement_Switch(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                      uint32 lui32MaxCharacterCount,
                      cRFont* lpcRFont)
      : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont)
    {
      mu8State = 0;
      menType = cUiElement::tenType::nSwitch;
    }


    void vSetState(u8 lu8NewState, bool lbDoCallback = True)
    {
      if (mu8State != lu8NewState)
      {
        mu8State = lu8NewState;
        if (lbDoCallback) 
        {
          cUiElement::vParentCallback(this, cUiElement::tenEvent::nSwitchStateChanged);
        }
        this->vRepaint();
      }
    }

    virtual void OnPressStart() override
    {
      if (mu8State) vSetState(0);
              else  vSetState(1);
    }
    
    virtual void OnPaint() override
    {
      mpcRFont->mui32Col = mui32TextCol;
      if (mu8State)
      {
        mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2 + 1, 3 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, cUiElement::mcRefScreen);
      }
      else
      {
        mpcRFont->i8PutStringXY((miGfxWidth - miGfxTextLength) / 2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, mChText, cUiElement::mcRefScreen);
      }
    }
};



class cUiElement_SwitchPressOnly: public cUiElement_Switch
{
  public:
    cUiElement_SwitchPressOnly(cMsgBox *lcMsgBox, cScreen *lpcScreen, 
                               uint32 lui32MaxCharacterCount,
                               cRFont* lpcRFont)
      : cUiElement_Switch(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont)
    {
      menType = cUiElement::tenType::nSwitchPressOnly;
    }


    virtual void OnPressStart() override
    {
      vSetState(1);
    }
};



class cUiElement_SwitchGroup: public cUiElement
{
  public:
    cUiElement_SwitchGroup(cMsgBox *lcMsgBox, cScreen *lpcScreen)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      menType = cUiElement::tenType::nSwitchGroup;
    }

    bool virtual bAdd(GfxInt liGfxRefX, GfxInt liGfxRefY, GfxInt liGfxWidth, GfxInt liGfxHeight, cUiElement *lcEle) override    
    { 
      if (lcEle->menType == cUiElement::tenType::nSwitchPressOnly)
      {
        return cUiElement::bAdd(liGfxRefX, liGfxRefY, liGfxWidth, liGfxHeight, lcEle);
      }
      else
      {
        return False;
      }
    }

    virtual void vParentCallback(cUiElement *lpcChild, tenEvent lenEvent) override
    {
      // Alle Chilred durchparsen
      cUiElement_SwitchPressOnly* lcChild = (cUiElement_SwitchPressOnly*)mpcFirstChild;
      while (lcChild) 
      {
        if (lcChild != (cUiElement_SwitchPressOnly*) lpcChild)
        {
          if (lcChild->mu8State)
          {
            lcChild->mu8State = 0;
            if (mpcParent)
            {
              mpcParent->vParentCallback(lcChild, cUiElement::tenEvent::nSwitchStateChanged);
            }
            lcChild->vRepaint();
          }
        }
        lcChild = (cUiElement_SwitchPressOnly*) lcChild->mpcNext;
      }

      cUiElement::vParentCallback(lpcChild, lenEvent);
    }

};

#endif  //__UI_ELEMENT_SWITCH_H__
