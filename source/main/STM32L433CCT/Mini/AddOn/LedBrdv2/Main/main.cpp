
#include "main.h"
#include "stdlib.h" // for Rand

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz


//  Mainboard01. BotNetId 21 = 0x15
//
//  PB09  -> Status Led
//
//
//   Switch:
//      PA08: Solid State Enable SSEN
//
//      PA00: LED1 (TIM2_CH1, AF1)
//      PA01: LED2 (TIM2_CH2, AF1)
//      PA02: LED3 (TIM2_CH3, AF1)
//      PA06: Led_Sig (e.g. WS2812)
//
//
//  I2C1  -> BotNet
//    PB06  -> I2C1 SCL
//    PB07  -> I2C1 SDA
//
//  U1    -> BotNet
//    PA09  -> U1 Tx/Rx
//
//
//  I2C2  -> INA3221, Temperatursensoren
//    PB10  -> I2C2 SCL AF4
//    PB14  -> I2C2 SDA AF4
//
//
//  Timer Usage:
//    TIM2  -> PWM für LED1..3 <-> DiffTimer (optional)
//    TIM6  -> CyclicCaller
//    TIM15 -> BotCom MpHd
//    TIM16 -> PWM für WS2812 vir DMA1.Chl3
//
//  DMA Usage:
//    DMA1:
//      0:
//      1:
//      2:
//      3: TIM16 für WS2812
//      4: I2C2 Tx: Mainboard  CS:3
//      5: I2C2 Rx: Mainboard  CS:3
//      6: I2C1 Tx: BotNet CS:3 / Usart2 Rx BotNet CS:2
//      7: I2C1 Rx: BotNet CS:3 / Usart2 Tx BotNet CS:2
//
//    DMA2:
//      0:
//      1:
//      2:
//      3:
//      4:
//      5:
//      6: I2C1 Rx: BotNet CS:5 / Usart1 Tx BotNet CS:2
//      7: I2C1 Tx: BotNet CS:5 / Usart1 Rx BotNet CS:2
//
//  Interrupt Usage:
//    TIM6_DAC_IRQHandler:      CyclicCaller:  Prio: 8.8
//    I2C1_EV_IRQHandler:       BotCom:        Prio: 8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio: 8.8
//    I2C2_EV_IRQHandler:       Board:         Prio: 8.8
//    I2C2_ER_IRQHandler:       Board:         Prio: 8.8
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio: 6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio: 6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio: 6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio: 6.8 => U1
//
// Backup-Register Usage:
//    00: Clock Valid by Clock
//    01: Wakeup Counter
//


// Call first to get the system clocks
// This is even called before the inlcuded classs in the header
cClockInfo mcClockInfo;

#define LEDMAXLEDCNT 256
u16 mu16LedCnt = LEDMAXLEDCNT;
u8 mu8MyLedRgbMemory[LEDCTRL_RGB_MEMSIZE(LEDMAXLEDCNT)];
u8 mu8MyLedBitMemory[LEDCTRL_LED_MEMSIZE(LEDMAXLEDCNT)];
cWs2812 mcWs2812(mu8MyLedRgbMemory, mu8MyLedBitMemory, LEDMAXLEDCNT);


class  __attribute__((__packed__)) cData
{
public:

  typedef struct
  {
    u32  u32ID;
    u32  u32Cnt;
    u8   Res1[32 - 4 - 4];
    // 32

    u16  u16Brigthness;
    u16  u16LedAnimationIdx;
    u16  u16StartTimerReload;
    u16  u16LedBlockCnt;
    u16  u16XLedBlockCnt;
    u16  u16LedLedsPerBlock;
    u8   Res2[32 - 2 - 2 - 2 - 2 - 2 - 2];
    // 64

    u32  u32PirOnTime;
    u8   bPirEnable;
    u8   u8RotInv;
    i16  i16MaxTemp;
    u8   Res3[32 - 4 - 1 - 1 - 2];
    // 96

    u8   u8CustR;
    u8   u8CustG;
    u8   u8CustB;
    u8   Res4[32 - 1 - 1 - 1];
    // 128

    u16  u8TempAdc1;
    u16  u8TempAdc2;
    u8   Res5[32 - 2 - 2];
    // 160

  }tstData;

  tstData mData;

  cData()
  {
    vSetDefault();
  }

  void vSetDefault()
  {
    mData.u32Cnt              = 0;
    mData.bPirEnable          = True;
    mData.u8RotInv            = 0;
    mData.u32PirOnTime        = 1000 * 60 * 10; /* 10min = 10 * 60 * 1000ms */;
    mData.i16MaxTemp          = 80;   // °C
    mData.u16Brigthness       = 0; // 5%
    mData.u16LedAnimationIdx  = 0;
    mData.u16StartTimerReload = 1000 * 5; // 5s
    mData.u16LedBlockCnt      = 4;
    mData.u16XLedBlockCnt     = 8;
    mData.u16LedLedsPerBlock  = 64;
    mData.u8CustR             = 255;
    mData.u8CustG             = 255;
    mData.u8CustB             = 255;
    mData.u8TempAdc1          = 100;
    mData.u8TempAdc2          =  77;
  }

  void vCheckAndSetDefault()
  {
    if (mData.u32ID != 0x12345678)
    {
      // ROM Defaults
      mData.u32ID               = 0x12345678;
      vSetDefault();
      vStore();
    }
  }

  virtual void vStore() = 0;
};

class cRomData : public cData
{
public:
  bool     mbLoad;
  bool     mbError;

