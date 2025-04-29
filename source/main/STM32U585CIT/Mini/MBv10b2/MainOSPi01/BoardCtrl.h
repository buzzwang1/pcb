#ifndef __BOARD_CONTROL_H__
#define __BOARD_CONTROL_H__


#include "TypeDef.h"
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

#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "ina3221.h"

#include "cClock.h"
#include "ClockInfo.h"

#include "RomConst.h"

class cSystemPowerDown
{
public:
  virtual bool bContinue() = 0;           // Continue after PowerDown
  virtual bool bExitRun() = 0;            // 1. stage PowerDown. Should be used to inform connected devices to go to powerdown
  virtual bool bPreparePowerdown() = 0;   // 2. stage. Stop communication
  virtual bool bGotoPowerDown() = 0;    // 3. goto powerdeon. Turn everything off, that is not needed.
                                          //    Bring everything to a defined/safe state
                                          //    Set WakeUp Sources

  virtual void vEnterPowerDown(u16 lu16WakeUpTime) = 0;
};


class cPowerManager
{
public:
  typedef enum
  {
    nStIdle = 0,
    nStWaitForRun,
    nStWaitForExitRun,
    nStWaitForPreparePowerdown,
    nStWaitForGotoPowerDown
  }tenStates;

  typedef enum
  {
    nStInit = 0,
    nStRun,
    nStPreparePowerDown,
    nStWaitForPowerDown,
  }tenSysStates;

  typedef struct
  {
    u16 isWuTimerRtc : 1;
    u16 isWuStandBy : 1;
    u16 isWuPin1 : 1;
    u16 isWuPinRst : 1;
    u16 isWuSftRst : 1;
  }tstWakeupSources;

  typedef union
  {
    u16              u16WakeupSources;
    tstWakeupSources stWakeupSources;
  }tunWakeupSources;

  u16      mu16WakeUpTime;
  u16      mu16DRunTimer;
  u16      mu16DRunTimerReload;
  u16      mu16DExitRunTimer;
  u16      mu16DExitRunTimerReload;
  u16      mu16PreparePowerDownTimer;
  u16      mu16PreparePowerDownTimerReload;


  tunWakeupSources munWuSources;

  cSystemPowerDown* mcPowerDown;

  tenStates    enSm;
  tenSysStates enSysState;


  cPowerManager(u16 lu16DRunTimer, u16 lu16DExitRunTimer,
    u16 lu16PreparePowerDownTimer, u16 lu16WakeUpTime,
    cSystemPowerDown* lcPowerDown)
  {
    enSm = nStIdle;
    enSysState = nStInit;

    mu16DRunTimer = lu16DRunTimer;
    mu16DRunTimerReload = mu16DRunTimer;
    mu16DExitRunTimer = lu16DExitRunTimer;
    mu16DExitRunTimerReload = mu16DExitRunTimer;
    mu16PreparePowerDownTimer = lu16PreparePowerDownTimer;
    mu16PreparePowerDownTimerReload = mu16PreparePowerDownTimer;

    mu16WakeUpTime = lu16WakeUpTime;

    mcPowerDown = lcPowerDown;

    vCheckWakeuUpSources();
  }

  void vContinueAfterWakeup()
  {
    if (mcPowerDown->bContinue())
    {
      enSm = nStWaitForRun;
      vStart();
    }
  }

