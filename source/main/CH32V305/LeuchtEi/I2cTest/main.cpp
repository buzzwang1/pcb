
#include "main.h"


// CH32V305 => D8C, Core V4F IMAFC
// RiscV 32b MCU, (144 MHz max)
// Rom 128KB
// 
// 
// 
// 24Mhz Quarz
// 
// PC02: LED
// 
// WS2812
//   PC08: Power MOSFET; 1 = Enable ; 0 = Disable
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
// Rotatory Switch:
//   PD02: D
//   PC12: B
//   PC11: A
// 
// PIR:
//   PC10: Pir
// 
// 
// I2C: 
//   PB10: I2C2_SCL
//   PB11: I2C2_SDA
//     0x3C: Display
//     0x40 : Temp + Hum Platform
//     0x48 : Temp Led
//     0x50 : EEPROM
//     0x51 : EEPROM
//     0x52 : EEPROM
//     0x53 : EEPROM
// 
// SPI:
// Flash:
//   PB12: SPI2_CS
//   PB13: SPI2_CLK
//   PB14: SPI2_MISO
//   PB15: SPI2_MOSI
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

#define LED_PER_BLOCK 12
#define LEDMAXLEDCNT  12 * 6

//LED<GPIOB_BASE, 14> mcTestLed; // v0
LED<GPIOC_BASE, 2> mcTestLed; // v9

cClockInfo mcClockInfo; // Must be at the front

u8 mu8MyRgbMemory[LEDCTRL_RGB_MEMSIZE(LEDMAXLEDCNT)];
u8 mu8MyLedMemory[LEDCTRL_LED_MEMSIZE(LEDMAXLEDCNT)];
cWs2812 mcWs2812(mu8MyRgbMemory, mu8MyLedMemory, LEDMAXLEDCNT);

u16 mu16LedCnt = LED_PER_BLOCK;

cGpPin      mcI2c2_SCL_Board(GPIOB_BASE, 10, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 1);
cGpPin      mcI2c2_SDA_Board(GPIOB_BASE, 11, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 1);
cI2cMaster  mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 8);

cI2cEep     mcEep(&mcI2C2_Board, 0xA0);
cTmp102     mcTmp(&mcI2C2_Board, TMP102_I2C_ADDRESS_DEFAULT);
cSSD1306    mcSSD1306(&mcI2C2_Board, 0x78);
cHTU21D     mcHTU21D(&mcI2C2_Board, HTU21D_I2C_ADDRESS_DEFAULT);


#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory1[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm1(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory1);
cScreen_Bpp1_1G          mcScreen1(&mcBm1);

uint8                    mDisplayMemory2[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm2(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory2);
cScreen_Bpp1_1G          mcScreen2(&mcBm2);

cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);

// Fall was geändert wurde dann wird das Menu im Display solange angezeigt
u32 mu32GuiShowBrigthness_ms  = 4000;
u32 mu32GuiShowBrigthness2_ms = 0;
u32 mu32GuiShowAnimationIndex_ms = 4000;


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
    u16  u16DisplayOffset;
    u8   Res2[32 - 2 - 2 - 2 - 2 - 2];
    // 64

    u32  u32PirOnTime;
    u8   bPirEnable;
    u8   u8RotInv;
    u8   Res3[32 - 4 - 1 - 1];
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
    mData.u32PirOnTime        = 1000 * 60 * 10; /* 10min = 10 * 60 * 1000ms */;
    mData.u16Brigthness       = 64; // 50%
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
      mData.u32Cnt              = 0;
      mData.bPirEnable          = True;
      mData.u32PirOnTime        = 1000 * 60 * 10; /* 10min = 10 * 60 * 1000ms */;
      mData.u16Brigthness       = 128; // 50%
      mData.u16LedAnimationIdx  = 0;
      mData.u16StartTimerReload = 1000 * 5; // 5s
      mData.u16LedBlockCnt      = 1;
      mData.u8CustR             = 255;
      mData.u8CustG             = 255;
      mData.u8CustB             = 255;
      mData.u8TempAdc1          = 100;
      mData.u8TempAdc2          =  77;
      mData.u16DisplayOffset    = 0;
      vStore();
    }
  }


  virtual void vStore() = 0;
};