  cRomData()
  {
    mbLoad  = False;
    mbError = False;
  }


  void vStore() override
  {
  }

  void vUpdate()
  {
  }

  void vTick10ms()
  {
  }
};

cRomData mcData;

class cLedAnimation
{
  public:
  cWs2812* mcWs2812;
  i16      mi16Value;
  char     mcStrData[12];
  cStr     mcName;

  cLedAnimation(cWs2812* lcWs2812)
    : mcName(mcStrData, sizeof(mcStrData))
  {
    mcWs2812  = lcWs2812;
    mi16Value = 0;
    mcName    = (rsz)"LedAnimTpl";
  }

  void vLedXY(u8 lu8Block, u8 x, u8 y, u8 r, u8 g, u8 b)
  {
    u16 Idx = y * mcData.mData.u16XLedBlockCnt + x + mcData.mData.u16LedLedsPerBlock * lu8Block;
    mcWs2812->vSetLedPxl(Idx, r, g, b);
  }

  virtual void vSetValue(i16 li16Value)
  {
    mi16Value = li16Value;
  }

  void vRect(u8 lu8Block, u8 x1, u8 y1, u8 x2, u8 y2, u8 r, u8 g, u8 b)
  {
    u8 x, y;

    for (x = x1; x <= x2; x++)
    {
      vLedXY(lu8Block, x, y1, r, g, b);
      vLedXY(lu8Block, x, y2, r, g, b);
    }

    for (y = (y1 + 1); y <= (y2 - 1); y++)
    {
      vLedXY(lu8Block, x1, y, r, g, b);
      vLedXY(lu8Block, x2, y, r, g, b);
    }
  }

  virtual void vInit()
  {
  }

  virtual void vProcess10ms()
  {
  }
};



class cLedAnimation_Static : public cLedAnimation
{
public:
  u8 R, G, B;
  cLedAnimation_Static(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB, rsz lszName)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;
    mcName = lszName;
  }

  void vInit() override
  {
    for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
  }

  void vProcess10ms() override
  {
  }
};

class cLedAnimation_StaticRgb : public cLedAnimation_Static
{
public:
  cLedAnimation_StaticRgb(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"JustRgb")
  {
  }

  void vInit() override
  {
    R = mcData.mData.u8CustR;
    G = mcData.mData.u8CustG;
    B = mcData.mData.u8CustB;
    cLedAnimation_Static::vInit();
  }
};

class cLedAnimation_Alarm : public cLedAnimation
{
public:
  u8   R, G, B;
  u8   mu8LineIdx;
  u16  mu16Timer;
  cLedAnimation_Alarm(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB, rsz lszName)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;
    mcName = lszName;
  }


  void vInit() override
  {
    mu16Timer = 1000;
    vProcess10ms();
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 120)
    {
      mu16Timer = 0;
      mu8LineIdx++;

      if ((mu8LineIdx & 1) == 0)
      {
        for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
        {
          mcWs2812->vSetLedPxl(lu16i, R, G, B);
        }
      }
      else
      {
        for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
        {
          mcWs2812->vSetLedPxl(lu16i, 0, 0, 0);
        }
      }
    }
  }
};


class cLedAnimation_AlarmRgb : public cLedAnimation_Alarm
{
public:
  cLedAnimation_AlarmRgb(cWs2812* lcWs2812)
    : cLedAnimation_Alarm(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"AlarmRgb")
  {
  }

  void vInit() override
  {
    R = mcData.mData.u8CustR;
    G = mcData.mData.u8CustG;
    B = mcData.mData.u8CustB;
    cLedAnimation_Alarm::vInit();
  }
};


class cLedAnimation_Warp : public cLedAnimation
{
public:
  u8   R, G, B;
  i16  mi16PxlIdx;
  u16  mu16Timer;
  cLedAnimation_Warp(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB, rsz lszName)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;

    mcName = lszName;
  }

  void vInit() override
  {
    mi16PxlIdx = 0;
    mu16Timer = 1000;

    vProcess10ms();
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 100)
    {

      mu16Timer = 0;

      for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
      {
        mcWs2812->vSetLedPxl(lu16i, R / 8, G / 8, B / 8);
      }

      asm volatile ("NOP");
      if (mi16PxlIdx < 4)
      {
        for (u8 lu8BlockIdx = 0; lu8BlockIdx < mcData.mData.u16LedBlockCnt; lu8BlockIdx++)
        {
          vRect(lu8BlockIdx, 3 - mi16PxlIdx, 3 - mi16PxlIdx, 4 + mi16PxlIdx, 4 + mi16PxlIdx, R, G, B);
        }
      }

      mi16PxlIdx++;
      if (mi16PxlIdx >= (4 * 3 / 2))
      {
        mi16PxlIdx = 0;
      }
    }
  }
};


class cLedAnimation_WarpRgb : public cLedAnimation_Warp
{
public:
  cLedAnimation_WarpRgb(cWs2812* lcWs2812)
    : cLedAnimation_Warp(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"WarpRgb")
  {
  }

  void vInit() override
  {
    R = mcData.mData.u8CustR;
    G = mcData.mData.u8CustG;
    B = mcData.mData.u8CustB;
    cLedAnimation_Warp::vInit();
  }
};

