
#ifndef __SYS_PKG_BOARD_H__
#define __SYS_PKG_BOARD_H__

#include "TypeDef.h"
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "cPidFloat.h"
#include "cPidFloat.h"
#include "cStrT.h"

#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dac.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

#include "Eep.h"
#include "ina3221.h"
#include "MP8859.h"

#include "RomConst.h"

class cStatusLedMode
{
  public:
  typedef enum
  {
    nStToggle100ms,
    nStToggle200ms,
    nStToggle500ms,
    nStToggle1000ms,
    nStPuls500ms,
    nStPuls1000ms,
    nStPuls5000ms,
    nStSinglePuls1000ms,
    nStSinglePuls2000ms,
    nStSinglePuls3000ms,
    nStOn,
    nStOff,
  }tenMode;
};

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class cStatusLed : public LED<mpstPort, mui16Pin>, public cStatusLedMode
{
  public:
  tenMode mSm;
  u8      mCounter;

  cStatusLed(bool lbInverse);
  void vSetMode(cStatusLedMode::tenMode lSm);
  void vTick100ms();
};


class cDigiPower
{
  public:

  enum
  {
    nPin1   = 10,
    nPinLed = 6
  };

  //Digi Power
  cGpPin lcDigi1;
  cStatusLed<GPIOA_BASE, nPinLed> lcDigiLed;

  cDigiPower(u8 lu8InitState = 0);

  void vSetLedMode(cStatusLedMode::tenMode lSm);
  void vLedOn3000ms();
  void vLedOff();
  void vPowerOn();
  void vPowerOff();
  void vTick_100ms();
};

class cAnalogConvert
{
public:

  static u16 u16GetValue(u16 lu16Cnt, const u16* lu16CalDigi, const u16* lu16CalValue, u16 lu16Digi)
  {
    u16 lu16Digi1, lu16Digi2;
    u16 lu16Value1, lu16Value2;
    float lfZwerg;

    //Nächaten Punkt in der Tabelle suchen
    u16 lu16t = lu16Cnt / 2;


    while (1)
    {
      if ((lu16Digi >= lu16CalDigi[lu16t]) && (lu16Digi < lu16CalDigi[lu16t + 1]))
      {
        break;
      }
      else
        if (lu16Digi >= lu16CalDigi[lu16t])
        {
          if (lu16t < (lu16Cnt - 1))
          {
            lu16t++;
          }
          else
          {
            break;
          }
        }
        else // (lu16Digi < lu16CalDigi[lu16t])
        {
          if (lu16t > 0)
          {
            lu16t--;
          }
          else
          {
            break;
          }
        }
    }

    lu16Digi1 = lu16CalDigi[lu16t];  lu16Digi2 = lu16CalDigi[lu16t + 1];
    lu16Value1 = lu16CalValue[lu16t]; lu16Value2 = lu16CalValue[lu16t + 1];

    lfZwerg = (float)(lu16Value2 - lu16Value1) / (float)(lu16Digi2 - lu16Digi1);
    lfZwerg *= (float)(lu16Digi - lu16Digi1);
    if (lfZwerg > 0) lfZwerg += 0.5f;
    else lfZwerg -= 0.5f;
    return (u16)((i16)lfZwerg + lu16Value1);
  }

  static u16 u16GetDigi(u16 lu16Cnt, const u16* lu16CalDigi, const u16* lu16CalValue, u16 lu16Value)
  {
    u16 lu16Digi1, lu16Digi2;
    u16 lu16Value1, lu16Value2;
    float lfZwerg;

    //Nächaten Punkt in der Tabelle suchen
    u16 lu16t = lu16Cnt / 2;


    while (1)
    {
      if ((lu16Value >= lu16CalValue[lu16t]) && (lu16Value < lu16CalValue[lu16t + 1]))
      {
        break;
      }
      else
        if (lu16Value >= lu16CalValue[lu16t])
        {
          if (lu16t < (lu16Cnt - 1))
          {
            lu16t++;
          }
          else
          {
            break;
          }
        }
        else // (lu16Digi < lu16CalValue[lu16t])
        {
          if (lu16t > 0)
          {
            lu16t--;
          }
          else
          {
            break;
          }
        }
    }

    lu16Digi1 = lu16CalDigi[lu16t];  lu16Digi2 = lu16CalDigi[lu16t + 1];
    lu16Value1 = lu16CalValue[lu16t]; lu16Value2 = lu16CalValue[lu16t + 1];

    lfZwerg = (float)(lu16Digi2 - lu16Digi1) / (float)(lu16Value2 - lu16Value1);
    lfZwerg *= (float)(lu16Value - lu16Value1);
    if (lfZwerg > 0) lfZwerg += 0.5f;
    else lfZwerg -= 0.5f;
    return (u16)((i32)lfZwerg + lu16Digi1);
  }
};


class cVoltageConverter
{
public:

  // Modultypen :
  // 0: Brücke
  // 1: Ein / Aus - Schalter
  // 2: MT3608 Modul : DAC für Spannung + Brücke Strombegrenzung
  // 3: MT3608 Modul1 : DAC für Spannung + Enable
  // 4: MP2315 Modul : DAC für Spannung + Enable
  // 5: MP2315 Modul1 : DAC für Spannung + Enable
  // 6: MP8859 Modul : I2C für Spannung + Brücke Strombegrenzung
  // 7: TP4056 Modul : DAC für Strombegrenzung + Enable
  u8  mType;
  u16 mVoltage_mV;
  u16 mVoltage_mVmax;
  u16 mVoltage_mVmin;
  u16 mCurrentLimit_mV;
  bool mbEnable;

  u8  mu8Slot;

  float         mfPidSetVolt;

  float         mfPidSetVoltIn;
  float         mfPidSetVoltOut;
  float         mfPidSetCurrIn;
  float         mfPidSetCurrOut;


  cFloatLowPass mcLpInCurr;
  cFloatLowPass mcLpInVolt;
  cFloatLowPass mcLpOutCurr;
  cFloatLowPass mcLpOutVolt;

  cFloatPid     mcPidInCurr;
  cFloatPid     mcPidInVolt;
  cFloatPid     mcPidOutCurr;
  cFloatPid     mcPidOutVolt;
  cFloatPid*    mcPid[4];

  float         mfPidInCurrOutputRel;
  float         mfPidInVoltOutputRel;
  float         mfPidOutCurrOutputRel;
  float         mfPidOutVoltOutputRel;

  float         mfPidInCurrOutput;
  float         mfPidInVoltOutput;
  float         mfPidOutCurrOutput;
  float         mfPidOutVoltOutput;

  // Operations Modus :
  //   0: Nichts
  //   1: SW Konstant I
  //   2: SW Konstant I mit Spannungbegrenzung
  //   3: SW Konstant U
  //   4: SW Konstant U mit Strombegrenzung
  //  11: HW Konstant I
  //  12: HW Konstant I mit Spannungbegrenzung
  //  13: HW Konstant U
  //  14: HW Konstant U mit Strombegrenzung

  u8            mu8PidModeIn;
  u8            mu8PidModeInAlt;
  u8            mu8PidModeOut;
  u8            mu8PidModeOutAlt;
  float         mfPidMinVolt;
  float         mfPidMaxVolt;
  float         mfPidMaxCurr;

  cVoltageConverter()
    : mcLpInCurr(0.5f),
      mcLpInVolt(0.5f),
      mcLpOutCurr(0.5f),
      mcLpOutVolt(0.5f),
      mcPidInCurr(0.5f),
      mcPidInVolt(0.5f),
      mcPidOutCurr(0.5f),
      mcPidOutVolt(0.5f)
  {
    mVoltage_mV = 0;
    mVoltage_mVmax = 18000;
    mVoltage_mVmin = 2500;
    mCurrentLimit_mV = 50;
    mbEnable = False;

    mcPid[0] = &mcPidInCurr;
    mcPid[1] = &mcPidInVolt;
    mcPid[2] = &mcPidOutCurr;
    mcPid[3] = &mcPidOutVolt;

    for (u8 lu8t = 0; lu8t < 4; lu8t++)
    {
      mcPid[lu8t]->mfKp = 1.0f;
      mcPid[lu8t]->mfKi = 0.0f;
      mcPid[lu8t]->mfKd = 0.0f;
      mcPid[lu8t]->mfOutputLimit = 100.0f;
    }

    mfPidSetVoltIn  = 5000.0f;
    mfPidSetVoltOut = 5000.0f;
    mfPidSetCurrIn  = 200.0f;
    mfPidSetCurrOut = 100.0f;

    mfPidInCurrOutput = 0.0f;
    mfPidInVoltOutput = 0.0f;
    mfPidOutCurrOutput = 0.0f;
    mfPidOutVoltOutput = 0.0f;

    mu8PidModeIn  = 255;
    mu8PidModeInAlt = mu8PidModeIn;
    mu8PidModeOut = 255;
    mu8PidModeOutAlt = mu8PidModeOut;
    mfPidMaxCurr = 500.0f;   // [mA]

    mfPidSetVolt = 10000.0f;
  }

