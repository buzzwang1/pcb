#pragma once

#include "cSysDPool.h"

class cCompComOutCntrl : public cComponent
{
  public:

  cINA3221 mcIna0x40;

  cGpPin mCh1_EN;
  cGpPin mCh2_EN;
  cGpPin mCh3_EN;

  u8       mu8TaskCnt;
  u8       mu8SampleCnt;
  bool     mbStartupFinished;

  cCompComOutCntrl()
    : cComponent(cDepTreeCfg::cComp::nComOutCntrl, cDepTreeCfg::cComp::nBoardI2C2),
      mcIna0x40(cSysDPool::mBoard.mcI2c, INA3221_I2C_ADDRESS_CONF_0),
      mCh1_EN(GPIOB_BASE, 0),
      mCh2_EN(GPIOC_BASE, 2),
      mCh3_EN(GPIOC_BASE, 3)
  {
    cSysDPool::mCom.mpcCh1_EN = &mCh1_EN;
    cSysDPool::mCom.mpcCh2_EN = &mCh2_EN;
    cSysDPool::mCom.mpcCh3_EN = &mCh3_EN;
  }

  bool bInit() override
  {
    mCh1_EN.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mCh2_EN.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mCh3_EN.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    cIna3221_RegisterMap::tunConfig lenConfig =
    {
      .stBit =
      {
         .Mode          = cIna3221_RegisterMap::tenOperatingMode::nMODE_ShuntBus_Cont,
         .ShuntConvTime = cIna3221_RegisterMap::tenConvTime::nCONV_332us,
         .BusConvTime   = cIna3221_RegisterMap::tenConvTime::nCONV_332us,
         .AveragingMode = cIna3221_RegisterMap::tenAvgMode::nAVG_4,
         .Ch3Enable = 1,
         .Ch2Enable = 1,
         .Ch1Enable = 1,
         .Reset = 0
       }
    };

    mcIna0x40.mcSEQ02_Init2.vSetCfg(lenConfig);

    // Mit AddNode wird auch die Init Sequenzen requested
    cSysDPool::mBoard.mcI2c->vAddNode(&mcIna0x40);

    vWaitCom();

    mu8TaskCnt = 0;
    mu8SampleCnt = 0;
    cSysDPool::mBoard.mcMonitor.mu8SysVoltOk = 0;
    mbStartupFinished = False;

    cComponentList::mcList16ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList16ms.vRemove(this->mu8Idx);

    mcIna0x40.vClearAllRequest();

    vWaitCom();

    mcIna0x40.vRequest(cIna3221_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcIna0x40.vRequest(cIna3221_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }



  bool bRun() override
  {
    if (mcIna0x40.isDone(cIna3221_RegisterMap::nSEQ08ReadAllCh))
    {
      //      INA1 Adr. 0x40
      //        CH1: ExCom CH3
      //        CH2: ExCom CH2
      //        CH3: ExCom CH1
      if (!mbStartupFinished)
      {
        cSysDPool::mBoard.mcMonitor.mfComV_mV[0] = mcIna0x40.fGetV_mv(3);
        cSysDPool::mBoard.mcMonitor.mfComI_mA[0] = mcIna0x40.fGetI_mA(3, 0.01f);
        cSysDPool::mBoard.mcMonitor.mfComV_mV[1] = mcIna0x40.fGetV_mv(2);
        cSysDPool::mBoard.mcMonitor.mfComI_mA[1] = mcIna0x40.fGetI_mA(2, 0.01f);
        cSysDPool::mBoard.mcMonitor.mfComV_mV[2] = mcIna0x40.fGetV_mv(1);
        cSysDPool::mBoard.mcMonitor.mfComI_mA[2] = mcIna0x40.fGetI_mA(1, 0.01f);
      }
      else
      {
        cSysDPool::mBoard.mcMonitor.mfComV_mV[0] += (0.1f * (mcIna0x40.fGetV_mv(3)        - cSysDPool::mBoard.mcMonitor.mfComV_mV[0]));
        cSysDPool::mBoard.mcMonitor.mfComI_mA[0] += (0.1f * (mcIna0x40.fGetI_mA(3, 0.01f) - cSysDPool::mBoard.mcMonitor.mfComI_mA[0]));
        cSysDPool::mBoard.mcMonitor.mfComV_mV[1] += (0.1f * (mcIna0x40.fGetV_mv(2)        - cSysDPool::mBoard.mcMonitor.mfComV_mV[1]));
        cSysDPool::mBoard.mcMonitor.mfComI_mA[1] += (0.1f * (mcIna0x40.fGetI_mA(2, 0.01f) - cSysDPool::mBoard.mcMonitor.mfComI_mA[1]));
        cSysDPool::mBoard.mcMonitor.mfComV_mV[2] += (0.1f * (mcIna0x40.fGetV_mv(1)        - cSysDPool::mBoard.mcMonitor.mfComV_mV[2]));
        cSysDPool::mBoard.mcMonitor.mfComI_mA[2] += (0.1f * (mcIna0x40.fGetI_mA(1, 0.01f) - cSysDPool::mBoard.mcMonitor.mfComI_mA[2]));
      }


      if (mu8SampleCnt < 250) mu8SampleCnt++;

      if (!mbStartupFinished)
      {
        if (mu8SampleCnt >= 4)
        {
           mbStartupFinished = True;
        }
      }
    }
    mcIna0x40.vRequest(cIna3221_RegisterMap::nSEQ08ReadAllCh);
    if (mu8TaskCnt < 250) mu8TaskCnt++;
    else
    {
      if (mu8TaskCnt == 250)
      {
        mu8TaskCnt++;
      }
    }

    // return True to signal finished
    return cComponent::bRun();
  };

  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if (mcIna0x40.isAnySeqPending())
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }

  bool bCheckIna3221Error()
  {
    cSysDPool::mBoard.mcMonitor.mu8I2cInaErr = 0;
    if (mcIna0x40.IsError()) cSysDPool::mBoard.mcMonitor.mu8I2cInaErr |= 1;

    if (cSysDPool::mBoard.mcMonitor.mu8I2cInaErr) return True;
    return False;
  }
};




