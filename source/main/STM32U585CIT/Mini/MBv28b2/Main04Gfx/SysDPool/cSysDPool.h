#pragma once


#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"



#include "Typedef.h"
#include "cBuRam.h"
#include "cWufHandler.h"
#include "ClockInfo.h"
#include "cErr.h"
#include "cClock.h"
#include "cI2c.h"
#include "cSpi.h"
#include "cUsart_MpHd.h"
#include "TPS55288.h"
#include "BQ25798.h"
#include "ili9341.h"
#include "cNRF905.h"
#include "cursor.h"
#include "msg.h"



//Data
#include "cRingBufT.h"
#include "cBnSpop.h"
#include "cBnLinkI2c.h"
#include "cBnLinkUsartMpHd.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortBase.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortCmd.h"
#include "cBotnet.h"

template <u32 tu32MemInBytes>
struct tcTask
{
  constexpr u32 StackSize() { return (tu32MemInBytes / sizeof(StackType_t)); }

  osThreadId_t Handle;
  StaticTask_t Tcb;
  StackType_t  Stack[tu32MemInBytes / sizeof(StackType_t)];

  void vInit()
  {
    cMemTools::vMemSet((u8*)Stack, 0xAA, tu32MemInBytes);
  }
};

typedef tcTask<2048 * 1> tcTaskSmall;
typedef tcTask<2048 * 2> tcTaskMid;
typedef tcTask<2048 * 8> tcTaskLarge;


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

  struct cTasks
  {
    tcTaskSmall Idle;
    tcTaskSmall Mcp;

    struct cDepTree
    {
      tcTaskSmall Base;
      tcTaskMid   Init1;
      tcTaskMid   Init2;
      tcTaskSmall Tick1ms;
    };

    cDepTree    DepTree;
    tcTaskMid   Com;
    tcTaskLarge Gfx;
  };

  cTasks mcTasks;
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

  struct
  {
    cIli9341* mpcILI9341;
    cMsgBox*  mpcMsgBox;
    cCursor*  mpcCursor;

    u16  u16TouchRawX;
    u16  u16TouchRawY;

    u16  u16TouchRawXMax;
    u16  u16TouchRawYMax;
    u16  u16TouchRawXMin;
    u16  u16TouchRawYMin;

    u16  u16TouchX;
    u16  u16TouchY;
    
    u8   u8TouchIrq;
    u8   u8DisplayDim_Percent;

    bool bShowScreen;
  }mcGfx;


  cI2cMaster*  mcI2c;
  cSpiMasterMulti*  mcSpi2;

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

struct cPoolCom
{
  cGpPin* mpcCh1_EN;
  cGpPin* mpcCh2_EN;
  cGpPin* mpcCh3_EN;

  cI2cMaster* mcI2c3_Out;
  cI2cSlave*  mcI2c4_In;

  cUartMpHdMaster* mcU2_Out;
  cUartMpHdSlave*  mcU3_In;

  cBotNet*                 mpcBn;
  cNRF905BnSlave*          mpcNRF905Bn;
  cBotNet_UpLinknRf905Net* mpcSideLink;

  //cNRF905Slave*     mcNRF905BnSlave;
};


struct cPoolGfx
{
  u8* pu8GfxRam;
};


struct cSysDPool
{
  static cDPoolSys   mSys;
  static cDPoolBoard mBoard;
  static cPoolCom    mCom;
  static cPoolGfx    mGfx;
};