  virtual void vSetPidOutputVoltage(float lfValue)               { mfPidSetVoltOut = lfValue;}
  virtual void vSetPidOutputCurrentLimit(float lfValue)          { mfPidSetCurrOut = lfValue;}
  virtual void vSetPidOutput(float lfVolt, float lfCurrentLimit) { vSetPidOutputVoltage(lfVolt); vSetPidOutputCurrentLimit(lfCurrentLimit); }
  virtual void vSetPidInputVoltage(float lfValue)                { mfPidSetVoltIn  = lfValue;}
  virtual void vSetPidInputCurrentLimit(float lfValue)           { mfPidSetCurrIn  = lfValue;}
  virtual void vSetPidInput(float lfVolt, float lfCurrentLimit)  { vSetPidInputVoltage(lfVolt); vSetPidInputCurrentLimit(lfCurrentLimit); }

  virtual void vSetHwOutputVoltage(i16 li16Value) = 0;
  virtual void vSetHwOutputCurrentLimit(i16 li16Value) = 0;
  virtual void vSetHwOutput(i16 li16Volt, i16 li16CurrentLimit)  { vSetHwOutputCurrentLimit(li16CurrentLimit); vSetHwOutputVoltage(li16Volt);}
  virtual void vSetHwInputVoltage(i16 li16Value) = 0;
  virtual void vSetHwInputCurrentLimit(i16 li16Value) = 0;
  virtual void vSetHwInput(i16 li16Volt, i16 li16CurrentLimit)   { vSetHwInputCurrentLimit(li16CurrentLimit); vSetHwInputVoltage(li16Volt); }

  virtual bool bHasMPPT() = 0;
  virtual bool bHasCurrentLimit() = 0;


  u16  u16GetVoltage()
  {
    return mVoltage_mV;
  }

  bool isEnable()
  {
    return mbEnable;
  }

  virtual void vEnable()
  {
    if (mu8Slot == 0)
    {
      GPIOA->BSRR = (1 << 7);
    }
    else
    {
      GPIOB->BSRR = (1 << 8);
    }

    mbEnable = True;
  }

  virtual void vDisable()
  {
    if (mu8Slot == 0)
    {
      GPIOA->BRR = (1 << 7);
    }
    else
    {
      GPIOB->BRR = (1 << 8);
    }

    mbEnable = False;
  }

  virtual void vDoProcess(float lfVoltIn, float lfCurrIn, float lfVoltOut, float lfCurrOut)
  {
    float lfError = 0.0f;
    mfPidSetVolt = 999999.0f;
    bool lbSet = False;
    //bool lbModeSwitch;

    if (!mbEnable) return;

    //lbModeSwitch = (mu8PidModeInAlt != mu8PidModeIn) || (mu8PidModeOutAlt != mu8PidModeOut);

    switch (mu8PidModeIn) // Soll - Ist
    {
      case 1: // Konstant Strom Eingang
        lfError = mfPidSetCurrIn - lfCurrIn;
        mfPidInCurrOutputRel = mcPidInCurr.fDoProcess(lfError);
        mfPidInCurrOutput += mfPidInCurrOutputRel;
        if (mfPidInCurrOutput < mVoltage_mVmin) mfPidInCurrOutput = mVoltage_mVmin;
        if (mfPidInCurrOutput > mVoltage_mVmax) mfPidInCurrOutput = mVoltage_mVmax;
        mfPidSetVolt = mfPidInCurrOutput;
        lbSet = True;
        break;
      case 3: // Konstant Spannung Eingang
        lfError = mfPidSetVoltIn - lfVoltIn;
        mfPidInVoltOutputRel = mcPidInVolt.fDoProcess(lfError);
        mfPidInVoltOutput += mfPidInVoltOutputRel;
        if (mfPidInVoltOutput < mVoltage_mVmin) mfPidInVoltOutput = mVoltage_mVmin;
        if (mfPidInVoltOutput > mVoltage_mVmax) mfPidInVoltOutput = mVoltage_mVmax;
        mfPidSetVolt = mfPidInVoltOutput;
        lbSet = True;
        break;
    }

    switch (mu8PidModeOut) // Soll - Ist
    {
      case 1: // SW: Konstant Strom Ausgang
        lfError = mfPidSetCurrOut - lfCurrOut;
        mfPidOutCurrOutputRel = mcPidOutCurr.fDoProcess(lfError);
        mfPidOutCurrOutput += mfPidOutCurrOutputRel;
        if (mfPidOutCurrOutput < mVoltage_mVmin) mfPidOutCurrOutput = mVoltage_mVmin;
        if (mfPidOutCurrOutput > mVoltage_mVmax) mfPidOutCurrOutput = mVoltage_mVmax;
        mfPidSetVolt = mfPidOutCurrOutput;
        lbSet = True;
        break;
      case 3: // SW: Konstant Spannungausgang Ausgang
        lfError = mfPidSetVoltOut - lfVoltOut;
        mfPidOutVoltOutputRel = mcPidOutVolt.fDoProcess(lfError);
        mfPidOutVoltOutput += mfPidOutVoltOutputRel;
        if (mfPidOutVoltOutput < mVoltage_mVmin) mfPidOutVoltOutput = mVoltage_mVmin;
        if (mfPidOutVoltOutput > mVoltage_mVmax) mfPidOutVoltOutput = mVoltage_mVmax;
        mfPidSetVolt = mfPidOutVoltOutput;
        lbSet = True;
        break;
      case 4: // SW: Konstant Spannung mit Strombegrenzung
        {
          lfError = mfPidSetCurrOut - lfCurrOut;
          mfPidOutCurrOutputRel = mcPidOutCurr.fDoProcess(lfError);
          mfPidOutCurrOutput += mfPidOutCurrOutputRel;
          if (mfPidOutCurrOutput < mVoltage_mVmin) mfPidOutCurrOutput = mVoltage_mVmin;
          if (mfPidOutCurrOutput > mVoltage_mVmax) mfPidOutCurrOutput = mVoltage_mVmax;

          lfError = mfPidSetVoltOut - lfVoltOut;
          mfPidOutVoltOutputRel = mcPidOutVolt.fDoProcess(lfError);
          mfPidOutVoltOutput += mfPidOutVoltOutputRel;
          if (mfPidOutVoltOutput < mVoltage_mVmin) mfPidOutVoltOutput = mVoltage_mVmin;
          if (mfPidOutVoltOutput > mVoltage_mVmax) mfPidOutVoltOutput = mVoltage_mVmax;

          mfPidSetVolt = mfPidOutCurrOutput;
          if (mfPidOutVoltOutput < mfPidSetVolt) mfPidSetVolt = mfPidOutVoltOutput;
          lbSet = True;
        }
        break;
      case 14: // HW: Konstant Spannung mit Strombegrenzung
          vSetHwOutput(mfPidSetVoltOut, mfPidSetCurrOut);
          return;
        break;
    }

    switch (mu8PidModeIn) // Soll - Ist
    {
      case 1: // Konstant Strom Eingang
        if (mfPidInCurrOutput < mfPidSetVolt) mfPidSetVolt = mfPidInCurrOutput;
        break;
      case 3: // Konstant Spannung Eingang
        if (mfPidInVoltOutput < mfPidSetVolt) mfPidSetVolt = mfPidInVoltOutput;
        break;
    }

    mfPidInCurrOutput = mfPidSetVolt;
    mfPidInVoltOutput = mfPidSetVolt;
    mfPidOutCurrOutput = mfPidSetVolt;
    mfPidOutVoltOutput = mfPidSetVolt;

    mu8PidModeInAlt = mu8PidModeIn;
    mu8PidModeOutAlt = mu8PidModeOut;


    if (lbSet)
    {
      vSetHwOutput(mfPidSetVolt, 2000);
    }
    else
    {
      vSetHwOutput(mVoltage_mVmin, 2000);
    }
  }

};

class cPmMt3608 : public cVoltageConverter
{
public:
  //Slot0:
  // PA4 DAC1 Channel1
  // PA7 Enable

  //Slot1:
  // PA5  DAC1 Channel2
  // PB08 Enable

  u32 mu32Channel;

