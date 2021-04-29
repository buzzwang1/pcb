
#include "main.h"

// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB


__IO uint32_t TimingDelay = 0;

//Gnd freischalten
RCC_ClocksTypeDef RCC_Clocks;
cClockInfo mcClockInfo; //On top to init this class at first
LED<GPIOC_BASE, 13> mcLedRed;

cHx8347 mcDisplay;

#define MAIN_nDISPLAY_X HX8347A_WIDTH
#define MAIN_nDISPLAY_Y HX8347A_HEIGHT

uint8                     mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];   // 40.960 Bytes
cBitmap_Bpp1_1G           mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G           mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


tcGpPin<GPIOB_BASE, 0>  mPinBgPwm(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 1);


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
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}


void DMA1_Channel5_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel5->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA1_FLAG_GL5 | DMA1_FLAG_HT5 | DMA1_FLAG_TC5;
  mcDisplay.vEvDmaTc();
}


void MAIN_vTick100msLp(void)
{
  mcLedRed.Toggle();

  if (mcDisplay.isReady())
  {
    static int16 li16RectX = 0;
    static int16 li16RectY = 5;
    static int16 li16RectXDir = 1;
    static int16 li16RectYDir = 1;

    mcScreen1.vFill(0);

    mcScreen1.vLine(0,0, MAIN_nDISPLAY_X-1, MAIN_nDISPLAY_Y-1, 1);

    cPaint::vRect(10, 10, MAIN_nDISPLAY_X-20, MAIN_nDISPLAY_Y-20, 1, &mcScreen1);

    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 10, (char8*)"Spg Solar 1234",  &mcScreen1);

    if (li16RectX > MAIN_nDISPLAY_X) li16RectXDir = -1;
    if (li16RectY > MAIN_nDISPLAY_Y)  li16RectYDir = -1;
    if (li16RectX < 1)   li16RectXDir = 1;
    if (li16RectY < 1)   li16RectYDir = 1;

    li16RectX += li16RectXDir;
    li16RectY += li16RectYDir;

    cPaint::vRect(li16RectX, li16RectY, 2, 2, 1, &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(li16RectX, li16RectY, (char8*)"xyz",  &mcScreen1);

    mcDisplay.vShowScreenDma(mcScreen1.mpcBm->mpui8Data);
  }
}

void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  // SysTick end of count event each 10ms
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();

  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL               /*1s_LP*/);


}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

