

#ifndef _C_CNC_H
#define _C_CNC_H

#include "TYPEDEF.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_tim.h"
#include "stm32h7xx_ll_exti.h"

#include "cRingBufT.h"
#include "cStrT.h"
#include "cPal.h"
#include "math.h"
#include "ClockInfo.h"
#include "tGPPin.h"
#include "stdlib.h"


class cDrvCmd
{
public:
  typedef struct
  {
    u16 RefX : 1;
    u16 RefY : 1;
    u16 RefZ : 1;
    u16 DrvX : 1;
    u16 DrvY : 1;
    u16 DrvZ : 1;
  }tstDrvCmdBits;

  typedef union
  {
    u16           u16DrvCmd;
    tstDrvCmdBits Bit;
  }tunDrvCmd;

  tunDrvCmd munCmd;
  i32 mi32StepX;
  i32 mi32StepY;
  i32 mi32StepZ;
  i32 mi32Spd;

  cDrvCmd()
  {
    munCmd.u16DrvCmd = 0;

    mi32StepX = 0;
    mi32StepY = 0;
    mi32StepZ = 0;
    mi32Spd = 0;
  }

  cDrvCmd(u8 lu8RefX, u8 lu8RefY, u8 lu8RefZ,
          u8 lu8DrvX, u8 lu8DrvY, u8 lu8DrvZ,
          i32 li32StepX, i32 li32StepY, i32 li32StepZ,
          i32 li32Spd)
  {
    munCmd.Bit.RefX = lu8RefX;
    munCmd.Bit.RefY = lu8RefY;
    munCmd.Bit.RefZ = lu8RefZ;
    munCmd.Bit.DrvX = lu8DrvX;
    munCmd.Bit.DrvY = lu8DrvY;
    munCmd.Bit.DrvZ = lu8DrvZ;

    mi32StepX = li32StepX;
    mi32StepY = li32StepY;
    mi32StepZ = li32StepZ;
    mi32Spd   = li32Spd;
  }
};


class cCncAchse
{
public:
  i32 mi32PosIst;       // [Ticks]
  u32 mu32RefSpeed;     // [Hz]
  i8  mi8Dir;           // -1;0;1

  u8  mu8Ref;
  u8  mu8RefChanged;

  i32 mi32StepIst;       // [Ticks]
  i32 mi32StepIst_Last;  // [Ticks]
  float mdStep;        // Wenn dieser Wert > 1 wird, muss ein Schritt ausgelöst werden
  u8    mu8Step;       // =1: Es wurde ein Schritt ausgelöst
  float mdFactor;      // Umrechnungsfactor zur Referenzachse

  bool mbEnable;
  bool mbActive;

  bool bisDriving;

  cGpPin* mPinRef;
  cGpPin* mPinDir;
  cGpPin* mPinTakt;

  cCncAchse(cGpPin* lPinRef, cGpPin* lPinDir, cGpPin* lPinTakt)
  {
    mPinRef     = lPinRef;
    mPinDir     = lPinDir;
    mPinTakt    = lPinTakt;

    mu32RefSpeed   = 2000;
    mi32PosIst     = 0;             // [Ticks]
    mi8Dir         = 0;
    mu8Ref         = 0;
    mu8RefChanged  = 0;

    mbEnable       = True;
  }

  bool isRefSwitch()
  {
    u8 lu8Ref = !mPinRef->ui8Get();
    if (mu8Ref != lu8Ref)
    {
      mu8Ref = lu8Ref;
      mu8RefChanged = 1;
    }
    else
    {
      mu8RefChanged = 0;
    }
    return (mu8Ref > 0);
  };

  bool hasRefSwitchChanged()
  {

    return (mu8RefChanged == 1);
  }


  //  Der Richtungsausgang gibt je nach vorgegebener Drehrichtung des Schrittmotors
  //  ein + 5 V - Signal(Drehrichtung CCW) oder ein 0 V - Signal(Drehrichtung CW) aus.
  // positive Bewegungen finden in der Richtung vom Motor weg statt.
  // 0 = negative Drehrichtung des Schrittmotors CW; 1 = positive Drehrichtung des Schrittmotors CCW
  void vSetDir(u8 lu8Dir)
  {
    mPinDir->vSet(lu8Dir);
  };

  bool isDriving()
  {
    return bisDriving;
  }
};




class cCncControl
{
public:
  cCncAchse* X;
  cCncAchse* Y;
  cCncAchse* Z;

  TIM_TypeDef* mstTimDelay;
  TIM_TypeDef* mstTimStep;

  u32    mu32RefSpeed;     // [Hz]
  u32    mu32SpeedSoll;     // [Hz]

  i32    mi32PosSoll;      // [Ticks]

  u32    mu32TimeNoMove;       // [ms]
  u32    mu32TimeNoMoveReload; // [ms]
  cGpPin* mPinStromAbs;

  u32    mu32Acc;            // [Hz/ms]

  u32    mu32StepsSoll;              // [Ticks]
  u32    mu32StepsIst;               // [Ticks]
  u32    mu32StepsAcc;               // [Ticks]
  u32    mu32StepsRun;               // [Ticks]
  u32    mu32StepsDeAcc;             // [Ticks]


  u16    mu16TimeStep;

  cDrvCmd* mcDrvCmd;

  u16 mau16AccTable[2000];

  typedef enum
  {
    evRefNop = 0,
    evRefStart,
    evRefStepsFinish,
    evRefDrvFinish,
    evRefSwitch,
  }tenRefEvents;

  typedef enum
  {
    enSpeedAccel = 0,
    enSpeedKeep,
    enSpeedStop
  }tenSpeedMode;

  tenSpeedMode menSpeedMode;

  typedef enum
  {
    enDrvModeSteps = 0,
    enDrvModeSpeed
  }tenDriveMode;

  tenDriveMode menDriveMode;

  bool   mbRefEnable;
  bool   mbRefFinished;
  u32    mu32Ref1s;
  u32    mu32RefTimeout;
  u32    mu32RefTimeoutReload;

  typedef enum
  {
    enMove1sForward = 0,
    enMove1sForwardFinish,
    enMoveOnRefSwitchStart,
    enMoveOnRefSwitchRun,
    enMoveOnRefSwitchFinish,
    enMoveOffRefSwitchStart,
    enMoveOffRefSwitchRun,
    enMoveOffRefSwitchFinish,
  }tenRefMode;
  tenRefMode menRefMode;

  cCncControl(TIM_TypeDef* lstTimDelay, TIM_TypeDef* lstTimStep,
              cCncAchse* lcAX, cCncAchse* lcAY, cCncAchse* lcAZ,
              cGpPin* lPinStromAbs)

