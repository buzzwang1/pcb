
#include "main.h"

__IO uint32_t TimingDelay = 0;

//cI2cMaster* mcI2C1;
//cAPDS9960* mcAPDS9960;

LED<GPIOF_BASE, 9>   mcLedRed;
cClockInfo mcClkInfo;

#define DISPLAY_X 320
#define DISPLAY_Y 240

uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2] __attribute__((section(".ExtMem")));
uint8 mExtMemory[10000] __attribute__((section(".ExtMem")));
uint8 mIntMemory[10000];

cDiffTimer                              mcDiffTimer;

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);


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

void DMA1_Channel1_IRQHandler(void)
{
}

void DMA1_Channel2_IRQHandler(void)
{
}

void DMA1_Channel3_IRQHandler(void)
{

}

void DMA1_Channel4_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    /* Disable the DMA1 Channel 4 */
    DMA_Cmd(DMA1_Stream4, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    /* Disable the DMA1 Channel 5 */
    DMA_Cmd(DMA1_Stream5, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
  }
}

void DMA1_Channel6_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Tx

  //if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
  {
    /* Disable the DMA1 Channel 6 */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
  }
}

void DMA1_Channel7_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Rx
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    /* Disable the DMA1 Channel 7 */
    DMA_Cmd(DMA1_Stream7, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
  }
}


void I2C1_EV_IRQHandler(void)
{
  mcI2C1->I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1->I2C_ER_IRQHandler();
}

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


class cMatrix_4x4
{
public:

  float m[4][4];

  arm_matrix_instance_f32 mstMat1;
  arm_matrix_instance_f32 mstMat2;
  arm_matrix_instance_f32 mstRet;

  cMatrix_4x4(bool lbInit)
  {
    if (lbInit) Set_Default();

    arm_mat_init_f32(&mstMat1, 4, 4, null);
    arm_mat_init_f32(&mstMat2, 4, 4, null);
    arm_mat_init_f32(&mstRet, 4, 4, (float*)m);
  }

  void Set_Default()
  {
    float DefaultMat[4][4] =
    { {1.0f,   0.0f,   0.0f,   0.0f},
     {0.0f,   1.0f,   0.0f,   0.0f},
     {0.0f,   0.0f,   1.0f,   0.0f},
     {0.0f,   0.0f,   0.0f,   1.0f} };

    Set(DefaultMat);
  }

  void Set(float lMatrix[4][4])
  {
    unsigned char r, c;

    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        m[c][r] = lMatrix[c][r];
      }
    }
  }

  void Set(cMatrix_4x4 lcMatrix)
  {
    unsigned char r, c;

    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        m[c][r] = lcMatrix.m[c][r];
      }
    }
  }

  void Multiply(cMatrix_4x4& mat1, cMatrix_4x4& mat2)
  {
    cMatrix_4x4 lcRet(false);
    unsigned char r, c;
    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        lcRet.m[c][r] = mat1.m[r][0] * mat2.m[0][c] +
                        mat1.m[r][1] * mat2.m[1][c] +
                        mat1.m[r][2] * mat2.m[2][c] +
                        mat1.m[r][3] * mat2.m[3][c];
      }
    }
    *this = lcRet;
  }

  void MultiplyDsp(cMatrix_4x4& mat1, cMatrix_4x4& mat2)
  {
    mstMat1.pData = (float*)mat1.m;
    mstMat2.pData = (float*)mat2.m;
    mstRet.pData = (float*)m;
    arm_mat_mult_f32(&mstMat1, &mstMat2, &mstRet);
  }
};

class cVector4f
{
public:

  float n[4];


  arm_matrix_instance_f32 mstMat;
  arm_matrix_instance_f32 mstVec;

  float nret[4];
  arm_matrix_instance_f32 mstRetVec;

  cVector4f()
  {                         //   R, C
    arm_mat_init_f32(&mstMat,    4, 4, null);
    arm_mat_init_f32(&mstVec,    4, 1, null);
    arm_mat_init_f32(&mstRetVec, 4, 1, (float*)n);
  }

  void Set(float lVec[4])
  {
    unsigned char c;

    for (c = 0; c < 4; c++)
    {
      n[c] = lVec[c];
    }
  }

  void Multiply(cMatrix_4x4& m, cVector4f& v)
  {
    nret[0] = v.n[0] * m.m[0][0] + v.n[1] * m.m[0][1] + v.n[2] * m.m[0][2] + v.n[3] * m.m[0][3];
    nret[1] = v.n[0] * m.m[1][0] + v.n[1] * m.m[1][1] + v.n[2] * m.m[1][2] + v.n[3] * m.m[1][3];
    nret[2] = v.n[0] * m.m[2][0] + v.n[1] * m.m[2][1] + v.n[2] * m.m[2][2] + v.n[3] * m.m[2][3];
    nret[3] = v.n[0] * m.m[3][0] + v.n[1] * m.m[3][1] + v.n[2] * m.m[3][2] + v.n[3] * m.m[3][3];

    n[0] = nret[0];
    n[1] = nret[1];
    n[2] = nret[2];
    n[3] = nret[3];
  }

  void MultiplyDsp(cMatrix_4x4& mat, cVector4f& v)
  {
    mstMat.pData    = (float*)mat.m;
    mstVec.pData    = (float*)v.n;
    mstRetVec.pData = (float*)v.nret;

    arm_mat_mult_f32(&mstMat, &mstVec, &mstRetVec);
  }
};




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


