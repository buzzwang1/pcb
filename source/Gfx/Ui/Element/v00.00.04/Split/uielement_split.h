#ifndef __UI_ELEMENT_SPLIT_H__
#define __UI_ELEMENT_SPLIT_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_Base.h"


class cUiElement_VSplit: public cUiElement
{
  public:
    u8 muiSplitRatio;

    cUiElement mcElementLeft;
    cUiElement mcElementRight;

    GfxInt  miGfxWidthLeft_Limit;
    GfxInt  miGfxWidthRigth_Limit;

    cUiElement_VSplit(cMsgBox *lcMsgBox, cScreen *lpcScreen, u8 luiSplitRatio)
      : cUiElement_VSplit(lcMsgBox, lpcScreen, luiSplitRatio, -1, -1)
      {}


    cUiElement_VSplit(cMsgBox *lcMsgBox, cScreen *lpcScreen, u8 luiSplitRatio, GfxInt liGfxWidthLeftLimit, GfxInt liGfxWidthRigthLimit)
      : cUiElement(lcMsgBox, lpcScreen),
        mcElementLeft(lcMsgBox, lpcScreen),
        mcElementRight(lcMsgBox, lpcScreen)
      {
        vSetLimit(liGfxWidthLeftLimit, liGfxWidthRigthLimit);

        muiSplitRatio = luiSplitRatio;

        mcElementLeft.mstStatus.DragAble = 0;
        mcElementLeft.mstStatus.Sizeable = 0;
        mcElementRight.mstStatus.DragAble = 0;
        mcElementRight.mstStatus.Sizeable = 0;

        menType = cUiElement::tenType::nVSplit;
      }

    void vSetLimit(GfxInt liGfxWidthLeftLimit, GfxInt liGfxWidthRigthLimit)
    {
       miGfxWidthLeft_Limit  = liGfxWidthLeftLimit;
       miGfxWidthRigth_Limit = liGfxWidthRigthLimit;

       if (liGfxWidthLeftLimit > 0)
       {
         miGfxWidthRigth_Limit = -1;
       }
       else
       {
         liGfxWidthLeftLimit = -1;
         if (liGfxWidthRigthLimit <= 0)
         {
           miGfxWidthLeft_Limit = -1;
         }
       }
    }

    virtual void OnLoaded() override
    {
      GfxInt liGfxWidthLeft;
      GfxInt liGfxWidthRigth;

      liGfxWidthLeft  = (miGfxWidth * muiSplitRatio) / 100;

      if ((miGfxWidthLeft_Limit > 0) && (liGfxWidthLeft > miGfxWidthLeft_Limit))
      {
        liGfxWidthLeft = miGfxWidthLeft_Limit;
      }
      
      liGfxWidthRigth =  miGfxWidth - liGfxWidthLeft;
      if ((miGfxWidthRigth_Limit > 0) && (liGfxWidthRigth > miGfxWidthRigth_Limit))
      {
        liGfxWidthRigth = miGfxWidthRigth_Limit;
        liGfxWidthLeft  =  miGfxWidth - liGfxWidthRigth;
      }

      cUiElement::bAdd(0 ,             0, liGfxWidthLeft + 1,  this->miGfxHeight, &mcElementLeft);
      cUiElement::bAdd(liGfxWidthLeft, 0, liGfxWidthRigth,     this->miGfxHeight, &mcElementRight);
    }