class cLedAnimation_Bunt1Rgb : public cLedAnimation
{
public:
  u8   R[LEDMAXLEDCNT], G[LEDMAXLEDCNT], B[LEDMAXLEDCNT];
  u8   SetR[LEDMAXLEDCNT], SetG[LEDMAXLEDCNT], SetB[LEDMAXLEDCNT];
  i8   IncR[LEDMAXLEDCNT], IncG[LEDMAXLEDCNT], IncB[LEDMAXLEDCNT];

  cLedAnimation_Bunt1Rgb(cWs2812* lcWs2812)
    : cLedAnimation(lcWs2812)
  {
    mcName = (rsz)"Bunt1Rgb";
  }

  void vRandCol()
  {
    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      SetR[lu16Idx] = rand() % mcData.mData.u8CustR;
      SetG[lu16Idx] = rand() % mcData.mData.u8CustG;
      SetB[lu16Idx] = rand() % mcData.mData.u8CustB;

      IncR[lu16Idx] = (SetR[lu16Idx] - R[lu16Idx]) / 32;
      IncG[lu16Idx] = (SetG[lu16Idx] - G[lu16Idx]) / 32;
      IncB[lu16Idx] = (SetB[lu16Idx] - B[lu16Idx]) / 32;
    }
  }

  void vShow()
  {
    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
  }

  void vInit() override
  {
    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      R[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 0];
      G[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 1];
      B[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 2];
    }
    vRandCol();
  }

  u8 u8Dim(i16 Col, i16 SetCol, i16 IncCol)
  {
    if (Col != SetCol)
    {
      if (IncCol == 0)
      {
        Col = SetCol;
      }
      else
      {
        if (IncCol > 0)
        {
          if ((Col + IncCol) > SetCol)
          {
            Col = SetCol;
          }
          else
          {
            Col += IncCol;
          }
        }
        else
        {
          if ((Col + IncCol) < SetCol)
          {
            Col = SetCol;
          }
          else
          {
            Col += IncCol;
          }
        }
      }
    }
    return (u8)Col;
  }

  void vProcess10ms() override
  {
    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      R[lu16Idx] = u8Dim(R[lu16Idx], SetR[lu16Idx], IncR[lu16Idx]);
      G[lu16Idx] = u8Dim(G[lu16Idx], SetG[lu16Idx], IncG[lu16Idx]);
      B[lu16Idx] = u8Dim(B[lu16Idx], SetB[lu16Idx], IncB[lu16Idx]);

      if ((R[lu16Idx] == SetR[lu16Idx]) &&
          (G[lu16Idx] == SetG[lu16Idx]) &&
          (B[lu16Idx] == SetB[lu16Idx]))
      {
        SetR[lu16Idx] = rand() % mcData.mData.u8CustR;
        SetG[lu16Idx] = rand() % mcData.mData.u8CustG;
        SetB[lu16Idx] = rand() % mcData.mData.u8CustB;

        IncR[lu16Idx] = (SetR[lu16Idx] - R[lu16Idx]) / 32;
        IncG[lu16Idx] = (SetG[lu16Idx] - G[lu16Idx]) / 32;
        IncB[lu16Idx] = (SetB[lu16Idx] - B[lu16Idx]) / 32;
      }
    }

    vShow();
  }
};


class cLedAnimation_Bunt2Rgb : public cLedAnimation
{
public:
  u8   R[LEDMAXLEDCNT], G[LEDMAXLEDCNT], B[LEDMAXLEDCNT];
  i8   IncR[LEDMAXLEDCNT], IncG[LEDMAXLEDCNT], IncB[LEDMAXLEDCNT];

  u16  mu16Timer;

  cLedAnimation_Bunt2Rgb(cWs2812* lcWs2812)
    : cLedAnimation(lcWs2812)
  {
    mcName = (rsz)"Bunt2Rgb";
  }

  void vShow()
  {
    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
  }

  void vInit() override
  {
    mu16Timer = 0;

    for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
    {
      R[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 0];
      G[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 1];
      B[lu16Idx] = mcWs2812->mu8GfxData[(lu16Idx * 3) + 2];

      IncR[lu16Idx] = -R[lu16Idx] / 32;
      IncG[lu16Idx] = -G[lu16Idx] / 32;
      IncB[lu16Idx] = -B[lu16Idx] / 32;
    }
  }

  u8 u8Dim(i16 Col, i16 IncCol)
  {
    if (Col > 0)
    {
      if (IncCol == 0)
      {
        Col = 0;
      }
      else
      {
        if ((Col + IncCol) < 0)
        {
          Col = 0;
        }
        else
        {
          Col += IncCol;
        }
      }
    }
    return (u8)Col;
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 40)
    {
      mu16Timer = 0;

      for (u16 lu16Idx = 0; lu16Idx < mu16LedCnt; lu16Idx++)
      {
        R[lu16Idx] = u8Dim(R[lu16Idx], IncR[lu16Idx]);
        G[lu16Idx] = u8Dim(G[lu16Idx], IncG[lu16Idx]);
        B[lu16Idx] = u8Dim(B[lu16Idx], IncB[lu16Idx]);

        if ((R[lu16Idx] == 0) &&
            (G[lu16Idx] == 0) &&
            (B[lu16Idx] == 0))
        {
          R[lu16Idx] = rand() % mcData.mData.u8CustR;
          G[lu16Idx] = rand() % mcData.mData.u8CustG;
          B[lu16Idx] = rand() % mcData.mData.u8CustB;

          IncR[lu16Idx] = -R[lu16Idx] / 32;
          IncG[lu16Idx] = -G[lu16Idx] / 32;
          IncB[lu16Idx] = -B[lu16Idx] / 32;
        }
      }

      vShow();
    }
  }
};

