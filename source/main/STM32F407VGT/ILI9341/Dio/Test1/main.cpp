
#include "main.h"



// STM32F429
// ARM®-based Cortex®-M4 32b MCU, (180 MHz max)
// Rom 2048KB
// Ram 256KB



RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

uint8  mui8Heap[8];
Screen MAIN_stDisplay;

LED* lcLedRed;


void NMI_Handler(void)
{
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
}

void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
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
  static uint8 lui8t = 0;
  lcLedRed->Toggle();
  lui8t++;
  //TFT_Clear(lui8t);
}


int8 BPP1_1G_Tools_i8ClearScreen(uint16 lui16Col, Screen lstScr)
{
  uint32      lui32t;

  SCREEN_CHK_INIT(lstScr);

  for (lui32t = 0; lui32t < SCREEN_BPP16_5R6G5B_SCREEN_SIZE(lstScr); lui32t+=2)
  {
    lstScr->aui8Data[lui32t]   = lui16Col >> 8;
    lstScr->aui8Data[lui32t+1] = lui16Col & 0xFF;
  }

  return 0;
}


int8 BPP1_1G_Tools_i8ClearScreen2(uint16 lui16Col, Screen lstScr)
{
  uint32      lui32t;
  uint16*     lpui16ScreenData;

  SCREEN_CHK_INIT(lstScr);

  lpui16ScreenData = (uint16*)lstScr->aui8Data;

  for (lui32t = 0; lui32t < 320*240; lui32t++)
  {
    *lpui16ScreenData = lui16Col;
    lpui16ScreenData++;
  }

  return 0;
}

void TIM2_vInit();
void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();

  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  MemWrap_vInit(mui8Heap, sizeof(mui8Heap));
  //Screen_i8Create(SCREEN_BPP16_5R6G5B, ILI9341_WIDTH, ILI9341_HEIGHT, &MAIN_stDisplay);

  //ILI9341_Init(MAIN_stDisplay);

  ILI9341_Init();

  //LCD_SetLayer(LCD_BACKGROUND_LAYER);
  //LCD_Clear(LCD_COLOR_GREY);

  //LCD_SetLayer(LCD_FOREGROUND_LAYER);
  //LCD_Clear(LCD_COLOR_MAGENTA);


  /*uint16 lui16Col;

  BPP1_1G_Tools_i8ClearScreen(ILI9341_COLOR_RED, MAIN_stDisplay);
  ILI9341_Show(MAIN_stDisplay);

  lui16Col = ((uint16*)(MAIN_stDisplay->aui8Data))[0];

  Delay(100);

  BPP1_1G_Tools_i8ClearScreen(ILI9341_COLOR_BLUE, MAIN_stDisplay);
  ILI9341_Show(MAIN_stDisplay);

  lui16Col = ((uint16*)(MAIN_stDisplay->aui8Data))[0];

  Delay(100);

  BPP1_1G_Tools_i8ClearScreen(ILI9341_COLOR_GREEN, MAIN_stDisplay);
  ILI9341_Show(MAIN_stDisplay);

  lui16Col = ((uint16*)(MAIN_stDisplay->aui8Data))[0];*/

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  lcLedRed = new LED(RCC_AHB1Periph_GPIOD, GPIOD, GPIO_Pin_12);
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

