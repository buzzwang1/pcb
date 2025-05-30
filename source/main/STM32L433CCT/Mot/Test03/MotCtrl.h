#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_adc.h"

#include "math.h"
#include "cFixPti1814.h"

#include "tGPPin.h"
#include "LED.h"

#include "math_tool.h"
#include "ClockInfo.h"

#include "control_table.h"
#include "math_tool.h"
#include "binary_tool.h"

// CONTROL LOOP
#define LOOP_FREQUENCY_HZ 1000.0f //Hz

// MOTOR
#define MOTOR_PWM_BRAKE 99
#define MOTOR_PWM_COAST 0

// POSITION FILTER
#define ALPHA_POSITION        0.05f // (default:0.05) F = 20kHz ==> Fc (-3dB) = 170.0Hz
#define ALPHA_VOLTAGE        0.05f // (default:0.05) F = 20kHz ==> Fc (-3dB) = 170.0Hz
#define ALPHA_CURRENT_SENSE      0.05f // (default:0.05) F = 20kHz ==> Fc (-3dB) = 170.0Hz
#define ALPHA_CURRENT_SENSE_OFFSET   0.001f


#define ALPHA_VELOCITY      0.12f // (default:0.12) F = 1000Hz ==> Fc (-3dB) = 20Hz
#define ALPHA_CURRENT_SETPOINT   0.96f // (default:0.12) F = 1000Hz ==> Fc (-3dB) = 20Hz
#define ALPHA_PWM_SETPOINT    0.12f // (default:0.12) F = 1000Hz ==> Fc (-3dB) = 20.0Hz
#define VOLTAGE_CALIBRATION   1.08f


class cfLowPass
{
  public:
  float mValue;
  float mFactor;

  cfLowPass(float lfFactor)
  {
    mFactor = lfFactor;
    mValue  = 0;
  }

  float DoProcess(float lfNewValue)
  {
    mValue = ((mFactor * lfNewValue) + ((1.0f - mFactor) * mValue));
    return mValue;
  }
};


class cfPid
{
  public:
  float err_last_one;
  float err_integral;
  float derivative_filtered;

  cfLowPass mLpDerivative;
  cfLowPass mLpOutput;

  cfPid()
    : mLpDerivative(ALPHA_VELOCITY),
      mLpOutput(ALPHA_VELOCITY)
  {
    pid_reset();
  }

  void pid_reset()
  {
    err_last_one = 0.0f;
    err_integral = 0.0f;
    derivative_filtered = 0.0f;
  }

  float pid_process_antiwindup_clamp(
      float error,
      float kp,
      float ki,
      float kd,
      float output_limit
  )
  {
    // filter derivative
    derivative_filtered = mLpDerivative.DoProcess(error - err_last_one);
    // update derivative state
    err_last_one = error;

    // PID
    float const p_term = kp*error;
    float const i_term = err_integral;
    float const d_term = kd*derivative_filtered;
    // compute output before saturation
    float const v = p_term + i_term + d_term;
    // saturation
    float const u = cMathTool::fconstrain(v,-output_limit,output_limit);
    // output saturating
    bool saturating = (u!=v);
    // error and output same sign
    bool sign = (error*v >= 0);
    // zero
    bool clamp = saturating && sign;
    if(!clamp)
      err_integral = err_integral + ki*error;
    // output
    return mLpOutput.DoProcess(u);
  }
};


class cFpLowPass
{
  public:
  cFixPti1814 mValue;
  cFixPti1814 mFactor;

  cFpLowPass(i32 li32Factor)
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


class cFpPid
{
  public:
  cFixPti1814 Fperr;
  cFixPti1814 Fperr_last_one;

  cFixPti1814 Fperr_integral;
  cFixPti1814 Fpderivative_filtered;

  cFpLowPass mLpDerivative;
  cFpLowPass mLpOutput;

  cFpPid()
    : mLpDerivative((i16)(ALPHA_VELOCITY*16384.0f)),
      mLpOutput((i16)(ALPHA_VELOCITY*16384.0f))
  {
    pid_reset();
  }

  void pid_reset()
  {
  }

