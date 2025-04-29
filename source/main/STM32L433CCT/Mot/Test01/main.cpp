
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB

//  USART1 -> ComPort (115200)
//  PA09  -> UsartTx
//  PA10  -> UsartRx

// DC Motor
// PB13 -> Motor A: TIM1_CH1N, BIN1 an DRV8833
// PB14 -> Motor B: TIM1_CH2N, BIN2 an DRV8833
// PA1  -> Poti ADC Channel 6
// I2C1  -> INA219
// PB06  -> I2C1 SCL
// PB07  -> I2C1 SDA

// For Debug
// mcMot1.mu8Mode
// mcOpMode
// mcMot1.mu16PwmA
// mcMot1.mu16PwmB
// mcIna219.mRegVBus.ui16Value
// mcIna219.mRegVShunt.ui16Value
// mi16InaCur
// mi16InaVolt
// miAdcVolt

// DRV8833

// IN1   IN2 |
// ----------+------------------------
//  0     0  | coast/fast decay
//  0     1  | reverse
//  1     0  | forward
//  1     1  | breake, slow decay


// IN1   IN2 |
// ----------+------------------------
// PWM    0  | Forward PWM. fast decay
// PWM    1  | Reverse PWM, slow decay
//  0    PWM | Reverse PWM, fast decay
//  1    PWM | Forward PWM, slow decay
//


__IO uint32_t TimingDelay = 0;

cClockInfo    mcClockInfo;
LED<GPIOC_BASE, 13> lcLedRed;
//                  Tx              Rx
//                  Weiß(Rx)        Grün(Tx)
//tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcComPort(115200, GPIO_AF7_USART2, 64, 64);
tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort(115200, GPIO_AF7_USART2, 64, 64);

// I2C1:
cGpPin mcI2c1_SCL(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c1_SDA(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cI2cMaster   mcI2C1(I2C1, &mcI2c1_SCL, &mcI2c1_SDA, 8);
cINA219      mcIna219(&mcI2C1, INA219_I2C_ADDRESS_CONF_0, 100);

cWorkMode        mcOpMode;

cMotTest    mcMot1(TIM1);
cAdcControl mcAdc;

// For debugging live view
i16         mi16InaCur  = 0;
i16         mi16InaVolt = 0;
i16         miAdcVolt   = 0;

class __attribute__((packed)) cLogData
{
  public:
  i16 mi16Pwm;
  u16 mu16Pos;
  i16 mi16Current;
  i16 mi16Spg;
}; // 6Byte

cLogData cLogBuf[5000]; // 35kb Reicht für 5s @1khz
cRingBufT<cLogData, u16> cRingBufLog(cLogBuf, 5000);

class cCliCmd_ShowLog : public cCliCmd
{
  u16 mu16Cnt;

  public:
  cCliCmd_ShowLog():cCliCmd((const char*)"ShowLog", (const char*)"Shows Log") {}
  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);

    char8  lszStrBuf[32];
    cStr   lszStr(lszStrBuf, 32);

    if (lbFirstCall)
    {
      mu16Cnt = 0;
      mcOpMode.vSet(cWorkMode::nStManuel);
    }

    if (mu16Cnt < 5000)
    {
      if (lcCli->isSpace(20))
      {

        lszStr.Setf("%d,%d,%d,%d,%d", mu16Cnt + 1, cLogBuf[mu16Cnt].mi16Pwm, cLogBuf[mu16Cnt].mu16Pos, cLogBuf[mu16Cnt].mi16Spg, cLogBuf[mu16Cnt].mi16Current);
        lcCli->bPrintLn(lszStr);
        mu16Cnt++;
        return False;
      }
    }
    return True;
  }
};


class cCliCmd_ClearLog : public cCliCmd
{
  public:
  cCliCmd_ClearLog():cCliCmd((const char*)"ClearLog", (const char*)"Clears Log") {}
  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);
    UNUSED(lcCli);

    cRingBufLog.reset();

    return True;
  }
};

class cCliCmd_SetMode_Manual: public cCliCmd
{
  public:

  cCliCmd_SetMode_Manual():cCliCmd((const char*)"m", (const char*)"manual mode") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lcCli->bPrintLn((const char8*)"ok");
    mcOpMode.vSet(cWorkMode::nStManuel);
    mcMot1.vSetMode(0);

    return True;
  }
};

class cCliCmd_SetMode_Auto: public cCliCmd
{
  public:

  cCliCmd_SetMode_Auto():cCliCmd((const char*)"a", (const char*)"auto mode") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    cRingBufLog.reset();
    lcCli->bPrintLn((const char8*)"ok");
    mcOpMode.vSet(cWorkMode::nStAuto);