  void vCheckWakeuUpSources()
  {
    munWuSources.u16WakeupSources = 0;

    // Wakeup Timer RTC
    if (LL_RTC_IsActiveFlag_WUT(RTC))
    {
      /* Reset RTC Internal Wake up flag */
      LL_RTC_DisableWriteProtection(RTC);

      LL_RTC_ClearFlag_WUT(RTC);
      /* Disable wake up timer to modify it */
      LL_RTC_WAKEUP_Disable(RTC);
      LL_RTC_DisableIT_WUT(RTC);

      LL_RTC_EnableWriteProtection(RTC);

      munWuSources.stWakeupSources.isWuTimerRtc = True;
    }

    /* Check if the system was resumed from Standby mode */
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
    {
      /* Clear Standby flag */
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
      munWuSources.stWakeupSources.isWuStandBy = True;
    }

    if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF1))
    {
      /* Clear Standby flag */
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
      munWuSources.stWakeupSources.isWuPin1 = True;
    }

    // RCC_FLAG_LSERDY   LSE oscillator clock ready
    // RCC_FLAG_LSECSSD  Clock security system failure on LSE oscillator detection
    // RCC_FLAG_LSIRDY   LSI oscillator clock ready
    // RCC_FLAG_BORRST   BOR reset
    // RCC_FLAG_OBLRST   OBLRST reset
    // RCC_FLAG_PINRST   Pin reset
    // RCC_FLAG_FWRST    FIREWALL reset
    // RCC_FLAG_SFTRST   Software reset
    // RCC_FLAG_IWDGRST  Independent Watchdog reset
    // RCC_FLAG_WWDGRST  Window Watchdog reset
    // RCC_FLAG_LPWRRST  Low Power reset
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) { munWuSources.stWakeupSources.isWuPinRst = True; }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) { munWuSources.stWakeupSources.isWuSftRst = True; }

    __HAL_RCC_CLEAR_RESET_FLAGS();
  }


  void vStart()
  {
    if ((enSm == nStIdle) || (enSm == nStWaitForRun))
    {
      mu16DRunTimer = mu16DRunTimerReload;
      mu16DExitRunTimer = mu16DExitRunTimerReload;
      mu16PreparePowerDownTimer = mu16PreparePowerDownTimerReload;
      enSm = nStWaitForRun;
    }
  }

  void vTick10ms()
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;
      switch (enSm)
      {
      case nStIdle:
        break;
      case nStWaitForRun:
        enSysState = nStRun;

        if (mu16DRunTimer)
        {
          mu16DRunTimer--;
        }
        else
        {
          if (mcPowerDown->bExitRun())
          {
            enSm = nStWaitForExitRun;
          }
        }
        break;
      case nStWaitForExitRun:
        if (mu16DExitRunTimerReload)
        {
          mu16DExitRunTimerReload--;
        }
        else
        {
          enSysState = nStPreparePowerDown;
          if (mcPowerDown->bPreparePowerdown())
          {
            enSm = nStWaitForPreparePowerdown;
          }
        }
        break;
      case nStWaitForPreparePowerdown:
        if (mu16PreparePowerDownTimer)
        {
          mu16PreparePowerDownTimer--;
        }
        else
        {
          enSysState = nStWaitForPowerDown;
          if (mcPowerDown->bGotoPowerDown())
          {
            enSm = nStWaitForGotoPowerDown;
            lbLoop = True;
          }
        }
        break;
      case nStWaitForGotoPowerDown:

        mcPowerDown->vEnterPowerDown(mu16WakeUpTime);

        if (mcPowerDown->bContinue())
        {
          enSm = nStWaitForRun;
          vStart();
        }
        break;
      default:
        break;
      }
    }
  }
};



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
      mCounter = 60 * 10; // 10min
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


class cDigiPower
{
public:

  enum
  {
    nPin1 = 10
  };

  //Digi Power
  cGpPin lcDigi1;

  cDigiPower(u8 lu8InitState = 0)
    : lcDigi1(GPIOA_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, lu8InitState)
  {
  }

  void vPowerOn()
  {
    GPIOA->BSRR = (1 << nPin1);
  }

  void vPowerOff()
  {
    GPIOA->BRR = (1 << nPin1);
  }
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
  u16 mVoltage_mV;
  bool mbEnable;

  cVoltageConverter()
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    mVoltage_mV = 0;
    mbEnable = False;
  }

  virtual void vSetVoltage(u16 lu16Value) = 0;
  u16  u16GetVoltage()
  {
    return mVoltage_mV;
  }

  virtual void vEnable() = 0;
  virtual void vDisable() = 0;

  bool isEnable()
  {
    return mbEnable;
  }
};

class cVoltageConverterIn : public cVoltageConverter
{
public:
  // PA4 DAC1 Channel1
  // PB8 Enable

  enum
  {
    nPinEn = 7
  };

