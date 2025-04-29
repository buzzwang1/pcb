
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
// WS2812
// PB15: Power MOSFET; 1 = Enable ; 0 = Disable
// PA8:  TIM1_CH1:DMA Channel2: TIM1 ADTM
// PA5:  10k NTC Temp Sensor:     => ADC1 ADC_IN5 PGA 1x
// PA6:  10mOhm Shunt LED Strom:  => ADC2 ADC_IN6 PGA 64x 
//                                   ADC_Ref = 3300mV / 64 ~= 52mV => 52mV / 10mOhm = 5.2A (max)
//                                   ADC_Res = 4096 => ~1,25mA/digit
//                                    100mA =>  125 digit
//                                    400mA =>  503 digit
//                                    500mA =>  629 digit
//                                   1000mA => 1258 digit
//                                   2000mA => 2517 digit  (max. USBC Power with 5k1)
//       50mOhm Shunt LED Strom:  => ADC2 ADC_IN6 PGA 16x 
//                                   ADC_Ref = 3300mV / 16 ~= 206mV => 206mV / 50mOhm = 4.125A (max)
//                                   ADC_Res = 4096 => ~1,01mA/digit
//                                    100mA =>  101 digit
//                                    400mA =>  403 digit
//                                    500mA =>  504 digit
//                                   1000mA => 1007 digit
//                                   2000mA => 2014 digit  (max. USBC Power with 5k1)// 
// 
// I2C: 
// PB10: I2C2_SCL
// PB11: I2C2_SDA
// 
// 
// USB: FS = Full Speed (12MBit/s); HS = High Speed (480MBit/s)
// PB7: PA12_USB_DP
// PB6: PA11_USB_DN
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
//     0: do not use
//     1: do not use
//     2: High Prio
//     3: do not use
//     4: Middle Prio
//     5: do not use
//     6: do not use
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
cI2cEep     mcEep(&mcI2C2_Board, 0xA0);


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

  cLedAnimation(cWs2812* lcWs2812)
  {
    mcWs2812 = lcWs2812;
  }

  virtual void vInit()
  {
  }

  virtual void vProcess10ms()
  {
  }
};



class cLedAnimation_Blink : public cLedAnimation
{
public:
  u8   R, G, B;
  u16  mu16Timer;
  u16  mu8LineIdx;
  cLedAnimation_Blink(cWs2812* lcWs2812, u8 lR, u8 lG, u8 lB)
    : cLedAnimation(lcWs2812)
  {
    R = lR; G = lG; B = lB;
  }

  void vInit() override
  {
    mu16Timer  = 0;
    mu8LineIdx = 0;

    for (u16 lu16i = 0; lu16i < 12; lu16i++)
    {
      mcWs2812->vSetLedPxl(lu16i, R, G, B);
    }
  }

  void vProcess10ms() override
  {
    mu16Timer += 10;
    if (mu16Timer >= 120)
    {
      mu16Timer = 0;
      mu8LineIdx++;

      if (mu8LineIdx & 1)
      {      
        for (u16 lu16i = 0; lu16i < 12; lu16i++)
        {
          mcWs2812->vSetLedPxl(lu16i, R, G, B);
        }
      }
      else
      {
        for (u16 lu16i = 0; lu16i < 12; lu16i++)
        {
          mcWs2812->vSetLedPxl(lu16i, 0, 0, 0);
        }
      }
    }
  }
};


cLedAnimation_Blink     mcLA_BlinkRed(     &mcWs2812, 255, 0,   0);
cLedAnimation_Blink     mcLA_BlinkBlue(    &mcWs2812, 0,   0,   255);
cLedAnimation_Blink     mcLA_BlinkGreen(   &mcWs2812, 0,   255, 0);

cLedAnimation *mLstLedAnims[] = { &mcLA_BlinkRed,
                                  &mcLA_BlinkBlue,
                                  &mcLA_BlinkGreen,
                                  NULL
                                };


u16 mu16LedAnimationIdx = 0;

class cFat12
{
public:

  // 0x08000: Start
  // 0x08C00: FAT
  // 0x08E00: FAT copy
  // 0x09000: Root
  // 0x0D000: Data
  // 0x0D600: First sector of firmware.bin == Sector 5 in the FAT
  //          Therefore firmware has to be linked to 0x0D600