class cLedAnimation_Bunt3Rgb : public cLedAnimation_Static
{
public:
  i16 SetR, SetG, SetB;
  i8  IncR, IncG, IncB;
  u16 mu16Timer;
  u16 Cnt;
  cLedAnimation_Bunt3Rgb(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"Bunt3Rgb")
  {
  }

  void vShow()
  {
    for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
  }

  void vInit() override
  {
    mu16Timer = 0;
    Cnt = 0;
    R = mcData.mData.u8CustR;
    G = mcData.mData.u8CustG;
    B = mcData.mData.u8CustB;
    vRand();
  }

  void vRand()
  {
    u8 t;
    t = 10;
    do
    {
      u8 lu8Rand;
      lu8Rand = rand() & 3;
      if (lu8Rand)
        SetR = mcData.mData.u8CustR / lu8Rand;
      else
        SetR = 0;

      lu8Rand = rand() & 3;
      if (lu8Rand)
        SetG = mcData.mData.u8CustG / lu8Rand;
      else
        SetG = 0;

      lu8Rand = rand() & 3;
      if (lu8Rand)
        SetB = mcData.mData.u8CustB / lu8Rand;
      else
        SetB = 0;

      t--;
    } while ((t > 0) && (SetR == 0) && (SetG == 0) && (SetB == 0));

    IncR = (SetR - R) / 32;
    IncG = (SetG - G) / 32;
    IncB = (SetB - B) / 32;
  }

  u8 u8Dim(i16 Col, i16 IncCol, i16 SetCol)
  {
    if (IncCol == 0)
    {
      Col = SetCol;
    }
    else
    {
      if (IncCol < 0)
      {
        if ((Col + IncCol) < SetCol)
        {
          Col = SetCol;
        }
        else
        {
          Col += IncCol;
        }
      }
      else
      {
        if ((Col + IncCol) > SetCol)
        {
          Col = SetCol;
        }
        else
        {
          Col += IncCol;
        }
      }
    }
    return (u8)Col;
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 120)
    {
      Cnt++;
      if (Cnt > 400)
      {
        Cnt = 0;
        vRand();
      }

      R = u8Dim(R, IncR, SetR);
      G = u8Dim(G, IncG, SetG);
      B = u8Dim(B, IncB, SetB);

      vShow();
    }
  }
};

class cLedAnimation_PulseRgb : public cLedAnimation_Static
{
public:
  i8 IncR, IncG, IncB;
  i8 Dir;
  u16  mu16Timer;
  cLedAnimation_PulseRgb(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"PulseRgb")
  {
  }

  void vShow()
  {
    for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
  }

  void vInit() override
  {
    mu16Timer = 0;

    R = mcData.mData.u8CustR;
    G = mcData.mData.u8CustG;
    B = mcData.mData.u8CustB;

    IncR = R / 32;
    IncG = G / 32;
    IncB = B / 32;
    Dir = -1;
  }

  u8 u8Dim(i16 Col, i16 IncCol, i16 SetCol)
  {
    if (IncCol == 0)
    {
      Col = SetCol;
    }
    else
    {
      if (IncCol < 0)
      {
        if ((Col + IncCol) < SetCol)
        {
          Col = SetCol;
        }
        else
        {
          Col += IncCol;
        }
      }
      else
      {
        if ((Col + IncCol) > SetCol)
        {
          Col = SetCol;
        }
        else
        {
          Col += IncCol;
        }
      }
    }
    return (u8)Col;
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 30)
    {
      mu16Timer = 0;
      if (Dir < 0)
      {
        R = u8Dim(R, -IncR, 8);
        G = u8Dim(G, -IncG, 8);
        B = u8Dim(B, -IncB, 8);

        if ((R == 8) &&
          (G == 8) &&
          (B == 8))
        {
          Dir = 1;
        }
      }
      else
      {
        R = u8Dim(R, IncR, mcData.mData.u8CustR);
        G = u8Dim(G, IncG, mcData.mData.u8CustG);
        B = u8Dim(B, IncB, mcData.mData.u8CustB);

        if ((R == mcData.mData.u8CustR) &&
            (G == mcData.mData.u8CustG) &&
            (B == mcData.mData.u8CustB))
        {
          Dir = -1;
        }
      }

      vShow();
    }
  }
};

class cLedAnimation_LampeRgb : public cLedAnimation_Static
{
public:
  cLedAnimation_LampeRgb(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"Lampe")
  {
  }

  void vInit() override
  {
    cLedAnimation_Static::vInit();
  }

  void vSetValue(i16 li16Value) override
  {
    mi16Value = li16Value;

    if      (mi16Value <  80) { R = 255; G =   0; B =   0;}
    else if (mi16Value < 100) { R = 255; G =  64; B =  64;}
    else if (mi16Value < 120) { R = 255; G = 128; B = 128;}
    else if (mi16Value < 140) { R = 255; G = 192; B = 192;}
    else                      { R = 255; G = 255; B = 255;}

    for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
  }

  void vProcess10ms() override
  {
    cLedAnimation_Static::vProcess10ms();
  }
};