  cPmMt3608(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    mu8Slot = lu8Slot;

    LL_DAC_InitTypeDef DAC_InitStructure;

    if (lu8Slot == 0)
    {
      mu32Channel = LL_DAC_CHANNEL_1;
      cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
      cGpPin mEnable(GPIOA_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    }
    else
    {
      mu32Channel = LL_DAC_CHANNEL_2;
      cGpPin mDac(GPIOA_BASE, 5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
      cGpPin mEnable(GPIOB_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    }

    /* DAC channel1 Configuration */
    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE; // Enable, sonst bricht die ausgangsspannung zusammen
    LL_DAC_Init(DAC1, mu32Channel, &DAC_InitStructure);
    mfPidSetVolt = lu16Voltage_mV;
    vSetPidOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
    vSetHwOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
    LL_DAC_Enable(DAC1, mu32Channel);
  }

  void vSetHwOutputVoltage(i16 li16Value) override
  {
    if (mVoltage_mV != li16Value)
    {
      mVoltage_mV = li16Value;
      LL_DAC_ConvertData12RightAligned(DAC1, mu32Channel, u16VoltToDigit(mVoltage_mV));
    }
  }
  void vSetHwOutputCurrentLimit(i16 li16Value) override
  {
    mCurrentLimit_mV = li16Value;
  }
  void vSetHwInputVoltage(i16 li16Value) override
  {
    UNUSED(li16Value);
  }
  void vSetHwInputCurrentLimit(i16 li16Value) override
  {
    UNUSED(li16Value);
  }

  u16 u16VoltToDigit(u16 lu16Value)
  {
    if (mu8Slot == 0)
    {
      lu16Value = cAnalogConvert::u16GetDigi(17, u16GetRomConstPowerInDac(), u16GetRomConstPowerInVout(), lu16Value);
    }
    else
    {
      // Hier müsste später PowerOut verwendet werden
      lu16Value = cAnalogConvert::u16GetDigi(17, u16GetRomConstPowerInDac(), u16GetRomConstPowerInVout(), lu16Value);
    }

    if (lu16Value > 4095) lu16Value = 4095;
    return (u16)lu16Value;
  }

  bool bHasMPPT()         {return False;}
  bool bHasCurrentLimit() {return False;}
};

class cPmSwitch : public cVoltageConverter
{
public:
  //Slot0:
  // PA7 Enable

  //Slot1:
  // PB08 Enable

  cPmSwitch(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    mu8Slot = lu8Slot;

    if (lu8Slot == 0)
    {
      cGpPin mEnable(GPIOA_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    }
    else
    {
      cGpPin mEnable(GPIOB_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    }

    vSetPidOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
  }

  void vSetHwOutputVoltage(i16 li16Value) override
  {
    mVoltage_mV = li16Value;
  }
  void vSetHwOutputCurrentLimit(i16 li16Value) override
  {
    mCurrentLimit_mV = li16Value;
  }
  void vSetHwInputVoltage(i16 li16Value) override
  {
    UNUSED(li16Value);
  }
  void vSetHwInputCurrentLimit(i16 li16Value) override
  {
    UNUSED(li16Value);
  }


  bool bHasMPPT() { return False; }
  bool bHasCurrentLimit() { return False; }
};


class cPmMp8859 : public cVoltageConverter
{
public:
  cI2cMaster* mcI2C;
  cMP8859*    mcMP8859;
  bool        mbEnableRem;
  //Slot0:
  // I2C Adr

  //Slot1:
  // I2C Adr xxx


  cPmMp8859(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot, cI2cMaster* lcI2C)
  {
    mu8Slot = lu8Slot;
    mcI2C   = lcI2C;

    if (lu8Slot == 0)
    {
      mcMP8859 = new cMP8859(mcI2C, MP8859_I2C_ADDRESS_CONF_0);
    }
    else
    {
      mcMP8859 = new cMP8859(mcI2C, MP8859_I2C_ADDRESS_CONF_1);
    }

    mfPidSetVolt = lu16Voltage_mV;
    vSetPidOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
  }


  void vSetHwOutputVoltage(i16 li16Value) override
  {
    mVoltage_mV = li16Value;
    mcMP8859->i8SetVoltage_mV(mVoltage_mV, True);

    u8 lu8Ctrl1;
    if (mbEnableRem)
    {
      lu8Ctrl1 = mcMP8859->u8GetReg(MP8859_CTL1);
      lu8Ctrl1 |= 128;
      mcMP8859->vSetReg(MP8859_CTL1, lu8Ctrl1);
      mbEnableRem = False;
    }
  }
  void vSetHwOutputCurrentLimit(i16 li16Value) override
  {
    mCurrentLimit_mV = li16Value;
    mcMP8859->i8SetCurrent_mA(mCurrentLimit_mV, True);
  }
  void vSetHwInputVoltage(i16 li16Value) override
  {
    UNUSED(li16Value);
  }
  void vSetHwInputCurrentLimit(i16 li16Value) override
  {
    UNUSED(li16Value);
  }



  bool bHasMPPT() { return False; }
  bool bHasCurrentLimit() { return True; }

  void vEnable() override
  {
    if (!mbEnable)
    {
      mbEnableRem = True;
      mbEnable = True;
    }
  }

  void vDisable() override
  {
    u8 lu8Ctrl1;
    if (mbEnable)
    {
      lu8Ctrl1 = mcMP8859->u8GetReg(MP8859_CTL1);
      lu8Ctrl1 &= 0x7F;
      mcMP8859->vSetReg(MP8859_CTL1, lu8Ctrl1);
      mbEnable = False;
    }
  }
};



class cPmTp4056 : public cVoltageConverter
{
  public:
  //Slot0:
  // PA4 DAC1 Channel1
  // PA7 Enable

  u32 mu32Channel;

  cPmTp4056(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    mu8Slot = lu8Slot;

    LL_DAC_InitTypeDef DAC_InitStructure;

    if (lu8Slot == 0)
    {
      mu32Channel = LL_DAC_CHANNEL_1;
      cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
      cGpPin mEnable(GPIOA_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

      /* DAC channel1 Configuration */
      LL_DAC_StructInit(&DAC_InitStructure);
      DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE; // Enable, sonst bricht die ausgangsspannung zusammen
      LL_DAC_Init(DAC1, mu32Channel, &DAC_InitStructure);
      mfPidSetVolt = lu16Voltage_mV;
      vSetPidOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
      vSetHwOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
      LL_DAC_Enable(DAC1, mu32Channel);
    }
  }

  void vSetHwOutputVoltage(i16 li16Value) override
  {
    mVoltage_mV = li16Value;
  }
  void vSetHwOutputCurrentLimit(i16 li16Value) override
  {
    mCurrentLimit_mV = li16Value;
  }

  void vSetHwInputVoltage(i16 li16Value) override
  {
    UNUSED(li16Value);
  }
  void vSetHwInputCurrentLimit(i16 li16Value) override
  {
    UNUSED(li16Value);
  }


  bool bHasMPPT() { return False; }
  bool bHasCurrentLimit() { return False; }
};




class cPmDummy : public cVoltageConverter
{
public:
  cPmDummy(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    mu8Slot = lu8Slot;
    mfPidSetVolt = lu16Voltage_mV;
    vSetPidOutput(lu16Voltage_mV, lu16CurrentLimit_mA);
  }

  void vSetHwOutputVoltage(i16 li16Value) override
  {
    mVoltage_mV = li16Value;
  }
  void vSetHwOutputCurrentLimit(i16 li16Value) override
  {
    mCurrentLimit_mV = li16Value;
  }
  void vSetHwInputVoltage(i16 li16Value) override
  {
    UNUSED(li16Value);
  }
  void vSetHwInputCurrentLimit(i16 li16Value) override
  {
    UNUSED(li16Value);
  }

  bool bHasMPPT() { return False; }
  bool bHasCurrentLimit() { return False; }

  void vEnable() override
  {
    mbEnable = True;
  }

  void vDisable() override
  {
    mbEnable = False;
  }
};


class cLipoMon
{
public:

  class cStateHandler
  {
    public:
    u8 mu8State;
    u8 mu8StateNew;
    u16 mu16Timer_ms;

    cStateHandler()
    {
      vSet(0);
    }

    void vSet(u8 lu8State)
    {
      mu8StateNew = mu8State = lu8State;
      mu16Timer_ms = u16GetRomConstChrgTimeOut1();
    }

    u8 u8Get()
    {
      return mu8State;
    }

    bool isSet()
    {
      return (mu8State > 0);
    }

    void vSetNew(u8 lu8StateNew)
    {
      if (mu8StateNew != lu8StateNew)
      {
        mu16Timer_ms = 0;
        mu8StateNew = lu8StateNew;
      }
      vProcess_10ms();
    }

    void vProcess_10ms()
    {
      if (mu16Timer_ms < u16GetRomConstChrgTimeOut1()) // for >2s
      {
        mu16Timer_ms += 10;
        if (mu16Timer_ms >= u16GetRomConstChrgTimeOut1()) // for >2s
        {
          mu8State = mu8StateNew;
        }
      }
    }
  };

  typedef enum
  {
    enS1_Lipo = 0,

    enS_Balancer_Lipo1,
    enS_Balancer_Lipo2,
    enS_Balancer_Lipo3,
    enS_Balancer_Lipo4,

    enINA_P1 = 0,
    enINA_P2 = 1,
  }tenLipoMonGeneral;

  typedef enum
  {
    enSmStNotCharging = 0,
    enSmStNotCharging_Error_LowInputVoltage,
    enSmStNotCharging_Error_CellsDefect,
    enSmStNotCharging_Error_Ina,
    enSmStNotCharging_PowerDown,
    enSmStChargingSave,
    enSmStChargingNormal,
    enSmStCharged
  }tenLipoMonChargerStates;


  typedef struct
  {
    u8 SysOff : 1;
    u8 UV : 1;
    u8 OV : 1;
    u8 Defect : 1;
  }tstCellState;

  typedef union
  {
    tstCellState Bits;
    u8           u8Data;
  }tunCellState;

  typedef enum
  {
    enSmStOff = 0,
    enSmStOn
  }tenLipoMonS_States;

  typedef enum
  {
    enSmEvInit = 0,
    enSmEvOff,
    enSmEvOn,
    enSmEvAuto
  }tenLipoMonS_Events;

  typedef enum
  {
    enLipoUThres1 = 3200,
    enLipoUThres2 = 3500,
    enLipoUThres3 = 3800,
    enLipoUThres4 = 4200, // 16600 * 4 =

    //Strom in mA
    enLipoISave = 25,
    enLipoI100mA = 100,
    enLipoICharge = enLipoI100mA,

    enPowerInIMax = 500,

  }tenLipoMonS_Const;


  cI2cMaster* mcI2C;

  cINA3221* mcP[2];
  u8 mu8InaErr;

  // ---- Seit v10
  // Bei 1x INA
  // mcP[0] = Adr 0x40
  //  INA13: Eingang  (Vor Charger)
  //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
  //  INA11: Bat1


  // ---- Vor v10
  // Bei 1x INA
  // mcP[0] = Adr 0x40
  //  v9B2
  //    INA11: Eingang  (Vor Charger)
  //    INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
  //    INA13: Bat1
  //  
  //  v10B2
  //    INA13: Eingang  (Vor Charger)
  //    INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
  //    INA11: Bat1
  //
  // Bei 2x INA
  // mcP[0] = Adr 0x40
  //  INA11: Eingang  (Vor Charger)
  //  INA12: Batterie/System (Nach Charger); Spannung Bat4 + Spannungsabfall am Widerstand über Bat 4
  //  INA13: Ausgang (Nach Ausgangs Konverter)
  //
  // mcP[1] = Adr 0x41
  //  INA21: Spannung an Bat3
  //  INA22: Spannung an Bat2
  //  INA23: Spannung an Bat1

  u8     mu8ForceDirectRead;
  int32  mi32VBus[6];
  float  mfVBusAvg[6];
  int32  mi32IShunt[6];
  float  mfIShuntAvg[6];
  int32  mi32VShunt[6];
  float  mfVShuntAvg[6];

  float  mfVBat[4];

  uint8 mui8P2Mode;

  u16            mBalancingActive;
  tunCellState   munCellState[4];
  
  cStateHandler  mcCellStHdlSysOff[4];
  cStateHandler  mcCellStHdlUV[4];
  cStateHandler  mcCellStHdlOV[4];
  cStateHandler  mcCellStHdlDefect[4];


  tenLipoMonS_States      lenSmS[5];
  tenLipoMonChargerStates lenSmLipo;
  tenLipoMonChargerStates lenSmLipoPrev;

  int32  mi32LipoChargeSum_10ms;
  uint32 mui32LipoChargeTime_10ms;

  uint32   mui32LipoChargeCount1;
  uint32   mui32LipoChargeCount2;
  uint32   mui32LipoLoadCount;

  uint32   mu32ChargerReadyCnt;

  i16 mi16ChargerVoltCell;
  i16 mi16ChargerCurrent;
  i16 mi16ChargerCurrentSave;

  int32    mi32LipoCap;
  int32    mi32LipoCapMax;

  u16   mu16Startup_Delay;

  bool  mbPowerDown;
  bool  mbPowerDownEnter;

  u8    mTestMode;
  u8    mCellCnt;
  u8    mInaCnt;
  u8    mBoard;  // 0 = 2Bat Board, 1 = 4 Bat Board

  tcGpPin<GPIOC_BASE, 13> mS_Lipo;

  cGpPin* mS_Balancer[4];

  cVoltageConverter* mcPowerIn;
  cVoltageConverter* mcPowerOut;


  cVoltageConverter* cGetVoltageConverter(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8PmType, u8 lu8Slot)
  {
    cVoltageConverter* cVc = null;

    // 0: Brücke
    // 1: Ein / Aus - Schalter
    // 2: MT3608 Modul : DAC für Spannung + Brücke Strombegrenzung
    // 3: MT3608 Modul1 : DAC für Spannung + Enable
    // 4: MP2315 Modul : DAC für Spannung + Enable
    // 5: MP2315 Modul1 : DAC für Spannung + Enable
    // 6: MP8859 Modul : I2C für Spannung + Brücke Strombegrenzung
    // 7: TP4056 Modul : DAC für Strombegrenzung + Enable
    switch (lu8PmType)
    {
      case 1:
        cVc = new cPmSwitch(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      case 2:
        cVc = new cPmMt3608(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      case 3:
        cVc = new cPmMt3608(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      case 4:
        cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      case 5:
        cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      case 6:
        cVc = new cPmMp8859(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot, mcI2C);
        break;
      case 7:
        cVc = new cPmTp4056(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
      default:
        cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
        break;
    }
    cVc->mType = lu8PmType;
    return cVc;
  }


  cLipoMon(cI2cMaster* lcI2C)
    : mS_Lipo(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1)
  {
    mcI2C = lcI2C;

    mCellCnt = u8GetRomConstChrgCellCount();
    mBoard   = u8GetRomConstBoardType();
    if (mBoard == 0)
    {
      mInaCnt = 1;
      // mS_Balancer[0]: v7->PA5 und v8->PA15
      mS_Balancer[0] = new cGpPin(GPIOA_BASE, 15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mS_Balancer[1] = new cGpPin(GPIOB_BASE,  2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mS_Balancer[2] = null;
      mS_Balancer[3] = null;
      mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 50, 50, 50);
      mcP[0]->i8Setup();
      mcPowerIn  = cGetVoltageConverter(u16GetRomConstModulInSetVoltOutput(),  u16GetRomConstModulInSetCurrOutput(),  u8GetRomConstModulInType(), 0);
      mcPowerOut = cGetVoltageConverter(u16GetRomConstModulOutSetVoltOutput(), u16GetRomConstModulOutSetCurrOutput(),                          1, 1);
    }
    else
    {
      mInaCnt = 2;
      mS_Balancer[0] = new cGpPin(GPIOA_BASE, 15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mS_Balancer[1] = new cGpPin(GPIOB_BASE, 2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mS_Balancer[2] = new cGpPin(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mS_Balancer[3] = new cGpPin(GPIOB_BASE, 0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
      mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 50, 50, 50);
      mcP[1] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_1, 50, 50, 50);
      mcP[0]->i8Setup();
      mcP[1]->i8Setup();
      mcPowerIn  = cGetVoltageConverter(u16GetRomConstModulInSetVoltOutput(),  u16GetRomConstModulInSetCurrOutput(),  u8GetRomConstModulInType(),  0);
      mcPowerOut = cGetVoltageConverter(u16GetRomConstModulOutSetVoltOutput(), u16GetRomConstModulOutSetCurrOutput(), u8GetRomConstModulOutType(), 1);
    }
    mu8InaErr = 0;

    lenSmS[enS1_Lipo] = enSmStOn;
    lenSmS[enS_Balancer_Lipo1] = enSmStOff;
    lenSmS[enS_Balancer_Lipo2] = enSmStOff;
    lenSmS[enS_Balancer_Lipo3] = enSmStOff;
    lenSmS[enS_Balancer_Lipo4] = enSmStOff;

    u8 lui8t;
    for (lui8t = 0; lui8t < 6; lui8t++)
    {
      mi32VBus[lui8t] = 0;
      mi32IShunt[lui8t] = 0;
      mi32VShunt[lui8t] = 0;
    }

    for (lui8t = 0; lui8t < 4; lui8t++)
    {
      munCellState[lui8t].u8Data = 0;
    }


    mu16Startup_Delay = 0; //[ms]
    mTestMode         = 0;

    lenSmLipo = enSmStNotCharging;
    lenSmLipoPrev = lenSmLipo;

    mi32LipoChargeSum_10ms = 0;
    mui32LipoChargeTime_10ms = 0;

    mui32LipoChargeCount1 = 0;
    mui32LipoChargeCount2 = 0;
    mui32LipoLoadCount = 0;
    mu32ChargerReadyCnt = 0;

    mi32LipoCap = 0;
    mi32LipoCapMax = 0;

    mui8P2Mode = 0;
    mBalancingActive = 0;

    mbPowerDown = False;
    mbPowerDownEnter = False;
    mu8ForceDirectRead = True;

    mi16ChargerVoltCell = u16GetRomConstChrgMaxVoltCell();
    mi16ChargerCurrent  = u16GetRomConstChrgMaxCurrentBat();
    mi16ChargerCurrentSave = u16GetRomConstChrgCurrentSave();

    mcPowerIn->mType                   = u8GetRomConstModulInType();

    mcPowerIn->mVoltage_mVmax = u16GetRomConstModulInVoltMax();
    mcPowerIn->mVoltage_mVmin = u16GetRomConstModulInVoltMinSupply();

    mcPowerIn->mcLpInCurr.mFactor            = fGetRomConstModulInInCurrLpIn();
    mcPowerIn->mcPidInCurr.mfKp              = fGetRomConstModulInInCurrKp();
    mcPowerIn->mcPidInCurr.mfKi              = fGetRomConstModulInInCurrKi();
    mcPowerIn->mcPidInCurr.mfKd              = fGetRomConstModulInInCurrKd();
    mcPowerIn->mcPidInCurr.mfOutputLimit     = fGetRomConstModulInInCurrKiCut();
    mcPowerIn->mcPidInCurr.mLpOutput.mFactor = fGetRomConstModulInInCurrLpOut();

    mcPowerIn->mcLpInVolt.mFactor            = fGetRomConstModulInInVoltLpIn();
    mcPowerIn->mcPidInVolt.mfKp              = fGetRomConstModulInInVoltKp();
    mcPowerIn->mcPidInVolt.mfKi              = fGetRomConstModulInInVoltKi();
    mcPowerIn->mcPidInVolt.mfKd              = fGetRomConstModulInInVoltKd();
    mcPowerIn->mcPidInVolt.mfOutputLimit     = fGetRomConstModulInInVoltKiCut();
    mcPowerIn->mcPidInVolt.mLpOutput.mFactor = fGetRomConstModulInInVoltLpOut();

    mcPowerIn->mcLpOutCurr.mFactor            = fGetRomConstModulInOutCurrLpIn();
    mcPowerIn->mcPidOutCurr.mfKp              = fGetRomConstModulInOutCurrKp();
    mcPowerIn->mcPidOutCurr.mfKi              = fGetRomConstModulInOutCurrKi();
    mcPowerIn->mcPidOutCurr.mfKd              = fGetRomConstModulInOutCurrKd();
    mcPowerIn->mcPidOutCurr.mfOutputLimit     = fGetRomConstModulInOutCurrKiCut();
    mcPowerIn->mcPidOutCurr.mLpOutput.mFactor = fGetRomConstModulInOutCurrLpOut();

    mcPowerIn->mcLpOutVolt.mFactor            = fGetRomConstModulInOutVoltLpIn();
    mcPowerIn->mcPidOutVolt.mfKp              = fGetRomConstModulInOutVoltKp();
    mcPowerIn->mcPidOutVolt.mfKi              = fGetRomConstModulInOutVoltKi();
    mcPowerIn->mcPidOutVolt.mfKd              = fGetRomConstModulInOutVoltKd();
    mcPowerIn->mcPidOutVolt.mfOutputLimit     = fGetRomConstModulInOutVoltKiCut();
    mcPowerIn->mcPidOutVolt.mLpOutput.mFactor = fGetRomConstModulInOutVoltLpOut();


    mcPowerIn->mfPidSetVoltIn  = u16GetRomConstModulInSetVoltInput();
    mcPowerIn->mfPidSetVoltOut = u16GetRomConstModulInSetVoltOutput();
    mcPowerIn->mfPidSetCurrIn  = u16GetRomConstModulInSetCurrInput();
    mcPowerIn->mfPidSetCurrOut = u16GetRomConstModulInSetCurrOutput();
    mcPowerIn->mfPidMaxVolt    = u16GetRomConstChrgMaxVoltCell() * mCellCnt;
    mcPowerIn->mfPidSetVolt    = mcPowerIn->mfPidSetVoltOut;

    mcPowerIn->mu8PidModeIn  = u8GetRomConstModulInModeInput();
    mcPowerIn->mu8PidModeOut = u8GetRomConstModulInModeOutput();

    if (mBoard > 0)
    {
      mcPowerOut->mType = u8GetRomConstModulOutType();

      mcPowerOut->mVoltage_mVmax = u16GetRomConstModulOutVoltMax();
      mcPowerOut->mVoltage_mVmin = u16GetRomConstModulOutVoltMinSupply();

      mcPowerOut->mcLpOutCurr.mFactor            = fGetRomConstModulOutOutCurrLpIn();
      mcPowerOut->mcPidOutCurr.mfKp              = fGetRomConstModulOutOutCurrKp();
      mcPowerOut->mcPidOutCurr.mfKi              = fGetRomConstModulOutOutCurrKi();
      mcPowerOut->mcPidOutCurr.mfKd              = fGetRomConstModulOutOutCurrKd();
      mcPowerOut->mcPidOutCurr.mfOutputLimit     = fGetRomConstModulOutOutCurrKiCut();
      mcPowerOut->mcPidOutCurr.mLpOutput.mFactor = fGetRomConstModulOutOutCurrLpOut();

      mcPowerOut->mcLpOutVolt.mFactor            = fGetRomConstModulOutOutVoltLpIn();
      mcPowerOut->mcPidOutVolt.mfKp              = fGetRomConstModulOutOutVoltKp();
      mcPowerOut->mcPidOutVolt.mfKi              = fGetRomConstModulOutOutVoltKi();
      mcPowerOut->mcPidOutVolt.mfKd              = fGetRomConstModulOutOutVoltKd();
      mcPowerOut->mcPidOutVolt.mfOutputLimit     = fGetRomConstModulOutOutVoltKiCut();
      mcPowerOut->mcPidOutVolt.mLpOutput.mFactor = fGetRomConstModulOutOutVoltLpOut();

      mcPowerOut->mfPidSetVoltIn  = u16GetRomConstModulOutSetVoltInput();
      mcPowerOut->mfPidSetVoltOut = u16GetRomConstModulOutSetVoltOutput();
      mcPowerOut->mfPidSetCurrIn  = u16GetRomConstModulOutSetCurrInput();
      mcPowerOut->mfPidSetCurrOut = u16GetRomConstModulOutSetCurrOutput();
      mcPowerOut->mfPidMaxVolt    = u16GetRomConstChrgMaxVoltCell() * mCellCnt;
      mcPowerOut->mfPidSetVolt    = mcPowerOut->mfPidSetVoltOut;

      mcPowerOut->mu8PidModeIn  = u8GetRomConstModulOutModeInput();
      mcPowerOut->mu8PidModeOut = u8GetRomConstModulOutModeOutput();
    }
  }

  bool bPowerDown(cStr& lcStr)
  {
    bool lbRet = True;


    // Während Laden nicht runterfahren.
    if ((lenSmLipo == enSmStChargingNormal) ||
        (lenSmLipo == enSmStChargingSave))
    {
      lbRet = False;
      if (lcStr.Len() > 0) lcStr += (rsz)", ";
      lcStr += (rsz)"Charging";
    }


    // Während Output an, nicht runterfahren.
    if (mcPowerOut->isEnable())
    {
      lbRet = False;
      if (lcStr.Len() > 0) lcStr += (rsz)", ";
      lcStr += (rsz)"Powerout";
    }

    if (lbRet)
    {
      mbPowerDown = True;
      mbPowerDownEnter = True;
    }

    return lbRet;
  }


  i16 i16GetBatVoltageAbs(u8 lu8Idx)
  {
    return (i16)fGetBatVoltageAbs(lu8Idx);
  }

  u8 u8Ina2Idx(const u8 lu8InaIdx, const u8 lu8InaChl)
  {
    return (lu8InaIdx - 1) * 2 + (lu8InaChl - 1);
  }

  float fGetBatVoltageAbs(u8 lu8Idx)
  {
    float lfValue = 0;
    //  INA11: mfVBusAvg[0]
    //  INA12: mfVBusAvg[1]
    //  INA13: mfVBusAvg[2]
    //  INA21: mfVBusAvg[3]
    //  INA22: mfVBusAvg[4]
    //  INA23: mfVBusAvg[5]
    if (mBoard == 0)  // 2 Bat Board
    {
      // Bei 1x INA
      // mcP[0] = Adr 0x40
      // 
      // v9B2
      //  INA11: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA13: Bat1
      // 
      // v10B2
      //  INA13: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA11: Bat1
      if (u16GetRomConstHwInfo() == 9)
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfVBusAvg[u8Ina2Idx(1, 3)]; break;
          case 2: lfValue = mfVBusAvg[u8Ina2Idx(1, 2)]; break;
        }
      }
      else
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfVBusAvg[u8Ina2Idx(1, 1)]; break;
          case 2: lfValue = mfVBusAvg[u8Ina2Idx(1, 2)]; break;
        }
      }
    }
    else
    {
      // Bei 2x INA
      // mcP[0] = Adr 0x40
      //  INA11: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat4 + Spannungsabfall am Widerstand über Bat 4
      //  INA13: Ausgang (Nach Ausgangs Konverter)
      //
      // mcP[1] = Adr 0x41
      //  INA21: Spannung an Bat3
      //  INA22: Spannung an Bat2
      //  INA23: Spannung an Bat1
      switch (lu8Idx)  // 4 Bat Board
      {
        // Wert verkleinern, verkleinert den ADC wert
        case 4: lfValue = mfVBusAvg[u8Ina2Idx(1,2)] - 4; break; // -4 wegen Mosfet spannungsabfall
        case 3: lfValue = mfVBusAvg[u8Ina2Idx(2,1)];     break;
        case 2: lfValue = mfVBusAvg[u8Ina2Idx(2,2)];     break;
        case 1: lfValue = mfVBusAvg[u8Ina2Idx(2,3)];     break;
      }
    }

    return lfValue;
  }


  i16 i16GetSysVoltageAbs(u8 lu8Idx)
  {
    return (i16)fGetSysVoltageAbs(lu8Idx);
  }

  float fGetSysVoltageAbs(u8 lu8Idx)
  {
    float lfValue = 0;
    if (mBoard == 0)  // 2 Bat Board
    {
      // v9B2
      //  INA11: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA13: Bat1
      // 
      // v10B2
      //  INA13: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA11: Bat1
      if (u16GetRomConstHwInfo() == 9)
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfVBusAvg[u8Ina2Idx(1,1)]; break; // Board Input
          case 2: lfValue = mfVBusAvg[u8Ina2Idx(1,2)]; break; // Board Bat
          case 3: lfValue = 0;                         break; // Board Output
        }
      }
      else
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfVBusAvg[u8Ina2Idx(1,3)]; break; // Board Input
          case 2: lfValue = mfVBusAvg[u8Ina2Idx(1,2)]; break; // Board Bat
          case 3: lfValue = 0;                         break; // Board Output
        }
      }
    }
    else // 4 Bat Board
    {
      switch (lu8Idx)
      {
        case 1: lfValue = mfVBusAvg[u8Ina2Idx(1,1)]; break; // Board Input
        case 2: lfValue = mfVBusAvg[u8Ina2Idx(1,2)]; break; // Board Bat
        case 3: lfValue = mfVBusAvg[u8Ina2Idx(1,3)]; break; // Board Output
      }
    }
    return lfValue;
  }

  i16 i16GetSysCurrentAbs(u8 lu8Idx)
  {
    return (i16)fGetSysCurrentAbs(lu8Idx);
  }

  float fGetSysCurrentAbs(u8 lu8Idx)
  {
    float lfValue = 0;
    if (mBoard == 0)  // 2 Bat Board
    {
      // v9B2
      //  INA11: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA13: Bat1
      // 
      // v10B2
      //  INA13: Eingang  (Vor Charger)
      //  INA12: Batterie/System (Nach Charger); Spannung Bat2 + Spannungsabfall am Widerstand über Bat 2
      //  INA11: Bat1
      if (u16GetRomConstHwInfo() == 9)
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfIShuntAvg[u8Ina2Idx(1,1)]; break; // Board Input
          case 2: lfValue = mfIShuntAvg[u8Ina2Idx(1,2)]; break; // Board Bat
          case 3: lfValue = 0;                           break; // Board Output
        }
      }
      else
      {
        switch (lu8Idx)
        {
          case 1: lfValue = mfIShuntAvg[u8Ina2Idx(1,3)]; break; // Board Input
          case 2: lfValue = mfIShuntAvg[u8Ina2Idx(1,2)]; break; // Board Bat
          case 3: lfValue = 0;                           break; // Board Output
        }
      }
    }
    else  // 4 Bat Board
    {
      switch (lu8Idx)
      {
        case 1: lfValue = mfIShuntAvg[u8Ina2Idx(1,1)]; break; // Board Input
        case 2: lfValue = mfIShuntAvg[u8Ina2Idx(1,2)]; break; // Board Bat
        case 3: lfValue = mfIShuntAvg[u8Ina2Idx(1,3)]; break; // Board Output
      }
    }
    return lfValue;
  }


  i16 i16GetBatVoltageRel(u8 lu8Idx)
  {
    return (i16)fGetBatVoltageRel(lu8Idx);
  }

  float fGetBatVoltageRel(u8 lu8Idx)
  {
    switch (lu8Idx)
    {
      case 1: return mfVBat[lu8Idx - 1]; break;
      case 2: return mfVBat[lu8Idx - 1]; break;
      case 3: return mfVBat[lu8Idx - 1]; break;
      case 4: return mfVBat[lu8Idx - 1]; break;
    }
    return 0;
  }

  u8 u8Get_S_Balancer(u8 lu8Idx)
  {
    switch (lu8Idx)
    {
      case 1: if (mS_Balancer[0]) { return mS_Balancer[0]->ui8Get();}; break;
      case 2: if (mS_Balancer[1]) { return mS_Balancer[1]->ui8Get();}; break;
      case 3: if (mS_Balancer[2]) { return mS_Balancer[2]->ui8Get();}; break;
      case 4: if (mS_Balancer[3]) { return mS_Balancer[3]->ui8Get();}; break;
    }
    return 0;
  }

  void vUpdateBatState_10ms(bool lu8ForceDirectRead)
  {
    // Update Battery State
    // There is a debouncing of 2s
    u8 lui8t;
    i16 lui16Volt;

    if (lu8ForceDirectRead)
    {
      for (lui8t = 0; lui8t < mCellCnt; lui8t++)
      {
        lui16Volt = i16GetBatVoltageRel(lui8t + 1);

        munCellState[lui8t].u8Data = 0;
        if (lui16Volt > u16GetRomConstChrgVoltBalance())    munCellState[lui8t].Bits.OV = 1;
        if (lui16Volt < u16GetRomConstChrgVoltCellDefect()) munCellState[lui8t].Bits.Defect = 1;
        if (lui16Volt < u16GetRomConstChrgVoltLipoOff())    munCellState[lui8t].Bits.UV = 1;
        if (lui16Volt < u16GetRomConstChrgVoltSysOff())     munCellState[lui8t].Bits.SysOff = 1;

        mcCellStHdlOV[lui8t].vSet(munCellState[lui8t].Bits.OV);
        mcCellStHdlDefect[lui8t].vSet(munCellState[lui8t].Bits.Defect);
        mcCellStHdlUV[lui8t].vSet(munCellState[lui8t].Bits.UV);
        mcCellStHdlSysOff[lui8t].vSet(munCellState[lui8t].Bits.SysOff);
      }
    }
    else
    {
      for (lui8t = 0; lui8t < mCellCnt; lui8t++)
      {
        tunCellState lunCellState;

        lui16Volt = i16GetBatVoltageRel(lui8t + 1);

        lunCellState.u8Data = 0;
        if (lui16Volt > u16GetRomConstChrgVoltBalance())    lunCellState.Bits.OV = 1;
        if (lui16Volt < u16GetRomConstChrgVoltCellDefect()) lunCellState.Bits.Defect = 1;
        if (lui16Volt < u16GetRomConstChrgVoltLipoOff())    lunCellState.Bits.UV = 1;
        if (lui16Volt < u16GetRomConstChrgVoltSysOff())     lunCellState.Bits.SysOff = 1;

        if (lu8ForceDirectRead)
        {
          // Werte sofort übernehmen
          munCellState[lui8t].u8Data = lunCellState.u8Data;

          mcCellStHdlOV[lui8t].vSet(lunCellState.Bits.OV);
          mcCellStHdlDefect[lui8t].vSet(lunCellState.Bits.Defect);
          mcCellStHdlUV[lui8t].vSet(lunCellState.Bits.UV);
          mcCellStHdlSysOff[lui8t].vSet(lunCellState.Bits.SysOff);
        }
        else
        {
          // Werte müssen 2s lang anliegen, um übernommen zu werden
          mcCellStHdlOV[lui8t].vSetNew(lunCellState.Bits.OV);
          mcCellStHdlDefect[lui8t].vSetNew(lunCellState.Bits.Defect);
          mcCellStHdlUV[lui8t].vSetNew(lunCellState.Bits.UV);
          mcCellStHdlSysOff[lui8t].vSetNew(lunCellState.Bits.SysOff);

          munCellState[lui8t].Bits.OV = mcCellStHdlOV[lui8t].u8Get();
          munCellState[lui8t].Bits.Defect = mcCellStHdlDefect[lui8t].u8Get();
          munCellState[lui8t].Bits.UV = mcCellStHdlUV[lui8t].u8Get();
          munCellState[lui8t].Bits.SysOff = mcCellStHdlSysOff[lui8t].u8Get();
        }
      }
    }
  }


  bool isBatCharging()
  {
    if (i16GetSysCurrentAbs(2) > 10) // Battery is charging
    {
      return True;
    }
    return False;
  }

  bool isBatDisCharging()
  {
    if (i16GetSysCurrentAbs(2) < 0) // Battery is discharging
    {
      return True;
    }
    return False;
  }

  bool isChargeNeeded()
  {
    i16 lui16Volt;

    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      lui16Volt = i16GetBatVoltageRel(lui8t + 1);
      if (lui16Volt  <= u16GetRomConstChrgVoltStart()) return True;
    }
    return False;
  }

  bool isExternalSupplyAvailable()
  {
    if (i16GetSysVoltageAbs(1) > u16GetRomConstModulInVoltMinSupply()) // Battery is charging
    {
      return True;
    }
    return False;
  }


  bool isBatOV()
  {
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      if (munCellState[lui8t].Bits.OV) return True;
    }
    return False;
  }

  bool isBatMaxed()
  {
    i16 lui16Volt;
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      lui16Volt = i16GetBatVoltageRel(lui8t + 1);
      if (lui16Volt < u16GetRomConstChrgMaxVoltCell()) return False;
    }
    return True;
  }