  typedef struct __attribute__((packed))
  {
      u8     JmpCode[3];
      u8     BS_OEMName[8];
      u16    BPB_BytesPerSec;
      u8     BPB_SecPerClus;
      u16    BPB_RsvdSecCnt;
      u8     BPB_NumFATs;
      u16    BPB_RootEntCnt;
      u16    BPB_TotSec16;
      u8     BPB_Media;
      u16    BPB_FATSz16;
      u16    BPB_SecPerTrk;
      u16    BPB_NumHeads;
      u32    BPB_HiddSec;
      u32    BPB_TotSec32;
      u8     BS_DrvNum;
      u8     BS_Reserved1;
      u8     BS_BootSig;
      u32    BS_VolID;
      u8     BS_VolLab[11];
      u8     BS_FileSysType[8];
  }tstHeader;

  typedef struct __attribute__((packed))
  {
      u8     DIR_Name[8];
      u8     DIR_NameExt[3];
      u8     DIR_Attr;
      u8     Reserve[10];
      u16    DIR_WrtTime;
      u16    DIR_WrtDate;
      u16    DIR_FstClus;
      u32    DIR_FileSize;
  }tstFileEntry;

  typedef enum
  {
    FILE_DEL_u8      = 0xE5,
    FILE_ATTR_RO     = 0x01,
    FILE_ATTR_HIDDEN = 0x02,
    FILE_ATTR_SYSTEM = 0x04,
    FILE_ATTR_VOLLAB = 0x08,
    FILE_ATTR_DIR    = 0x10,
    FILE_ATTR_ARCH   = 0x20,

    EOF_CLUSTER_NUM      = 0xFFF,
    NOT_USED_CLUSTER_NUM = 0x000
  }tenDefs;


  bool mbIsValid;

  tstHeader* Header;


  cFat12(u32 lu32StartAdr)
  {
    Header    = (tstHeader*) lu32StartAdr;
    mbIsValid = False;
  }

  u32 u32GetAdrFat()
  {
    u16 lu16ClustIdx;
    lu16ClustIdx = Header->BPB_RsvdSecCnt;
    return (u32)Header + lu16ClustIdx * Header->BPB_BytesPerSec;
  }

  u32 u32GetAdrRoot()
  {
    u16 lu16ClustIdx;
    lu16ClustIdx = (Header->BPB_RsvdSecCnt) + (Header->BPB_NumFATs * Header->BPB_FATSz16);
    return (u32)Header + lu16ClustIdx * Header->BPB_BytesPerSec;
  }


  u32 u32GetAdrData()
  {
    u16 lu16ClustIdx;
    lu16ClustIdx = (Header->BPB_RsvdSecCnt) + (Header->BPB_NumFATs * Header->BPB_FATSz16) + ((Header->BPB_RootEntCnt * 32) / Header->BPB_BytesPerSec) - 2;
    return (u32)Header + lu16ClustIdx * Header->BPB_BytesPerSec;
  }


  bool isValid()
  {
    bool lbValid = False;

    if ((Header->BS_OEMName[0] == 'M') &&
        (Header->BS_OEMName[1] == 'S') &&
        (Header->BS_OEMName[2] == 'D') &&
        (Header->BS_OEMName[3] == 'O') &&
        (Header->BS_OEMName[4] == 'S') &&
        (Header->BS_OEMName[5] == '5') &&
        (Header->BS_OEMName[6] == '.') &&
        (Header->BS_OEMName[7] == '0'))
    {
      if ((Header->BS_FileSysType[0] == 'F') &&
          (Header->BS_FileSysType[1] == 'A') &&
          (Header->BS_FileSysType[2] == 'T') &&
          (Header->BS_FileSysType[3] == '1') &&
          (Header->BS_FileSysType[4] == '2'))
        {
          lbValid = True;
        }
    }

    mbIsValid = lbValid;

    return mbIsValid;
  }

  u16 u16GetClustNext(u16 lu16ClustIdx)
  {
    u8* lu8Fat = (u8*)u32GetAdrFat();
    u32 lu16FatBitIdx     = 12 * lu16ClustIdx;
    u16 lu16FatByteIdx    = lu16FatBitIdx / 8;
    u16 lu16FatByteIdxRst = lu16FatBitIdx % 8;
    u16 lu16FatIdxNext = (lu8Fat[lu16FatByteIdx + 1] << 8) + lu8Fat[lu16FatByteIdx];
    lu16FatIdxNext = lu16FatIdxNext >> lu16FatByteIdxRst;
    lu16FatIdxNext = lu16FatIdxNext & 0xFFF;
    return lu16FatIdxNext;
  }