    return True;
  }
};


class cCliCmd_SetDir: public cCliCmd
{
  public:

  cCliCmd_SetDir():cCliCmd((const char*)"dir", (const char*)"Direction") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Not in auto");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        if (lcParam.Len() != 0)
        {
          i16 li16Value = lcParam.Atoi();
          mcMot1.vSetMode((u8)li16Value);
        }
      }
      return True;
    }
};


class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"pwm", (const char*)"pmw A,B") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (lcParam.Len() != 0)
      {
        u16 lu16ValA = lcParam.Atoi();
        u16 lu16ValB = 0;

        u16 lu16Pos = lcParam.Instr(0, ',');
        if (lu16Pos != (u16)-1)
        {
          lu16ValB = lcParam.Atoi(lu16Pos + 1);
          mcMot1.vSetPwm(lu16ValA, lu16ValB);
        }
        else
        {
          mcMot1.vSetPwm(lu16ValA);
        }

      }
      else
      {
        lszStr.Setf("A: %d, B: %d", mcMot1.mu16PwmA, mcMot1.mu16PwmB);
        lcCli->bPrintLn(lszStr);
      }

      return True;
    }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"Pos") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);


      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Not in auto");
      }
      else
      {
        if (lcParam.Len() != 0)
        {
          i16 li16Value = lcParam.Atoi();
          cRingBufLog.reset();
          mcMot1.vSetPosRel(li16Value);
        }
        else
        {
          lszStr.Setf("%d, %d, %d", mcAdc.u32GetPoti_mv(), mcIna219.i32CalcVBus_uV() / 1000, mcIna219.i32CalcIShunt_uA() / 1000);
          lcCli->bPrintLn(lszStr);
        }
      }
      return True;
    }
};

class cCliCmd_Pid: public cCliCmd
{
  public:

  cCliCmd_Pid():cCliCmd((const char*)"pid", (const char*)"Kp, Ki, Kd") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);


      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Not in auto");
      }
      else
      {
        if (lcParam.Len() != 0)
        {
          u16 lu16ValKp = lcParam.Atoi();
          u16 lu16ValKi = 0;
          u16 lu16ValKd = 0;

          u16 lu16Pos = lcParam.Instr(0, ',');
          if (lu16Pos != (u16)-1)
          {
            lu16ValKi = lcParam.Atoi(lu16Pos + 1);

            lu16Pos = lcParam.Instr(lu16Pos + 1, ',');
            if (lu16Pos != (u16)-1)
            {
              lu16ValKd = lcParam.Atoi(lu16Pos + 1);
            }
          }

          mcMot1.mfKp = lu16ValKp;
          mcMot1.mfKi = lu16ValKi;
          mcMot1.mfKd = lu16ValKd;
        }
        else
        {
          lszStr.Setf("Kp: %d, Ki: %d, Kd: %d", (i16)mcMot1.mfKp, (i16)mcMot1.mfKi, (i16)mcMot1.mfKd);
          lcCli->bPrintLn(lszStr);
        }
      }
      return True;
    }
};


cCliCmd_Version mcCliCmd_Ver;
cCliCmd_Help    mcCliCmd_Help;
cCliCmd_ShowLog        mcCliCmd_ShowLog;
cCliCmd_ClearLog       mcCliCmd_ClearLog;
cCliCmd_SetMode_Manual mcCliCmd_SetMode_Manual;
cCliCmd_SetMode_Auto   mcCliCmd_SetMode_Auto;
cCliCmd_SetDir         mcCliCmd_SetDir;
cCliCmd_SetPwm         mcCliCmd_SetPwm;
cCliCmd_SetPos         mcCliCmd_SetPos;
cCliCmd_Pid            mcCliCmd_Pid;

cCliCmdList mcCliCmdList((cCliCmd*[]) {&mcCliCmd_Ver, &mcCliCmd_ShowLog, &mcCliCmd_ClearLog, &mcCliCmd_Help, &mcCliCmd_SetMode_Manual, &mcCliCmd_SetMode_Auto, &mcCliCmd_SetDir, &mcCliCmd_SetPwm, &mcCliCmd_SetPos, &mcCliCmd_Pid}, 10);
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
  lcLedRed.Toggle();
  mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcComPort.vIRQHandler();
}

void MAIN_vTick1msHp(void)
{
  mcIna219.i8ReadVBus_digit();
  mcIna219.i8ReadVShunt_digit();
  mcI2C1.bStartNext();
  /*if (!mcOpMode.IsAuto())
  {
    mcMot1.vSetActualPos(mcAdc.u32GetPoti_mv());
    mcMot1.vTick1ms();
  }*/

}

