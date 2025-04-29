#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;


//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
//
//  PB09  -> Status Led
//
//  PA00   -> Wakeup (100k Pull Down)
//  PA08   -> reserve
//  PB01   -> reserve
//  PB12   -> reserve
//
//  USART1 -> BotCom ComPort (19200)
//  ---
//  ---
//
//  Power Control
//    Battery Control:
//      Balancer:
//      PB00  -> U_Bat4
//      PA06  -> U_Bat3
//      PB02  -> U_Bat2
//      PA15  -> U_Bat1
//
//    Regulator In:
//      PA04  -> Dac
//      PA07  -> En
//
//    Regulator Out:
//      PA05  -> Dac
//      PB08  -> En
//      PA06  -> U_Sys: ADC_IN11
//
//   Switch:
//      PC13: Battery MainSwitch
//      PA10: LowPower(3V3, ADC, I2C)
//
//
//  I2C1  -> BotNet 1.0.0.0
//    PB06  -> I2C1 SCL
//    PB07  -> I2C1 SDA
//
//  U1    -> BotNet 1.E.0.0
//    PA09  -> U1 Tx/Rx
//
//  U2    -> Debug
//    PA02  -> U2 Tx
//    PA03  -> U2 Rx
//
//  I2C2  -> INA3221 2x, Display (Optional) , Apds9960 (Optional)
//    PB10  -> I2C2 SCL
//    PB11  -> I2C2 SDA
//
//
//        nRF905:            Display:
//  PB03: SPI1 Clock         SPI1 Clock
//  PB04: SPI1 MISO          SPI1 MISO
//  PB05: SPI1 MOSI          SPI1 MOSI
//  PA12: SPI1 Chip Select   SPI1 Display Chip Select
//  PB14: TX or RX mode      SPI1 Touch Chip Select
//  PA11: Standby            Dipslay Reset
//  PB15: Power up           Display DC
//  PB13: Data Ready         Touch IRQ


LED<GPIOB_BASE, 9> mcLedRed;
LED<GPIOA_BASE, 8> mcPA08;
LED<GPIOB_BASE, 1> mcPB01;
LED<GPIOB_BASE, 12> mcPB12;

cGpPin mLowPower(GPIOA_BASE, 10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin mMod2_En(GPIOB_BASE,  8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin mLed_En(GPIOA_BASE,  6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);

cClockInfo mcClkInfo;

#define DISPLAY_X 320
#define DISPLAY_Y 240

uint8 mDisplayMemory2[DISPLAY_X * DISPLAY_Y * 2]  __attribute__((section(".ExtMem")))  __attribute__((aligned(4)));
uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2]  __attribute__((section(".ExtMem")))  __attribute__((aligned(4)));


cDiffTimer                               mcDiffTimer;
cMsgBox                                  mcMsgBox(20);
cCursor                                  mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y, mDisplayMemory);
cBitmap_Bpp16_5R6G5B                     mc16GBm2(DISPLAY_X, DISPLAY_Y, mDisplayMemory2);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);

cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

stPxli16                            mstChnXYBPts[10];
ciGraphXY_Channel                   mcChnXYB(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnXYBPts, 10, null);

stPxli16                            mstChnXYAPts[10];
ciGraphXY_Channel                   mcChnXYA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnXYAPts, 10, &mcChnXYB);

i16                                 mstChnYAPts[100];
ciGraphY_Channel                    mcChnYA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYAPts, 100, null);