  {
    mi32PosSoll    = 0.0;      // [Ticks]

    mu32TimeNoMove   = mu32TimeNoMoveReload = 200; // 200ms

    mu16TimeStep   = 0;
    mu32Acc        = 100;

    mcDrvCmd       = NULL;

    mu32StepsSoll  = 0;             // [Ticks]
    mu32StepsIst   = 0;             // [Ticks]
    mu32StepsAcc   = 0;             // [Ticks]
    mu32StepsRun   = 0;             // [Ticks]
    mu32StepsDeAcc = 0;             // [Ticks]


    mbRefEnable    = False;
    mbRefFinished  = False;
    mu32Ref1s      = 1 * 1000; // 1s
    mu32RefTimeout = mu32RefTimeoutReload = 1 * 1000 * 60; // 1min

    mPinStromAbs = lPinStromAbs;

    TIM_HandleTypeDef lhTimX = {};

    X = lcAX;
    Y = lcAY;
    Z = lcAZ;

    mstTimDelay = lstTimDelay;
    mstTimStep  = lstTimStep;

    if (lstTimStep == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
    if (lstTimStep == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
    if (lstTimStep == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();

    if (mstTimDelay == TIM15) __HAL_RCC_TIM15_CLK_ENABLE();
    if (mstTimDelay == TIM16) __HAL_RCC_TIM16_CLK_ENABLE();
    if (mstTimDelay == TIM17) __HAL_RCC_TIM17_CLK_ENABLE();


    lhTimX.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) - 1;
    lhTimX.Init.CounterMode = TIM_COUNTERMODE_UP;
    lhTimX.Init.Period = 20 - 1; // 20us
    lhTimX.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    lhTimX.Init.RepetitionCounter = 0;
    lhTimX.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    lhTimX.Instance = lstTimStep;
    HAL_TIM_Base_Init(&lhTimX);
    lstTimStep->SR &= ~TIM_SR_UIF; // clear UIF flag


    if (lstTimStep == TIM12)
    {
      HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
    }

    if (lstTimStep == TIM13)
    {
      HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
    }

    if (lstTimStep == TIM14)
    {
      HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
    }

    LL_TIM_EnableIT_UPDATE(lstTimStep);
    LL_TIM_SetOnePulseMode(lstTimStep, LL_TIM_ONEPULSEMODE_SINGLE);


    // 20000Hz =>    50us
    //    30Hz => 33333us
    lhTimX.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) - 1;
    lhTimX.Init.CounterMode = TIM_COUNTERMODE_UP;
    lhTimX.Init.Period = 50000 - 1; // 50ms
    lhTimX.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    lhTimX.Init.RepetitionCounter = 0;
    lhTimX.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    lhTimX.Instance = mstTimDelay;
    HAL_TIM_Base_Init(&lhTimX);
    mstTimDelay->SR &= ~TIM_SR_UIF; // clear UIF flag

    if (mstTimDelay == TIM15)
    {
      HAL_NVIC_SetPriority(TIM15_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM15_IRQn);
    }

    if (mstTimDelay == TIM16)
    {
      HAL_NVIC_SetPriority(TIM16_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM16_IRQn);
    }

    if (mstTimDelay == TIM17)
    {
      HAL_NVIC_SetPriority(TIM17_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(TIM17_IRQn);
    }

    LL_TIM_EnableIT_UPDATE(mstTimDelay);
    LL_TIM_SetOnePulseMode(mstTimDelay, LL_TIM_ONEPULSEMODE_SINGLE);

    for (u16 lui16t = 0; lui16t < 2000; lui16t++)
    {
      mau16AccTable[lui16t] = u32TimeToNextTick_us(0, mu32Acc, lui16t);
    }
  }


  float dTimeToTickAbs_ms(float ldV0, float ldAcc, u32 lu32StepAbs)
    // ldV0      [Ticks/s]
    // ldAcc     [(Ticks/s)/ms] => 0.001 Ticks/ms² => 1000 Ticks/s²
    // lu32Step
    // return in [ms]
  {
    // (sqrt(v0²+2as)-v0)/a
    if (ldAcc != 0)
      return (sqrtf((ldV0 * ldV0) + 2 * lu32StepAbs * 0.001f * ldAcc) - ldV0) / (0.001f * ldAcc);
    else
      if (ldV0 != 0)
        return (lu32StepAbs / ldV0) * 1000;
      else return 0.0f;
  }

  u32 u32TimeToTickAbs_us(float ldV0, float ldAcc, u32 lu32StepAbs)
  {
    return (u32)(dTimeToTickAbs_ms(ldV0, ldAcc, lu32StepAbs) * 1000.0f);
  }

  u32 u32TimeToNextTick_us(float ldV0, float ldAcc, u32 lu32CurrentStep)
  {
    u32 lu32Time1 = u32TimeToTickAbs_us(ldV0, ldAcc, lu32CurrentStep);
    u32 lu32Time2 = u32TimeToTickAbs_us(ldV0, ldAcc, lu32CurrentStep + 1);
    return lu32Time2 - lu32Time1;
  }

  u32 i32TicksToSpeed(float ldVIst, float ldVsoll, float ldAcc)
    // ldVIst    [Ticks/s] = [0.001 Ticks/ms]
    // ldVIst    [Ticks/s]
    // ldAcc     [(Ticks/s)/ms] => 0.001 Ticks/ms² => 1000 Ticks/s²
    // return in [ms]

  {
    // vsoll = vist + a * t => (vsoll - vist) / a
    float ldTime_ms = fabsf(ldVsoll-ldVIst) / (ldAcc);

    // s = 0.5 * a*t^2 + vi * t
    //                       [Ticks/ms²]       [ms]         [ms]       [Ticks/ms]
    float ldTicks = 0.5f * (0.001f * ldAcc) * ldTime_ms * ldTime_ms + (ldVIst * 0.001f) * ldTime_ms;
    return (u32)(ldTicks);
  }


  void vStop()
  {
    vStopTimer();

    X->mPinTakt->vSet0();
    Y->mPinTakt->vSet0();
    Z->mPinTakt->vSet0();

    mbRefEnable   = False;
    mbRefFinished = False;

    mcDrvCmd = NULL;
  }

  void vBreak()
  {
    vStop();
  }


  bool isRefSwitch()
  {
    if (X->mbActive) if (X->isRefSwitch()) return True;
    if (Y->mbActive) if (Y->isRefSwitch()) return True;
    if (Z->mbActive) if (Z->isRefSwitch()) return True;
    return False;
  }

  void vStopTimer()
  {
    LL_TIM_DisableCounter(mstTimDelay);
    LL_TIM_DisableCounter(mstTimStep);
  }

  void vRefSm(tenRefEvents lenEvent)
  {
    bool lbLoop;
    do
    {
      lbLoop = False;
      switch (menRefMode)
      {
        case enMove1sForward: // Falls Achse schon auf dem Referenzschalter steht,
                              // dann direkt zum Runterfahren
                              // sonst erstmal drauffahren
          if (lenEvent == evRefStart)
          {
            if (!isRefSwitch())
            {
              isRefSwitch();
              menRefMode = enMoveOnRefSwitchStart;
              lbLoop = True;
            }
            else
            {
              isRefSwitch();
              menRefMode = enMoveOffRefSwitchStart;
              lbLoop = True;
            }
          }
          break;
        /*case enMove1sForward: // 1000 Ticks vorfahren, falls man auf dem Referenzschalter steht
          if (lenEvent == evRefStart)
          {
            if (!isRefSwitch())
            {
              menRefMode = enMoveOnRefSwitchStart;
              lbLoop = True;
            }
            else
            {
              if (X->mbActive) { X->mi8Dir = 1; X->vSetDir(0); };
              if (Y->mbActive) { Y->mi8Dir = 1; Y->vSetDir(0); };
              if (Z->mbActive) { Z->mi8Dir = 1; Z->vSetDir(0); };

              mu32StepsSoll = 10000;

              X->mdFactor = Y->mdFactor = Z->mdFactor = 1.0f;

              mu32StepsIst  = 1;
              mu32Acc       = 100;

              if (X->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};
              if (Y->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};
              if (Z->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};


              mu32StepsAcc = mu32StepsDeAcc = i32TicksToSpeed(0, mu32RefSpeed, mu32Acc) - 1;

              if ((mu32StepsAcc + mu32StepsDeAcc) > mu32StepsSoll)
              {
                mu32StepsAcc = mu32StepsDeAcc = mu32StepsSoll / 2;
              }

              mu32StepsRun = mu32StepsSoll - (mu32StepsAcc + mu32StepsDeAcc);

              X->mu8Step = Y->mu8Step = Z->mu8Step = 0;

              X->mi32StepIst_Last = 0;
              Y->mi32StepIst_Last = 0;
              Z->mi32StepIst_Last = 0;

              if (X->mbActive) { X->mi32StepIst = mu32StepsIst * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;  X->mPinTakt->vSet1(); X->mu8Step = 1; } }
              if (Y->mbActive) { Y->mi32StepIst = mu32StepsIst * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;  Y->mPinTakt->vSet1(); Y->mu8Step = 1; } }
              if (Z->mbActive) { Z->mi32StepIst = mu32StepsIst * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;  Z->mPinTakt->vSet1(); Z->mu8Step = 1; } }

              mu32TimeNoMove = mu32TimeNoMoveReload;
              mPinStromAbs->vSet1();

              LL_TIM_EnableCounter(mstTimStep);
              menDriveMode = enDrvModeSteps;
              vSetDelayCounter();

              menRefMode = enMove1sForwardFinish;
            }
          }
          break;*/
        case enMove1sForwardFinish:
          menRefMode = enMoveOnRefSwitchStart;
          lbLoop = True;
          break;

        case enMoveOnRefSwitchStart: // Auf Referenzschalter fahren
          if (X->mbActive) { X->mi8Dir = -1; X->vSetDir(1);};
          if (Y->mbActive) { Y->mi8Dir = -1; Y->vSetDir(1);};
          if (Z->mbActive) { Z->mi8Dir = -1; Z->vSetDir(1);};

          X->mdFactor = Y->mdFactor = Z->mdFactor = 1.0f;

          mu32StepsIst  = 1;
          mu32Acc       = 100;

          if (X->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};
          if (Y->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};
          if (Z->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f);};

          mu32StepsAcc = mu32StepsDeAcc = i32TicksToSpeed(0, mu32RefSpeed, mu32Acc) - 1;

          mu32StepsRun = 1000;

          X->mu8Step = Y->mu8Step = Z->mu8Step = 0;

          X->mi32StepIst_Last = 0;
          Y->mi32StepIst_Last = 0;
          Z->mi32StepIst_Last = 0;

          if (X->mbActive) { X->mi32StepIst = mu32StepsIst * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;  X->mPinTakt->vSet1(); X->mu8Step = 1; }}
          if (Y->mbActive) { Y->mi32StepIst = mu32StepsIst * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;  Y->mPinTakt->vSet1(); Y->mu8Step = 1; }}
          if (Z->mbActive) { Z->mi32StepIst = mu32StepsIst * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;  Z->mPinTakt->vSet1(); Z->mu8Step = 1; }}

          mu32TimeNoMove = mu32TimeNoMoveReload;
          mPinStromAbs->vSet1();

          mu32SpeedSoll = mu32RefSpeed;

          LL_TIM_EnableCounter(mstTimStep);
          menDriveMode = enDrvModeSpeed;
          vSetDelayCounter();

          menRefMode = enMoveOnRefSwitchRun;
          break;

        case enMoveOnRefSwitchRun: // Auf Referenzschalter fahren
          if (lenEvent == evRefSwitch)
          {
            mu32SpeedSoll = 0;
            menRefMode    = enMoveOnRefSwitchFinish;
          }
          break;

        case enMoveOnRefSwitchFinish: // Runter-Bremsen
          if (lenEvent == evRefDrvFinish)
          {
            menRefMode = enMoveOffRefSwitchStart;
            lbLoop     = True;
          }
          break;

        case enMoveOffRefSwitchStart: // Von Referenzschalter runterfahren
          {
            if (X->mbActive) { X->mi8Dir = 1; X->vSetDir(0); };
            if (Y->mbActive) { Y->mi8Dir = 1; Y->vSetDir(0); };
            if (Z->mbActive) { Z->mi8Dir = 1; Z->vSetDir(0); };

            X->mdFactor = Y->mdFactor = Z->mdFactor = 1.0f;

            mu32StepsIst = 1;
            mu32Acc = 100;

            if (X->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f); };
            if (Y->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f); };
            if (Z->mbActive) { mu16TimeStep = ((1.0f / (float)mu32RefSpeed) * 1000000.0f); };