void MAIN_vTick1msLp(void)
{
  mcAdc.vMain_SmAdc(cAdcControl::nEvDummy);

  cLogData lcLogData;
  lcLogData.mi16Pwm     = mcMot1.mi16Pwm;
  lcLogData.mu16Pos     = mcAdc.u32GetPoti_mv();
  lcLogData.mi16Current = mcIna219.i32CalcIShunt_uA() / 1000;
  lcLogData.mi16Spg     = mcIna219.i32CalcVBus_uV() / 1000;

  cRingBufLog.put(lcLogData, False);

  mi16InaCur  = mcIna219.i32CalcIShunt_uA() / 1000;
  mi16InaVolt = mcIna219.i32CalcVBus_uV() / 1000;
  miAdcVolt   = mcAdc.u32GetPoti_mv();
}

void MAIN_vTick10msLp(void)
{
}


i32 mi32Pos200ms;
i32 mi32Pos300ms;

/*
// Impulsantwort
// Aus dem Stand 100% in die eine Richtung, Pause und dann 100% in die anderen Richtung
void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms  = 0;

  if (mcOpMode.IsAuto())
  {
    switch (luCnt100ms)
    {
      // 100ms  // Pause
    case 0: luCnt100ms++; mcMot1.vSetPwm(0); mcMot1.vSetMode(3); break;

      // 200ms // Voll in die eine Richtung drehen
    case 1: luCnt100ms++; mcMot1.vSetPwm(256); mcMot1.vSetMode(3); break;

      // 300ms // Pause
    case 2: luCnt100ms++; mcMot1.vSetPwm(0); mcMot1.vSetMode(3); break;

      // 400ms // Voll in die andere Richtung drehen
    case 3: luCnt100ms=0; mcMot1.vSetPwm(-256); mcMot1.vSetMode(3); break;
    }
  }
}*/

// Zick-Zack
// Aus dem Stand 100% in die eine Richtung, wie Impulsantwort ohne Pause.
//    dann doppelt so lange 100% in die anderen Richtung
//    dann doppelt so lange 100% in die anderen Richtung
void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms = 0;

  /*
  if (mcOpMode.IsAuto())
  {
    switch (luCnt100ms)
    {
      // 100ms  // Pause
    case 0: luCnt100ms++; mcMot1.vSetPwm(0); mcMot1.vSetMode(3); break;

      // 200ms // Voll in die eine Richtung drehen
    case 1: luCnt100ms++; mcMot1.vSetPwm(256); mcMot1.vSetMode(3); break;

      // 300ms // Voll in die andere Richtung drehen
    case 2: luCnt100ms++; mcMot1.vSetPwm(-256); mcMot1.vSetMode(3); break;
      // 400ms // Voll in die andere Richtung drehen
    case 3: luCnt100ms++; mcMot1.vSetPwm(-256); mcMot1.vSetMode(3); break;

      // 500ms // Voll in die eine Richtung drehen
    case 4: luCnt100ms++; mcMot1.vSetPwm(256); mcMot1.vSetMode(3); break;
      // 600ms // Voll in die eine Richtung drehen
    case 5: luCnt100ms = 2; mcMot1.vSetPwm(256); mcMot1.vSetMode(3); break;

    }
  }*/
}


/*
// Poti Messung speziell für 360° Servos
// Aus dem Stand 100% in die eine Richtung, Pause und dann 100% in die anderen Richtung
void MAIN_vTick100msLp(void)
{
  if (mcOpMode.IsAuto())
  {
    mcMot1.vSetPwm(256); mcMot1.vSetMode(3);
  }
}*/



/*void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms    = 5;

  switch (luCnt100ms)
  {
    // 100ms
    case 0: luCnt100ms++; break;

    // 200ms
    case 1: luCnt100ms++; mi32Pos200ms = mcAdc.u32GetPoti_mv(); break;

    // 300ms
    case 2: luCnt100ms++; mi32Pos300ms = mcAdc.u32GetPoti_mv(); break;

    // 400ms
    default:
    {
      luCnt100ms = 0;
      if (mcOpMode.IsAuto())
      {
        switch (mcMot1.mu8Mode)
        {
          case 0: mcMot1.vSetMode(1); break;
          case 1: mcMot1.vSetMode(2); break;
          case 2: mcMot1.vSetMode(1); break;
        }

        char8 lszStrBuf[32];
        cStr  lszStr(lszStrBuf, 32);

        i32 li32Speed = ((mi32Pos300ms - mi32Pos200ms) * 1000) / 100;

        lszStr.Setf("%d, %d, %d, %d", li32Speed, mcAdc.u32GetPoti_mv(), mcIna219.i32CalcVBus_uV() / 1000, mcIna219.i32CalcIShunt_uA() / 1000);

        mcCli.bPrintLn(lszStr);
        mcComPort.vSend();
      }
    }
  }
}*/