cLedAnimation_Static    mcLA_JustWhite(    &mcWs2812, 255, 255, 255, (rsz)"JustWhite");
cLedAnimation_Static    mcLA_JustRed(      &mcWs2812, 255, 0,   0  , (rsz)"JustRed");
cLedAnimation_Static    mcLA_JustGreen(    &mcWs2812, 0,   255, 0  , (rsz)"JustGreen");
cLedAnimation_Static    mcLA_JustBlue(     &mcWs2812, 0,   0,   255, (rsz)"JustBlue");
cLedAnimation_Static    mcLA_JustYellow(   &mcWs2812, 255, 255, 0  , (rsz)"JustYellow");
cLedAnimation_Static    mcLA_JustPink(     &mcWs2812, 255, 0,   255, (rsz)"JustPink");
cLedAnimation_Static    mcLA_JustTurquoise(&mcWs2812, 0,   255, 255, (rsz)"JustOcean");
cLedAnimation_Alarm     mcLA_AlarmBlue(&mcWs2812, 0, 0, 255, (rsz)"AlarmBlue");
cLedAnimation_AlarmRgb  mcLA_AlarmRgb(&mcWs2812);
cLedAnimation_StaticRgb mcLA_JustRgb(&mcWs2812);
cLedAnimation_Warp      mcLA_WarpRed(&mcWs2812, 255, 0, 0, (rsz)"WarpRed");
cLedAnimation_WarpRgb   mcLA_WarpRgb(&mcWs2812);
cLedAnimation_Bunt1Rgb  mcLA_Bunt1Rgb(&mcWs2812);
cLedAnimation_Bunt2Rgb  mcLA_Bunt2Rgb(&mcWs2812);
cLedAnimation_Bunt3Rgb  mcLA_Bunt3Rgb(&mcWs2812);
cLedAnimation_PulseRgb  mcLA_PulseRgb(&mcWs2812);
cLedAnimation_LampeRgb  mcLA_LampeRgb(&mcWs2812);


cLedAnimation *mLstLedAnims[] = { &mcLA_JustWhite,
                                  &mcLA_JustRed,
                                  &mcLA_JustGreen,
                                  &mcLA_JustBlue,
                                  &mcLA_JustYellow,
                                  &mcLA_JustPink,
                                  &mcLA_JustTurquoise,
                                  &mcLA_JustRgb,
                                  &mcLA_AlarmBlue,
                                  &mcLA_AlarmRgb,
                                  &mcLA_WarpRed,
                                  &mcLA_WarpRgb,
                                  &mcLA_Bunt1Rgb,
                                  &mcLA_Bunt2Rgb,
                                  &mcLA_Bunt3Rgb,
                                  &mcLA_PulseRgb,
                                  &mcLA_LampeRgb,
                                  NULL
                                };


u16 mu16LedAnimationIdx = 0;

class cTempSensor
{
public:
  i16 mi16Tmp[4];

  cTempSensor()
  {
    mi16Tmp[0] = 24;
    mi16Tmp[1] = 24;
    mi16Tmp[2] = 24;
    mi16Tmp[3] = 24;
  }

  i16 i16GetTemp(u8 lu8Idx)
  {
    return mi16Tmp[lu8Idx];
  }
};

cTempSensor mcTemp;

