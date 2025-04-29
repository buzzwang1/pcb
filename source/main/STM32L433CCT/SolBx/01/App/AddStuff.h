#ifndef __ADD_STUFF_H__
#define __ADD_STUFF_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_dac.h"

#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "ina219.h"
#include "cStrT.h"
#include "cClock.h"

#include "ClockInfo.h"


class cWorkMode
{
  public:

  typedef enum
  {
    nStManuel,
    nStAuto,
  }tenState;

  tenState mMode;
  u16      mCounter;

  cWorkMode()
  {
    mCounter = 0;
    vSet(cWorkMode::nStAuto);
  }

  void vSet(cWorkMode::tenState lMode)
  {
    mMode = lMode;
    if (mMode == cWorkMode::nStManuel)
    {
      mCounter = 60*10; // 10min
    }
  }

  bool IsAuto()
  {
    return (mMode == cWorkMode::nStAuto);
  }

  void vTick1000ms()
  {
    if (mCounter)
    {
      mCounter--;
      if (mCounter == 0)
      {
        vSet(cWorkMode::nStAuto);
      }
    }
  }

  const char* ToString()
  {
    if (mMode == cWorkMode::nStManuel)
    {
      return (const char*)"manuel";
    }
    else
    {
      return (const char*)"auto";
    }
  }
};

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class cStatusLed : public LED<mpstPort, mui16Pin>
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
  }tenState;

  tenState mSm;
  u8       mCounter;

  cStatusLed()
   : LED<mpstPort, mui16Pin>()
  {
    mSm = nStToggle500ms;
    mCounter = 0;
  };

  void vSetMode(tenState lSm)
  {
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
        if ((mCounter == 0) || (mCounter == 5))
        {
          this->Toggle();
        }
        break;
      case nStToggle1000ms:
        if (mCounter == 0)
        {
          this->Toggle();
        }
        break;
      case nStPuls500ms:
        if ((mCounter == 0) || (mCounter == 5))
        {
          this->On();
        }
        else
        {
          this->Off();
        }
        break;
      case nStPuls1000ms:
        if (mCounter == 0)
        {
          this->On();
        }
        else
        {
          this->Off();
        }
        break;
    }

    if (mCounter < 10)
    {
      mCounter++;
    }
    else
    {
      mCounter = 0;
    }
  }
};


class cPowerMonitor
{
  public:

  typedef enum
  {
    // Events für allgemeine Verwenung
    MAIN_nEvReset = 0,
    MAIN_nEvStart,
    MAIN_nEvStop,

    MAIN_nEvDummy
  }tenEvents;

  typedef enum
  {
    MAIN_nSmIdle = 0,

    // States für ADC
    MAIN_nSmAdcRun,

    MAIN_nSmDisplayIdle,
    MAIN_nSmDisplayOn,
    MAIN_nSmDisplayPowerSave,

    MAIN_nSmDummy
  }tenStates;

  typedef enum
  {
    MAIN_nTempOft = (uint32)((270)),                    // Temperaturoffset

    MAIN_nVDummy
  }tenConsts;


  typedef enum
  {
    MAIN_nButton_1 = 0,
    MAIN_nButton_2,
    MAIN_nButton_3,
    MAIN_nButton_4,
    MAIN_nButton_Max
  }tenButtons;

  typedef enum
  {
    MAIN_nS1 = 0,
    MAIN_nS2,
    MAIN_nS3,
    MAIN_nS_Max
  }tenSwitches;

  typedef enum
  {
    MAIN_nGuiTopMenu_FirstEntry = 0,
    MAIN_nGuiTopMenu_Status = cPowerMonitor::MAIN_nGuiTopMenu_FirstEntry,
    MAIN_nGuiTopMenu_Konsole,
    MAIN_nGuiTopMenu_Info,
    MAIN_nGuiTopMenu_Einstellungen,
    MAIN_nGuiTopMenu_LastEntry
  }tenGuiTopMenu;

  typedef enum
  {
    MAIN_nGuiTopMenuSub_Status_FirstEntry = 0,
    MAIN_nGuiTopMenuSub_Status_Power = MAIN_nGuiTopMenuSub_Status_FirstEntry,
    MAIN_nGuiTopMenuSub_Status_ADC1,
    MAIN_nGuiTopMenuSub_Status_ADC2,
    MAIN_nGuiTopMenuSub_Status_ADC3,
    MAIN_nGuiTopMenuSub_Status_ADC4,
    MAIN_nGuiTopMenuSub_Status_DCF,
    MAIN_nGuiTopMenuSub_Status_Funk,
    MAIN_nGuiTopMenuSub_Status_LastEntry
  }tenGuiTopMenuSub_Status;

  typedef enum
  {
    MAIN_nGuiTopMenuSub_Einstellungen_FirstEntry = 0,
    MAIN_nGuiTopMenuSub_Einstellungen_Dimmung = MAIN_nGuiTopMenuSub_Einstellungen_FirstEntry,
    MAIN_nGuiTopMenuSub_Einstellungen_LastEntry = 0
  }tenGuiTopMenuSub_Einstellungen;

  typedef struct
  {
    tenGuiTopMenu                  enTopTab;
    tenGuiTopMenuSub_Status        enTopSubTab_Status;
    tenGuiTopMenuSub_Einstellungen enTopSubTab_Einstellungen;
  } tstGuiTabs;