/*void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms    = 0;

  if (mcOpMode.IsAuto())
  {
    switch (luCnt100ms)
    {
      // 100ms
      case  0: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case  1: luCnt100ms++; mcMot1.vSetPwm(128);       mcMot1.vSetMode(3); break;
      case  2: luCnt100ms++; mcMot1.vSetPwm(128);       mcMot1.vSetMode(3); break;
      case  3: luCnt100ms++; mcMot1.vSetPwm(128);       mcMot1.vSetMode(3); break;
      case  4: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case  5: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case  6: luCnt100ms++; mcMot1.vSetPwm(0);      mcMot1.vSetMode(3); break;
      case  7: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case  8: luCnt100ms++; mcMot1.vSetPwm(-128);      mcMot1.vSetMode(3); break;
      case  9: luCnt100ms++; mcMot1.vSetPwm(-128);      mcMot1.vSetMode(3); break;
      case 10: luCnt100ms++; mcMot1.vSetPwm(-128);      mcMot1.vSetMode(3); break;
      case 11: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 12: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 13: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 14: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 15: luCnt100ms++; mcMot1.vSetPwm(192);         mcMot1.vSetMode(3); break;
      case 16: luCnt100ms++; mcMot1.vSetPwm(192);         mcMot1.vSetMode(3); break;
      case 17: luCnt100ms++; mcMot1.vSetPwm(192);       mcMot1.vSetMode(3); break;
      case 18: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 19: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 20: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 21: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 22: luCnt100ms++; mcMot1.vSetPwm(-192);       mcMot1.vSetMode(3); break;
      case 23: luCnt100ms++; mcMot1.vSetPwm(-192);      mcMot1.vSetMode(3); break;
      case 24: luCnt100ms++; mcMot1.vSetPwm(-192);      mcMot1.vSetMode(3); break;
      case 25: luCnt100ms++; mcMot1.vSetPwm(0);      mcMot1.vSetMode(3); break;
      case 26: luCnt100ms++; mcMot1.vSetPwm(0);      mcMot1.vSetMode(3); break;
      case 27: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 28: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 29: luCnt100ms++; mcMot1.vSetPwm(256);         mcMot1.vSetMode(3); break;
      case 30: luCnt100ms++; mcMot1.vSetPwm(256);         mcMot1.vSetMode(3); break;
      case 31: luCnt100ms++; mcMot1.vSetPwm(256);       mcMot1.vSetMode(3); break;
      case 32: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 33: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 34: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case 35: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 36: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case 37: luCnt100ms++; mcMot1.vSetPwm(-256);      mcMot1.vSetMode(3); break;
      case 38: luCnt100ms++; mcMot1.vSetPwm(-256);      mcMot1.vSetMode(3); break;
      case 39: luCnt100ms++; mcMot1.vSetPwm(0);      mcMot1.vSetMode(3); break;
      case 40: luCnt100ms++; mcMot1.vSetPwm(0);      mcMot1.vSetMode(3); break;
      case 41: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 42: luCnt100ms=1; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
    }
  }
}*/