            mu32StepsAcc = mu32StepsDeAcc = i32TicksToSpeed(0, mu32RefSpeed, mu32Acc) - 1;

            mu32StepsRun = 1000;

            X->mu8Step = Y->mu8Step = Z->mu8Step = 0;

            X->mi32StepIst_Last = 0;
            Y->mi32StepIst_Last = 0;
            Z->mi32StepIst_Last = 0;

            if (X->mbActive) { X->mi32StepIst = mu32StepsIst * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;  X->mPinTakt->vSet1(); X->mu8Step = 1; } }
            if (Y->mbActive) { Y->mi32StepIst = mu32StepsIst * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;  Y->mPinTakt->vSet1(); Y->mu8Step = 1; } }
            if (Z->mbActive) { Z->mi32StepIst = mu32StepsIst * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;  Z->mPinTakt->vSet1(); Z->mu8Step = 1; } }

            mu32TimeNoMove = mu32TimeNoMoveReload;
            mPinStromAbs->vSet1();

            mu32SpeedSoll = mu32RefSpeed;

            LL_TIM_EnableCounter(mstTimStep);
            menDriveMode = enDrvModeSpeed;
            vSetDelayCounter();

            menRefMode = enMoveOffRefSwitchRun;
          }
          break;

        case enMoveOffRefSwitchRun: // Auf Referenzschalter fahren
          if (lenEvent == evRefSwitch)
          {
            mu32SpeedSoll = 0;
            vStopTimer(); // Hard Stop
            X->mPinTakt->vSet0();
            Y->mPinTakt->vSet0();
            Z->mPinTakt->vSet0();
            menRefMode = enMoveOffRefSwitchFinish;
            lbLoop = True;
          }
          break;

        case enMoveOffRefSwitchFinish: // Runter-Bremsen
          mbRefEnable   = False;
          mbRefFinished = True;

          if (X->mbActive) { X->mi32PosIst = 0; }
          if (Y->mbActive) { Y->mi32PosIst = 0; }
          if (Z->mbActive) { Z->mi32PosIst = 0; }

          vDoStepEnd();
          break;

        default:
          break;
      }
    } while (lbLoop);
  }


  bool isBusy()
  {
    return (mbRefEnable || (isDriving()));
  }


  void vSetDelayCounter()
  {
    if (menDriveMode == enDrvModeSteps)
    {
      if (mu32StepsIst < mu32StepsSoll)
      {
        if (mu32StepsIst <= mu32StepsAcc)
        {
          menSpeedMode = enSpeedAccel;
        }
        else if (mu32StepsIst < (mu32StepsAcc + mu32StepsRun))
        {
          menSpeedMode = enSpeedKeep;
        }
        else
        {
          menSpeedMode = enSpeedStop;
        }

        u32 lu32NextTimValue_us = 1000;

        switch (menSpeedMode)
        {
          case enSpeedAccel:
            //lu32NextTimValue_us = u32TimeToNextTick_us(0, mdAcc, mu32StepsIst);
            lu32NextTimValue_us = mau16AccTable[mu32StepsIst];
            break;

          case enSpeedKeep:
            //lu32NextTimValue_us = u32TimeToNextTick_us(mdSpeedSoll, 0, mu32StepsIst);
            lu32NextTimValue_us = mu16TimeStep;
            break;

          case enSpeedStop:
            //lu32NextTimValue_us = u32TimeToNextTick_us(0, mdAcc, mu32StepsSoll - mu32StepsIst + 1);
            lu32NextTimValue_us = mau16AccTable[mu32StepsSoll - mu32StepsIst];
            break;
        }

        if (lu32NextTimValue_us > 50000) lu32NextTimValue_us = 50000;
        if (lu32NextTimValue_us >  3) lu32NextTimValue_us  -= 3;
                                 else lu32NextTimValue_us  -= 0;
        mstTimDelay->ARR = lu32NextTimValue_us;
        mstTimDelay->CNT = 0;
        LL_TIM_EnableCounter(mstTimDelay);
      }
    }
    else
    {
      if (mu32StepsIst <= mu32StepsAcc)
      {
        menSpeedMode = enSpeedAccel;
      }
      else
      {
        menSpeedMode = enSpeedKeep;
      }

      if (mu32SpeedSoll == 0)
      {
        menSpeedMode = enSpeedStop;
      }

      u32 lu32NextTimValue_us = 1000;

      switch (menSpeedMode)
      {
        case enSpeedAccel:
          //lu32NextTimValue_us = u32TimeToNextTick_us(0, mdAcc, mu32StepsIst);
          lu32NextTimValue_us = mau16AccTable[mu32StepsIst];
          break;

        case enSpeedKeep:
          //lu32NextTimValue_us = u32TimeToNextTick_us(mdSpeedSoll, 0, mu32StepsIst);
          lu32NextTimValue_us = mu16TimeStep;
          break;

        case enSpeedStop:
          //lu32NextTimValue_us = u32TimeToNextTick_us(0, mdAcc, mu32StepsSoll - mu32StepsIst + 1);
          lu32NextTimValue_us = mau16AccTable[mu32StepsDeAcc];
          mu32StepsDeAcc--;
          break;
      }

      if (mu32StepsDeAcc)
      {
        if (lu32NextTimValue_us > 50000) lu32NextTimValue_us = 50000;
        if (lu32NextTimValue_us > 3) lu32NextTimValue_us -= 3;
        else lu32NextTimValue_us -= 0;
        mstTimDelay->ARR = lu32NextTimValue_us;
        mstTimDelay->CNT = 0;
        LL_TIM_EnableCounter(mstTimDelay);
      }
      else
      {
        vRefSm(evRefDrvFinish);
      }
    }
  }

  void vStepStart(cDrvCmd* lcDrvCmd)
  {
    mcDrvCmd = lcDrvCmd;

    if ((mcDrvCmd->munCmd.Bit.DrvX) ||
        (mcDrvCmd->munCmd.Bit.DrvY) ||
        (mcDrvCmd->munCmd.Bit.DrvZ))
    {
      if ((mcDrvCmd->mi32StepX == 0) &&
          (mcDrvCmd->mi32StepY == 0) &&
          (mcDrvCmd->mi32StepZ == 0))
      {
        vDoStepEnd();
        return;
      }


      if (mcDrvCmd->mi32StepX >= 0) { X->mi8Dir =  1; X->vSetDir(0); }
                             else   { X->mi8Dir = -1; X->vSetDir(1); mcDrvCmd->mi32StepX = -mcDrvCmd->mi32StepX; }
      if (mcDrvCmd->mi32StepY >= 0) { Y->mi8Dir =  1; Y->vSetDir(0); }
                             else   { Y->mi8Dir = -1; Y->vSetDir(1); mcDrvCmd->mi32StepY = -mcDrvCmd->mi32StepY; }
      if (mcDrvCmd->mi32StepZ >= 0) { Z->mi8Dir =  1; Z->vSetDir(0); }
                             else   { Z->mi8Dir = -1; Z->vSetDir(1); mcDrvCmd->mi32StepZ = -mcDrvCmd->mi32StepZ; }
      X->mbActive = False;
      Y->mbActive = False;
      Z->mbActive = False;

      if (mcDrvCmd->mi32StepX != 0) { X->mbActive = True; }
      if (mcDrvCmd->mi32StepY != 0) { Y->mbActive = True; }
      if (mcDrvCmd->mi32StepZ != 0) { Z->mbActive = True; }


      mu32StepsSoll = mcDrvCmd->mi32StepX;

      if ((u32)mcDrvCmd->mi32StepY > mu32StepsSoll)
      {
        mu32StepsSoll = mcDrvCmd->mi32StepY;
      }

      if ((u32)mcDrvCmd->mi32StepZ > mu32StepsSoll)
      {
        mu32StepsSoll = mcDrvCmd->mi32StepZ;
      }

      if (mu32StepsSoll > 0)
      {
        X->mdFactor = (float)(mcDrvCmd->mi32StepX) / mu32StepsSoll;
        Y->mdFactor = (float)(mcDrvCmd->mi32StepY) / mu32StepsSoll;
        Z->mdFactor = (float)(mcDrvCmd->mi32StepZ) / mu32StepsSoll;
      }

      mu32StepsIst  = 1;
      mu32Acc       = 100;

      mu16TimeStep = ((1.0f / (float)mcDrvCmd->mi32Spd) * 1000000.0f);

      mu32StepsAcc = mu32StepsDeAcc = i32TicksToSpeed(0, mcDrvCmd->mi32Spd, mu32Acc) - 1;

      if ((mu32StepsAcc + mu32StepsDeAcc) > mu32StepsSoll)
      {
        mu32StepsAcc = mu32StepsSoll / 2;
        mu32StepsDeAcc = mu32StepsSoll - mu32StepsAcc;
      }

      mu32StepsRun = mu32StepsSoll - (mu32StepsAcc + mu32StepsDeAcc);

      X->mu8Step = Y->mu8Step = Z->mu8Step = 0;

      X->mi32StepIst_Last = 0;
      Y->mi32StepIst_Last = 0;
      Z->mi32StepIst_Last = 0;

      if (X->mbActive) { X->mi32StepIst = 1.0f * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;  X->mPinTakt->vSet1(); X->mu8Step = 1; } }
      if (Y->mbActive) { Y->mi32StepIst = 1.0f * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;  Y->mPinTakt->vSet1(); Y->mu8Step = 1; } }
      if (Z->mbActive) { Z->mi32StepIst = 1.0f * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;  Z->mPinTakt->vSet1(); Z->mu8Step = 1; } }

      mu32TimeNoMove = mu32TimeNoMoveReload;
      mPinStromAbs->vSet1();

      LL_TIM_EnableCounter(mstTimStep);
      if (mu32StepsSoll > 1)
      {
        menDriveMode = enDrvModeSteps;
        vSetDelayCounter();
      }
    }
    else
    {
      if ((mcDrvCmd->munCmd.Bit.RefX) ||
          (mcDrvCmd->munCmd.Bit.RefY) ||
          (mcDrvCmd->munCmd.Bit.RefZ))
      {
        X->mbActive = False;
        Y->mbActive = False;
        Z->mbActive = False;

        if (mcDrvCmd->munCmd.Bit.RefZ) { Z->mbActive = True; mu32RefSpeed = Z->mu32RefSpeed; }
        if (mcDrvCmd->munCmd.Bit.RefY) { Y->mbActive = True; mu32RefSpeed = Y->mu32RefSpeed; }
        if (mcDrvCmd->munCmd.Bit.RefX) { X->mbActive = True; mu32RefSpeed = X->mu32RefSpeed; }

        mbRefEnable   = True;
        mbRefFinished = False;

        menRefMode = enMove1sForward;
        vRefSm(evRefStart);
      }
    }
  }

  bool isDriving()
  {
    return (mcDrvCmd != NULL);
  }

  virtual void vJobFinished()
  {
  }

  void DoProcess1ms()
  {
    if (mu32TimeNoMove > 0)
    {
      mu32TimeNoMove--;
      if (mu32TimeNoMove == 0)
      {
        // Alle Achsen wurden seit einiger Zeit nicht mehr bewegt.
        // Stromabsenkung
        mPinStromAbs->vSet0();
      }
    }
  }

  void vDoStepEnd()
  {
    mcDrvCmd = NULL;
    vJobFinished();
  };

  void vIrqHandlerTimDelay()
  {
    // letzter Schritt immer alle an
    if ((mu32StepsIst == mu32StepsSoll) && (!mbRefEnable))
    {
      if (X->mbActive) { X->mi32StepIst = mu32StepsIst * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;}; X->mPinTakt->vSet1(); X->mu8Step = 1; }
      if (Y->mbActive) { Y->mi32StepIst = mu32StepsIst * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;}; Y->mPinTakt->vSet1(); Y->mu8Step = 1; }
      if (Z->mbActive) { Z->mi32StepIst = mu32StepsIst * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;}; Z->mPinTakt->vSet1(); Z->mu8Step = 1; }
    }
    else
    {
      if (X->mbActive) {X->mi32StepIst = mu32StepsIst * X->mdFactor; if (X->mi32StepIst > X->mi32StepIst_Last) { X->mi32StepIst_Last = X->mi32StepIst;  X->mPinTakt->vSet1(); X->mu8Step = 1; }}
      if (Y->mbActive) {Y->mi32StepIst = mu32StepsIst * Y->mdFactor; if (Y->mi32StepIst > Y->mi32StepIst_Last) { Y->mi32StepIst_Last = Y->mi32StepIst;  Y->mPinTakt->vSet1(); Y->mu8Step = 1; }}
      if (Z->mbActive) {Z->mi32StepIst = mu32StepsIst * Z->mdFactor; if (Z->mi32StepIst > Z->mi32StepIst_Last) { Z->mi32StepIst_Last = Z->mi32StepIst;  Z->mPinTakt->vSet1(); Z->mu8Step = 1; }}
      vSetDelayCounter();
    }
    LL_TIM_EnableCounter(mstTimStep);
  }

  void vIrqHandlerTimStep()
  {
    X->mPinTakt->vSet0();
    Y->mPinTakt->vSet0();
    Z->mPinTakt->vSet0();

    X->isRefSwitch();
    Y->isRefSwitch();
    Z->isRefSwitch();

    if (X->mu8Step) { X->mi32PosIst += X->mi8Dir; X->mu8Step = 0; };
    if (Y->mu8Step) { Y->mi32PosIst += Y->mi8Dir; Y->mu8Step = 0; };
    if (Z->mu8Step) { Z->mi32PosIst += Z->mi8Dir; Z->mu8Step = 0; };

    mu32TimeNoMove = mu32TimeNoMoveReload;

    if (mbRefEnable)
    {
      if ((X->hasRefSwitchChanged()) || (Y->hasRefSwitchChanged()) || (Z->hasRefSwitchChanged()))
      {
        vRefSm(evRefSwitch);
      }
    }
    else
    {
      if (mu32StepsIst >= mu32StepsSoll)
      {
        vDoStepEnd();
        return;
      }
    }

    mu32StepsIst++;
  }
};