cUiElement_Root                     mc16GuiRoot(10, 10, DISPLAY_X-20, DISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));
cUiElement                          mc16GuiBase01(&mcMsgBox, &mc16GScreen1);
cUiElement_TextBox                  mc16GuiText01(&mcMsgBox, &mc16GScreen1, 128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement_TextBox                  mc16GuiText_Perf(&mcMsgBox, &mc16GScreen1, 128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement                          mc16GuiBase02(&mcMsgBox, &mc16GScreen1);


cUiElement_Box3D_Cube               mcCube;
cUiElement_Box3D_Torus              mcTorus;
cUiElement_Box3D_Sphere             mcSphere;
cUiElement_Box3D_Monkey             mcMonkey;

cUiElement_Box3D                    mc16GuiBox3D_Cube(&mcMsgBox, &mc16GScreen1, &mcCube);
cUiElement_Box3D                    mc16GuiBox3D_Torus(&mcMsgBox, &mc16GScreen1, &mcTorus);
cUiElement_Box3D                    mc16GuiBox3D_Sphere(&mcMsgBox, &mc16GScreen1, &mcSphere);
cUiElement_Box3D                    mc16GuiBox3D_Monkey(&mcMsgBox, &mc16GScreen1, &mcMonkey);

cUiElement_Window                   mc16GuiWindow01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphXYBox              mc16GuiGraphXY(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnXYA);

cUiElement_Window                   mc16GuiWindow02(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphYBox               mc16GuiGraphY(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnYA);


class cMyButton: public cUiElement_Button
{
  public:

    cMyButton(cMsgBox *lcMsgBox, cScreen *lpcScreen, uint32 lui32MaxCharacterCount, cRFont* lpcRFont)
      : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont)
    {
      this->mu32BaseCol = 0x101080;
    }

    virtual void OnPressEnd() override
    {
      switch (mc16GuiText01.menTextFitMode)
      {
        case cUiElement_TextBox::nFit:         mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitSmart;break;
        case cUiElement_TextBox::nFitSmart:    mc16GuiText01.menTextFitMode = cUiElement_TextBox::nNoFit;break;
        case cUiElement_TextBox::nNoFit:       mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitRev;break;
        case cUiElement_TextBox::nFitRev:      mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitSmartRev;break;
        case cUiElement_TextBox::nFitSmartRev: mc16GuiText01.menTextFitMode = cUiElement_TextBox::nNoFitRev;break;
        case cUiElement_TextBox::nNoFitRev:    mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFit;break;
      }


      if (mc16GuiBox3D_Cube.mboWireFrame)
      {
        mc16GuiBox3D_Cube.mboWireFrame   = False;
        mc16GuiBox3D_Monkey.mboWireFrame = False;
        mc16GuiBox3D_Sphere.mboWireFrame = False;
        mc16GuiBox3D_Torus.mboWireFrame  = False;
      }
      else
      {
        mc16GuiBox3D_Cube.mboWireFrame   = True;
        mc16GuiBox3D_Monkey.mboWireFrame = True;
        mc16GuiBox3D_Sphere.mboWireFrame = True;
        mc16GuiBox3D_Torus.mboWireFrame  = True;
      }

      mc16GuiRoot.vRepaint();
    }
};

cMyButton                   mc16GuiBtn01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);


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

#if 0
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
#endif

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}



/*
static void Error_Handler(void)
{
  while (1)
  {
  }
}*/


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


#include "math.h"

class cXpt2046_SharedSpi
{
public:
  cGpPin mTpPinCs;
  cGpPin mTpPinIrq;

  SPI_TypeDef* mSpi;

  u16 xa[5];
  u16 ya[5];
  u16 z1a[5];
  u16 z2a[5];

  u16 x_raw;
  u16 y_raw;
  u16 z1_raw;
  u16 z2_raw;

  u16 x_raw_min;
  u16 x_raw_max;
  u16 y_raw_min;
  u16 y_raw_max;


  u16 x;
  u16 y;

  //float pressure;

  u8  u8TouchState;

  u16 u16DelayCS;
  u16 u16DelayBusy;

  u8 CMD_RDZ1;
  u8 CMD_RDZ2;
  u8 CMD_RDX;
  u8 CMD_RDY;

  cXpt2046_SharedSpi(SPI_TypeDef* lSpi)
    : mTpPinCs(GPIOB_BASE,  14, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1),
      mTpPinIrq(GPIOB_BASE, 13, GPIO_MODE_INPUT,     GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0)
  {
    mSpi = lSpi;

    CMD_RDX  = 0X90; // 1001 0000 // Y (240x320), X (320x240)
    CMD_RDZ1 = 0XB0; // 1011 0000 // Z1
    CMD_RDZ2 = 0XC0; // 1100 0000 // Z2
    CMD_RDY  = 0XD0; // 1101 0000 // X (240x320), Y (320x240)
                     // +--- ---- : Start, immer 1
                     // -+++ ---- : 3 Bit Channel select
                     //  001: YP Driver
                     //  011: Z1 Driver
                     //  100: Z2 Driver
                     //  101: XP Driver
                     // ---- +--- : Mode: 0=12Bit; 1 = 8Bit
                     // ---- -+-- : Ser/Dfr: 0=Differntial Modet; 1 = Single Ended Mode
                     // ---- --++ : PowerDown: 0 = Power-Down Between Conversions

    x_raw_min = 150;
    x_raw_max = 1900;
    y_raw_min = 120;
    y_raw_max = 1900;

    u16DelayCS   = 4;
    u16DelayBusy = 10;
  }