  cVoltageConverterIn(u16 lu16Value)
    :cVoltageConverter()
  {
    cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mEnable(GPIOA_BASE, nPinEn, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1 /* In-Converter default an */);
    mbEnable = True;

    LL_DAC_InitTypeDef DAC_InitStructure;

    /* DAC channel1 Configuration */
    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE; // Enable, sonst bricht die ausgangsspannung zusammen
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_InitStructure);
    mVoltage_mV = lu16Value;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, u16VoltToDigit(mVoltage_mV));
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  }

  void vSetVoltage(u16 lu16Value) override
  {
    mVoltage_mV = lu16Value;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, u16VoltToDigit(mVoltage_mV));
  }

  u16 u16VoltToDigit(u16 lu16Value)
  {
    lu16Value = cAnalogConvert::u16GetDigi(17, u16GetRomConstPowerInDac(), u16GetRomConstPowerInVout(), lu16Value);

    if (lu16Value > 4095) lu16Value = 4095;
    return (u16)lu16Value;
  }


  void vEnable() override
  {
    GPIOA->BSRR = (1 << nPinEn);
    mbEnable = True;
  }

  void vDisable() override
  {
    GPIOA->BRR = (1 << nPinEn);
    mbEnable = False;
  }
};

class cVoltageConverterOut : public cVoltageConverter
{
public:
  // PA5  DAC1 Channel2
  // PA15 Enable

  enum
  {
    nPinEn = 8
  };


  cVoltageConverterOut(u16 lu16Value)
  {
    cGpPin mDac(GPIOA_BASE, 5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mEnable(GPIOB_BASE, nPinEn, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    LL_DAC_InitTypeDef DAC_InitStructure;

    /* DAC channel1 Configuration */
    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE; // Enable, sonst bricht die ausgangsspannung zusammen
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_2, &DAC_InitStructure);
    mVoltage_mV = lu16Value;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, u16VoltToDigit(mVoltage_mV));
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_2);
  }

  void vSetVoltage(u16 lu16Value) override
  {
    mVoltage_mV = lu16Value;
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, u16VoltToDigit(mVoltage_mV));
  }

  u16 u16VoltToDigit(u16 lu16Value)
  {
    // Digi = ((0,8/R2 - ((lu16Value/1000)-0,8)/R1)) * Rb + 0,8) * 4094/3,3
    // ^Widerstände in kOhm R1 = 100; R2 = 10; Rb = 47

    float lfU_Out = (float)lu16Value / 1000.0f + 0.2f; //0,2 ist Offset
    float lfZwErg = (((0.8f / 10.0f) - ((lfU_Out - 0.8f) / 100.0f)) * 47.0f + 0.8f) * 4096.0f / 3.3f;

    u32 lu32Value = (u32)lfZwErg;
    if (lu32Value > 4095) lu32Value = 4095;
    return (u16)lu32Value;
  }

  void vEnable() override
  {
    GPIOB->BSRR = (1 << nPinEn);
    mbEnable = True;
  }

  void vDisable() override
  {
    GPIOB->BRR = (1 << nPinEn);
    mbEnable = False;
  }
};