  bool isBatUV()
  {
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      if (munCellState[lui8t].Bits.UV) return True;
    }
    return False;
  }

  bool isBatSysOff()
  {
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      if (munCellState[lui8t].Bits.SysOff) return True;
    }
    return False;
  }

  bool isBatDefect()
  {
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      if (munCellState[lui8t].Bits.Defect) return True;
    }
    return False;
  }

  bool isBatNormal()
  {
    for (u8 lui8t = 0; lui8t < mCellCnt; lui8t++)
    {
      if (munCellState[lui8t].u8Data == 0) return True;
    }
    return False;
  }

  bool isCharging()
  {
    if ((lenSmLipo == enSmStChargingNormal) ||
        (lenSmLipo == enSmStChargingSave)) return True;
    return False;
  }


  void vUpdate_S1_Lipo(bool lbForceOn)
  {
    // Battery Main Switch
    // If any Bat is below low and not charging, then turn the Bat off

    if (lbForceOn)
    {
      mS_Lipo.vSet1();
      lenSmS[enS1_Lipo] = enSmStOn;
      return;
    }

    switch (lenSmS[enS1_Lipo])
    {
      case enSmStOff:
        if (isBatDefect())
        {
          return;
        }
        else
        {
          // Battery Switch always on, when possible.
          //if (isBatCharging()) // Battery is charging
          {
            mS_Lipo.vSet1();
            lenSmS[enS1_Lipo] = enSmStOn;
          }
        }
        break;
      case enSmStOn:
        if (isBatDefect())
        {
          mS_Lipo.vSet0();
          lenSmS[enS1_Lipo] = enSmStOff;
        }
        else
        {
          if ((isBatUV()) && (isBatDisCharging())) // Battery is discharging
          {
            mS_Lipo.vSet0();
            lenSmS[enS1_Lipo] = enSmStOff;
          }
        }
        break;
    }
  }



  void vUpdate_S_Balancer(bool lbForceOff)
  {
    if (lbForceOff)
    {
      if (mS_Balancer[0]) {mS_Balancer[0]->vSet0();lenSmS[1] = enSmStOff;}
      if (mS_Balancer[1]) {mS_Balancer[1]->vSet0();lenSmS[2] = enSmStOff;}
      if (mS_Balancer[2]) {mS_Balancer[2]->vSet0();lenSmS[3] = enSmStOff;}
      if (mS_Balancer[3]) {mS_Balancer[3]->vSet0();lenSmS[4] = enSmStOff;}
      mBalancingActive = 0;
      return;
    }

    if ((!isBatOV()) && (!mBalancingActive)) return;

    for (u8 lu8Idx = 0; lu8Idx < mCellCnt; lu8Idx++)
    {
      switch (lenSmS[lu8Idx + 1])
      {
        case enSmStOff:
          if ((munCellState[lu8Idx].Bits.OV) && (isBatCharging()))
          {
            mS_Balancer[lu8Idx]->vSet1();
            mBalancingActive |= 1 << lu8Idx;
            lenSmS[lu8Idx + 1] = enSmStOn;
          }
          break;
        case enSmStOn:
          // Balancer wieder abschalten, sobald man die Spannung der höchsten nicht gebalance'ten Zelle erreicht hat
          u8 lu8CellIdx;
          u16 lu16TurnOffVolt = 0;
          for (lu8CellIdx = 0; lu8CellIdx < mCellCnt; lu8CellIdx++)
          {
            if ((mBalancingActive & (1 << lu8CellIdx)) == 0)
            {
              if (lu16TurnOffVolt < i16GetBatVoltageRel(lu8CellIdx + 1))
              {
                lu16TurnOffVolt = i16GetBatVoltageRel(lu8CellIdx + 1);
              }
            }
          }
          if (lu16TurnOffVolt == 0) lu16TurnOffVolt = u16GetRomConstChrgMaxVoltCell();

          if (i16GetBatVoltageRel(lu8Idx + 1) <= lu16TurnOffVolt)
          {
            mS_Balancer[lu8Idx]->vSet0();
            mBalancingActive &= (255 - (1 << lu8Idx));
            lenSmS[lu8Idx + 1] = enSmStOff;
          }
          break;
      }
    }
  }



  void vUpdateCharger()
  {
    i16 li16I_ChargerLimit;
    i16 li16U_ChargerLimit;
    i16 li16BatteryVolt;
    i16 li16BatteryCurrent;

    lenSmLipoPrev = lenSmLipo;

    if (isBatDefect())
    {
      lenSmLipo = enSmStNotCharging_Error_CellsDefect;
      mcPowerIn->vSetPidOutput(u16GetRomConstModulInSetVoltOutput(), u16GetRomConstChrgCurrentSave());
      mcPowerIn->vDisable();
      return;
    }

    if (!isExternalSupplyAvailable())
    {
      lenSmLipo = enSmStNotCharging_Error_LowInputVoltage;
      mcPowerIn->vSetPidOutput(u16GetRomConstModulInSetVoltOutput(), u16GetRomConstChrgCurrentSave());
      mcPowerIn->vDisable();
      return;
    }


    li16U_ChargerLimit = mCellCnt * mi16ChargerVoltCell;
    li16I_ChargerLimit = mi16ChargerCurrent;
    li16BatteryVolt    = mfVBusAvg[1];
    li16BatteryCurrent = mfIShuntAvg[1];

    if ((isChargeNeeded()) ||
        ((u8GetRomConstChrgModeStart() == 1) && (lenSmLipo != enSmStCharged)) ||
        (lenSmLipo == enSmStChargingNormal) ||
        (lenSmLipo == enSmStChargingSave))
    {
      lenSmLipo = enSmStChargingNormal;

      if ((isBatUV()) || (mBalancingActive != 0))
      {
        lenSmLipo = enSmStChargingSave;
        li16I_ChargerLimit = mi16ChargerCurrentSave;
      }

      bool lbCharging = (lenSmLipoPrev == enSmStChargingNormal) || (lenSmLipoPrev == enSmStChargingSave);
      if (!lbCharging) // Regelung direkt auf die aktuelle Batteriespannung setzen.
      {
        mcPowerIn->mfPidSetVolt = li16BatteryVolt;
      }

      mcPowerIn->vSetPidOutput(li16U_ChargerLimit, li16I_ChargerLimit);
      mcPowerIn->vEnable();
    }

    switch (lenSmLipo)
    {
      case cLipoMon::enSmStNotCharging_Error_Ina:
      case cLipoMon::enSmStNotCharging_Error_LowInputVoltage:
      case cLipoMon::enSmStNotCharging_Error_CellsDefect:
        lenSmLipo = enSmStNotCharging;
        break;
      default:
        break;
    }

    if ((li16BatteryVolt >= (li16U_ChargerLimit - 25)) && // Geladen ?
        (li16BatteryCurrent < u16GetRomConstChrgCurrentCharged()))
    {
      mu32ChargerReadyCnt += 10;

      if (mu32ChargerReadyCnt > u16GetRomConstChrgTimeOut1())
      {
        mu32ChargerReadyCnt = 0;
        lenSmLipo = enSmStCharged;

        switch (u8GetRomConstChrgModeCharged())
        {
          case 0: // 0 : Charger aus
            mcPowerIn->vDisable();
            break;
          case 1: // 1: Erhaltungsspannung
            // Nichts machen
            mcPowerIn->vSetPidOutput(li16U_ChargerLimit, li16I_ChargerLimit);
            mcPowerIn->vEnable();
            break;
          default:
            break;
        }
      }
    }
    else
    {
      mu32ChargerReadyCnt = 0;
    }
  }

  // Check die Ina. Gibt True zurück für den Fall von Error oder Ina-Initialisiert
  // Initialisierung von einem Ina dauert ca. 500us
  bool bStartupCheck()
  {
    u8 lu8InaInitCnt;

    mu8InaErr = 0;
    lu8InaInitCnt = 0;
    for (u8 lui8t = 0; lui8t < mInaCnt; lui8t++)
    {
      if (mcP[lui8t]->mStatus.IsError == 1)
      {
        mu8InaErr += (1 << lui8t);
      }

      if (mcP[lui8t]->mStatus.IsInit)
      {
        lu8InaInitCnt++;
      }
    }
    if (mu8InaErr) return True;
    if (lu8InaInitCnt == mInaCnt) return True;

    return False;
  }

  void vStartupInaFirstRead()
  {
    for (u8 lui8t = 0; lui8t < mInaCnt; lui8t++)
    {
      for (u8 lui8i = 1; lui8i <= 3; lui8i++)
      {
        mcP[lui8t]->i8ReadVShunt_digit(lui8i);
        mcP[lui8t]->i8ReadVBus_digit(lui8i);
      }
    }
  }

  bool bCheckStartupInaFirstReadFinished()
  {
    bool lbRet = True;
    for (u8 lui8t = 0; lui8t < mInaCnt; lui8t++)
    {
      for (u8 lui8i = 1; lui8i <= 3; lui8i++)
      {
        if ((mcP[lui8t]->mStatus.IsStartRequested) ||
            (mcP[lui8t]->menCmd != mcP[lui8t]->enCmdIdle))
        {
          lbRet = False;
        }
      }
    }

    return lbRet;
  }

  bool bCheckInaError()
  {
    mu8InaErr = 0;
    for (u8 lui8t = 0; lui8t < mInaCnt; lui8t++)
    {
      if ((mcP[lui8t]->mStatus.IsError == 1) || (mcP[lui8t]->mStatus.IsInit == 0))
      {
        mu8InaErr += (1 << lui8t);
      }
    }
    if (mu8InaErr) return True;
    return False;
  }

  bool isStatusOk()
  {
    if (mCellCnt == 0) return True;
    if (mTestMode) return True;

    if (!mu8InaErr)
    {
      if (lenSmLipo == enSmStNotCharging) return True;
      if (lenSmLipo == enSmStChargingNormal) return True;
      if (lenSmLipo == enSmStCharged) return True;
    }
    return False;
  }

  void vUpdateAll_10ms(bool bIsAuto)
  {
    if (!mbPowerDown)
    {
      u8 lui8t;
      u8 lui8i;

      volatile float lfInputFactor;

      lfInputFactor = fGetRomConstModulInOutVoltLpIn();

      if (bCheckInaError())
      {
        mu16Startup_Delay = 100;
      }

      if (mu16Startup_Delay)
      {
        mu8ForceDirectRead = True;
      }

      // Strom und Spannung einlesen und Durchschnitt ermitteln
      if (!mu8InaErr)
      {
        // --------------  Eingangwerte lesen ------------------
        for (lui8t = 0; lui8t < mInaCnt; lui8t++)
        {
          for (lui8i = 1; lui8i <= 3; lui8i++)
          {
            uint8 lui8Idx = 3 * lui8t + (lui8i - 1);

            mcP[lui8t]->i8ReadVShunt_digit(lui8i);
            mcP[lui8t]->i8ReadVBus_digit(lui8i);

            mi32VBus[lui8Idx]   = mcP[lui8t]->i32CalcVBus_uV(lui8i)   / 1000;
            mi32IShunt[lui8Idx] = mcP[lui8t]->i32CalcIShunt_uA(lui8i) / 1000;
            mi32VShunt[lui8Idx] = mcP[lui8t]->i32CalcVShunt_uV(lui8i) / 1000;

            if (mu8ForceDirectRead)
            {
              mfVBusAvg[lui8Idx]   = mi32VBus[lui8Idx];
              mfIShuntAvg[lui8Idx] = mi32IShunt[lui8Idx];
              mfVShuntAvg[lui8Idx] = mi32VShunt[lui8Idx];
            }
            else
            {
              float lfError = (mcP[lui8t]->i32CalcVBus_uV(lui8i) / 1000.0f - mfVBusAvg[lui8Idx]);
              mfVBusAvg[lui8Idx] = mfVBusAvg[lui8Idx] + lfError * lfInputFactor;

              lfError = (mcP[lui8t]->i32CalcIShunt_uA(lui8i) / 1000.0f - mfIShuntAvg[lui8Idx]);
              mfIShuntAvg[lui8Idx] = mfIShuntAvg[lui8Idx] + lfError * lfInputFactor;

              lfError = (mcP[lui8t]->i32CalcVShunt_uV(lui8i) / 1000.0f - mfVShuntAvg[lui8Idx]);
              mfVShuntAvg[lui8Idx] = mfVShuntAvg[lui8Idx] + lfError * lfInputFactor;
            }
          }
        }

        if (mCellCnt > 0)
        {
          // -------------- Batteriespannung auswerten -------------------------------
          mfVBat[0] = fGetBatVoltageAbs(1);
          for (lui8i = 2; lui8i <= mCellCnt; lui8i++)
          {
            mfVBat[lui8i - 1] = fGetBatVoltageAbs(lui8i) - fGetBatVoltageAbs(lui8i - 1);
          }
          for (lui8i = 0; lui8i < mCellCnt; lui8i++)
          {
            if (mfVBat[lui8i] < 0.0f) mfVBat[lui8i] = 0.0f;
          }

          // --------------  Batterizustand erfassen -----------------------------------
          vUpdateBatState_10ms(mu8ForceDirectRead);

          if (mu8ForceDirectRead)
          {
            mu8ForceDirectRead = False;
          }
        }

        if ((mu16Startup_Delay == 0) && (bIsAuto))
        {
          if (mTestMode)
          {
            vUpdate_S1_Lipo(True);
            vUpdate_S_Balancer(True);
          }
          else
          {
            switch (mCellCnt)
            {
              case 0:
                // ------------------  Schalter setzten --------------------------------------
                vUpdate_S1_Lipo(True);    // immer an
                vUpdate_S_Balancer(True); // immer aus
                break;
              case 1:
                // ------------------  Schalter setzten --------------------------------------
                vUpdate_S1_Lipo(False);    // automatik
                vUpdate_S_Balancer(True);  // immer aus
                // ------------------  Batterie laden --------------------------------------
                vUpdateCharger();
                break;
              default:
                // ------------------  Schalter setzten --------------------------------------
                vUpdate_S1_Lipo(False);   // automatik
                if (!isBatDefect())
                {
                  vUpdate_S_Balancer(False);  // automatik
                }
                else
                {
                  vUpdate_S_Balancer(True);  // immer aus
                }

                // ------------------  Batterie laden --------------------------------------
                vUpdateCharger();
              }
          }

          // ------------------ Die Spannungswandler updaten -------------------------
          mcPowerIn->vDoProcess(fGetSysVoltageAbs(1), fGetSysCurrentAbs(1), fGetSysVoltageAbs(2), fGetSysCurrentAbs(2));
          mcPowerOut->vDoProcess(fGetSysVoltageAbs(2), fGetSysCurrentAbs(2), fGetSysVoltageAbs(3), fGetSysCurrentAbs(3));
        }
        else
        {
          if (mu16Startup_Delay > 10) mu16Startup_Delay -= 10;
          else mu16Startup_Delay = 0;
        }
      }
      else // InaError
      {
        u8 lu8CellIdx;

        lenSmLipo = enSmStNotCharging_Error_Ina;

        for (lu8CellIdx = 0; lu8CellIdx < mCellCnt; lu8CellIdx++)
        {
          mS_Balancer[lu8CellIdx]->vSet0();
        }

        mcPowerOut->vDisable();
        mcPowerIn->vDisable();
      }
    }
    else
    {
      if (mbPowerDownEnter)
      {
        mbPowerDownEnter = False;

        lenSmLipo = enSmStNotCharging_PowerDown;

        u8 lu8CellIdx;
        for (lu8CellIdx = 0; lu8CellIdx < mCellCnt; lu8CellIdx++)
        {
          mS_Balancer[lu8CellIdx]->vSet0();
        }

        mcPowerOut->vDisable();
        mcPowerIn->vDisable();
      }
    }
  }
};


class cSysPkgBoard
{
  public:

  /*Test*/
  cGpPin mcWakeup;

  /*PA8 im Testmode für Interrupt Timing */
  cStatusLed<GPIOB_BASE, 9> mcStatusLed;

  cDigiPower           mcDigiPower; // DigiPower (3V3) vor I2C init, sonst error in I2C_Init.CheckPins

  // I2C2:
  cGpPin mcI2c2_SCL_Board;
  cGpPin mcI2c2_SDA_Board;

  cI2cMaster           mcI2C2_Board;
  cI2cEep              mcEep;

  cLipoMon             mcLipoMon;

  cSysPkgBoard();

  void vInit1(void);
  void vInit2();

  bool isError(cStr& lszErrorInfo);
  bool isReadyForSleep(cStr& lcStatus);

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};



#ifdef __cplusplus
  extern "C" {
#endif

extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // __BSP___SYS_PKG_BASE_MBv10b2_BOARD_H__