class cFlashData: public cData
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

  u16     mu16StoreTimer;

  cFlashData()
  {
    mu16StoreTimer = 0;
    vLoad();
    vCheckAndSetDefault();
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
    bool mbKeyPressed;
    i8   mi8RotDir; // -1 = CCW, 0 = Nothing; 1 = CW
    u8   mu8RotInv; // Flip rotatory switch's CW<->CCW direction. 
                    // Depending on switch, output for CW and CCW are different
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
        mcRotDsA_Deb(mcRotDsA.ui8Get(), 2, 2),
        mcRotDsB_Deb(mcRotDsB.ui8Get(), 2, 2),
        mcPir_Deb(0, 50, 1000 * 60 * 10), /* 10min = 10 * 60 * 1000ms */
        mcRotDsA(GPIOC_BASE, 11, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcRotDsB(GPIOC_BASE, 12, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcRotDsD(GPIOD_BASE,  2, GPIO_Mode_IPU, GPIO_Speed_10MHz, 0),
        mcPir(   GPIOC_BASE, 10, GPIO_Mode_IN_FLOATING, GPIO_Speed_10MHz, 0)
    {
      mbKeyPressed = False;
      mi8RotDir = 0;
      mu8RotInv = 0;
      mu8RotCnt = 128;
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

  // Ein Block hat 4 Seiten, je 3 Leds => 12 LEDs (0..11)
  // Anordung:
  //         Block1              |    |        Block2             |        Block3 
  //    |S1    S2    S3    S4    |    | S4     S1     S2     S3   | S3     S4     S1     S2
  //----+------------------------+----+---------------------------+-------------------------
  // H3 |2-,   5-,   8-,   11-,  | H6 | 14-,   17-,   20-,   23-, | 26-,   29-,   32-,   35
  // H2 |1  \  4  \  7  \  10  \ | H5 | 13  \  16  \  19  \  22  \| 25  \  28  \  31  \  34 
  // H1 |0   `-3   `-6   `-9    -| H4 |-12   `-15   `-18   `-21   |-24   `-27   `-30   `-33 
  //
  // Ein weiterer Block begint an der letzten LED des vorherigen Blocks
  // D.h. Block2.S1 hängt an Block1.S4 ist also um 90° gedreht

  u8 u8GetBlocks()
  {
    return mcData.mData.u16LedBlockCnt;
  }
  
  // Seite: 1..4
  // Höhe:  1..
  // Beispiel:
  // Seite1 Led 0 => LedIdx:  0
  // Seite1 Led 1 => LedIdx:  1
  // Seite1 Led 2 => LedIdx:  2
  // Seite1 Led 3 => LedIdx: 15
  // Seite4 Led 0 => LedIdx:  9
  // Seite4 Led 1 => LedIdx: 10
  // Seite4 Led 2 => LedIdx: 11
  // Seite4 Led 3 => LedIdx: 12
  u8 u8GetLedIdx_SH(u8 lu8S, u8 lu8H)
  {
    if ((lu8S > 0) && (lu8H > 0))
    {
      // 3 LEDs pro Seite pro Block
      u8 lu8BlockIdx = (lu8H - 1) / 3;  // S1/H5 => B = 1
                                        // S4/H6 => B = 1

      // 3 LEDs pro Seite
      //                                      90° Drehung (bei jedem weiteren Block 3 addieren)
      u8 lu8SeitenOffset = (((lu8S - 1) * 3 + (lu8BlockIdx * 3)) % 12); // S1/H5 => ((1 - 1) * 3 + (1 * 3)) % 12 = (0 + 3) % 12 = 3
                                                                        // S4/H6 => ((4 - 1) * 3 + (1 * 3)) % 12 = (9 + 3) % 12 = 0

      u8 lu8LedIdx = lu8BlockIdx * 12 + lu8SeitenOffset + ((lu8H - 1) % 3); // S1/H5 => (1 * 12) + 3 + 1 = 16
                                                                            // S4/H6 => (1 * 12) + 0 + 2 = 14

      return lu8LedIdx;
    }
    return 0;
  }


  void vSetSLine(u8 u8S, u8 lR, u8 lG, u8 lB)
  {
    // 3 LEDs pro Seite pro Block
    u8 lu8BlockCnd = u8GetBlocks();

    for (u8 lu8H = 1; lu8H <= lu8BlockCnd * 3; lu8H++)
    {
      u8 lu8LedIdx = u8GetLedIdx_SH(u8S, lu8H);
      mcWs2812->vSetLedPxl(lu8LedIdx, lR, lG, lB);
    }
  }

  void vSetRing(u8 u8H, u8 lR, u8 lG, u8 lB)
  {
    mcWs2812->vSetLedPxl(u8GetLedIdx_SH(1, u8H), lR, lG, lB);
    mcWs2812->vSetLedPxl(u8GetLedIdx_SH(2, u8H), lR, lG, lB);
    mcWs2812->vSetLedPxl(u8GetLedIdx_SH(3, u8H), lR, lG, lB);
    mcWs2812->vSetLedPxl(u8GetLedIdx_SH(4, u8H), lR, lG, lB);
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
    mu8LineIdx = 0;
    mu16Timer = 0;

    vSetSLine(mu8LineIdx + 0, R / 1, G / 2, B / 1);
    vSetSLine(mu8LineIdx + 1, R / 2, G / 2, B / 2);
    vSetSLine(mu8LineIdx + 2, 0,     0,     0);
    vSetSLine(mu8LineIdx + 3, R / 2, G / 2, B / 2);
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
      vSetSLine(mu8LineIdx + 0, R, G, B);
      vSetSLine(mu8LineIdx + 1, 0, 0, 0);
      vSetSLine(mu8LineIdx + 2, 0, 0, 0);
      vSetSLine(mu8LineIdx + 3, 0, 0, 0);
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
  i16  mi16RingIdx;
  u16  mu16Timer;
  cLedAnimation_Warp(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB, rsz lszName)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;

    mcName = lszName;
  }

  void vInit() override
  {
    mi16RingIdx = -10;
    mu16Timer   = 0;

    for (u16 lu16i = 0; lu16i < mu16LedCnt; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R/8, G/8, B/8);
    }
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

      if (mi16RingIdx > 0)
      {
        vSetRing(mi16RingIdx, R, G, B);
      }
                  
      mi16RingIdx++;
      if (mi16RingIdx > u8GetBlocks() * 3)
      {
        mi16RingIdx = -10;
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
}

void DMA1_Channel5_IRQHandler(void)
{
  DMA1->INTFCR = DMA1_IT_TC5;
  DMA1_Channel5->CFGR &= ~(1 << 0); // ~EN
  mcI2C2_Board.I2C_EV_DMAHandler(cComNode::tenEvent::enEvDmaRxTc);
}

void I2C2_EV_IRQHandler(void)
{
  mcI2C2_Board.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
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

void vSetGuiShow(u32 lu32GuiShowBrigthness_ms, u32 lu32GuiShowBrigthness2_ms, u32 lu32GuiShowAnimationIndex_ms)
{
  // Fall was geändert wurde dann wird das Menu im Display solange angezeigt
  mu32GuiShowBrigthness_ms = lu32GuiShowBrigthness_ms;
  mu32GuiShowBrigthness2_ms = lu32GuiShowBrigthness2_ms;
  mu32GuiShowAnimationIndex_ms = lu32GuiShowAnimationIndex_ms;
}


u8 mu8RotCntOld = 0;
u8 mu8Brightness = 0;

void MAIN_vTick10msLp()
{
  static volatile u8 mu8BrightnessOld = 128;
  static u16 lu16Tick_ms = 0;
  static i16 li16TempAlarm = 0;
  static i16 li16TempOld = 0;

  mcWs2812.vTick10msLp();

  //mcTestLed.vSet(mcUI.mcPir.ui8Get());

  mcWs2812.vEnable();

  if (mcUI.isKeyPressed())
  {
    mu16LedAnimationIdx = mu16LedAnimationIdx + 1;
    if (mLstLedAnims[mu16LedAnimationIdx] == NULL) mu16LedAnimationIdx = 0;
    mLstLedAnims[mu16LedAnimationIdx]->vInit();
    mcData.mData.u16LedAnimationIdx = mu16LedAnimationIdx;
    vSetGuiShow(0, 0, 4000);
    mcData.vUpdate();
  }

  u8 lu8RotCnt;
  
  mu8Brightness = lu8RotCnt = mcUI.u8GetRotCnt();

  if (mu8RotCntOld != lu8RotCnt)
  {
    mu8RotCntOld = lu8RotCnt;
    mcData.mData.u16Brigthness = lu8RotCnt;
    mcData.vUpdate();
  }

  if (mcData.mData.bPirEnable)
  {
    mu8Brightness *= mcUI.isHumanPresent();
  }

  bool lbTempAlarmChanged = False;
  i16 li16Temp = mcTmp.i16GetTemp();

  lu16Tick_ms+=10;
  if (lu16Tick_ms > 15000) // 15s
  {
    lu16Tick_ms = 0;

    // Einfache Temperatur Regelung    
    // if (mcWs2812.i16GetNtcTemp() > 70) // analog
    if ((mcTmp.i16GetTemp() > 71) && (li16TempOld <= li16Temp)) // digital
    {    
      if (li16TempAlarm <= -250)
      {
        li16TempAlarm = -250;
      }
      else
      {
        li16TempAlarm -= 5;
        lbTempAlarmChanged = True;
      }
    }
    else if ((mcTmp.i16GetTemp() < 69) && (li16TempOld >= li16Temp))
    {
      if (li16TempAlarm >= 0)
      {
        li16TempAlarm = 0;
      }
      else
      {
        li16TempAlarm += 5;
        lbTempAlarmChanged = True;
      }
    }
    li16TempOld = li16Temp;
  }

  if (mu8BrightnessOld != mu8Brightness) // Benutzer hat Helligkeit verändert
  {
    li16TempAlarm = 0;
  }



  if ((mu8BrightnessOld != mu8Brightness) || (lbTempAlarmChanged))
  {
    if (mu8Brightness + li16TempAlarm >= 0)
    {
      mcWs2812.vSetBrigthness(mu8Brightness + li16TempAlarm);
    }
    else
    {
      mcWs2812.vSetBrigthness(mu8Brightness + li16TempAlarm);
    }

    if (lbTempAlarmChanged)
      vSetGuiShow(0, 1000, 0);
    else
      vSetGuiShow(4000, 0, 0);

    mu8BrightnessOld = mu8Brightness;
  }

  mLstLedAnims[mu16LedAnimationIdx]->vProcess10ms();
  mcWs2812.vShow();
  mcData.vTick10ms();
  mcI2C2_Board.vTick10ms();
  mcHTU21D.vTick10ms();  
  mcEep.vTick10ms();

  // Am Ende starten, damit er bis zum nächsten Call fertig ist
  ADC1->CTLR2 |= (1 << 22); // ADC Scan SWSTART
}


void MAIN_vItoa_HUD21D(int num, char8* str)
{
  int32 li32Val1 = 0;
  int32 li32Val2 = 0;
  char lszVal1[8] = "";
  char lszVal2[8] = "";
  char lszPkt[] = ",";

  li32Val1 = num / 100;
  li32Val2 = (num - (li32Val1 * 100)) / 10;

  cStrTools::uixItoa(li32Val1, lszVal1, 10);
  cStrTools::uixItoa(li32Val2, lszVal2, 10);

  cStrTools::szStrCpy(str, lszVal1);
  cStrTools::szStrCat(str, lszPkt);
  cStrTools::szStrCat(str, lszVal2);
}


void MAIN_vTick100msLp(void)
{
  static u32 lu32Counter_ms = 2000;
  static u8  lu8Tick = 0;

  mcTmp.vCmdReadTemp();

  // Wenn EEPROM Daten schreibt, dann keine Grafik updaten
  //if (mcEep.menCmd == cI2cEep::enCmdIdle)
  {
    if (mcSSD1306.mStatus.IsInit)
    {
      int32 li32Temp = 0;
      int32 li32Hum = 0;

      char lszValue[16] = "";
      char lszGrad[] = " °C";
      char lszProz[] = " %";
      cStr_Create(lszStr, 40);

      // Diese Werte nur einmal in der Sekunde einlesen, 
      // damit sie nicht so viel springen
      if (lu32Counter_ms >= 1000) // 1s
      {
        mcHTU21D.i8ReadHumidityNoHold_digit();
        mcHTU21D.i8ReadTemperatureNoHold_digit();

        lu32Counter_ms = 0;
      }
      else
      {
        lu32Counter_ms += 100;
      }

      li32Hum = mcHTU21D.i32CalcHumidity();
      li32Temp = mcHTU21D.i32CalcTemperature();


      mcScreen1.vFill(0);

      if (mu32GuiShowBrigthness_ms > 100)
      {
        mu32GuiShowBrigthness_ms -= 100;

        u32 lu32Brightness = (((u32)mu8Brightness * 100) / 256);
        if (mu8Brightness > 250) lu32Brightness = 100;

        lszStr.Setf((rsz)"%d%%", lu32Brightness);

        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(7, 10, lszStr.ToString(), &mcScreen1);

        for (u16 lu16x = 0; lu16x < MAIN_nDISPLAY_X; lu16x++)
        {
          for (u16 lu16y = 0; lu16y < MAIN_nDISPLAY_Y; lu16y++)
          {
            mcScreen2.vPpAbsRaw(lu16x, lu16y, mcScreen1.u32GpAbsRaw(lu16x / 4, lu16y / 4));
          }
        }
      }
      else
      {
        mu32GuiShowBrigthness_ms = 0;

        if (mu32GuiShowBrigthness2_ms > 100)
        {
          mu32GuiShowBrigthness2_ms -= 100;

          u32 lu32Brightness = (((u32)mcWs2812.mu8Brigthness * 100) / 256);
          if (mcWs2812.mu8Brigthness > 250) lu32Brightness = 100;

          lszStr.Setf((rsz)"%d%%", lu32Brightness);

          cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(7, 10, lszStr.ToString(), &mcScreen1);

          for (u16 lu16x = 0; lu16x < MAIN_nDISPLAY_X; lu16x++)
          {
            for (u16 lu16y = 0; lu16y < MAIN_nDISPLAY_Y; lu16y++)
            {
              mcScreen2.vPpAbsRaw(lu16x, lu16y, mcScreen1.u32GpAbsRaw(lu16x / 4, lu16y / 4));
            }
          }
        }
        else
        {
          mu32GuiShowBrigthness2_ms = 0;

          if (mu32GuiShowAnimationIndex_ms > 100)
          {
            mu32GuiShowAnimationIndex_ms -= 100;

            lszStr = mLstLedAnims[mu16LedAnimationIdx]->mcName;
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, 13, lszStr.ToString(), &mcScreen1);

            lszStr.Setf((rsz)"%d-%d", mu16LedAnimationIdx + 1, ((sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1));
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, 23, lszStr.ToString(), &mcScreen1);

            for (u16 lu16x = 0; lu16x < MAIN_nDISPLAY_X; lu16x++)
            {
              for (u16 lu16y = 0; lu16y < MAIN_nDISPLAY_Y; lu16y++)
              {
                mcScreen2.vPpAbsRaw(lu16x, lu16y, mcScreen1.u32GpAbsRaw(lu16x / 2, lu16y / 2));
              }
            }
          }
          else
          {
            mu32GuiShowAnimationIndex_ms = 0;
            MAIN_vItoa_HUD21D(li32Temp, lszValue);
            cStrTools::szStrCat(lszValue, lszGrad);
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, 13, (char8*)"Temp.:", &mcScreen1);
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(32, 13, lszValue, &mcScreen1);

            MAIN_vItoa_HUD21D(li32Hum, lszValue);
            cStrTools::szStrCat(lszValue, lszProz);
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, 23, (char8*)"Hum.:", &mcScreen1);
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(32, 23, lszValue, &mcScreen1);

            for (u16 lu16x = 0; lu16x < MAIN_nDISPLAY_X; lu16x++)
            {
              for (u16 lu16y = 0; lu16y < MAIN_nDISPLAY_Y; lu16y++)
              {
                mcScreen2.vPpAbsRaw(lu16x, lu16y, mcScreen1.u32GpAbsRaw(lu16x / 2, lu16y / 2));
              }
            }

            lszStr.Setf((rsz)"iT: %d/70", mcTmp.i16GetTemp());
            cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, MAIN_nDISPLAY_Y - 4, lszStr.ToString(), &mcScreen2);
            
            if (mcData.mData.bPirEnable)
            {
              lszStr.Setf((rsz)"PIR: %d", mcUI.mcPir_Deb.mu32StableCnt / 1000);
              cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(MAIN_nDISPLAY_X / 2, MAIN_nDISPLAY_Y - 4, lszStr.ToString(), &mcScreen2);
            }
            //lszStr.Setf((rsz)"I: %dmA", mcWs2812.i16GetCurrent());
            
          }
        }
      }

      switch (lu8Tick & 7)
      {
        case 0: mcScreen2.vLine(1, 3, 5, 3, 1); break; // "-"
        case 1: mcScreen2.vLine(1, 1, 5, 5, 1); break; // "\"
        case 2: mcScreen2.vLine(3, 1, 3, 5, 1); break; // "|"
        case 3: mcScreen2.vLine(1, 5, 5, 1, 1); break; // "/"
        case 4: mcScreen2.vLine(1, 3, 5, 3, 1); break; // "-"
        case 5: mcScreen2.vLine(1, 1, 5, 5, 1); break; // "\"
        case 6: mcScreen2.vLine(3, 1, 3, 5, 1); break; // "|"
        case 7: mcScreen2.vLine(1, 5, 5, 1, 1); break; // "/"
      }

      if (mcEep.menCmd != cI2cEep::enCmdIdle)
      {
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(MAIN_nDISPLAY_X - 16, 6, (rsz)"EEP", &mcScreen2);
      }


      mcSSD1306.vShowScreen(mcScreen2.mpcBm->mpui8Data);
      mcSSD1306.Update();
    }
  }

  lu8Tick++;
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
          lszStr.Setf((rsz)"  ATemp:   %d[°C]", mcWs2812.i16GetNtcTemp()); lcCli->bPrintLn(lszStr);
          lszStr.Setf((rsz)"  DTemp:   %d[°C]", mcTmp.i16GetTemp()); lcCli->bPrintLn(lszStr);
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


class cCliCmd_BlockCnt : public cCliCmd
{
public:
  cCliCmd_BlockCnt() :cCliCmd((const char*)"BlockCnt", (const char*)"Set count of Led blocks") {}
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
      if ((li16Val >= 1) || (li16Val <= 6))
      {
        mcData.mData.u16LedBlockCnt = li16Val;
        mu16LedCnt = mcData.mData.u16LedBlockCnt * LED_PER_BLOCK;
        mcData.vUpdate();
      }
    }
    lszStr.Setf((rsz)"  Value: %d", mcData.mData.u16LedBlockCnt); lcCli->bPrintLn(lszStr);
    return True;
  }
};



