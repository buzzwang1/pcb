
#include "main.h"
#include "stdlib.h" // for Rand

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz



//  Power Switch v3 LED add on
//
//  PB09  -> Status Led
//
//  AddOn:
//    WS2812:
//      PA08: Led_Sig (e.g. WS2812), 12xLEDs, TIM1_CH1, AF1
//      PA02: Led_Sig (e.g. WS2812), 12xLEDs, TIM2_CH3, AF1 ****
//      PA15: High side switch for LEDs
//    Rotatory Switch:
//      PB05: A
//      PB08: B
//      PB15: D (Button)
//
//  OutPuts:
//    PA01: High side switch for 3V3
//  OutPuts:
//    PB00: High side switch for CH1
//    PB01: High side switch for CH2
//    PB02: High side switch for CH3
//
//  I2C3  -> Board (Eeprom Adr.:0x50, INA3221 Adr.: 0x40)
//    PA07  -> I2C3 SCL  AF4
//    PB04  -> I2C3 SDA  AF4
//
//  Ext. Flash
//    SPI1
//      PA10: CS
//      PA11: Miso AF5
//      PA12: Mosi AF5
//      PB03: Clk  AF5
//
//  Power Module
//    PA04: DAC
//    PA05: Enable
//
//  BotNet DownLink
//    I2C2
//      PB13  -> I2C2 SCL AF4
//      PB14  -> I2C2 SDA AF4
//    U3
//      PB10  -> TX/RX    AF7
//
//  BotNet UpLink
//    I2C1
//      PB06  -> I2C1 SCL AF4
//      PB07  -> I2C1 SDA AF4
//    U1
//      PA09  -> TX/RX    AF7
//
//  Timer Usage:
//    TIM1  -> PWM für WS2812 for DMA1.Chl2
//    TIM2  -> PWM für WS2812 for DMA1.Chl3 ****
//    TIM6  -> CyclicCaller
//    TIM15 -> BotCom MpHd UpLink
//    TIM16 -> BotCom MpHd DownLink

//
//  DMA Usage:
//    DMA1:
//      0:
//      1: TIM2 für WS2812   CS:4
//      2: I2C3 Tx: Board    CS:3 / U3 Tx DownLink CS:2 / TIM1 for WS2812 CS:7
//      3: I2C3 Rx: Board    CS:3 / U3 Rx DownLink CS:2
//      4: I2C2 Tx: DownLink CS:3 / U1 Tx UpLink CS:2
//      5: I2C2 Rx: DownLink CS:3 / U1 Rx UpLink CS:2
//      6: I2C1 Tx: UpLink   CS:3
//      7: I2C1 Rx: UpLink   CS:3
//
//    DMA2:
//      0:
//      1:
//      2:
//      3: SPI1 Rx  CS:4
//      4: SPI1 Tx  CS:4
//      5:
//      6: I2C1 Rx: BotNet CS:5 / U1 Tx UpLink CS:2
//      7: I2C1 Tx: BotNet CS:5 / U1 Rx UpLink CS:2
//
//  Interrupt Usage:
//    DMA1_Channel1_IRQHandler: WS2812:        Prio: 8.8
//    I2C1_EV_IRQHandler:       BotCom:        Prio: 8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio: 8.8
//    I2C2_EV_IRQHandler:       Board:         Prio: 8.8
//    I2C2_ER_IRQHandler:       Board:         Prio: 8.8
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio: 6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio: 6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio: 6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio: 6.8 => U1
//
//


// Call first to get the system clocks
// This is even called before the inlcuded classs in the header
cClockInfo mcClockInfo;

#define LEDMAXLEDCNT 12
u16 mu16LedCnt = LEDMAXLEDCNT;
u8 mu8MyLedRgbMemory[LEDCTRL_RGB_MEMSIZE(LEDMAXLEDCNT)];
u8 mu8MyLedBitMemory[LEDCTRL_LED_MEMSIZE(LEDMAXLEDCNT)];
cWs2812 mcWs2812(mu8MyLedRgbMemory, mu8MyLedBitMemory, LEDMAXLEDCNT);


