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
#include "cFixPti2210.h"

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

template <const i16 tyiTestPoint> struct cPosCnt5P
{
  typedef enum
  {
    nIdle = 0,
    nMinus2,
    nMinus1,
    nNull,
    nPlus1,
    nPlus2,
  }tenStates;

  tenStates mSmUZ;
  tenStates mSmGUZ;

  cPosCnt5P()
  {
    mSmUZ = mSmGUZ = nIdle;
  }

  bool isIn(i16 i16Pos, const i16 i16Limit)
  {
    if ((i16Pos >= (i16Limit - 2)) && (i16Pos <= (i16Limit + 2)))
    {
      return True;
    }
    else
    {
      return False;
    }
  }


  bool isOutUZ(i16 i16Pos, const i16 i16Limit)
  {
    if (i16Pos > (i16Limit + 2)) {return True;}
                            else {return False;}
  }

  // UZ: Uhrzeigersinn mathematisch negativ
  void vProcessUZ(i16 &li16Cnt, i16 i16Pos)
  {
    switch (mSmUZ)
    {
      case nIdle:
        if (isIn(i16Pos, tyiTestPoint + 10)) { mSmUZ = nPlus2; }
        break;
      case nPlus2:
        if (isIn(i16Pos, tyiTestPoint + 5)) { mSmUZ = nPlus1; }
        else if (isOutUZ(i16Pos, tyiTestPoint + 10)) { mSmUZ = nIdle; }
        break;
      case nPlus1:
        if (isIn(i16Pos, tyiTestPoint)) { mSmUZ = nNull; }
        else if (isOutUZ(i16Pos, tyiTestPoint + 5)) { mSmUZ = nIdle; }
        break;
      case nNull:
        if (isIn(i16Pos, tyiTestPoint - 5)) { mSmUZ = nMinus1; }
        else if (isOutUZ(i16Pos, tyiTestPoint)) { mSmUZ = nIdle; }
        break;
      case nMinus1:
        if (isIn(i16Pos, tyiTestPoint - 10)) { mSmUZ = nMinus2; }
        else if (isOutUZ(i16Pos, tyiTestPoint - 5 )) { mSmUZ = nIdle; }
        break;
      case nMinus2:
        li16Cnt--;  mSmUZ = nIdle;
        break;
    }
  }

  // GUZ: Gegenuhrzeigersinn mathematisch positiv
  bool isOutGUZ(i16 i16Pos, const i16 i16Limit)
  {
    if (i16Pos < (i16Limit - 2)) {return True;}
                           else  {return False;}
  }

  void vProcessGUZ(i16& li16Cnt, i16 i16Pos)
  {
    switch (mSmGUZ)
    {
      case nIdle:
        if (isIn(i16Pos, tyiTestPoint - 10)) { mSmGUZ = nMinus2; }
        break;
      case nMinus2:
        if (isIn(i16Pos, tyiTestPoint - 5)) { mSmGUZ = nMinus1; }
        else if (isOutGUZ(i16Pos, tyiTestPoint - 10)) { mSmGUZ = nIdle; }
        break;
      case nMinus1:
        if (isIn(i16Pos, tyiTestPoint)) { mSmGUZ = nNull; }
        else if (isOutGUZ(i16Pos, tyiTestPoint - 5)) { mSmGUZ = nIdle; }
        break;
      case nNull:
        if (isIn(i16Pos, tyiTestPoint + 5)) { mSmGUZ = nPlus1; }
        else if (isOutGUZ(i16Pos, tyiTestPoint)) { mSmGUZ = nIdle; }
        break;
      case nPlus1:
        if (isIn(i16Pos, tyiTestPoint + 10)) { mSmGUZ = nPlus2; }
        else if (isOutGUZ(i16Pos, tyiTestPoint + 5)) { mSmGUZ = nIdle; }
        break;
      case nPlus2:
        li16Cnt++;  mSmGUZ = nIdle;
        break;
    }
  }

  void vProcess(i16& li16Cnt, i16 i16Pos, i8 i8Pwm)
  {
    if (i8Pwm < 0) vProcessUZ(li16Cnt,  i16Pos);
    if (i8Pwm > 0) vProcessGUZ(li16Cnt, i16Pos);
  }
};

