
#include "main.h"

__IO uint32_t TimingDelay = 0;


LED<GPIOF_BASE, 9>   mcLedRed;
cClockInfo mcClkInfo;

cGpPin lcSCL(GPIOB_BASE, 8, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 9, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 0);

LED<GPIOB_BASE, 4> lcLedI2cDmaTxTc;
LED<GPIOB_BASE, 5> lcLedI2cDmaRxTc;
LED<GPIOB_BASE, 6> lcLedI2cInt;
LED<GPIOB_BASE, 7> lcLedI2cErr;

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cLSM303DLHC_MAG mcMag(&mcI2C1);
cLSM303DLHC_ACC mcAcc(&mcI2C1);

#define DISPLAY_X 320
#define DISPLAY_Y 240

uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2] __attribute__((section(".ExtMem")));

cDiffTimer                              mcDiffTimer;
cMsgBox                                 mcMsgBox(10);
cCursor                                 mcCursor(200, 800, &mcMsgBox);


cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

stPxli16                            mstChnBPts[10];
ciGraphXY_Channel                     mcChnB(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnBPts, 10, null);

stPxli16                            mstChnAPts[10];
ciGraphXY_Channel                     mcChnA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnAPts, 10, &mcChnB);

cUiEleTree                          mcUi16GTree(25);

cUiElement_Box3D_Cube               mcCube;
cUiElement_Box3D_Torus              mcTorus;

