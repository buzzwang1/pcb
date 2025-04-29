#ifndef __SERVO1_APPLIKATION_H__
#define __SERVO1_APPLIKATION_H__

#include "Typedef.h"
#include "cCli.h"
#include "cBnStreamSysPortBase.h"


#include "TypeDef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_adc.h"
#include "gd32f1x0_gpio.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_timer.h"
#include "gd32f1x0_misc.h"


class cServo1_Applikation
{
public:
  enum
  {
    nV_ntc    =  0, // ADC Channel  0: externen Temperatursensor (ntc)
    nV_bat    =  1, // ADC Channel  1:
    nV_imot   =  2, // ADC Channel  2:
    nV_poti   =  7, // ADC Channel  7:
    nV_temp   = 17, // ADC Channel 17: interner Temperatursensor

    nIdx_ntc    =  0,
    nIdx_bat    =  1,
    nIdx_imot   =  2,
    nIdx_poti   =  3,
    nIdx_temp   =  4,

    nChnCount = 5,

    nSpeedInc = 5,
    nDegree2Digit = 30
  };

  typedef struct
  {
    u8 mMotEnable : 1;
    u8 mMode      : 1; // 0: Servo; 1: Motor
    u8 mAutomatik : 1; // 0: Maneller Modus; 1: Automatischer Modus
  }tstStatus;

  u16       mAdcResult[nChnCount];
  tstStatus mStatus;

  i16 mSpeed_Soll;
  i16 mSpeed_Ist;

  i16 mPos_Soll;

  class cPid_i32
  {
    public:
    i16 mPidKp;
    i16 mPidKd;
    i16 mPidKi;

    i16 mPidKi_Limit;

    i16 mErr_Alt;

    i32 mErr_Sum;

    i32 mPidP;
    i32 mPidD;
    i32 mPidI;
    i32 mPidOut;

    typedef struct
    {
      u8 mWindupEn1 : 1; // 1: PID I-Windup Enable 
      u8 mWindupEn2 : 1; // 1: PID I-Windup Enable 
      u8 mWindupEn3 : 1; // 1: PID I-Windup Enable 
    }tstStatus;

    tstStatus mStatus;

    cPid_i32()
    {
      vReset();
    }

    void vReset()
    {
      cMemTools::vMemSet((u8*)this, 0, sizeof(cPid_i32));

      mPidKp = 32;
      mPidKd = 0;
      mPidKi = 1;

      mPidKi_Limit = 256;

      mStatus.mWindupEn1 = 1;
      mStatus.mWindupEn2 = 1;
      mStatus.mWindupEn3 = 1;
    }

    i32 i32DoProcess(i16 li16Error)
    {
      // https://rn-wissen.de/wiki/index.php/Regelungstechnik

      // Integral windup supression
      // - Initializing the controller integral to a desired value, for instance to the value before the problem[citation needed]
      // - Increasing the setpoint in a suitable ramp
      // - "Conditional Integration"--disabling the integral function until the to-be-controlled process variable (PV) has entered the controllable region[3]
      // - Preventing the integral term from accumulating above or below pre-determined bounds[4]
      // - Back-calculating the integral term to constrain the process output within feasible bounds.[5][6][3]
      // - Zeroing the integral value every time the error is equal to, or crosses zero. This avoids having the controller attempt to drive the system to have the same error integral in the opposite direction as was caused by a perturbation.[citation needed]

      if ((mPidOut > -100) || (mPidOut < 100) && (mStatus.mWindupEn1))
      {
        mErr_Sum += li16Error;
      }

      if (mStatus.mWindupEn2)
      {
        u8 lu8SignError = 0;
        if (li16Error >= 0) lu8SignError = 1;

        u8 lu8SignError_Alt = 0;
        if (mErr_Alt >= 0)  lu8SignError_Alt = 1;

        if (lu8SignError != lu8SignError_Alt) mErr_Sum = 0;
      }

      if (mStatus.mWindupEn3)
      {
        if (mErr_Sum > mPidKi_Limit) mErr_Sum = mPidKi_Limit;
        else if (mErr_Sum < -(mPidKi_Limit)) mErr_Sum = -mPidKi_Limit;
      }

      mPidP = (mPidKp * li16Error) / 256;
      mPidI = (mPidKi * mErr_Sum) / 256;
      mPidD = (mPidKd * (li16Error - mErr_Alt)) / 256;
      mPidOut = mPidP + mPidI + mPidD;

      mErr_Alt = li16Error;

      return mPidOut;
    }
  };

  cPid_i32 mcMyPid;

  cServo1_Applikation()
  {
    mStatus.mMotEnable = 1;
    mStatus.mMode = 0;
    mStatus.mAutomatik = 1;

    mSpeed_Soll = 0;
    mSpeed_Ist  = 0;

    mPos_Soll = 2048;

    // ------------- gpio_config
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure PC0 (adc channel10) as analog input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_7);