  float pid_process_antiwindup_clamp(
      float error,
      float kp,
      float ki,
      float kd,
      i32   output_limit
  ) // __attribute__((optimize("-O0")))
  {
    Fperr.vSetFrac((i32)(error * 16384.0f));

    // filter derivative37174
    Fpderivative_filtered = mLpDerivative.DoProcess(Fperr - Fperr_last_one);

    // update derivative state
    Fperr_last_one = Fperr;

    // PID
    cFixPti1814 p_term;    p_term.vSetFrac((i32)(kp * 16384.0f)); p_term *= Fperr;
    cFixPti1814 i_term;    i_term = Fperr_integral;
    cFixPti1814 d_term;    d_term.vSetFrac((i32)(kd * 16384.0f)); d_term *= Fpderivative_filtered;

    cFixPti1814 v = p_term + i_term + d_term;

    // saturation
    cFixPti1814 u;
    u = v;

    if (u.i32Get() >= output_limit)       u = cFixPti1814(output_limit);
    else if (u.i32Get() <= -output_limit) u = cFixPti1814(-output_limit);

    // output saturating
    bool saturating = (u.mFp.i32Fp !=v.mFp.i32Fp);
    // error and output same sign
    bool sign = (Fperr.i8Sign() *v.i8Sign() > 0);
    // zero
    bool clamp = saturating && sign;
    if(!clamp)
    {
      cFixPti1814 lFpZwerg;    lFpZwerg.vSetFrac((i32)(ki * 16384.0f));
      Fperr_integral += (lFpZwerg * Fperr);
    }

    // output
    return mLpOutput.DoProcess(u).i32Get();
  }
};


class cAdcControl
{
public:
  // https://github.com/STMicroelectronics/STM32CubeL4/tree/master/Projects/NUCLEO-L476RG/Examples_LL/ADC/ADC_SingleConversion_TriggerSW/Src
  // ADC
  typedef enum
  {
    // Events f�r allgemeine Verwenung
    nEvReset = 0,
    nEvStart,
    nEvStop,

    nEvDummy
  }MAIN_tenEvents;

  typedef enum
  {
    nSmIdle = 0,

    // States f�r ADC
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

    // ADC_EN wurdenur beim Debuggen gesetzt. Nicht w�hrend run
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
  i32 mi32ActualCurrent;

  u8  mu8Mode;

  cMotControl_Base(cGpPin* lcPinMotA, cGpPin* lcPinMotB, TIM_TypeDef *lstTim)
  {
    mcPinMotA = lcPinMotA;
    mcPinMotB = lcPinMotB;
    mstTim    = lstTim;

    vSetPwm(100, 100);

    mi16ActualPos   = 1400;
    mi16SetPos      = 1400;
    mu8Mode         = 0;
  }

  void vTick1ms()
  {
  }

  void vSetPwm(u16 lu16PwmA, u16 lu16PwmB)
  {
    if (lu16PwmA > 100) lu16PwmA = 100;
    if (lu16PwmB > 100) lu16PwmB = 100;

    mu16PwmA = lu16PwmA;
    mu16PwmB = lu16PwmB;
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
      vSetPwm(100, 100);
    }
  }

  i16 i16GetPosGrad()
  {
    return mi16ActualPos;
  }
};


class cMotTest : public cMotControl_Base
{
  public:

  // --- Pins
  cGpPin mcPinMotA;
  cGpPin mcPinMotB;

  // raw sensor inputs
  volatile uint16_t ADC_DMA[3] = { 0,0,0 };
  // filtered sensor inputs
  float motor_current_input_adc = 0.0f;
  float position_input_adc = 0.0f;
  float voltage_input_adc = 0.0f;
  float motor_current_input_adc_offset = 0.0f;
  float pwm_sign = 0.0f;
  float pwm_ratio = 0.0f;
  // scaled sensor inputs
  float present_motor_current_ma = 0.0f;
  float present_position_deg = 0.0f;
  float present_voltage_0v1 = 0.0f;
  // setpoints
  float setpoint_pwm = 0.0f;
  float setpoint_current_ma = 0.0f;
  float setpoint_acceleration_dpss = 0.0f;
  float setpoint_velocity_dps = 0.0f;
  float setpoint_position_deg = 0.0f;
  // control loop state
  bool entering_state = true;
  uint32_t current_control_mode = REG_CONTROL_MODE_POSITION;
  uint16_t const period_us = (uint16_t)(1000000.0f/LOOP_FREQUENCY_HZ);
  uint32_t counter = 0;

