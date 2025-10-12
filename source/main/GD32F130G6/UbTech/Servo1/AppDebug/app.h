#ifndef __SERVO1_APPLIKATION_H__
#define __SERVO1_APPLIKATION_H__

#include "Typedef.h"
#include "cCli.h"
#include "cBnStreamSysPortBase.h"
#include "cBnSpop.h"

#include "TypeDef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_adc.h"
#include "gd32f1x0_gpio.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_timer.h"
#include "gd32f1x0_misc.h"
#include "cFixPti1814.h"

struct cSystem
{
  static u8 mAutomatik; // 0: Maneller Modus; 1: Automatischer Modus
  static u8 mLed; // 0: Aus; 1: An
  static u8 mLedSet; // 0: Aus; 1: An

  static void vInit()
  {
    mAutomatik = 1;
    #ifndef TEST_BnLinkUsartMpHd
    vSetLed(u8GetRomConstLedEnable());
    #endif
    vLedInit();
  }

  static void vLedInit()
  {
    // PA0
    rcu_periph_clock_enable(RCU_GPIOB);
    GPIO_CTL(GPIOB) |= 1 << 2;  // Output Mode
  }

  static void vLedOff()
  {
    gpio_bit_set(GPIOB, GPIO_PIN_1);
  }

  static void vLedOn()
  {
    gpio_bit_reset(GPIOB, GPIO_PIN_1);
  }

  static void vSetLed(u8 lu8Value)
  {
    if (mLedSet != lu8Value)
    {
      mLed = mLedSet = lu8Value;
    }
  }

  static void vLedTick250ms()
  {
    if (mLed & 128)
    {
      mLed <<= 1;
      mLed |= 1;
      vLedOn();
    }
    else
    {
      mLed <<= 1;
      vLedOff();
    }
  }
};

u8 cSystem::mAutomatik = 0;
u8 cSystem::mLed = 0;
u8 cSystem::mLedSet = 0;

template <typename T> struct cLowPassT
{
  T mValue;
  T mFactor;

  cLowPassT(const T lFactor)
  {
    mFactor = lFactor;
    mValue = T((const i32)0);
  }

  T DoProcess(const T lNewValue)
  {
    mValue = mFactor * lNewValue + (T((const i32)1) - mFactor) * mValue;
    return mValue;
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }

  operator float()
  {
    return (float)mValue;
  }

  operator i32()
  {
    return (i32)mValue;
  }
};


template <typename T> struct cClipT
{
  T mClip;

  cClipT(const T lClip)
  {
    mClip = lClip;
  }

  T operator()(const T lVal)
  {
    if      (lVal >  (mClip)) { return  (mClip);}
    else if (lVal < (-mClip)) { return (-mClip);}
    return lVal;
  }
};

template <typename T> struct cConvertT
{
  T mOffset;
  T mFactor;
  cClipT<T> mClip;

  cConvertT(const T lFactor, const T lOffset, const T lClip)
    : mClip(lClip)
  {
    mOffset = lOffset;
    mFactor = lFactor;
  }

  T DoProcess(const T lVal)
  {
    return mClip((mFactor * lVal - mOffset));
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }
};


template <typename T> struct cPidT
{
  T mKp;
  T mKi;
  T mKd;

  T mOutputLimit;

  T mErrOld;

  T mErrIntegral;
  T mDeltaError;

  cPidT(const T lKp, const T lKi, const T lKd, const T lClamp)
  {
    mKp = lKp;
    mKi = lKi;
    mKd = lKd;
    mOutputLimit = lClamp;

    mErrOld      = T((i32)0);
    mErrIntegral = T((i32)0);
    mDeltaError  = T((i32)0);
  }


  T DoProcess(const T lErr) // __attribute__((optimize("-O0")))
  {
    mDeltaError = lErr - mErrOld;
    mErrOld = lErr;

    // PID
    T v = mKp * lErr + mErrIntegral + mKd * mDeltaError;

    bool saturating = False;

    if (v >=  mOutputLimit)
    {
      v =  mOutputLimit;
    }
    else if (v <= -mOutputLimit)
    {
      v = -mOutputLimit;
    }

    if (mErrIntegral >= mOutputLimit)
    {
      mErrIntegral = mOutputLimit;
      saturating = True;
    }
    else if (mErrIntegral <= -mOutputLimit)
    {
      mErrIntegral = -mOutputLimit;
      saturating = True;
    }

    // error and output same sign
    bool clamp = False;
    if (saturating)
    {
      if (((lErr > T((i32)0)) && (v > T((i32)0)))) clamp = True;
      if (((lErr < T((i32)0)) && (v < T((i32)0)))) clamp = True;
    }

    if (!clamp)
    {
      mErrIntegral += (mKi * lErr);
    }

    // output
    return v;
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }
};


