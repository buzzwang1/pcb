#ifndef __SERVO1_APPLIKATION_H__
#define __SERVO1_APPLIKATION_H__

#include "Typedef.h"
#include "cCli.h"
#include "cBnStreamSysPortBase.h"
#include "cBnSpop.h"

#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_adc.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_tim.h"
#include "ch32v30x_misc.h"
#include "cFixPti1814.h"
#include "tgppin.h"


class cServo1_Applikation
{
public:
  enum
  {
    nV_imot   =  ADC_Channel_4,
    nV_ntc    =  ADC_Channel_5,
    nV_poti   =  ADC_Channel_12,
    nV_temp   =  ADC_Channel_TempSensor,


    nIdx_imot1  =  0,
    nIdx_imot2  =  1,
    nIdx_imot3  =  2,
    nIdx_poti   =  3,
    nIdx_ntc    =  4,

    nChnCount = 5,
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // configure PA0 (adc channel10) as analog input
    cGpCfg::vSet(GPIOA, 4, cGpCfg::nAnalog, cGpCfg::nInput); // Mot Strom
    cGpCfg::vSet(GPIOA, 5, cGpCfg::nAnalog, cGpCfg::nInput); // Ntc
    cGpCfg::vSet(GPIOC, 2, cGpCfg::nAnalog, cGpCfg::nInput); // Poti

    // Test Trigger ADC
    cGpCfg::vSet(GPIOA, 8, cGpCfg::nAfPp, cGpCfg::nOutput_10MHz);

    // Motor
    // Output
    cGpCfg::vSet(GPIOA, 6, cGpCfg::nAfPp, cGpCfg::nOutput_10MHz); // IN1, TIM3_CH1(AF2)
    cGpCfg::vSet(GPIOA, 7, cGpCfg::nAfPp, cGpCfg::nOutput_10MHz); // IN2, TIM3_CH2(AF2)


    // Poti versorgung: PB6: Poti 3v3: PB7: Poti GND
    cGpCfg::vSet(GPIOB, 6, cGpCfg::nPp, cGpCfg::nOutput_2MHz);
    cGpCfg::vSet(GPIOB, 7, cGpCfg::nPp, cGpCfg::nOutput_2MHz);
    GPIOB->BSHR = (1 << 6); // Versorgung an

    //Sleep
    cGpCfg::vSet(GPIOB, 14, cGpCfg::nPp, cGpCfg::nOutput_2MHz); // 1 = enable H-Bridge, 0 = disable H-Bridge


    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    /*GPIO_CTL(GPIOA) = 0x2802C57F;
    GPIO_OSPD0(GPIOA)  = 0x0C030000;
    GPIO_PUD(GPIOA)    = 0x24000000;
    GPIO_OCTL(GPIOA)   = 0x00000020; // Brake anschalten
    GPIO_AFSEL1(GPIOA) = 0x00000002;*/