cI2cEep     mcEep(&mcSys.mcBoard.mcI2C3_Board, 0xA0);


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
    u16  u16TurnOnState;
    u8   Res2[32 - 2 - 2 - 2 - 2 - 2];
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
    mData.i16MaxTemp          = 80; // °C
    mData.u16Brigthness       = 16; // 5%
    mData.u16TurnOnState      = 1;
    mData.u16LedAnimationIdx  = 0;
    mData.u16StartTimerReload = 1000 * 5; // 5s
    mData.u16LedBlockCnt      = 1;
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

  virtual void vSetValue(i16 li16Value)
  {
    if (li16Value < 0)          li16Value = 0;
    if (li16Value > mu16LedCnt) li16Value = mu16LedCnt;
    mi16Value = li16Value;
  }

  virtual void vInit()
  {
  }

  virtual void vProcess10ms()
  {
  }
};

class cEepData: public cData
{
  public: 
  typedef enum
  {
     nSize = (1 * 1024)
  }tenCfg;


  u16      mu16StoreTimer;
  u16      mu16LoadTimer;
  cI2cEep* mcEep;
  bool     mbLoad;
  bool     mbError;

  cEepData(cI2cEep* lcEep)
  {
    mcEep   = lcEep;
    mbLoad  = True;
    mbError = False;
    mu16StoreTimer = 0;
    mu16LoadTimer  = 0;
  }



  void vLoad()
  {
    cMemTools::vMemCpy((u8*)&mData, (u8*)((u32)mcEep->mu8RamMirrow), sizeof(tstData));
  }

  void vStore()
  {
    mData.u32Cnt++;
    cMemTools::vMemCpy((u8*)((u32)mcEep->mu8RamMirrow), (u8*)&mData, sizeof(tstData));
    mcEep->vCmdWriteAll();
  }

  void vUpdate()
  {
    mu16StoreTimer = mData.u16StartTimerReload;
  }

  void vTick10ms()
  {
    if (mu16StoreTimer > 0)
    {
      if (mu16StoreTimer > 10)
      {
        mu16StoreTimer -= 10;
      }
      else
      {
        mu16StoreTimer = 0;
        vStore();
      }
    }

    // Warten bis Daten gelesen wurden
    if (mbLoad)
    {
      // Timeout zum warten auf EEPROM
      if (mu16LoadTimer < 1000)
      {
        mu16LoadTimer += 10;
      }
      else // EEPROM wurde nicht geladen
      {
        mbError = True;
        mbLoad = False;
        vCheckAndSetDefault();
      }
      
      if (mcEep->mStatus.IsInit)
      {
        mbLoad = False;
        vLoad();
        vCheckAndSetDefault();
      }
    }
  }
};

cEepData mcData(&mcEep);

class cUserInterface
{
  class cInputDebounce
  {
    // Debounce
    // Switch must be have stable state for 50ms to be debounced

    public:
      u8   mu8StateOld;
      u8   mu8StateDebounced;
      u32  mu32StableCnt;       // [ms]
      u32  mu32DebounceTimeOn;  // [ms]
      u32  mu32DebounceTimeOff; // [ms]

      cInputDebounce(u8 lu8InitState, u32  lu32DebounceTimeOn = 50, u32 lu32DebounceTimeOff = 50)
      {
        mu8StateOld = lu8InitState;
        mu8StateDebounced = lu8InitState;
        mu32StableCnt = 100;
        mu32DebounceTimeOn  = lu32DebounceTimeOn;
        mu32DebounceTimeOff = lu32DebounceTimeOff;
      }

      u8 u8Process(u8 lu8StateNew, u8 lu8Cycle)
      {
        if (lu8StateNew != mu8StateOld)
        {
          if (lu8StateNew == 0)
          {
            mu32StableCnt = mu32DebounceTimeOff;
          }
          else
          {
            mu32StableCnt = mu32DebounceTimeOn;
          }

          mu8StateOld   = lu8StateNew;
        }
        else
        {
          if (mu32StableCnt > lu8Cycle)
          {
            mu32StableCnt -= lu8Cycle; // [ms]
          }
          else
          {
            mu32StableCnt = 0;
            mu8StateDebounced = lu8StateNew;
          }
        }
        return mu8StateDebounced;
      }

      u8 u8Get()
      {
        return mu8StateDebounced;
      }
  };

