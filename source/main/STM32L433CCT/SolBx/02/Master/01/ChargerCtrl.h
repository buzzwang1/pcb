#ifndef __CHARGER_CONTROL_H__
#define __CHARGER_CONTROL_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_dac.h"

#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "ina219.h"

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


class cDigiPower
{
  public:

  enum
  {
    nPin1 = 15
  };

  //Digi Power
  cGpPin lcDigi1;

  cDigiPower()
  : lcDigi1(GPIOA_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1)
  {
  }

  void vPowerOn()
  {
    GPIOA->BRR = (1 << nPin1);
  }

  void vPowerOff()
  {
    GPIOA->BSRR = (1 << nPin1);
  }
};

class cLipoMon
{
public:

  typedef enum
  {
    enS10 = 0,
    enS11 = 1,
    enS12 = 2,
    enS13 = 3,
    enS14 = 4,
    enS20 = 5,

    enINA_0x41 = 0,
    enINA_0x44 = 1,
    enINA_0x45 = 2,

    enINA_P2 = enINA_0x41,
    enINA_P3 = enINA_0x44,
    enINA_P4 = enINA_0x45,
  }tenLipoMonGeneral;

  typedef enum
  {
    enSmStNotCharging,
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
    enS4OffThres = 8000,
    enS4OnThres = 15000,

    enS10OffThres = 6500,
    enS10OnThres = 7000,

    enS11OffThres = 2800,
    enS11OnThres = 3000,

    enLipoUThres1 = 2600,
    enLipoUThres2 = 2800,
    enLipoUThres3 = 3800,
    enLipoUThres4 = 4100,

    //Strom in mA
    enLipoIModeOff = 10,
    enLipoISave = 25,
    enLipoIMode1 = 25,
    enLipoIMode2 = 50,
    enLipoIMode3 = 100,
    enLipoIMode4 = 250,
    enLipoIMode5 = 500,
    /*enLipoIModeOff =   10,
    enLipoISave    =   25,
    enLipoIMode1   =   25,
    enLipoIMode2   =   25,
    enLipoIMode3   =   25,
    enLipoIMode4   =   25,
    enLipoIMode5   =   25,*/

    enP2Mode1LoThres = 0,
    enP2Mode1UpThres = 7500,
    enP2Mode2LoThres = 8000,
    enP2Mode2UpThres = 8500,
    enP2Mode3LoThres = 9000,
    enP2Mode3UpThres = 10500,
    enP2Mode4LoThres = 11000,
    enP2Mode4UpThres = 12500,
    enP2Mode5LoThres = 13000,
    enP2Mode5UpThres = 99999

  }tenLipoMonS_Const;


  cI2cMaster* mcI2C;

  cINA219* mcP[4];

  int32 mi32VBus[4];
  int32 mi32IShunt[4];

  uint8 mui8P2Mode;
  int32 mi32Dac;

  tenLipoMonS_States    lenSmS[6];
  tenLipoMonLipo_States lenSmLipo;
  tenLipoMonLipo_States lenSmLipoPrev;

  int32  mi32LipoChargeSum_10ms;
  uint32 mui32LipoChargeTime_10ms;

  uint32   mui32LipoChargeCount1;
  uint32   mui32LipoChargeCount2;
  uint32   mui32LipoLoadCount;

  int32    mi32LipoCap;
  int32    mi32LipoCapMax;

  uint8 mui8Startup_Delay;

  tcGpPin<GPIOB_BASE, 15> mS10;
  tcGpPin<GPIOB_BASE, 14> mS11;
  tcGpPin<GPIOB_BASE, 13> mS12;
  tcGpPin<GPIOB_BASE, 12> mS13;
  tcGpPin<GPIOA_BASE, 8>  mS14;
  tcGpPin<GPIOB_BASE, 20> mS20;