  typedef struct
  {
    //Screen          stDisplay;
    //BPP1_1G_RFont   RFont01;
    //BPP1_1G_RFont   RFont5x5UCase;

    //stPxli16                    astGraph1_Points[24];
    //BPP1_1G_tstGraph            stGraph1;
    //BPP1_1G_tstGraph_i16Channel stGraph1_Chn1;

    //stPxli16                    astGraph2_Points[100];
    //BPP1_1G_tstGraph            stGraph2;
    //BPP1_1G_tstGraph_i16Channel stGraph2_Chn1;

  } tstGraphics;


  struct tstGlobals
  {
    public:

    uint8             ui8DCF;
    uint8             aui8Switches[3];
    uint8             aui8Buttons[4];
    uint8             ui8PowerTimer;
    int32             i32U_Supply;   // Direkt vom Sensor
    int32             i32I_Supply;   // Direkt vom Sensor
    
    float             fU_BatAvg;     // i32U_Bat mit 1min Dämpfung
    float             fI_BatAvg;     // i32I_Bat mit 1min Dämpfung

    int32             i32U_Bat;  //  Direkt vom Sensor
    int32             i32I_Bat;  //  Direkt vom Sensor
    int32             i32I_Sys_NoRadio_NoDisplay; // i32I_Bat, wenn Funk und Display aus
    uint8             ui8DimValue;

    
    u32               u32PowerCounterOut; // Summiert den Strom auf, der von der Akku kommt
    u32               u32PowerCounterIn;  // Summiert den Strom auf, der in der Akku kommt
                                          // Summiert nur auf bei keiner Netzversorgung (aui8Switches[cPowerMonitor::MAIN_nS1] == 0)

    uint8             ui8AdcIdx;
    //Fifo16H           stADC_1h[4];
    //Fifo16H           stADC_1d[4];

    cClock            mClock;
    char8             mBufStr[64];
    cStr              mcStr;

    tstGlobals()
      : mcStr(mBufStr, 64)
    {}

  };

  tstGuiTabs  stGuiTabs;
  tstGraphics stGfx;
  tstGlobals  stGlobals;
  cWorkMode  *mcWorkMode;

  cPowerMonitor(cWorkMode *lcWorkMode)
  {
    /*MAIN_TmpStr = null;
    DStr8_Create(&MAIN_TmpStr, 40);

    {
      uint8 lui8;
      mpstMain->stGlobals.ui8AdcIdx = 0;

      for (lui8 = 0; lui8 < 4; lui8++)
      {
        Fifo16_i8Create(&mpstMain->stGlobals.stADC_1h[lui8], 24);
        Fifo16_i8Create(&mpstMain->stGlobals.stADC_1d[lui8], 100);
      }
    }*/

    stGlobals.mClock.mDate.stDate.ui16Year = 2020;
    stGlobals.mClock.mDate.stDate.ui8Month = 8;
    stGlobals.mClock.mDate.stDate.ui8Day   = 5;

    stGlobals.mClock.mTime.stTime.ui8Hour      = 14;
    stGlobals.mClock.mTime.stTime.ui8Minute    = 5;
    stGlobals.mClock.mTime.stTime.ui8Second    = 1;
    stGlobals.mClock.mTime.stTime.ui8SubSecond = 0;

    //CLOCK_vRTC_Set(CLOCK_ui32TimeDate_to_32Bit(&mpstMain->stGlobals.stClock));

    stGlobals.aui8Buttons[MAIN_nButton_1] = 0;
    stGlobals.aui8Buttons[MAIN_nButton_2] = 0;
    stGlobals.aui8Buttons[MAIN_nButton_3] = 0;
    stGlobals.aui8Buttons[MAIN_nButton_4] = 0;

    stGlobals.aui8Switches[MAIN_nS1] = 0;
    stGlobals.aui8Switches[MAIN_nS2] = 1;
    stGlobals.aui8Switches[MAIN_nS3] = 0;

    stGlobals.ui8DCF = 0;
    stGlobals.ui8DCF = 0;

    stGlobals.ui8DimValue = 25;

    stGlobals.i32U_Supply = 0;
    stGlobals.i32I_Supply = 0;
    stGlobals.i32U_Bat    = 4000;
    stGlobals.i32I_Bat    = 20;
    stGlobals.i32I_Sys_NoRadio_NoDisplay = 0;

    stGlobals.u32PowerCounterOut = 0;
    stGlobals.u32PowerCounterIn  = 0; 

    stGlobals.fU_BatAvg = 4000;
    stGlobals.fI_BatAvg = 10;


    stGuiTabs.enTopTab = MAIN_nGuiTopMenu_FirstEntry;
    stGuiTabs.enTopSubTab_Status = MAIN_nGuiTopMenuSub_Status_FirstEntry;
    stGuiTabs.enTopSubTab_Einstellungen = MAIN_nGuiTopMenuSub_Einstellungen_FirstEntry;

    stGlobals.ui8PowerTimer = 0;

    mcWorkMode = lcWorkMode;
  }

  void vInitPowerTimer(void)
  {
    if (mcWorkMode->IsAuto())
    {
      stGlobals.ui8PowerTimer = 10;
    }
  }
};



#endif  //__ADD_STUFF_H__