  void vGetFileName(tstFileEntry* lstFileWork, char* szName)
  {
    cStrTools::szStrnCpy(szName, (rsz)lstFileWork->DIR_Name, 8);
    if (lstFileWork->DIR_NameExt[0] != 0)
    {
      u8 lu8StrLen = cStrTools::uixStrnLen(szName, 8);
      szName[lu8StrLen] = '.';
      lu8StrLen++; szName[lu8StrLen] = lstFileWork->DIR_NameExt[0];
      lu8StrLen++; szName[lu8StrLen] = lstFileWork->DIR_NameExt[1];
      lu8StrLen++; szName[lu8StrLen] = lstFileWork->DIR_NameExt[2];
    }
  }

  u32 u32GetFileAdr(rsz lszFilename83)
  {
    // Check for file
    // Check, if the file's cluster are in sequence. No "holes" or "jumps" in the FAT

    tstFileEntry* lstFileWork;
    char szName1[12];
    
    cStrTools::szStrnCpy(szName1, (rsz)lszFilename83, 12);
    cStrTools::szStrnLwr(szName1, 12);

    lstFileWork = (tstFileEntry*)u32GetAdrRoot();
    while (lstFileWork->DIR_Name[0])
    {
      char szName2[12];

      vGetFileName(lstFileWork, szName2);
      cStrTools::szStrnLwr(szName2, 12);

      if (cStrTools::i8StrnCmp(szName1, szName2, 12) == 0)
      {
        // Check, if the file's cluster are in sequence.

        u32 lu32Clust = lstFileWork->DIR_FstClus;
        u32 lu32ClustOld = lu32Clust;

        while (lu32Clust != EOF_CLUSTER_NUM)
        {
          lu32Clust = u16GetClustNext(lu32Clust);
          if (lu32Clust != EOF_CLUSTER_NUM)
          {
            if ((lu32ClustOld + 1) != lu32Clust)
            {
              return 0;
            }
            lu32ClustOld = lu32Clust;
          }
        }
        // file's cluster are in sequence.
        // File start Adress
        return u32GetAdrData() + lstFileWork->DIR_FstClus * Header->BPB_BytesPerSec;
      }

      lstFileWork = (tstFileEntry*)((u32)lstFileWork + 32u);
    }
    
    return 0;
  }
};

cFat12 mcFat12(0x8000);


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void DMA1_Channel2_IRQHandler(void)
{
  mcWs2812.Dma_vIrqTc();
}


void MAIN_vTick1msLp()
{
  mcUI.vTick1msLp();
}

void MAIN_vTick10msLp()
{
  mcWs2812.vTick10msLp();

  mcWs2812.vEnable();

  u8 lu8Brightness = 64; 

  mcWs2812.vSetBrigthness(lu8Brightness);

  mLstLedAnims[mu16LedAnimationIdx]->vProcess10ms();
  mcWs2812.vShow();
}


void MAIN_vTick100msLp(void)
{
}


void MAIN_vTick1000msLp(void)
{
  mcTestLed.Toggle();
}


void MAIN_vGotoApp(void* p)
{
  register uint32_t app_base = (u32)p;
  asm volatile ("jalr ra, %0" : : "r" (app_base));
}

void MAIN_vInitSystem(void)
{
  SystemCoreClock = cClockInfo::mstClocks.HCLK_Frequency;
  mcWs2812.vDisable();

  // Check if keypressed
  //  -- keypressed => stay in BL
  //  -- call App

  // Check if Valid Filesystem and firmware is plausible
  // if not stay in BL

  if (mcFat12.isValid())
  {
    u32 lu32FileAdr = mcFat12.u32GetFileAdr((rsz)"firmware.bin");
    if (lu32FileAdr > (u32)mcFat12.Header)
    {
      // Check if not keypressed
      if (mcUI.mcRotDsD.ui8Get() == 1)
      {
        MAIN_vGotoApp((void*)lu32FileAdr);
      }
      mu16LedAnimationIdx = 2;
    }
  }

  /* Enable Udisk */
  Udisk_Capability = IFLASH_UDISK_SIZE / DEF_UDISK_SECTOR_SIZE;
  Udisk_Status    |= DEF_UDISK_EN_FLAG;

  /* USB20 device init */
  USBHS_RCC_Init();
  USBHS_Device_Init(ENABLE);

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

