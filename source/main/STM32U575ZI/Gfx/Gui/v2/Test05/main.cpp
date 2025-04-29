#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;


LED<GPIOB_BASE, 7> mcLedRed;
cClockInfo mcClkInfo;

cILI9341 mcILI9341;

#define DISPLAY_X 320
#define DISPLAY_Y 240

uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2];
uint8 mDisplayMemory2[DISPLAY_X * DISPLAY_Y * 2];

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



void GPDMA1_Channel6_IRQHandler(void)
{
  mcILI9341.vDmaTcHandler();
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

#define OTT_MAX_TOUCH  5
#define TP_PRES_DOWN 0x80
#define TP_CATH_PRES 0x40


#define TP_SPI 1

cGpPin mTpSCK(GPIOD_BASE,    1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mTpMISO(GPIOD_BASE,   3, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mTpMOSI(GPIOD_BASE,   4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mTpPinCs(GPIOD_BASE,  5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);
cGpPin mTpPinIrq(GPIOD_BASE, 6, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


typedef struct
{
  u16 x[OTT_MAX_TOUCH];
  u16 y[OTT_MAX_TOUCH];

  u8  sta;

  float xfac;
  float yfac;
  short xoff;
  short yoff;

  u8 touchtype;
}_m_tp_dev;

_m_tp_dev tp_dev =
{
  {0,0,0,0,0},
  {0,0,0,0,0},

  0,

  -0.0891*2,
  -0.0681*2,
  340,
  260,

  0,
};

u8 CMD_RDX = 0X90; // 1001 0000 // Y Driver
u8 CMD_RDY = 0XD0; // 1101 0000 // X Driver
                   // +--- ---- : Start, immer 1
                   // -+++ ---- : 3 Bit Channel select
                   //  001: YP
                   //  011: Z1
                   //  100: Z2
                   //  101: XP
                   // ---- +--- : Mode: 0=12Bit; 1 = 8Bit
                   // ---- -+-- : Ser/Dfr: 0=Differntial Modet; 1 = Single Ended Mode
                   // ---- --++ : PowerDown: 0 = Power-Down Between Conversions

void TP_Delay(volatile u32 lu32Num)
{
  lu32Num *= 8;

  while (lu32Num)
  {
    lu32Num--;
  }
}

u8 TP_Write_Byte(u8 num)
{
  #if (TP_SPI == 1)
    // Master transfer start
    volatile u8 lu8Rx;
    SET_BIT(SPI2->CR1, SPI_CR1_CSTART);
    SPI2->IFCR = 0xFFFF;

    while (!LL_SPI_IsActiveFlag_TXP(SPI2)); // Tx-Packet space available
    LL_SPI_TransmitData8(SPI2, num);
    while (!LL_SPI_IsActiveFlag_EOT(SPI2)); // End of Transmission
    lu8Rx = LL_SPI_ReceiveData8(SPI2);
    return lu8Rx;
  #else
  u8 count = 0;
  for (count = 0; count < 8; count++)
  {
    if (num & 0x80)  mTpMOSI.vSet1();
    else mTpMOSI.vSet0();
    num <<= 1;
    mTpSCK.vSet0();
    TP_Delay(1);
    mTpSCK.vSet1();
    return 0;
  }
  #endif
}


u16 TP_Read_AD(u8 CMD)
{
  #if (TP_SPI == 1)
    u16 Num = 0;
    mTpPinCs.vSet0();
    TP_Write_Byte(CMD);
    TP_Delay(4);
    Num += (TP_Write_Byte(0) << 8);
    Num += TP_Write_Byte(0);
    Num >>= 4;
    TP_Delay(4);
    mTpPinCs.vSet1();
    return(Num);
  #else
    u8 count = 0;
    u16 Num = 0;
    mTpSCK.vSet0();
    mTpMOSI.vSet0();
    mTpPinCs.vSet0();
    TP_Write_Byte(CMD);
    TP_Delay(6);
    mTpSCK.vSet0();
    TP_Delay(1);
    mTpSCK.vSet1();
    TP_Delay(1);
    mTpSCK.vSet0();
    for (count = 0; count < 16; count++)
    {
      Num <<= 1;
      mTpSCK.vSet0();
      TP_Delay(1);
      mTpSCK.vSet1();
      if (mTpMISO.ui8Get())
      {
        Num++;
      }
    }
    Num >>= 4;
    mTpPinCs.vSet1();
    return(Num);
  #endif
}

#define READ_TIMES 5
#define LOST_VAL 1
u16 TP_Read_XOY(u8 xy)
{
  u16 i, j;
  u16 buf[READ_TIMES];
  u16 sum = 0;
  u16 temp;
  for (i = 0; i < READ_TIMES; i++)buf[i] = TP_Read_AD(xy);
  for (i = 0; i < READ_TIMES - 1; i++)
  {
    for (j = i + 1; j < READ_TIMES; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  sum = 0;
  for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++) sum += buf[i];
  temp = sum / (READ_TIMES - 2 * LOST_VAL);
  return temp;
}


u8 TP_Read_XY(u16 *x, u16 *y)
{
  u16 xtemp, ytemp;
  xtemp = TP_Read_XOY(CMD_RDX);
  ytemp = TP_Read_XOY(CMD_RDY);
  //if(xtemp<100||ytemp<100)return 0;
  *x = xtemp;
  *y = ytemp;
  return 1;
}

#define ERR_RANGE 50
u8 TP_Read_XY2(u16 *x, u16 *y)
{
  u16 x1, y1;
  u16 x2, y2;
  u8 flag;
  flag = TP_Read_XY(&x1, &y1);
  if (flag == 0)return(0);
  flag = TP_Read_XY(&x2, &y2);
  if (flag == 0)return(0);
  if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))
    && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
  {
    *x = (x1 + x2) / 2;
    *y = (y1 + y2) / 2;
    return 1;
  }
  else return 0;
}


u8 TP_Scan(u8 tp)
{
  if (mTpPinIrq.ui8Get() == 0)
  {
    if (tp)TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);
    else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))
    {
      tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff;
      tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
    }
    if ((tp_dev.sta & TP_PRES_DOWN) == 0)
    {
      tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;
      tp_dev.x[4] = tp_dev.x[0];
      tp_dev.y[4] = tp_dev.y[0];
    }

    mcCursor.vTick10ms(tp_dev.x[0], tp_dev.y[0], 1);
  }
  else
  {
    if (tp_dev.sta & TP_PRES_DOWN)
    {
      tp_dev.sta &= ~(1 << 7);
      mcCursor.vTick10ms(tp_dev.x[0], tp_dev.y[0], 0);
    }
    else
    {
      tp_dev.x[4] = 0;
      tp_dev.y[4] = 0;
      tp_dev.x[0] = 0xffff;
      tp_dev.y[0] = 0xffff;
    }
  }
  return tp_dev.sta & TP_PRES_DOWN;
}





u8 TP_Init(void)
{
  #if (TP_SPI == 1)
    LL_SPI_InitTypeDef SPI_InitStruct;
    LL_RCC_SetSPIClockSource(LL_RCC_SPI2_CLKSOURCE_SYSCLK);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);


    mTpSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // SCK
    mTpMISO.vSetAF(GPIO_MODE_AF_OD, GPIO_AF5_SPI2);  // MISO
    mTpMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // MOSI

    // USER CODE END SPI2_Init 1
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
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256; // 160Mhz/256Mhz = 625kHz
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 0x7;
    LL_SPI_Init(SPI2, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(SPI2);

    LL_SPI_SetTransferSize(SPI2, 1);

    LL_SPI_Enable(SPI2);
  #endif

  TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]);

  return 1;
}


u8 mu8Repaint_Cnt = 0;
u8 mu8Repaint     = 0;

void MAIN_vTick10msHp(void)
{
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
  TP_Scan(0);

  //mcI2C1->bStartNext();

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

      li16Value  = (li16LValue + (rand() % 100)) / 2;
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

    //ILI9341_Init();
    TP_Init();
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

  mc16GuiRoot.bAdd( 20, 15, 60, 60, &mc16GuiBox3D_Cube);
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
  LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  {
  }

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
  SystemInit();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}





