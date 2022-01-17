
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;

//Gnd freischalten
cClockInfo mcClockInfo; //On top to init this class at first
LED<GPIOC_BASE, 13> mcLedRed;

cST7735 mcST7735(cST7735::ScrOrientation_TypeDef::scr_CCW);

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cINA219      mcINA219_A(&mcI2C1, INA219_I2C_ADDRESS_CONF_1, 100);
#define MAIN_nDISPLAY_X ST7735_HEIGTH
#define MAIN_nDISPLAY_Y ST7735_WIDTH 

uint8                     mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y * 2];   // 40.960 Bytes
cBitmap_Bpp16_5R6G5B      mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B      mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


stPxli16                  mcGraphChnlVBusPts[100];
ciGraphXY_Channel         mcGraphChnlVBus(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcGraphChnlVBusPts, 100, null);
ciGraphXY                 mcGraphXY(&cRFont_Res8b_Bpp1_1G_5x5Ucase, MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, &mcScreen1);


// 1,2,3,4 Tasten
cGpPin lcBtn_Gnd(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin lcBtn_1(GPIOB_BASE, 1, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0);
cGpPin lcBtn_2(GPIOB_BASE, 0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0);
cGpPin lcBtn_3(GPIOB_BASE,11, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0);
cGpPin lcBtn_4(GPIOB_BASE,10, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0);

int32 li32VBus   = 0;
int32 li32IShunt = 0;

int32 li32VBusAvg   =  0;
int32 li32VBusMin   =  99999;
int32 li32VBusMax   =  0;
int32 li32VPowerCounter   =  0;

int32 li32IShuntAvg =  0;
int32 li32IShuntMin =  99999;
int32 li32IShuntMax =  0;

int32 mi32Dac;

typedef enum
{
  MAIN_nGuiMode_FirstEntry = 0,
  MAIN_nGuiMode_VI = MAIN_nGuiMode_FirstEntry,
  MAIN_nGuiMode_ConstVoltage,
  MAIN_nGuiMode_ConstCurrent,
  MAIN_nGuiMode_LastEntry
}tenGuiMode;

tenGuiMode menGuiMode    = MAIN_nGuiMode_VI;
uint32     mu32SetVoltage = 0;
uint32     mu32SetCurrent = 0;

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
  HAL_IncTick();
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


void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          =  DMA_FLAG_TC3;
  mcST7735.vEvDmaTc();
}

void I2C1_EV_IRQHandler(void)
{
  mcI2C1.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1.I2C_ER_IRQHandler();
}