template <const i16 tyiTestPoint> struct cPosCnt3P
{
  typedef enum
  {
    nIdle = 0,
    nMinus1,
    nNull,
    nPlus1,
  }tenStates;

  tenStates mSmUZ;
  tenStates mSmGUZ;

  cPosCnt3P()
  {
    mSmUZ = mSmGUZ = nIdle;
  }

  bool isIn(i16 i16Pos, const i16 i16Limit)
  {
    if ((i16Pos >= (i16Limit - 2)) && (i16Pos <= (i16Limit + 2)))
    {
      return True;
    }
    else
    {
      return False;
    }
  }


  bool isOutUZ(i16 i16Pos, const i16 i16Limit)
  {
    if (i16Pos > (i16Limit + 2)) { return True; }
    else { return False; }
  }

  // UZ: Uhrzeigersinn mathematisch negativ
  void vProcessUZ(i16& li16Cnt, i16 i16Pos)
  {
    switch (mSmUZ)
    {
    case nIdle:
      if (isIn(i16Pos, tyiTestPoint + 5)) { mSmUZ = nPlus1; }
      break;
    case nPlus1:
      if (isIn(i16Pos, tyiTestPoint)) { mSmUZ = nNull; }
      else if (isOutUZ(i16Pos, tyiTestPoint + 5)) { mSmUZ = nIdle; }
      break;
    case nNull:
      if (isIn(i16Pos, tyiTestPoint - 5)) { mSmUZ = nMinus1; }
      else if (isOutUZ(i16Pos, tyiTestPoint)) { mSmUZ = nIdle; }
      break;
    case nMinus1:
      li16Cnt--;  mSmUZ = nIdle;
      break;
    }
  }

  // GUZ: Gegenuhrzeigersinn mathematisch positiv
  bool isOutGUZ(i16 i16Pos, const i16 i16Limit)
  {
    if (i16Pos < (i16Limit - 2)) { return True; }
    else { return False; }
  }

  void vProcessGUZ(i16& li16Cnt, i16 i16Pos)
  {
    switch (mSmGUZ)
    {
    case nIdle:
      if (isIn(i16Pos, tyiTestPoint - 5)) { mSmGUZ = nMinus1; }
      break;
    case nMinus1:
      if (isIn(i16Pos, tyiTestPoint)) { mSmGUZ = nNull; }
      else if (isOutGUZ(i16Pos, tyiTestPoint - 5)) { mSmGUZ = nIdle; }
      break;
    case nNull:
      if (isIn(i16Pos, tyiTestPoint + 5)) { mSmGUZ = nPlus1; }
      else if (isOutGUZ(i16Pos, tyiTestPoint)) { mSmGUZ = nIdle; }
      break;
    case nPlus1:
      li16Cnt++;  mSmGUZ = nIdle;
      break;
    }
  }

  void vProcess(i16& li16Cnt, i16 i16Pos, i8 i8Pwm)
  {
    if (i8Pwm < 0) vProcessUZ(li16Cnt, i16Pos);
    if (i8Pwm > 0) vProcessGUZ(li16Cnt, i16Pos);
  }
};


template <typename T> struct cLowPassT
{
  T mValue;
  T mFactor;

  cLowPassT() {}

  cLowPassT(const T lFactor)
  {
    vInit(lFactor);
  }

  void vInit(const T lFactor)
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

  u8* u8Encode(u8* lu8Data)
  {
    return mFactor.u8Encode(lu8Data);
  }

  u8* u8Decode(u8* lu8Data)
  {
    return mFactor.u8Decode(lu8Data);
  }
};


