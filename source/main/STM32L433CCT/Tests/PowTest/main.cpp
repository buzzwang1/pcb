
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> lcLedRed;
tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcComPort(19200, GPIO_AF7_USART2, 256, 64);
//tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort(19200, GPIO_AF7_USART2, 64, 64);

bool mbLedActivated = True;

class cCliCmd_Test : public cCliCmd
{
  u16 mu16Cnt;

  public:
  cCliCmd_Test():cCliCmd((const char*)"Test", (const char*)"This is a Test. Prints a big text") {}
  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);

    if (lbFirstCall)
    {
      mu16Cnt = 16;
    }

    while (mu16Cnt)
    {
      if (lcCli->isSpace(20))
      {
        if (!lbFirstCall)
        {
          lcCli->bPrint('X');
        }

        for (u8 t = 0; t < 16; t++)
        {
          lcCli->bPrint('a' + t);
        }
        lcCli->bPrint('\r');
        lcCli->bPrint('\n');
      }
      else
      {
        return False;
      }
      mu16Cnt--;
    }
    return True;
  }
};


class cCliCmd_SysInfo: public cCliCmd
{
  public:
    cCliCmd_SysInfo():cCliCmd((const char*)"SysInfo", (const char*)"System Info") 
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lcCli->bPrintLn((const char8*)"Clock:");
      lszStr.Setf((const char8*)"Core: %dMHz",  cClockInfo::u32GetClockCore() / 1000000); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Sys:  %dMHz",  cClockInfo::u32GetClockSys()  / 1000000); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Port: %dMHz",  cClockInfo::u32GetClockPort() / 1000000); lcCli->bPrintLn(lszStr);

      if (FLASH->ACR & FLASH_ACR_ICEN)  {lszStr.Setf((const char8*)"Flash Data cache: enable");  lcCli->bPrintLn(lszStr);}
                                  else  {lszStr.Setf((const char8*)"Flash Data cache: disable"); lcCli->bPrintLn(lszStr);}

      if (FLASH->ACR & FLASH_ACR_DCEN)  {lszStr.Setf((const char8*)"Flash Instruction cache: enable");  lcCli->bPrintLn(lszStr);}
                                  else  {lszStr.Setf((const char8*)"Flash Instruction cache: disable"); lcCli->bPrintLn(lszStr);}

      if (FLASH->ACR & FLASH_ACR_PRFTEN)  {lszStr.Setf((const char8*)"Flash Prefetch: enable");  lcCli->bPrintLn(lszStr);}
                                    else  {lszStr.Setf((const char8*)"Flash Prefetch: disable"); lcCli->bPrintLn(lszStr);}

      u8 lu8Lat = (u8)((u32)(FLASH->ACR & FLASH_ACR_LATENCY_Msk) >> FLASH_ACR_LATENCY_Pos);
      lszStr.Setf((const char8*)"Flash Latency: %d", lu8Lat); lcCli->bPrintLn(lszStr);

      return True;
    }
};

class cCliCmd_Led: public cCliCmd
{
  public:
    cCliCmd_Led():cCliCmd((const char*)"Led", (const char*)"on/off") 
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (lcParam.Instr(0, (const char8*)"on") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        mbLedActivated = True;
      }
      else if (lcParam.Instr(0, (const char8*)"off") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        mbLedActivated = False;
        lcLedRed.Off();
      }
      return True;
    }
};


void PreparePA0(bool lbPin)
{
  lcLedRed.Off();
  mcComPort.vSend();
  while (!mcComPort.mcUartDataOut.isEmpty()) {};
  HAL_SuspendTick();
  cClockInfo::Delay_ms(100);
  mcComPort.vSetWakeup(True);


  if (lbPin)
  {
    //cGpPin lcWakeupPA0(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0);

    /* Disable all used wakeup sources: WKUP pin */
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    
    /* Clear wake up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
      
    /* Nur PullUDown, also Aktivierung durch High Level, hat funktioniert
    */

    /* Enable wakeup pin WKUP2 */
    HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, PWR_GPIO_BIT_0);
    
    //PWR->CR4 |= PWR_CR4_WP1;   // Detection on low level
    
    /* PWR_PUCRx/PDCRx registers have been set in BSP_SMPS_Init(), */
    /* for SMPS as well as non-SMPS pins. It only remains to set   */
    /* APC bit:                                                    */ 
    HAL_PWREx_EnablePullUpPullDownConfig();

    PWR->CR3 |= PWR_CR3_EWUP1; // Enable Wakeup pin WKUP1 (PA0)
  }
}

class cCliCmd_LowPower: public cCliCmd
{
  public:
    cCliCmd_LowPower():cCliCmd((const char*)"lp", (const char*)"sleep/stop0/stop1") 
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (lcParam.Instr(0, (const char8*)"sleep") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(False);

        __asm("wfi");
      }
      else if (lcParam.Instr(0, (const char8*)"stop0") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(False);

        HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFI);
        MainSystemInit();
      }
      else if (lcParam.Instr(0, (const char8*)"stop1") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);

        HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
        MainSystemInit();
      }
      else if (lcParam.Instr(0, (const char8*)"stop2") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);

        HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        MainSystemInit();
      }
      else if (lcParam.Instr(0, (const char8*)"standby") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);
    
        /* Enter STANDBY mode */
        HAL_PWR_EnterSTANDBYMode();
      }
      else if (lcParam.Instr(0, (const char8*)"shutdown") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);

        HAL_PWREx_EnterSHUTDOWNMode();
      }

      mcComPort.vSetWakeup(False);
      lcCli->bPrintLn((const char8*)"continue");

      return True;
    }
};


cCliCmd_Version  mcCliCmd_Ver;
cCliCmd_Help     mcCliCmd_Help;
cCliCmd_Test     mcCliCmd_Test;
cCliCmd_SysInfo  mcCliCmd_SysInfo;
cCliCmd_Led      mcCliCmd_Led;
cCliCmd_LowPower mcCliCmd_LowPower;

cCliCmdList mcCliCmdList((cCliCmd*[]) {&mcCliCmd_Ver, &mcCliCmd_Help, &mcCliCmd_Test, &mcCliCmd_SysInfo, &mcCliCmd_Led, &mcCliCmd_LowPower}, 6);
cCli mcCli(&mcComPort.mcUartDataIn, &mcComPort.mcUartDataOut, &mcCliCmdList, 4);


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


void USART1_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcComPort.vIRQHandler();
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
}



int main(void)
{
  u32 lu32Counter = 0;

  /*
  __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_FWRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)
  __HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)*/


  MAIN_vInitSystem();

  /* Check if the system was resumed from Standby mode */ 
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    
    /* Wait that user release the User push-button */
    // BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
    // while(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_SET){} 
    mcComPort.vSend("Start from Standby\n\r");
  }
  else
  {
    mcComPort.vSend("Start\n\r");
  }
  
 
  while (1)
  {

    if (mcCli.bDoProcess())
    {
      mcComPort.vSend();
    }

    lu32Counter++;
    if (lu32Counter > 100000)
    {
      lu32Counter = 0;
      if (mbLedActivated)
      {
        lcLedRed.Toggle();
      }
    }    
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK| RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
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

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

