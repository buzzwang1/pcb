#pragma once

#include "cSysDPool.h"


class cCompPomoOut1 : public cComponent
{
  public:

  cGpPin    mcTPS55288_En;
  cTPS55288 mcTPS55288_0x74;

  cCompPomoOut1()
    : cComponent(cDepTreeCfg::cComp::nPomoOut1, cDepTreeCfg::cComp::nPomoOut1Tmp),
      mcTPS55288_En(GPIOB_BASE, 12),
      mcTPS55288_0x74(cSysDPool::mBoard.mcI2c, TPS55288_I2C_ADDRESS_CONF_0)
  {
    cSysDPool::mBoard.mcPomoOut1.mpcTPS55288 = &mcTPS55288_0x74;
  }

  bool bInit() override
  {
    mcTPS55288_En.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mcTPS55288_En.vSet1();

    vTaskDelay(pdMS_TO_TICKS(3));

    // Mit AddNode wird auch die Init Sequenzen requested
    cSysDPool::mBoard.mcI2c->vAddNode(&mcTPS55288_0x74);

    cComponentList::mcList128ms.Add(this->mu8Idx);

    vWaitCom();

    mcTPS55288_0x74.vSetOutput(5000, 500);

    cTPS55288_RegisterMap::tunMode lunMode;

    lunMode.u8Raw = 0b00100000;
    lunMode.stBit.Oe = 1;

    mcTPS55288_0x74.vSetMode(lunMode);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList128ms.vRemove(this->mu8Idx);

    mcTPS55288_0x74.vClearAllRequest();

    vWaitCom();

    mcTPS55288_0x74.vRequest(cTPS55288_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcTPS55288_0x74.vRequest(cTPS55288_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    //mcTPS55288_En.vSet0();

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    if (mcTPS55288_0x74.isDone(cTPS55288_RegisterMap::nSEQ09ReadStatus))
    {
      cSysDPool::mBoard.mcPomoOut1.munStatus = mcTPS55288_0x74.unGetStatus();
    }

    mcTPS55288_0x74.vCmdReadStatus();

    mcTPS55288_0x74.vSetOutput(5000, 500);

    cTPS55288_RegisterMap::tunMode lunMode;

    lunMode.u8Raw = 0b00100000;
    lunMode.stBit.Oe = 1;

    mcTPS55288_0x74.vSetMode(lunMode);

    // return True to signal finished
    return cComponent::bRun();
  };

  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if (mcTPS55288_0x74.isAnySeqPending())
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }

  bool bCheckTps55288Error()
  {
    cSysDPool::mBoard.mcPomoOut1.mu8I2cTps55288Err = 0;
    if (mcTPS55288_0x74.IsError()) cSysDPool::mBoard.mcPomoOut1.mu8I2cTps55288Err |= 1;

    if (cSysDPool::mBoard.mcPomoOut1.mu8I2cTps55288Err) return True;
    return False;
  }
};




