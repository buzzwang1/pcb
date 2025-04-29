
#include "main.h"


// CH32V305 => D8C, Core V4F IMAFC
// RiscV 32b MCU, (144 MHz max)
// Rom 128KB
// 
// 
// 
// 24Mhz Quarz
// 
// PB14: LED:          Tim2_Ch2(AF1); Tim0_Ch1_On(AF2)
// 
// WS2812:
//   PB15: Power MOSFET; 1 = Enable ; 0 = Disable
//   PA8:  TIM1_CH1:DMA Channel2: TIM1 ADTM
//   PA5:  10k NTC Temp Sensor:     => ADC1 ADC_IN5 PGA 1x
//   PA6:  10mOhm Shunt LED Strom:  => ADC2 ADC_IN6 PGA 64x 
//                                     ADC_Ref = 3300mV / 64 ~= 52mV => 52mV / 10mOhm = 5.2A (max)
//                                     ADC_Res = 4096 => ~1,25mA/digit
//                                       100mA =>  125 digit
//                                       400mA =>  503 digit
//                                       500mA =>  629 digit
//                                      1000mA => 1258 digit
//                                      2000mA => 2517 digit  (max. USBC Power with 5k1)
//         50mOhm Shunt LED Strom:  => ADC2 ADC_IN6 PGA 16x 
//                                     ADC_Ref = 3300mV / 16 ~= 206mV => 206mV / 50mOhm = 4.125A (max)
//                                     ADC_Res = 4096 => ~1,01mA/digit
//                                      100mA =>  101 digit
//                                      400mA =>  403 digit
//                                      500mA =>  504 digit
//                                     1000mA => 1007 digit
//                                     2000mA => 2014 digit  (max. USBC Power with 5k1)// 
// 
// I2C: 
//   PB10: I2C2_SCL
//   PB11: I2C2_SDA
// 
// 
// USB: FS = Full Speed (12MBit/s); HS = High Speed (480MBit/s)
//   PB7: PA12_USB_DP
//   PB6: PA11_USB_DN
//
// HW-USage
//   System:
//     Externer Quarz: HSE: 24Mhz
//     PLL = 4       =>   SYSCLK   = 96Mhz
//     AHB-Pres = 1  =>   HCLK     = 96MHz (bus/core/memory/DMA)
//       SysTickTimer-Pres = 8     = 12Mhz (SysTickTimer)
//     APB1-Pres = 2 =>   PLCK1    = 48Mhz (APB1 peripherals/TIM2,3,4,5,6,7)
//     APB2-Pres = 1 =>   PLCK2    = 96Mhz (APB2 peripherals/TIM1,8,9,10)
//       ADC-Pres = 8 =>  APB2_ADC = 12Mhz (ADC1,2)
//    
// 
//   Systick: 12Mhz eingestellt für 1ms IRQ => alle 12000 Takte
//     SysTick_Irq: Prio 7
// 
// 
//   EEPROM:
//     I2C2: 400khz
//     I2C2_EvIrq:  Event IRQ: Prio 2
//     I2C2_ErrIrq: Error IRQ: Prio 2
//     DMA1_Chl4->I2C Tx DMA
//     DMA1_Chl5->I2C Rx DMA
//     DMA1_Chl4_TcIrq: Transmission Complete IRQ: Prio 2
//     DMA1_Chl5_TcIrq: Transmission Complete IRQ: Prio 2
//     
// 
//   WS2812:
//     TIM1: Erzeugung 800khz für WS2812 und Trigger für DMA1_Chl2
//     TIM1_CCR1: Erzeugung PWM für WS2812 und Trigger für DMA1_Chl2
//     DMA1_Chl2: Füttert TIM1-CCR1 mit LED Data
//     DMA1_Chl2_TcIrq: Transmission Complete IRQ: Prio 2
//
//     ADCx: 96 / 8 => 12Mhz
//     ADC1: Verstärkung 1x: Temperatur Messung
//     ADC2: Verstärkung 16x: Shunt Strom Messung
//     DMA1_Chl1: Kopiert Daten von ADC in den RAM
// 
//
//   USB:
//     TIM2: 1khz für USART Emuulation
//     TIM2_Irq: Prio 2
//
//     UsbHs (480MBit/s)
//     UsbHs_Irq: Prio 2
//
//
//
//   Irq: Der Core kann 3 HW Interrupts und Level 0..7 (0 = höchste Prio)
//     0: --- do not use
//     1: --- do not use
//     2: High Prio
//     3: --- do not use
//     4: Middle Prio
//     5: --- do not use
//     6: --- do not use
//     7: Low Prio
//
//