/*void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms    = 0;

  if (mcOpMode.IsAuto())
  {
    switch (luCnt100ms)
    {
      // 100ms
      case  0: luCnt100ms++; mcMot1.vSetPwm(-256);         mcMot1.vSetMode(3); break;
      case  1: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case  2: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case  3: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case  4: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case  5: luCnt100ms++; mcMot1.vSetPwm(0);       mcMot1.vSetMode(3); break;
      case  6: luCnt100ms++; mcMot1.vSetPwm(10);      mcMot1.vSetMode(3); break;
      case  7: luCnt100ms++; mcMot1.vSetPwm(10);         mcMot1.vSetMode(3); break;
      case  8: luCnt100ms++; mcMot1.vSetPwm(20);      mcMot1.vSetMode(3); break;
      case  9: luCnt100ms++; mcMot1.vSetPwm(20);      mcMot1.vSetMode(3); break;
      case 10: luCnt100ms++; mcMot1.vSetPwm(30);      mcMot1.vSetMode(3); break;
      case 11: luCnt100ms++; mcMot1.vSetPwm(30);         mcMot1.vSetMode(3); break;
      case 12: luCnt100ms++; mcMot1.vSetPwm(40);         mcMot1.vSetMode(3); break;
      case 13: luCnt100ms++; mcMot1.vSetPwm(40);         mcMot1.vSetMode(3); break;
      case 14: luCnt100ms++; mcMot1.vSetPwm(50);      mcMot1.vSetMode(3); break;
      case 15: luCnt100ms++; mcMot1.vSetPwm(50);         mcMot1.vSetMode(3); break;
      case 16: luCnt100ms++; mcMot1.vSetPwm(60);      mcMot1.vSetMode(3); break;
      case 17: luCnt100ms++; mcMot1.vSetPwm(60);      mcMot1.vSetMode(3); break;
      case 18: luCnt100ms++; mcMot1.vSetPwm(70);      mcMot1.vSetMode(3); break;
      case 19: luCnt100ms++; mcMot1.vSetPwm(70);         mcMot1.vSetMode(3); break;
      case 20: luCnt100ms++; mcMot1.vSetPwm(70);         mcMot1.vSetMode(3); break;
      case 21: luCnt100ms++; mcMot1.vSetPwm(80);         mcMot1.vSetMode(3); break;
      case 22: luCnt100ms++; mcMot1.vSetPwm(80);         mcMot1.vSetMode(3); break;
      case 23: luCnt100ms++; mcMot1.vSetPwm(80);         mcMot1.vSetMode(3); break;
      case 24: luCnt100ms++; mcMot1.vSetPwm(80);      mcMot1.vSetMode(3); break;
      case 25: luCnt100ms++; mcMot1.vSetPwm(70);         mcMot1.vSetMode(3); break;
      case 26: luCnt100ms++; mcMot1.vSetPwm(70);      mcMot1.vSetMode(3); break;
      case 27: luCnt100ms++; mcMot1.vSetPwm(70);         mcMot1.vSetMode(3); break;
      case 28: luCnt100ms++; mcMot1.vSetPwm(60);      mcMot1.vSetMode(3); break;
      case 29: luCnt100ms++; mcMot1.vSetPwm(60);      mcMot1.vSetMode(3); break;
      case 30: luCnt100ms++; mcMot1.vSetPwm(50);      mcMot1.vSetMode(3); break;
      case 31: luCnt100ms++; mcMot1.vSetPwm(50);         mcMot1.vSetMode(3); break;
      case 32: luCnt100ms++; mcMot1.vSetPwm(40);         mcMot1.vSetMode(3); break;
      case 33: luCnt100ms++; mcMot1.vSetPwm(40);         mcMot1.vSetMode(3); break;
      case 34: luCnt100ms++; mcMot1.vSetPwm(30);      mcMot1.vSetMode(3); break;
      case 35: luCnt100ms++; mcMot1.vSetPwm(30);         mcMot1.vSetMode(3); break;
      case 36: luCnt100ms++; mcMot1.vSetPwm(20);      mcMot1.vSetMode(3); break;
      case 37: luCnt100ms++; mcMot1.vSetPwm(20);      mcMot1.vSetMode(3); break;
      case 38: luCnt100ms++; mcMot1.vSetPwm(10);      mcMot1.vSetMode(3); break;
      case 39: luCnt100ms++; mcMot1.vSetPwm(10);         mcMot1.vSetMode(3); break;
      case 40: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 41: luCnt100ms++; mcMot1.vSetPwm(0);         mcMot1.vSetMode(3); break;
      case 42: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case 43: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case 44: luCnt100ms++; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
      case 45: luCnt100ms=1; mcMot1.vSetPwm(-256);       mcMot1.vSetMode(3); break;
    }
  }
}*/

/*void MAIN_vTick100msLp(void)
{
  static u8 luCnt100ms    = 0;

  if (mcOpMode.IsAuto())
  {
    switch (luCnt100ms)
    {
      // 100ms
      case 0: luCnt100ms=0; mcMot1.vSetPwm(0);       mcMot1.vSetMode(0); break;
    }
  }
}*/


void MAIN_vTick1000msLp(void)
{
  lcLedRed.Toggle();
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

  /* ADC */
  mcAdc.vMain_SmAdc(cAdcControl::nEvReset);
  mcAdc.vMain_SmAdc(cAdcControl::nEvStart);

  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(50);
  }

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

}



int main(void)
{
  MAIN_vInitSystem();

  mcComPort.vSend("Start\n\r");

  while (1)
  {
    CycCall_vIdle();

    if (mcCli.bDoProcess())
    {
      mcComPort.vSend();
    }

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