class cCliCmd_HwStatus : public cCliCmd
{
public:
  cCliCmd_HwStatus()
    : cCliCmd((const char*)"hwstatus", (const char*)"Hw-Status")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
  {
    cStr_Create(lszStr, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    if (lbFirstCall)
    {
      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Switches:");
      lszStr.Setf((rsz)"Relais: %d", (u16)mcSys.mcBoard.mcBoardCntrl.lenSmS[cBoardCtrl::enS_Ch220V]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch1: %d", (u16)mcWs2812.u8GetPwm(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch2: %d", (u16)mcWs2812.u8GetPwm(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch3: %d", (u16)mcWs2812.u8GetPwm(3)); lcCli->bPrintLn(lszStr);
      return False;
    }
    else
    {
      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Ina:");
      lszStr.Setf((rsz)"Ch1: %d mV -> %d mA", (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(1), (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch2: %d mV -> %d mA", (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(2), (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch3: %d mV -> %d mA", (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(3), (i16)mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(3)); lcCli->bPrintLn(lszStr);


      lcCli->bPrintLn((rsz)"");
      lszStr.Setf((const char8*)"Error: ");
      lcCli->bPrintLn(mcSys.mszErrorInfo);

      lcCli->bPrintLn((rsz)"");
      lszStr.Setf((const char8*)"Mode %s (%d)", mcSys.mcSMan.mcOpMode.ToString(), mcSys.mcSMan.mcOpMode.mCounter);
      lcCli->bPrintLn(lszStr);
      return True;
    }
  }
};

class cCliCmd_LedAnimList : public cCliCmd
{
  u16 mu16Cnt;

public:
  cCliCmd_LedAnimList() :cCliCmd((const char*)"LedAnimList", (const char*)"Shows list of LED Animations") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);

    cStr_Create(lszStr, 32);

    if (lbFirstCall)
    {
      mu16Cnt = 0;
    }

    while (mLstLedAnims[mu16Cnt - 0] != NULL)
    {
      if (lcCli->isSpace(20))
      {
        if ((mu16Cnt) == mu16LedAnimationIdx)
        {
          lcCli->bPrint((rsz)">> ");
        }

        lszStr.Setf((rsz)"%d. ", mu16Cnt + 1);
        lszStr += mLstLedAnims[mu16Cnt]->mcName;
        lcCli->bPrintLn(lszStr);
      }
      else
      {
        return False;
      }
      mu16Cnt++;
    }
    return True;
  }
};

class cCliCmd_Status_Led : public cCliCmd
{
  u16 mu16Calls;

public:
  cCliCmd_Status_Led() :cCliCmd((const char*)"Status", (const char*)"Shows Status") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);

    cStr_Create(lszStr, 32);

    if (lbFirstCall)
    {
      mu16Calls = 0;
    }

    if (lcCli->isSpace(100))
    {
      switch (mu16Calls)
      {
        case 0:
          lcCli->bPrintLn((rsz)"Led Status:");
          lszStr.Setf((rsz)"  AnimIdx: %d", mu16LedAnimationIdx + 1); lcCli->bPrintLn(lszStr);
          lszStr =    (rsz)"  Anim:    "; lszStr += mLstLedAnims[mu16LedAnimationIdx]->mcName; lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Illu:    %d[%%]", (u32)(mcWs2812.mu8Brigthness * 100u) / 255u); lcCli->bPrintLn(lszStr);
          //lszStr.Setf((rsz)"  DTemp:   %d[°C]", mcTmp.i16GetTemp()); lcCli->bPrintLn(lszStr);
          break;
        case 1:
          lcCli->bPrintLn((rsz)"Cpu Status:");
          lszStr.Setf((rsz)"  Core:      %d[Mhz]", cClockInfo::mstClocks.HCLK_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  PClk1:     %d[Mhz]", cClockInfo::mstClocks.PCLK1_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  PClk2:     %d[Mhz]", cClockInfo::mstClocks.PCLK2_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          return True;
          break;
      }
      mu16Calls++;
    }
    return False;
  }
};

class cCliCmd_AnimIdx : public cCliCmd
{
  u16 mu16Calls;

public:
  cCliCmd_AnimIdx() :cCliCmd((const char*)"AnimIdx", (const char*)"Set/Get AnimIdx") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      u16 lu16NewIdx = lcParam.Atoi();

      if ((lu16NewIdx < 1) ||  (lu16NewIdx >= (sizeof(mLstLedAnims) / sizeof(cLedAnimation*))))
      {
        // Wrong Input
        lcCli->bPrintLn((rsz)"  !!! Wrong Input");
      }
      else
      {
        lcCli->bPrintLn((rsz)"  Ok");
        if (mu16LedAnimationIdx != (lu16NewIdx - 1))
        {
          mcData.vUpdate();
          mu16LedAnimationIdx = lu16NewIdx - 1;
          mLstLedAnims[mu16LedAnimationIdx]->vInit();
        }
      }
    }

    lszStr.Setf((rsz)"AnimIdx: %d", mu16LedAnimationIdx + 1); lcCli->bPrintLn(lszStr);
    lszStr = (rsz)"Anim:    "; lszStr += mLstLedAnims[mu16LedAnimationIdx]->mcName; lcCli->bPrintLn(lszStr);

    return True;
  }
};

class cCliCmd_Rgb : public cCliCmd
{
public:
  cCliCmd_Rgb() :cCliCmd((const char*)"Rgb", (const char*)"Set/Get custom RGB: rgb r,g,b") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      if (lcParam.CountOf(',') == 2)
      {
        u16 lu16PosKomma1 = lcParam.Instr(0, ',');
        u16 lu16PosKomma2 = lcParam.Instr(lu16PosKomma1 + 1, ',');

        if ((lu16PosKomma1 != (u16)-1) && (lu16PosKomma2 != (u16)-1))
        {
          lszStr = lcParam;
          lszStr.Mid(0, lu16PosKomma1 - 0);
          u16 lu16R = lszStr.Atoi();

          lszStr = lcParam;
          lszStr.Mid(lu16PosKomma1 + 1, lu16PosKomma2 - lu16PosKomma1 - 1);
          u16 lu16G = lszStr.Atoi();

          lszStr = lcParam;
          lszStr.Mid(lu16PosKomma2 + 1, 0xFFFF);
          u16 lu16B = lszStr.Atoi();

          mcData.mData.u8CustR = lu16R;
          mcData.mData.u8CustG = lu16G;
          mcData.mData.u8CustB = lu16B;
          mcData.vUpdate();
          mLstLedAnims[mu16LedAnimationIdx]->vInit();
        }
      }
    }

    lszStr.Setf((rsz)"  RGB: %X (%d,%d,%d)", (mcData.mData.u8CustR << 16) + (mcData.mData.u8CustG << 8) + mcData.mData.u8CustB, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB); lcCli->bPrintLn(lszStr);

    return True;
  }
};

cCliCmd_HwStatus     mcCliCmd_HwStatus;
cCliCmd_LedAnimList  mcCliCmd_LedAnimList;
cCliCmd_Status_Led   mcCliCmd_Status_Led;
cCliCmd_AnimIdx      mcCliCmd_AnimIdx;
cCliCmd_Rgb          mcCliCmd_Rgb;

cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_HwStatus,    &mcCliCmd_LedAnimList,
                                           &mcCliCmd_Status_Led,  &mcCliCmd_AnimIdx,
                                           &mcCliCmd_Rgb}, 5);