cUiElement_Window                   mc16GUi1(30, 30, 260, 180, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_TextBox                  mc16GUi2(20, 15,  40,  40, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement_Box3D                    mc16GUi2b1(10, 10, 50, 50, &mcMsgBox, &mcCube);
cUiElement_Box3D                    mc16GUi2b2(10, 70, 50, 50, &mcMsgBox, &mcTorus);


cUiElement_Window                   mc16GUi3( 60, 40, 150, 100, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphXYBox                mc16GUi3a(20, 15, 250, 200, &mcMsgBox, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnA);

cUiElement_Window                   mc16GUi4( 60, 40, 60, 50, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement                          mc16GUi4a( 0, 10, 10, 10, &mcMsgBox);
cUiElement_Button                   mc16GUi4b(30, 10, 30, 10, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);



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


void DMA1_Stream5_IRQHandler(void)
{
  // I2C1 Rx Transmission Complete
  lcLedI2cDmaRxTc.vSet1();
  DMA_Cmd(DMA1_Stream5, DISABLE);
  DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
  mcI2C1.I2C_EV_DMAHandler(cComNode::tenEvent::enEvDmaRxTc);
  lcLedI2cDmaRxTc.vSet0();
}


void I2C1_EV_IRQHandler(void)
{
  lcLedI2cInt.vSet1();
  mcI2C1.I2C_EV_IRQHandler();
  lcLedI2cInt.vSet0();
}

void I2C1_ER_IRQHandler(void)
{
  lcLedI2cErr.vSet1();
  mcI2C1.I2C_ER_IRQHandler();
  lcLedI2cErr.vSet0();
}


void MAIN_vTick10msHp(void)
{
  mcI2C1.bStartNext();
}


void MAIN_vTick100msLp(void)
{
  mcMag.vReadMag();
  mcAcc.vReadAcc();
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


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

#define OTT_MAX_TOUCH  5
#define TP_PRES_DOWN 0x80
#define TP_CATH_PRES 0x40

typedef struct
{
  u8(*init)(void);
  u8(*scan)(u8);
  void(*adjust)(void);
  u16 x[OTT_MAX_TOUCH];
  u16 y[OTT_MAX_TOUCH];

  u8  sta;

  float xfac;
  float yfac;
  short xoff;
  short yoff;

  u8 touchtype;
}_m_tp_dev;


#define PEN  		PBin(1)     //T_PEN
#define DOUT 		PBin(2)     //T_MISO
#define TDIN 		PFout(11)   //T_MOSI
#define TCLK 		PBout(0)    //T_SCK
#define TCS  		PCout(13)   //T_CS

u8 TP_Init(void);
u8 TP_Scan(u8 tp);

_m_tp_dev tp_dev =
{
  TP_Init,
  TP_Scan,
  null,
  {0,0,0,0,0},
  {0,0,0,0,0},

  0,

  -0.0891,
  -0.0681,
  340,
  260,

  0,
};

u8 CMD_RDX = 0X90;
u8 CMD_RDY = 0XD0;

void TP_Delay(volatile u32 lu32Num)
{
  lu32Num *= 8;

  while (lu32Num)
  {
    lu32Num--;
  }
}



void TP_Write_Byte(u8 num)
{
  u8 count = 0;
  for (count = 0; count < 8; count++)
  {
    if (num & 0x80)TDIN = 1;
    else TDIN = 0;
    num <<= 1;
    TCLK = 0;
    TP_Delay(1);
    TCLK = 1;
  }
}


u16 TP_Read_AD(u8 CMD)
{
  u8 count = 0;
  u16 Num = 0;
  TCLK = 0;
  TDIN = 0;
  TCS = 0;
  TP_Write_Byte(CMD);
  TP_Delay(6);
  TCLK = 0;
  TP_Delay(1);
  TCLK = 1;
  TP_Delay(1);
  TCLK = 0;
  for (count = 0; count < 16; count++)
  {
    Num <<= 1;
    TCLK = 0;
    TP_Delay(1);
    TCLK = 1;
    if (DOUT)Num++;
  }
  Num >>= 4;
  TCS = 1;
  return(Num);
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
  for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)sum += buf[i];
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
  if (PEN == 0)
  {
    if (tp)TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);
    else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))
    {
      tp_dev.x[0] = tp_dev.xfac*tp_dev.x[0] + tp_dev.xoff;
      tp_dev.y[0] = tp_dev.yfac*tp_dev.y[0] + tp_dev.yoff;
    }
    if ((tp_dev.sta&TP_PRES_DOWN) == 0)
    {
      tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;
      tp_dev.x[4] = tp_dev.x[0];
      tp_dev.y[4] = tp_dev.y[0];
    }

    mcCursor.vTick10ms(tp_dev.x[0], tp_dev.y[0], 1);
  }
  else
  {
    if (tp_dev.sta&TP_PRES_DOWN)
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
  return tp_dev.sta&TP_PRES_DOWN;
}



u8 TP_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE);

  //GPIOB1,2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]);

  return 1;
}





void FSMC_SRAM_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);//??PD,PE,PF,PG??
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);//??FSMC??


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 ????,????
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOB, &GPIO_InitStructure);//??? //PB15 ????,????

  GPIO_InitStructure.GPIO_Pin = (3 << 0) | (3 << 4) | (0XFF << 8);//PD0,1,4,5,8~15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOD, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (3 << 0) | (0X1FF << 7);//PE0,1,7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOE, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (0X3F << 0) | (0XF << 12);   //PF0~5,12~15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOF, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (0X3F << 0) | GPIO_Pin_10;//PG0~5,10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOG, &GPIO_InitStructure);//???


  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//PD15,AF12

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);//PE15,AF12

  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);//PF0,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);//PF1,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);//PF2,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);//PF3,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);//PF4,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);//PF5,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);//PF13,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);//PF14,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);//PF15,AF12

  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);


  readWriteTiming.FSMC_AddressSetupTime = 0x00;   //??????(ADDSET)?1?HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;   //??????(ADDHLD)??A???
  readWriteTiming.FSMC_DataSetupTime = 0x08;     ////??????(DATAST)?9?HCLK 6*9=54ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;   //??A



  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  ??????NE3 ,????BTCR[4],[5]?
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//????????16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;  //??????
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // ?????????
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //??????

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //???FSMC??

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // ??BANK3
}

