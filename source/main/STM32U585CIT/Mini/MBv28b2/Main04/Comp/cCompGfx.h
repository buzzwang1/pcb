#pragma once

#include "FreeRTOS.h"
#include "cSysDPool.h"
#include "ili9341.h"
#include "cSPi.h"

class cCompGfx : public cComponent
{
  public:

  cIli9341 mcILI9341;

  cCompGfx()
    : cComponent(cDepTreeCfg::cComp::nGfx, cDepTreeCfg::cComp::nGfxSpi2),
      mcILI9341(cSysDPool::mBoard.mcSpi2)
  {
  }

  bool bInit() override
  {
    mcILI9341.vInitHw();

    // Mit AddNode wird auch die Init Sequenzen requested

    cSysDPool::mBoard.mcSpi2->vAddNode(&mcILI9341);

    vWaitCom();

    mcILI9341.vSetLigth(100);

    mcILI9341.vRequest(cIna3221_RegisterMap::nSEQ07WriteCfg);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    mcILI9341.vClearAllRequest();

    vWaitCom();

    mcILI9341.vRequest(cIna3221_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcILI9341.vRequest(cIna3221_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };


  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if (mcILI9341.isAnySeqPending())
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