class cCliCmd_Pir : public cCliCmd
{
public:
  cCliCmd_Pir() :cCliCmd((const char*)"Pir", (const char*)"Sets Pir Timeout in [s]") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 40);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      i32 li32Val = lcParam.Atoi();

      if (li32Val > 60 * 60) // max. 1 Stunde
      {
        li32Val = 60 * 60;
      }

      if (li32Val >= 1)
      {
        mcData.mData.bPirEnable = True;
        mcData.mData.u32PirOnTime = li32Val * 1000;
        mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
        mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
        mcData.vUpdate();
      }
      else
      {
        mcData.mData.bPirEnable = False;
        mcData.mData.u32PirOnTime = 0;
        mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
        mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
        mcData.vUpdate();
      }
    }

    lcCli->bPrintLn((rsz)"Pir 0 => Pir off => LED always on");
    lcCli->bPrintLn((rsz)"Pir x => LED stays on for x [s] after PIR activation");
    lcCli->bPrintLn((rsz)"Pir Status:");
    lszStr.Setf((rsz)"  Enable:    %d", mcData.mData.bPirEnable); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"  Status:    %d", mcUI.isHumanPresent()); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"  Pir:       %d", mcUI.mcPir.ui8Get()); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"  Time:      %d/%d[s]", mcUI.mcPir_Deb.mu32StableCnt / 1000, mcData.mData.u32PirOnTime / 1000); lcCli->bPrintLn(lszStr);
    return True;
  }
};