  cLipoMon(cI2cMaster* lcI2C)
    : mS10(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mS11(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mS12(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1),
      mS13(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mS14(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mS20(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    uint8 lui8t;

    mcI2C = lcI2C;

    mui8Startup_Delay = 250;

    lenSmLipo = enSmStNotCharging;
    lenSmLipoPrev = lenSmLipo;

    mi32LipoChargeSum_10ms = 0;
    mui32LipoChargeTime_10ms = 0;

    mui32LipoChargeCount1 = 0;
    mui32LipoChargeCount2 = 0;
    mui32LipoLoadCount = 0;

    mi32LipoCap = 0;
    mi32LipoCapMax = 0;

    mui8P2Mode = 0;

    for (lui8t = (uint8)enINA_P2; lui8t <= (uint8)enINA_P4; lui8t++)
    {
      mi32VBus[lui8t] = 0;
      mi32IShunt[lui8t] = 0;
    }


    lenSmS[enS10] = enSmStOff;
    lenSmS[enS11] = enSmStOff;
    lenSmS[enS12] = enSmStOn;
    lenSmS[enS13] = enSmStOff;
    lenSmS[enS14] = enSmStOff;
    lenSmS[enS20] = enSmStOff;

    mcP[enINA_P2] = new cINA219(mcI2C, INA219_I2C_ADDRESS_CONF_1, 100);
    mcP[enINA_P3] = new cINA219(mcI2C, INA219_I2C_ADDRESS_CONF_4, 100);
    mcP[enINA_P4] = new cINA219(mcI2C, INA219_I2C_ADDRESS_CONF_5, 100);

    /* DAC */
    LL_DAC_InitTypeDef DAC_InitStructure;

    tcGpPin<GPIOA_BASE, 4> mGnd1(GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);

    __HAL_RCC_DAC1_IS_CLK_ENABLED();


    /* DAC channel1 Configuration */
    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_InitStructure);

    mi32Dac = 0;
    vSetDacVoltage(mi32Dac);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  }

  ~cLipoMon()
  {
    uint8 lui8t;

    for (lui8t = (uint8)enINA_P2; lui8t <= (uint8)enINA_P4; lui8t++)
    {
      delete mcP[lui8t];
    }
  }

  void vSetDacVoltage(uint32 lui32Value_mV)
  {
    uint32 lui32DacValue;

    if (lui32Value_mV > 5220) lui32Value_mV = 5220;
    if (lui32Value_mV < 270) lui32Value_mV = 270;

    lui32DacValue = 4096 - (((lui32Value_mV - 270) * 4096) / 4950);
    if (lui32DacValue > 4095) lui32DacValue = 4095;

    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, lui32DacValue);
  }

  void vUpdate_S10()
  {
    tenLipoMonGeneral lenSx = enS10;

    switch (lenSmS[lenSx])
    {
    case enSmStOff:
      if (/*(lenSmS[enS04] == enSmStOn) &&*/ (lenSmLipo != enSmStCharged))
      {
        mS10.vSet1();
        lenSmS[lenSx] = enSmStOn;
      }
      break;
    case enSmStOn:
      if (/*(lenSmS[enS04] == enSmStOff) ||*/ (lenSmLipo == enSmStCharged))
      {
        mS10.vSet0();
        mi32Dac = 0;
        vSetDacVoltage(mi32Dac);
        lenSmS[lenSx] = enSmStOff;
      }
      break;
    }
  }

  void vUpdate_S11()
  {
    tenLipoMonGeneral lenSx = enS11;

    switch (lenSmS[lenSx])
    {
    case enSmStOff:
      if ((mi32VBus[enINA_P3] >= enS11OnThres) /*&& (lenSmS[enS04] == enSmStOff)*/)
      {
        mS11.vSet1();
        lenSmS[lenSx] = enSmStOn;
      }
      break;
    case enSmStOn:
      if ((mi32VBus[enINA_P3] < enS11OffThres) /*|| (lenSmS[enS04] == enSmStOn)*/)
      {
        mS11.vSet0();
        lenSmS[lenSx] = enSmStOff;
      }
      break;
    }
  }

  void vUpdate_S12_S14()
  {
    if (lenSmS[enS11] == enSmStOff)
    {
      lenSmS[enS12] = enSmStOn;
      lenSmS[enS14] = enSmStOff;
      mS12.vSet1();
      mS14.vSet0();
    }
    else
    {
      lenSmS[enS12] = enSmStOff;
      lenSmS[enS14] = enSmStOn;
      mS14.vSet1();
      mS12.vSet0();
    }
  }

  void vUpdate_S13()
  {
    if ((lenSmS[enS12] == enSmStOn) || (lenSmS[enS11] == enSmStOn))
    {
      lenSmS[enS13] = enSmStOn;
      mS13.vSet1();
    }
    else
    {
      lenSmS[enS13] = enSmStOff;
      mS13.vSet0();
    }
  }

  void vUpdate_S20()
  {
    /*
    if ((lenSmS[enS12] == enSmStOn) || (lenSmS[enS11] == enSmStOn))
    {
      lenSmS[enS20] = enSmStOn;
      mS20.vSet1();
    }
    else
    {
      lenSmS[enS20] = enSmStOff;
      mS20.vSet0();
    }*/
  }


  void vUpdateDac()
  {
    lenSmLipoPrev = lenSmLipo;

    if (lenSmS[enS10] == enSmStOff)
    {
      mui8P2Mode = 0;
    }

    // Ladestrom abhängig von V_p2
    // Mode0: init
    // Mode1:     -  7.5V:   25mA
    // Mode2: 8V  -  8,5V:   50mA
    // Mode3: 9V  - 10,5V:  100mA
    // Mode4: 11V - 12,5V:  200mA
    // Mode5: 13V -      :  400mA
    if (mui8P2Mode == 0)
    {
      /*mui8P2Mode = 5;
      if ((mi32VBus[enINA_P2] < enP2Mode5LoThres)) mui8P2Mode = 4;
      if ((mi32VBus[enINA_P2] < enP2Mode4LoThres)) mui8P2Mode = 3;
      if ((mi32VBus[enINA_P2] < enP2Mode3LoThres)) mui8P2Mode = 2;
      if ((mi32VBus[enINA_P2] < enP2Mode2LoThres)) mui8P2Mode = 1;*/
      mui8P2Mode = 2;
    }
    else
    {
      /*if ((mi32VBus[enINA_P2] >= enP2Mode1LoThres) && (mi32VBus[enINA_P2] <= enP2Mode1UpThres)) mui8P2Mode = 1;
      if ((mi32VBus[enINA_P2] >= enP2Mode2LoThres) && (mi32VBus[enINA_P2] <= enP2Mode2UpThres)) mui8P2Mode = 2;
      if ((mi32VBus[enINA_P2] >= enP2Mode3LoThres) && (mi32VBus[enINA_P2] <= enP2Mode3UpThres)) mui8P2Mode = 3;
      if ((mi32VBus[enINA_P2] >= enP2Mode4LoThres) && (mi32VBus[enINA_P2] <= enP2Mode4UpThres)) mui8P2Mode = 4;
      if ((mi32VBus[enINA_P2] >= enP2Mode5LoThres) && (mi32VBus[enINA_P2] <= enP2Mode5UpThres)) mui8P2Mode = 5;*/
      mui8P2Mode = 2;
    }

    if (lenSmS[enS10] == enSmStOn)
    {
      // Ladespannung modus

      //   Init:        NotCharging
      //   U_P3 < 2600mV: ChargingSave (25mA)
      //  (U_P3 < 4100mV &&
      //   I_P3 > 10mA): ChargingNormal*/

      // Test für Unterspannung
      if (mi32VBus[enINA_P3] < enLipoUThres1) lenSmLipo = enSmStChargingSave;
      //  Test für offenen Kontakt
      if (1) // (mi32IShunt[enINA_P3] > 2)
      {
        switch (lenSmLipo)
        {
        case enSmStNotCharging:
          if (mi32VBus[enINA_P3] < enLipoUThres3)
          {
            lenSmLipo = enSmStChargingNormal;
            mi32Dac = enLipoUThres2;
          }
          else
          {
            lenSmLipo = enSmStCharged;
          }
          break;
        case enSmStChargingSave:
          if (mi32VBus[enINA_P3] > enLipoUThres2)
          {
            lenSmLipo = enSmStChargingNormal;
          }
          else
          {
            mi32Dac -= 10 * (mi32IShunt[enINA_P3] - enLipoISave);
          }
          break;
        case enSmStChargingNormal:
          if ((mi32IShunt[enINA_P3] < enLipoIModeOff) &&
            ((mi32VBus[enINA_P3] >= enLipoUThres4)))
          {
            lenSmLipo = enSmStCharged;
          }
          else
          {
            if (mi32VBus[enINA_P3] < enLipoUThres4)
            {
              switch (mui8P2Mode)
              {
              case 1:
                if (mi32VBus[enINA_P2] > 5200)
                {
                  if (mi32IShunt[enINA_P3] < enLipoIMode1) mi32Dac += 10;
                }
                else
                {
                  mi32Dac -= 10;
                }
                break;
              case 2:
                if (mi32VBus[enINA_P2] > 5200)
                {
                  if (mi32IShunt[enINA_P3] < enLipoIMode2) mi32Dac += 10;
                }
                else
                {
                  mi32Dac -= 10;
                }
                break;
              case 3:
                if (mi32VBus[enINA_P2] > 5200)
                {
                  if (mi32IShunt[enINA_P3] < enLipoIMode3) mi32Dac += 10;
                }
                else
                {
                  mi32Dac -= 10;
                }
                break;
              case 4:
                if (mi32VBus[enINA_P2] > 5200)
                {
                  if (mi32IShunt[enINA_P3] < enLipoIMode4) mi32Dac += 10;
                }
                else
                {
                  mi32Dac -= 10;
                }
                break;
              case 5:
                if (mi32VBus[enINA_P2] > 5200)
                {
                  if (mi32IShunt[enINA_P3] < enLipoIMode5) mi32Dac += 10;
                }
                else
                {
                  mi32Dac -= 10;
                }
                break;
              }
            }
            else
            {
              mi32Dac -= 10;
            }
          }

          break;
        case enSmStCharged:
          if (mi32VBus[enINA_P3] < enLipoUThres3)
          {
            lenSmLipo = enSmStChargingNormal;
            mi32Dac = enLipoUThres3;
          }
          break;
        }
      }
      else
      {
        mi32Dac = 0;
        lenSmLipo = enSmStChargingSave;
      }

      if (mi32Dac < 0) mi32Dac = 0;
      //if (mi32VBus[enINA_P3] < enLipoUThres4)
      {
        vSetDacVoltage(mi32Dac);
      }
    }

    //State Transition
    if (lenSmLipoPrev != lenSmLipo)
    {
      switch (lenSmLipo)
      {
      case enSmStNotCharging:
        break;
      case enSmStChargingSave:
        mui32LipoChargeCount1 = 0;
        break;
      case enSmStChargingNormal:
        mui32LipoChargeCount1 = 0;
        break;
      case enSmStCharged:
        mui32LipoLoadCount = 0;
        break;
      }
    }

    mi32LipoChargeSum_10ms += mi32IShunt[enINA_P3];

    if (mui32LipoChargeTime_10ms == 0)
    {
      mui32LipoChargeTime_10ms = 100;
      mi32LipoChargeSum_10ms /= 100;

      mi32LipoCap += mi32LipoChargeSum_10ms;
      if (mi32LipoCap > mi32LipoCapMax)
      {
        mi32LipoCapMax = mi32LipoCap;
      }

      if (mi32LipoCap < 0)
      {
        mi32LipoCapMax += (-mi32LipoCap);
        mi32LipoCap = 0;
      }

      if (mi32LipoChargeSum_10ms > 0)
      {
        //Aufladen
        mui32LipoChargeCount1 += mi32LipoChargeSum_10ms;
        mui32LipoChargeCount2 += mi32LipoChargeSum_10ms;
      }
      else
      {
        //Entladen
        mui32LipoLoadCount -= mi32LipoChargeSum_10ms;
      }

      mi32LipoChargeSum_10ms = 0;
    }
    else
    {
      mui32LipoChargeTime_10ms--;
    }
  }


  void vUpdateAll(bool bIsAuto)
  {
    uint8 lui8t;

    // Strom und Spannung einlesen
    for (lui8t = (uint8)enINA_P2; lui8t <= (uint8)enINA_P4; lui8t++)
    {
      mcP[lui8t]->i8ReadVBus_digit();
      mcP[lui8t]->i8ReadVShunt_digit();

      mi32VBus[lui8t] = mcP[lui8t]->i32CalcVBus_uV() / 1000;
      mi32IShunt[lui8t] = mcP[lui8t]->i32CalcIShunt_uA() / 1000;
    }

    if ((mui8Startup_Delay == 0) && (bIsAuto))
    {
      //Schalter setzten
      vUpdate_S10();
      vUpdate_S11();
      vUpdate_S12_S14();
      vUpdate_S13();
      vUpdate_S20();
      vUpdateDac();
    }
    else
    {
      mui8Startup_Delay--;
    }
  }
};



#endif  //__CHARGER_CONTROL_H__