  public:
    bool mbSingleClick;
    bool mbDoubleClick;

    i8   mi8RotDir; // -1 = CCW, 0 = Nothing; 1 = CW
    u8   mu8RotInv; // Flip rotatory switch's CW<->CCW direction.
                    // Depending on switch, output for CW and CCW are different
    i16  mi16RotCnt;
    i16  mi16RotCntMin;
    i16  mi16RotCntMax;
    u8   mu8RotCntInc;
    u8   mu8RotSeq;
    u8   mu8Cnt1ms;

    u8  mu8RotDsD_State; // 0 pressed; 1 released
    u8  mcRot_State;     // 0, 1, 3, 4
    u8  mcPir_State;     // 1 = Motion detected; 0 = Idle

    u8  mu8BtnState;     // 0 = Pressed
                         // 1 = Released
                         // 2 = Pressed again after a delay => Doubleclick
                         // 3 = Released again

    u16 mu16BtnDoubleClickDelay_ms;
    u16 mu16BtnDoubleClickDelayCnt_ms;


    // Rotatory Encoder:
    //
    // DsA = 2
    // DSB = 1
    //
    // CW  = 3 1 0 2
    // CCW = 3 2 0 1
    //

    cInputDebounce  mu8RotDsD_Deb;
    cInputDebounce  mcRotDsA_Deb;
    cInputDebounce  mcRotDsB_Deb;
    //cInputDebounce  mcPir_Deb;


    // --- Pins
    cGpPin mcRotDsA; // Rotatory Encoder A
    cGpPin mcRotDsB; // Rotatory Encoder B

    cGpPin mcRotDsD; // Rotatory Encoder Switch

    //cGpPin mcPir;    // Person Sensor

    cUserInterface()
      : mu8RotDsD_Deb(mcRotDsD.ui8Get(), 10, 10),
        mcRotDsA_Deb(mcRotDsA.ui8Get(), 2, 2),
        mcRotDsB_Deb(mcRotDsB.ui8Get(), 2, 2),
        //mcPir_Deb(0, 50, 1000 * 60 * 10), /* 10min = 10 * 60 * 1000ms */
        mcRotDsA(GPIOB_BASE,  5, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0),
        mcRotDsB(GPIOB_BASE,  8, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0),
        mcRotDsD(GPIOB_BASE, 15, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0)//,
        //mcPir(   GPIOC_BASE, 10, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
    {
      mbSingleClick = False;
      mbDoubleClick = False;

      mi8RotDir = 0;
      mu8RotInv = 0;
      mi16RotCnt    = 0;
      mi16RotCntMin = -100;
      mi16RotCntMax =  100;
      mu8RotCntInc  =    1;

      mu8BtnState = 0;

      mu16BtnDoubleClickDelay_ms = 300;
      mu16BtnDoubleClickDelayCnt_ms    = 0;

      mu8Cnt1ms = 0;

      mu8RotDsD_State = mcRotDsD.ui8Get();
      mcRot_State = (mcRotDsA.ui8Get() << 1) + mcRotDsB.ui8Get();
    }

    void vSetRotInv(u8 lu8RotInv)
    {
      if (lu8RotInv)
      {
        mu8RotInv = 1;
      }
      else
      {
        mu8RotInv = 0;
      }
    }

    void vSetRotRange(i16 li16RotCntMin, i16 li16RotCntMax)
    {
      mi16RotCntMin = li16RotCntMin;
      mi16RotCntMax = li16RotCntMax;
    }

    void vSetRotInc(u8 lu8RotCntInc)
    {
      mu8RotCntInc = lu8RotCntInc;
    }

