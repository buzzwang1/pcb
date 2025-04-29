
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB

//                    Voltage Reg.                        DS      real
//             Clock   MP      LP    PC13 PB8 PB9  DAC    uA      uA
// Stop0        Off    On      On     On  On  On   On    110      300
// Stop1        Off    Off     On     On  On  On   On      5      200
// Stop2        Off    Off     On     On² On  On   Off     2      300
// Standby      Off    Off     Off    On² On³ On³  Off    <1       63
// Shutdown     Off    Off     Off    On² On³ On³  Off   <<1       63

//²: no source, max 3ma Sink
//³: Only pull up and pull down: gemessen 2,07V an 120k bei 2,71 VCC und interner pullup (40k) => 2,7V * 120/160 = 2,02V

__IO uint32_t TimingDelay = 0;

LED<GPIOB_BASE, 9> lcLedRed;
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

class cCliCmd_SysClock: public cCliCmd
{
  public:
    cCliCmd_SysClock():cCliCmd((const char*)"Clock", (const char*)"System Time")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lcCli->bPrintLn((const char8*)"Time:");
      lszStr.Setf((const char8*)"%02d:%02d:%02d", __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC))); lcCli->bPrintLn(lszStr);


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

void EnableRTC(void)
{
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();  
}

void ConfigureRTC(void)
{
  EnableRTC();

  u8 lu8t = 10;

  // --------------- First try LSE ---------------

  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
  // RTC_CLOCK_SOURCE_LSE
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();
  LL_RCC_LSE_Enable();


  while ((LL_RCC_LSE_IsReady() != 1) && (lu8t > 0))
  {
    cClockInfo::Delay_us(10);
    lu8t--;
  }

  if (LL_RCC_LSE_IsReady())
  {
    //mbLse = True;
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();

    LL_RTC_InitTypeDef lstRtc;
    LL_RTC_StructInit(&lstRtc);

    lstRtc.HourFormat = LL_RTC_HOURFORMAT_24HOUR;

    // 32768 = 256*128
    // ck_apre = LSEFreq / (ASYNC prediv + 1) = 256Hz
    // ck_apre = 32768Hz / (127 + 1)          = 256Hz
    lstRtc.AsynchPrescaler = ((uint32_t)0x7F);

    // ck_spre = ck_apre / (SYNC prediv + 1) = 1 Hz
    // ck_spre = 256Hz   / (255 + 1)         = 1 Hz
    lstRtc.SynchPrescaler = ((uint32_t)0x00FF);
    LL_RTC_Init(RTC, &lstRtc);
  }
  else
  {
    //mbLse = False;
    LL_RCC_LSE_Disable();

    LL_RCC_LSI_Enable();
    while (LL_RCC_LSI_IsReady() != 1)
    {
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();

    LL_RTC_InitTypeDef lstRtc;
    LL_RTC_StructInit(&lstRtc);

    lstRtc.HourFormat = LL_RTC_HOURFORMAT_24HOUR;


    // 32768 = 256*125
    // ck_apre = LSEFreq / (ASYNC prediv + 1) = 256Hz
    // ck_apre = 32768Hz / (124 + 1)          = 256Hz
    lstRtc.AsynchPrescaler = ((uint32_t)0x7C);

    // ck_spre = ck_apre / (SYNC prediv + 1) = 1 Hz
    // ck_spre = 256Hz   / (255 + 1)         = 1 Hz
    lstRtc.SynchPrescaler = ((uint32_t)0x00FF);
    LL_RTC_Init(RTC, &lstRtc);
  }


  LL_RTC_DateTypeDef lstDate;
  LL_RTC_DATE_StructInit(&lstDate);

  lstDate.WeekDay = 6;
  lstDate.Day     = 27;
  lstDate.Month   = 2;
  lstDate.Year    = 21;

  LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &lstDate);

  LL_RTC_TimeTypeDef lstTime;
  LL_RTC_TIME_StructInit(&lstTime);

  lstTime.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24;
  lstTime.Hours      = 12;
  lstTime.Minutes    = 0;
  lstTime.Seconds    = 0;
  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &lstTime);
}


