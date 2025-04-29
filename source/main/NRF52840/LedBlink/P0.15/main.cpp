
#include "main.h"



// nRF52840
// ARM®-based Cortex®-M4 32b MCU, (64 MHz max)
// Rom 1024KB
// Ram 256KB

// pca10056 = > nRF52840 eval board
// pca10059 = > nRF52840 usb dongle


u32 TimingDelay = 0;

//LED<GPIOC_BASE, 13> lcLedRed;

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


void Delay(u32 nTime)
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
  //HAL_IncTick();
}



void MAIN_vTick1000msLp(void)
{
  static u8 lu85s = 0;


  if (lu85s == 5)
  {
    lu85s = 0;
    //lcLedRed.Toggle();
  }
  else
  {
    lu85s++;
  }
}


void MAIN_vInitSystem(void)
{
  //cClockInfo::Update();
  //SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  SystemInit();


  //CycCall_Start(NULL /*1ms_HP*/,
  //              NULL /*10ms_HP*/,
  //              NULL /*100ms_HP*/,
  //              NULL /*1s_HP*/,

  //              NULL               /*1ms_LP*/,
  //              NULL               /*10ms_LP*/,
  //              NULL               /*100ms_LP*/,
  //              MAIN_vTick1000msLp /*1s_LP*/);
}

#include "nrf_drv_systick.h"
#include "boards.h"

int main(void)
{
  /*MAIN_vInitSystem();

  while (1)
  {
    //CycCall_vIdle();
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    //__asm("wfi");
  }*/

  /* Configure LED-pins as outputs. */
  bsp_board_init(BSP_INIT_LEDS);

  /* Init systick driver */
  nrf_drv_systick_init();

  /* Toggle LEDs. */
  while (true)
  {
    for (int i = 0; i < LEDS_NUMBER; i++)
    {
      bsp_board_led_invert(i);
      nrf_drv_systick_delay_ms(500);
    }
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{/*
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }

  // Initializes the CPU, AHB and APB busses clocks
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                                       |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                                       |RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  // Configure the main internal regulator output voltage
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }*/
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  //SystemInit();
  //SystemClock_Config(); // Decomment for 16Mhz HSI
//  SystemCoreClockUpdate();
}