  void vInitSpi(void)
  {
    LL_SPI_Disable(mSpi);

    LL_SPI_InitTypeDef SPI_InitStruct;

    // SPI 1
    // SPI2 parameter configuration
    // XPT2046 max. Baudrate = 2Mhz => 125 Samples/s * 16bit
    // DS: 200ns (t_CH = DCLK High) + 200ns (t_CL = DCLK Low) = 400ns (2,5Mhz)
    // t_ACQ (Acquisition Time) = 3xCLK = 1500ms => 1x CLK = 500us => 2Mhz

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128; // 160Mhz/128 = 1,4Mhz
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 0x7;
    LL_SPI_Init(mSpi, &SPI_InitStruct);
    LL_SPI_SetStandard(mSpi, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(mSpi);

    LL_SPI_SetTransferSize(mSpi, 3);


    LL_SPI_Enable(mSpi);
  }

  void vDelay(volatile u32 lu32Num)
  {
    lu32Num *= 8;
    while (lu32Num)
    {
      lu32Num--;
    }
  }

  u16 u16ReadTouch(u8 lu8Cmd)
  {
    u16 Num = 0;

    SET_BIT(mSpi->CR1, SPI_CR1_CSTART);
    mSpi->IFCR = 0xFFFF; // SPI interrupt / status flags clear register
    // TSIZE wurde vorher auf 3 gestellt
    // 3 Byte in den TxFifo
    LL_SPI_TransmitData8(mSpi, lu8Cmd);
    LL_SPI_TransmitData8(mSpi, 0);
    LL_SPI_TransmitData8(mSpi, 0);
    // Warten die 3 Byte übertragen wurden
    while (!LL_SPI_IsActiveFlag_EOT(mSpi)); // End of Transmission
    // Dann 3 Byte aus dem RxFifi
    LL_SPI_ReceiveData8(mSpi);
    Num = LL_SPI_ReceiveData8(mSpi) << 8;
    Num += LL_SPI_ReceiveData8(mSpi);
    return(Num >> 4);
  }

  u16 u16Read(u8 lu8Cmd, u16* lu16Dest)
  {
    u16 lu16i, lu16j;
    u16 lu16Val;
    u32 lu32Sum;

    lu32Sum = 0;
    // 5 Punkte aufzeichnen
    for (lu16i = 0; lu16i < 5; lu16i++)
    {
      lu16Val = this->u16ReadTouch(lu8Cmd);
      lu16Dest[lu16i] = lu16Val;
    }

    // Der Größe nach sortieren
    for (lu16i = 0; lu16i < 5 - 1; lu16i++)
    {
      for (lu16j = lu16i + 1; lu16j < 5; lu16j++)
      {
        if (lu16Dest[lu16i] > lu16Dest[lu16j])
        {
          lu16Val = lu16Dest[lu16i];
          lu16Dest[lu16i] = lu16Dest[lu16j];
          lu16Dest[lu16j] = lu16Val;
        }
      }
    }

    // Mittelwert vom Punkte (ohne größter und kleinster Wert)
    for (lu16i = 1; lu16i < 4; lu16i++)
    {
      lu32Sum += lu16Dest[lu16i];
    }
    lu32Sum /= 3;

    // Überprüfen, ob alle Werte +/-32 Bereich liegen, sonst verwerfen
    for (lu16i = 0; lu16i < 5; lu16i++)
    {
      if (lu16Dest[lu16i] < lu32Sum)
        if ((lu32Sum - lu16Dest[lu16i]) > 32) return 0;
      if (lu16Dest[lu16i] > lu32Sum)
        if ((lu16Dest[lu16i] - lu32Sum) > 32) return 0;
    }

    // Überprüfen, ob alle gültig, sonst verwerfen
    for (lu16i = 0; lu16i < 5; lu16i++)
    {
      if ((lu16Dest[lu16i] < 32) || (lu16Dest[lu16i] > 2047 - 32)) return 0;
    }

    return (u16)lu32Sum;
  }

  bool isTouch() {return (mTpPinIrq.ui8Get() == 0);}

  u8 u8Scan()
  {
    if (isTouch()) // Touch betätigt
    {
      mTpPinCs.vSet0();
      this->vDelay(u16DelayCS);
      this->x_raw = this->u16Read(this->CMD_RDX, this->xa);
      this->y_raw = this->u16Read(this->CMD_RDY, this->ya);
      //this->z1_raw = this->u16Read(this->CMD_RDZ1, this->z1a);
      //this->z2_raw = this->u16Read(this->CMD_RDZ2, this->z2a);
      this->vDelay(u16DelayCS);
      mTpPinCs.vSet1();

      if ((this->x_raw != 0) && (this->y_raw != 0))
      {
        if (x_raw < x_raw_min) x_raw_min = x_raw;
        if (x_raw > x_raw_max) x_raw_max = x_raw;
        if (y_raw < y_raw_min) y_raw_min = y_raw;
        if (y_raw > y_raw_max) y_raw_max = y_raw;

        u32 lu32Zwerg;
        lu32Zwerg  = 320.0f * (this->x_raw_max - this->x_raw);
        lu32Zwerg /= (this->x_raw_max - this->x_raw_min);
        this->x = (u16)lu32Zwerg;

        lu32Zwerg  = 240.0f * (this->y_raw_max - this->y_raw);
        lu32Zwerg /= (this->y_raw_max - this->y_raw_min);
        this->y = (u16)lu32Zwerg;

        //pressure = 100.0f * this->x_raw / 4096.0f * ((float)this->z2_raw / (float)this->z1_raw - 1);

        mcCursor.vTick10ms(this->x, this->y, 1);
        u8TouchState = 1;
      }
    }
    else
    {
      if (u8TouchState > 0)
      {
        mcCursor.vTick10ms(this->x, this->y, 0);
        u8TouchState = 0;
      }      
    }
    return u8TouchState;
  }

  u8 u8ResetTouch()
  {
    if (!isTouch()) // Touch nicht betätigt
    {
      if (u8TouchState > 0)
      {
        mcCursor.vTick10ms(this->x, this->y, 0);
        u8TouchState = 0;
      }
    }
    return u8TouchState;
  }
};

cXpt2046_SharedSpi mcXpt2046(SPI1);
cILI9341 mcILI9341;


bool mbDmaBreak = False;

void GPDMA1_Channel6_IRQHandler(void)
{
  //mcPA08.vSet1();
  mbDmaBreak = mcILI9341.bDmaTcHandler();
  //mcPA08.vSet0();
}

void vDoTouch()
{
  if (mbDmaBreak)
  {
    mbDmaBreak = False;
    if (mcXpt2046.isTouch())
    {
      mcXpt2046.vInitSpi();
      mcXpt2046.u8Scan();
      mcILI9341.vInitSpi();
    }
    else
    {
      mcXpt2046.u8ResetTouch();
    }
  }
}


u8 mu8Repaint_Cnt = 0;
u8 mu8Repaint     = 0;
u32 mu32SystemTime = 0;

void MAIN_vTick10msHp(void)
{
  mu32SystemTime += 10;
  if (mu8Repaint_Cnt == 0)
  {
    mu8Repaint = 1;
    mu8Repaint_Cnt = 3;
  }
  else
  {
    mu8Repaint_Cnt--;
  }
}


void MAIN_vTick10msLp(void)
{
  static i16 li16LValue = 0;
  static i16 li16Value = 0;

  if (mu8Repaint)
  {
    mu8Repaint = 0;
    mc16GBm.vCopy(mc16GBm2);
    mcILI9341.vShow(&mc16GBm2);
    //if (mcILI9488.isReady())
    {
      cMsg lcMsg;

      mcDiffTimer.start();
      lcMsg.mID         =  cMsg::tenMsgIDs::nTimestamp;
      lcMsg.mSubID      =  0;
      lcMsg.mui8Data[7] = 25;
      mcMsgBox.vput(&lcMsg);

      while (!mcMsgBox.isEmpty())
      {
        mcMsgBox.vget(&lcMsg);
        mc16GuiRoot.vMsgHdl(&lcMsg);
      }

      li16Value = (li16LValue + (rand() % 100)) / 2;
      li16LValue = li16Value;
      mcChnYA.vAdd(li16Value);
      mc16GuiGraphY.vRepaint();

      mc16GuiRoot.vPaint();

      mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);

      mc16GuiText_Perf.vSetText(mcDiffTimer.toString());
    }
  }
}