class cCliCmd_DisplayOft : public cCliCmd
{
public:
  cCliCmd_DisplayOft() :cCliCmd((const char*)"Do", (const char*)"Display Offset") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 40);

    // check for Parameter
    if (lcParam.Len() != 0)
    {
      i32 li32Val = lcParam.Atoi();

      mcSSD1306.mui8Offset = mcData.mData.u16DisplayOffset = li32Val;

      mcData.vUpdate();
    }

    lszStr.Setf((rsz)"Display Offset: %d", mcData.mData.u16DisplayOffset); lcCli->bPrintLn(lszStr);
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
cCliCmd_EepTest      mcCliCmd_EepTest;
cCliCmd_RotInv       mcCliCmd_RotInv;
cCliCmd_BlockCnt     mcCliCmd_BlockCnt;
cCliCmd_Pir          mcCliCmd_Pir;
cCliCmd_DisplayOft   mCliCmd_DisplayOft;

cRingBufDynT<u8, u16> mcCliDataIn(1024);
cRingBufDynT<u8, u16> mcCliDataOut(1024);

cCliCmdList mcCliCmdList((cCliCmd* []) {&mcCliCmd_Ver,         &mcCliCmd_Help, 
                                        &mcCliCmd_LedAnimList, &mcCliCmd_Status, 
                                        &mcCliCmd_AnimIdx,     &mcCliCmd_Rgb, 
                                        &mcCliCmd_TempCal,     &mcCliCmd_EepTest, 
                                        &mcCliCmd_RotInv,      &mcCliCmd_BlockCnt, 
                                        &mcCliCmd_Pir,         &mCliCmd_DisplayOft}, 12);

