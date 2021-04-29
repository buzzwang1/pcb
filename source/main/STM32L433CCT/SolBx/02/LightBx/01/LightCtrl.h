#ifndef __LIGHT_CONTROL_H__
#define __LIGHT_CONTROL_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_adc.h"

#include "tGPPin.h"
#include "LED.h"

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
    nPin1 = 15,
    nPin2 = 11,
    nPin3 = 12
  };

  //Digi Power
  cGpPin lcDigi1;
  cGpPin lcDigi2;
  cGpPin lcDigi3;

  cDigiPower()
  : lcDigi1(GPIOA_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    lcDigi2(GPIOA_BASE, nPin2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    lcDigi3(GPIOA_BASE, nPin3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
  }

  void vPowerOn()
  {
    GPIOA->BSRR = (1 << nPin1) | (1 << nPin2) | (1 << nPin3);
  }

  void vPowerOff()
  {
    GPIOA->BRR = (1 << nPin1) | (1 << nPin2) | (1 << nPin3);
  }
};

class cAnaPower
{
public:

  enum
  {
    nPin1 = 0,
    nPin2 = 1,
  };

  //Digi Power
  cGpPin lcAna1;
  cGpPin lcAna2;

  cAnaPower()
    : lcAna1(GPIOB_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      lcAna2(GPIOB_BASE, nPin2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
  }

  void vPowerOn()
  {
    GPIOB->BSRR = (1 << nPin1) | (1 << nPin2);
  }

  void vPowerOff()
  {
    GPIOB->BRR = (1 << nPin1) | (1 << nPin2);
  }
};

class cLightControl
{
public:
  // https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/NUCLEO-L476RG/Examples_LL/ADC/ADC_SingleConversion_TriggerSW/Src
  // ADC
  typedef enum
  {
    // Events für allgemeine Verwenung
    nEvReset = 0,
    nEvStart,
    nEvStop,

    nEvDummy
  }MAIN_tenEvents;

  typedef enum
  {
    nSmIdle = 0,

    // States für ADC
    nSmAdcRun,

    nSmDummy
  }MAIN_tenStates;


  typedef enum
  {
    nChnLdrL = LL_ADC_CHANNEL_6,
    nChnLdrR = LL_ADC_CHANNEL_7,
    nChnLdrO = LL_ADC_CHANNEL_8,
    nChnLdrH = LL_ADC_CHANNEL_9,
    nChnLdrU = LL_ADC_CHANNEL_10,
    nChnVSol = LL_ADC_CHANNEL_11,

    nAdcResolution10 = 1024, //1 << 10, // 10 Bit
    nAdcResolution12 = 4096, //1 << 12, // 12 Bit
    nAdcResolution16 = 65536, //1 << 16, // 16 Bit
    nAdcResolution   = nAdcResolution12,
    nAdcRefmV        = 3300,

    nAdcMeasureCnt   = 6,
    nAdcMeasureAvg   = 8,

    MAIN_nVDummy
  }MAIN_tenConsts;


  typedef struct tstAdc_type
  {
    uint8  ui8AdcConvDone;
    uint8  ui8ChlIdx;
    uint32 ui32ChlList[nAdcMeasureCnt];
    uint16 ui16Value[nAdcMeasureCnt];
    uint16 ui16ValueRaw[nAdcMeasureCnt];
    uint16 ui16ValueAvg[nAdcMeasureCnt];
    uint8  ui8Cnt[nAdcMeasureCnt];

  }tstAdc;

  typedef struct tstADCValues_type
  {
    uint16 ui16LdrL;
    uint16 ui16LdrR;
    uint16 ui16LdrO;
    uint16 ui16LdrH;
    uint16 ui16LdrU;
    uint16 ui16VSol;

  }tstADCValues;

  tstAdc         mstAdc;
  MAIN_tenStates menSmMain_Adc = nSmIdle;

  tstADCValues* pstADCValuesAvg;
  tstADCValues* pstADCValuesRaw;

  cLightControl()
  {
    pstADCValuesAvg = (tstADCValues*)mstAdc.ui16ValueAvg;
    pstADCValuesRaw = (tstADCValues*)mstAdc.ui16ValueRaw;

    vInitAdc();
  }

  void vInitAdc()
  {
    cGpPin lcAIn1(GPIOA_BASE, 1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN6
    cGpPin lcAIn2(GPIOA_BASE, 2, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN7
    cGpPin lcAIn3(GPIOA_BASE, 3, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN8
    cGpPin lcAIn4(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN9
    cGpPin lcAIn5(GPIOA_BASE, 5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN10
    cGpPin lcAIn6(GPIOA_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN11

    vDeInitAdc();

    //enable ADC1 clock
    __HAL_RCC_ADC_CLK_ENABLE();

    /* PCLK2 is the APB2 clock */
    /* ADCCLK = PCLK2/4 = 16/4 = 4MHz*/
    LL_ADC_CommonInitTypeDef ADC_CommonInit;
    LL_ADC_CommonStructInit(&ADC_CommonInit);
    ADC_CommonInit.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInit);

    //ADC1 configuration
    LL_ADC_InitTypeDef ADC_Init;
    LL_ADC_StructInit(&ADC_Init);
    ADC_Init.Resolution    = LL_ADC_RESOLUTION_12B;
    ADC_Init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_Init.LowPowerMode  = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_Init);

    LL_ADC_REG_InitTypeDef ADC_RegInit;
    LL_ADC_REG_StructInit(&ADC_RegInit);
    ADC_RegInit.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_RegInit.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_RegInit.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
    ADC_RegInit.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_NONE;
    ADC_RegInit.Overrun          = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &ADC_RegInit);

    /* Disable ADC deep power down (enabled by default after reset state) */
    LL_ADC_DisableDeepPowerDown(ADC1);

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);

    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */

    cClockInfo::Delay_us(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);

    //Calibrate and Enable ADC1
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1));

    // ADC_EN wurdenur beim Debuggen gesetzt. Nicht während run
    // Mit den 10us Wartezeiten ging es dann.
    cClockInfo::Delay_us(10);

    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);
    cClockInfo::Delay_us(10);

    while (!LL_ADC_IsEnabled(ADC1));

    mstAdc.ui8ChlIdx = 0;
  }

  void vDeInitAdc()
  {
    LL_ADC_CommonDeInit(__LL_ADC_COMMON_INSTANCE(ADC1));
    LL_ADC_DeInit(ADC1);
    __HAL_RCC_ADC_CLK_DISABLE();
  }

  void vMain_SmAdc(MAIN_tenEvents lenEvent) __attribute__((optimize("-O0")))
  {
    if (lenEvent == nEvReset)
    {
      uint8 lui8t;
      for (lui8t = 0; lui8t < nAdcMeasureCnt; lui8t++)
      {
        mstAdc.ui16Value[lui8t]    = 0;
        mstAdc.ui16ValueRaw[lui8t] = 0;
        mstAdc.ui16ValueAvg[lui8t] = 0;
        mstAdc.ui8Cnt[lui8t]       = 0;
      }

      mstAdc.ui8AdcConvDone = 0;

      mstAdc.ui32ChlList[0] = nChnLdrL;
      mstAdc.ui32ChlList[1] = nChnLdrR;
      mstAdc.ui32ChlList[2] = nChnLdrO;
      mstAdc.ui32ChlList[3] = nChnLdrH;
      mstAdc.ui32ChlList[4] = nChnLdrU;
      mstAdc.ui32ChlList[5] = nChnVSol;

      menSmMain_Adc = nSmIdle;
    }
    else
    if (lenEvent == nEvStop)
    {
      vDeInitAdc();
      menSmMain_Adc = nSmIdle;
    }
    else
    {
      switch (menSmMain_Adc)
      {
        case nSmIdle:
          if (lenEvent == nEvStart)
          {
            vInitAdc();
            LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, mstAdc.ui32ChlList[mstAdc.ui8ChlIdx]);
            LL_ADC_SetChannelSamplingTime(ADC1, mstAdc.ui32ChlList[0], LL_ADC_SAMPLINGTIME_24CYCLES_5);
            LL_ADC_REG_StartConversion(ADC1);
            menSmMain_Adc = nSmAdcRun;
          }
          break;

        case nSmAdcRun:
            if (LL_ADC_IsActiveFlag_EOS(ADC1))
            {
              mstAdc.ui16ValueRaw[mstAdc.ui8ChlIdx]  = LL_ADC_REG_ReadConversionData12(ADC1);
              mstAdc.ui16Value[mstAdc.ui8ChlIdx]    += mstAdc.ui16ValueRaw[mstAdc.ui8ChlIdx];
              if (mstAdc.ui8Cnt[mstAdc.ui8ChlIdx] < (nAdcMeasureAvg - 1))
              {
                mstAdc.ui8Cnt[mstAdc.ui8ChlIdx]++;
              }
              else
              {
                mstAdc.ui16ValueAvg[mstAdc.ui8ChlIdx] = mstAdc.ui16Value[mstAdc.ui8ChlIdx] / nAdcMeasureAvg;
                mstAdc.ui16Value[mstAdc.ui8ChlIdx] = 0;
                mstAdc.ui8Cnt[mstAdc.ui8ChlIdx]    = 0;
                mstAdc.ui8AdcConvDone++;
              }

              mstAdc.ui8ChlIdx++;
              if (mstAdc.ui8ChlIdx == nAdcMeasureCnt) mstAdc.ui8ChlIdx = 0;

              LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, mstAdc.ui32ChlList[mstAdc.ui8ChlIdx]);
              LL_ADC_SetChannelSamplingTime(ADC1, mstAdc.ui32ChlList[mstAdc.ui8ChlIdx], LL_ADC_SAMPLINGTIME_24CYCLES_5);
              LL_ADC_ClearFlag_EOS(ADC1); //Clear EOC flag
              LL_ADC_REG_StartConversion(ADC1);
            }
          break;
        default:
          break;

      }
    }
  }

  u32 u32GetLdrL_mv() { return (mstAdc.ui16ValueAvg[0] * nAdcRefmV) / nAdcResolution; }
  u32 u32GetLdrR_mv() { return (mstAdc.ui16ValueAvg[1] * nAdcRefmV) / nAdcResolution; }
  u32 u32GetLdrO_mv() { return (mstAdc.ui16ValueAvg[4] * nAdcRefmV) / nAdcResolution; }
  u32 u32GetLdrU_mv() { return (mstAdc.ui16ValueAvg[2] * nAdcRefmV) / nAdcResolution; }
  u32 u32GetLdrH_mv() { return (mstAdc.ui16ValueAvg[3] * nAdcRefmV) / nAdcResolution; }

  //     2350
  //   -------
  //    10350
  u32 u32GetU_Sol_mv() { return ((mstAdc.ui16ValueAvg[5] * nAdcRefmV) * 10350) / 2350 / nAdcResolution; }
};

#endif  //__LIGHT_CONTROL_H__