    virtual void OnUpdateSize() override
    {
      GfxInt liGfxWidthLeft;
      GfxInt liGfxWidthRigth;

      liGfxWidthLeft  = (this->miGfxWidth * muiSplitRatio) / 100;

      if ((miGfxWidthLeft_Limit > 0) && (liGfxWidthLeft > miGfxWidthLeft_Limit))
      {
        liGfxWidthLeft = miGfxWidthLeft_Limit;
      }
      
      liGfxWidthRigth = this->miGfxWidth - liGfxWidthLeft;
      if ((miGfxWidthRigth_Limit > 0) && (liGfxWidthRigth > miGfxWidthRigth_Limit))
      {
        liGfxWidthRigth = miGfxWidthRigth_Limit;
        liGfxWidthLeft  = this->miGfxWidth - liGfxWidthRigth;
      }

      mcElementLeft.miGfxWidth   =  liGfxWidthLeft + 1;
      mcElementRight.miGfxWidth  =  liGfxWidthRigth;

      mcElementLeft.miGfxHeight  = this->miGfxHeight;
      mcElementRight.miGfxHeight = this->miGfxHeight;

      
      GfxInt liGfxRefx_Old;
      GfxInt liGfxRefY_Old;
      cUiElement* lcChild;

      liGfxRefx_Old = mcElementLeft.mcPaintArea.miGfxRefPosx1;
      liGfxRefY_Old = mcElementLeft.mcPaintArea.miGfxRefPosy1;
      mcElementLeft.vSetPos(0, 0);

      // Referenzepunkt aller Children anpassen
      lcChild = mcElementLeft.mpcFirstChild;

      while (lcChild) 
      {
        lcChild->vAddPos(mcElementLeft.mcPaintArea.miGfxRefPosx1 - liGfxRefx_Old, 
                         mcElementLeft.mcPaintArea.miGfxRefPosy1 - liGfxRefY_Old);
        lcChild = lcChild->mpcNext;
      }

      liGfxRefx_Old = mcElementRight.mcPaintArea.miGfxRefPosx1;
      liGfxRefY_Old = mcElementRight.mcPaintArea.miGfxRefPosy1;

      mcElementRight.vSetPos(liGfxWidthLeft, 0);

      // Referenzepunkt aller Children anpassen
      lcChild = mcElementRight.mpcFirstChild;

      while (lcChild) 
      {
        lcChild->vAddPos(mcElementRight.mcPaintArea.miGfxRefPosx1 - liGfxRefx_Old, 
                         mcElementRight.mcPaintArea.miGfxRefPosy1 - liGfxRefY_Old);
        lcChild = lcChild->mpcNext;
      }
    }
};


class cUiElement_HSplit: public cUiElement
{
  public:
    u8 muiSplitRatio;

    cUiElement mcElementTop;
    cUiElement mcElementBot;

    GfxInt  miGfxHeightTop_Limit;
    GfxInt  miGfxHeightBot_Limit;

    cUiElement_HSplit(cMsgBox *lcMsgBox, cScreen *lpcScreen, u8 luiSplitRatio)
      : cUiElement_HSplit(lcMsgBox, lpcScreen, luiSplitRatio, -1, -1)
      {}

    cUiElement_HSplit(cMsgBox *lcMsgBox, cScreen *lpcScreen, u8 luiSplitRatio, GfxInt liGfxHeightTopLimit, GfxInt liGfxHeightBotLimit)
      : cUiElement(lcMsgBox, lpcScreen),
        mcElementTop(lcMsgBox, lpcScreen),
        mcElementBot(lcMsgBox, lpcScreen)
      {
        vSetLimit(liGfxHeightTopLimit, liGfxHeightBotLimit);

        muiSplitRatio = luiSplitRatio;

        mcElementTop.mstStatus.DragAble = 0;
        mcElementTop.mstStatus.Sizeable = 0;
        mcElementBot.mstStatus.DragAble = 0;
        mcElementBot.mstStatus.Sizeable = 0;

        menType = cUiElement::tenType::nHSplit;
      }

    void vSetLimit(GfxInt liGfxHeightTopLimit, GfxInt liGfxHeightBotLimit)
    {
       miGfxHeightTop_Limit  = liGfxHeightTopLimit;
       miGfxHeightBot_Limit = liGfxHeightBotLimit;

       if (liGfxHeightTopLimit > 0)
       {
         miGfxHeightBot_Limit = -1;
       }
       else
       {
         liGfxHeightTopLimit = -1;
         if (liGfxHeightBotLimit <= 0)
         {
           miGfxHeightTop_Limit = -1;
         }
       }
    }

