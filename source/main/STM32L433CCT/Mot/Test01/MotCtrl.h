#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__


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
    nPin1 = 14,
    nPin2 = 15
  };

  //Digi Power
  cGpPin lcDigi1;
  cGpPin lcDigi2;

  cDigiPower()
  : lcDigi1(GPIOB_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    lcDigi2(GPIOB_BASE, nPin2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
  }

  void vPowerOn()
  {
    GPIOB->BSRR = ((1 << nPin1) |  (1 << nPin2));
  }

  void vPowerOff()
  {
    GPIOB->BRR = ((1 << nPin1) |  (1 << nPin2));
  }
};

class cAdcControl
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
    nChnPoti = LL_ADC_CHANNEL_6,

    nAdcResolution10 = 1024, //1 << 10, // 10 Bit
    nAdcResolution12 = 4096, //1 << 12, // 12 Bit
    nAdcResolution16 = 65536, //1 << 16, // 16 Bit
    nAdcResolution   = nAdcResolution12,
    nAdcRefmV        = 3300,

    nAdcMeasureCnt   = 6,
    nAdcMeasureAvg   = 8,

    MAIN_nVDummy
  }MAIN_tenConsts;


  MAIN_tenStates menSmMain_Adc = nSmIdle;

  cAdcControl()
  {
    vInitAdc();
  }

  void vInitAdc()
  {
    cGpPin lcAIn1(GPIOA_BASE, 1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // ADC1_IN6

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
    ADC_RegInit.ContinuousMode   = LL_ADC_REG_CONV_CONTINUOUS;
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

    LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_16, LL_ADC_OVS_SHIFT_RIGHT_4);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_GRP_REGULAR_CONTINUED);

    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);
    cClockInfo::Delay_us(10);

    while (!LL_ADC_IsEnabled(ADC1));
  }

  void vDeInitAdc()
  {
    LL_ADC_CommonDeInit(__LL_ADC_COMMON_INSTANCE(ADC1));
    LL_ADC_DeInit(ADC1);
    __HAL_RCC_ADC_CLK_DISABLE();
  }

  void vMain_SmAdc(MAIN_tenEvents lenEvent)
  {
    if (lenEvent == nEvReset)
    {
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

            // To convert a single channel, program a sequence with a length of 1.
            // It is not possible to have both discontinuous mode and continuous mode enabled: it is forbidden to set both DISCEN=1 and CONT=1

            LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, nChnPoti);
            LL_ADC_SetChannelSamplingTime(ADC1, nChnPoti, LL_ADC_SAMPLINGTIME_12CYCLES_5);
            LL_ADC_REG_StartConversion(ADC1);
            menSmMain_Adc = nSmAdcRun;
          }
          break;

        case nSmAdcRun:
          // continous mode, nothing todo
          break;
        default:
          break;

      }
    }
  }

  u32 u32GetPoti_mv() { return (LL_ADC_REG_ReadConversionData12(ADC1) * nAdcRefmV) / nAdcResolution; }
};

class cMotControl_Base
{
  public:

  enum class cenDriveMode : u8 {nSpeed, nPos};

  // --- Pins
  cGpPin* mcPinMotA;
  cGpPin* mcPinMotB;

  // --- Timer for PW
  TIM_TypeDef* mstTim;

  i16     mi16Pwm;
  u16     mu16PwmA;
  u16     mu16PwmB;

  i16 mi16SetPos;
  i16 mi16ActualPos;

  u8  mu8Mode;

  cMotControl_Base(cGpPin* lcPinMotA, cGpPin* lcPinMotB, TIM_TypeDef *lstTim)
  {
    mcPinMotA = lcPinMotA;
    mcPinMotB = lcPinMotB;
    mstTim    = lstTim;

    vSetPwm(256, 256);

    mi16ActualPos   = 1400;
    mi16SetPos      = 1400;
    mu8Mode         = 0;
  }

  void vTick1ms()
  {
  }

  void vSetPwm(u16 lu16PwmA, u16 lu16PwmB)
  {
    if (lu16PwmA > 256) lu16PwmA = 256;
    if (lu16PwmB > 256) lu16PwmB = 256;

    mu16PwmA = lu16PwmA;
    mu16PwmB = lu16PwmB;

    vSetMode(mu8Mode);
  }

  void vSetPwm(i16 li16Pwm)
  {
    mi16Pwm = li16Pwm;
    if (li16Pwm > 0)
    {
      vSetPwm(li16Pwm, 0);
    }
    else if (li16Pwm < 0)
    {
      vSetPwm(0, -li16Pwm);
    }
    else
    {
      vSetPwm(256, 256);
    }
  }