#define LEDCNT 12

LED<GPIOB_BASE, 14> mcTestLed;

cClockInfo mcClockInfo; // Must be at the front

u8 mu8MyRgbMemory[LEDCTRL_RGB_MEMSIZE(LEDCNT)];
u8 mu8MyLedMemory[LEDCTRL_LED_MEMSIZE(LEDCNT)];
cWs2812 mcWs2812(mu8MyRgbMemory, mu8MyLedMemory, LEDCNT);

cGpPin      mcI2c2_SCL_Board(GPIOB_BASE, 10, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 1);
cGpPin      mcI2c2_SDA_Board(GPIOB_BASE, 11, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 1);
cI2cMaster  mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 8);
//cI2cEep     mcEep(&mcI2C2_Board, 0xA0);
cLm75       mcLm75(&mcI2C2_Board, LM75_I2C_ADDRESS_DEFAULT);


class cFlashData
{
  public: 
  
  // Der letzte flash sector wird zum Speicher von Daten benutzt
  typedef enum
  {
     // Fast programming : The page operation mode(Recommended) is used for this method.After unlocking
     // in a specific sequence, a single 256 - byte programming and 256 - byte erasing are performed, 32K - byte erasing,
     // 64K - byte erasing and whole chip erasure are performed
     nAdr  = 0x08000000 + (128 * 1024) - (4 * 1024),
     nSize = (4 * 1024),
  }tenCfg;


  typedef struct
  {
    u32  u32ID;
    u32  u32Cnt;
    bool bPirEnable;
    u32  u32PirOnTime;
    u16  u16Brigthness;
    u16  u16LedAnimationIdx;
    u16  u16StartTimerReload;
    u8   u8CustR;
    u8   u8CustG;
    u8   u8CustB;
    u16  u8TempAdc1;
    u16  u8TempAdc2;
  }tstData;


  tstData mData;
  u16     mu16StoreTimer;

  cFlashData()
  {
    mu16StoreTimer = 0;
    vLoad();
    if (mData.u32ID != 0x12345678)
    {
      // ROM Defaults
      mData.u32ID = 0x12345678;
      mData.u32Cnt              = 0;
      mData.bPirEnable          = True;
      mData.u32PirOnTime        = 1000 * 60 * 10; /* 10min = 10 * 60 * 1000ms */;
      mData.u16Brigthness       = 128; // 50%
      mData.u16LedAnimationIdx  = 0;
      mData.u16StartTimerReload = 1000 * 5; // 5s
      mData.u8CustR = 255;
      mData.u8CustG = 255;
      mData.u8CustB = 255;
      mData.u8TempAdc1 = 100;
      mData.u8TempAdc2 =  77;
      vStore();
    }
  }

  void vLoad()
  {
    cMemTools::vMemCpy((u8*)&mData, (u8*)((u32)nAdr), sizeof(tstData));
  }