    void vSwitchSm10ms()
    {
      switch (mu8BtnState)
      {
        case 0: // Wait for Button Pressed
          if (mu8RotDsD_State) // Low active, if Btn not pressed
          {
            if (mu8RotDsD_Deb.u8Get() == 0) // if Butten was pressed
            {
              mu8RotDsD_State = 0;
              mu8BtnState++;
            }
          }
          break;
        case 1: // Wait for Button released
          if (!mu8RotDsD_State) // Low active, if Btn pressed
          {
            if (mu8RotDsD_Deb.u8Get() == 1) // if Butten was released
            {
              mu8RotDsD_State = 1;
              mu16BtnDoubleClickDelayCnt_ms = 0;
              mu8BtnState++;
            }
          }
          break;
        case 2: // Wait for Button Pressed in mu16BtnDoubleClickDelayReload_ms

          if (mu16BtnDoubleClickDelayCnt_ms > mu16BtnDoubleClickDelay_ms)
          {
            // kein zweiter Click in der Wartezeit
            mbSingleClick = True;
            mu8BtnState = 0;
          }
          else
          {
            if (mu8RotDsD_Deb.u8Get() == 0) // if Butten was pressed
            {
              mu8RotDsD_State = 0;
              mu8BtnState++;
            }
          }
          break;
        case 3: // Wait for Button released
          if (!mu8RotDsD_State) // Low active, if Btn pressed
          {
            if (mu8RotDsD_Deb.u8Get() == 1) // if Butten was released
            {
              mu8RotDsD_State = 1;
              mbDoubleClick = True;
              mu8BtnState = 0;
            }
          }
          break;
      }
    }

    void vRotSm1ms()
    {
      u8 lcRot_StateNew;

      if (mu8RotInv)
      {
        lcRot_StateNew = (mcRotDsA_Deb.u8Get() << 1) + mcRotDsB_Deb.u8Get();
      }
      else
      {
        lcRot_StateNew = (mcRotDsB_Deb.u8Get() << 1) + mcRotDsA_Deb.u8Get();
      }

      // Rotatory Encoder:
      //
      // DsA = 2
      // DSB = 1
      //
      // Seq:  0 0 0 1 2 3 0
      // CW  = 3 3 1 0 2 3 3
      // CCW = 3 3 2 0 1 3 3
      //
      if (lcRot_StateNew != mcRot_State)
      {
        switch (mu8RotSeq)
        {
          case 0:
            if (mcRot_State == 3)
            {
              switch (lcRot_StateNew)
              {
                case 1: mi8RotDir =  1; mu8RotSeq++;  break;
                case 2: mi8RotDir = -1; mu8RotSeq++;  break;
              }
            }
            break;
          case 1:
            if (lcRot_StateNew == 0)
            {
              //mu8RotSeq++;
              mu8RotSeq = 3;
            }
            break;
          //case 2:
          //  switch (lcRot_StateNew)
          //  {
          //    case 1: if (mi8RotDir == -1)  mu8RotSeq++;  break;
          //    case 2: if (mi8RotDir ==  1)  mu8RotSeq++;  break;
          //  }
          //  break;
          case 3:
            if (lcRot_StateNew == 3)
            {
              i16 mi16RotCntNew = mi16RotCnt;
              mi16RotCntNew += (mi8RotDir * mu8RotCntInc);
              if (mi16RotCntNew < mi16RotCntMin) mi16RotCntNew = mi16RotCntMin;
              if (mi16RotCntNew > mi16RotCntMax) mi16RotCntNew = mi16RotCntMax;
              mi16RotCnt = mi16RotCntNew;
            }
            break;
        }
        mcRot_State = lcRot_StateNew;
      }
      else
      {
        if (lcRot_StateNew == 3) mu8RotSeq = 0;
      }
    }


    void vTick1msLp(void)
    {
      mu8Cnt1ms++;
      if (mu8Cnt1ms >= 10)
      {
        mu8Cnt1ms = 0;
        mu8RotDsD_Deb.u8Process(mcRotDsD.ui8Get(), 10);
        mu16BtnDoubleClickDelayCnt_ms += 10;
        vSwitchSm10ms();
        //mcPir_Deb.u8Process(mcPir.ui8Get(), 10);
      }

      mcRotDsA_Deb.u8Process(mcRotDsA.ui8Get(), 1);
      mcRotDsB_Deb.u8Process(mcRotDsB.ui8Get(), 1);
      if (mcData.mData.u16TurnOnState) vRotSm1ms();
    }

    bool isBtnSingleClicked()
    {
      if (mbSingleClick)
      {
        mbSingleClick = False;
        return True;
      }
      return False;
    }

    bool isBtnDoubleClicked()
    {
      if (mbDoubleClick)
      {
        mbDoubleClick = False;
        return True;
      }
      return False;
    }

    i16 i16GetRotCnt()
    {
      return mi16RotCnt;
    }

