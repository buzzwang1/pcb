#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"

#include "tGPPin.h"
#include "LED.h"
#include "math.h"

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


class cMotControl_Base
{
  public:

  enum class cenDriveMode : u8 {nSpeed, nPos};

  // --- Pins
  cGpPin* mcPinMotA1;
  cGpPin* mcPinMotA2;
  cGpPin* mcPinMotB1;
  cGpPin* mcPinMotB2;

  // Vollschritt: 4 Ticks eine Motorumdrehung
  // Motorgetriebe 1:43,2 => 172,8 Ticks für 360°
  // Äußeres Ritzel: 25 Zähne
  // Äußerer Zahngranz: 229 Zähne
  //
  // ==> 172,8 * 229 / 25 = 1582,8 Ticks/360°
  // ==> ~4,4 Ticks/°.

  // Halbschritt: 8 Ticks eine Motorumdrehung
  // Motorgetriebe 1:43,2 => 345,6 Ticks für 360°
  // Äußeres Ritzel: 25 Zähne
  // Äußerer Zahngranz: 229 Zähne
  //
  // ==> 345,6 * 229 / 25 = 3165,7 Ticks/360°
  // ==> ~8 Ticks/°.
  i16 mi16ActualPos;
  i16 mi16TargetPos;

  i16 mi16SpeedCnt;
  u16 mu16SpeedTicks;

  // -200 bis 200 Ticks/Sekunde
  i16   mi16ActualSpeed;
  i16   mi16TargetSpeed;


  cenDriveMode menDriveMode;

  bool mbInverse;
  bool mbVollSchritt;


  cMotControl_Base(cGpPin* lcPinMotA1, cGpPin* lcPinMotA2, cGpPin* lcPinMotB1, cGpPin* lcPinMotB2, bool lbInverse, bool lbVollSchritt)
  {
    vNull();

    mcPinMotA1 = lcPinMotA1;
    mcPinMotA2 = lcPinMotA2;
    mcPinMotB1 = lcPinMotB1;
    mcPinMotB2 = lcPinMotB2;

    mi16SpeedCnt = 0;
    mu16SpeedTicks = 0;
    mi16ActualSpeed = 0;
    mi16TargetSpeed = 0;
    menDriveMode = cenDriveMode::nPos;

    mbInverse = lbInverse;
    mbVollSchritt = lbVollSchritt;
  }