template <typename tycRingBufT, typename tycRingBufTIndex>
class cCncRingBufT
{
public:
  tycRingBufT*     mtyBuf;
  tycRingBufTIndex mtySize;
  tycRingBufTIndex mtyHead;
  tycRingBufTIndex mtyCnt;
  tycRingBufTIndex mtyTail;

  inline void inc(tycRingBufTIndex &lVal)
  {
    lVal++;
    if (lVal >= mtySize) lVal = 0;
  }

  inline void dec(tycRingBufTIndex &lVal)
  {
    if (lVal == 0) lVal = mtySize;
    lVal--;
  }

  cCncRingBufT(const tycRingBufT* lptyBuf, const tycRingBufTIndex ltySize)
  {
    reset();
    mtySize = ltySize;
    mtyBuf  = (tycRingBufT*)lptyBuf;
  }

  inline uint32 SizeOf()
  {
    return sizeof(cRingBufT<tycRingBufT, tycRingBufTIndex>) + mtySize * sizeof(tycRingBufT);
  }

  inline void reset()
  {
    mtyHead = mtyTail = mtyCnt = 0;
  }

  inline bool isEmpty()
  {
    return mtyCnt == 0;
  }

  inline tycRingBufTIndex cnt()
  {
    return mtyCnt;
  }

