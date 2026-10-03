
#include "main.h"
#include "tusb.h"

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

/**
 * @brief Callback wird von TinyUSB aufgerufen, sobald der PC Daten an den 
 *        Vendor-OUT-Endpoint sendet.
 */
void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize)
{
  // - CFG_TUD_VENDOR_TXRX_BUFFERED = 1: buffer and bufsize must not be used (both NULL,0) since data is in RX FIFO
  (void)idx;
  (void)buffer;
  (void)bufsize;

  uint8_t buf[64];
  uint32_t count = tud_vendor_read(buf, sizeof(buf));

  if (count == 0) return;

  uint8_t cmd = buf[0];

  switch (cmd) 
  {
    case 0xF1: // STLINK_CMD_GET_VERSION
    {
      // Antworte mit vorgegaukelter ST-Link/V2 Firmware (6 Bytes)
      // Format: V2J37S7 -> V2 (0x22, 0x25), JTAG V37, SWIM V7
      uint8_t version[6] = { 0x22, 0x25, 0x00, 0x25, 0x00, 0x07 };
      tud_vendor_write(version, sizeof(version));
      tud_vendor_write_flush();
      break;
    }

    case 0xF2: // STLINK_CMD_GET_CURRENT_MODE
    {
      if (count == 1)
      {
        // Antworte mit Modus : 0x02 (JTAG / SWD Mode)
        uint8_t mode[2] = { 0x02, 0x00 };
        tud_vendor_write(mode, sizeof(mode));
        tud_vendor_write_flush();
      }
      else
      {
        // Wenn der ST - Link im JTAG / SWD - Modus ist(0x02), schickt der PC Befehle, 
        // die fast alle mit dem Byte 0xF2 beginnen.Das zweite Byte(pbuf[1]) bestimmt dann die 
        // eigentliche Aktion(Sub - Command):
        // Wenn pbuf[0] == 0xF2, werte pbuf[1] aus:
        uint8_t subcmd = buf[1];

        switch (subcmd)
        {
          case 0x30: // SWD - Initialisierung: STLINK_DEBUG_ENTER_SWD(Aktiviert das SWD - Protokoll am Pin).
          {
            __asm("nop");
            break;
          }
          case 0x31: // Target - Reset : Schaltet die Reset - Leitung(NRST) des Ziel - Mikrocontrollers.
          {
            __asm("nop");
            break;
          }
          case 0x32: // Speicher lesen : STLINK_DEBUG_READMEM_32BIT(Liest 32 - Bit - Werte aus dem RAM / Flash des Ziel - Chips).
          {
            __asm("nop");
            break;
          }
          case 0x33: // Speicher schreiben : STLINK_DEBUG_WRITEMEM_32BIT(Schreibt Daten in das RAM / Flash des Ziel - Chips).
          {
            __asm("nop");
            break;
          }
          case 0x34: // Run / Go : Startet die CPU des Ziel - Chips(STLINK_DEBUG_RUNCORE).
          {
            __asm("nop");
            break;
          }
          case 0x35: // Halt / Stop : Stoppt die CPU des Ziel - Chips für das Debugging(STLINK_DEBUG_HALTCORE).
          {
            __asm("nop");
            break;
          }
          case 0x36: // Einzelschritt : STLINK_DEBUG_STEPCORE(Führt exakt einen Assembler - Befehl aus).
          {
            __asm("nop");
            break;
          }
          case 0x42: // Register lesen : Liest die Core - Register(R0 - R15, SP, PC, xPSR) der ARM - CPU aus.
          {
            __asm("nop");
            break;
          }
          case 0x43: // Register schreiben : Ändert den Inhalt der Core - Register.
          {
            __asm("nop");
            break;
          }
          case 0x48: // DAP - Transfer : (Ab ST - Link V2J24) Direkter Zugriff auf den ARM Debug Access Port.
          {
            __asm("nop");
            break;
          }
        }
      }
      break;
    }

    case 0xF5: // STLINK_CMD_GET_TARGET_VOLTAGE
    {
      // Sendet die Target-Spannung als ADC-Werte zurück (entspricht ca. 3.3V)
      uint8_t voltage[8] = { 0xE4, 0x07, 0x00, 0x00, 0xE4, 0x07, 0x00, 0x00 };
      tud_vendor_write(voltage, sizeof(voltage));
      tud_vendor_write_flush();
      break;
    }

    case 0xF7: // STLINK_CMD_GET_VERSION_EXT
    {
     // Erweiterte Versionsabfrage(wird von neueren ST - Link - Firmwares genutzt).
     __asm("nop");
     break;
    }

    case 0x07: // STLINK_CMD_EXIT_DFU_MODE	Verlässt den Bootloader - Modus(DFU).
    {
      __asm("nop");
      break;
    }

    case 0x20: // STLINK_CMD_ENTER_JTAG_MODE	Schaltet den ST - Link in den JTAG / SWD - Modus um.
    {
      __asm("nop");
      break;
    }

    case 0x21: // STLINK_CMD_ENTER_SWIM_MODE	Schaltet in den STM8 - SWIM - Modus um.
    {
      __asm("nop");
      break;
    }

    case 0x22: // STLINK_CMD_EXIT_MODE	Verlässt den aktuellen Modus und geht zurück in den IDLE - Zustand.
    {
      __asm("nop");
      break;
    }

    default: 
    {
        // Unbekannter / spezifischer Programmierbefehl: 
        // Direkt an die serielle Schnittstelle (UART) weiterleiten
        //HAL_UART_Transmit_DMA(&huart1, buf, count);
        break;
    }
  }
}


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
    tud_task(); // Verarbeitet USB-Events im Hintergrund
  }
}


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
