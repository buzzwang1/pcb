
#include "main.h"


// Test Projekt zum Vermessen eines 10k NTC
// 10k NTC ist über einen 10K in Reihe an PA2 angeschlossen (ADC1_IN7)
// Über I2C ist ein LM75 in parallel als Temperaturreferenz


// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz


//  Power Switch v3 LED add on
//
//  PB09  -> Status Led
//
//
//  OutPuts:
//    PA01: High side switch for 3V3
//  OutPuts:
//    PB00: High side switch for CH1
//    PB01: High side switch for CH2
//    PB02: High side switch for CH3
//
//  I2C3  -> Board (Eeprom Adr.:0x50, INA3221 Adr.: 0x40)
//    PA07  -> I2C3 SCL  AF4
//    PB04  -> I2C3 SDA  AF4
//
//  Ext. Flash
//    SPI1
//      PA10: CS
//      PA11: Miso AF5
//      PA12: Mosi AF5
//      PB03: Clk  AF5
//
//  Power Module
//    PA04: DAC
//    PA05: Enable
//
//  BotNet DownLink
//    I2C2
//      PB13  -> I2C2 SCL AF4
//      PB14  -> I2C2 SDA AF4
//    U3
//      PB10  -> TX/RX    AF7
//
//  BotNet UpLink
//    I2C1
//      PB06  -> I2C1 SCL AF4
//      PB07  -> I2C1 SDA AF4
//    U1
//      PA09  -> TX/RX    AF7
//
//  Timer Usage:
//    TIM1  -> PWM für WS2812 for DMA1.Chl2
//    TIM2  -> PWM für WS2812 for DMA1.Chl3
//    TIM6  -> CyclicCaller
//    TIM15 -> BotCom MpHd UpLink
//    TIM16 -> BotCom MpHd DownLink

//
//  DMA Usage:
//    DMA1:
//      0:
//      1: TIM2 für WS2812   CS:4
//      2: I2C3 Tx: Board    CS:3 / U3 Tx DownLink CS:2 / TIM1 for WS2812 CS:7
//      3: I2C3 Rx: Board    CS:3 / U3 Rx DownLink CS:2
//      4: I2C2 Tx: DownLink CS:3 / U1 Tx UpLink CS:2
//      5: I2C2 Rx: DownLink CS:3 / U1 Rx UpLink CS:2
//      6: I2C1 Tx: UpLink   CS:3
//      7: I2C1 Rx: UpLink   CS:3
//
//    DMA2:
//      0:
//      1:
//      2:
//      3: SPI1 Rx  CS:4
//      4: SPI1 Tx  CS:4
//      5:
//      6: I2C1 Rx: BotNet CS:5 / U1 Tx UpLink CS:2
//      7: I2C1 Tx: BotNet CS:5 / U1 Rx UpLink CS:2
//
//  Interrupt Usage:
//    DMA1_Channel1_IRQHandler: WS2812:        Prio: 8.8
//    I2C1_EV_IRQHandler:       BotCom:        Prio: 8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio: 8.8
//    I2C2_EV_IRQHandler:       Board:         Prio: 8.8
//    I2C2_ER_IRQHandler:       Board:         Prio: 8.8
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio: 6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio: 6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio: 6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio: 6.8 => U1
//
//

enum
{
  nV_Temp = 7,

  nIdx_Temp = 0,

  nChnCount = 1,
};

u16    mAdcResult[nChnCount];
float  mfAdcResultAvg[nChnCount];


// Call first to get the system clocks
// This is even called before the inlcuded classs in the header
cClockInfo mcClockInfo;
cGpPin mcDigi1(GPIOA_BASE, 1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);