    // ------------ Timer0_Config
    // TIMER3 configuration: generate PWM signals with different duty cycles
    // PA06: Mot IN1:        TIM3_CH1(AF2)
    // PA07: Mot IN2:        TIM3_CH2(AF2)
    // TIMER1 configuration: generate PWM signals to trigger ADC


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    // TIMER3 configuration
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 3;  // 0.5us   (@8Mhz/4 = 2Mhz = 0.5us)
    TIM_TimeBaseInitStructure.TIM_Period        = 99; // 0.5us * 100 = 50us = 20khz
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // CH1 configuration in PWM mode for Motor Driver
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure); //TIM3.CH1 => Mot.In1
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); //TIM3.CH2 => Mot.In2
    TIM_OC1Init(TIM1, &TIM_OCInitStructure); //TIM1.CH1 => ADC1 Trigger

    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM1->CH1CVR = (u32)1;
    TIM_Cmd(TIM1, ENABLE);

    // Das Gleiche wie oben 2Mhz-PWM-Timer mit 100PWM => 20khz = 50us , Autoreload, @8Mhz
    // Braucht aber so ein paar Byte weniger Code
    /*
    TIMER_CHCTL0(TIMER0) = 0x6060; // PWM CH0 und CH1
    TIMER_CHCTL2(TIMER0) = 0x11;   // Ch0 und Ch1 enable
    TIMER_PSC(TIMER0)    = 3;      // 2 Mhz
    TIMER_CAR(TIMER0)    = 0x63;   // 100
    TIMER_CH1CV(TIMER0)  = 1;
    TIMER_CCHP(TIMER0)   = 0x8000; // 100
    TIMER_CTL0(TIMER0)   = 0x081;*/


    // dma_ch0 config used for ADC
    // enable the DMA clock
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(mAdcResult);
    DMA_InitStructure.DMA_DIR            = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize     = nChnCount;
    DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc      = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode           = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority       = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M            = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    /*DMA_CH0CNT = nChnCount;
    DMA_CH0PADDR = (u32)&ADC_RDATA;
    DMA_CH0MADDR = (u32)&mAdcResult;
    DMA_CH0CTL = 0x000015A1;*/


    // ------------ ADC_Config
    // enable the DMA clock
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div2); // => 4Mhz


    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = nChnCount; // 3 x imot, poti, ntc
    ADC_InitStructure.ADC_OutputBuffer       = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga                = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStructure);

    //adc_vbat_enable();
    // Temp sensor = ADC_IN16. The sampling time for the temperature sensor is recommended to be set to at least 17.1μs
    // When TKENABLE bit or TSVREFE position 1, buffer is on by default
    // At this time, the ADC sampling time is not recommended to be less than 7.5T
    ////ADC_TempSensorVrefintCmd(ENABLE);

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
    ADC_RegularChannelConfig(ADC1, nV_imot, nIdx_imot1 + 1, ADC_SampleTime_7Cycles5);  //  5,0us
    ADC_RegularChannelConfig(ADC1, nV_imot, nIdx_imot2 + 1, ADC_SampleTime_7Cycles5);  //  5,0us
    ADC_RegularChannelConfig(ADC1, nV_imot, nIdx_imot3 + 1, ADC_SampleTime_7Cycles5);  //  5,0us
    ADC_RegularChannelConfig(ADC1, nV_poti, nIdx_poti  + 1, ADC_SampleTime_7Cycles5);  //  5,0us
    ADC_RegularChannelConfig(ADC1, nV_ntc,  nIdx_ntc   + 1, ADC_SampleTime_7Cycles5);  //  5,0us
    //                                                                                  -------- 
    //                                                                                    25,0us

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    u16 Calibrattion_Val = Get_CalibrationValue(ADC1);
    UNUSED(Calibrattion_Val);

    // Das Gleiche wie oben
    // Braucht aber so ein paar Byte weniger Code
    /*ADC_SAMPT0 = 0x00180000;
    ADC_SAMPT1 = 0x00000000;
    ADC_RSQ0   = 0x00600000;
    ADC_RSQ1   = 0x00000010;
    ADC_RSQ2   = 0x00138842;
    ADC_CTL0   = 0x00000100;
    ADC_CTL1   = 0x00800101;*/
  }

  void vSetPwm(i16 li16Value)
  {
    if (li16Value >  100) li16Value =  100;
    if (li16Value < -100) li16Value = -100;

    if (li16Value > 0)
    {
      // >0: im Uhrzeigersinn drehen
      TIM3->CH1CVR = (uint32_t)100;
      TIM3->CH2CVR = (uint32_t)100 - li16Value;
    }
    else // if (li16Value < 0)
    {
      // <0: im Gegen-Uhrzeigersinn drehen
      TIM3->CH1CVR = (uint32_t)100 - (-li16Value); // ENABLE
      TIM3->CH2CVR = (uint32_t)100; // ENABLE
    }

    if (li16Value == 0)
    {
      // Set Motor sleep
      GPIOB->BCR = (1 << 14); // SLEEP
      TIM3->CH1CVR = (u32)0;
      TIM3->CH2CVR = (u32)0;
    }
    else
    {
      // wakeup Motor driver
      GPIOB->BSHR = (1 << 14); //SLEEP
    }
  }

  u8 u8GetLedEnable() { return u8GetRomConstLedEnable(); }

  u16 u16GetSupplyVoltage()
  {
    //u32 lu32Zwerg;
    //lu32Zwerg = ((u32)mAdcResult[nIdx_bat] * 14520)/4096;
    return (u16)6000;
  }

  i16 i16GetCurrent()
  {
    return (u16)mcInputFp.mFpI.mFp.stFp.Int;
  }

  i16 i16ReadCurrent()
  {
    u32 lu32Zwerg;

    // Falls Motor deaktiviert ist
    if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)) // Sleep ?
    {
      return 0;
    }

    // Den größeren Wert von imot2 und imot3 nehmen
    // imot1 wird ignoriert
    lu32Zwerg = mAdcResult[nIdx_imot2];
    if (lu32Zwerg < mAdcResult[nIdx_imot3]) lu32Zwerg = mAdcResult[nIdx_imot3];

    // Strom von Digit in mA umrechnen.
    // 100uA/1A => 10000
    // 0,5V * 10000 /  5000 = 1A
    // 0,5V * 10000 / 20000 = 250mA
    // 0,5V * 10000 / 22000 = 227mA
    //
    // Bei 22k
    // 0,5V entspricht 227mA => 3,3V wären 1498mA
    //   => 1498mA/4096= 0,3657mA/digit => * 16384 => 5993
    // Strombegrenzung geht bei 0,5V rein => 227mA
    //

    // Offset abziehen, gemesserner offset is ca. 65digit
    if (lu32Zwerg > 65) lu32Zwerg -= 65;
                   else lu32Zwerg  = 0;

    lu32Zwerg = (lu32Zwerg * 16384)/ 5993;

    // Mit PWM-Verhältnis multiplizieren
    // Kann übersprungen werden, weil Glättungskondensator groß genug ist.
    //lu32Zwerg = (lu32Zwerg * (100-TIMER_CH0CV(TIMER1)))/100;


    if (mSpeed_Soll < 0)  lu32Zwerg = -lu32Zwerg;
    return (i16)lu32Zwerg;
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
  i16 i16GetPosDegree()        { return (i16)(((i32)((i32)(2048 - mAdcResult[nIdx_poti]) * (i32)330)) / 4096);}

  cFixPti1814 Fp1814GetPosDegree()
  {
    cFixPti1814 lFpZwerg;
    lFpZwerg.mFp.i32Fp = (((i32)(2048 - mAdcResult[nIdx_poti]) * (i32)330 * (i32)(1 << (14-12))));
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
      GPIO_SetBits(GPIOB, GPIO_Pin_14); // sleep

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
      GPIO_ResetBits(GPIOB, GPIO_Pin_14); // sleep
    }
  }
};

#endif // __SERVO1_APPLIKATION_H__