  // PIDs
  cFpPid pid_position;
  cFpPid pid_current;

  // variables
  float present_velocity_dps = 0.0f;
  float last_present_position_deg = 0.0f;
  float last_setpoint_velocity_dps = 0.0f;

  cControlTable* mcControlTable;


  cMotTest(TIM_TypeDef *lstTim, cControlTable* lcControlTable)
    : cMotControl_Base(&mcPinMotA, &mcPinMotB, lstTim),
      mcPinMotA(GPIOB_BASE, 13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcPinMotB(GPIOB_BASE, 14, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    mcPinMotA.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);
    mcPinMotB.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);

    vSetPwm(100,100);

    TIM_vInit();

    APP_Control_Init();

    mcControlTable = lcControlTable;
  }

  // called once after SW REBBOT or HW RESET, and every time entering a new control loop mode
  void APP_Control_Reset()
  {
    // reset
    entering_state = true;
    counter = 0;
    pid_position.pid_reset();
    pid_current.pid_reset();
    setpoint_pwm = 0.0f;
    setpoint_current_ma = 0.0f;
    setpoint_acceleration_dpss = 0.0f;
    setpoint_velocity_dps = 0.0f;
    last_setpoint_velocity_dps = 0.0f;
    setpoint_position_deg = present_position_deg;
    last_present_position_deg = present_position_deg;
    // when re-entering in the control mode 'position', avoid glitch from past goal position
    mcControlTable->regs[REG_GOAL_POSITION_DEG_L] = LOW_BYTE((int16_t)(present_position_deg*10.0f));
    mcControlTable->regs[REG_GOAL_POSITION_DEG_H] = HIGH_BYTE((int16_t)(present_position_deg*10.0f));
  }

  void scale_all_sensors()
  {
    // scale motor current sense (unit:mA) and estimated average motor current with sign (using PWM ratio and setpoint PWM sign)
    //float const a = (float)(MAKE_SHORT(mcControlTable->regs[REG_CAL_CURRENT_SENSE_A_L],mcControlTable->regs[REG_CAL_CURRENT_SENSE_A_H]));
    //present_motor_current_ma = (motor_current_input_adc-motor_current_input_adc_offset)/a*1000.0f*pwm_sign*pwm_ratio;
    present_motor_current_ma = motor_current_input_adc;

    // scale position (unit:degrees)
    //float const min_position_adc = (float)(MAKE_SHORT(mcControlTable->regs[REG_MIN_POSITION_ADC_L],mcControlTable->regs[REG_MIN_POSITION_ADC_H]));
    //float const max_position_adc = (float)(MAKE_SHORT(mcControlTable->regs[REG_MAX_POSITION_ADC_L],mcControlTable->regs[REG_MAX_POSITION_ADC_H]));
    //float const max_rotation_deg = (float)(mcControlTable->regs[REG_MAX_ROTATION_DEG]);

    //present_position_deg = cMathTool::fmap(position_input_adc, min_position_adc, max_position_adc, 0.0f, max_rotation_deg);
    present_position_deg = (position_input_adc /*- 2048*/) / 14.83f;


    // potentiometer leads maybe inverted, user can reverse polarity of potentiometer (EEPROM parameter)
    if (mcControlTable->regs[REG_INV_ROTATION_SENSOR] > 0)
      present_position_deg = -present_position_deg;//max_rotation_deg-present_position_deg;

    // scale voltage (unit:0.1V)
    //present_voltage_0v1 = voltage_input_adc/16384.0f*3.3f*24.2f/2.2f*10.0f*VOLTAGE_CALIBRATION;
    present_voltage_0v1 = voltage_input_adc;
  }

  // called once after SW REBOOT or HW RESET
  void APP_Control_Init()
  {
    // reset (EWMA) filtered sensor inputs
    motor_current_input_adc = 0.0f;
    position_input_adc = 0.0f; // NOTE : init by zero will delay the present position estimation by 1 ms at least
    voltage_input_adc = 0.0f; // NOTE : init by zero will delay the present voltage estimation by 1 ms at least
    motor_current_input_adc_offset = (float)(MAKE_SHORT(mcControlTable->regs[REG_CAL_CURRENT_SENSE_B_L],mcControlTable->regs[REG_CAL_CURRENT_SENSE_B_H]));

    // reset all state control loop variables
    APP_Control_Reset();
  }

