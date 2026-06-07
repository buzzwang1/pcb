#pragma once

#include "FreeRTOS.h"
#include "cSysDPool.h"
#include "ili9341.h"
#include "XPT2046.h"
#include "cSPi.h"

class cCompGfx : public cComponent
{
  public:

  cIli9341 mcILI9341;
  cXPT2046 mcXPT2046;

  cCompGfx()
    : cComponent(cDepTreeCfg::cComp::nGfx, cDepTreeCfg::cComp::nGfxSpi2),
      mcILI9341(cSysDPool::mBoard.mcSpi2),
      mcXPT2046(cSysDPool::mBoard.mcSpi2)
  {
    cSysDPool::mBoard.mcGfx.u16TouchRawXMax = 0;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMax = 0;
    cSysDPool::mBoard.mcGfx.u16TouchRawXMin = 0xFFFF;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMin = 0xFFFF;
  }

  bool bInit() override
  {
    mcILI9341.vInitHw();
    mcXPT2046.vInitHw();

    // Mit AddNode wird auch die Init Sequenzen requested

    cSysDPool::mBoard.mcSpi2->vAddNode(&mcILI9341);
    cSysDPool::mBoard.mcSpi2->vAddNode(&mcXPT2046);

    vWaitCom();

    mcILI9341.vSetLigth(25);

    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ07WriteGRAMCfg);
    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ08WriteGRAMData);

    cComponentList::mcList16ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList16ms.vRemove(this->mu8Idx);

    mcILI9341.vClearAllRequest();
    mcXPT2046.vClearAllRequest();

    vWaitCom();

    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ05Disable);
    mcXPT2046.vRequest(cIli9341_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ06DeInit);
    mcXPT2046.vRequest(cIli9341_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    u8 lu8Touch = 0;

    if (mcXPT2046.mIrq.ui8Get() == 0)
    {
      if (mcXPT2046.isDone((u8)cXPT2046_RegisterMap::nSEQ07_ReadXYZ))
      {
        u16 lu16Xa;
        u16 lu16Xb;
        u16 lu16Ya;
        u16 lu16Yb;
        u16 lu16XDiff;
        u16 lu16YDiff;


        lu16Xa = (((u16)mcXPT2046.stRegs.unCtrlXa.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlXa.au8Byte[1]);
        lu16Xb = (((u16)mcXPT2046.stRegs.unCtrlXb.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlXb.au8Byte[1]);
        lu16Ya = (((u16)mcXPT2046.stRegs.unCtrlYa.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlYa.au8Byte[1]);
        lu16Yb = (((u16)mcXPT2046.stRegs.unCtrlYb.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlYb.au8Byte[1]);

        lu16Xa = (lu16Xa & 0x7FFF) >> 3;
        lu16Xb = (lu16Xb & 0x7FFF) >> 3;
        lu16Ya = (lu16Ya & 0x7FFF) >> 3;
        lu16Yb = (lu16Yb & 0x7FFF) >> 3;


        if (lu16Xa > lu16Xb) lu16XDiff = lu16Xa - lu16Xb;
                        else lu16XDiff = lu16Xb - lu16Xa;

        if (lu16Ya > lu16Yb) lu16YDiff = lu16Ya - lu16Yb;
                        else lu16YDiff = lu16Yb - lu16Ya;


        if ((lu16XDiff < 50) && (lu16YDiff < 50))
        {
          u16 lu16X = (lu16Xa + lu16Xb) / 2;
          u16 lu16Y = (lu16Ya + lu16Yb) / 2;


          cSysDPool::mBoard.mcGfx.u16TouchRawX = lu16X;
          cSysDPool::mBoard.mcGfx.u16TouchRawY = lu16Y;

          if (lu16X > cSysDPool::mBoard.mcGfx.u16TouchRawXMax) cSysDPool::mBoard.mcGfx.u16TouchRawXMax = lu16X;
          if (lu16Y > cSysDPool::mBoard.mcGfx.u16TouchRawYMax) cSysDPool::mBoard.mcGfx.u16TouchRawYMax = lu16Y;
          if (lu16X < cSysDPool::mBoard.mcGfx.u16TouchRawXMin) cSysDPool::mBoard.mcGfx.u16TouchRawXMin = lu16X;
          if (lu16Y < cSysDPool::mBoard.mcGfx.u16TouchRawYMin) cSysDPool::mBoard.mcGfx.u16TouchRawYMin = lu16Y;

          float lfXDiffMinMax = cSysDPool::mBoard.mcGfx.u16TouchRawXMax - cSysDPool::mBoard.mcGfx.u16TouchRawXMin;
          float lfYDiffMinMax = cSysDPool::mBoard.mcGfx.u16TouchRawYMax - cSysDPool::mBoard.mcGfx.u16TouchRawYMin;

          cSysDPool::mBoard.mcGfx.u16TouchX = (float)(lu16X - cSysDPool::mBoard.mcGfx.u16TouchRawXMin) * 240.0f / lfXDiffMinMax;
          cSysDPool::mBoard.mcGfx.u16TouchY = (float)(lu16Y - cSysDPool::mBoard.mcGfx.u16TouchRawYMin) * 320.0f / lfYDiffMinMax;

          lu8Touch = 1;
        }
      }
      mcXPT2046.vRequest(cXPT2046_RegisterMap::nSEQ07_ReadXYZ);
    }

    cSysDPool::mBoard.mcGfx.u8TouchIrq = lu8Touch;

    // return True to signal finished
    return cComponent::bRun();
  };


  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if ((mcILI9341.isAnySeqPending()) ||
          (mcXPT2046.isAnySeqPending()))
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }
};




