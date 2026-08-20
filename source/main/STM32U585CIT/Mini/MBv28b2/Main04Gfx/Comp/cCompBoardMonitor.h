#pragma once

#include "cSysDPool.h"
#include "ina3221.h"

class cCompBoardMonitor : public cComponent
{
public:

  cINA3221 mcIna0x41;

  u8       mu8TaskCnt;
  u8       mu8SampleCnt;
  bool     mbStartupFinished;

  cCompBoardMonitor()
    : cComponent(cDepTreeCfg::cComp::nBoardMonitor, cDepTreeCfg::cComp::nBoardI2C2),
      mcIna0x41(cSysDPool::mBoard.mcI2c, INA3221_I2C_ADDRESS_CONF_1)
  {
  }

  bool bInit() override
  {
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

    mcIna0x41.mcSEQ02_Init2.vSetCfg(lenConfig);

    // Mit AddNode wird auch die Init Sequenzen requested
    cSysDPool::mBoard.mcI2c->vAddNode(&mcIna0x41);

    vWaitCom();

    mu8TaskCnt = 0;
    mu8SampleCnt = 0;
    cSysDPool::mBoard.mcMonitor.mu8SysVoltOk = 0;
    mbStartupFinished = False;

    // am Anfang mit 1ms sampeln, damit man weniger Zeit braucht, um die Versorgungsspannung zu checken
    // später dann mit 16ms
    cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);
    cComponentList::mcList16ms.vRemove(this->mu8Idx);

    mcIna0x41.vClearAllRequest();

    vWaitCom();

    mcIna0x41.vRequest(cIna3221_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcIna0x41.vRequest(cIna3221_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    if (mcIna0x41.isDone(cIna3221_RegisterMap::nSEQ08ReadAllCh))
    {
      //      INA2 Adr. 0x41
      //        CH1: SysIn
      //        CH2: PomoIn
      //        CH3: PomoOut
      if (!mbStartupFinished)
      {
        cSysDPool::mBoard.mcMonitor.mfSysInV_mV   = mcIna0x41.fGetV_mv(1);
        cSysDPool::mBoard.mcMonitor.mfSysInI_mA   = mcIna0x41.fGetI_mA(1, 0.01f);
        cSysDPool::mBoard.mcMonitor.mfPomoInV_mV  = mcIna0x41.fGetV_mv(2);
        cSysDPool::mBoard.mcMonitor.mfPomoInI_mA  = mcIna0x41.fGetI_mA(2, 0.01f);
        cSysDPool::mBoard.mcMonitor.mfPomoOutV_mV = mcIna0x41.fGetV_mv(3);
        cSysDPool::mBoard.mcMonitor.mfPomoOutI_mA = mcIna0x41.fGetI_mA(3, 0.01f);
      }
      else
      {
        cSysDPool::mBoard.mcMonitor.mfSysInV_mV   += (0.1f * (mcIna0x41.fGetV_mv(1)        - cSysDPool::mBoard.mcMonitor.mfSysInV_mV));
        cSysDPool::mBoard.mcMonitor.mfSysInI_mA   += (0.1f * (mcIna0x41.fGetI_mA(1, 0.01f) - cSysDPool::mBoard.mcMonitor.mfSysInI_mA));
        cSysDPool::mBoard.mcMonitor.mfPomoInV_mV  += (0.1f * (mcIna0x41.fGetV_mv(2)        - cSysDPool::mBoard.mcMonitor.mfPomoInV_mV));
        cSysDPool::mBoard.mcMonitor.mfPomoInI_mA  += (0.1f * (mcIna0x41.fGetI_mA(2, 0.01f) - cSysDPool::mBoard.mcMonitor.mfPomoInI_mA));
        cSysDPool::mBoard.mcMonitor.mfPomoOutV_mV += (0.1f * (mcIna0x41.fGetV_mv(3)        - cSysDPool::mBoard.mcMonitor.mfPomoOutV_mV));
        cSysDPool::mBoard.mcMonitor.mfPomoOutI_mA += (0.1f * (mcIna0x41.fGetI_mA(3, 0.01f) - cSysDPool::mBoard.mcMonitor.mfPomoOutI_mA));
      }

      if (mu8SampleCnt < 250) mu8SampleCnt++;

      if (!mbStartupFinished)
      {
        if (mu8SampleCnt >= 4)
        {
           mbStartupFinished = True;
           // Falls vier Messungen erreicht sind, dann die Samplerate auf 16ms ändern
           cComponentList::mcList16ms.Add(this->mu8Idx);
           cComponentList::mcList1ms.vRemove(this->mu8Idx);
        }
      }
    }

    if (mu8SampleCnt >= 4)
    {
      if ((cSysDPool::mBoard.mcMonitor.mfSysInV_mV < 8000.0f) &&
          (cSysDPool::mBoard.mcMonitor.mfSysInV_mV > 3000.0f))
      {
        cSysDPool::mBoard.mcMonitor.mu8SysVoltOk = 1;
        if (cSysDPool::mBoard.mcMonitor.mfSysInV_mV > 6000.0f)
        {
          cSysDPool::mBoard.mu16LedMode = 0x3333;
        }
        else
        {
          cSysDPool::mBoard.mu16LedMode = 0x00FF;
        }
      }
      else
      {
        cSysDPool::mBoard.mcMonitor.mu8SysVoltOk = 0;
      }
    }
    else
    {
      cSysDPool::mBoard.mcMonitor.mu8SysVoltOk = 0;
    }

    mcIna0x41.vRequest(cIna3221_RegisterMap::nSEQ08ReadAllCh);

    if (mu8TaskCnt < 250) mu8TaskCnt++;
    else
    {
      if (mu8TaskCnt == 250)
      {
        // Spätestens nach 250 Samples die Rate auf 16ms ändern
        // Im Normalfall kommt es aber nicht hierhin.
        cComponentList::mcList16ms.Add(this->mu8Idx);
        cComponentList::mcList1ms.vRemove(this->mu8Idx);
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
      if (mcIna0x41.isAnySeqPending())
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
    if (mcIna0x41.IsError()) cSysDPool::mBoard.mcMonitor.mu8I2cInaErr |= 2;

    if (cSysDPool::mBoard.mcMonitor.mu8I2cInaErr) return True;
    return False;
  }
};