  // called from main loop
  void APP_Control_Process()
  {
    // scale sensor at process rate
    scale_all_sensors();

    // compute velocity from position (derivative), and filter velocity (EWMA)
    float present_speed_dps_unfiltered = (present_position_deg - last_present_position_deg)*LOOP_FREQUENCY_HZ;
    last_present_position_deg =  present_position_deg;
    present_velocity_dps = ALPHA_VELOCITY * present_speed_dps_unfiltered + (1.0f-ALPHA_VELOCITY)*present_velocity_dps;

    // torque enable logic
    bool torque_enable = (mcControlTable->regs[REG_TORQUE_ENABLE]!=0) && (mcControlTable->regs[REG_HARDWARE_ERROR_STATUS]==0);
    if(torque_enable)
    {
      // Simple PID from position to pwm
      // Mg92 Kp = 400 Ki = 20 Kd = 400
      if(entering_state)
      {
        entering_state = false;
        // init goal RAM registers according this control mode
        mcControlTable->regs[REG_GOAL_POSITION_DEG_L] = LOW_BYTE((int16_t)(present_position_deg*10.0f));
        mcControlTable->regs[REG_GOAL_POSITION_DEG_H] = HIGH_BYTE((int16_t)(present_position_deg*10.0f));
        // init limit RAM registers according this control mode
        mcControlTable->regs[REG_GOAL_PWM_100_L] = mcControlTable->regs[REG_MAX_PWM_100_L];
        mcControlTable->regs[REG_GOAL_PWM_100_H] = mcControlTable->regs[REG_MAX_PWM_100_H];
        // reset others
        mcControlTable->regs[REG_GOAL_VELOCITY_DPS_L] = 0;
        mcControlTable->regs[REG_GOAL_VELOCITY_DPS_H] = 0;
        mcControlTable->regs[REG_GOAL_CURRENT_MA_L] = 0;
        mcControlTable->regs[REG_GOAL_CURRENT_MA_H] = 0;
        // set setpoint position at current position
        setpoint_position_deg = present_position_deg;
      }
      {
        // limit goal position
        float const min_goal_position_deg = (float)(MAKE_SHORT(mcControlTable->regs[REG_MIN_POSITION_DEG_L],mcControlTable->regs[REG_MIN_POSITION_DEG_H]));
        float const max_goal_position_deg = (float)(MAKE_SHORT(mcControlTable->regs[REG_MAX_POSITION_DEG_L],mcControlTable->regs[REG_MAX_POSITION_DEG_H]));
        float goal_position_deg           = 0.1f * (float)(MAKE_SHORT(mcControlTable->regs[REG_GOAL_POSITION_DEG_L],mcControlTable->regs[REG_GOAL_POSITION_DEG_H]));
        goal_position_deg = cMathTool::fconstrain(goal_position_deg,min_goal_position_deg,max_goal_position_deg);
        // compute position setpoint from goal position
        // there is no profil, so position setpoint is goal position
        setpoint_position_deg = goal_position_deg;
        // compute position error
        float const error_position = setpoint_position_deg - present_position_deg;
        // compute pwm setpoint from position error using a PID position
        float const pid_pos_kp = (float)(MAKE_SHORT(mcControlTable->regs[REG_PID_POSITION_KP_L], mcControlTable->regs[REG_PID_POSITION_KP_H]))/4.0f;
        float const pid_pos_ki = (float)(MAKE_SHORT(mcControlTable->regs[REG_PID_POSITION_KI_L], mcControlTable->regs[REG_PID_POSITION_KI_H]))/512.0f;
        float const pid_pos_kd = (float)(MAKE_SHORT(mcControlTable->regs[REG_PID_POSITION_KD_L], mcControlTable->regs[REG_PID_POSITION_KD_H]))/4.0f;
        float const pwm_limit  = (float)(MAKE_SHORT(mcControlTable->regs[REG_GOAL_PWM_100_L],    mcControlTable->regs[REG_GOAL_PWM_100_H]));
        setpoint_pwm = pid_position.pid_process_antiwindup_clamp(
                          error_position,
                          pid_pos_kp,
                          pid_pos_ki,
                          pid_pos_kd,
                          pwm_limit
                      );
      }
      // unused setpoints zero
      setpoint_velocity_dps = 0.0f;
      setpoint_current_ma   = 0.0f;
      // mode change
      if(mcControlTable->regs[REG_CONTROL_MODE]!=REG_CONTROL_MODE_POSITION)
      {
        APP_Control_Reset();
        current_control_mode = mcControlTable->regs[REG_CONTROL_MODE];
      }

      // motor leads maybe inverted, user can reverse polarity of motor (EEPROM parameter)
      float const pwm_inv = mcControlTable->regs[REG_INV_ROTATION_MOTOR] > 0 ? -1.0f : 1.0f;
      float pwm = pwm_inv * setpoint_pwm;

      vSetPwm((i16)pwm);
      vSetMode(3);
    }
    else
    {
      APP_Control_Reset();
      // motor brake
      //__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,MOTOR_PWM_BRAKE);
      //__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,MOTOR_PWM_BRAKE);
      mstTim->CCR1 = 0;
      mstTim->CCR2 = 0;
    }

    // Note : This is an unipolar current sensing architecture,
    // then motor current is always positive in FORWARD and REVERSE drive phase,
    // and zero in BRAKE phases. So, the sign of the current, is build from the sign of the PWM setpoint
    pwm_sign = ( setpoint_pwm < 0.0f ) ? -1.0f : 1.0f;
    pwm_ratio = fabsf(setpoint_pwm)/100.0f;

    // live update of RAM mcControlTable->regs
    mcControlTable->regs[REG_PRESENT_POSITION_DEG_L] = LOW_BYTE((uint16_t)(present_position_deg*10.0f));
    mcControlTable->regs[REG_PRESENT_POSITION_DEG_H] = HIGH_BYTE((uint16_t)(present_position_deg*10.0f));

    mcControlTable->regs[REG_PRESENT_VELOCITY_DPS_L] = LOW_BYTE((int16_t)present_velocity_dps);
    mcControlTable->regs[REG_PRESENT_VELOCITY_DPS_H] = HIGH_BYTE((int16_t)present_velocity_dps);

    mcControlTable->regs[REG_PRESENT_CURRENT_MA_L] = LOW_BYTE((int16_t)present_motor_current_ma);
    mcControlTable->regs[REG_PRESENT_CURRENT_MA_H] = HIGH_BYTE((int16_t)present_motor_current_ma);

    mcControlTable->regs[REG_PRESENT_VOLTAGE] = (uint8_t)(present_voltage_0v1);
    mcControlTable->regs[REG_PRESENT_TEMPERATURE] = 0;

    float moving_threshold = mcControlTable->regs[REG_MOVING_THRESHOLD_DPS];
    mcControlTable->regs[REG_MOVING] = ( fabsf(present_velocity_dps) > moving_threshold ) ? 1 : 0;

    mcControlTable->regs[REG_SETPOINT_POSITION_DEG_L] = LOW_BYTE((uint16_t)(setpoint_position_deg*10.0f));
    mcControlTable->regs[REG_SETPOINT_POSITION_DEG_H] = HIGH_BYTE((uint16_t)(setpoint_position_deg*10.0f));

    mcControlTable->regs[REG_SETPOINT_VELOCITY_DPS_L] = LOW_BYTE((int16_t)setpoint_velocity_dps);
    mcControlTable->regs[REG_SETPOINT_VELOCITY_DPS_H] = HIGH_BYTE((int16_t)setpoint_velocity_dps);
    //mcControlTable->regs[REG_SETPOINT_VELOCITY_DPS_L] = LOW_BYTE((int16_t)pid_position.err_integral); // DEBUG
    //mcControlTable->regs[REG_SETPOINT_VELOCITY_DPS_H] = HIGH_BYTE((int16_t)pid_position.err_integral); // DEBUG

    mcControlTable->regs[REG_SETPOINT_CURRENT_MA_L] = LOW_BYTE((int16_t)setpoint_current_ma);
    mcControlTable->regs[REG_SETPOINT_CURRENT_MA_H] = HIGH_BYTE((int16_t)setpoint_current_ma);

    mcControlTable->regs[REG_SETPOINT_PWM_100_L] = LOW_BYTE((int16_t)setpoint_pwm);
    mcControlTable->regs[REG_SETPOINT_PWM_100_H] = HIGH_BYTE((int16_t)setpoint_pwm);

    mcControlTable->regs[REG_MOTOR_CURRENT_INPUT_ADC_L] = LOW_BYTE((uint16_t)motor_current_input_adc);
    mcControlTable->regs[REG_MOTOR_CURRENT_INPUT_ADC_H] = HIGH_BYTE((uint16_t)motor_current_input_adc);

    mcControlTable->regs[REG_MOTOR_CURRENT_INPUT_ADC_OFFSET_L] = LOW_BYTE((uint16_t)motor_current_input_adc_offset);
    mcControlTable->regs[REG_MOTOR_CURRENT_INPUT_ADC_OFFSET_H] = HIGH_BYTE((uint16_t)motor_current_input_adc_offset);

    mcControlTable->regs[REG_POSITION_INPUT_ADC_L] = LOW_BYTE((uint16_t)position_input_adc);
    mcControlTable->regs[REG_POSITION_INPUT_ADC_H] = HIGH_BYTE((uint16_t)position_input_adc);

    mcControlTable->regs[REG_VOLTAGE_INPUT_ADC_L] = LOW_BYTE((uint16_t)voltage_input_adc);
    mcControlTable->regs[REG_VOLTAGE_INPUT_ADC_H] = HIGH_BYTE((uint16_t)voltage_input_adc);

    // steps
    ++counter;
  }


