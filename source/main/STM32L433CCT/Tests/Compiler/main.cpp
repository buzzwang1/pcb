
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// HSI 16MHz


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


const u8 RomData[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                      10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                      20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                      30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                      40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                      50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                      60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
                      70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                      80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

typedef struct __attribute__ ((__packed__))
{
  u8 u8Src[64];
  u8 u8Dest[64];

} tstMemTest;

tstMemTest mstMemTest;

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  HAL_Init();

  u16 lu16Len = 1;
  while (1)
  {
    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, lu16Len);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, 2);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, 3);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, 4);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, 5);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Src, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Src, (u8*)RomData, 64);

    //----------------------------------------------

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, lu16Len);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, 2);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, 3);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, 4);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, 5);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest, (u8*)RomData, 64);



    //----------------------------------------------

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData + 1, 1);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData + 1, 2);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData + 1, 8);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData + 1, 40);


    //----------------------------------------------

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData, 1);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData, 2);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData, 8);

    __asm("nop");
    cMemToolsHw::vMemSet((u8*)mstMemTest.u8Dest, 0, 64);
    cMemToolsHw::vMemCpy((u8*)mstMemTest.u8Dest + 1, (u8*)RomData, 40);
  }
}

void SysError_Handler()
{
  while (1)
  {
  };
}

void SystemClock_Config(void)
{
  // SystemClock = HSI (== 16Mhz)
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART1|
                                       RCC_PERIPHCLK_I2C3|
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C1;
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
  __HAL_RCC_RTCAPB_CLK_ENABLE();

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

