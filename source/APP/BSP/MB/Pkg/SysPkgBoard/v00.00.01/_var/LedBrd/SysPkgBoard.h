
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


class cBoardCtrl
{
public:

  typedef enum
  {
    enS_Ch220V = 0,

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


  tcGpPin<GPIOA_BASE, 8> lcEnCh220V;

  cBoardCtrl(cI2cMaster* lcI2C)
    : lcEnCh220V(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    mcI2C = lcI2C;

    mcP[0] = new cINA3221(mcI2C, INA3221_I2C_ADDRESS_CONF_0, 10, 10, 10);
    mcP[0]->i8Setup();

    mu8InaErr = 0;

    lenSmS[enS_Ch220V] = enSmStOff;

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

  void vSetRelais(u8 lu8State)
  {
    if (lu8State)
    {
      lcEnCh220V.vSet1();
      lenSmS[enS_Ch220V] = enSmStOn;
    }
    else
    {
      lcEnCh220V.vSet0();
      lenSmS[enS_Ch220V] = enSmStOff;
    }
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
      }
      else
      {
        if (mu16Startup_Delay > 10) mu16Startup_Delay -= 10;
        else mu16Startup_Delay = 0;
      }
    }
    else // InaError
    {
    }
  }
};


class cSysPkgBoard
{
  public:

  /*PA8 im Testmode für Interrupt Timing */
  cStatusLed<GPIOB_BASE, 9> mcStatusLed;


  // I2C2:
  cGpPin mcI2c2_SCL_Board;
  cGpPin mcI2c2_SDA_Board;

  cI2cMaster           mcI2C2_Board;
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

extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // __BSP___SYS_PKG_BASE_MBv10b2_BOARD_H__
