
#include "main.h"



// STM32F407
// ARM®-based Cortex®-M4 32b MCU, (180 MHz max)
// Rom 1024KB
// Ram 192KB


__IO uint32_t TimingDelay = 0;

LED<GPIOF_BASE, 9> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 8, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 1);
cGpPin lcSDA(GPIOB_BASE, 9, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 1);

LED<GPIOB_BASE, 4> lcLedI2cDmaTxTc;
LED<GPIOB_BASE, 5> lcLedI2cDmaRxTc;
LED<GPIOB_BASE, 6> lcLedI2cInt;
LED<GPIOB_BASE, 7> lcLedI2cErr;


cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);

#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G          mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


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


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  //1ms at 168Mhs
  uint32 lui32DecValue = 168000000 / cClockInfo::mstClocks.HCLK_Frequency;

  if (TimingDelay > lui32DecValue)
  {
    TimingDelay -= lui32DecValue;
  }
  else
  {
    TimingDelay = 0;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
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
  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }
}



void MAIN_vTick100msLp(void)
{
  static int16 li16RectX = 0;
  static int16 li16RectY = 5;
  static int16 li16RectXDir = 1;
  static int16 li16RectYDir = 1;

  mcLedRed.Toggle();

  mcScreen1.vFill(0);
  mcScreen1.vLine(0,0, MAIN_nDISPLAY_X-1, MAIN_nDISPLAY_Y-1, 1);

  cPaint::vRect(10, 10, 100, 40, 1, &mcScreen1);

  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 10, (char8*)"Spg Solar 1234",  &mcScreen1);

  if (li16RectX > 127) li16RectXDir = -1;
  if (li16RectY > 63)  li16RectYDir = -1;
  if (li16RectX < 1)   li16RectXDir = 1;
  if (li16RectY < 1)   li16RectYDir = 1;

  li16RectX += li16RectXDir;
  li16RectY += li16RectYDir;

  cPaint::vRect(li16RectX, li16RectY, 2, 2, 1, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(li16RectX, li16RectY, (char8*)"xyz",  &mcScreen1);


  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
}




void MAIN_vInitSystem(void)
{
  SystemInit();
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL               /*1s_LP*/);

  if (mcI2C1.bStartNext())
  {
    Delay(100);
  }
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}