void vPrepareDac()
{
  __HAL_RCC_DAC1_CLK_ENABLE();
  cGpPin mDac(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);

  LL_DAC_InitTypeDef DAC_InitStructure;

  /* DAC channel1 Configuration */
  LL_DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;
  LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_InitStructure);
  LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, (4096 * 2000) / 2700);
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
}


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



inline void vEnableIrqRtcWut()
{
  __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();
}

inline void vDisableIrqRtcWut()
{
  __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT();
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_20); // 20 = RTC wakeup timer
}

void RTC_WKUP_IRQHandler(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_20);
  vDisableIrqRtcWut();
}



inline void vEnableIrqPa0()
{
  EXTI->IMR1 |= (1 << 0);
}

inline void vDisableIrqPa0()
{
  EXTI->IMR1 &= ~(1 << 0);
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_0);
}


void EXTI0_IRQHandler(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0);
  vDisableIrqPa0();
}

void PreparePA0_Exti()
{
  lcLedRed.Off();
  mcComPort.vSend();
  while (!mcComPort.mcUartDataOut.isEmpty()) {};
  HAL_SuspendTick();
  cClockInfo::Delay_ms(100);
  mcComPort.vSetWakeup(True);

  {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0);}

  __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0);

  // Enable clock for SYSCFG
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  // Tell system that you will use PA0 for EXTI_Line0
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);

  //EXTI init
  LL_EXTI_InitTypeDef EXTI_InitStruct;
  // PA0 is connected to EXTI_Line0
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  //NVIC init
  // Add IRQ vector to NVIC
  HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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
        PreparePA0_Exti();

        //EXTI init
        LL_EXTI_InitTypeDef EXTI_InitStruct;
        // PA0 is connected to EXTI_Line0
        EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_20;
        EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
        EXTI_InitStruct.LineCommand = ENABLE;
        EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
        EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
        LL_EXTI_Init(&EXTI_InitStruct);

        //EXTI->SWIER1 |= 1 << 20; // Software interrupt on EXTI20

        //NVIC init
        // Add IRQ vector to NVIC
        HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

        ConfigureRTC();

        LL_RTC_DisableWriteProtection(RTC);

        /* Disable wake up timer to modify it */
        LL_RTC_WAKEUP_Disable(RTC);

        while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1) {}

        LL_RTC_WAKEUP_SetAutoReload(RTC, 4);
        LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

        LL_RTC_WAKEUP_Enable(RTC);
        LL_RTC_EnableIT_WUT(RTC);

        /* Enable RTC registers write protection */
        LL_RTC_EnableWriteProtection(RTC);

        LL_RTC_ClearFlag_WUT(RTC);

        //__HAL_RCC_RTCAPB_CLK_SLEEP_ENABLE();
        //__HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();


        __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
        HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
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
      else if (lcParam.Instr(0, (const char8*)"rtcstandby") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);

        LL_RTC_DisableWriteProtection(RTC);

        /* Disable wake up timer to modify it */
        LL_RTC_WAKEUP_Disable(RTC);

        while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1) {}

        LL_RTC_WAKEUP_SetAutoReload(RTC, 10);
        LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

        LL_RTC_WAKEUP_Enable(RTC);
        LL_RTC_EnableIT_WUT(RTC);

        /* Enable RTC registers write protection */
        LL_RTC_EnableWriteProtection(RTC);

        LL_RTC_ClearFlag_WUT(RTC);

        /* Check that PWR Internal Wake-up is enabled */
        //if (LL_PWR_IsEnabledInternWU() == 0)
        //{
          /* Need to enable the Internal Wake-up line */
          //LL_PWR_EnableInternWU();
        //}

        /* Enter STANDBY mode */
        HAL_PWR_EnterSTANDBYMode();
      }
      else if (lcParam.Instr(0, (const char8*)"shutdown") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        PreparePA0(True);

        HAL_PWREx_EnterSHUTDOWNMode();
      }
      else if (lcParam.Instr(0, (const char8*)"sd1") == 0)
      {
        lcCli->bPrintLn((const char8*)"sd1 ok");

        //mcComPort.vSend("Enable RTC\n\r");

        PreparePA0(True);

        
        //ConfigureRTC();
        //EnableRTC();
        //LL_RTC_DisableWriteProtection(RTC);
        //PWR->CR3 |= PWR_CR3_APC;
        //PWR->PUCRB |= 1 << 8;
        //PWR->PDCRB |= 1 << 9;

        //LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
        //LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
        ////LL_RTC_OUTPUTPOLARITY_PIN_HIGH, LL_RTC_OUTPUTPOLARITY_PIN_LOW
        //LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high 

        HAL_PWREx_EnterSHUTDOWNMode();
      }
      else if (lcParam.Instr(0, (const char8*)"sd2") == 0)
      {
        lcCli->bPrintLn((const char8*)"sd1 ok");
        mcComPort.vSend("Enable RTC\n\r");

        PreparePA0(True);

        ConfigureRTC();

        LL_RTC_DisableWriteProtection(RTC);
        /* Disable wake up timer to modify it */
        LL_RTC_WAKEUP_Disable(RTC);
        while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1) {}
        LL_RTC_WAKEUP_SetAutoReload(RTC, 10);
        LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
        LL_RTC_WAKEUP_Enable(RTC);
        LL_RTC_EnableIT_WUT(RTC);
        /* Enable RTC registers write protection */
        LL_RTC_EnableWriteProtection(RTC);

        
        LL_RTC_ClearFlag_WUT(RTC);

        ////LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
        ////LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
        //////LL_RTC_OUTPUTPOLARITY_PIN_HIGH, LL_RTC_OUTPUTPOLARITY_PIN_LOW
        ////LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high 

        HAL_PWREx_EnterSHUTDOWNMode();
      }
      if (lcParam.Instr(0, (const char8*)"pc13off") == 0)
      {
        lcCli->bPrintLn((const char8*)"pc13off");
        PreparePA0(False);

        //LL_RTC_OUTPUTPOLARITY_PIN_HIGH, LL_RTC_OUTPUTPOLARITY_PIN_LOW
        LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_HIGH);


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
cCliCmd_SysClock mcCliCmd_SysClock;
cCliCmd_Led      mcCliCmd_Led;
cCliCmd_LowPower mcCliCmd_LowPower;