#define PI                         (float)     3.14159265f
float AccBuffer[3];
float MagBuffer[3];
float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng2 = 0.0f, PitchAng2 = 0.0f;
float fTiltedX,fTiltedY = 0.0f;
float HeadingValue = 0.0f;
int i;

void MAIN_vTick10msLp(void)
{
  static uint8 lui8t = 0;
  //static int8 lui8DirX = 1;
  //static int8 lui8DirY = 1;
  TP_Scan(0);

  if (lui8t == 5)
  {
    cMsg lcMsg;

    mcDiffTimer.start();

    while (!mcMsgBox.isEmpty())
    {
      mcMsgBox.vget(&lcMsg);
      mcUi16GTree.vMsgHdl(&lcMsg, &mc16GScreen1);
    }

    lui8t = 0;
    mc16GBm.vFill(BM_BPP16_5R6G5B_RGB(0xFF,0,0));



    mcMag.vReadMag();
    mcAcc.vReadAcc();

    char8 lszValue[12];

    cStrTools::uixItoa(mcMag.i16GetMagX(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 30, lszValue, &mc16GScreen1);
    cStrTools::uixItoa(mcMag.i16GetMagY(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 40, lszValue, &mc16GScreen1);
    cStrTools::uixItoa(mcMag.i16GetMagZ(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 50, lszValue, &mc16GScreen1);

    cStrTools::uixItoa(mcAcc.i16GetAccX(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(40, 30, lszValue, &mc16GScreen1);
    cStrTools::uixItoa(mcAcc.i16GetAccY(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(40, 40, lszValue, &mc16GScreen1);
    cStrTools::uixItoa(mcAcc.i16GetAccZ(), lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(40, 50, lszValue, &mc16GScreen1);


    AccBuffer[0] = mcAcc.i16GetAccX();
    AccBuffer[1] = mcAcc.i16GetAccY();
    AccBuffer[2] = mcAcc.i16GetAccZ();


    MagBuffer[0] = mcMag.i16GetMagX();
    MagBuffer[1] = mcMag.i16GetMagY();
    MagBuffer[2] = mcMag.i16GetMagZ();

    MagBuffer[0] = MagBuffer[0] * 1000 / 230;
    MagBuffer[1] = MagBuffer[1] * 1000 / 230;
    MagBuffer[2] = MagBuffer[2] * 1000 / 205;


    for(i=0;i<3;i++)
      AccBuffer[i] /= 100.0f;

    fNormAcc = sqrtf((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));

    fSinRoll = -AccBuffer[1]/fNormAcc;
    fCosRoll = sqrtf(1.0f-(fSinRoll * fSinRoll));
    fSinPitch = AccBuffer[0]/fNormAcc;
    fCosPitch = sqrtf(1.0f-(fSinPitch * fSinPitch));

    if (AccBuffer[2] < 0 ) {fCosPitch = -fCosPitch;fCosRoll = -fCosRoll;}

    RollAng2 = acosf(fCosRoll)*180.0f/PI;

    if ( fSinRoll < 0)
    {
      RollAng2 = -RollAng2;
    }

    PitchAng2 = acosf(fCosPitch)*180.0f/PI;

    if ( fSinPitch < 0)
    {
      PitchAng2 = -PitchAng2;
    }
    
    // https://www.jameco.com/jameco/products/prodds/2150248.pdf
    // page 23
    fTiltedY = 0 +
               MagBuffer[1]* fCosRoll +
               MagBuffer[2]* fSinRoll;
    fTiltedX = MagBuffer[0]* fCosPitch +
               MagBuffer[1]* fSinRoll*fSinPitch
             - MagBuffer[2]* fCosRoll*fSinPitch;

    HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;


    if (HeadingValue < 0)
    {
      HeadingValue = HeadingValue + 360;
    }

    cStrTools::uixItoa((int)HeadingValue, lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 60, lszValue, &mc16GScreen1);

    cStrTools::uixItoa((int)RollAng2, lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 70, lszValue, &mc16GScreen1);
    cStrTools::uixItoa((int)PitchAng2, lszValue, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 80, lszValue, &mc16GScreen1);

    if (RollAng2 < 0)
    {
      RollAng2 = RollAng2 + 360;
    }

    if (PitchAng2 < 0)
    {
      PitchAng2 = PitchAng2 + 360;
    }

    mc16GUi2b1.mesh_rotation.x = PitchAng2;
    mc16GUi2b1.mesh_rotation.y = HeadingValue;
    mc16GUi2b1.mesh_rotation.z = RollAng2;

    if (HeadingValue > 180)
    {
      HeadingValue = HeadingValue - 360;
    }

    cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 20, mcDiffTimer.toString(), &mc16GScreen1);

    mcUi16GTree.vPaint(&mc16GScreen1);
    ILI9341_Show(&mc16GBm);
  }
  else
  {
    lui8t++;
  }
}


void MAIN_vTick1000msLp(void)
{
  static uint8 lui8t = 0;
  mcLedRed.Toggle();
  lui8t++;
}


void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();

  mcClkInfo.Update();

  FSMC_SRAM_Init();
  ILI9341_Init();
  TP_Init();

  mcChnA.vAddXY(10, 10);
  mcChnA.vAddXY(20, 20);
  mcChnA.vAddXY(30, 10);
  mcChnA.vAddXY(40, 10);
  mcChnA.vAddXY(50, 10);
  mcChnA.vAddXY(60, 20);
  mcChnA.vAddXY(70, 20);
  mcChnA.vAddXY(80, 10);
  mcChnA.mui32Col = 0xFF0000;

  mcChnB.vAddXY(10, 30);
  mcChnB.vAddXY(20, 20);
  mcChnB.vAddXY(30, 30);
  mcChnB.vAddXY(40, 40);
  mcChnB.vAddXY(50, 50);
  mcChnB.vAddXY(60, 60);
  mcChnB.vAddXY(70, 90);
  mcChnB.vAddXY(80, 80);
  mcChnB.mui32Col = 0x00FF00;

  mcUi16GTree.vAdd(0, &mc16GUi1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b2);

  //mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi3);
  //mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi3a);

  //mc16GUi3a.mstStatus.DragAble  = 0;
  //mc16GUi3a.mstStatus.Maximized = 1;
  //mcUi16GTree.vUpdateMaximize(&mc16GUi3a);

  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi4);
  mcUi16GTree.vAdd(&mc16GUi4, &mc16GUi4a);
  mcUi16GTree.vAdd(&mc16GUi4, &mc16GUi4b);
  mc16GUi2.vSetText("TEXT");

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

  
  mcI2C1.bStartNext();
  mcClkInfo.Delay_ms(100);

  mcMag.vSetConfig(LSM303DLHC_MAG_TEMPSENSOR_ENABLE, LSM303DLHC_MAG_ODR_15_HZ, LSM303DLHC_MAG_FS_8_1_GA, LSM303DLHC_MAG_CONTINUOS_CONVERSION);
  mcAcc.vSetControl(LSM303DLHC_ACC_ODR_25_HZ | LSM303DLHC_ACC_NORMAL_MODE | LSM303DLHC_ACC_X_ENABLE | LSM303DLHC_ACC_Y_ENABLE | LSM303DLHC_ACC_Z_ENABLE,
                     LSM303DLHC_ACC_HPM_NORMAL_MODE | LSM303DLHC_ACC_HPFCF_32,
                     0, // No Ints
                     LSM303DLHC_ACC_BlockUpdate_Continous | LSM303DLHC_ACC_FULLSCALE_2G,
                     0,
                     0
                     );

  mcI2C1.bStartNext();
  mcClkInfo.Delay_ms(100);

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
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
    __asm("wfi");
  }
}

