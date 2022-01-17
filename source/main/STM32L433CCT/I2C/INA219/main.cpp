
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB

__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);
cINA219      mcINA219_A(&mcI2C1, INA219_I2C_ADDRESS_CONF_0, 100);
cINA219      mcINA219_B(&mcI2C1, INA219_I2C_ADDRESS_CONF_1, 100);

tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE,  10> mcComPort(9600, GPIO_AF7_USART2, 64, 64);
//tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort(9600, GPIO_AF7_USART2, 64, 64);

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


void I2C1_EV_IRQHandler(void)
{
  mcI2C1.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1.I2C_ER_IRQHandler();
}

void USART1_IRQHandler(void)
{
  mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  mcComPort.vIRQHandler();
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
  mcINA219_A.i8ReadVShunt_digit();
  mcINA219_A.i8ReadVBus_digit();

  mcINA219_B.i8ReadVShunt_digit();
  mcINA219_B.i8ReadVBus_digit();
}

void MAIN_vTick1000msLp(void)
{
  int32 li32IShunt = 0;
  int32 li32VBus   = 0;

  char lszValue[16] = "";
  char lszGrad[] = " mA";
  char lszProz[] = " mV";

  //mcHTU21D->i8ReadUser_digit();

  mcLedRed.Toggle();

  mcScreen1.vFill(0);

  li32IShunt = mcINA219_A.i32CalcIShunt_uA() / 1000;
  li32VBus   = mcINA219_A.i32CalcVBus_uV()   / 1000;

  cStrTools::uixItoa(li32IShunt, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszGrad);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 10, (char8*)"I:",  &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 10, lszValue,      &mcScreen1);

  mcComPort.vAdd((char8*)"I:");mcComPort.vAdd(lszValue);

  cStrTools::uixItoa(li32VBus, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszProz);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 20, (char8*)"V:",   &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 20, lszValue,       &mcScreen1);

  mcComPort.vAdd((char8*)" | V:");mcComPort.vAdd(lszValue);

  li32IShunt = mcINA219_B.i32CalcIShunt_uA() / 1000;
  li32VBus   = mcINA219_B.i32CalcVBus_uV()   / 1000;

  cStrTools::uixItoa(li32IShunt, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszGrad);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 40, (char8*)"I:",  &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 40, lszValue,      &mcScreen1);

  mcComPort.vAdd((char8*)" | I:");mcComPort.vAdd(lszValue);

  cStrTools::uixItoa(li32VBus, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszProz);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 50, (char8*)"V:",   &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 50, lszValue,       &mcScreen1);

  mcComPort.vAdd((char8*)" | V:");mcComPort.vSend(lszValue);

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
}




void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);


  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(100);
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  mcComPort.vSend("Start\n\r");
  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
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
  PeriphClkInit.PeriphClockSelection = //RCC_PERIPHCLK_USART1|
                                       //RCC_PERIPHCLK_USART2|
                                       //RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1;//|
                                       //RCC_PERIPHCLK_I2C2|
                                       //RCC_PERIPHCLK_I2C3;
  //PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  //PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  //PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  //PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  //PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
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