    /* Motor */
    //Input
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);  // Fault indication. Open-drain output, logic low when in fault condition (OCP, OTP, OVP).

    //Output
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4); // Sleep mode input. Logic low to enter low-power sleep mode. Internal pulldown.

    //gpio_mode_set(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_8); // H-bridge enable input. High to enable H-bridge. Internal pulldown.
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3); // H-bridge phase input (motor direction). Internal pulldown.
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5); // Brake input. Internal pulldown.

    /*Configure PA8(TIMER0_CH0) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);


    // ------------ Timer0_Config
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
        timer0CLK = SystemCoreClock / 108 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler = 3; // 0.5us
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 99;  // 0.5us * 100 = 50us = 20khz
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0/*250*/);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);


    // dma_ch0 config used for ADC
    // enable the DMA clock
    rcu_periph_clock_enable(RCU_DMA);

    dma_parameter_struct dma_init_struct;

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
    dma_channel_enable(DMA_CH0);


    // ------------ ADC_Config
    // enable the DMA clock
    rcu_periph_clock_enable(RCU_ADC);

    // ADC Takt 4Mhz = 0 => 0,25us
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV2);

    // ADC SCAN function enable
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);

    // ADC channel length config
    adc_channel_length_config(ADC_REGULAR_CHANNEL, nChnCount);

    //adc_vbat_enable();
    adc_tempsensor_vrefint_enable();

    // 55.5cycles: 57 => 0,25us * 57 = 14,25us
    // 71.5cycles: 73 => 0,25us * 73 = 18,25us
    // ADC regular channel config
    adc_regular_channel_config(0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(3, ADC_CHANNEL_7, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(4, ADC_CHANNEL_17, ADC_SAMPLETIME_71POINT5);

    // ADC data alignment config
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    // ADC SCAN function enable
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    // ADC DMA enable
    adc_dma_mode_enable();

    // enable ADC interface
    adc_enable();
    // ADC calibration and reset calibration
    adc_calibration_enable();

    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_SWRCST);
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
  }

  void vSetPwm(i16 li16Value)
  {
    if (li16Value > 0)
    {
      // >0: im Uhrzeigersinn drehen
      gpio_bit_set(GPIOA, GPIO_PIN_3);
    }
    else // if (li16Value < 0)
    {
      // <0: im Gegen-Uhrzeigersinn drehen
      gpio_bit_reset(GPIOA, GPIO_PIN_3);
    }

    if (li16Value > 100) li16Value = 100;
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, li16Value);
  }

  u16 u6GetSupplyVoltage()
  {
    u32 lu32Zwerg;
    lu32Zwerg = ((u32)mAdcResult[nIdx_bat] * 14520)/4096;
    return (u16)lu32Zwerg;
  }

  void vSetMotEnable()     {mStatus.mMotEnable = 1;}
  void vSetMotDisable()    {mStatus.mMotEnable = 0;}

  void vSetMotAuto_Enable()  { mStatus.mAutomatik = 1; }
  void vSetMotAuto_Disable() { mStatus.mAutomatik = 0; }

  void vSetMotMode_Servo() { mStatus.mMode = 0; }
  void vSetMotMode_Motor() { mStatus.mMode = 1; }

  void vSetSpeed(i16 li16Value)  { mSpeed_Soll = li16Value; }

  i16 i16GetPosDigit()              { return (i16)mAdcResult[nIdx_poti];}
  void vSetPosDigit(i16 li16Value)  { mPos_Soll = li16Value; }
  // ca. 30 digit pro grad
  void vSetPosDegree(i16 li16Value)  { mPos_Soll = (li16Value * nDegree2Digit) + 2048; }


  void vTick1ms()
  {
    if (u6GetSupplyVoltage() < 6000)
    {
      mStatus.mMotEnable = 0;
    }

    if (mStatus.mMotEnable)
    {
      // Enable motor driver
      gpio_bit_set(GPIOA, GPIO_PIN_4);

      if (mStatus.mMode) // Motor-Modus: Geschwindigkeit-Regelung
      {

      }
      else // Servo-Modus: Positions-Regelung
      {
        i32 li32Zwerg;
        li32Zwerg = mcMyPid.i32DoProcess(mPos_Soll - i16GetPosDigit());

        mSpeed_Soll = (i16)li32Zwerg;
      }

      if (mStatus.mAutomatik)
      {
        if (mSpeed_Ist < mSpeed_Soll)
        {
          if ((mSpeed_Ist + nSpeedInc) < mSpeed_Soll)
          {
            mSpeed_Ist += nSpeedInc;
          }
          else
          {
            mSpeed_Ist = mSpeed_Soll;
          }
        }
        else if (mSpeed_Ist > mSpeed_Soll)
        {
          if ((mSpeed_Ist - nSpeedInc) > mSpeed_Soll)
          {
            mSpeed_Ist -= nSpeedInc;
          }
          else
          {
            mSpeed_Ist = mSpeed_Soll;
          }
        }

        vSetPwm(mSpeed_Ist);
      }
    }
    else
    {
      vSetPwm(0);
      // Disable motor driver
      gpio_bit_reset(GPIOA, GPIO_PIN_4);
    }
  }
};

#endif // __SERVO1_APPLIKATION_H__