  inline tycRingBufTIndex space_left()
  {
    return (mtySize - mtyCnt);
  }

  inline bool isFull()
  {
    return mtyCnt == mtySize;
  }

  void put_unsafe(tycRingBufT ltyItem, bool lbOverwrite)
  {
    if (mtyCnt < mtySize)
    {
      mtyBuf[mtyHead] = ltyItem;
      inc(mtyHead);
      mtyCnt++;
    }
    else
    {
      if (lbOverwrite)
      {
        mtyBuf[mtyHead] = ltyItem;
        inc(mtyHead);
        inc(mtyTail);
      }
    }
  }

  void put_unsafe(tycRingBufT ltyItem)
  {
    put_unsafe(ltyItem, True);
  }


  inline void put(tycRingBufT ltyItem, bool lbOverwrite)
  {
    _dai();
    put_unsafe(ltyItem, lbOverwrite);
    _eai();
  }

  inline void put(tycRingBufT ltyItem)
  {
    put(ltyItem, True);
  }


  tycRingBufT get_unsafe(void)
  {
    tycRingBufT ltyZwerg;

    if (isEmpty())
    {
      tycRingBufT lcDummy;
      return lcDummy;
    }

    //Read data and advance the tail (we now have a free space)
    ltyZwerg = mtyBuf[mtyTail];
    inc(mtyTail);
    mtyCnt--;

    return ltyZwerg;
  }


