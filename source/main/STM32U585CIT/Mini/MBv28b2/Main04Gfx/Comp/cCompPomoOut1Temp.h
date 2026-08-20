#pragma once

#include "cSysDPool.h"
#include "TMP102.h"

class cCompPomoOut1Tmp : public cComponent
{
  public:

  cTMP102 mcTMP102_0x48;

  cCompPomoOut1Tmp()
    : cComponent(cDepTreeCfg::cComp::nPomoOut1Tmp, cDepTreeCfg::cComp::nBoardMonitor),
      mcTMP102_0x48(cSysDPool::mBoard.mcI2c, TMP102_I2C_ADDRESS0)
  {
  }

  bool bInit() override
  {
    // Mit AddNode wird auch die Init Sequenzen requested
    cSysDPool::mBoard.mcI2c->vAddNode(&mcTMP102_0x48);

    vWaitCom();

    cComponentList::mcList128ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList128ms.vRemove(this->mu8Idx);

    mcTMP102_0x48.vClearAllRequest();

    vWaitCom();

    mcTMP102_0x48.vRequest(cTMP102_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcTMP102_0x48.vRequest(cTMP102_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    if (mcTMP102_0x48.isDone(cTMP102_RegisterMap::nSEQ07_ReadTemp))
    {
      cSysDPool::mBoard.mcPomoOut1.mfTemp_C = mcTMP102_0x48.fGetTemp();
    }

    mcTMP102_0x48.vRequest(cTMP102_RegisterMap::nSEQ07_ReadTemp);

    // return True to signal finished
    return cComponent::bRun();
  };

  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if (mcTMP102_0x48.isAnySeqPending())
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }

  bool bCheckTmp102Error()
  {
    cSysDPool::mBoard.mcPomoOut1.mu8I2cTempErr = 0;
    if (mcTMP102_0x48.IsError()) cSysDPool::mBoard.mcPomoOut1.mu8I2cTempErr |= 1;

    if (cSysDPool::mBoard.mcPomoOut1.mu8I2cTempErr) return True;
    return False;
  }
};