class cServo1_Applikation
{
public:
  enum
  {
    nV_ntc    =  ADC_CHANNEL_0, // ADC Channel  0: externen Temperatursensor (ntc)
    nV_bat    =  ADC_CHANNEL_1, // ADC Channel  1:
    nV_imot   =  ADC_CHANNEL_2, // ADC Channel  2:
    nV_poti   =  ADC_CHANNEL_7, // ADC Channel  7:
    nV_temp   =  ADC_CHANNEL_16, // ADC Channel 16: interner Temperatursensor

    nIdx_imot1  =  0,
    nIdx_imot2  =  1,
    nIdx_imot3  =  2,
    nIdx_poti   =  3,
    nIdx_bat    =  4,
    nIdx_ntc    =  5,
    nIdx_temp   =  6,

    nChnCount = 7,

    nDegree2Digit = 4096/280
  };

  typedef struct
  {
    u8 mMode      : 3; // 0: Mot Disabled
                       // 1: Servo by Pos
                       // 2: Reserve
                       // 3: Reserve
                       // 4: Motor by PMW
                       // 5: Motor by Speed
                       // 6: Motor by Distance
                       // 7: Reserve

    u8 Res1      : 1; // 
    u8 mILim     : 1; // 0: Ok; 1: Current > Limit
    u8 mPowLim   : 1; // 0: Ok; 1: Power > Limit
    u8 mPowFail  : 1; // 0: Ok; 1: Voltage < 5500
    u8 mTempFail : 1; // 0: Ok; 1: Temp > Limit
  }tstStatus;

  typedef union
  {
    u8        u8Status;
    tstStatus stStatus;
  }tunStatus;

  u16       mAdcResult[nChnCount];
  u16       mPowerFailCounter;
  u16       mTempFailCounter;

  i16 mPos_Soll;   // [Grad]
  i16 mSpeed_Soll; // [Grad/s]
  i16 mDist_Soll;  // [mm]
  i16 mPwm_Soll;   // [%]
  i16 mCur_Limit;  // [mA]
  i16 mPow_Limit;  // [mW]
  i16 mTemp_Limit; // [°C]

  i16 mPidPwmPos;
  i16 mPidPwmCur;

  i16 mPwmMotOut;

  cFixPti1814 mcInputPos_Grad;
  cFixPti1814 mcInputI_mA;
  cFixPti1814 mcInputV_mV;
  cFixPti1814 mcInputInt_Grad;
  cFixPti1814 mcInputExt_Grad;

  cLowPassT<cFixPti1814> mcInLpPos;
  cLowPassT<cFixPti1814> mcInLpI;
  cLowPassT<cFixPti1814> mcInLpV;
  cLowPassT<cFixPti1814> mcInLpTempInt;
  cLowPassT<cFixPti1814> mcInLpTempExt;


  cPidT<cFixPti1814> mcInPidPos;
  cPidT<cFixPti1814> mcInPidPower;

  tunStatus mStatus;

  cServo1_Applikation()
    : mcInLpPos(Fp1814GetRomConstLpInputPos()),
      mcInLpI(Fp1814GetRomConstLpInputCurrent()),
      mcInLpV(Fp1814GetRomConstLpInputSupply()),
      mcInLpTempInt(Fp1814GetRomConstLpInputTemp1()),
      mcInLpTempExt(Fp1814GetRomConstLpInputTemp2()),