  i16 i16GetPosGrad()
  {
    return mi16ActualPos;
  }

  void vSetMode(u8 lu8Mode)
  {
    mu8Mode = lu8Mode;

    switch (lu8Mode)
    {
    case 0: mstTim->CCR1 = 0;        mstTim->CCR2 = 0; break;
    case 1: mstTim->CCR1 = 0;        mstTim->CCR2 = mu16PwmB; break;
    case 2: mstTim->CCR1 = mu16PwmA; mstTim->CCR2 = 0; break;
    case 3: mstTim->CCR1 = mu16PwmA; mstTim->CCR2 = mu16PwmB; break;
    }
  }
};

class cMotTest : public cMotControl_Base
{
  public:

  // --- Pins
  cGpPin mcPinMotA;
  cGpPin mcPinMotB;

  u16 mu16DriveTime;
  u8  mu8Mode_LastDir;

  float mfealt, mfesum, mfe, mfy;
  float mfKp, mfKi, mfKd, mfTa;

  cMotTest(TIM_TypeDef *lstTim)
    : cMotControl_Base(&mcPinMotA, &mcPinMotB, lstTim),
      mcPinMotA(GPIOB_BASE, 13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcPinMotB(GPIOB_BASE, 14, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    mu16DriveTime = 0;
    mu8Mode_LastDir = 1;

    mcPinMotA.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);
    mcPinMotB.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);

    vSetPwm(256,256);

    TIM_vInit();

    mfealt = mfesum = mfe = mfy = 0.0f;

    mfKp = 10.0f; //10.0f;
    mfKi = 3.0f;
    mfKd = 1.0f;

    mfTa = 0.001f;
  }

  void vPID()
  {
    i16   liDiffPos = mi16SetPos - mi16ActualPos;

    i16   liDiffPosAbs = liDiffPos;
    if (liDiffPosAbs < 0) liDiffPosAbs = -liDiffPosAbs;

    float lfyAbs = mfy;
    if (lfyAbs < 0) lfyAbs = -lfyAbs;

    if ((liDiffPosAbs < 64) &&  (lfyAbs < 50.0f))
    {
      vSetMode(0);
    }
    else
    {
      volatile float lfy_Kp,lfy_Kd,lfy_Ki;

      mfe = (float)(liDiffPos); //4096.0f;

      mfesum = mfesum + mfe;

      if (mfesum>10000.0f)       mfesum =  10000.0f;
      else if (mfesum<-10000.0f) mfesum = -10000.0f;


      lfy_Kp = mfKp * mfe;
      lfy_Ki = mfKi * mfTa * mfesum;
      lfy_Kd = mfKd * (mfe - mfealt)/mfTa;


      mfy = lfy_Kp + lfy_Ki + lfy_Kd;


      mfealt = mfe;

      //if ((mfy >  16.0f) && (mfy <  50.0f)) mfy =  50.0f;
      //if ((mfy < -16.0f) && (mfy > -50.0f)) mfy = -50.0f;

      if (mfy>256.0f)       mfy =  256.0f;
      else if (mfy<-256.0f) mfy = -256.0f;

      vSetPwm((i16)(mfy));
      vSetMode(3);
    }
  }

  void TIM_vInit(void)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();

    TIM_HandleTypeDef TimHandle = {};

    /* Set TIM1 instance */
    TimHandle.Instance = mstTim;

    /* Initialize TIM2 peripheral as follows:
         + Period = 10000 - 1
         + Prescaler = (SystemCoreClock/1000000) - 1
         + ClockDivision = 0
         + Counter direction = Up
    */
    TimHandle.Init.Period            = 256-1; // 256us
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) -1; // 1 us
    //TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    //TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM_OC_InitTypeDef sConfigOC = {};
    HAL_TIM_PWM_Init(&TimHandle);

    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.Pulse        = 0;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_2);

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    HAL_TIM_Base_Start(&TimHandle);

    // Complementary
    HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_2);

    // Normal
    //HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
    //HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
  }


  void vSetActualPos(i16 li16Pos)
  {
     mi16ActualPos = li16Pos;
  }

  void vSetPosRel(i16 li16Pos)
  {
     mi16SetPos = li16Pos;
  }

  void vTick1ms()
  {
    vPID();
  }

};


#endif  //__MOTOR_CONTROL_H__