  void HAL_ADC_ConvCpltCallback(i32 li32Pos, i32 li32Voltage, i32 li32Current)
  {
    //if(hadc==&hadc1)
    {
      // Filter (EWMA) position and voltage ADC samples
      //voltage_input_adc = ALPHA_VOLTAGE * (float)(ADC_DMA[2]) + (1.0f-ALPHA_VOLTAGE) * voltage_input_adc;
      //position_input_adc = ALPHA_POSITION * (float)(ADC_DMA[1]) + (1.0f-ALPHA_POSITION) * position_input_adc;
      voltage_input_adc  = 0.2f * (float)(li32Voltage) + (1.0f-0.2f) * voltage_input_adc;
      position_input_adc = 0.2f * (float)(li32Pos)     + (1.0f-0.2f) * position_input_adc;

      // Filter (EWMA) motor current sense ADC samples

      // Note : In center aligned mode, two periods of TIM4 are used for motor PWM generation
      // TIM4 is 40KHz, motor PWM is 20KHz
      // So ADC is triggered twice per motor PWM period by TIM4
      // We will measure ON and OFF instant motor current

      // In FORWARD or REVERSE DRIVE phases, PWM is ON, counter decreases
      /*
      if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4))
      {
        // filter motor current
        motor_current_input_adc = ALPHA_CURRENT_SENSE*(float)(ADC_DMA[0]) + (1.0f-ALPHA_CURRENT_SENSE)*motor_current_input_adc;
      }
      // In BRAKE phase, PWM is OFF, counter increases
      else
      {
        // self-calibrate ADC offset (b) when motor is stopped
        if(setpoint_pwm==0.0f)
        {
          motor_current_input_adc_offset = ALPHA_CURRENT_SENSE_OFFSET*(float)(ADC_DMA[0]) + (1.0f-ALPHA_CURRENT_SENSE_OFFSET)*motor_current_input_adc_offset;
        }
      }
      */
      motor_current_input_adc = 0.2f*(float)(li32Current) + (1.0f-0.2f)*motor_current_input_adc;

    }
  }

  void vPID()
  {
    //vSetPwm((i16)(mfy));
    //vSetMode(3);
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
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 2000000) -1; // 0,5 us = 2Mhz
    TimHandle.Init.Period            = 100 - 1; // 2Mhz/100 = 20khz
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

  void vSetActualCurrent(i32 li32Current)
  {
     mi32ActualCurrent = li32Current;
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
    APP_Control_Process();
  }

};


#endif  //__MOTOR_CONTROL_H__