  // GD32F130xx_Datasheet_Rev3.1.pdf
  // Symbol       Parameter                          Conditions        Min Typ Max Unit
  // Fprog        Programming frequency)             TA = -40°C~85°C           60  MHz
  // tprog_page   Page(256 bytes) programming time   TA = -40°C~85°C        2 2.5  ms
  // terase_page  Page(256 bytes) erase time         TA = -40°C~85°C       16  20  ms
  // terase_sec   Sector(4K bytes) erase time        TA = -40°C~85°C       16  20  ms
  void vStore()
  {
    u32 lu16Data;
    u32 lu32Size = sizeof(mData);
    u8* lpui8Src    = (u8*)&mData;
    u8* lpui8Adress = (u8*)((u32)nAdr);

    mData.u32Cnt++;

    /* Unlock the Flash to enable the flash control register access *************/
    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage((uint32)nAdr);

    while (lu32Size > 0)
    {
      lu16Data = *((uint16*)lpui8Src);

      if (FLASH_ProgramHalfWord((uint32)lpui8Adress, lu16Data) == FLASH_COMPLETE)
      {
        lpui8Adress += 2;
        lpui8Src += 2;
        lu32Size -= 2;
      }
      else
      {
        // Error occurred while writing data in Flash memory.
        //   User can add here some code to deal with this error
        //mui8Error = 2;
        break;
      }
    }

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) */
    FLASH_Lock();
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
  }
};