template <typename T> struct cClipT
{
  T mClip;

  cClipT() {}

  cClipT(const T lClip)
  {
    vInit(lClip);
  }

  void vInit(const T lClip)
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

  cConvertT() {}

  cConvertT(const T lFactor, const T lOffset, const T lClip)
  {
    vInit(lFactor, lOffset, lClip);
  }

  void vInit(const T lFactor, const T lOffset, const T lClip)
  {
    mOffset = lOffset;
    mFactor = lFactor;
    mClip.vInit(lClip);
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

  cPidT() {};

  cPidT(const T lKp, const T lKi, const T lKd, const T lClamp)
  {
    vInit(lKp, lKi, lKd, lClamp);
  }

  void vInit(const T lKp, const T lKi, const T lKd, const T lClamp)
  {
    mKp = lKp;
    mKi = lKi;
    mKd = lKd;
    mOutputLimit = lClamp;

    mErrOld = T((i32)0);
    mErrIntegral = T((i32)0);
    mDeltaError = T((i32)0);
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

  u8* u8Encode(u8* lu8Data)
  {
    return mOutputLimit.u8Encode(mKd.u8Encode(mKi.u8Encode(mKp.u8Encode(lu8Data))));
  }

  u8* u8Decode(u8* lu8Data)
  {
    return mOutputLimit.u8Decode(mKd.u8Decode(mKi.u8Decode(mKp.u8Decode(lu8Data))));
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

  // Order must match RomConst

  typedef enum
  {
    nPos,     // [°]
    nSpeed,   // [°/s]
    nCurrent, // [mA]
    nPower,   // [mW]
    nSupply,  // [mV]
    nInt,     // [°C]
    nExt,     // [°C]
    nPwm      // [0..100 %]
  }tenValues;

  // Sollwerte
  cFixPti2210 mcSoll[8];
  cFixPti2210 mcIst[8];
  cFixPti2210 mcMin[8];

  cLowPassT<cFixPti2210> mcLp[8];
  cConvertT<cFixPti2210> mcCvt[8];

  cPidT<cFixPti2210> mcPid[4];
  cFixPti2210        mcPidOut[4];

  //cPosCnt5P< 90>        mcCnt5P1;
  //cPosCnt5P<-90>        mcCnt5P2;

  cPosCnt5P<0>          mcCnt3P;

  i16                 mi16Dist;

  tunStatus mStatus;

  cServo1_Applikation()
  {
    u8 lu8Idx;

    mStatus.stStatus.mMode = 1;

    // Romconst Init werte für Sollwerte rüberkopieren
    cFixPti2210* lpFpSoll =    (cFixPti2210*)u8PtrRomConstInitPos_degree();
    // Romconst Min werte rüberkopieren
    cFixPti2210* lpFpMin = (cFixPti2210*)u8PtrRomConstMinDiffInputPos();
    // Romconst Lowpass werte rüberkopieren
    cFixPti2210* lpFpLp =  (cFixPti2210*)u8PtrRomConstLpInputPos();
    // Romconst Skalierungswerte rüberkopieren
    cFixPti2210* lpFpCvt = (cFixPti2210*)u8PtrRomConstCvrtInputPosF();
    for (lu8Idx = 0; lu8Idx < 8; lu8Idx++)
    {
      mcSoll[lu8Idx] = *lpFpSoll++;
      mcMin[lu8Idx]  = *lpFpMin++;
      mcLp[lu8Idx].vInit(*lpFpLp++);
      mcCvt[lu8Idx].vInit(lpFpCvt[0], lpFpCvt[1], lpFpCvt[2]);
      lpFpCvt += 3;
    }

    // Romconst PID-Werte rüberkopieren
    cFixPti2210* lpFpPid = (cFixPti2210*)u8PtrRomConstPidPosKp();
    for (lu8Idx = 0; lu8Idx < 4; lu8Idx++)
    {
      mcPid[lu8Idx].vInit(lpFpPid[0], lpFpPid[1], lpFpPid[2], lpFpPid[3]);
      lpFpPid += 4;
    }

    mi16Dist = 0;

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
    return (u16)((i32)mcIst[nSupply]);
  }

  i16 i16GetCurrent_mA()
  {
    return (i16)((i32)mcIst[nCurrent]);
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

  i16 i16GetRomConstTabTempValue(u16* lu16PtrTab, const i16 i16MinVal, const i16 i16MaxVal, const u8 lu8MaxIdx, const u16 lu16AdcNtc)
  {
    if (lu16AdcNtc >= lu16PtrTab[0])         return i16MinVal;
    if (lu16AdcNtc <= lu16PtrTab[lu8MaxIdx]) return i16MaxVal;

    for (u8 lu8Idx = 1; lu8Idx < lu8MaxIdx; lu8Idx++)
    {
      if (lu16AdcNtc >= lu16PtrTab[lu8Idx])
      {
        i16 li16DiffDigit = lu16PtrTab[lu8Idx - 1] - lu16PtrTab[lu8Idx];
        i16 li16DiffTemp = -((i16MaxVal - i16MinVal) / (lu8MaxIdx - 1));
        i16 li16IdxTemp = i16MinVal - li16DiffTemp * lu8Idx;

        return li16IdxTemp + (((i16)((i16)lu16AdcNtc - (i16)lu16PtrTab[lu8Idx])) * li16DiffTemp) / li16DiffDigit;
      }
    }
    return 0;
  }

  i16 i16GetIntTemp_Grad()
  {
    return i16GetRomConstTabTempValue((u16*)u8PtrRomConstIntTempTab1(), -40, 115, 31, mAdcResult[nIdx_temp]);
  }

  i16 i16GetExtTemp_Grad()
  {
    return i16GetRomConstTabTempValue((u16*)u8PtrRomConstExtTempTab1(), -40, 115, 31, mAdcResult[nIdx_ntc]);
  }

  void vSetMotMode(u8 lu8Mode)        {mStatus.stStatus.mMode = lu8Mode;}
  void vSetPos_Grad(i16 li16Value)    { mcSoll[nPos] = (i32)li16Value;}
  void vSetSpeed_Grads(i16 li16Value) { mcSoll[nSpeed] = (i32)li16Value;}
  void vSetPwm_Percent(i16 li16Value) { mcSoll[nPwm] = (i32)li16Value;}

  void vSetLimCur_mA(u16 lu16Value)       {mcSoll[nCurrent] = (i32)lu16Value;}
  void vSetLimPow_W(u16 lu16Value)        {mcSoll[nPower]   = (i32)lu16Value;}
  void vSetLimTemp_degree(i16 li16Value)  {mcSoll[nExt]     = mcSoll[nInt] = (i32)li16Value;}
  u16  u16Get_SetLimCur_mA()          {return (u16)(i32)mcSoll[nCurrent];}
  u16  u16Get_SetLimPow_mW()          {return (u16)(i32)mcSoll[nPower];}
  i16  i16Get_SetLimTemp_Grad()       {return (i16)(i32)mcSoll[nExt];}

  i16 i16GetPosDigit()                { return (i16)mAdcResult[nIdx_poti];}
  // ca. 30 digit pro grad
  i16 i16GetPos_Grad()                { return (i16)(((i32)((i32)(mAdcResult[nIdx_poti]-2048) * (i32)280)) / 4096);}

  i16  i16GetPosFiltered_Grad()       {return (i16)(i32)mcIst[nPos];}
  i16  i16GetCurrentFiltered_mA()     {return (i16)(i32)mcIst[nCurrent];}
  i16  i16GetVoltFiltered_mV()        {return (i16)(i32)mcIst[nSupply];}
  i16  u16GetPowFiltered_mW()         {return (u16)(i32)mcIst[nPower];}
  i16  i168GetTempFilteredDegree()    {return (i16)(i32)mcIst[nExt];}
  i8   u8GetPwm_Percent()             {return (i8)(i32)mcIst[nPwm];}

  bool boDoLimit(cFixPti2210 lcLimit)
  {
    bool lbLimitPow = False;

    lcLimit = -lcLimit;

    if (lcLimit > cFixPti2210(0.0f))
    {
      if (mcIst[nPwm] > cFixPti2210(0.0f))
      {
        if (mcIst[nPwm] >= lcLimit)
        {
          lbLimitPow = True;
          mcIst[nPwm] -= lcLimit;
        }
      }
      if (mcIst[nPwm] < cFixPti2210(0.0f))
      {
        if (-mcIst[nPwm] >= lcLimit)
        {
          lbLimitPow = True;
          mcIst[nPwm] += lcLimit;
        }
      }
    }
    return lbLimitPow;
  }


  void vTick1ms()
  {
    mcIst[nPos]     = mcCvt[nPos](mcLp[nPos]((i32)mAdcResult[nIdx_poti]));
    mcIst[nCurrent] = mcCvt[nCurrent](mcLp[nCurrent]((i32)i16ReadCurrent()));
    mcIst[nSupply]  = mcCvt[nSupply](mcLp[nSupply]((i32)mAdcResult[nIdx_bat]));
    mcIst[nInt]     = mcCvt[nInt](mcLp[nInt]((i32)i16GetIntTemp_Grad()));
    mcIst[nExt]     = mcCvt[nExt](mcLp[nExt]((i32)i16GetExtTemp_Grad()));

    mcIst[nPower]   = mcCvt[nPower]((mcIst[nSupply]  * cFixPti2210(0.001f)) * mcIst[nCurrent]);

    // Note:
    //   - I und P sind richtungsunabhängig und daher immer posetiv
    //   - Pos, PWM, Speed und Distance werden im uhrzeigersinn (UZS) größer
    //      - Pos: Mitte 0, wird größer in UZS
    //      - Speed: Speed > 0, d.h. Motor dreht sich im UZS
    //      - PWM: PWM > 0, d.h. Motor dreht sich im UZS
    //      - Distance: Distance > 0, d.h. Motor dreht sich im UZS


    if (mcIst[nSupply] < mcSoll[nSupply])
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

    if (mcIst[nExt] > mcSoll[nExt])
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


      u8 lu8PidIdx;
      for (lu8PidIdx = 0; lu8PidIdx < 4; lu8PidIdx++)
      {
        cFixPti2210 lcDiff = mcSoll[lu8PidIdx] - mcIst[lu8PidIdx];

        if (lcDiff.isIn(mcMin[lu8PidIdx]))
        {
          lcDiff = cFixPti2210(0.0f);
        }
        mcPidOut[lu8PidIdx] = mcCvt[nPwm](mcPid[lu8PidIdx](lcDiff));
      }


      switch(mStatus.stStatus.mMode)
      {
        case 1: // Servo by Pos
          {
            mcIst[nPwm] = mcPidOut[nPos];
            mStatus.stStatus.mILim   = boDoLimit(mcPidOut[nCurrent]);
            mStatus.stStatus.mPowLim = boDoLimit(mcPidOut[nPower]);
          }
          break;

        case 4: // Motor by PMW
          {
            mcIst[nPwm] = mcSoll[nPwm];
            mStatus.stStatus.mILim = boDoLimit(mcPidOut[nCurrent]);
            mStatus.stStatus.mPowLim = boDoLimit(mcPidOut[nPower]);

            //mcCnt5P1.vProcess(mi16Dist, i16GetPosFiltered_Grad(), u8GetPwm_Percent());
            //mcCnt5P2.vProcess(mi16Dist, i16GetPosFiltered_Grad(), u8GetPwm_Percent());

            mcCnt3P.vProcess(mi16Dist, i16GetPosFiltered_Grad(), u8GetPwm_Percent());
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
          mcIst[nPwm] = cFixPti2210(0.0f);
          break;
      }

      if (cSystem::mAutomatik)
      {
        mcIst[nPwm] = mcLp[nPwm](mcIst[nPwm]);
        if (mcIst[nPwm].isIn(mcMin[nPwm]))
        {
          vSetPwm(0);
        }
        else
        {
          vSetPwm((i16)(i32)mcIst[nPwm]);
        }
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