      mcInPidPos(Fp1814GetRomConstPidPosKp(),   Fp1814GetRomConstPidPosKi(), Fp1814GetRomConstPidPosKd(), Fp1814GetRomConstPidPosLimit()),
      mcInPidPower(Fp1814GetRomConstPidCurKp(), Fp1814GetRomConstPidCurKi(), Fp1814GetRomConstPidCurKd(), Fp1814GetRomConstPidCurLimit())
  {
    mStatus.stStatus.mMode = 1;

    mPos_Soll    =    0; // [Grad]
    mSpeed_Soll  =    0; // [Grad/s]
    mDist_Soll   =    0; // [mm]
    mPwm_Soll    =    0; // [%]
    mCur_Limit   = 1000; // [mA]
    mPow_Limit   = 9000; // [mW]
    mTemp_Limit  =   80; // [°C]

    // ------------- gpio_config
    rcu_periph_clock_enable(RCU_GPIOA);

    // configure PC0 (adc channel10) as analog input
    /*gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_7);


    // Motor
    //Input
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);  // Fault indication. Open-drain output, logic low when in fault condition (OCP, OTP, OVP).

    //Output
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4); // Sleep mode input. Logic low to enter low-power sleep mode. Internal pulldown.

    //gpio_mode_set(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_8); // H-bridge enable input. High to enable H-bridge. Internal pulldown.
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3); // H-bridge phase input (motor direction). Internal pulldown.
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5); // Brake input. Internal pulldown.

    //Configure PA8(TIMER0_CH0) as alternate function
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);*/

    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    GPIO_CTL(GPIOA)    = 0x2802C57F;
    GPIO_OSPD(GPIOA)   = 0x0C030000;
    GPIO_PUD(GPIOA)    = 0x24000000;
    GPIO_OCTL(GPIOA)   = 0x00000020; // Brake anschalten
    GPIO_AFSEL1(GPIOA) = 0x00000002;

    // ------------ Timer0_Config
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
        timer0CLK = SystemCoreClock / 108 = 1MHz */
    rcu_periph_clock_enable(RCU_TIMER0);

    /*timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER0);

    // TIMER0 configuration
    timer_initpara.prescaler = 3; // 0.5us   (@8Mhz/4 = 2Mhz = 0.5us)
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 99;  // 0.5us * 100 = 50us = 20khz
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    // CH0 configuration in PWM mode for Motor Driver
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    // CH1 configuration in PWM mode for trigger ADC
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);

    //CH0
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    //CH1
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 0);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    TIMER_CH1CV(TIMER0) = 1;


    timer_primary_output_config(TIMER0, ENABLE);
    // auto-reload preload enable
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);*/


    // Das Gleiche wie oben 2Mhz-PWM-Timer mit 100PWM => 20khz = 50us , Autoreload, @8Mhz
    // Braucht aber so ein paar Byte weniger Code

    TIMER_CHCTL0(TIMER0) = 0x6060; // PWM CH0 und CH1
    TIMER_CHCTL2(TIMER0) = 0x11;   // Ch0 und Ch1 enable
    TIMER_PSC(TIMER0)    = 11;     // 2 Mhz => //  11 für 24Mhz, 7 für 16Mhz und 3 für 8Mhz
    TIMER_CAR(TIMER0)    = 0x63;   // 100
    TIMER_CH1CV(TIMER0)  = 1;
    TIMER_CCHP(TIMER0)   = 0x8000; // 100
    TIMER_CTL0(TIMER0)   = 0x081;


    // dma_ch0 config used for ADC
    // enable the DMA clock
    rcu_periph_clock_enable(RCU_DMA);

    /*dma_parameter_struct dma_init_struct;

    // initialize DMA channel0
    dma_deinit(DMA_CH0);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t) & (mAdcResult);
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number = nChnCount;
    dma_init_struct.periph_addr = (uint32_t) & (ADC_RDATA);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
    dma_init(DMA_CH0, dma_init_struct);

    // configure DMA mode
    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);

    // enable DMA channel0
    dma_channel_enable(DMA_CH0);*/


    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    DMA_CH0CNT   = nChnCount;
    DMA_CH0PADDR = (u32)&ADC_RDATA;
    DMA_CH0MADDR = (u32)&mAdcResult;
    DMA_CH0CTL = 0x000015A1;


    // ------------ ADC_Config
    // enable the DMA clock
    rcu_periph_clock_enable(RCU_ADC);

    // ADC Takt 4Mhz = 0 => 0,25us
    // ADC Takt 4Mhz  =>  RCU_ADCCK_APB2_DIV6 für 24Mhz
    //                    RCU_ADCCK_APB2_DIV4 für 16Mhz
    //                    RCU_ADCCK_APB2_DIV2 für 8Mhz
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    /*
    // ADC SCAN function enable
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);

    // ADC channel length config
    adc_channel_length_config(ADC_REGULAR_CHANNEL, nChnCount);

    //adc_vbat_enable();
    // Temp sensor = ADC_IN16. The sampling time for the temperature sensor is recommended to be set to at least 17.1μs
    adc_tempsensor_vrefint_enable();

    // For the 12-bit resolution, the total conversion time is sampling time + 12.5” ADC_CLK cycles
    //
    //  1.5cycles: => Sampling time =  0,375us; Total conversion time:  1.5 + 12.5 = 14cycles =  3,50us
    //  7.5cycles: => Sampling time =  1,875us; Total conversion time:  7.5 + 12.5 = 20cycles =  5,00us
    // 13.5cycles: => Sampling time =  3,375us; Total conversion time: 13.5 + 12.5 = 26cycles =  6,50us
    // 28.5cycles: => Sampling time =  7,125us; Total conversion time: 28.5 + 12.5 = 41cycles = 10,25us
    // 41.5cycles: => Sampling time = 10,375us; Total conversion time: 41.5 + 12.5 = 54cycles = 13,50us
    // 55.5cycles: => Sampling time = 13,875us; Total conversion time: 55.5 + 12.5 = 68cycles = 17,00us
    // 71.5cycles: => Sampling time = 17,875us; Total conversion time: 71.5 + 12.5 = 84cycles = 21,00us

    // ADC regular channel config
    // Alle 5 Kanäle sollte innerhalb 50us gesammpled werden,
    // weil Timer0 mit 20khz läuft.
    adc_regular_channel_config(0, nV_imot, ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(1, nV_imot, ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(2, nV_imot, ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(3, nV_poti, ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(4, nV_bat,  ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(5, nV_ntc,  ADC_SAMPLETIME_1POINT5);  //  3,5us
    adc_regular_channel_config(6, nV_temp, ADC_SAMPLETIME_71POINT5); // 21,0us
    //                                                               ---------
    //                                                                  42,0us

    // ADC data alignment config
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    // ADC SCAN function enable
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    // ADC DMA enable
    adc_dma_mode_enable();

    // enable ADC interface
    adc_enable();*/

    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    ADC_SAMPT0 = 0x00180000;
    ADC_SAMPT1 = 0x00000000;
    ADC_RSQ0   = 0x00600000;
    ADC_RSQ1   = 0x00000010;
    ADC_RSQ2   = 0x00138842;
    ADC_CTL0   = 0x00000100;
    ADC_CTL1   = 0x00800101;

    // ADC calibration and reset calibration
    adc_calibration_enable();

    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T0_CH1);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
  }

  void vSetPwm(i16 li16Value)
  {
    if (li16Value > 0)
    {
      // >0: im Gegen-Uhrzeigersinn drehen
      gpio_bit_reset(GPIOA, GPIO_PIN_3); // PHASE
    }
    else // if (li16Value < 0)
    {
      // <0: im Uhrzeigersinn drehen
      gpio_bit_set(GPIOA, GPIO_PIN_3);  // PHASE
      li16Value = -li16Value;
    }

    if (li16Value > 100) li16Value = 100;

    if (li16Value == 0)
    {
      // Set Motor sleep
      gpio_bit_reset(GPIOA, GPIO_PIN_4); // SLEEP
    }
    else
    {
      // wakeup Motor driver
      gpio_bit_set(GPIOA, GPIO_PIN_4); //SLEEP
    }

    TIMER_CH0CV(TIMER0) = (uint32_t)li16Value; // ENABLE
  }

  u16 u16GetSupplyVoltage_mV()
  {
    i32 li32V = mcInputV_mV;
    return li32V;
  }

  i16 i16GetCurrent_mA()
  {
    return (u16)((i32)mcInputI_mA);
  }

  i16 i16ReadCurrent()
  {
    u32 lu32Zwerg;

    // Falls Motor deaktiviert ist
    if (!gpio_input_bit_get(GPIOA, GPIO_PIN_4)) // Sleep ?
    {
      return 0;
    }

    // Den größeren Wert von imot2 und imot3 nehmen
    // imot1 wird ignoriert
    lu32Zwerg = mAdcResult[nIdx_imot2];
    if (lu32Zwerg < mAdcResult[nIdx_imot3]) lu32Zwerg = mAdcResult[nIdx_imot3];

    // Strom von Digit in mA umrechnen.
    // An ISET ist ein 25kOhm Widerstand => 2,5V/A
    // Strombegrenzung geht bei 1,5V rein => 1,5/2,5 = 600mA
    // 4096 digit = (digit*3,3/4096)/2,5 = 1,32A => 1,32A/4096 = 0,322mA/digit
    lu32Zwerg = (lu32Zwerg * 3300)/10240;

    // Mit PWM-Verhältnis multiplizieren
    lu32Zwerg = (lu32Zwerg * TIMER_CH0CV(TIMER0))/100;

    return (i16)lu32Zwerg;
  }

  i16 i16GetIntTemp_Grad()
  {
    i32 li32Zwerg;
    // V25: Vtemperature value at 25°C, the typical value is 1.43 => 1,43V * 4096digit / 3,3V = 1762 digit
    // Avg_Slope: Average Slope for curve between Temperature vs. Vtemperature, the typical value is 4.3 mV/°C.
    //
    //  ((1430 * 10) - ADC * (3300 * 10) / 4096) / (4,3 * 10) + 25
    //    (14300 - ADC * 8) / 43 + 25
    //
    li32Zwerg = (14300 - (i16)mAdcResult[nIdx_temp]*8) / 43 + 25;
    return (i16)li32Zwerg;
  }

  i16 i16GetExtTemp_Grad()
  {
    // Für 10kOhm in Reihe mit 10KOhm NTC, B = 3380K
    const u16 lu16NtcRomTable[25] = {3898, 3839, 3767, 3680, 3577,  // -40°C, -35°C, -30°C, -25°C, -20°C,
                                     3458, 3322, 3169, 3002, 2823,  // -15°C, -10°C,  -5°C,   0°C,   5°C,
                                     2634, 2439, 2243, 2048, 1859,  //  10°C,  15°C,  20°C,  25°C,  30°C,
                                     1678, 1508, 1349, 1203, 1071,  //  35°C,  40°C,  45°C,  50°C,  55°C,
                                      951,  843,  748,  662,  587}; //  60°C,  65°C,  70°C,  75°C,  80°C,

    i16 li16AdcNtc = mAdcResult[nIdx_ntc];

    if (li16AdcNtc >= lu16NtcRomTable[0])  return -40;
    if (li16AdcNtc <= lu16NtcRomTable[24]) return  80;

    for (u8 lu8Idx = 1; lu8Idx < sizeof(lu16NtcRomTable) / 2; lu8Idx++)
    {
      if (li16AdcNtc > lu16NtcRomTable[lu8Idx])
      {
         i16 li16DiffDigit = lu16NtcRomTable[lu8Idx-1] - lu16NtcRomTable[lu8Idx];
         i16 li16DiffTemp  = -5;
         i16 li16IdxTemp   = -40 - li16DiffTemp * lu8Idx;

         return li16IdxTemp + ((li16AdcNtc - lu16NtcRomTable[lu8Idx]) * li16DiffTemp) / li16DiffDigit;
      }
    }
    return 0;
  }

  void vSetMotMode(u8 lu8Mode)        {mStatus.stStatus.mMode = lu8Mode;}
  void vSetPos_Grad(i16 li16Value)    {mPos_Soll   = li16Value;}
  void vSetSpeed_Grads(i16 li16Value) {mSpeed_Soll = li16Value;}
  void vSetDist_mm(i16 li16Value)     {mDist_Soll  = li16Value;}
  void vSetPwm_Percent(i16 li16Value) {mPwm_Soll   = li16Value;}

  void vSetLimCur_mA(i16 li16Value)   {mCur_Limit = li16Value;}
  void vSetLimPow_mW(i16 li16Value)   {mPow_Limit = li16Value;}
  void vSetLimTemp_mA(i16 li16Value)  {mPow_Limit = li16Value;}
  i16  i16Get_SetLimCur_mA()          {return mCur_Limit;}
  i16  i16Get_SetLimPow_mW()          {return mPow_Limit;}
  i16  i16Get_SetLimTemp_Grad()       {return mTemp_Limit;}

  i16 i16GetPosDigit()                { return (i16)mAdcResult[nIdx_poti];}
  // ca. 30 digit pro grad
  i16 i16GetPos_Grad()                { return (i16)(((i32)((i32)(mAdcResult[nIdx_poti]-2048) * (i32)280)) / 4096);}

  i16  i16GetPosFiltered_Grad()       {return (i16)(i32)mcInputPos_Grad;}
  i16  i16GetCurrentFiltered_mA()     {return (i16)(i32)mcInputI_mA;}
  i16  i16GetVoltFiltered_mV()        {return (i16)(i32)mcInputV_mV;}
  i16  i16GetPowFiltered_mW()         {return (i16)(((i32)mcInputV_mV * (i32)mcInputI_mA) / 1000);}
  i16  i168GetTempFilteredDegree()    {return (i16)(i32)mcInputExt_Grad;}
  i8   u8GetPwm_Percent()             {return  mPwmMotOut;}

  void vDoCurrentLimit()
  {
    u8 lu8LimitPow = 0;

    mPidPwmCur = -mPidPwmCur;

    if (mPidPwmCur > 0)
    {
      if (mPwmMotOut > 0)
      {
        lu8LimitPow = 1;
        if (mPwmMotOut >= mPidPwmCur)
        {
          mPwmMotOut -= mPidPwmCur;
        }
        else 
        { 
          mPwmMotOut = 0;
        }
      }
      if (mPwmMotOut < 0)
      {
        lu8LimitPow = 1;
        if (-mPwmMotOut >= mPidPwmCur)
        {
          mPwmMotOut += mPidPwmCur;
        }
        else
        {
          mPwmMotOut = 0;
        }
      }
    }
    mStatus.stStatus.mILim = lu8LimitPow;
  }


  void vTick1ms()
  {
    cConvertT<cFixPti1814> lcInCnvrtPos(Fp1814GetRomConstCvrtInputPosF(),     Fp1814GetRomConstCvrtInputPosO(),     Fp1814GetRomConstCvrtInputPosC());
    cConvertT<cFixPti1814> lcInCnvrtI(Fp1814GetRomConstCvrtInputCurrentF() ,  Fp1814GetRomConstCvrtInputCurrentO(), Fp1814GetRomConstCvrtInputCurrentC());
    cConvertT<cFixPti1814> lcInCnvrtU(Fp1814GetRomConstCvrtInputSupplyF(),    Fp1814GetRomConstCvrtInputSupplyO(),  Fp1814GetRomConstCvrtInputSupplyC());
    cConvertT<cFixPti1814> lcInCnvrtInt(Fp1814GetRomConstCvrtInputTemp1F(),   Fp1814GetRomConstCvrtInputTemp1O(),   Fp1814GetRomConstCvrtInputTemp1C());
    cConvertT<cFixPti1814> lcInCnvrtExt(Fp1814GetRomConstCvrtInputTemp2F(),   Fp1814GetRomConstCvrtInputTemp2O(),   Fp1814GetRomConstCvrtInputTemp2C());


    mcInputPos_Grad   = lcInCnvrtPos(mcInLpPos((i32)mAdcResult[nIdx_poti]));
    mcInputI_mA       = lcInCnvrtI(mcInLpI((i32)i16ReadCurrent()));
    mcInputV_mV       = lcInCnvrtU(mcInLpV((i32)mAdcResult[nIdx_bat]));
    mcInputInt_Grad   = lcInCnvrtInt(mcInLpTempInt((i32)i16GetIntTemp_Grad()));
    mcInputExt_Grad   = lcInCnvrtExt(mcInLpTempExt((i32)i16GetExtTemp_Grad()));


    if (u16GetSupplyVoltage_mV() < 5500)
    {
      mPowerFailCounter++;
      if (mPowerFailCounter > 100)
      {
        mStatus.stStatus.mPowFail = 1;
      }
    }
    else
    {
      mPowerFailCounter = 0;
      mStatus.stStatus.mPowFail = 0;
    }

    if (i168GetTempFilteredDegree() > 90)
    {
      mTempFailCounter++;
      if (mTempFailCounter > 100)
      {
        mStatus.stStatus.mTempFail = 1;
      }
    }
    else
    {
      mTempFailCounter = 0;
      mStatus.stStatus.mTempFail = 0;
    }

    if ((mStatus.stStatus.mPowFail) ||
        (mStatus.stStatus.mTempFail))
    {
      cSystem::vSetLed(0x55);
    }
    else
    {
      cSystem::vSetLed(u8GetRomConstLedEnable());
    }

    if ((mStatus.stStatus.mMode > 0) &&
        (!mStatus.stStatus.mPowFail) &&
        (!mStatus.stStatus.mTempFail))
    {
      // Enable motor driver
      gpio_bit_set(GPIOA, GPIO_PIN_4);  // Sleep

      cConvertT<cFixPti1814> lcScalPwmPosClip(cFixPti1814(1.0f), cFixPti1814(0.0f) , cFixPti1814(100.0f));

      if (((cFixPti1814((i32)mPos_Soll) - mcInputPos_Grad) >  Fp1814GetRomConstMinDiffInputPos()) ||
          ((cFixPti1814((i32)mPos_Soll) - mcInputPos_Grad) < -Fp1814GetRomConstMinDiffInputPos()))
      {
        mPidPwmPos = (i16)((i32)lcScalPwmPosClip(mcInPidPos(cFixPti1814((i32)mPos_Soll) - mcInputPos_Grad)));
      }
      else
      {
        mPidPwmPos = (i16)((i32)lcScalPwmPosClip(mcInPidPos(cFixPti1814((i32)0))));
      }

      cConvertT<cFixPti1814> lcScalPwmCurClip(cFixPti1814(1.0f), cFixPti1814(0.0f), cFixPti1814(100.0f));

      if (((cFixPti1814((i32)mCur_Limit) - mcInputI_mA) >  Fp1814GetRomConstMinDiffInputCurrent()) ||
          ((cFixPti1814((i32)mCur_Limit) - mcInputI_mA) < -Fp1814GetRomConstMinDiffInputCurrent()))
      {
        mPidPwmCur = (i16)((i32)lcScalPwmCurClip(mcInPidPower(cFixPti1814((i32)mCur_Limit) - mcInputI_mA)));
      }
      else
      {
        mPidPwmCur = (i16)((i32)lcScalPwmCurClip(mcInPidPower(cFixPti1814((i32)0))));
      }


      switch(mStatus.stStatus.mMode)
      {
        case 1: // Servo by Pos
          {
            mPwmMotOut = mPidPwmPos;
            vDoCurrentLimit();
          }
          break;

        case 4: // Motor by PMW
          {
            mPwmMotOut = mPwm_Soll;
            vDoCurrentLimit();
          }
          break;
        case 5: // Motor by Speed
          {
            
          }
          break;
        case 6: // Motor by Distance
          {
            
          }
          break;
        default:
          mPwmMotOut = 0;
          break;
      }

      if (cSystem::mAutomatik)
      {
        if ((mPwmMotOut > -u8GetRomConstMinPwm()) && (mPwmMotOut < u8GetRomConstMinPwm()))
        {
          mPwmMotOut = 0;
        }
        vSetPwm(mPwmMotOut);
      }
    }
    else
    {
      vSetPwm(0);
      // Disable motor driver
      gpio_bit_reset(GPIOA, GPIO_PIN_4);  // Sleep
    }
  }
};

#endif // __SERVO1_APPLIKATION_H__