class cBnMsgHandlerApp : public cBotNet_MsgSysProcess
{
public:
  cBnMsgHandlerApp()
    : cBotNet_MsgSysProcess(&mcSys.mcCom.mcBn)
  {
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    bool lbConsumed = False;

    switch (lcMsg.mu16Idx)
    {
      case 32: // Request message
        switch (lcMsg.mcPayload[0])
        {
          case 2: // Power
            // Power.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[19];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[ 0] = 2; // R1
              lu8Data[ 1] = 0; // S1
              lu8Data[ 2] = 0; // S2

              lu8Data[ 3] = mcSys.mcBoard.mcBoardCntrl.lenSmS[cBoardCtrl::enS_Ch220V];

              lu8Data[ 4] = mcWs2812.u8GetPwm(1);
              lu8Data[ 5] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(1) >> 8;
              lu8Data[ 6] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(1) & 0xFF;
              lu8Data[ 7] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(1) >> 8;
              lu8Data[ 8] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(1) & 0xFF;

              lu8Data[ 9] = mcWs2812.u8GetPwm(2);
              lu8Data[10] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(2) >> 8;
              lu8Data[11] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(2) & 0xFF;
              lu8Data[12] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(2) >> 8;
              lu8Data[13] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(2) & 0xFF;

              lu8Data[14] = mcWs2812.u8GetPwm(3);
              lu8Data[15] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(3) >> 8;
              lu8Data[16] = mcSys.mcBoard.mcBoardCntrl.i16GetChVoltageAbs(3) & 0xFF;
              lu8Data[17] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(3) >> 8;
              lu8Data[18] = mcSys.mcBoard.mcBoardCntrl.i16GetChCurrentAbs(3) & 0xFF;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
        }
        break;

      case 34: // Set message
        switch (lcMsg.mcPayload[0])
        {
          case 2: // Power
            // Power.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              if (lcMsg.mcPayload[3] <= 1) mcSys.mcBoard.mcBoardCntrl.vSetRelais(lcMsg.mcPayload[3]);
              mcWs2812.vSetPwmLed1(lcMsg.mcPayload[4]);
              mcWs2812.vSetPwmLed2(lcMsg.mcPayload[5]);
              mcWs2812.vSetPwmLed3(lcMsg.mcPayload[6]);

              lbConsumed = True;
            }
            break;
        }
        break;

      // --------------------------- SLED Messages -----------------------------
      case 40: // Request message
        switch (lcMsg.mcPayload[0])
        {
          case 0: // SLED.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[11];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 41);

              lu8Data[0] = 0; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 0; // S2

              if (mcWs2812.u8GetPwm(1) > 0)
              {
                lu8Data[3] = 1;
              }
              else
              {
                lu8Data[3] = 0;
              }

              lu8Data[ 4] = mcData.mData.u16Brigthness;
              lu8Data[ 5] = mcData.mData.u16LedAnimationIdx;
              lu8Data[ 6] = mcData.mData.u8CustR;
              lu8Data[ 7] = mcData.mData.u8CustG;
              lu8Data[ 8] = mcData.mData.u8CustB;
              lu8Data[ 9] = mcTemp.i16GetTemp(0) >> 8;
              lu8Data[10] = mcTemp.i16GetTemp(0) & 0xFF;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
        }
        break;

      case 42: // Set message
        //  Set                  TX 00 | 00 | 00 | ES.P1.AI.RR.GG.BB.TH.TL //  [3] ES: Enable status LEDs
        //                                                                 //  [4] Px: Channel x PWM/Dimming (0..100%);
        //                                                                 //  [5] AI: Animation Index
        //                                                                 //  [6] RGB: RGB value
        //                                                                 //  [7] TH.TL: max. Temperature value (°C,  int16)
        switch (lcMsg.mcPayload[0])
        {
          case 0: // SLED.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Brightness;
              u8 lu8AnimCnt;

              lu8Brightness = lcMsg.mcPayload[3];
              if (lu8Brightness > 1) lu8Brightness = 1;

              mcData.mData.u16Brigthness = lcMsg.mcPayload[4] * lu8Brightness;

              lu8AnimCnt = ((sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1);

              mcData.mData.u16LedAnimationIdx = lcMsg.mcPayload[5] % lu8AnimCnt;
              mcData.mData.u8CustR            = lcMsg.mcPayload[6];
              mcData.mData.u8CustG            = lcMsg.mcPayload[7];
              mcData.mData.u8CustB            = lcMsg.mcPayload[8];
              mcData.mData.i16MaxTemp         = (lcMsg.mcPayload[9] << 8) + lcMsg.mcPayload[10];

              if (mu16LedAnimationIdx != mcData.mData.u16LedAnimationIdx)
              {
                mu16LedAnimationIdx = mcData.mData.u16LedAnimationIdx;
                mLstLedAnims[mu16LedAnimationIdx]->vInit();
              }
              mLstLedAnims[mu16LedAnimationIdx]->vSetValue(mcData.mData.u16Brigthness);

              mcData.vUpdate();

              lbConsumed = True;
            }
            break;
        }
        break;
    }

    return lbConsumed;
  }
};

cBnMsgHandlerApp mcBnMsgHandlerApp;

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

// ---------------------------- WS2812 ---------------------------

void DMA1_Channel3_IRQHandler(void)
{
  mcWs2812.Dma_vIrqTc();
}


void MAIN_vTick1msHp(void)
{
  mcSys.vTick1msHp();
}


void MAIN_vTick10msLp();

void MAIN_vTick1msLp(void)
{
  static u8 lu8Cnt1ms = 0;

  mcSys.vTick1msLp();

  lu8Cnt1ms++;
  if (lu8Cnt1ms == 10)
  {
    lu8Cnt1ms = 0;
    MAIN_vTick10msLp();
  }
}