void MAIN_vTick10msLp(void)
{
  //mcI2C1->bStartNext();
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
    // Construct function
    SystemInit();

    mcClkInfo.Update();

    FSMC_SRAM_Init();
    ILI9341_Init();
  #endif


  #ifndef _WIN32
  #if 0
    CycCall_Start(NULL /*1ms_HP*/,
                  NULL /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
  #endif
  #endif
}

int main(void)
{
  u8     lu8t;
  u16    lu16Y = 30;
  uint32 lu32TestLoop;

  static volatile uint32 lu32Value1 = 5;
  static volatile uint32 lu32Value2 = 4;
  static volatile uint32 lu32Value3 = 3;

  static volatile float  lfValue1 = 5.0f;
  static volatile float  lfValue2 = 4.0f;
  static volatile float  lfValue3 = 3.0f;

  UNUSED(lu32Value3);
  UNUSED(lfValue3);

  float MyMatA[4][4] = { {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f}};

  float MyMatB[4][4] = { {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f},
                         {1.0f,     2.0f,     3.0f,    4.0f}};

  float MyMatAmulB[4][4] = { {  10.0,   20.0,   30.0,   40.0},
                             {  10.0,   20.0,   30.0,   40.0},
                             {  10.0,   20.0,   30.0,   40.0},
                             {  10.0,   20.0,   30.0,   40.0}};

  UNUSED(MyMatAmulB);

  cMatrix_4x4           lcMat1(false);
  cMatrix_4x4           lcMat2(false);
  cMatrix_4x4           lcMat3(false);

  lcMat1.Set(MyMatA);
  lcMat2.Set(MyMatB);

  float MyVec[4] = { 1.0f,   2.0f,   3.0f,  4.0f };

  cVector4f             lcVec1;
  cVector4f             lcVec2;

  lcVec1.Set(MyVec);
  lcVec2.Set(MyVec);

  cDStr lcStrOutput(64);

  MAIN_vInitSystem();

  mc16GScreen1.vFill(0x0);

  cRFont_Res8b_Bpp1_1G_Full.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, 10, (const char*)"MemTest", &mc16GScreen1);
  ILI9341_Show(&mc16GBm);


  mcDiffTimer.start();
  for (lu8t = 0; lu8t < 100; lu8t++)
  {
    cMemTools::vMemSet(mExtMemory, 0x55, 10000);
  }
  lcStrOutput.Setf("ExtMem Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  lu16Y += 10;
  mcDiffTimer.start();
  for (lu8t = 0; lu8t < 100; lu8t++)
  {
    cMemTools::vMemSet(mIntMemory, 0x55, 10000);
  }
  lcStrOutput.Setf("IntMem Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);


  #define TEST_REPETITIONS 10000

  // ----- Nop -----
  lu16Y += 10;
  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint

  mcDiffTimer.start();
  #define MyTest asm("nop")
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("Just NOP Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint


  // ----- Integer multiplication -----
  lu16Y += 10;
  mcDiffTimer.start();
  #define MyTest lu32Value3 = lu32Value3 * lu32Value2
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("Int mul: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint


  // ----- Integer division -----
  lu16Y += 10;
  lu32Value3 = 1000000;
  mcDiffTimer.start();
  #define MyTest lu32Value3 = lu32Value3 / lu32Value2
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("Int div: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint


  // ----- Float multiplication -----
  lu16Y += 10;
  mcDiffTimer.start();
  #define MyTest lfValue3 = lfValue3 * lfValue2
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("Float mul: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint


  // ----- Float division -----
  lu16Y += 10;
  mcDiffTimer.start();
  #define MyTest lfValue3 = lfValue3 / lfValue2
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("Float div: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint


  // ----- 4x4 Matrix multiplication -----
  lu16Y += 10;
  mcDiffTimer.start();
  #define MyTest lcMat3.Multiply(lcMat1, lcMat2);
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("4x4 Matrix Mul Raw Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint

  // ----- 4x4 Matrix multiplication using ARM Dsp lib -----
  mcDiffTimer.start();
  lu16Y += 10;
  #define MyTest lcMat3.MultiplyDsp(lcMat1, lcMat2);
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("4x4 Matrix Mul DspLib Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint

  // ----- 4x4 Matrix Vector multiplication -----
  lu16Y += 10;
  lcVec2 = lcVec1;
  mcDiffTimer.start();
  #define MyTest lcVec2.Multiply(lcMat2, lcVec2);
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("4x4 Matrix Vec Mul Raw Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);
  ILI9341_Show(&mc16GBm);

  asm("nop"); // for breakpoint
  asm("nop"); // for breakpoint

  // ----- 4x4 Matrix Vector multiplication using ARM Dsp lib -----
  lu16Y += 10;
  lcVec2 = lcVec1;
  mcDiffTimer.start();
  #define MyTest lcVec2.MultiplyDsp(lcMat2, lcVec2);
  for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
  {
    MyTest; MyTest; MyTest; MyTest; MyTest;
    MyTest; MyTest; MyTest; MyTest; MyTest;
  }
  #undef MyTest
  lcStrOutput.Setf("4x4 Matrix Vec Mul DspLib Test: %s us", mcDiffTimer.toString());
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(0, lu16Y, lcStrOutput.ToString(), &mc16GScreen1);


  ILI9341_Show(&mc16GBm);

  while (1)
  {
    CycCall_vIdle();
    __asm("wfi");
  }
}