class cLipoMon
{
public:

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
    enSmStNotCharging,
    enSmStNotCharging_Error_LowInputVoltage,
    enSmStNotCharging_Error_CellsDefect,
    enSmStChargingSave,
    enSmStChargingNormal,
    enSmStCharged
  }tenLipoMonLipo_States;


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
  // mcP[0] = Adr 0x40
  //  Kanal11: Eingang  (Vor Charger)
  //  Kanal12: Batterie/System (Nach Charger); Spannung Bat4 + Spannungsabfall am Widerstand über Bat 4
  //  Kanal13: Ausgang (Nach Ausgangs Konverter)
  //
  // mcP[1] = Adr 0x41
  //  Kanal21: Spannung an Bat3
  //  Kanal22: Spannung an Bat2
  //  Kanal23: Spannung an Bat1

  u8     mu8ForceDirectRead;
  int32  mi32VBus[6];
  float  mfVBusAvg[6];
  int32  mi32IShunt[6];
  float  mfIShuntAvg[6];
  int32  mi32VShunt[6];
  float  mfVShuntAvg[6];

  float  mfVBat[4];

  uint8 mui8P2Mode;

  u16                   mBalancingActive;
  u16                   mu16UnderVoltageCnt[4];
  u16                   mu16OverVoltageCnt[4];

  tenLipoMonS_States    lenSmS[5];
  tenLipoMonLipo_States lenSmLipo;
  tenLipoMonLipo_States lenSmLipoPrev;

  int32  mi32LipoChargeSum_10ms;
  uint32 mui32LipoChargeTime_10ms;

  uint32   mui32LipoChargeCount1;
  uint32   mui32LipoChargeCount2;
  uint32   mui32LipoLoadCount;

  uint32   mu32ChargerReadyCnt;

  int32    mi32LipoCap;
  int32    mi32LipoCapMax;

  u16   mu16Startup_Delay;

  bool  mbPowerDown;
  bool  mbPowerDownEnter;

  tcGpPin<GPIOC_BASE, 13> mS_Lipo;
  tcGpPin<GPIOB_BASE, 0>  mS_BalancerLipo4;
  tcGpPin<GPIOA_BASE, 6>  mS_BalancerLipo3;
  tcGpPin<GPIOB_BASE, 2>  mS_BalancerLipo2;
  tcGpPin<GPIOA_BASE, 15> mS_BalancerLipo1;

  cVoltageConverterIn  mcPowerIn;
  cVoltageConverterOut mcPowerOut;

  cLipoMon(cI2cMaster* lcI2C)
    : mS_Lipo(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1),
    mS_BalancerLipo4(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    mS_BalancerLipo3(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    mS_BalancerLipo2(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    mS_BalancerLipo1(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    mcPowerIn(10000), mcPowerOut(6000)
  {
    mcI2C = lcI2C;

    mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 50, 50, 50);
    mcP[1] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_1, 50, 50, 50);


    u8 lui8t;

    mu16Startup_Delay = 1000; //[ms]

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

    for (lui8t = 0; lui8t <= 1; lui8t++)
    {
      mcP[lui8t]->i8Setup();
    }
    for (lui8t = 0; lui8t < 6; lui8t++)
    {
      mi32VBus[lui8t] = 0;
      mi32IShunt[lui8t] = 0;
      mi32VShunt[lui8t] = 0;
    }


    lenSmS[enS1_Lipo] = enSmStOn;
    lenSmS[enS_Balancer_Lipo1] = enSmStOff;
    lenSmS[enS_Balancer_Lipo2] = enSmStOff;
    lenSmS[enS_Balancer_Lipo3] = enSmStOff;
    lenSmS[enS_Balancer_Lipo4] = enSmStOff;

    for (lui8t = 0; lui8t < 4; lui8t++)
    {
      mu16UnderVoltageCnt[lui8t] = 0;
      mu16OverVoltageCnt[lui8t] = 0;
    }

    mbPowerDown = False;
    mbPowerDownEnter = False;
    mu8ForceDirectRead = 5;
  }

  bool bPowerDown()
  {
    bool lbRet = True;


    // Während Laden nicht runterfahren.
    if ((lenSmLipo == enSmStChargingNormal) ||
      (lenSmLipo == enSmStChargingSave))
    {
      lbRet = False;
    }


    // Während Output an, nicht runterfahren.
    if (mcPowerOut.isEnable())
    {
      lbRet = False;
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

  float fGetBatVoltageAbs(u8 lu8Idx)
  {
    float lfValue = 0;
    switch (lu8Idx)
    {
      // Wert verkleinern, verkleinert den ADC wert
      case 1: lfValue = mfVBusAvg[5] - 4; break; // -4 wegen Mosfet spannungsabfall
      case 2: lfValue = mfVBusAvg[4];     break;
      case 3: lfValue = mfVBusAvg[3];     break;
      case 4: lfValue = mfVBusAvg[1];     break;
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
    switch (lu8Idx)
    {
    
      case 1: lfValue = mfVBusAvg[0]; break; // Board Input
      case 2: lfValue = mfVBusAvg[1]; break; // Board Bat
      case 3: lfValue = mfVBusAvg[2]; break; // Board Output
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
    switch (lu8Idx)
    {
      case 1: lfValue = mfIShuntAvg[0]; break; // Board Input
      case 2: lfValue = mfIShuntAvg[1]; break; // Board Bat
      case 3: lfValue = mfIShuntAvg[2]; break; // Board Output
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


  void vUpdate_S1_Lipo()
  {
    // Battery Main Switch
    // If any Lipo is below enLipoUThres1 for 1s and not charging, then turn off the lipo
    u8 lui8t;
    i16 lui16Volt;

    switch (lenSmS[enS1_Lipo])
    {
    case enSmStOff:
      for (lui8t = 0; lui8t < 4; lui8t++)
      {
        lui16Volt = i16GetBatVoltageRel(lui8t + 1);

        if (lui16Volt > u16GetRomConstVoltLipoOff())
        {
          mu16UnderVoltageCnt[lui8t] += 10;
          if (mu16UnderVoltageCnt[lui8t] >= u16GetRomConstTimeOut1()) // Undervoltage detected for >2s
          {
            mS_Lipo.vSet1();
            mu8ForceDirectRead = 5;
            mu16UnderVoltageCnt[lui8t] = u16GetRomConstTimeOut1();
            lenSmS[enS1_Lipo] = enSmStOn;
          }
        }
        else
        {
          mu16UnderVoltageCnt[lui8t] = 0;
        }
      }
      break;
    case enSmStOn:
      for (lui8t = 0; lui8t < 4; lui8t++)
      {
        lui16Volt = i16GetBatVoltageRel(lui8t + 1);

        if (lui16Volt < u16GetRomConstVoltLipoOff())
        {
          mu16UnderVoltageCnt[lui8t] += 10;
          if ((mu16UnderVoltageCnt[lui8t] >= u16GetRomConstTimeOut1()) && // Undervoltage detected for >2s
            (mi32IShunt[1] < 0)) // Battery is discharging
          {
            mS_Lipo.vSet0();
            mu8ForceDirectRead = 5;
            mu16UnderVoltageCnt[lui8t] = u16GetRomConstTimeOut1();
            lenSmS[enS1_Lipo] = enSmStOff;
          }
        }
        else
        {
          mu16UnderVoltageCnt[lui8t] = 0;
        }
      }
      break;
    }
  }

  void vUpdate_S_Balancer(u8 lu8Idx)
  {
    i16 lui16Volt;

    if ((lu8Idx >= 1) && (lu8Idx <= 4))
    {

      lui16Volt = i16GetBatVoltageRel(lu8Idx);

      switch (lenSmS[lu8Idx])
      {
        case enSmStOff:
          if (lui16Volt > u16GetRomConstVoltBalance())
          {
            mu16OverVoltageCnt[lu8Idx - 1] += 10;;
            if (mu16OverVoltageCnt[lu8Idx - 1] >= u16GetRomConstTimeOut1()) // Overvoltage detected for >1s
            {
              mu16OverVoltageCnt[lu8Idx - 1] = u16GetRomConstTimeOut1();
              if (mi32IShunt[1] > 0) // Battery is charging
              {
                switch (lu8Idx)
                {
                case 1: mS_BalancerLipo1.vSet1(); mBalancingActive |= 1; break;
                case 2: mS_BalancerLipo2.vSet1(); mBalancingActive |= 2; break;
                case 3: mS_BalancerLipo3.vSet1(); mBalancingActive |= 4; break;
                case 4: mS_BalancerLipo4.vSet1(); mBalancingActive |= 8; break;
                }
                lenSmS[lu8Idx] = enSmStOn;
                mu16OverVoltageCnt[lu8Idx - 1] = 0;
              }
            }
          }
          break;
        case enSmStOn:
          // Balancer wieder abschalten, sobald man die Spannung der höchsten nicht gebalance'ten Zelle erreicht hat
          u8 lu8CellIdx;
          u16 lu16TurnOffVolt = 0;
          for (lu8CellIdx = 0; lu8CellIdx < u8GetRomConstCellCount(); lu8CellIdx++)
          {
            if ((mBalancingActive & (1 << lu8CellIdx)) == 0)
            {
              if (lu16TurnOffVolt < i16GetBatVoltageRel(lu8CellIdx + 1))
              {
                lu16TurnOffVolt = i16GetBatVoltageRel(lu8CellIdx + 1);
              }
            }
          }

          if (lu16TurnOffVolt == 0) lu16TurnOffVolt = u16GetRomConstMaxVoltCell();

          if (lui16Volt < lu16TurnOffVolt)
          {
            mu16OverVoltageCnt[lu8Idx - 1] += 10;;
            if (mu16OverVoltageCnt[lu8Idx - 1] >= u16GetRomConstTimeOut1()) // Exit Overvoltage detected for >1s
            {
              switch (lu8Idx)
              {
              case 1: mS_BalancerLipo1.vSet0(); mBalancingActive &= (255 - 1); break;
              case 2: mS_BalancerLipo2.vSet0(); mBalancingActive &= (255 - 2); break;
              case 3: mS_BalancerLipo3.vSet0(); mBalancingActive &= (255 - 4); break;
              case 4: mS_BalancerLipo4.vSet0(); mBalancingActive &= (255 - 8); break;
              }
              lenSmS[lu8Idx] = enSmStOff;
              mu16OverVoltageCnt[lu8Idx - 1] = 0;
            }
          }
          break;
      }
    }
  }



  void vUpdateCharger()
  {
    lenSmLipoPrev = lenSmLipo;

    {
      i16 li16I_ChargerLimit;
      i16 li16U_ChargerLimit;
      i16 li16I_SupplyLimit;

      u8 lui8t;
      i16 lui16SupplyCurrent;
      i16 lui16SupplyVolt;
      i16 lui16BatteryVolt;
      i16 lui16BatteryCurrent;


      lui16SupplyVolt = mfVBusAvg[0];
      lui16SupplyCurrent = mfIShuntAvg[0];
      lui16BatteryVolt = mfVBusAvg[1];
      lui16BatteryCurrent = mfIShuntAvg[1];

      li16I_ChargerLimit = u16GetRomConstMaxCurrentBat();
      li16U_ChargerLimit = u8GetRomConstCellCount() * u16GetRomConstMaxVoltCell();
      li16I_SupplyLimit = u16GetRomConstMaxCurrentInput();

      for (u8 lu8CellIdx = 1; lu8CellIdx <= u8GetRomConstCellCount(); lu8CellIdx++)
      {
        if (i16GetBatVoltageRel(lu8CellIdx) < u16GetRomConstVoltCellDefect())
        {
          lenSmLipo = enSmStNotCharging_Error_CellsDefect;
          mcPowerIn.vSetVoltage(u16GetRomConstVoltMinPowerIn());
          mcPowerIn.vDisable();
          return;
        }
      }

      if (lenSmLipo == enSmStNotCharging_Error_LowInputVoltage)
      {
        if (lui16SupplyVolt > u16GetRomConstVoltMinSupply())
        {
          lenSmLipo = enSmStNotCharging;
        }
        else
        {
          return;
        }
      }
      else
      //Überprüfen, ob es eine Eingangsspannung gibt
      if (lui16SupplyVolt < u16GetRomConstVoltMinSupply())
      {
        lenSmLipo = enSmStNotCharging_Error_LowInputVoltage;
        mcPowerIn.vSetVoltage(u16GetRomConstVoltMinPowerIn());
        mcPowerIn.vDisable();
        return;
      }

      // Während Laden nicht runterfahren.
      if ((lenSmLipo == enSmStChargingNormal) ||
        (lenSmLipo == enSmStChargingSave))
      {
        lenSmLipo = enSmStChargingNormal;

        // Falls eine Zelle Unterspannung hat, dann Ladestrom begrenzen
        for (lui8t = 1; lui8t <= u8GetRomConstCellCount(); lui8t++)
        {
          lui16BatteryVolt = i16GetBatVoltageRel(lui8t);
          if (lui16BatteryVolt < enLipoUThres1)
          {
            li16I_ChargerLimit = enLipoISave;
            lenSmLipo = enSmStChargingSave;
          }
        }

        if (mBalancingActive)
        {
          li16I_ChargerLimit = enLipoISave / 2;
          lenSmLipo = enSmStChargingSave;
        }

        lui16SupplyCurrent = mfIShuntAvg[0];
        lui16BatteryVolt    = mfVBusAvg[1];
        lui16BatteryCurrent = mfIShuntAvg[1];


        if ((lui16BatteryVolt < li16U_ChargerLimit) &&
            (lui16BatteryCurrent < li16I_ChargerLimit) &&
            (lui16SupplyCurrent < li16I_SupplyLimit))
        {
          i16 lui16BatteryVoltTemp;
          lui16BatteryVoltTemp = mcPowerIn.mVoltage_mV + 1;
          if (lui16BatteryVoltTemp >= u16GetRomConstVoltMaxPowerIn()) lui16BatteryVoltTemp = u16GetRomConstVoltMaxPowerIn();
          mcPowerIn.vSetVoltage(lui16BatteryVoltTemp);
          mcPowerIn.vEnable();
        }
        else
        {
          i16 lui16BatteryVoltTemp;
          lui16BatteryVoltTemp = mcPowerIn.mVoltage_mV - 1;
          if (lui16BatteryVoltTemp <= u16GetRomConstVoltMinPowerIn()) lui16BatteryVoltTemp = u16GetRomConstVoltMinPowerIn();
          mcPowerIn.vSetVoltage(lui16BatteryVoltTemp);
          mcPowerIn.vEnable();
        }

        if ((lui16BatteryVolt >= li16U_ChargerLimit - 10) && // Geladen ?
            (lui16BatteryVolt <= li16U_ChargerLimit + 10) &&
            (lui16BatteryCurrent < enLipoISave))
        {
          mu32ChargerReadyCnt += 10;

          if (mu32ChargerReadyCnt > u16GetRomConstTimeOut1())
          {
            mu32ChargerReadyCnt = 0;
            i16 lui16BatteryVoltTemp;
            lui16BatteryVoltTemp = mcPowerIn.mVoltage_mV - u8GetRomConstCellCount() * u16GetRomConstVoltPowerInCharged();
            mcPowerIn.vSetVoltage(lui16BatteryVoltTemp);
            mcPowerIn.vEnable();
            lenSmLipo = enSmStCharged;
          }
        }
        else
        {
          mu32ChargerReadyCnt = 0;
        }
      }
      else
        //if (lenSmLipo == enSmStCharged)
        {
          if (lui16BatteryVolt < li16U_ChargerLimit-500)
          {
            lenSmLipo = enSmStChargingNormal;
          }
        }
    }
  }

  void vUpdateAll(bool bIsAuto)
  {
    if (!mbPowerDown)
    {
      uint8 lui8t;
      uint8 lui8i;

      float lfInputFactor;

      lfInputFactor = fGetRomConstP_In();

      // Strom und Spannung einlesen
      for (lui8t = 0; lui8t <= 1; lui8t++)
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

      if (mu8ForceDirectRead)
      {
        mu8ForceDirectRead--;
      }

      mfVBat[0] = fGetBatVoltageAbs(1);
      mfVBat[1] = fGetBatVoltageAbs(2) - fGetBatVoltageAbs(1);
      mfVBat[2] = fGetBatVoltageAbs(3) - fGetBatVoltageAbs(2);
      mfVBat[3] = fGetBatVoltageAbs(4) - fGetBatVoltageAbs(3);

      for (lui8i = 1; lui8i <= 4; lui8i++)
      {
        fGetBatVoltageRel(lui8i);
      }

      if ((mu16Startup_Delay == 0) && (bIsAuto))
      {
        //Schalter setzten
        vUpdate_S1_Lipo();
        vUpdateCharger();
        for (u8 lu8CellIdx = 1; lu8CellIdx <= u8GetRomConstCellCount(); lu8CellIdx++)
        {
          if (lenSmLipo != enSmStNotCharging_Error_CellsDefect)
          {
            vUpdate_S_Balancer(lu8CellIdx);
          }
        }
      }
      else
      {
        if (mu16Startup_Delay > 10) mu16Startup_Delay -= 10;
                               else mu16Startup_Delay  = 0;
      }
    }
    else
    {
      if (mbPowerDownEnter)
      {
        mbPowerDownEnter = False;

        mS_BalancerLipo1.vSet0();
        mS_BalancerLipo2.vSet0();
        mS_BalancerLipo3.vSet0();
        mS_BalancerLipo4.vSet0();

        mcPowerOut.vDisable();
        mcPowerIn.vDisable();
      }
    }

  }
};



#endif  //__BOARD_CONTROL_H__
