
#include "main.h"



// STM32F429
// ARM®-based Cortex®-M4 32b MCU, (180 MHz max)
// Rom 2048KB
// Ram 256KB

__IO uint32_t TimingDelay = 0;

LED<GPIOG_BASE, 13> mcLedRed;

#define MAIN_nDISPLAY_X ILI9341_WIDTH
#define MAIN_nDISPLAY_Y ILI9341_HEIGHT

uint8                                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y * 2];
cBitmap_Bpp16_5R6G5B                     mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B                     mcScreen1(&mcBm);
cSprite_Res8b_Bpp1_1G                    mcSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mcSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


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


void MAIN_vTick1000msLp(void)
{
  mcLedRed.Toggle();
}

void MAIN_vInitSystem(void)
{
  SystemInit();
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  LCD_Init();
  LCD_LayerInit();

  /* LTDC reload configuration */
  LTDC_ReloadConfig(LTDC_IMReload);

  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);

  LCD_SetLayer(LCD_BACKGROUND_LAYER);
  LCD_Clear(LCD_COLOR_GREY);

  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  LCD_Clear(LCD_COLOR_MAGENTA);

  ILI9341_Show(&mcBm);
  cClockInfo::Delay_ms(100);


  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
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