cFlashData mcData;


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
    bool mbKeyPressed;
    i8   mi8RotDir; // -1 = CCW, 0 = Nothing; 1 = CW
    u8   mu8RotCnt;
    u8   mu8RotSeq;
    u8   mu8Cnt1ms;

    u8  mu8RotDsD_State; // 0 pressed; 1 released
    u8  mcRot_State;     // 0, 1, 3, 4
    u8  mcPir_State;   // 1 = Motion detected; 0 = Idle


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
    cInputDebounce  mcPir_Deb;


    // --- Pins
    cGpPin mcRotDsA; // Rotatory Encoder A
    cGpPin mcRotDsB; // Rotatory Encoder B

    cGpPin mcRotDsD; // Rotatory Encoder Switch

    cGpPin mcPir;    // Person Sensor

    cUserInterface()
      : mu8RotDsD_Deb(mcRotDsD.ui8Get()),
        mcRotDsA_Deb(mcRotDsA.ui8Get(), 3, 3),
        mcRotDsB_Deb(mcRotDsB.ui8Get(), 3, 3),
        mcPir_Deb(0, 50, 1000 * 60 * 10), /* 10min = 10 * 60 * 1000ms */
        mcRotDsA(GPIOC_BASE, 11, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcRotDsB(GPIOC_BASE, 12, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcRotDsD(GPIOD_BASE,  2, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcPir(   GPIOC_BASE, 10, GPIO_Mode_IN_FLOATING, GPIO_Speed_10MHz, 0)
    {
      mbKeyPressed = False;
      mi8RotDir = 0;
      mu8RotCnt = 128;
      mu8Cnt1ms = 0;

      mu8RotDsD_State = mcRotDsD.ui8Get();
      mcRot_State = (mcRotDsA.ui8Get() << 1) + mcRotDsB.ui8Get();
    }


    void vSwitchSm10ms()
    {
      switch (mu8RotDsD_State)
      {
        case 0: // Switch pressed
          if (mu8RotDsD_Deb.u8Get() == 1)
          {
            mu8RotDsD_State = mu8RotDsD_Deb.u8Get();
            mbKeyPressed = True;
          }
          break;
        case 1:
          if (mu8RotDsD_Deb.u8Get() == 0)
          {
            mu8RotDsD_State = mu8RotDsD_Deb.u8Get();
          }
          break;
      }
    }

    void vRotSm1ms()
    {
      u8 lcRot_StateNew;

      lcRot_StateNew = (mcRotDsA_Deb.u8Get() << 1) + mcRotDsB_Deb.u8Get();

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
              mu8RotSeq++;
            }
            break;
          case 2:
            switch (lcRot_StateNew)
            {
              case 1: if (mi8RotDir == -1)  mu8RotSeq++;  break;
              case 2: if (mi8RotDir ==  1)  mu8RotSeq++;  break;
            }
            break;
          case 3:
            if (lcRot_StateNew == 3)
            {
              u8  lu8RotInc = 5;
              i16 mi16RotCntNew = mu8RotCnt;
              mi16RotCntNew += (mi8RotDir * lu8RotInc);
              if (mi16RotCntNew <   0) mi16RotCntNew = 0;
              if (mi16RotCntNew > 255) mi16RotCntNew = 255;
              mu8RotCnt = (u8)mi16RotCntNew;
              
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
        vSwitchSm10ms();
        mcPir_Deb.u8Process(mcPir.ui8Get(), 10);
      }

      mcRotDsA_Deb.u8Process(mcRotDsA.ui8Get(), 1);
      mcRotDsB_Deb.u8Process(mcRotDsB.ui8Get(), 1);
      vRotSm1ms();
    }

    bool isKeyPressed()
    {
      if (mbKeyPressed)
      {
        mbKeyPressed = False;
        return True;
      }
      return False;
    }

    u8 u8GetRotCnt()
    {
      return mu8RotCnt;
    }

    i8 i8GetRotDir()
    {
      i8 li8RotDirSave = mi8RotDir;
      mi8RotDir = 0;
      return li8RotDirSave;
    }

    bool isHumanPresent()
    {
      return (mcPir_Deb.u8Get() == 1);
    }
};

cUserInterface mcUI;



class cLedAnimation
{
  public:
  cWs2812* mcWs2812;
  char     mcStrData[12];
  cStr     mcName;

  cLedAnimation(cWs2812* lcWs2812)
    : mcName(mcStrData, sizeof(mcStrData))
  {
    mcWs2812 = lcWs2812;
    mcName   = (rsz)"LedAnimTpl";
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
    for (u16 lu16i = 0; lu16i < 12; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
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

  void vSetLine(u8 u8LineIdx, u8 lR, u8 lG, u8 lB)
  {
    u8LineIdx  = u8LineIdx % 4;
    u8LineIdx *= 3;

    mcWs2812->vSetLedPxl(u8LineIdx + 0, lR, lG, lB);
    mcWs2812->vSetLedPxl(u8LineIdx + 1, lR, lG, lB);
    mcWs2812->vSetLedPxl(u8LineIdx + 2, lR, lG, lB);
  }


  void vInit() override
  {
    mu8LineIdx = 0;
    mu16Timer = 0;

    vSetLine(mu8LineIdx + 0, R / 1, G / 2, B / 1);
    vSetLine(mu8LineIdx + 1, R / 2, G / 2, B / 2);
    vSetLine(mu8LineIdx + 2, 0,     0,     0);
    vSetLine(mu8LineIdx + 3, R / 2, G / 2, B / 2);
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 120)
    {
      mu16Timer = 0;
      mu8LineIdx++;

      //vSetLine(mu8LineIdx + 0, R /  1, G /  1, B /  1);
      //vSetLine(mu8LineIdx + 1, R / 16, G / 16, B / 16);
      //vSetLine(mu8LineIdx + 2, 0,      0,      0);
      //vSetLine(mu8LineIdx + 3, R / 16, G / 16, B / 16);
      vSetLine(mu8LineIdx + 0, R, G, B);
      vSetLine(mu8LineIdx + 1, 0, 0, 0);
      vSetLine(mu8LineIdx + 2, 0, 0, 0);
      vSetLine(mu8LineIdx + 3, 0, 0, 0);
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
  u8   mu8RingIdx;
  u16  mu16Timer;
  cLedAnimation_Warp(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB, rsz lszName)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;
    mcName = lszName;
  }

  void vSetRing(u8 u8RingIdx, u8 lR, u8 lG, u8 lB)
  {
    u8RingIdx = u8RingIdx % 3;

    mcWs2812->vSetLedPxl(u8RingIdx +  0, lR, lG, lB);
    mcWs2812->vSetLedPxl(u8RingIdx +  3, lR, lG, lB);
    mcWs2812->vSetLedPxl(u8RingIdx +  6, lR, lG, lB);
    mcWs2812->vSetLedPxl(u8RingIdx +  9, lR, lG, lB);
  }


  void vInit() override
  {
    mu8RingIdx = 0;
    mu16Timer = 0;

    vSetRing(0, R / 4, G / 4, B / 4);
    vSetRing(1, R / 4, G / 4, B / 4);
    vSetRing(2, R / 4, G / 4, B / 4);
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 100)
    {
      mu16Timer = 0;

      vSetRing(0, R / 8, G / 8, B / 8);
      vSetRing(1, R / 8, G / 8, B / 8);
      vSetRing(2, R / 8, G / 8, B / 8);
      
      mu8RingIdx++;
      switch (mu8RingIdx)
      {
        case 4: vSetRing(0, R, G, B); break;
        case 5: vSetRing(1, R, G, B); break;
        case 6: vSetRing(2, R, G, B); break;
        case 8: mu8RingIdx = 0; break;
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
  u8   R[LEDCNT], G[LEDCNT], B[LEDCNT];
  u8   SetR[LEDCNT], SetG[LEDCNT], SetB[LEDCNT];
  i8   IncR[LEDCNT], IncG[LEDCNT], IncB[LEDCNT];

  cLedAnimation_Bunt1Rgb(cWs2812* lcWs2812)
    : cLedAnimation(lcWs2812)
  {
    mcName = (rsz)"Bunt1Rgb";
  }

  void vRandCol()
  {
    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
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
    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
  }

  void vInit() override
  {
    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
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
    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
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
  u8   R[LEDCNT], G[LEDCNT], B[LEDCNT];
  i8   IncR[LEDCNT], IncG[LEDCNT], IncB[LEDCNT];

  u16  mu16Timer;

  cLedAnimation_Bunt2Rgb(cWs2812* lcWs2812)
    : cLedAnimation(lcWs2812)
  {
    mcName = (rsz)"Bunt2Rgb";
  }

  void vShow()
  {
    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
    {
      mcWs2812->vSetLedPxl(lu16Idx, R[lu16Idx], G[lu16Idx], B[lu16Idx]);
    }
  }

  void vInit() override
  {
    mu16Timer = 0;

    for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
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

      for (u16 lu16Idx = 0; lu16Idx < LEDCNT; lu16Idx++)
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
    for (u16 lu16i = 0; lu16i < 12; lu16i++)
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

class cLedAnimation_Bunt4Rgb : public cLedAnimation_Static
{
public:
  i16 SetR, SetG, SetB;
  i8  IncR, IncG, IncB;
  u16 mu16Timer;
  u16 Cnt;
  cLedAnimation_Bunt4Rgb(cWs2812* lcWs2812)
    : cLedAnimation_Static(lcWs2812, mcData.mData.u8CustR, mcData.mData.u8CustG, mcData.mData.u8CustB, (rsz)"Bunt4RgbPir")
  {
  }

  void vShow()
  {
    for (u16 lu16i = 0; lu16i < 12; lu16i++)
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
      if (Cnt > 100)
      {
        if (mcUI.mcPir.ui8Get())
        {
          Cnt = 0;
          vRand();
        }
      }

      if (Cnt > 2000)
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
    for (u16 lu16i = 0; lu16i < 12; lu16i++)
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


cLedAnimation_Static    mcLA_JustWhite(    &mcWs2812, 255, 255, 255, (rsz)"JustWhite");
cLedAnimation_Static    mcLA_JustRed(      &mcWs2812, 255, 0,   0  , (rsz)"JustRed");
cLedAnimation_Static    mcLA_JustGreen(    &mcWs2812, 0,   255, 0  , (rsz)"JustGreen");
cLedAnimation_Static    mcLA_JustBlue(     &mcWs2812, 0,   0,   255, (rsz)"JustBlue");
cLedAnimation_Static    mcLA_JustYellow(   &mcWs2812, 255, 255, 0  , (rsz)"JustYellow");
cLedAnimation_Static    mcLA_JustPink(     &mcWs2812, 255, 0,   255, (rsz)"JustPink");
cLedAnimation_Static    mcLA_JustTurquoise(&mcWs2812, 0,   255, 255, (rsz)"JustOcean");
cLedAnimation_Alarm     mcLA_AlarmBlue(&mcWs2812, 0, 0, 255, (rsz)"AlarmBlue");
cLedAnimation_AlarmRgb  mcLA_AlarmRgb(&mcWs2812);
cLedAnimation_StaticRgb mcLA_JustRgb(      &mcWs2812);
cLedAnimation_Warp      mcLA_WarpRed(      &mcWs2812, 255,   0,   0, (rsz)"WarpRed");
cLedAnimation_WarpRgb   mcLA_WarpRgb(      &mcWs2812);
cLedAnimation_Bunt1Rgb  mcLA_Bunt1Rgb(     &mcWs2812);
cLedAnimation_Bunt2Rgb  mcLA_Bunt2Rgb(     &mcWs2812);
cLedAnimation_Bunt3Rgb  mcLA_Bunt3Rgb(     &mcWs2812);
cLedAnimation_Bunt4Rgb  mcLA_Bunt4Rgb(     &mcWs2812);
cLedAnimation_PulseRgb  mcLA_PulseRgb(     &mcWs2812);


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
                                  &mcLA_Bunt4Rgb,
                                  &mcLA_PulseRgb,
                                  NULL
                                };


u16 mu16LedAnimationIdx = 0;



void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}


void DMA1_Channel4_IRQHandler(void)
{
  DMA1->INTFCR = DMA1_IT_TC4;
  DMA1_Channel4->CFGR &= ~(1 << 0); // ~EN
  mcI2C2_Board.I2C_EV_DMAHandler(cComNode::tenEvent::enEvDmaRxTc);
}

void DMA1_Channel5_IRQHandler(void)
{
  DMA1->INTFCR = DMA1_IT_TC5;
  DMA1_Channel5->CFGR &= ~(1 << 0); // ~EN
  //mcI2C1.I2C_EV_DMAHandler(cComNode::tenEvent::enEvDmaTxTc);
}

void I2C1_EV_IRQHandler(void)
{
  mcI2C2_Board.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C2_Board.I2C_ER_IRQHandler();
}


void DMA1_Channel2_IRQHandler(void)
{
  mcWs2812.Dma_vIrqTc();
}


void MAIN_vTick1msLp()
{
  mcUI.vTick1msLp();
  mcI2C2_Board.bStartNext();
}

u8 mu8RotCntOld = 0;

void MAIN_vTick10msLp()
{
  static volatile u8 lu8BrightnessOld = 128;
  static u16 lu16Minute = 0;
  static i16 li16TempAlarm = 0;

  mcWs2812.vTick10msLp();

  //mcTestLed.vSet(mcUI.mcPir.ui8Get());

  mcWs2812.vEnable();

  if (mcUI.isKeyPressed())
  {
    mu16LedAnimationIdx = mu16LedAnimationIdx + 1;
    if (mLstLedAnims[mu16LedAnimationIdx] == NULL) mu16LedAnimationIdx = 0;
    mLstLedAnims[mu16LedAnimationIdx]->vInit();
    mcData.mData.u16LedAnimationIdx = mu16LedAnimationIdx;
    mcData.vUpdate();
  }

  u8 lu8Brightness; 
  u8 lu8RotCnt;
  
  lu8Brightness = lu8RotCnt = mcUI.u8GetRotCnt();

  if (mu8RotCntOld != lu8RotCnt)
  {
    mu8RotCntOld = lu8RotCnt;
    mcData.mData.u16Brigthness = lu8RotCnt;
    mcData.vUpdate();
  }

  if (mcData.mData.bPirEnable)
  {
    lu8Brightness *= mcUI.isHumanPresent();
  }

  lu16Minute++;
  if (lu16Minute > 6000)
  {
    lu16Minute = 0;

    // Einfache Temperatur Regelung
    if (mcWs2812.i16GetNtcTemp() > 70)
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

  // Am Ende starten, damit er bis zum nächsten Call fertig ist
  ADC1->CTLR2 |= (1 << 22); // ADC Scan SWSTART
}


void MAIN_vTick100msLp(void)
{
  mcLm75.vCmdReadTemp();
  mcI2C2_Board.vTick10ms();
}


void MAIN_vTick1000msLp(void)
{
  mcTestLed.Toggle();
}


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


class cCliCmd_Status : public cCliCmd
{
  u16 mu16Calls;

public:
  cCliCmd_Status() :cCliCmd((const char*)"Status", (const char*)"Shows Status") {}
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
          lszStr.Setf((rsz)"  Cur:     %d[mA]", mcWs2812.i16GetCurrent()); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Temp:    %d[°C]", mcWs2812.i16GetNtcTemp()); lcCli->bPrintLn(lszStr);
          break;
        case 1:
          lcCli->bPrintLn((rsz)"Pir Status:");
          lszStr.Setf((rsz)"  Enable:    %d", mcData.mData.bPirEnable); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Status:    %d", mcUI.isHumanPresent()); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Pir:       %d", mcUI.mcPir.ui8Get()); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Time:      %d/%d[s]", mcUI.mcPir_Deb.mu32StableCnt / 1000, mcData.mData.u32PirOnTime / 1000); lcCli->bPrintLn(lszStr);
          break;
        case 2:
          lcCli->bPrintLn((rsz)"Cpu Status:");
          lszStr.Setf((rsz)"  Core:      %d[Mhz]", cClockInfo::mstClocks.HCLK_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  PClk1:     %d[Mhz]", cClockInfo::mstClocks.PCLK1_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  PClk2:     %d[Mhz]", cClockInfo::mstClocks.PCLK2_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Adc:       %d[Mhz]", cClockInfo::mstClocks.ADCCLK_Frequency / 1000000); lcCli->bPrintLn(lszStr);
          break;
        case 3:
          lcCli->bPrintLn((rsz)"Eep Status:");
          lszStr.Setf((rsz)"  Cnt:      %d", mcData.mData.u32Cnt); lcCli->bPrintLn(lszStr);
          break;
        case 4:
          lcCli->bPrintLn((rsz)"Adc Status:");
          lszStr.Setf((rsz)"  Cal1T:    %d", mcWs2812.mi16ADC1_Calibrattion_Val); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Cal2S:    %d", mcWs2812.mi16ADC2_Calibrattion_Val); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Raw1T:    %d", mcWs2812.mAdcResult[0] & 0xffff); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Raw2S:    %d", mcWs2812.mAdcResult[0] >> 16);    lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Val1T:    %d", mcWs2812.mi16TempDigi); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  Val2S:    %d", mcWs2812.mi16ShuntDigi); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  SOft:     %d", mcWs2812.mi16ShuntOffset); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  VRef:     %d", mcWs2812.mAdcResult[4] & 0xffff); lcCli->bPrintLn(lszStr);
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


class cCliCmd_TempCal : public cCliCmd
{
public:
  cCliCmd_TempCal() :cCliCmd((const char*)"TempCal", (const char*)"Set/Get temp. calibration: TempCal Val1,Val2") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      if (lcParam.CountOf(',') == 1)
      {
        u16 lu16PosKomma1 = lcParam.Instr(0, ',');

        if (lu16PosKomma1 != (u16)-1)
        {
          lszStr = lcParam;
          lszStr.Mid(0, lu16PosKomma1 - 0);
          i16 li16Val1 = lszStr.Atoi();

          lszStr = lcParam;
          lszStr.Mid(lu16PosKomma1 + 1, 0xFFFF);
          i16 li16Val2 = lszStr.Atoi();

          mcData.mData.u8TempAdc1 = li16Val1;
          mcData.mData.u8TempAdc2 = li16Val2;
          mcWs2812.mu16TempAdc1 = mcData.mData.u8TempAdc1;
          mcWs2812.mu16TempAdc2 = mcData.mData.u8TempAdc2;
          mcData.vUpdate();
          mLstLedAnims[mu16LedAnimationIdx]->vInit();
        }
      }
    }

    lszStr.Setf((rsz)"  Temp. cal: %d,%d", mcData.mData.u8TempAdc1, mcData.mData.u8TempAdc2); lcCli->bPrintLn(lszStr);

    return True;
  }
};


cCliCmd_Version      mcCliCmd_Ver;
cCliCmd_Help         mcCliCmd_Help;
cCliCmd_LedAnimList  mcCliCmd_LedAnimList;
cCliCmd_Status       mcCliCmd_Status;
cCliCmd_AnimIdx      mcCliCmd_AnimIdx;
cCliCmd_Rgb          mcCliCmd_Rgb;
cCliCmd_TempCal      mcCliCmd_TempCal;

cRingBufDynT<u8, u16> mcCliDataIn(1024);
cRingBufDynT<u8, u16> mcCliDataOut(1024);

cCliCmdList mcCliCmdList((cCliCmd* []) {&mcCliCmd_Ver, &mcCliCmd_Help, &mcCliCmd_LedAnimList, &mcCliCmd_Status, &mcCliCmd_AnimIdx, &mcCliCmd_Rgb, &mcCliCmd_TempCal}, 7);
cCli mcCli(&mcCliDataIn, &mcCliDataOut, &mcCliCmdList, 4);


void MAIN_vInitSystem(void)
{
  /* Tim2 init */
  SystemCoreClock = cClockInfo::mstClocks.HCLK_Frequency;
  
  USBHS_RCC_Init();
  
  CdcCli_vInit();

  /* USB20 device init */
  USBHS_Device_Init(ENABLE);


  // Daten setzen
  mu16LedAnimationIdx = mcData.mData.u16LedAnimationIdx;
  if (mu16LedAnimationIdx > (sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1)
  {
    mu16LedAnimationIdx = 0;
  }

  mu8RotCntOld = mcUI.mu8RotCnt = mcData.mData.u16Brigthness;
  mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
  mcWs2812.mu16TempAdc1 = mcData.mData.u8TempAdc1;
  mcWs2812.mu16TempAdc2 = mcData.mData.u8TempAdc2;


  mcWs2812.vDisable();
  ADC1->CTLR2 |= (1 << 22); // SWSTART
  for (u32 lit = 0; lit < 100000; lit++);
  i32 li32ShuntOffset;  
  li32ShuntOffset = mcWs2812.i16GetCurrent() + mcWs2812.i16GetCurrent() + mcWs2812.i16GetCurrent() + mcWs2812.i16GetCurrent();
  mcWs2812.mi16ShuntOffset = li32ShuntOffset / 4;


  mLstLedAnims[mu16LedAnimationIdx]->vInit();
  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp    /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    CdcCli_vUsbRx_Deal(&mcCliDataIn);
    CdcCli_vUsbTx_Deal(&mcCliDataOut);
    mcCli.bDoProcess();
  }
}


static void SetSysClockTo96_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;
  } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    /* PCLK2 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    /* PCLK1 = HCLK */
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

    /*  PLL configuration: PLLCLK = HSE * 4 * 24Mhz = 96 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));

    #ifdef CH32V30x_D8
      RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL4);
    #else
      RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL4_EXTEN);
    #endif

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
    /*
     * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error
     */
  }
}


void MainSystemRccInit(void)
{
  #ifdef CH32V30x_D8C
    RCC->CFGR0 &= (uint32_t)0xF8FF0000;
  #else
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
  #endif 

  RCC->CTLR &= (uint32_t)0xFEF6FFFF;
  RCC->CTLR &= (uint32_t)0xFFFBFFFF;
  RCC->CFGR0 &= (uint32_t)0xFF80FFFF;

  #ifdef CH32V30x_D8C
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;
  #else
    RCC->INTR = 0x009F0000;
  #endif
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  MainSystemRccInit();
  SetSysClockTo96_HSE();

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  SystemCoreClockUpdate();
  cClockInfo::Update();
}

