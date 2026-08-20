#pragma once

#include "cSysDPool.h"

class cCompAddOnCharger : public cComponent
{
  public:

  cGpPin mBQ25798_CE;
  cBQ25798 mcBQ25798;

  cCompAddOnCharger()
    : cComponent(cDepTreeCfg::cComp::nCharger, cDepTreeCfg::cComp::nAddOn),
      mBQ25798_CE(GPIOA_BASE, 7),
      mcBQ25798(cSysDPool::mBoard.mcI2c, BQ25798_I2C_ADDRESS_CONF_0)
  {
    cSysDPool::mBoard.mcCharger.mpcBQ25798 = &mcBQ25798;
  }

  bool bInit() override
  {
    // Charge Enable Low active
    mBQ25798_CE.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
    mBQ25798_CE.vSet0();

    // kurz warten zum Stabilisieren
    vTaskDelay(pdMS_TO_TICKS(1));

    cSysDPool::mBoard.mcI2c->vAddNode(&mcBQ25798);

    vWaitCom();

    mcBQ25798.tunREG0F_Charger_Control_0.stBits.EN_CHG = 1;
    mcBQ25798.tunREG0F_Charger_Control_0.stBits.EN_ICO = 0;
    mcBQ25798.tunREG10_Charger_Control_1.stBits.WD_MODE = 0; // Off
    mcBQ25798.tunREG14_Charger_Control_5.stBits.EN_IBAT = 1; // Strommessung wenn im Entlademodus
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ11_WriteControlCharger);


    mcBQ25798.tunREG18_NTC_Control_1.stBits.TS_IGNORE = 1; // Ignore Temp sensor
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ13_WriteControlTemp);

    mcBQ25798.tunREG2E_ADC_Control.stBits.ADC_EN = 1; // enable
    mcBQ25798.tunREG2E_ADC_Control.stBits.ADC_SAMPLE = 0; // 15bit
    mcBQ25798.tunREG2E_ADC_Control.stBits.ADC_AVG = 1;
    mcBQ25798.tunREG2E_ADC_Control.stBits.ADC_AVG_INIT = 1;
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ14_WriteControlAdc);

    vWaitCom();

    cComponentList::mcList128ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList128ms.vRemove(this->mu8Idx);
    
    mcBQ25798.vClearAllRequest();

    mcBQ25798.tunREG14_Charger_Control_5.stBits.EN_IBAT = 0; // Strommessung wenn im Entlademodus
    mcBQ25798.tunREG2E_ADC_Control.stBits.ADC_EN = 0; // enable

    vWaitCom();
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ05Disable);
    vWaitCom();
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ06DeInit);
    vWaitCom();
    
    mBQ25798_CE.vSet1();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  i16 i16Flip(i16 liValue)
  {
    u8 lHB = liValue >> 8;
    u8 lLB = liValue;
    return (i16)((lLB << 8) + lHB);
  }

  u16 u16Flip(u16 liValue)
  {
    u8 lHB = liValue >> 8;
    u8 lLB = liValue;
    return (u16)((lLB << 8) + lHB);
  }


  bool bRun() override
  {
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ15_ReadStatusCharger);
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ16_ReadStatusFaults);
    mcBQ25798.vRequest(cBQ25798_RegisterMap::nSEQ17_ReadStatusAdc);

    if (mcBQ25798.isDone(cBQ25798_RegisterMap::nSEQ17_ReadStatusAdc))
    {

      cSysDPool::mBoard.mcCharger.mfIBus_mA = i16Flip(mcBQ25798.tunREG31_IBUS_ADC.i16Word);
      cSysDPool::mBoard.mcCharger.mfIBat_mA = i16Flip(mcBQ25798.tunREG33_IBAT_ADC.i16Word);

      cSysDPool::mBoard.mcCharger.mfVBus_mV = u16Flip(mcBQ25798.tunREG35_VBUS_ADC.u16Word);

      cSysDPool::mBoard.mcCharger.mfVAC1_mV = u16Flip(mcBQ25798.tunREG37_VAC1_ADC.u16Word);
      cSysDPool::mBoard.mcCharger.mfVAC2_mV = u16Flip(mcBQ25798.tunREG39_VAC2_ADC.u16Word);

      cSysDPool::mBoard.mcCharger.mfVBat_mV = u16Flip(mcBQ25798.tunREG3B_VBAT_ADC.u16Word);
      cSysDPool::mBoard.mcCharger.mfVSys_mV = u16Flip(mcBQ25798.tunREG3D_VSYS_ADC.u16Word);

      cSysDPool::mBoard.mcCharger.mfTSens_C = u16Flip(mcBQ25798.tunREG3F_TS_ADC.u16Word) * 0.0976563f;
      cSysDPool::mBoard.mcCharger.mfTDie_C  = i16Flip(mcBQ25798.tunREG41_TDIE_ADC.i16Word) * 0.5;
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
      if (mcBQ25798.isAnySeqPending())
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }

  bool bCheckI2cError()
  {
    cSysDPool::mBoard.mcCharger.mu8I2cErr = 0;
    if (mcBQ25798.IsError()) cSysDPool::mBoard.mcCharger.mu8I2cErr |= 1;

    if (cSysDPool::mBoard.mcCharger.mu8I2cErr) return True;
    return False;
  }
};