cCliCmdList mcCliCmdList((cCliCmd*[]) {&mcCliCmd_Ver, &mcCliCmd_Help, &mcCliCmd_Test, &mcCliCmd_SysInfo, &mcCliCmd_SysClock, &mcCliCmd_Led, &mcCliCmd_LowPower}, 7);
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
  if (mbLedActivated) lcLedRed.Toggle();
  mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  if (mbLedActivated) lcLedRed.Toggle();
  mcComPort.vIRQHandler();
}


void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  vPrepareDac();

  cGpPin mPin1(GPIOB_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  cGpPin mPin2(GPIOB_BASE, 9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);



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
    mcComPort.vSend("Start");
    mcComPort.vSend("Configure RTC\n\r");
    if (!LL_RTC_IsActiveFlag_WUT(RTC))
    {
      ConfigureRTC();
    }
  }

  if (LL_RTC_IsActiveFlag_WUT(RTC))
  {
    mcComPort.vSend("From RTC WUT\n\r");
    /* Reset RTC Internal Wake up flag */
    EnableRTC();
    LL_RTC_DisableWriteProtection(RTC);

    LL_RTC_ClearFlag_WUT(RTC);
    /* Disable wake up timer to modify it */
    LL_RTC_WAKEUP_Disable(RTC);
    LL_RTC_DisableIT_WUT(RTC);

    LL_RTC_EnableWriteProtection(RTC);
  }

  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
  LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
  //LL_RTC_OUTPUTPOLARITY_PIN_HIGH, LL_RTC_OUTPUTPOLARITY_PIN_LOW
  LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high 


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
  __HAL_RCC_RTCAPB_CLK_ENABLE();

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