cCli mcCli(&mcCliDataIn, &mcCliDataOut, &mcCliCmdList, 4);


void MAIN_vTick1msLp_Init()
{
  mcI2C2_Board.bStartNext();
}

void MAIN_vTick10msLp_Init()
{
  mcI2C2_Board.vTick10ms();
  mcData.vTick10ms();
}



void MAIN_vInitSystem(void)
{
  /* Tim2 init */
  SystemCoreClock = cClockInfo::mstClocks.HCLK_Frequency;

  // Warten bis Eep geladen wurde
  // Init Callback nur um Eep zu laden
  mcWs2812.vDisable();
  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp_Init    /*1ms_LP*/,
                MAIN_vTick10msLp_Init   /*10ms_LP*/,
                NULL                    /*100ms_LP*/,
                NULL                    /*1s_LP*/);

  while ((mcData.mbLoad) && (!mcData.mbError)) {CycCall_vIdle();}

  // ------ EEP-Daten laden
  // Im Fehlerfall nochmals ein paar probieren, weil ...
  //    Time required after VCC is stable before the device can accept commands. 100 µs
  u8 lu8EepRetrys = 10;
  while ((mcData.mbError) && (lu8EepRetrys > 0))
  {
    mcData.mbError = False;
    while ((mcData.mbLoad) && (!mcData.mbError)) { CycCall_vIdle(); }
    lu8EepRetrys--;
  }


  // ----- USB COM-Port einrichten
  USBHS_RCC_Init();
  
  CdcCli_vInit();

  /* USB20 device init */
  USBHS_Device_Init(ENABLE);


  // ----- Daten setzen
  mu16LedAnimationIdx = mcData.mData.u16LedAnimationIdx;
  if (mu16LedAnimationIdx > (sizeof(mLstLedAnims) / sizeof(cLedAnimation*)) - 1)
  {
    mu16LedAnimationIdx = 0;
  }

  if ((mcData.mData.u16LedBlockCnt < 1) || (mcData.mData.u16LedBlockCnt > 6)) mcData.mData.u16LedBlockCnt = 1;
  mu16LedCnt = mcData.mData.u16LedBlockCnt * LED_PER_BLOCK;

  mu8RotCntOld = mcUI.mu8RotCnt = mcData.mData.u16Brigthness;
  mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;

  if (mcData.mData.u32PirOnTime > 0)
  {
    mcData.mData.bPirEnable = True;
    mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
    mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
  }
  else
  {
    mcData.mData.u32PirOnTime = 0;
    mcData.mData.bPirEnable = False;
    mcUI.mcPir_Deb.mu32DebounceTimeOff = mcData.mData.u32PirOnTime;
    mcUI.mcPir_Deb.mu32StableCnt       = mcData.mData.u32PirOnTime;
  }

  mcWs2812.mu16TempAdc1 = mcData.mData.u8TempAdc1;
  mcWs2812.mu16TempAdc2 = mcData.mData.u8TempAdc2;
  mcUI.vSetRotInv(mcData.mData.u8RotInv);

  mcSSD1306.mui8Offset = mcData.mData.u16DisplayOffset;

  //------ ADC Kalibirerung
  // Adc sollte schon vom Konstruktor aktiviert worden sein.
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

