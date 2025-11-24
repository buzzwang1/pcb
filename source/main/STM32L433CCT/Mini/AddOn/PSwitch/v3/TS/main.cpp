
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz


//  Power Switch v3 LED add on
//
//  PB09  -> Status Led
//
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
//    TIM2  -> PWM für WS2812 for DMA1.Chl3
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

//cSSD1306    mcSSD1306(&mcSys.mcBoard.mcI2C3_Board, 0x78, 56); // == 3C * 2
//cAPDS9960   mcAPDS9960(&mcSys.mcBoard.mcI2C3_Board, nAPDS9960_I2C_ADDR);
//cLm75       mcLm75(&mcSys.mcBoard.mcI2C3_Board, LM75_I2C_ADDRESS_DEFAULT);
//
//
//#define MAIN_nDISPLAY_X cSSD1306_WIDTH
//#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT
//
//uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
//cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
//cScreen_Bpp1_1G          mcScreen1(&mcBm);
//cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
//cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
//cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);

u16    mu16Data[3] = { 0xFFFF, 0xFFFF, 0xFFFF };

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

cEepData mcData(&mcSys.mcBoard.mcEep);

class cUserInterface
{
  public:
    i16  mi16RotCnt;

    cUserInterface()
    {
      mi16RotCnt    = 0;
    }

    i16 i16GetRotCnt()
    {
      return mi16RotCnt;
    }

    bool isHumanPresent()
    {
      return 1;// (mcPir_Deb.u8Get() == 1);
    }
};

cUserInterface mcUI;

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
        mcSys.mcBoard.mcEep.vCmdSetup();
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

cCliCmd_HwStatus2    mcCliCmd_HwStatus2;
cCliCmd_EepTest      mcCliCmd_EepTest;

cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_HwStatus2,  &mcCliCmd_EepTest}, 2);


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

    u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
    switch (lcMsg.u16GetIdx())
    {
      // --------------------------- SLED Messages -----------------------------
      case 40: // Request message
        switch (lcMsg.mcPayload[0])
        {
          case 0: // SLED.Status
            break;
        }
        break;

      case 42: // Set message
        switch (lpu8PayloadRx[0])
        {
          case 1: //  Set 16Bit Values
            // RX 01 | 00 | 00 | 01.D0.D0.D1.D1.D2.D2
            if ((lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
            {
              mu16Data[0] = (lpu8PayloadRx[4] << 8) + lpu8PayloadRx[5];
              mu16Data[1] = (lpu8PayloadRx[6] << 8) + lpu8PayloadRx[7];
              mu16Data[2] = (lpu8PayloadRx[8] << 8) + lpu8PayloadRx[9];
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


//void MAIN_vItoa_HUD21D(int num, char8* str)
//{
//  int32 li32Val1 = 0;
//  int32 li32Val2 = 0;
//  char lszVal1[8] = "";
//  char lszVal2[8] = "x";
//  char lszPkt[] = ",";
//
//  li32Val1 = num / 2;
//  li32Val2 = num - (li32Val1 * 2);
//
//  cStrTools::uixItoa(li32Val1, lszVal1, 10);
//
//  if (li32Val2)
//  {
//    lszVal2[0] = '5';
//  }
//  else
//  {
//    lszVal2[0] = '0';
//  }
//
//  cStrTools::szStrCpy(str, lszVal1);
//  cStrTools::szStrCat(str, lszPkt);
//  cStrTools::szStrCat(str, lszVal2);
//}


i16 mi16RotCntOld = 0;
bool mbEditMode = False;
u8   mu8ProxiCnt = 0;

void MAIN_vTick100msLp()
{
  //char lszValue[16] = "";
  //cStr_Create(lszStrClock, 32);
  //static u8  lu8ProxiLast;
  //static u8  lu8RectsLast;
  //static u16 lu8HoldCnt_ms;
  //
  //mcScreen1.vFill(0);
  ////mcScreen1.vLine(0, 0, MAIN_nDISPLAY_X - 1, MAIN_nDISPLAY_Y - 1, 1);
  //
  //if (mcAPDS9960.isEnabledProximitySensor())
  //{
  //  mcAPDS9960.i8ReadProximitySensor();
  //  if (mcAPDS9960.boGetProximitySensor_Valid())
  //  {
  //    u8 lu8Proxi = mcAPDS9960.ui8GetProximitySensor();
  //    cStrTools::uixItoa(lu8Proxi, lszValue, 10);
  //    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(4, 55, lszValue,       &mcScreen1);
  //
  //    // ca. 25 bis 255
  //    // (255 - 25) / 12 = 19 digit
  //
  //    if (lu8ProxiLast >= 10)
  //    {
  //      if ((lu8Proxi >= (lu8ProxiLast + 10)) || (lu8Proxi <= (lu8ProxiLast - 10)))
  //      {
  //        lu8ProxiLast = lu8Proxi;
  //      }
  //    }
  //    else
  //    {
  //      if (lu8Proxi >= (lu8ProxiLast + 5))
  //      {
  //        lu8ProxiLast = lu8Proxi;
  //      }
  //    }
  //
  //    if ((lu8ProxiLast > 50) && (lu8Proxi > 32))
  //    {
  //      mu8ProxiCnt = lu8ProxiLast;
  //      if (mu8ProxiCnt > 50) mu8ProxiCnt -= 50;
  //      else mu8ProxiCnt = 0;
  //      mu8ProxiCnt /= 17;
  //      mbEditMode = True;
  //    }
  //    else
  //    {
  //      mu8ProxiCnt = (u8)mi16RotCntOld;
  //      mbEditMode = False;
  //    }
  //
  //    if (lu8RectsLast != mu8ProxiCnt)
  //    {
  //      lu8RectsLast = mu8ProxiCnt;
  //      lu8HoldCnt_ms = 0;
  //    }
  //
  //    u8 lu8Rect;
  //
  //    for (lu8Rect = 0; lu8Rect < 12; lu8Rect++)
  //    {
  //      if (lu8Rect < mu8ProxiCnt)
  //      {
  //        cPaint::vRectFull(30 + lu8Rect * 7, 46, 8, 10, 1, &mcScreen1);
  //      }
  //      else
  //      {
  //        cPaint::vRect(30 + lu8Rect * 7, 46, 8, 10, 1, &mcScreen1);
  //      }
  //    }
  //
  //    if (mbEditMode)
  //    {
  //      cPaint::vRect(28, 44, 11 * 8 + 2, 14, 1, &mcScreen1);
  //      if (lu8HoldCnt_ms < 2000)
  //      {
  //        lu8HoldCnt_ms += 100;
  //
  //        if (lu8HoldCnt_ms == 2000)
  //        {
  //          mcUI.mi16RotCnt = mu8ProxiCnt;
  //        }
  //      }
  //
  //      cPaint::vRect(28, 39, 80, 4, 1, &mcScreen1);
  //      cPaint::vRectFull(28, 40, lu8HoldCnt_ms / 100 * 4, 2, 1, &mcScreen1);
  //    }
  //  }
  //}
  //
  //mcSys.mcClock.mClock.toStringDate(lszStrClock);
  //cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(4, 10, (char8*)lszStrClock.ToString(), &mcScreen1);
  //
  //if (mu16Data[0] != 0xFFFF)
  //{
  //  lszStrClock.Setf("%d", mu16Data[0]);
  //  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(128 - 40, 10, (char8*)lszStrClock.ToString(), &mcScreen1);
  //}
  //
  //if (mu16Data[1] != 0xFFFF)
  //{
  //  lszStrClock.Setf("%d", mu16Data[1]);
  //  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(128 - 40, 20, (char8*)lszStrClock.ToString(), &mcScreen1);
  //}
  //
  //
  //{
  //  int32 li32Temp = 0;
  //
  //  char lszValue[16] = "";
  //
  //  mcLm75.vCmdReadTemp();
  //
  //  li32Temp = mcLm75.i16GetTemp();
  //
  //  MAIN_vItoa_HUD21D(li32Temp, lszValue);
  //  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(128-50, 30, (char8*)"T:",  &mcScreen1);
  //  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(128-40, 30, lszValue,      &mcScreen1);
  //}
  //
  //
  //mcSys.mcClock.mClock.toStringTime(lszStrClock);
  //cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(4, 20, (char8*)lszStrClock.ToString(), &mcScreen1);
  //
  //if (mcSys.mcClock.mu32ClockResyncTimeout_s > 0)
  //{
  //  lszStrClock.Setf((rsz)"ReSync: %d", mcSys.mcClock.mu32ClockResyncTimeout_s);
  //  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(4, 30, (char8*)lszStrClock.ToString(), &mcScreen1);
  //}
  //
  //mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  //mcSSD1306.Update();
}


void MAIN_vTick10msLp()
{
  static u16 luCnt100ms = 0;

  i16 li16RotCnt;

  if (mbEditMode)
  {
    li16RotCnt = mu8ProxiCnt;
  }
  else
  {
    li16RotCnt = mcUI.i16GetRotCnt();
  }

  if (mi16RotCntOld != li16RotCnt)
  {
    mi16RotCntOld = li16RotCnt;
  }

  mcData.vTick10ms();

  luCnt100ms++;
  if (luCnt100ms == 10)
  {
    luCnt100ms = 0;
    MAIN_vTick100msLp();
  }
}


void MAIN_vInitSystem(void)
{
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


  // Beim einschalten aus. Wird dann über Msg eingeschaltet
  mi16RotCntOld = mcUI.mi16RotCnt = 0;


  //mcAPDS9960.i8EnableProximitySensor();


  ////CycCall_Start(MAIN_vTick1msHp,
  ////              MAIN_vTick1msLp);

  //mcSys.mcBoard.mcI2C3_Board.vAddNode(&mcSSD1306);
  //mcSys.mcBoard.mcI2C3_Board.vAddNode(&mcAPDS9960);
  //mcSys.mcBoard.mcI2C3_Board.vAddNode(&mcLm75);

  mcSys.vInit2();

  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh1.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh2.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh3.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.mcPowerOut->vEnable();
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

  ////RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  ////GPIOA->MODER &= ~(3 << (2 * 8));
  ////GPIOA->MODER |=  (1 << (2 * 8));
  ////GPIOA->MODER &= ~(3 << (2 * 15));
  ////GPIOA->MODER |=  (1 << (2 * 15));
  ////RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
  ////GPIOB->MODER &= ~(3 << (2 * 8));
  ////GPIOB->MODER |= (1 << (2 * 8));

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
  cSysPkgPMon::vPA01_Init();
  cSysPkgPMon::vPA01_Set1();
  #endif

  SystemInit();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set0();
  #endif
  HAL_Init();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set1();
  #endif
  SystemClock_Config_HSE();
  SystemCoreClockUpdate();

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set0();
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

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set1();
  #endif

  cBuRam::vEnable();
  cErr::vInit();

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
    cSysPkgPMon::vPA01_Set0();
  #endif
}

