#ifndef __SYSTEM_STAGE_S5_H__
#define __SYSTEM_STAGE_S5_H__

#include "TypeDef.h"
#include "cSysStage.h"
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "cPidFloat.h"
#include "cStrT.h"

#include "ina3221.h"

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

  cStatusLed(bool lbInverse)
    : LED<mpstPort, mui16Pin>()
  {
    mSm = nStToggle500ms;
    mCounter = 0;
    this->mInverse = lbInverse;
  };

  void vSetMode(cStatusLedMode::tenMode lSm)
  {
    if (mSm != lSm) mCounter = 0;
    mSm = lSm;
  }

  void vTick100ms()
  {
    switch (mSm)
    {
      case nStToggle100ms:
        this->Toggle();
        break;
      case nStToggle200ms:
        if (mCounter & 1)
        {
          this->Toggle();
        }
        break;
      case nStToggle500ms:
        if ((mCounter % 5) == 0)
        {
          this->Toggle();
        }
        break;
      case nStToggle1000ms:
        if ((mCounter % 10) == 0)
        {
          this->Toggle();
        }
        break;
      case nStPuls500ms:
        if ((mCounter % 5) == 0)
        {
          this->On();
        }
        else
        {
          this->Off();
        }
        break;
      case nStPuls1000ms:
        if ((mCounter % 10) == 0)
        {
          this->On();
        }
        else
        {
          this->Off();
        }
        break;
      case nStPuls5000ms:
        if (mCounter == 0)
        {
          this->On();
        }
        else
        {
          this->Off();
        }
        break;
      case nStOn:
        this->On();
        break;
      case nStOff:
        this->Off();
        break;

      case nStSinglePuls1000ms:
        if (mCounter < 10)
        {
          this->On();
        }
        else
        {
          vSetMode(nStOff);
        }
        return;
        break;
      case nStSinglePuls2000ms:
        if (mCounter < 20)
        {
          this->On();
        }
        else
        {
          vSetMode(nStOff);
        }
        return;
        break;
      case nStSinglePuls3000ms:
        if (mCounter < 30)
        {
          this->On();
        }
        else
        {
          vSetMode(nStOff);
        }
        return;
        break;
      }

    if (mCounter < 49)
    {
      mCounter++;
    }
    else
    {
      mCounter = 0;
    }
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

    mfPidSetVoltIn = 5000.0f;
    mfPidSetVoltOut = 5000.0f;
    mfPidSetCurrIn = 200.0f;
    mfPidSetCurrOut = 100.0f;

    mfPidInCurrOutput = 0.0f;
    mfPidInVoltOutput = 0.0f;
    mfPidOutCurrOutput = 0.0f;
    mfPidOutVoltOutput = 0.0f;

    mu8PidModeIn = 255;
    mu8PidModeInAlt = mu8PidModeIn;
    mu8PidModeOut = 255;
    mu8PidModeOutAlt = mu8PidModeOut;
    mfPidMaxCurr = 500.0f;   // [mA]

    mfPidSetVolt = 10000.0f;
  }

  virtual void vSetPidOutputVoltage(float lfValue) { mfPidSetVoltOut = lfValue; }
  virtual void vSetPidOutputCurrentLimit(float lfValue) { mfPidSetCurrOut = lfValue; }
  virtual void vSetPidOutput(float lfVolt, float lfCurrentLimit) { vSetPidOutputVoltage(lfVolt); vSetPidOutputCurrentLimit(lfCurrentLimit); }
  virtual void vSetPidInputVoltage(float lfValue) { mfPidSetVoltIn = lfValue; }
  virtual void vSetPidInputCurrentLimit(float lfValue) { mfPidSetCurrIn = lfValue; }
  virtual void vSetPidInput(float lfVolt, float lfCurrentLimit) { vSetPidInputVoltage(lfVolt); vSetPidInputCurrentLimit(lfCurrentLimit); }

  virtual void vSetHwOutputVoltage(i16 li16Value) = 0;
  virtual void vSetHwOutputCurrentLimit(i16 li16Value) = 0;
  virtual void vSetHwOutput(i16 li16Volt, i16 li16CurrentLimit) { vSetHwOutputCurrentLimit(li16CurrentLimit); vSetHwOutputVoltage(li16Volt); }
  virtual void vSetHwInputVoltage(i16 li16Value) = 0;
  virtual void vSetHwInputCurrentLimit(i16 li16Value) = 0;
  virtual void vSetHwInput(i16 li16Volt, i16 li16CurrentLimit) { vSetHwInputCurrentLimit(li16CurrentLimit); vSetHwInputVoltage(li16Volt); }

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
      //GPIOB->BSRR = (1 << 8);
    }
    mbEnable = True;
  }

  virtual void vDisable()
  {
    if (mu8Slot == 0)
    {
      //GPIOB->BRR = (1 << 8);
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


class cBoardCntr
{
public:

  enum tenBoardCntrGeneral : u8
  {
    enINA_P1 = 0,
    enINA_P2 = 1,
  };

  enum tenBoardState : u8
  {
    enUV = 0,
    enOV = 1,
  };

  cI2cMaster* mcI2C;
  cINA3221*   mcP[2];
  u8 mu8InaErr;

  enum tenBoardInaChn : u8
  {
    // mcP[1] = Adr 0x41
    nSysIn = 0,  //  INA21: SysIn
    nPomoOutIn,  //  INA22: PomoOutIn
    nPomoOutOut, //  INA23: PomoOutOut
    // mcP[0] = Adr 0x40
    nComCh1,     //  INA13: CH1
    nComCh2,     //  INA12: CH2
    nComCh3,     //  INA11: CH3

  };

  u8     mu8ForceDirectRead;
  int32  mi32VBus[6];
  float  mfVBusAvg[6];
  int32  mi32IShunt[6];
  float  mfIShuntAvg[6];
  int32  mi32VShunt[6];
  float  mfVShuntAvg[6];


  bool  mbPowerDown;
  bool  mbPowerDownEnter;

  u8    mTestMode;
  u8    mInaCnt;

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
      //cVc = new cPmSwitch(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    case 2:
      //cVc = new cPmMt3608(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    case 3:
      //cVc = new cPmMt3608(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    case 4:
      //cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    case 5:
      cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    case 6:
      //cVc = new cPmMp8859(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot, mcI2C);
      break;
    case 7:
      //cVc = new cPmTp4056(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    default:
      cVc = new cPmDummy(lu16Voltage_mV, lu16CurrentLimit_mA, lu8Slot);
      break;
    }
    cVc->mType = lu8PmType;
    return cVc;
  }


  cBoardCntr(cI2cMaster* lcI2C)
  {
    mcI2C = lcI2C;

    mInaCnt = 2;
    mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 50, 50, 50);
    mcP[1] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_1, 50, 50, 50);
    mcP[0]->i8Setup();
    mcP[1]->i8Setup();

    //mcPowerOut = cGetVoltageConverter(u16GetRomConstModulOutSetVoltOutput(), u16GetRomConstModulOutSetCurrOutput(), u8GetRomConstModulOutType(), 0);
    mcPowerOut = cGetVoltageConverter(5000, 100, 5, 0);

    mu8InaErr = 0;


    u8 lui8t;
    for (lui8t = 0; lui8t < 6; lui8t++)
    {
      mi32VBus[lui8t] = 0;
      mi32IShunt[lui8t] = 0;
      mi32VShunt[lui8t] = 0;
    }

    mTestMode = 0;

    mbPowerDown = False;
    mbPowerDownEnter = False;
    mu8ForceDirectRead = True;

    //mcPowerOut->mType = u8GetRomConstModulOutType();
    //
    //mcPowerOut->mVoltage_mVmax = u16GetRomConstModulOutVoltMax();
    //mcPowerOut->mVoltage_mVmin = u16GetRomConstModulOutVoltMinSupply();
    //
    //mcPowerOut->mcLpOutCurr.mFactor = fGetRomConstModulOutOutCurrLpIn();
    //mcPowerOut->mcPidOutCurr.mfKp = fGetRomConstModulOutOutCurrKp();
    //mcPowerOut->mcPidOutCurr.mfKi = fGetRomConstModulOutOutCurrKi();
    //mcPowerOut->mcPidOutCurr.mfKd = fGetRomConstModulOutOutCurrKd();
    //mcPowerOut->mcPidOutCurr.mfOutputLimit = fGetRomConstModulOutOutCurrKiCut();
    //mcPowerOut->mcPidOutCurr.mLpOutput.mFactor = fGetRomConstModulOutOutCurrLpOut();
    //
    //mcPowerOut->mcLpOutVolt.mFactor = fGetRomConstModulOutOutVoltLpIn();
    //mcPowerOut->mcPidOutVolt.mfKp = fGetRomConstModulOutOutVoltKp();
    //mcPowerOut->mcPidOutVolt.mfKi = fGetRomConstModulOutOutVoltKi();
    //mcPowerOut->mcPidOutVolt.mfKd = fGetRomConstModulOutOutVoltKd();
    //mcPowerOut->mcPidOutVolt.mfOutputLimit = fGetRomConstModulOutOutVoltKiCut();
    //mcPowerOut->mcPidOutVolt.mLpOutput.mFactor = fGetRomConstModulOutOutVoltLpOut();
    //
    //mcPowerOut->mfPidSetVoltIn = u16GetRomConstModulOutSetVoltInput();
    //mcPowerOut->mfPidSetVoltOut = u16GetRomConstModulOutSetVoltOutput();
    //mcPowerOut->mfPidSetCurrIn = u16GetRomConstModulOutSetCurrInput();
    //mcPowerOut->mfPidSetCurrOut = u16GetRomConstModulOutSetCurrOutput();
    //mcPowerOut->mfPidMaxVolt = u16GetRomConstChrgMaxVoltCell() * mCellCnt;
    //mcPowerOut->mfPidSetVolt = mcPowerOut->mfPidSetVoltOut;
    //
    //mcPowerOut->mu8PidModeIn = u8GetRomConstModulOutModeInput();
    //mcPowerOut->mu8PidModeOut = u8GetRomConstModulOutModeOutput();
  }

  bool bPowerDown(cStr& lcStr)
  {
    bool lbRet = True;

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


  u8 u8Ina2Idx(const u8 lu8InaIdx, const u8 lu8InaChl)
  {
    return (lu8InaIdx - 1) * 2 + (lu8InaChl - 1);
  }



  i16 i16GetVoltageAbs(tenBoardInaChn lenIdx)
  {
    return (i16)fGetVoltageAbs(lenIdx);
  }

  float fGetVoltageAbs(tenBoardInaChn lenIdx)
  {
    float lfValue = 0;
    switch (lenIdx)
    {
      case nSysIn:      lfValue = mfVBusAvg[u8Ina2Idx(2, 1)]; break; // INA21: SysIn
      case nPomoOutIn:  lfValue = mfVBusAvg[u8Ina2Idx(2, 2)]; break; // INA22: PomoOutIn
      case nPomoOutOut: lfValue = mfVBusAvg[u8Ina2Idx(2, 3)]; break; // INA23: PomoOutOut
      case nComCh1:     lfValue = mfVBusAvg[u8Ina2Idx(1, 3)]; break; // INA13: CH1
      case nComCh2:     lfValue = mfVBusAvg[u8Ina2Idx(1, 2)]; break; // INA12: CH2
      case nComCh3:     lfValue = mfVBusAvg[u8Ina2Idx(1, 1)]; break; // INA11: CH3
    }
    return lfValue;
  }

  i16 i16GetCurrentAbs(tenBoardInaChn lenIdx)
  {
    return (i16)fGetCurrentAbs(lenIdx);
  }

  float fGetCurrentAbs(tenBoardInaChn lenIdx)
  {
    float lfValue = 0;
    switch (lenIdx)
    {
      case nSysIn:      lfValue = mfIShuntAvg[u8Ina2Idx(2, 1)]; break; // INA21: SysIn
      case nPomoOutIn:  lfValue = mfIShuntAvg[u8Ina2Idx(2, 2)]; break; // INA22: PomoOutIn
      case nPomoOutOut: lfValue = mfIShuntAvg[u8Ina2Idx(2, 3)]; break; // INA23: PomoOutOut
      case nComCh1:     lfValue = mfIShuntAvg[u8Ina2Idx(1, 3)]; break; // INA13: CH1
      case nComCh2:     lfValue = mfIShuntAvg[u8Ina2Idx(1, 2)]; break; // INA12: CH2
      case nComCh3:     lfValue = mfIShuntAvg[u8Ina2Idx(1, 1)]; break; // INA11: CH3
    }
    return lfValue;
  }


  bool isExternalSupplyAvailable()
  {
    return True;
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
    if (mTestMode) return True;

    return False;
  }

  void vUpdateAll_10ms(bool bIsAuto)
  {
    if (!mbPowerDown)
    {
      u8 lui8t;
      u8 lui8i;

      volatile float lfInputFactor;

      //lfInputFactor = fGetRomConstModulInOutVoltLpIn();
      lfInputFactor = 0.5f;

      bCheckInaError();


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

            mi32VBus[lui8Idx] = mcP[lui8t]->i32CalcVBus_uV(lui8i) / 1000;
            mi32IShunt[lui8Idx] = mcP[lui8t]->i32CalcIShunt_uA(lui8i) / 1000;
            mi32VShunt[lui8Idx] = mcP[lui8t]->i32CalcVShunt_uV(lui8i) / 1000;

            if (mu8ForceDirectRead)
            {
              mu8ForceDirectRead = False;
              mfVBusAvg[lui8Idx] = mi32VBus[lui8Idx];
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
      }
      else // InaError
      {
        if (bIsAuto)
        {
          mcPowerOut->vDisable();
        }
      }
    }
    else
    {
      if (mbPowerDownEnter)
      {
        mbPowerDownEnter = False;

        if (bIsAuto)
        {
          mcPowerOut->vDisable();
        }
      }
    }
  }
};

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

class cStages;

class cStage5
{
public:
  cGpPin mcWakeupPin;
  cStatusLed<GPIOB_BASE, 9> mcStatusLed;

  // I2C2:
  cGpPin mcI2c2_SCL_Board;
  cGpPin mcI2c2_SDA_Board;

  cI2cMaster  mcI2C2_Board;

  cBoardCntr  mcBoardCntr;

  osThreadId_t mTaskBoard2Handle;
  bool         mbInitBoard2Finished;

  cStage5();

  void vInit()
  {
  }

  void vDoInitBoard2(cStages* lpcStages);
  void vDoRunBoard2(cStages* lpcStages);

  bool isInit()
  {
    return (mbInitBoard2Finished && True);
  }

};

extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S5_H__
