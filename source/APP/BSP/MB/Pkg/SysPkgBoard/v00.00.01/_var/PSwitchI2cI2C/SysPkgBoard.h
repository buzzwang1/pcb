
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

  //Digi Power
  cGpPin lcDigi1;

  cDigiPower(u8 lu8InitState = 0);

  void vPowerOn();
  void vPowerOff();
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
    //if (mu8Slot == 0)
    //{
      GPIOA->BSRR = (1 << 5);
    //}
    //else
    //{
    //  GPIOB->BSRR = (1 << 5);
    //}

    mbEnable = True;
  }

  virtual void vDisable()
  {
    //if (mu8Slot == 0)
    //{
      GPIOA->BRR = (1 << 5);
    //}
    //else
    //{
    //  GPIOB->BRR = (1 << 8);
    //}

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
  // PA5 Enable


  u32 mu32Channel;

  cPmMt3608(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    mu8Slot = lu8Slot;

    LL_DAC_InitTypeDef DAC_InitStructure;

    mu32Channel = LL_DAC_CHANNEL_1;
    cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mEnable(GPIOA_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

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
  // PA5 Enable


  cPmSwitch(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    mu8Slot = lu8Slot;

    cGpPin mEnable(GPIOA_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

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


  cPmMp8859(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot, cI2cMaster* lcI2C)
  {
    mu8Slot = lu8Slot;
    mcI2C   = lcI2C;

    cGpPin mEnable(GPIOA_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mcMP8859 = new cMP8859(mcI2C, MP8859_I2C_ADDRESS_CONF_0);

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
      cVoltageConverter::vEnable();
      //mbEnable = True;
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
      cVoltageConverter::vDisable();
      //mbEnable = False;
    }
  }
};



class cPmTp4056 : public cVoltageConverter
{
  public:
  //Slot0:
  // PA4 DAC1 Channel1
  // PA5 Enable

  u32 mu32Channel;

  cPmTp4056(u16 lu16Voltage_mV, u16 lu16CurrentLimit_mA, u8 lu8Slot)
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    mu8Slot = lu8Slot;

    LL_DAC_InitTypeDef DAC_InitStructure;

    mu32Channel = LL_DAC_CHANNEL_1;
    cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mEnable(GPIOA_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

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


class cBoardCtrl
{
public:

  typedef enum
  {
    enS_Ch1 = 0,
    enS_Ch2,
    enS_Ch3,
    enS_3V3,
  }tenLipoMonGeneral;

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
    enDummy = 0,
  }tenLipoMonS_Const;


  cI2cMaster* mcI2C;

  cINA3221* mcP[1];
  u8 mu8InaErr;

  u8     mu8ForceDirectRead;
  int32  mi32VBus[3];
  float  mfVBusAvg[3];
  int32  mi32IShunt[3];
  float  mfIShuntAvg[3];
  int32  mi32VShunt[3];
  float  mfVShuntAvg[3];

  tenLipoMonS_States      lenSmS[3];

  u16   mu16Startup_Delay;
  u8    mTestMode;

  tcGpPin<GPIOB_BASE, 0> lcEnCh1;
  tcGpPin<GPIOB_BASE, 1> lcEnCh2;
  tcGpPin<GPIOB_BASE, 2> lcEnCh3;

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


  cBoardCtrl(cI2cMaster* lcI2C)
    : lcEnCh1(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      lcEnCh2(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      lcEnCh3(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    mcI2C = lcI2C;

    mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 50, 50, 50);
    mcP[0]->i8Setup();
    mcPowerOut = cGetVoltageConverter(u16GetRomConstModulOutSetVoltOutput(), u16GetRomConstModulOutSetCurrOutput(), u8GetRomConstModulOutType(), 1);

    mu8InaErr = 0;

    lenSmS[enS_Ch1] = enSmStOff;
    lenSmS[enS_Ch2] = enSmStOff;
    lenSmS[enS_Ch3] = enSmStOff;


    u8 lui8t;
    for (lui8t = 0; lui8t < 3; lui8t++)
    {
      mi32VBus[lui8t] = 0;
      mi32IShunt[lui8t] = 0;
      mi32VShunt[lui8t] = 0;
    }

    mu16Startup_Delay = 50; //[ms]
    mTestMode         = 0;

    mu8ForceDirectRead = True;

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
    mcPowerOut->mfPidMaxVolt    = u16GetRomConstModulOutVoltMax();
    mcPowerOut->mfPidSetVolt    = mcPowerOut->mfPidSetVoltOut;

    mcPowerOut->mu8PidModeIn  = u8GetRomConstModulOutModeInput();
    mcPowerOut->mu8PidModeOut = u8GetRomConstModulOutModeOutput();
  }

  bool bPowerDown(cStr& lcStr)
  {
    // Nie ausschalten
    bool lbRet = False;

    UNUSED(lcStr);

    //// // Während Output an, nicht runterfahren.
    //// if (mcPowerOut->isEnable())
    //// {
    ////   lbRet = False;
    ////   if (lcStr.Len() > 0) lcStr += (rsz)", ";
    ////   lcStr += (rsz)"Powerout";
    //// }

    return lbRet;
  }


  u8 u8Ina2Idx(const u8 lu8InaIdx, const u8 lu8InaChl)
  {
    return (lu8InaIdx - 1) * 2 + (lu8InaChl - 1);
  }

  i16 i16GetChVoltageAbs(u8 lu8Idx)
  {
    return (i16)fGetChVoltageAbs(lu8Idx);
  }

  float fGetChVoltageAbs(u8 lu8Idx)
  {
    float lfValue = 0.0f;
    switch (lu8Idx)
    {
      case 1: lfValue = mfVBusAvg[u8Ina2Idx(1,3)]; break;
      case 2: lfValue = mfVBusAvg[u8Ina2Idx(1,2)]; break;
      case 3: lfValue = mfVBusAvg[u8Ina2Idx(1,1)]; break;
    }
    return lfValue;
  }


  i16 i16GetChCurrentAbs(u8 lu8Idx)
  {
    return (i16)fGetChCurrentAbs(lu8Idx);
  }

  float fGetChCurrentAbs(u8 lu8Idx)
  {
    float lfValue = 0.0f;
    switch (lu8Idx)
    {
      case 1: lfValue = mfIShuntAvg[u8Ina2Idx(1,3)]; break;
      case 2: lfValue = mfIShuntAvg[u8Ina2Idx(1,2)]; break;
      case 3: lfValue = mfIShuntAvg[u8Ina2Idx(1,1)]; break;
    }
    return lfValue;
  }

  void vUpdateAll(bool bIsAuto)
  {
    u8 lui8i;

    volatile float lfInputFactor;

    lfInputFactor = fGetRomConstModulOutOutVoltLpIn();

    mu8InaErr = 0;
    if ((mcP[0]->mStatus.IsError == 1) || (mcP[0]->mStatus.IsInit == 0))
    {
      mu8InaErr += (1<<0);
      mu16Startup_Delay = 1000;
    }

    if (mu16Startup_Delay)
    {
      mu8ForceDirectRead = True;
    }

    // Strom und Spannung einlesen und Durchschnitt ermitteln
    if (!mu8InaErr)
    {
      // --------------  Eingangwerte lesen ------------------
      for (lui8i = 1; lui8i <= 3; lui8i++)
      {
        uint8 lui8Idx = 3 * 0 + (lui8i - 1);

        mcP[0]->i8ReadVShunt_digit(lui8i);
        mcP[0]->i8ReadVBus_digit(lui8i);

        mi32VBus[lui8Idx]   = mcP[0]->i32CalcVBus_uV(lui8i)   / 1000;
        mi32IShunt[lui8Idx] = mcP[0]->i32CalcIShunt_uA(lui8i) / 1000;
        mi32VShunt[lui8Idx] = mcP[0]->i32CalcVShunt_uV(lui8i) / 1000;

        if (mu8ForceDirectRead)
        {
          mfVBusAvg[lui8Idx]   = mi32VBus[lui8Idx];
          mfIShuntAvg[lui8Idx] = mi32IShunt[lui8Idx];
          mfVShuntAvg[lui8Idx] = mi32VShunt[lui8Idx];
        }
        else
        {
          float lfError = (mcP[0]->i32CalcVBus_uV(lui8i) / 1000.0f - mfVBusAvg[lui8Idx]);
          mfVBusAvg[lui8Idx] = mfVBusAvg[lui8Idx] + lfError * lfInputFactor;

          lfError = (mcP[0]->i32CalcIShunt_uA(lui8i) / 1000.0f - mfIShuntAvg[lui8Idx]);
          mfIShuntAvg[lui8Idx] = mfIShuntAvg[lui8Idx] + lfError * lfInputFactor;

          lfError = (mcP[0]->i32CalcVShunt_uV(lui8i) / 1000.0f - mfVShuntAvg[lui8Idx]);
          mfVShuntAvg[lui8Idx] = mfVShuntAvg[lui8Idx] + lfError * lfInputFactor;
        }
      }

      if ((mu16Startup_Delay == 0) && (bIsAuto))
      {
        // ------------------ Die Spannungswandler updaten -------------------------
        mcPowerOut->vDoProcess(5000.0f, 1000.0f, fGetChVoltageAbs(1), fGetChCurrentAbs(1));
      }
      else
      {
        if (mu16Startup_Delay > 10) mu16Startup_Delay -= 10;
        else mu16Startup_Delay = 0;
      }
    }
    else // InaError
    {
      mcPowerOut->vDisable();
    }
  }
};


class cSysPkgBoard
{
  public:

  cDigiPower           mcDigiPower; // DigiPower (3V3) vor I2C init, sonst error in I2C_Init.CheckPins

  cStatusLed<GPIOB_BASE, 9> mcStatusLed;

  // I2C2:
  cGpPin mcI2c3_SCL_Board;
  cGpPin mcI2c3_SDA_Board;

  cI2cMaster           mcI2C3_Board;
  cI2cEep              mcEep;

  cBoardCtrl           mcBoardCntrl;

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

extern void I2C3_EV_IRQHandler(void);
extern void I2C3_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // __BSP___SYS_PKG_BASE_MBv10b2_BOARD_H__
