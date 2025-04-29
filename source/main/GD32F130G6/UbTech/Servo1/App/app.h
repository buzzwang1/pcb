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
    u8 mMotEnable : 1;
    u8 mMode      : 3; // 0: Servo by Pos
                       // 4: Motor by PMW
                       // 5: Motor by Current
    u8 mAutomatik : 1; // 0: Maneller Modus; 1: Automatischer Modus
  }tstStatus;

  typedef union
  {
    u8        u8Status;
    tstStatus stStatus;
  }tunStatus;

  u16       mAdcResult[nChnCount];
  tunStatus mStatus;
  u16       mPowerFailCounter;

  i16 mSpeed_Soll; // [in Pwm]
  i16 mPos_Soll;   // [Grad]
  i16 mCur_Soll;   // [mA]

  class cFp1814LowPass
  {
    public:
    cFixPti1814 mValue;
    cFixPti1814 mFactor;

    cFp1814LowPass(i32 li32Factor)
    {
      mFactor.vSetFrac(li32Factor);
      mValue.vSet(0);
    }

    cFixPti1814 DoProcess(cFixPti1814 lFpNewValue) // __attribute__((optimize("-O0")))
    {
      mValue = mFactor * lFpNewValue + (cFixPti1814(1) - mFactor) * mValue;
      return mValue;
    }
  };

  class cFp1814InputValues
  {
    public:
    cFp1814LowPass mLpPos;
    cFp1814LowPass mLpI;
    cFp1814LowPass mLpV;
    cFp1814LowPass mLpTemp;

    cFixPti1814 mFpPos;
    cFixPti1814 mFpI;
    cFixPti1814 mFpV;
    cFixPti1814 mFpTemp;


    cFp1814InputValues()
      : mLpPos(u32GetRomConstLpInputPos()),
        mLpI(u32GetRomConstLpInputCurrent()),
        mLpV(u32GetRomConstLpInputSupply()),
        mLpTemp(u32GetRomConstLpInputTemp())
    {
    }
  };

  class cInputValuesi16
  {
  public:
    i16 mPos;
    i16 mI;
    i16 mV;
    i16 mTemp;

    void vSet(cFp1814InputValues* lcInputFp)
    {
      mPos  = lcInputFp->mFpPos.mFp.stFp.Int;
      mI    = lcInputFp->mFpI.mFp.stFp.Int;
      mV    = lcInputFp->mFpV.mFp.stFp.Int;
      mTemp = lcInputFp->mFpTemp.mFp.stFp.Int;
    }
  };

  cFp1814InputValues mcInputFp;
  cInputValuesi16 mcInputi16;


  class cFpPid
  {
    public:
    cFixPti1814 mFpErrOld;

    cFixPti1814 mFpErrIntegral;
    cFixPti1814 mFpDeltaError;

    cFp1814LowPass mLpOutput;

    cFixPti1814 mFpKp;
    cFixPti1814 mFpKi;
    cFixPti1814 mFpKd;

    i16         mi16OutputLimit;

    cFpPid(u32 lu32LpInitValue)
      : mLpOutput(lu32LpInitValue)
    {
    }

    i16 i16DoProcess(cFixPti1814 lFpErr) // __attribute__((optimize("-O0")))
    {
      mFpDeltaError = lFpErr - mFpErrOld;
      mFpErrOld = lFpErr;

      // PID
      cFixPti1814 v = mFpKp * lFpErr +
                      mFpErrIntegral +
                      mFpKd * mFpDeltaError;

      bool saturating = False;

      if (v.i32Get() >= mi16OutputLimit)       {v = cFixPti1814( mi16OutputLimit); saturating = True;}
      else if (v.i32Get() <= -mi16OutputLimit) {v = cFixPti1814(-mi16OutputLimit); saturating = True;}

      // error and output same sign
      bool sign = (lFpErr.i8Sign() * v.i8Sign() > 0);
      // zero
      bool clamp = saturating && sign;
      if(!clamp)
      {
        mFpErrIntegral += (mFpKi * lFpErr);
      }

      // output
      return mLpOutput.DoProcess(v).i32Get();
    }
  };

  class cFpPidPos : public cFpPid
  {
    public:
      cFpPidPos()
        :cFpPid(u32GetRomConstPidPosLpOutput())
      {
        mFpKp.vSetFrac(u32GetRomConstPidPosKp());
        mFpKi.vSetFrac(u32GetRomConstPidPosKi());
        mFpKd.vSetFrac(u32GetRomConstPidPosKd());
        mi16OutputLimit = u16GetRomConstPidPosLimit();
      }
  };

  class cFpPidCur : public cFpPid
  {
    public:
      cFpPidCur()
        :cFpPid(u32GetRomConstPidCurLpOutput())
      {
        mFpKp.vSetFrac(u32GetRomConstPidCurKp());
        mFpKi.vSetFrac(u32GetRomConstPidCurKi());
        mFpKd.vSetFrac(u32GetRomConstPidCurKd());
        mi16OutputLimit = u16GetRomConstPidCurLimit();
      }
  };

  cFpPidPos mcMyPidPos;
  cFpPidCur mcMyPidCur;

  cServo1_Applikation()
  {
    mStatus.stStatus.mMotEnable = 0;
    mStatus.stStatus.mMode = 0;
    mStatus.stStatus.mAutomatik = 1;

    mSpeed_Soll = 0;
    mPos_Soll   = 0;
    mCur_Soll   = 1000;

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
    TIMER_PSC(TIMER0)    = 3;      // 2 Mhz
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
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV2);

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
      // >0: im Uhrzeigersinn drehen
      gpio_bit_set(GPIOA, GPIO_PIN_3);  // PHASE
    }
    else // if (li16Value < 0)
    {
      // <0: im Gegen-Uhrzeigersinn drehen
      gpio_bit_reset(GPIOA, GPIO_PIN_3); // PHASE
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

  u8 u8GetLedEnable() { return u8GetRomConstLedEnable(); }

  u16 u16GetSupplyVoltage()
  {
    u32 lu32Zwerg;
    lu32Zwerg = ((u32)mAdcResult[nIdx_bat] * 14520)/4096;
    return (u16)lu32Zwerg;
  }

  i16 i16GetCurrent()
  {
    return (u16)mcInputFp.mFpI.mFp.stFp.Int;
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

    if (mSpeed_Soll < 0)  lu32Zwerg = -lu32Zwerg;
    return (i16)lu32Zwerg;
  }

  i16 i16GetIntTemp()
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

  i16 i16GetExtTemp()
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

  void vSetMotEnable()     {mStatus.stStatus.mMotEnable = 1;}
  void vSetMotDisable()    {mStatus.stStatus.mMotEnable = 0;}

  void vSetMotAuto_Enable()  { mStatus.stStatus.mAutomatik = 1; }
  void vSetMotAuto_Disable() { mStatus.stStatus.mAutomatik = 0; }

  void vSetMotMode_ServoPos()    { mStatus.stStatus.mMode = 0; }
  void vSetMotMode_ServoPosCur() { mStatus.stStatus.mMode = 1; }
  void vSetMotMode_MotorPwm()    { mStatus.stStatus.mMode = 4; }
  void vSetMotMode_MotorCur()    { mStatus.stStatus.mMode = 5; }

  void vSetSpeed(i16 li16Value)  { mSpeed_Soll = li16Value; }

  i16 i16GetPosDigit()         { return (i16)mAdcResult[nIdx_poti];}
  // ca. 30 digit pro grad
  i16 i16GetPosDegree()        { return (i16)(((i32)((i32)(mAdcResult[nIdx_poti]-2048) * (i32)280)) / 4096);}

  cFixPti1814 Fp1814GetPosDegree()
  {
    cFixPti1814 lFpZwerg;
    lFpZwerg.mFp.i32Fp = (((i32)(mAdcResult[nIdx_poti] - 2048) * (i32)280 * (i32)(1 << (14-12))));
    return lFpZwerg;
  }

  i16  i16GetPosFilteredDegree()      {return mcInputi16.mPos;}
  i16  i16GetCurrentFilteredDegree()  {return mcInputi16.mI;}
  i16  i16GetVoltFilteredDegree()     {return mcInputi16.mV;}
  i16  i16GetTempFilteredDegree()     {return mcInputi16.mTemp;}

  void vSetPosDegree(i16 li16Value)  { mPos_Soll = li16Value;}
  void vSetCur_mA(i16 li16Value)     { mCur_Soll = li16Value;}

  void vTick1ms()
  {
    static u8 lu8PowerFailCounter = 0;
    if (u16GetSupplyVoltage() < 5500)
    {
      mPowerFailCounter++;
      if (lu8PowerFailCounter > 100)
      {
        //mStatus.stStatus.mMotEnable = 0;
        lu8PowerFailCounter = 0;
      }
      else
      {
        lu8PowerFailCounter++;
      }
    }
    else
    {
      lu8PowerFailCounter = 0;
    }

    mcInputFp.mFpPos  = mcInputFp.mLpPos.DoProcess(Fp1814GetPosDegree());
    mcInputFp.mFpI    = mcInputFp.mLpI.DoProcess(cFixPti1814(i16ReadCurrent()));
    mcInputFp.mFpV    = mcInputFp.mLpV.DoProcess(cFixPti1814(u16GetSupplyVoltage()));
    mcInputFp.mFpTemp = mcInputFp.mLpTemp.DoProcess(cFixPti1814(i16GetExtTemp()));
    mcInputi16.vSet(&mcInputFp);

    if (mStatus.stStatus.mMotEnable)
    {
      // Enable motor driver
      gpio_bit_set(GPIOA, GPIO_PIN_4);  // Sleep

      switch(mStatus.stStatus.mMode)
      {
        case 0: // Servo-Modus: Positions-Regelung
          {
            i32 li32Pwm;

            mcMyPidPos.mi16OutputLimit = u16GetRomConstPidPosLimit();
            li32Pwm = mcMyPidPos.i16DoProcess(cFixPti1814(mPos_Soll) - mcInputFp.mFpPos);

              //Unter 8 passiert eh nichts
            if ((li32Pwm > -8) && (li32Pwm < 8))
            {
              li32Pwm = 0;
            }
            mSpeed_Soll = (i16)li32Pwm;
          }
          break;
        case 1: // Servo-Modus: Positions-Strom-Regelung
          {
            cFixPti1814 lFpCurrent_Soll;
            i32 li32Pwm;

            mcMyPidPos.mi16OutputLimit = mCur_Soll; //mA
            mcMyPidPos.i16DoProcess(cFixPti1814(mPos_Soll) - mcInputFp.mFpPos);
            lFpCurrent_Soll = mcMyPidPos.mLpOutput.mValue;

            if ((lFpCurrent_Soll.mFp.stFp.Int <  4) &&
                (lFpCurrent_Soll.mFp.stFp.Int > -4))
            {
              li32Pwm = 0;
            }
            else
            {
              li32Pwm = mcMyPidCur.i16DoProcess(lFpCurrent_Soll - mcInputFp.mFpI);
            }

            //Unter 8 passiert eh nichts
            if ((li32Pwm > -8) && (li32Pwm < 8))
            {
              li32Pwm = 0;
            }
            mSpeed_Soll = (i16)li32Pwm;
          }
          break;

        case 4: // Motor PWM-Modus
          {
          }
          break;
        case 5: // Motor Current-Modus
          {
            i32 li32Pwm;
            li32Pwm = mcMyPidCur.i16DoProcess(cFixPti1814(mCur_Soll) - mcInputFp.mFpI);

            // Unter 8 passiert eh nichts
            if ((li32Pwm > -8) && (li32Pwm < 8))
            {
              li32Pwm = 0;
            }
            mSpeed_Soll = (i16)li32Pwm;
          }
          break;
      }

      if (mStatus.stStatus.mAutomatik)
      {
        vSetPwm(mSpeed_Soll);
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