cGpPin     mcI2c3_SCL_Board(GPIOA_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin     mcI2c3_SDA_Board(GPIOB_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cI2cMaster mcI2C3_Board(I2C3, &mcI2c3_SCL_Board, &mcI2c3_SDA_Board, DMA1, DMA1_Channel3, DMA1_Channel2, 8);
LED<GPIOB_BASE, 9> mcLed;

cSSD1306    mcSSD1306(&mcI2C3_Board, 0x78, 56); // == 3C * 2
cLm75       mcLm75(&mcI2C3_Board, LM75_I2C_ADDRESS_DEFAULT);

#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory1[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
uint8                    mDisplayMemory2[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm1(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory1);
cScreen_Bpp1_1G          mcScreen1(&mcBm1);
cBitmap_Bpp1_1G          mcBm2(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory2);
cScreen_Bpp1_1G          mcScreen2(&mcBm2);
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

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void I2C3_EV_IRQHandler(void)
{
  mcI2C3_Board.I2C_EV_IRQHandler();
}

void I2C3_ER_IRQHandler(void)
{
  mcI2C3_Board.I2C_ER_IRQHandler();
}


void MAIN_vTick1msHp(void)
{
}


void MAIN_vTick10msLp();

void MAIN_vTick1msLp(void)
{
  static u8 lu8Cnt1ms = 0;

  mcI2C3_Board.bStartNext();

  lu8Cnt1ms++;
  if (lu8Cnt1ms == 10)
  {
    lu8Cnt1ms = 0;
    MAIN_vTick10msLp();
  }
}


void MAIN_vItoa_HUD21D(int num, char8* str)
{
  int32 li32Val1 = 0;
  int32 li32Val2 = 0;
  char lszVal1[8] = "";
  char lszVal2[8] = "x";
  char lszPkt[] = ",";

  li32Val1 = num / 2;
  li32Val2 = num - (li32Val1 * 2);

  cStrTools::uixItoa(li32Val1, lszVal1, 10);

  if (li32Val2)
  {
    lszVal2[0] = '5';
  }
  else
  {
    lszVal2[0] = '0';
  }

  cStrTools::szStrCpy(str, lszVal1);
  cStrTools::szStrCat(str, lszPkt);
  cStrTools::szStrCat(str, lszVal2);
}

void MAIN_vTick100msLp()
{
  static u8 lu8t = 0;

  //mcScreen1.vFill(0);
  mcScreen2.vFill(0);
  
  {
    int32 li32Temp = 0;
  
    char lszValue[16] = "";
  
    mcLm75.vCmdReadTemp();
  
    li32Temp = mcLm75.i16GetTemp();
  
    MAIN_vItoa_HUD21D(li32Temp, lszValue);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 10, (char8*)"T:",  &mcScreen2);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(20, 10, lszValue,      &mcScreen2);

    cStrTools::uixItoa(mAdcResult[nIdx_Temp], lszValue, 10);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(20, 20, lszValue, &mcScreen2);
  }

  for (u16 lu16x = 0; lu16x < MAIN_nDISPLAY_X; lu16x++)
  {
    for (u16 lu16y = 0; lu16y < MAIN_nDISPLAY_Y; lu16y++)
    {
      mcScreen1.vPpAbsRaw(lu16x, lu16y, mcScreen2.u32GpAbsRaw(lu16x / 2, lu16y / 2));
    }
  }

  if (lu8t & 1)
    mcScreen1.vLine(1, 1, 8, 8, 1);
  else
    mcScreen1.vLine(8, 1, 1, 8, 1);

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
  mcLed.Toggle();
  lu8t++;
}
void MAIN_vTick10msLp()
{
  static u16 luCnt100ms = 0;
  luCnt100ms++;
  if (luCnt100ms == 10)
  {
    luCnt100ms = 0;
    MAIN_vTick100msLp();
  }
}

void vSetSMPR(u8 lu8Chl, u8 lu8Value)
{
  if (lu8Chl <= 9) ADC1->SMPR1 |= lu8Value << (lu8Chl * 3);
  else ADC1->SMPR2 |= lu8Value << ((lu8Chl - 10) * 3);
}

void cInit_Adc()
{
  cGpPin mcTemp(GPIOA_BASE, 2, GPIO_MODE_ANALOG_ADC_CONTROL, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

  __HAL_RCC_DMA2_CLK_ENABLE();

  // ----------- DMA ---------
  DMA_HandleTypeDef lhDMA = {};

  // DMA interrupt is not used
  // DMA Tx
  lhDMA.Instance       = DMA2_Channel3;
  lhDMA.Init.Request   = DMA_REQUEST_0;
  lhDMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
  lhDMA.Init.PeriphInc = DMA_PINC_DISABLE;
  lhDMA.Init.MemInc    = DMA_MINC_ENABLE;
  lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  lhDMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  lhDMA.Init.Mode     = DMA_NORMAL;
  lhDMA.Init.Priority = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&lhDMA);

  /* Configure DMA Channel data length */
  lhDMA.Instance->CNDTR = nChnCount;
  /* Configure DMA Channel destination address */
  lhDMA.Instance->CPAR = (uint32_t) & (ADC1->DR);
  /* Configure DMA Channel source address */
  lhDMA.Instance->CMAR = (uint32_t) & (mAdcResult);

  // configure DMA mode
  LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);

  // enable DMA channel0
  DMA2_Channel3->CCR |= DMA_CCR_EN;


  // ------------ ADC_Config
// enable the DMA clock
  __HAL_RCC_ADC_CLK_ENABLE();

  // ADC clock
  // The ADC clock is derived from the system clock, or from the PLLSAI1 output(a). It can reach
  // 80 MHz and can be divided by the following prescalers values: 1,2,4,6,8,10,12,16,32,64,128
  // or 256 by configuring the ADC1_CCR register. It is asynchronous to the AHB clock.
  // Alternatively, the ADC clock can be derived from the AHB clock of the ADC bus interface,
  // divided by a programmable factor (1, 2 or 4). This programmable factor is configured using
  // the CKMODE bit fields in the ADC1_CCR.
  // If the programmed factor is ‘1’, the AHB prescaler must be set to ‘1’

  // 24Mhz / 6 => ADC Takt 4Mhz => 0.25us
  ADC1_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos); // ADC Clock = HCLK = 24Mhz
  ADC1_COMMON->CCR |= (3 << ADC_CCR_PRESC_Pos);  // Pres = 6 => 4Mhz

  // 1. Ensure DEEPPWD=0, ADVREGEN=1 and that ADC voltage regulator startup time (20us) has elapsed.
  // 2. Ensure that ADEN=0.
  // 3. Select the input mode for this calibration by setting ADCALDIF=0 (single-ended input) or ADCALDIF=1 (differential input).
  // 4. Set ADCAL=1.
  // 5. Wait until ADCAL=0.
  // 6. The calibration factor can be read from ADC_CALFACT register

  ADC1->CR &= ~ADC_CR_DEEPPWD;
  ADC1->CR |= ADC_CR_ADVREGEN;
  cClockInfo::Delay_us(20);

  // ADC calibration and reset calibration
  ADC1->CR |= ADC_CR_ADCAL;
  // Wait for calibration finished.
  while (ADC1->CR & ADC_CR_ADCAL) {};


  // ADC regular channel config
  ADC1->SQR1 =  nV_Temp; ADC1->SQR1 <<= 6;
  //ADC1->SQR1 |= nV_bat3; ADC1->SQR1 <<= 6;
  //ADC1->SQR1 |= nV_bat2; ADC1->SQR1 <<= 6;
  //ADC1->SQR1 |= nV_bat1; ADC1->SQR1 <<= 6;
  //ADC1->SQR2 |= nV_Vsys;
  ADC1->SQR1 |= nChnCount - 1;

  //   0  16000000  4000000   2, 5  12, 5  15     3,75  5   18,75
  //   1  16000000  4000000   6, 5  12, 5  19     4,75  5   23,75
  //   2  16000000  4000000  12, 5  12, 5  25     6,25  5   31,25
  //   3  16000000  4000000  24, 5  12, 5  37     9,25  5   46,25
  //   4  16000000  4000000  47, 5  12, 5  60    15,00  5   75,00
  //   5  16000000  4000000  92, 5  12, 5  105   26,25  5  131,25
  //   6  16000000  4000000 247, 5  12, 5  260   65,00  5  325,00
  //   7  16000000  4000000 640, 5  12, 5  653  163,25  5  816,25
  vSetSMPR(nV_Temp, 6);
  //vSetSMPR(nV_bat2, 6);
  //vSetSMPR(nV_bat3, 6);
  //vSetSMPR(nV_bat4, 6);
  //vSetSMPR(nV_Vsys, 6);


  // ADC DMA enable and Cont_Mode and Circular Mode
  ADC1->CFGR |= ADC_CFGR_DMAEN | ADC_CFGR_CONT | (1 << 1);

  // enable ADC interface
  ADC1->CR |= ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY)) {};

  // Start ADC convertion => runs forever
  ADC1->CR |= ADC_CR_ADSTART;
}

void MAIN_vInitSystem(void)
{
  cInit_Adc();

  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);
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
  {
    __asm("nop");
  }
}

void SystemClock_Config_HSE(void)
{
  // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct   = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  // Configure the main internal regulator output voltage
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

  SystemClock_Config_HSE();
  SystemCoreClockUpdate();
}