void MAIN_vTick10msLp()
{
  static volatile u8 lu8BrightnessOld = 128;
  static u16 lu16Minute     = 0;
  static i16 li16TempAlarm  = 0;
  static u16 lu16IsOn       = 1;
  static u16 mu16AnimIdxOld = 255;

  mcWs2812.vTick10msLp();

  if (mu16LedAnimationIdx != mu16AnimIdxOld)
  {
    mu16AnimIdxOld = mu16LedAnimationIdx;
    mLstLedAnims[mu16LedAnimationIdx]->vInit();
    mcData.vUpdate();
  }

  u8 lu8Brightness;

  lu8Brightness = mcData.mData.u16Brigthness;

  lu8Brightness *= lu16IsOn;

  lu16Minute++;
  if (lu16Minute > 6000)
  {
    lu16Minute = 0;

    // Einfache Temperatur Regelung
    // if (mcWs2812.i16GetNtcTemp() > 70) // analog
    if (mcTemp.i16GetTemp(0) > mcData.mData.i16MaxTemp)  // digital
    {
      li16TempAlarm -= 5;
      if (li16TempAlarm < -250) li16TempAlarm -= 250;
    }
    else
    {
      li16TempAlarm += 5;
      if (li16TempAlarm > 0) li16TempAlarm = 0;
    }
    if (lu8BrightnessOld != lu8Brightness) // Benutzer hat Helligkeit verändert
    {
      li16TempAlarm = 0;
    }
  }

  if ((lu8BrightnessOld != lu8Brightness) || (lu16Minute == 0))
  {
    mcWs2812.vSetBrigthness(lu8Brightness + li16TempAlarm);
    lu8BrightnessOld = lu8Brightness;
  }

  if (lu8Brightness) mcWs2812.vEnable();
                else mcWs2812.vDisable();
  mLstLedAnims[mu16LedAnimationIdx]->vProcess10ms();
  mcWs2812.vShow();
  mcData.vTick10ms();
}


void MAIN_vInitSystem(void)
{
  // Warten bis Eep geladen wurde
  // Init Callback nur um Eep zu laden
  mcWs2812.vDisable();

  mcSys.vInit1();

  mcSys.mcCom.mcBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);
  mcBnMsgHandlerApp.vAddMsgSys();

  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  // ------ EEP-Daten laden
  // Im Fehlerfall nochmals ein paar mal probieren, weil ...
  //    Time required after VCC is stable before the device can accept commands. 100 μs
  u8 lu8EepRetrys = 10;
  while ((mcData.mbError) && (lu8EepRetrys > 0))
  {
    mcData.mbError = False;
    while ((mcData.mbLoad) && (!mcData.mbError))
    {
      CycCall_vIdle();
    }
    lu8EepRetrys--;
  }

  // ----- Daten setzen
  mu16LedAnimationIdx = 0;
  if (mu16LedAnimationIdx > (sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1)
  {
    mu16LedAnimationIdx = 0;
  }

  if ((mcData.mData.u16LedBlockCnt < 1) || (mcData.mData.u16LedBlockCnt > 6)) mcData.mData.u16LedBlockCnt = 1;
  mu16LedCnt = mcData.mData.u16LedBlockCnt * mcData.mData.u16LedLedsPerBlock;

  //mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;

  //if (mcData.mData.u32PirOnTime > 0)
  //{
  //  mcData.mData.bPirEnable = True;
  //  mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
  //  mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
  //}
  //else
  //{
  //  mcData.mData.u32PirOnTime = 0;
  //  mcData.mData.bPirEnable = False;
  //  mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
  //  mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
  //}

  mLstLedAnims[mu16LedAnimationIdx]->vInit();

  mcData.mData.u16Brigthness = 0;

  ////CycCall_Start(MAIN_vTick1msHp,
  ////              MAIN_vTick1msLp);

  mcSys.vInit2();
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  #ifdef PCB_PROJECTCFG_Test
    #ifdef TESTSYSRUNTIME
      mcPB00.vSet1();
      mcTestClassTim[1].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
    #endif
  #endif

  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  }
}

void SystemClock_Config_HSI(void)
{
  // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct   = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  // Configure the main internal regulator output voltage
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}


// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  #ifdef PCB_PROJECTCFG_Test
    // Messung um die PowerUp Starttime zu messen
    // Wird vor Sleep wieder ausgeschaltet
    // Set PB01
  cSysPkgPMon::vPB01_Init();
  cSysPkgPMon::vPB01_Set1();
  #endif

  SystemInit();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPB01_Set0();
  #endif
  HAL_Init();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPB01_Set1();
  #endif
  SystemClock_Config_HSI();
  SystemCoreClockUpdate();

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPB01_Set0();
  #endif

  #ifdef PCB_PROJECTCFG_Test
    // Timer aufziehen nachdem der uC auf 16Mhz ist
    // Nach dem Reset, braucht der es ~6.3ms bis dieser Punkt
    //   + 5ms bei einer 10k/1uF Reset-Beschaltung
    //   + 1,3ms Laufzeit bis hierhin
    cSysPkgPMon::vInitTimer(6300);
  #endif

  if (!RomConst_IsValid())
  {
    // RomConst Error
    SysError_Handler();
  }

  cBuRam::vEnable();

  // 0x20007800
  //   Heap 32k
  // 0x2000F800
  //   Stack 2k
  // 0x20010000
  #ifdef PCB_PROJECTCFG_Test
    //cMemTools::vMemSet((u8*)0x2000F800, 0x55, 0x800 - 128);
    //cMemTools::vMemSet((u8*)0x20007800, 0x55, 0x8000);
  #endif

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPB01_Set1();
  #endif
}