void MAIN_vTick1000msLp(void)
{
  static uint8 lui8t = 0;
  mcLedRed.Toggle();
  lui8t++;
}


#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{

  #ifdef _WIN32
    mpcDisplay1 = new cWin32Display(0, 0,                        DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  #else
    mcClkInfo.Update();
  #endif

  mcChnXYA.vAddXY(10, 10);
  mcChnXYA.vAddXY(20, 20);
  mcChnXYA.vAddXY(30, 10);
  mcChnXYA.vAddXY(40, 10);
  mcChnXYA.vAddXY(50, 10);
  mcChnXYA.vAddXY(60, 20);
  mcChnXYA.vAddXY(70, 20);
  mcChnXYA.vAddXY(80, 10);
  mcChnXYA.mui32Col = 0xFF0000;

  mcChnXYB.vAddXY(10, 30);
  mcChnXYB.vAddXY(20, 20);
  mcChnXYB.vAddXY(30, 30);
  mcChnXYB.vAddXY(40, 40);
  mcChnXYB.vAddXY(50, -50);
  mcChnXYB.vAddXY(60, 60);
  mcChnXYB.vAddXY(70, 90);
  mcChnXYB.vAddXY(80, 80);
  mcChnXYB.mui32Col = 0x00FF00;

  mc16GuiRoot.bAdd(10, 10, 50, 50, &mc16GuiBase01);
  mc16GuiBase01.bAdd(10, 10, 20, 20, &mc16GuiText01);
  mc16GuiBase01.bAdd(0, 0, 50, 10, &mc16GuiText_Perf);


  mc16GuiRoot.bAdd(70, 70, 50, 50, &mc16GuiBase02);
  mc16GuiBase02.bAdd(10, 10, 30, 10, &mc16GuiBtn01);

  mc16GuiText01.vSetText("Text acbfgr-tdfgd fdgdee 2134");
  mc16GuiText_Perf.vSetText("----");

  mc16GuiRoot.bAdd(20, 80, 150, 100, &mc16GuiWindow01);
  mc16GuiWindow01.bAdd(20, 60, 150, 100, &mc16GuiGraphXY);
  mc16GuiGraphXY.vToggleMaximize();

  mc16GuiRoot.bAdd(180, 80, 100, 100, &mc16GuiWindow02);
  mc16GuiWindow02.bAdd(20, 20, 150, 100, &mc16GuiGraphY);
  mc16GuiGraphY.vToggleMaximize();

  mc16GuiRoot.bAdd(20, 15, 60, 60, &mc16GuiBox3D_Cube);
  mc16GuiRoot.bAdd( 70, 15, 60, 60, &mc16GuiBox3D_Torus);
  mc16GuiRoot.bAdd(120, 15, 60, 60, &mc16GuiBox3D_Sphere);
  mc16GuiRoot.bAdd(170, 15, 60, 60, &mc16GuiBox3D_Monkey);

  mc16GuiBox3D_Cube.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Torus.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Sphere.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Monkey.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);

  #ifndef _WIN32

    CycCall_Start(NULL /*1ms_HP*/,
                  MAIN_vTick10msHp /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
  #endif
}

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    vDoTouch();
    CycCall_vIdle();
    __asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