  inline tycRingBufT get(void)
  {
    tycRingBufT ltyZwerg;
    _dai();
    ltyZwerg = get_unsafe();
    _eai();

    return ltyZwerg;
  }


  inline void get(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    get_unsafe(ltyItem, lu32Cnt);
    _eai();
  }
};



class cCnc: public cPalCmd, public cCncControl
{
  public:
    u8 mu8AxisCount;

    typedef enum
    {
      enModeSingle = 0,
      enMode2D5,
      enMode3D
    }tenMoveMode;

    tenMoveMode menMode;

    typedef enum
    {
      enCmdNop = 0,
      enCmdAchsenanzahl_setzen,
      enCmdReferenzfahrt,
      enCmdReferenzgeschwindigkeit_setzen,
      enCmdBewegung_relativ,
      enCmdBewegung_zur_Position,
      enCmdStop,
      enCmdBreak
    }tenProcessedCmd;

    cDrvCmd  macDrvCmdBuf[16];
    cCncRingBufT<cDrvCmd, u16> mcDrvCmds;


    bool mbAnswer;

    tenProcessedCmd menProcessedCmd;

    cRingBufT<uint8, uint16>* mcStreamOut;

    cCnc(cRingBufT<uint8, uint16>* lcStreamOut,
         TIM_TypeDef* lstTimDelay, TIM_TypeDef* lstTimStep,
         cCncAchse* lcAX, cCncAchse* lcAY, cCncAchse* lcAZ,
         cGpPin* lPinStromAbs)
      : cCncControl(lstTimDelay, lstTimStep, lcAX, lcAY, lcAZ, lPinStromAbs),
        mcDrvCmds(macDrvCmdBuf, 16)
    {
      mu8AxisCount    = 0;
      menMode         = enMode2D5;
      menProcessedCmd = enCmdNop;
      mcStreamOut     = lcStreamOut;
      mbAnswer        = False;
    }

    bool isBusy()
    {
      return (cCncControl::isDriving() || (mcDrvCmds.cnt() > 0));
    }


    void vStringToParam(cStr* lcStrIn, u8 lu8Seperator, i32 li32Values[], u16 lu16ValueSize)
    {
      cStr_Create(lcZwerg, 64);
      cStr_Create(lcValue, 64);

      u16 lu16t;
      for (lu16t = 0; lu16t < lu16ValueSize; lu16t++)
      {
        li32Values[lu16t] = 0;
      }

      lcZwerg = *lcStrIn;

      u16 lu16SepCnt = lcZwerg.CountOf(lu8Seperator);

      if (lu16SepCnt >= lu16ValueSize) lu16SepCnt = lu16ValueSize - 1;

      u16 lu16PosStart = 0;
      u16 lu16PosEnd   = 0;

      for (lu16t = 0; lu16t < lu16SepCnt; lu16t++)
      {
        lu16PosEnd = lcZwerg.Instr(lu16PosStart, lu8Seperator);
        lcValue = lcZwerg;
        lcValue.Mid(lu16PosStart, lu16PosEnd - lu16PosStart);
        li32Values[lu16t] = lcValue.Atoi();
        lu16PosStart = lu16PosEnd + 1;
      }

      lu16PosEnd = lcZwerg.Len();
      lcValue = lcZwerg;
      lcValue.Mid(lu16PosStart, lu16PosEnd - lu16PosStart);
      li32Values[lu16t] = lcValue.Atoi();
    }

