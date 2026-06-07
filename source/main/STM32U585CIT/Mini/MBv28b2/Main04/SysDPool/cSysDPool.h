#pragma once

#include "Typedef.h"
#include "cBuRam.h"
#include "cWufHandler.h"
#include "ClockInfo.h"
#include "cErr.h"
#include "cClock.h"
#include "cI2c.h"
#include "TPS55288.h"
#include "BQ25798.h"


struct cDPoolSys
{
  struct cSleep
  {
    u8 u8WakeUpTime_s;
    u8 u8NoSleepAfterPort_s;
  };

  cClockInfo   mcClockInfo;
  cBuRam       mcBuRam;
  cWufHandler  mcWufHandler;
  cErr         mcErr;
  cClock*      mpcClock;
  cSleep       mcSleep;
};

struct cDPoolBoard
{
  struct cLogicPower
  {
    enum tenLogicPower : u8
    {
      nP1_2V09 = 0,
      nP2_2V75,
      nP3_3V26
    };

    tenLogicPower menLogicPower;
  }mLogicPower;

  struct cMonitor
  {
    float     mfComV_mV[3];
    float     mfComI_mA[3];

    float     mfSysInV_mV;
    float     mfSysInI_mA;
    float     mfPomoInV_mV;
    float     mfPomoInI_mA;
    float     mfPomoOutV_mV;
    float     mfPomoOutI_mA;

    u8        mu8SysVoltOk;

    u8        mu8PomoOut1TmpErr;
    u8        mu8I2cInaErr;
  }mcMonitor;

  struct 
  {
    cTPS55288* mpcTPS55288;
    float      mfTemp_C;

    cTPS55288_RegisterMap::tunStatus munStatus;
    u8         mu8I2cTps55288Err;
    u8         mu8I2cTempErr;
  }mcPomoOut1;

  struct 
  {
    cBQ25798*  mpcBQ25798;

    float     mfIBus_mA;
    float     mfIBat_mA;
    float     mfVBus_mV;
    float     mfVAC1_mV;
    float     mfVAC2_mV;
    float     mfVBat_mV;
    float     mfVSys_mV;

    float     mfTSens_C;
    float     mfTDie_C;

    u8         mu8I2cErr;
  }mcCharger;

  cI2cMaster* mcI2c;

  struct
  {
    struct
    {
      cGpPin* mOut5V0Ch1;
      cGpPin* mOut5V0Ch2;
      cGpPin* mOutBat;
    }mcOut;
  }mcAddon;


  u16         mu16LedMode;
};


struct cSysDPool
{
  static cDPoolSys   mSys;
  static cDPoolBoard mBoard;
};


cDPoolSys   cSysDPool::mSys;
cDPoolBoard cSysDPool::mBoard;