void MAIN_vTick10msHp(void)
{
  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }

  mcINA219_A.i8ReadVShunt_digit();
  mcINA219_A.i8ReadVBus_digit();

  li32IShunt = -mcINA219_A.i32CalcIShunt_uA() / 1000;
  li32VBus = mcINA219_A.i32CalcVBus_uV() / 1000;

  switch (menGuiMode)
  {
    case MAIN_nGuiMode_VI:
      break;

    case MAIN_nGuiMode_ConstVoltage:
            
      if ((u32)li32VBus < mu32SetVoltage)
      {
        if (((u32)li32VBus + 500) < mu32SetVoltage)
        {
          if (mi32Dac > 10) mi32Dac -= 10;
          else mi32Dac = 0;
        }
        else
        if (((u32)li32VBus + 250) < mu32SetVoltage)
        {
          if (mi32Dac > 5) mi32Dac -= 5;
          else mi32Dac = 0;
        }
        else
        if ((u32)li32VBus < mu32SetVoltage)
        {
          if (mi32Dac > 0) mi32Dac--;        }
      }
      else
      {
        if ((li32VBus + 500) > mu32SetVoltage)
        {
          mi32Dac+=10;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
        else if ((li32VBus + 250) > mu32SetVoltage)
        {
          mi32Dac+=5;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
        else
        {
          mi32Dac++;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
      }
      
      LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, mi32Dac);
      break;
    
    case MAIN_nGuiMode_ConstCurrent:
      if ((u32)li32IShunt > mu32SetCurrent)
      {
        if ((li32IShunt + 50) > mu32SetCurrent)
        {
          if (mi32Dac > 50) mi32Dac -= 50;
          else mi32Dac = 0;
        }
        else if ((li32IShunt + 15) > mu32SetCurrent)
        {
          if (mi32Dac > 25) mi32Dac -= 25;
          else mi32Dac = 0;
        }
        else
        {
          if (mi32Dac > 0) mi32Dac--;
        }
      }
      else
      {
        if (((u32)li32IShunt + 50) < mu32SetCurrent)
        {
          mi32Dac+=50;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
        else
        if (((u32)li32IShunt + 15) < mu32SetCurrent)
        {
          mi32Dac+=25;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
        else
        if ((u32)li32IShunt < mu32SetCurrent)
        {
          mi32Dac++;
          if (mi32Dac > 4095) mi32Dac = 4095;
        }
      }
      LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, mi32Dac);

      break;
    default:
      break;
  }
}

void MAIN_vTick100msLp(void)
{

  static u8 li8Idx = 0;
  static u8 lu8Btns = 0;
  static u8 lu8Btns_old = 0;
  char lszValue[16] = "";  
  
  mcLedRed.Toggle();

  if (mcST7735.isReady())
  {
    mcScreen1.vFill(BM_BPP16_5R6G5B_BLACK);

    cRFont_Res8b_Bpp1_1G_Full.mpcSpriteEng->menMode = Sprite_nModeCopy;

    if (li32VBus < li32VBusMin) li32VBusMin = li32VBus;
    if (li32VBus > li32VBusMax) li32VBusMax = li32VBus;
    if (li32IShunt < li32IShuntMin) li32IShuntMin = li32IShunt;
    if (li32IShunt > li32IShuntMax) li32IShuntMax = li32IShunt;

    li32VBusAvg   = (li32VBusAvg   + li32VBus)   / 2;
    li32IShuntAvg = (li32IShuntAvg + li32IShunt) / 2;

    if (lcBtn_1.ui8Get() != 0)
    {
      li32VBusAvg   =  li32VBus;
      li32VBusMin   =  99999;
      li32VBusMax   =  0;
      li32VPowerCounter   =  0;

      li32IShuntAvg =  li32IShunt;
      li32IShuntMin =  99999;
      li32IShuntMax =  0;

      li8Idx = 0;
      mcGraphChnlVBus.vClear();

      mcGraphChnlVBus.mi16SkalYMax = 10;
    }

    li8Idx++;
    if (li8Idx > 99)
    {
      li8Idx = 100;
    }
    mcGraphChnlVBus.vAddXY(li8Idx, li32IShunt);
      
    while (li32IShunt > mcGraphChnlVBus.mi16SkalYMax)
    {
      switch (mcGraphChnlVBus.mi16SkalYMax)
      {
        case  10: mcGraphChnlVBus.mi16SkalYMax =   20; break;
        case  20: mcGraphChnlVBus.mi16SkalYMax =   50; break;
        case  50: mcGraphChnlVBus.mi16SkalYMax =  100; break;
        case 100: mcGraphChnlVBus.mi16SkalYMax =  200; break;
        case 200: mcGraphChnlVBus.mi16SkalYMax =  500; break;
        case 500: mcGraphChnlVBus.mi16SkalYMax = 1000; break;
        default: mcGraphChnlVBus.mi16SkalYMax = li32IShuntMax;
      }
    }

    mcGraphXY.vPaintAndScroll(&mcGraphChnlVBus, 1);

    u8 lu8xOffset = 10;

    cRFont_Res8b_Bpp1_1G_Full.mpcSpriteEng->menMode = Sprite_nModeOr;

    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0x00FF00);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10 + lu8xOffset, 30, (char8*)"V [mV]:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(80 + lu8xOffset, 30, (char8*)"avg:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 20, (char8*)"max:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 40, (char8*)"min:",  &mcScreen1);

    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0x00FF00);
    cStrTools::uixItoa(li32VBus,    lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 50 + lu8xOffset, 30, lszValue,  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
    cStrTools::uixItoa(li32VBusAvg, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(105 + lu8xOffset, 30, lszValue,  &mcScreen1);
    cStrTools::uixItoa(li32VBusMax, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 75 + lu8xOffset, 20, lszValue,  &mcScreen1);
    cStrTools::uixItoa(li32VBusMin, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 75 + lu8xOffset, 40, lszValue,  &mcScreen1);

    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFF0000);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10 + lu8xOffset, 70, (char8*)"I [mA]:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(80 + lu8xOffset, 70, (char8*)"avg:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 60, (char8*)"max:",  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 80, (char8*)"min:",  &mcScreen1);

    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFF0000);
    cStrTools::uixItoa(li32IShunt,    lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 50 + lu8xOffset, 70, lszValue,  &mcScreen1);
    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
    cStrTools::uixItoa(li32IShuntAvg, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(105 + lu8xOffset, 70, lszValue,  &mcScreen1);
    cStrTools::uixItoa(li32IShuntMax, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 75 + lu8xOffset, 60, lszValue,  &mcScreen1);
    cStrTools::uixItoa(li32IShuntMin, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY( 75 + lu8xOffset, 80, lszValue,  &mcScreen1);

    lszValue[0] = 0;
    lu8Btns_old = lu8Btns;
    lu8Btns = 0;
    if (lcBtn_1.ui8Get() != 0) { cStrTools::szStrCat(lszValue, "1"); lu8Btns |= 1;}
                          else cStrTools::szStrCat(lszValue, "0");
    if (lcBtn_2.ui8Get() != 0) { cStrTools::szStrCat(lszValue, "1"); lu8Btns |= 2;}
                          else cStrTools::szStrCat(lszValue, "0");
    if (lcBtn_3.ui8Get() != 0) { cStrTools::szStrCat(lszValue, "1"); lu8Btns |= 4;}
                          else cStrTools::szStrCat(lszValue, "0");
    if (lcBtn_4.ui8Get() != 0) { cStrTools::szStrCat(lszValue, "1"); lu8Btns |= 8;}
                          else cStrTools::szStrCat(lszValue, "0");

    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10 + lu8xOffset, 100, lszValue, &mcScreen1);

    // Change Mode
    if (((lu8Btns & 2) == 2) && ((lu8Btns_old & 2) == 0))
    {
      if (menGuiMode == MAIN_nGuiMode_VI)           {menGuiMode = MAIN_nGuiMode_ConstVoltage;LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, 0);}
      else
      if (menGuiMode == MAIN_nGuiMode_ConstVoltage) {menGuiMode = MAIN_nGuiMode_ConstCurrent;LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, 0);}
      else
      if (menGuiMode == MAIN_nGuiMode_ConstCurrent) {menGuiMode = MAIN_nGuiMode_VI;LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, 0);}
      else menGuiMode = MAIN_nGuiMode_VI;
    }
   
    switch (menGuiMode)
    {
      case MAIN_nGuiMode_VI:
        cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(80, 8, (char8*)"Mode VI", &mcScreen1);
        break;

      case MAIN_nGuiMode_ConstVoltage:
        cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0x00FF00);
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(80, 8, (char8*)"Mode V_const", &mcScreen1);
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10 + lu8xOffset, 110, (char8*)"U [mV]:", &mcScreen1);
        cStrTools::uixItoa(mu32SetVoltage, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 110, lszValue, &mcScreen1);
        cStrTools::uixItoa(mi32Dac, lszValue, 10);        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(100 + lu8xOffset, 110, lszValue, &mcScreen1);
        if (lu8Btns & 4) { if (mu32SetVoltage > 100) mu32SetVoltage -= 100; else mu32SetVoltage = 0; }
        if (lu8Btns & 8) { if (mu32SetVoltage < 20000) mu32SetVoltage += 100; }
        if ((((u32)li32VBus) > (mu32SetVoltage - 250)) && (((u32)li32VBus) < (mu32SetVoltage + 250)))
        {
          li32VPowerCounter += li32IShunt;
        }
        cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFF00);
        cStrTools::uixItoa(li32VPowerCounter / 1000, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 100, lszValue, &mcScreen1);
        break;

      case MAIN_nGuiMode_ConstCurrent:
        cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFF0000);
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(80, 8, (char8*)"Mode I_const", &mcScreen1);
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10 + lu8xOffset, 110, (char8*)"I [mA]:", &mcScreen1);
        cStrTools::uixItoa(mu32SetCurrent, lszValue, 10); cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(50 + lu8xOffset, 110, lszValue, &mcScreen1);
        cStrTools::uixItoa(mi32Dac, lszValue, 10);        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(100 + lu8xOffset, 110, lszValue, &mcScreen1);
        if (lu8Btns & 4) { if (mu32SetCurrent > 5) mu32SetCurrent -= 5; else mu32SetCurrent = 0; }
        if (lu8Btns & 8) { if (mu32SetCurrent < 10000) mu32SetCurrent += 5; }
        break;
      default:
        break;
    }
    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);

    mcST7735.vShowScreenDma(mcScreen1.mpcBm->mpui8Data);
  }
}

void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* DAC */
  LL_DAC_InitTypeDef DAC_InitStructure;

  tcGpPin<GPIOA_BASE, 4> mGnd1(GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);

  __HAL_RCC_DAC1_CLK_ENABLE();

  /* DAC channel1 Configuration */
  LL_DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;
  LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_InitStructure);
  mi32Dac = 1023/2;
  LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, mi32Dac);
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);

  mcGraphChnlVBus.mi16SkalYMax = 10;
  mcGraphChnlVBus.mui32Col = 0x0FF0000;
  mcGraphChnlVBus.vSet_Y_Label((const char8*)"i [mA]");
  mcGraphChnlVBus.mi16SkalYCnt = 10;
  //mcGraph.vSetSpace(5, 5);
  mcGraphXY.vSet_X_Label((const char8*)"t");

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
    cClockInfo::Delay_ms(100);
  }
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

void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1|
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

