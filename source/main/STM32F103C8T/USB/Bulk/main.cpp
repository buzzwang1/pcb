
#include "main.h"


// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 24KB

//__IO uint32_t TimingDelay = 0;

//LED<GPIOC_BASE, 13> lcLedRed;


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


//void Delay(__IO uint32_t nTime)
//{
//  TimingDelay = nTime;
//
//  while(TimingDelay != 0);
//}
//
//
//void TimingDelay_Decrement(void)
//{
//  if (TimingDelay != 0x00)
//  {
//    TimingDelay--;
//  }
//}
//
//
//void SysTick_Handler(void)
//{
//  TimingDelay_Decrement();
//}



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

// Erzwingt, dass der PC das Gerät neu erkennt
void usb_hardware_reset(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA12 (USB D+) kurz hart auf Low ziehen
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(50); // 50ms reichen aus

  // Pin wieder freigeben, damit die USB-Peripherie ihn übernehmen kann
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);
}

//void MAIN_vTick100msLp(void)
//{
//}
//
//void MAIN_vTick1000msLp(void)
//{
//  //lcLedRed.Toggle();
//}


void MAIN_vInitSystem(void)
{
  /* SysTick end of count event each 10ms */
  HAL_Init();

  //CycCall_Start(NULL /*1ms_HP*/,
  //              NULL /*10ms_HP*/,
  //              NULL /*100ms_HP*/,
  //              NULL /*1s_HP*/,
  //
  //              NULL               /*1ms_LP*/,
  //              NULL               /*10ms_LP*/,
  //              MAIN_vTick100msLp /*100ms_LP*/,
  //              MAIN_vTick1000msLp /*1s_LP*/);

  // Blue Pill USB-Reset-Schnittstelle triggern
  usb_hardware_reset();

  board_init();
  board_led_write(0);
  // TinyUSB Stack initialisieren
  tusb_init();
}



int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    //CycCall_vIdle();

    tud_task(); // Verarbeitet USB-Events im Hintergrund

    // Eigene App-Logik für den Daten-Loopback
    if (tud_vendor_available())
    {
      uint8_t buffer[64];
      // Daten vom PC (Bulk OUT) lesen
      uint32_t count = tud_vendor_read(buffer, sizeof(buffer));

      // Verarbeitung... (z.B. eine LED umschalten)

      // Daten zurück an PC (Bulk IN) senden
      tud_vendor_write(buffer, count);
      tud_vendor_write_flush();
    }
  }
}

/*
void RCC_Config_72MHz_HSE_USB48(void)
{
  // 1. RCC auf Default-Zustand zurücksetzen (Sicherheits-Reset)
  RCC_DeInit();

  // 2. Den externen Quarzoszillator (HSE) einschalten
  RCC_HSEConfig(RCC_HSE_ON);

  // Warten, bis der HSE stabil bereitsteht
  if (RCC_WaitForHSEStartUp() == SUCCESS)
  {
    // 3. Flash-Speicher anpassen (Wichtig bei 72 MHz)
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);

    // 4. Bus-Vorteiler (Prescaler) einstellen
    RCC_HCLKConfig(RCC_SYSCLK_Div1);   // AHB CLK = 72 MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);    // APB1 CLK = 36 MHz (Max 36 MHz!)
    RCC_PCLK2Config(RCC_HCLK_Div1);    // APB2 CLK = 72 MHz


    // 5. USB-Takt konfigurieren (72MHz / 1.5 = 48MHz)
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

    // 6. PLL konfigurieren (8 MHz HSE / 1 * 9 = 72 MHz)
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    // PLL aktivieren und warten, bis sie stabil steht
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
      __asm("nop");
    }

    // 7. Die PLL als System-Taktquelle (SYSCLK) auswählen
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // Warten, bis die PLL wirklich als System-Takt genutzt wird
    while (RCC_GetSYSCLKSource() != 0x08)
    {
      // 0x08 bedeutet, dass die PLL als SYSCLK aktiv ist
      __asm("nop");
    }
  }
  else
  {
    // HSE Start fehlgeschlagen!
    while (1)
    {
      __asm("nop");
    }
  }
}*/

void RCC_Config_72MHz_HSE_USB48(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct  = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct  = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    __asm("nop");
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    __asm("nop");
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection    = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    __asm("nop");
  }
}



void MainSystemInit()
{
  SystemInit();
  RCC_Config_72MHz_HSE_USB48();
  SystemCoreClockUpdate();
}