    i8 i8GetRotDir()
    {
      i8 li8RotDirSave = mi8RotDir;
      mi8RotDir = 0;
      return li8RotDirSave;
    }

    bool isHumanPresent()
    {
      return 1;// (mcPir_Deb.u8Get() == 1);
    }
};

cUserInterface mcUI;



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
    vProcess10ms();
  }

  void vProcess10ms() override
  {
    for (u16 lu16i = 0; lu16i < mi16Value; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
    for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
    }
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

      for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
      {
        mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
      }

      if (((mu8LineIdx & 1) == 0) && (mcData.mData.u16TurnOnState == 1))
      {
        for (u16 lu16i = 0; lu16i < mi16Value; lu16i++)
        {
          mcWs2812->vSetLedPxl(lu16i, R, G, B);
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

      for (u16 lu16i = 0; lu16i < mi16Value; lu16i++)
      {
        mcWs2812->vSetLedPxl(lu16i, R / 8, G / 8, B / 8);
      }

      if (mi16PxlIdx < mu16LedCnt)
      {
        mcWs2812->vSetLedPxl(mi16PxlIdx, R, G, B);
      }

      mi16PxlIdx++;
      if (mi16PxlIdx >= (mu16LedCnt * 3 / 2))
      {
        mi16PxlIdx = 0;
      }

      for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
      {
        mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
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
    for (u16 lu16Idx = 0; lu16Idx < mi16Value; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
    for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
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
    for (u16 lu16Idx = 0; lu16Idx < mi16Value; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
    for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
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
    for (u16 lu16i = 0; lu16i < mi16Value; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
    for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
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
    for (u16 lu16i = 0; lu16i < mi16Value; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
    for (u16 lu16i = mi16Value * mcData.mData.u16TurnOnState; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, 16, 16, 16);
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


class cLedAnimation_Lampe : public cLedAnimation_Static
{
public:
  cLedAnimation_Lampe(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"Lampe")
  {
  }


  void vInit() override
  {
    cLedAnimation_Static::vInit();
  }


  void vProcess10ms() override
  {
    switch(mi16Value)
    {
      case 1:  R =  64; G =   0; B =   0; break;
      case 2:  R = 128; G =   0; B =   0; break;
      case 3:  R = 192; G =   0; B =   0; break;
      case 4:  R = 255; G =  64; B =  64; break;
      case 5:  R = 255; G = 128; B = 128; break;
      case 6:  R = 255; G = 192; B = 192; break;
      case 7:  R = 255; G = 255; B = 255; break;
      case 8:  R = 255; G = 255; B = 255; break;
      case 9:  R = 255; G = 255; B = 255; break;
      case 10: R = 255; G = 255; B = 255; break;
      case 11: R = 255; G = 255; B = 255; break;
      case 12: R = 255; G = 255; B = 255; break;
    }

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
cLedAnimation_Lampe     mcLA_LampeRgb(&mcWs2812);

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

class cCliCmd_HwStatus2 : public cCliCmd
{
public:
  cCliCmd_HwStatus2()
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
      //lszStr.Setf((rsz)"Relais: %d", (u16)mcSys.mcBoard.mcBoardCntrl.lenSmS[cBoardCtrl::enS_Ch220V]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Led: %d", (u16)mcWs2812.mcEn.ui8Get()); lcCli->bPrintLn(lszStr);

      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Temp:");
      lszStr.Setf((rsz)"Ch1: %d °C", (i16)mcTemp.i16GetTemp(0)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch2: %d °C", (i16)mcTemp.i16GetTemp(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch3: %d °C", (i16)mcTemp.i16GetTemp(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Ch4: %d °C", (i16)mcTemp.i16GetTemp(3)); lcCli->bPrintLn(lszStr);


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
        lszStr = (rsz)"  Anim:    "; lszStr += mLstLedAnims[mu16LedAnimationIdx]->mcName; lcCli->bPrintLn(lszStr);
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

      if ((lu16NewIdx < 1) || (lu16NewIdx >= (sizeof(mLstLedAnims) / sizeof(cLedAnimation*))))
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

class cCliCmd_EepTest : public cCliCmd
{
public:
  cCliCmd_EepTest() :cCliCmd((const char*)"EepTest", (const char*)"Test Eep") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 40);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      i16 li16Val = lcParam.Atoi();
      if (li16Val == 1)
      {
        mcData.vSetDefault();
        mcData.mData.u32ID = 0x12345678;
        mcData.vStore();
        lszStr.Setf((rsz)"  Eep set to default"); lcCli->bPrintLn(lszStr);
      }
      else
      {
        mcEep.vCmdSetup();
        lszStr.Setf((rsz)"  Eep Init"); lcCli->bPrintLn(lszStr);
      }
    }
    else
    {
      lszStr.Setf((rsz)"  EepReset 1: Set to default values"); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"  EepReset 2: Reload"); lcCli->bPrintLn(lszStr);
    }
    return True;
  }
};

class cCliCmd_RotInv : public cCliCmd
{
public:
  cCliCmd_RotInv() :cCliCmd((const char*)"RotInv", (const char*)"Inverse rotatory switch direction") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 40);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      i16 li16Val = lcParam.Atoi();
      if (li16Val)
      {
        if (mcData.mData.u8RotInv != 1)
        {
          mcData.mData.u8RotInv = 1;
          mcUI.vSetRotInv(mcData.mData.u8RotInv);
          mcData.vUpdate();
        }
      }
      else
      {
        if (mcData.mData.u8RotInv != 0)
        {
          mcData.mData.u8RotInv = 0;
          mcUI.vSetRotInv(mcData.mData.u8RotInv);
          mcData.vUpdate();
        }
      }
    }
    lszStr.Setf((rsz)"  Value: %d", mcData.mData.u8RotInv); lcCli->bPrintLn(lszStr);
    return True;
  }
};

cCliCmd_HwStatus2    mcCliCmd_HwStatus2;
cCliCmd_LedAnimList  mcCliCmd_LedAnimList;
cCliCmd_Status_Led   mcCliCmd_Status_Led;
cCliCmd_AnimIdx      mcCliCmd_AnimIdx;
cCliCmd_Rgb          mcCliCmd_Rgb;
cCliCmd_EepTest      mcCliCmd_EepTest;
cCliCmd_RotInv       mcCliCmd_RotInv;

cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_HwStatus2,  &mcCliCmd_LedAnimList,
                                           &mcCliCmd_Status_Led, &mcCliCmd_AnimIdx,
                                           &mcCliCmd_Rgb,        & mcCliCmd_EepTest,
                                           &mcCliCmd_RotInv}, 7);


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

              lu8Data[0] = 0;
              lu8Data[1] = 0;
              lu8Data[2] = 0;
              lu8Data[3] = mcData.mData.u16TurnOnState;  // mcWs2812.mcEn.ui8Get();
              lu8Data[4] = mcUI.mi16RotCnt; //mcData.mData.u16Brigthness;
              lu8Data[5] = mcData.mData.u16LedAnimationIdx;
              lu8Data[6] = mcData.mData.u8CustR;
              lu8Data[7] = mcData.mData.u8CustG;
              lu8Data[8] = mcData.mData.u8CustB;
              lu8Data[9] = mcTemp.i16GetTemp(0) >> 8;
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
                u8 lu8AnimCnt;
                lu8AnimCnt = ((sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1);

                mcData.mData.u16TurnOnState = lcMsg.mcPayload[3] & 1;

                if (lcMsg.mcPayload[4] < 12) mcUI.mi16RotCnt = lcMsg.mcPayload[4];
                                        else mcUI.mi16RotCnt = 12;

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
                mLstLedAnims[mu16LedAnimationIdx]->vSetValue(mcUI.mi16RotCnt);

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
  cErr::munErr->stErr.isHardFault = 1;
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  cErr::munErr->stErr.isMemManage = 1;
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  cErr::munErr->stErr.isBusFault = 1;
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  cErr::munErr->stErr.isUsageFault = 1;
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  cErr::munErr->stErr.isSVC = 1;
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  cErr::munErr->stErr.isDebugMon = 1;
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  cErr::munErr->stErr.isPendSV = 1;
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

// ---------------------------- WS2812 ---------------------------

void DMA1_Channel1_IRQHandler(void)
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
  mcUI.vTick1msLp();

  mcSys.vTick1msLp();

  lu8Cnt1ms++;
  if (lu8Cnt1ms == 10)
  {
    lu8Cnt1ms = 0;
    MAIN_vTick10msLp();
  }
}


i16 mi16RotCntOld = 0;


void MAIN_vTick10msLp()
{
  static volatile u8 lu8BrightnessOld = 128;
  static u16 lu16Minute    = 0;
  static i16 li16TempAlarm = 0;

  mcWs2812.vTick10msLp();

  i16 li16RotCnt;
  li16RotCnt = mcUI.i16GetRotCnt();

  if (mi16RotCntOld != li16RotCnt)
  {
    mi16RotCntOld = li16RotCnt;
    mLstLedAnims[mu16LedAnimationIdx]->vSetValue(li16RotCnt);
    mcData.vUpdate();
  }

  if (mcUI.isBtnSingleClicked())
  {
    
    if (mcData.mData.u16TurnOnState) mcData.mData.u16TurnOnState = 0;
                                else mcData.mData.u16TurnOnState = 1;
  }

  if ((mcUI.isBtnDoubleClicked()) && (mcData.mData.u16TurnOnState))
  {
    mu16LedAnimationIdx = mu16LedAnimationIdx + 1;
    if (mLstLedAnims[mu16LedAnimationIdx] == NULL) mu16LedAnimationIdx = 0;
    mLstLedAnims[mu16LedAnimationIdx]->vSetValue(li16RotCnt);
    mLstLedAnims[mu16LedAnimationIdx]->vInit();
    mcData.mData.u16LedAnimationIdx = mu16LedAnimationIdx;
    mcData.vUpdate();
  }

  u8 lu8Brightness;

  lu8Brightness = mcData.mData.u16Brigthness;

  ////if (mcData.mData.bPirEnable)
  ////{
  ////  lu8Brightness *= mcUI.isHumanPresent();
  ////}


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

  mLstLedAnims[mu16LedAnimationIdx]->vProcess10ms();
  mcWs2812.vShow();
  mcData.vTick10ms();
  mcEep.vTick10ms();
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
  mu16LedCnt = mcData.mData.u16LedBlockCnt * 12;

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

  mcUI.vSetRotInv(mcData.mData.u8RotInv);
  mcUI.vSetRotRange(0, 12);
  mcUI.vSetRotInc(1);

  // Beim einschalten aus. Wird dann über Msg eingeschaltet
  mi16RotCntOld = mcUI.mi16RotCnt = 0;
  mLstLedAnims[mu16LedAnimationIdx]->vSetValue(mi16RotCntOld);
  mLstLedAnims[mu16LedAnimationIdx]->vInit();

  mcWs2812.vEnable();

  // Status Nachricht alle 100ms senden     Name        Side   S        D                    MsgIdx ( 4 => RRpt) 
  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgSLedCyc, 32, 0x1000, mcSys.mcCom.mcBn.mcAdr.Get(), 4);

  //40: Request Messages
  //  Message Req : Rx   RI.S1.S2     // Payload Len:  3; Request           SI = Request Index 
  //                                         //                                    S1 = Sub Index1  
  //                                         //                                    S2 = Sub Index2  
  //
  //                                         // R1 S1 S2
  //                                         //  0  0  0:  SLED:     Status

  lcMsgSLedCyc.mcPayload[0] = 1;       // Set Command
  lcMsgSLedCyc.mcPayload[1] = 40;      // Request Message Index
  lcMsgSLedCyc.mcPayload[2] = 0;       // RI
  lcMsgSLedCyc.mcPayload[3] = 0;       // S1
  lcMsgSLedCyc.mcPayload[4] = 0;       // S2
  lcMsgSLedCyc.mcPayload[5] = 0x10;    // 0x1000 DAdr HB
  lcMsgSLedCyc.mcPayload[6] = 0x00;    // 0x1000 DAdr LB
  lcMsgSLedCyc.mcPayload[7] = 10;     // 10 * 10ms = 100ms cycle time

  lcMsgSLedCyc.vEncode();
  mcSys.mcCom.mcBn.mcRRpt.bMsg(lcMsgSLedCyc);


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

void SystemClock_Config_HSE(void)
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
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
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
  SystemClock_Config_HSE();
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