    virtual void OnLoaded() override
    {
      GfxInt liGfxHeightTop;
      GfxInt liGfxHeightBot;

      liGfxHeightTop  = (this->miGfxHeight * muiSplitRatio) / 100;

      if ((miGfxHeightTop_Limit > 0) && (liGfxHeightTop > miGfxHeightTop_Limit))
      {
        liGfxHeightTop = miGfxHeightTop_Limit;
      }
      
      liGfxHeightBot = this->miGfxHeight - liGfxHeightTop;
      if ((miGfxHeightBot_Limit > 0) && (liGfxHeightBot > miGfxHeightBot_Limit))
      {
        liGfxHeightBot = miGfxHeightBot_Limit;
        liGfxHeightTop = this->miGfxHeight - liGfxHeightBot;
      }

      cUiElement::bAdd(0,  0,              this->miGfxWidth, liGfxHeightTop + 1, &mcElementTop);
      cUiElement::bAdd(0,  liGfxHeightTop, this->miGfxWidth, liGfxHeightBot,     &mcElementBot);
    }

    virtual void OnUpdateSize() override
    {
      GfxInt liGfxHeightTop;
      GfxInt liGfxHeightBot;

      liGfxHeightTop  = (this->miGfxHeight * muiSplitRatio) / 100;

      if ((miGfxHeightTop_Limit > 0) && (liGfxHeightTop > miGfxHeightTop_Limit))
      {
        liGfxHeightTop = miGfxHeightTop_Limit;
      }
      
      liGfxHeightBot =  miGfxHeight - liGfxHeightTop;
      if ((miGfxHeightBot_Limit > 0) && (liGfxHeightBot > miGfxHeightBot_Limit))
      {
        liGfxHeightBot = miGfxHeightBot_Limit;
        liGfxHeightTop  =  miGfxHeight - liGfxHeightBot;
      }

      mcElementTop.miGfxHeight  =  liGfxHeightTop + 1;
      mcElementBot.miGfxHeight  =  liGfxHeightBot;

      mcElementTop.miGfxWidth = this->miGfxWidth;
      mcElementBot.miGfxWidth = this->miGfxWidth;

      GfxInt liGfxRefx_Old;
      GfxInt liGfxRefY_Old;
      cUiElement* lcChild;

      liGfxRefx_Old = mcElementTop.mcPaintArea.miGfxRefPosx1;
      liGfxRefY_Old = mcElementTop.mcPaintArea.miGfxRefPosy1;
      mcElementTop.vSetPos(0, 0);

      // Referenzepunkt aller Children anpassen
      lcChild = mcElementTop.mpcFirstChild;

      while (lcChild) 
      {
        lcChild->vAddPos(mcElementTop.mcPaintArea.miGfxRefPosx1 - liGfxRefx_Old, 
                         mcElementTop.mcPaintArea.miGfxRefPosy1 - liGfxRefY_Old);
        lcChild = lcChild->mpcNext;
      }

      liGfxRefx_Old = mcElementBot.mcPaintArea.miGfxRefPosx1;
      liGfxRefY_Old = mcElementBot.mcPaintArea.miGfxRefPosy1;

      mcElementBot.vSetPos(0, liGfxHeightTop);

      // Referenzepunkt aller Children anpassen
      lcChild = mcElementBot.mpcFirstChild;

      while (lcChild) 
      {
        lcChild->vAddPos(mcElementBot.mcPaintArea.miGfxRefPosx1 - liGfxRefx_Old, 
                         mcElementBot.mcPaintArea.miGfxRefPosy1 - liGfxRefY_Old);
        lcChild = lcChild->mpcNext;
      }
    }
};



#endif  //__UI_ELEMENT_SPLIT_H__