  void vStep()
  {
    if (mbVollSchritt)
    {
      if (mbInverse)
      {
        switch (mi16ActualPos & 3)
        {
          case 3: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 2: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 1: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 0: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
        }
      }
      else
      {
        switch (mi16ActualPos & 3)
        {
          case 0: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 1: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 2: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 3: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
        }
      }
    }
    else
    {
      if (mbInverse)
      {
        switch (mi16ActualPos & 7)
        {
          case 7: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet0(); break;
          case 6: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 5: mcPinMotA1->vSet0(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 4: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 3: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet0(); break;
          case 2: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 1: mcPinMotA1->vSet0(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 0: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
        }
      }
      else
      {
        switch (mi16ActualPos & 7)
        {
          case 0: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet0(); break;
          case 1: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 2: mcPinMotA1->vSet0(); mcPinMotA2->vSet0(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 3: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet1(); mcPinMotB2->vSet0(); break;
          case 4: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet0(); break;
          case 5: mcPinMotA1->vSet0(); mcPinMotA2->vSet1(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 6: mcPinMotA1->vSet0(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
          case 7: mcPinMotA1->vSet1(); mcPinMotA2->vSet0(); mcPinMotB1->vSet0(); mcPinMotB2->vSet1(); break;
        }
      }
    }
  }


  void vTick1ms()
  {
    
    if (menDriveMode == cenDriveMode::nPos)
    {
      if ((mi16ActualPos < mi16TargetPos + 5) &&
          (mi16ActualPos > mi16TargetPos - 5))
      {
        mi16ActualSpeed = 0;
      }
      else if (mi16ActualPos < mi16TargetPos)
      {
        mi16ActualSpeed = 100;
      }
      else if (mi16ActualPos > mi16TargetPos)
      {
        mi16ActualSpeed = -100;
      }
    }
    else // if (menDriveMode == cenDriveMode::nSpeed)
    {
      mi16ActualSpeed = mi16TargetSpeed;
    }


    if (mi16ActualSpeed > 0)
    {
      mi16SpeedCnt = 1000 / mi16ActualSpeed;
    }
    else if (mi16ActualSpeed < 0)
    {
      mi16SpeedCnt = 1000 / -mi16ActualSpeed;
    }
    else
    {
      mi16SpeedCnt = 10;
    }


    if (mu16SpeedTicks >= mi16SpeedCnt)
    {
      mu16SpeedTicks = 0;

      if (mi16ActualSpeed > 0)
      {
        mi16ActualPos++;
        vStep();
      }
      else
      if (mi16ActualSpeed < 0)
      {
        mi16ActualPos--;
        vStep();
      }
      else
      {
        mcPinMotA1->vSet0();
        mcPinMotA2->vSet0();
        mcPinMotB1->vSet0();
        mcPinMotB2->vSet0();
      }
    }
    else
    {
      mu16SpeedTicks++;
    }
  }


  void vSetPosTicks(i16 li16TargetPos)
  {
    menDriveMode = cenDriveMode::nPos;
    mi16TargetPos = li16TargetPos;
  }

  void vSetPosGrad(i16 li16TargetPos)
  {
    menDriveMode = cenDriveMode::nPos;
    mi16TargetPos = i16Grad2Ticks(li16TargetPos);
  }


  void vSetSpeed(i16 li16TargetSpeed)
  {
    menDriveMode = cenDriveMode::nSpeed;
    mi16TargetSpeed = li16TargetSpeed;
  }


  void vNull()
  {
    mi16ActualPos = 0;
    mi16TargetPos = 0;
  }

  bool isRdy()
  {
    return (mi16ActualPos == mi16TargetPos);
  }

  virtual i16 i16GetPosGrad()
  {
    return mi16ActualPos;
  }

  virtual i16 i16Grad2Ticks(i16 li16Grad)
  {
    return li16Grad;
  }
};


class cMot_TurmDrehung : public cMotControl_Base
{
  public:

  // --- Pins
  cGpPin mcPinMot1;
  cGpPin mcPinMot2;
  cGpPin mcPinMot3;
  cGpPin mcPinMot4;

  i8     miSensor; // -1 rot links, 0 = schwarz Referenzpunkt, 1 grün rechts

  //TIM1_CH1 PA8
  //TIM1_CH2 PA9
  //TIM1_CH3 PA10
  //TIM1_CH4 PA11
  cMot_TurmDrehung()
    : cMotControl_Base(&mcPinMot1, &mcPinMot2, &mcPinMot3, &mcPinMot4, False, False),
      mcPinMot1(GPIOA_BASE, 6,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot2(GPIOA_BASE, 7,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot3(GPIOB_BASE, 0,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot4(GPIOB_BASE, 1,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    miSensor = 0;
  }

  void vCheckPositionAls(u16 C, u16 R, u16 G, u16 B)
  {
    // rot     = C:95 R:60 G:16 B:23
    // schwarz = C:40 R:10 G:14 B:13
    // grün    = C:72 R:14 G:30 B:24

    if ((C > (95 - 10)) && (C < (95 + 10)))
    {
      if ((R > G) && (R > B)) miSensor = -1;
    }
    else if ((C > (40 - 10)) && (C < (40 + 10))) miSensor = 0;
    else if ((C > (72 - 10)) && (C < (72 + 10)))
          {
            if ((G > R) && (G > B)) miSensor = 1;
          }

  }

  i16 i16Grad2Ticks(i16 li16Grad) override
  {
    if (mbVollSchritt) return li16Grad * 4;
    return li16Grad * 8;
  }

  i16 i16GetPosGrad() override
  {
    if (mbVollSchritt) return mi16ActualPos / 4;
    return mi16ActualPos / 8;
  }
};


// 5530 Schritte für 360°
class cMot_TurmNeigung : public cMotControl_Base
{
  public:

  // --- Pins
  cGpPin mcPinMot1;
  cGpPin mcPinMot2;
  cGpPin mcPinMot3;
  cGpPin mcPinMot4;

  //TIM3_CH1 PA6
  //TIM3_CH2 PA7
  //TIM3_CH3 PB0
  //TIM3_CH4 PB1
  cMot_TurmNeigung()
    : cMotControl_Base(&mcPinMot1, &mcPinMot2, &mcPinMot3, &mcPinMot4, True, False),
      mcPinMot1(GPIOA_BASE,  8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot2(GPIOA_BASE,  9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot3(GPIOA_BASE, 10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
      mcPinMot4(GPIOA_BASE, 11, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
  }

  i16 i16Grad2Ticks(i16 li16Grad) override
  {
    if (mbVollSchritt) return li16Grad * 4;
    return li16Grad * 8;
  }

  i16 i16GetPosGrad() override
  {
    if (mbVollSchritt) return mi16ActualPos / 4;
    return mi16ActualPos / 8;
  }
};


#endif  //__MOTOR_CONTROL_H__