    void vSet(i32 li32StepsX, i32 li32SpeedX,
              i32 li32StepsY, i32 li32SpeedY,
              i32 li32StepsZ, i32 li32SpeedZ)
    {
      switch (menMode)
      {
        case enModeSingle:
          {
            mcDrvCmds.put(cDrvCmd(0, 0, 0, 1, 0, 0, li32StepsX, 0,          0,          li32SpeedX));
            mcDrvCmds.put(cDrvCmd(0, 0, 0, 0, 1, 0, 0,          li32StepsY, 0,          li32SpeedY));
            mcDrvCmds.put(cDrvCmd(0, 0, 0, 0, 0, 1, 0,          0,          li32StepsZ, li32SpeedZ));
          }
          break;
        case enMode2D5:
          {
            //  Im 2, 5D - Interpolationsbetrieb wird die
            //    Geschwindigkeitsangabe der Achse mit dem längsten
            //    Weg als Bahngeschwindigkeit übernommen, die
            //    Geschwindigkeit der anderen Achse entsprechend dem
            //    Wegeverhältnis angepasst.
            i32 li32Speed = 0;
            if (abs(li32StepsX) > abs(li32StepsY))
            {
              li32Speed = li32SpeedX;
            }
            else
            {
              li32Speed = li32SpeedY;
            }

            mcDrvCmds.put(cDrvCmd(0, 0, 0, 1, 0, 0, li32StepsX, li32StepsY, 0,          li32Speed));
            mcDrvCmds.put(cDrvCmd(0, 0, 0, 0, 0, 1, 0,          0,          li32StepsZ, li32SpeedZ));
          }
          break;
        case enMode3D:
          {
            // Im Gegensatz dazu wird im 3D - Interpolationsbetrieb die
            //   Geschwindigkeitsangabe der X - Achse als Vorgabewert
            //   für die Bahngeschwindigkeit herangezogen.
            mcDrvCmds.put(cDrvCmd(0, 0, 0, 1, 0, 0, li32StepsX, li32StepsY, li32StepsZ, li32SpeedX));
          }
          break;
      }

      if (mcDrvCmds.cnt() > 0)
      {
        cDrvCmd lcDrvCmd = mcDrvCmds.get();
        vStepStart(&lcDrvCmd);
      }
    }


    // Takt 0..50khz
    // Beschleunigung  25 Hz/ms, 50 Hz/ms, 75 Hz/ms, 100 Hz/ms

    // Durch Übergeben der Achsenanzahl wird die Prozessorkarte
    // neu initialisiert.Dabei wird der Datenspeicher gelöscht und zur
    // Speicheroptimierung entsprechend der Anzahl der Achsen neu
    // eingeteilt.
    void vAchsenanzahl_setzen(cStr* lcStrIn, bool lbAnswer) override
    { // @07
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdAchsenanzahl_setzen;
      mbAnswer = lbAnswer;

      switch (lcStrIn->mpu8Data[2])
      {
        case '1': // x
          X->mbEnable = True;
          mu8AxisCount = 1;
          break;
        case '3': // xy
          X->mbEnable = True;
          Y->mbEnable = True;
          mu8AxisCount = 2;
          break;
        case '5': // xz
          X->mbEnable = True;
          Z->mbEnable = True;
          mu8AxisCount = 2;
          break;
        case '7': // xyz
          X->mbEnable = True;
          Y->mbEnable = True;
          Z->mbEnable = True;
          mu8AxisCount = 3;
          break;
      }
    }

    // Die Prozessorkarte verfährt alle angegebenen Achsen an ihre
    // Nullpunkte(Referenzpunkte).Die Referenzpunkte der Achsen
    // liegen bei isel - Systemen an der dem Motor zugewandten
    // Seite; positive Bewegungen finden in der Richtung vom Motor
    // weg statt.
    void vReferenzfahrt(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdReferenzfahrt;
      mbAnswer = lbAnswer;

      // @0R7 oder @0r7
      switch (lcStrIn->mpu8Data[3])
      {
        case '1': // x
          mcDrvCmds.put(cDrvCmd(1, 0, 0, 0, 0, 0, 0, 0, 0, X->mu32RefSpeed));
          break;
        case '2': // y
          mcDrvCmds.put(cDrvCmd(0, 1, 0, 0, 0, 0, 0, 0, 0, Y->mu32RefSpeed));
          break;
        case '3': // xy
          mcDrvCmds.put(cDrvCmd(1, 0, 0, 0, 0, 0, 0, 0, 0, X->mu32RefSpeed));
          mcDrvCmds.put(cDrvCmd(0, 1, 0, 0, 0, 0, 0, 0, 0, Y->mu32RefSpeed));
          break;
        case '4': // z
          mcDrvCmds.put(cDrvCmd(0, 0, 1, 0, 0, 0, 0, 0, 0, Z->mu32RefSpeed));
          break;
        case '5': // xz
          mcDrvCmds.put(cDrvCmd(0, 0, 1, 0, 0, 0, 0, 0, 0, Z->mu32RefSpeed));
          mcDrvCmds.put(cDrvCmd(1, 0, 0, 0, 0, 0, 0, 0, 0, X->mu32RefSpeed));
          break;
        case '6': // yz
          mcDrvCmds.put(cDrvCmd(0, 0, 1, 0, 0, 0, 0, 0, 0, Z->mu32RefSpeed));
          mcDrvCmds.put(cDrvCmd(0, 1, 0, 0, 0, 0, 0, 0, 0, X->mu32RefSpeed));
          break;
        case '7': // xyz
          mcDrvCmds.put(cDrvCmd(0, 0, 1, 0, 0, 0, 0, 0, 0, Z->mu32RefSpeed));
          mcDrvCmds.put(cDrvCmd(1, 0, 0, 0, 0, 0, 0, 0, 0, X->mu32RefSpeed));
          mcDrvCmds.put(cDrvCmd(0, 1, 0, 0, 0, 0, 0, 0, 0, Y->mu32RefSpeed));
          break;
      }

      if (mcDrvCmds.cnt() > 0)
      {
        cDrvCmd lcDrvCmd = mcDrvCmds.get();
        vStepStart(&lcDrvCmd);
      }
    }

