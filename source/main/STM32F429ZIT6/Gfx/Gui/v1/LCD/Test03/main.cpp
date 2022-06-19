#include "main.h"



// STM32F429
// ARM®-based Cortex®-M4 32b MCU, (180 MHz max)
// Rom 2048KB
// Ram 256KB

__IO uint32_t TimingDelay = 0;


LED<GPIOG_BASE, 13> mcLedRed;
cClockInfo mcClkInfo;

#define MAIN_nDISPLAY_X ILI9341_WIDTH
#define MAIN_nDISPLAY_Y ILI9341_HEIGHT

uint8 mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y * 2];// __attribute__((section(".ExtMem")));


cMsgBox                                 mcMsgBox(10);
cCursor                                 mcCursor(200, 800, &mcMsgBox);


cBitmap_Bpp16_5R6G5B                     mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B                     mcScreen1(&mcBm);
cSprite_Res8b_Bpp1_1G                    mcSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mcSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);

cUiElement                          mc16GUi1(30, 30, 120, 90, &mcMsgBox);
cUiElement_TextBox                  mc16GUi2(20, 15,  45, 40, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement                          mc16GUi3(60, 40, 120, 90, &mcMsgBox);
cUiElement                          mc16GUi4(10,  0,  20, 10, &mcMsgBox);
cUiElement                          mc16GUi5(30, 30,  40, 40, &mcMsgBox);

cUiEleTree                          mcUi16GTree(10);

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


void MAIN_vTick10msLp(void)
{
  static uint8 lui8t = 0;
  static int8 lui8DirX = 1;
  static int8 lui8DirY = 1;

  if (lui8t == 5)
  {
    lui8t = 0;
    mcBm.vFill(BM_BPP16_5R6G5B_RGB(0xFF,0,0));
    mcBm.vPp(160, 100, 1);
    mcScreen1.vLine(0, 0, 319, 199, BM_BPP16_5R6G5B_RGB(0, 0xFF, 0));

    mcSpriteEng.vSetParam(Sprite_nModeOr, &mcSprite, &mcScreen1);

    mcSpriteEng.i8Set(10, 10, SPRTMST_DATA_MainBottom);

    mcUi16GTree.vPaint(&mcScreen1);

    if (mc16GUi1.miGfxRefX <   0) lui8DirX =  1;
    if ((mc16GUi1.miGfxRefX + mc16GUi1.miGfxWidth) > 320) lui8DirX = -1;
    if (mc16GUi1.miGfxRefY <   0) lui8DirY =  1;
    if ((mc16GUi1.miGfxRefY + mc16GUi1.miGfxHeight) > 240) lui8DirY = -1;

    mcUi16GTree.vMoveRelXY(&mc16GUi1, lui8DirX, lui8DirY, 0, 0);
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

  SDRAM_Init();

  mcBm.vFill(BM_BPP16_5R6G5B_GREEN);
  ILI9341_Init(&mcBm);

  mcUi16GTree.vAdd(0, &mc16GUi1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi3);
  mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi4);
  mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi5);
  //mcUi16GTree.vBringToTop(&mc16GUi2);

  mc16GUi2.vSetText("TEXT");

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                NULL               /*100ms_LP*/,
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