/**
 * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 160000000
  *            HCLK(Hz)                       = 160000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_MBOOST                     = 1
  *            PLL_M                          = 1
  *            PLL_N                          = 80
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 4
  */
static void SystemClock_Config(void)
{
  /* Enable PWR clock */
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

  /* Set the regulator supply output voltage */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }

  /* Switch to SMPS regulator instead of LDO */
  //LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  //while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  //{
  //}

  /* Enable MSI oscillator */
  LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4);
  LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
  LL_RCC_MSIS_Enable();
  while (LL_RCC_MSIS_IsReady() != 1)
  {
  }

  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  /* Configure PLL clock source */
  LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

  /* Enable the EPOD to reach max frequency */
  LL_PWR_EnableEPODBooster();
  while (LL_PWR_IsActiveFlag_BOOST() == 0)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL1_EnableDomain_SYS();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
  LL_RCC_PLL1_SetDivider(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);

  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1)
  {
  }

  /* Set AHB, APB1, APB2 and APB3 prescalers */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  /* Set PLL1 as System Clock Source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  /* Set systick to 1ms with frequency set to 160MHz */
  LL_Init1msTick(160000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(160000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  //SRAM3 füllen
  //0x20040000

  u8* lu8Sram3 = (u8*)0x20040000;
  u32 lu32t;

  for (lu32t = 0; lu32t < (510 * 1024); lu32t++)
  {
    *lu8Sram3++ = (u8)lu32t;
  }


  SystemInit();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}