    // Der Befehl definiert - getrennt für jede Achse - die Geschwindigkeit,
    // mit der eine Referenzfahrt ausgeführt wird.Dabei handelt es sich nur
    // um die Geschwindigkeit, mit der die Achse in negativer Richtung auf
    // den Motor zuläuft; die Geschwindigkeit aus dem Schalter heraus lässt
    // sich nicht beeinflussen(vgl.Referenzfahrt).
    void vReferenzgeschwindigkeit_setzen(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdReferenzgeschwindigkeit_setzen;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0d1000, 3000, 9000
      lcZwerg.Cut(0, 3); // "@0d" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 0;
      if (Y->mbEnable) lu16ExpectedCommas += 1;
      if (Z->mbEnable) lu16ExpectedCommas += 1;

      i32 li32Value[3];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas + 1);

      if (lu16ExpectedCommas == lu16Commas)
      {
        switch (mu8AxisCount)
        {
          case 1:
            X->mu32RefSpeed = li32Value[0];
            break;
          case 2:
            X->mu32RefSpeed = li32Value[0];
            if (Y->mbEnable) Y->mu32RefSpeed = li32Value[1];
                        else Z->mu32RefSpeed = li32Value[1];
            break;
          case 3:
            X->mu32RefSpeed = li32Value[0];
            Y->mu32RefSpeed = li32Value[1];
            Z->mu32RefSpeed = li32Value[2];
            break;
        }
      }
    }

    // Die Prozessorkarte gibt entsprechend der übergebenen
    // Schrittanzahl und Schrittgeschwindigkeit für jede
    // Leistungsendstufe eine Impulskette aus.Die Verfahrbewegung
    // wird sofort ausgeführt oder gespeichert.

    // Im 2,5D-Interpolationsbetrieb wird die
    // Geschwindigkeitsangabe der Achse mit dem längsten
    // Weg als Bahngeschwindigkeit übernommen, die
    // Geschwindigkeit der anderen Achse entsprechend dem
    // Wegeverhältnis angepasst.
    // Im Gegensatz dazu wird im 3D - Interpolationsbetrieb die
    // Geschwindigkeitsangabe der X - Achse als Vorgabewert
    // für die Bahngeschwindigkeit herangezogen.
    void vBewegung_relativ(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdBewegung_relativ;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0A 30,800,10,900,4,90,-4,30
      lcZwerg.Cut(0, 3); // "@0a" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 1;
      if (Y->mbEnable) lu16ExpectedCommas += 2;
      if (Z->mbEnable) lu16ExpectedCommas += 4;

      i32 li32Value[8];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas + 1);

      if (lu16Commas == lu16ExpectedCommas)
      {
        switch (mu8AxisCount)
        {
        case 1:
          vSet(X->mi32PosIst + li32Value[0], li32Value[1], 0.0, 0.0, 0.0, 0.0);
          break;
        case 2:
          if (Y->mbEnable)
          {
            vSet(li32Value[0], li32Value[1],
                 li32Value[2], li32Value[3],
                 0, 0);
          }
          else
          {
            vSet(li32Value[0], li32Value[1],
                 0, 0,
                 li32Value[4], li32Value[5]);
          }
          break;
        case 3:
            vSet(li32Value[0], li32Value[1],
                 li32Value[2], li32Value[3],
                 li32Value[4], li32Value[5]);
          break;
        }
      }
    }

    // Die Prozessorkarte fährt mit den angegebenen
    // Geschwindigkeiten an die angegebene Position.Die
    // Verfahrbewegung wird sofort ausgeführt.
    void vBewegung_zur_Position(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdBewegung_zur_Position;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0A 30,800,10,900,4,90,-4,30
      lcZwerg.Cut(0, 3); // "@0a" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 1;
      if (Y->mbEnable) lu16ExpectedCommas += 2;
      if (Z->mbEnable) lu16ExpectedCommas += 4;

      i32 li32Value[8];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas);

      if (lu16Commas == lu16ExpectedCommas)
      {
        switch (mu8AxisCount)
        {
        case 1:
          vSet(li32Value[0] - X->mi32PosIst, li32Value[1], 0, 0, 0, 0);
          break;
        case 2:
          if (Y->mbEnable)
          {
            vSet(li32Value[0] - X->mi32PosIst, li32Value[1],
                 li32Value[2] - Y->mi32PosIst, li32Value[3],
                 0, 0);
          }
          else
          {
            vSet(li32Value[0] - X->mi32PosIst, li32Value[1],
                 0, 0,
                 li32Value[4] - Z->mi32PosIst, li32Value[5]);
          }
          break;
        case 3:
            vSet(li32Value[0] - X->mi32PosIst, li32Value[1],
                 li32Value[2] - Y->mi32PosIst, li32Value[3],
                 li32Value[4] - Z->mi32PosIst, li32Value[5]);
          break;
        }
      }
    }

    // Die Prozessorkarte bricht eine momentane Verfahrbewegung ab.
    void vStop(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lcStrIn);
      UNUSED(lbAnswer);
      menProcessedCmd = enCmdStop;
      mbAnswer = lbAnswer;
      cCncControl::vStop();

      while (mcDrvCmds.cnt() > 0)
      {
        mcDrvCmds.get();
      }
    }

    // Der Break - Befehl dient zum Abbruch des Aktuellen Positioniervorganges.
    void vBreak(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lcStrIn);
      UNUSED(lbAnswer);
      menProcessedCmd = enCmdBreak;
      mbAnswer = lbAnswer;
      cCncControl::vBreak();

      while (mcDrvCmds.cnt() > 0)
      {
        mcDrvCmds.get();
      }
    }

    void vJobFinished() override
    {
      if (mcDrvCmds.cnt() > 0)
      {
        cDrvCmd lcDrvCmd = mcDrvCmds.get();
        vStepStart(&lcDrvCmd);
      }
      else
      {
        menProcessedCmd = enCmdNop;
        if (mbAnswer)
        {
          mbAnswer = False;
          mcStreamOut->put('0');
        }
      }
    }

    void DoProcess1ms()
    {
      cCncControl::DoProcess1ms();

      if (menProcessedCmd != enCmdNop)
      {
        if (!isBusy())
        {
          menProcessedCmd = enCmdNop;
          if (mbAnswer)
          {
            mbAnswer = False;
            mcStreamOut->put('0');
          }
        }
      }
    }
};

#endif /* _C_CNC_H */
