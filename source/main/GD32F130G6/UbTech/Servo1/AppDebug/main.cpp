#include "main.h"

// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, 8 Mhz (72 MHz max)
// Rom 32KB
//   - FBL  12k: 0x0800 0000 - 0x0800 2FFF
//   - APP  19k: 0x0800 3000 - 0x0800 7BFF
//   - DATA  1k: 0x0800 7C00 - 0x0800 7FFF
// Ram  4KB
// TIM1(32Bit)
// TIM0, TIM2, TIM13, TIM15, TIM16

// TIM0: Motor PWM(Ch0) und ADC (CH1),
// TIM2: System Tick
// TIM13:
// TIM15: optional (TimeStampArray)
// TIM16: Uart Timeout


// Servo:
//
// PB1: LED:          Tim2_Ch3(AF1); Tim13_Ch0(AF0); Tim0_Ch2_On(AF2)
//
// PA9: Usart Tx:     USART0_TX(AF1)
//
// PA8: Mot Enable:   Tim0_Ch0(AF2)
// PA7: Poti:         ADC_IN7
// PA6: Mot Fault:    -
// PA5: Mot Break:    Tim1_Ch0(AF2)
// PA4: Mot Sleep:    -
// PA3: Mot Phase:    Tim1_Ch3(AF2), Tim14_Ch1(AF0)
// PA2: Mot Strom:    ADC_IN2
// PA1: 7V:           ADC_IN1
// PA0: Mot Spannung: ADC_IN0


//#define PCB_PROJECTCFG_Test

#ifdef PCB_PROJECTCFG_Test
  LED<GPIOA, 10>    mcPA10;
#endif


void vLedInit()
{
  // PA0
  rcu_periph_clock_enable(RCU_GPIOB);
  GPIO_CTL(GPIOB) |= 1<<2;  // Output Mode
}

void vLedOff()
{
  gpio_bit_reset(GPIOB, GPIO_PIN_1);
}

void vLedOn()
{
  gpio_bit_set(GPIOB, GPIO_PIN_1);
}

void vLedToggle()
{
  if (gpio_input_bit_get(GPIOB, GPIO_PIN_1))
  {
    vLedOff();
  }
  else
  {
    vLedOn();
  }
}

cServo1_Applikation mcMotor;


class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:

  void vSendStatus(cBotNet* lpcBn, u16 lu16DestAdress)
  {
    cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgStatusMsg, 24, lpcBn->mcAdr.Get(), lu16DestAdress, 0x90);

    i16* lpi16Data;
    lpi16Data = (i16*)lcMsgStatusMsg.mcPayload.mpu8Data;

                                                       // Idx
    *lpi16Data++ = (i16)mcMotor.i16GetPosDegree();     // 0, 1
    *lpi16Data++ = (i16)mcMotor.u16GetSupplyVoltage(); // 2, 3
    *lpi16Data++ = (i16)mcMotor.i16GetCurrent();       // 4, 5
    *lpi16Data++ = (i16)mcMotor.i16GetExtTemp();       // 6, 7
    *(u8*)lpi16Data = (u8)mcMotor.mStatus.u8Status;       // 8
    lcMsgStatusMsg.mcPayload.muiLen = 9;


    lcMsgStatusMsg.vEncode();
    lpcBn->bSendMsg(&lcMsgStatusMsg);
  }

  bool bMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    switch (lcMsg.mu16Idx)
    {
      case 80: // i16 Sollpos
        {
          i16* lpi16PosSoll;
          //i16* lpi16Strom;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            lpi16PosSoll = (i16*)lcMsg.mcPayload.mpu8Data;
            mcMotor.vSetMotMode_ServoPos();
            mcMotor.vSetPosDegree(*lpi16PosSoll);

            if (*lpi16PosSoll <= 0)  *lpi16PosSoll-=*lpi16PosSoll;
            if (*lpi16PosSoll > 360)
            {
              mcMotor.vSetMotDisable();
            }
            else
            {
              mcMotor.vSetMotEnable();
            }
            vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
          }
        }
        break;
      case 81: // i16 Sollpos + i16Strombegrenzung
      {
        i16* lpi16Payload;
        i16  lpi16PosSoll;
        i16  lpi16PosCur;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          mcMotor.vSetMotMode_ServoPosCur();
          lpi16Payload = (i16*)lcMsg.mcPayload.mpu8Data;
          lpi16PosSoll = *lpi16Payload++;
          lpi16PosCur  = *lpi16Payload;
          mcMotor.vSetPosDegree(lpi16PosSoll);
          mcMotor.vSetCur_mA(lpi16PosCur);

          if (lpi16PosSoll <= 0)  lpi16PosSoll -= lpi16PosSoll;
          if ((lpi16PosSoll > 360) && (lpi16PosCur > 4))
          {
            mcMotor.vSetMotDisable();
          }
          else
          {
            mcMotor.vSetMotEnable();
          }
          vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
        }
      }
      break;
      case 84: // i16 Sollpwm
        {
          i16* lpi16PwmSoll;
          //i16* lpi16Strom;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            lpi16PwmSoll = (i16*)lcMsg.mcPayload.mpu8Data;
            mcMotor.vSetMotMode_MotorPwm();
            mcMotor.vSetSpeed(*lpi16PwmSoll);
            if (*lpi16PwmSoll != 0)
            {
              mcMotor.vSetMotEnable();
            }
            else
            {
              mcMotor.vSetMotDisable();
            }
            vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
          }
        }
        break;
      case 85: // Motor mit i16Strombegrenzung
      {
        i16* lpi16CurSoll;
        //i16* lpi16Strom;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          lpi16CurSoll = (i16*)lcMsg.mcPayload.mpu8Data;
          mcMotor.vSetMotMode_MotorCur();
          mcMotor.vSetCur_mA(*lpi16CurSoll);
          if (*lpi16CurSoll != 0)
          {
            mcMotor.vSetMotEnable();
          }
          else
          {
            mcMotor.vSetMotDisable();
          }
          vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
        }
      }
      break;
      case 90: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
          i32* lpi32Para;
          if (lcMsg.mcPayload.muiLen >= 3*4)
          {
            lpi32Para = (i32*)lcMsg.mcPayload.mpu8Data;

            mcMotor.mcMyPidPos.mFpKp.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidPos.mFpKi.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidPos.mFpKd.vSetFrac(*lpi32Para++);
          }
        }
        break;
      case 95: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
          i32* lpi32Para;
          if (lcMsg.mcPayload.muiLen >= 3*4)
          {
            lpi32Para = (i32*)lcMsg.mcPayload.mpu8Data;

            mcMotor.mcMyPidCur.mFpKp.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidCur.mFpKi.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidCur.mFpKd.vSetFrac(*lpi32Para++);
          }
        }
      break;
      default:
      break;
    }
    return True;
  }

};


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cBn_MsgProcess mcBn_MsgProcess;


cUartMpHdSlave mcUartMpHdU0;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);
cBotNet_UpLinkUsartMpHd_RMsg mcUpLink(&mcUartMpHdU0);

cBotNet mcBn(&mcMyBotNetCfg, &mcBn_MsgProcess);


class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (lcParam.Len() != 0)
      {
        i16 li16Val = lcParam.Atoi();
        mcMotor.vSetMotMode_MotorPwm();
        mcMotor.vSetSpeed(li16Val);
        if (li16Val != 0) mcMotor.vSetMotEnable();
                    else  mcMotor.vSetMotDisable();
      }
      else
      {
        lszStr.Setf("A: %d", mcMotor.mSpeed_Soll);
        lcCli->bPrintLn(lszStr);
      }
      return True;
    }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (lcParam.Len() != 0)
      {
        i16 li16Val = lcParam.Atoi();
        mcMotor.vSetMotMode_ServoPos();
        mcMotor.vSetPosDegree(li16Val);
        mcMotor.vSetMotEnable();
      }
      else
      {
        lszStr.Setf("A: %d", mcMotor.i16GetPosDegree());
        lcCli->bPrintLn(lszStr);
      }
      return True;
    }
};

class cCliCmd_Status: public cCliCmd
{
  public:
    cCliCmd_Status():cCliCmd((const char*)"?", (const char*)"status")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lszStr.Setf((const char8*)"P: %d", mcMotor.i16GetPosFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"U: %d", mcMotor.i16GetVoltFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I: %d", mcMotor.i16GetCurrentFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Ti: %d", mcMotor.i16GetIntTemp());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Te: %d", mcMotor.i16GetTempFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"PF: %d\r\n", mcMotor.mPowerFailCounter);
      lcCli->bPrintLn(lszStr);

      for (u8 lu8t=0; lu8t < mcMotor.nChnCount; lu8t++)
      {
        lszStr.Setf((const char8*)"%d: %d", lu8t + 1, mcMotor.mAdcResult[lu8t]);
        lcCli->bPrintLn(lszStr);
      }

      return True;
    }
};


class cBotNetMotCli
{
  public:
  cCliCmd_SetPwm  mcCliCmd_SetPwm;
  cCliCmd_SetPos  mcCliCmd_SetPos;
  cCliCmd_Status  mcCliCmd_Status;

  cBotNetMotCli()
  {
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPwm);
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPos);
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
  }
};

cBotNetMotCli mcBnMotCli;


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


/*static volatile u32 mu32SysTick_Delay;

void vSysTickInit(void)
{
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    while (1);
  }
  NVIC_SetPriority(SysTick_IRQn, 0x00);
}

void vDelay_1ms(u32 count)
{
  mu32SysTick_Delay = count;
  while (0 != mu32SysTick_Delay);
}

void vDelay_Decrement(void)
{
  if (0 != mu32SysTick_Delay)
  {
    mu32SysTick_Delay--;
  }
}*/


void SysTick_Handler(void)
{
  //vDelay_Decrement();
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


void DMA_Channel1_2_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
    vDtwStart();
  #endif
  // USART1 TX
  // Ender der Übertragung wird durch  U0.TC interrupt erledigt
  // Tx DMA Interrupt wird nicht benötigt
  // if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  // {
  //  #ifdef TEST_BnLinkUsartMpHd
  //    cTimeStampArray::vSetTimeStamp(25);
  //    //gpio_bit_reset(GPIOB, GPIO_PIN_1);
  //  #endif
  //   dma_channel_disable(DMA_CH1);
  //   DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
  //   mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  // }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vReset();
      cTimeStampArray::vSetTimeStamp(0);
      //gpio_bit_set(GPIOB, GPIO_PIN_1);
    #endif

    DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(1);
    #endif

  }
  #ifdef PCB_PROJECTCFG_Test
    __asm volatile ("nop");
    mcPA10.vSet0();
  #endif
}

void USART0_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (USART_CTL0(USART0) & USART_CTL0_UEN)
  {
    if (USART_STAT(USART0) & USART_STAT_TC)
    {
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(26);
      #endif

      USART_INTC(USART0) = USART_STAT_TC;
      mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(27);
      #endif
    }

    if (USART_STAT(USART0) & 0xF)
    {
      USART_INTC(USART0) = USART_STAT_ORERR;
      mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    //usart_deinit(USART0);
    mcUartMpHdU0.vInitHw();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}

void TIMER16_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (timer_interrupt_flag_get(TIMER16, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER16, TIMER_INT_FLAG_UP);
    timer_disable(TIMER16);
    mcUartMpHdU0.TIM_EV_IRQHandler();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}


u32 MAIN_u32Counter_10ms;
u32 MAIN_u32Counter_1000ms;
u32 MAIN_u32Counter_1000ms_old;

void TIMER2_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_10ms++;
    MAIN_u32Counter_1000ms++;
    mcBn.vTickHp1ms();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}


//static void system_clock_16m_irc8m(void)
//{
//  /* AHB = SYSCLK */
//  //RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
//  /* APB2 = AHB/2 */
//  //RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
//  /* APB1 = AHB/2 */
//  //RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;
//  /* PLL = (IRC8M/2) *  4 = 16 MHz */
//  RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4 | RCU_CFG0_PLLPREDV);
//  RCU_CFG1 &= ~(RCU_CFG1_PLLPRESEL | RCU_CFG1_PLLMF5 | RCU_CFG1_PREDV);
//  RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2 | (RCU_PLL_MUL4 & (~RCU_CFG1_PLLMF5)));
//  RCU_CFG1 |= (RCU_PLL_MUL18 & RCU_CFG1_PLLMF5);
//  /* enable PLL */
//  RCU_CTL0 |= RCU_CTL0_PLLEN;
//
//  /* wait until PLL is stable */
//  while (0U == (RCU_CTL0 & RCU_CTL0_PLLSTB)) {}
//
//  /* select PLL as system clock */
//  RCU_CFG0 &= ~RCU_CFG0_SCS;
//  RCU_CFG0 |= RCU_CKSYSSRC_PLL;
//
//  /* wait until PLL is selected as system clock */
//  while (0U == (RCU_CFG0 & RCU_SCSS_PLL)) {}
//}


void vInitTim2()
{
  rcu_periph_clock_enable(RCU_TIMER2);

  MAIN_u32Counter_10ms       = 0;
  MAIN_u32Counter_1000ms     = 0;
  MAIN_u32Counter_1000ms_old = 0;


  /*timer_parameter_struct timer_initpara;
  // TIMER configuration
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);

  // auto-reload preload enable
  timer_auto_reload_shadow_enable(TIMER2);
  // auto-reload preload enable
  timer_enable(TIMER2);*/


  // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
  // Braucht aber so 90Byte weniger
  TIMER_DMAINTEN(TIMER2) = 1;
  TIMER_PSC(TIMER2)      = 7;    //  15 für 16Mhz und 7 für 8Mhz
  TIMER_CAR(TIMER2)      = 0x3E7;
  TIMER_CTL0(TIMER2)     = 0x081;

  /*NVIC config*/
  nvic_irq_enable(TIMER2_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

  vLedInit();

  // Add Uplink
  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(0, 0xE000, 0);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  mcBn.vStreamPortConnect(1, 0xE000, 1);

  /*cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg, 16, 0x1100, 0xE000, 0x50);

  u8 t;

  for (t=0;t<16;t++)
  {
    lcMsgTestMsg.mcPayload[t] = t;
  }
  lcMsgTestMsg.vEncode();
  mcBn.mcUpLink.mStatus.IsOnline = 1;
  mcBn.bSendMsg(&lcMsgTestMsg);*/

  vInitTim2();
}


int main(void)
{
  fwdgt_counter_reload();
  MAIN_vInitSystem();

  #ifndef TEST_BnLinkUsartMpHd
    u8 lu8LedEnable = mcMotor.u8GetLedEnable();
  #endif

  while (1)
  {
    if (MAIN_u32Counter_1000ms_old != MAIN_u32Counter_1000ms)
    {
      MAIN_u32Counter_1000ms_old = MAIN_u32Counter_1000ms;
      mcMotor.vTick1ms();
    }

    if (MAIN_u32Counter_10ms >= 10)
    {
      mcBn.vTickLp10ms();
      MAIN_u32Counter_10ms = 0;
    }

    #ifndef TEST_BnLinkUsartMpHd
      // Im Testmodues wird die Led als Testpin benutzt
      if (MAIN_u32Counter_1000ms >= 1000)
      {
        //static i16 li16Pos = 30;
        //mcMotor.vSetPosDegree(li16Pos);
        //li16Pos = -li16Pos;

        if (lu8LedEnable) vLedToggle();
                     else vLedOff();
        MAIN_u32Counter_1000ms = 0;
      }
    #endif
    mcBn.vProcess10ms();

    fwdgt_counter_reload();

    __asm("wfi");
  }
}



void MainSystemInit(void)
{
  /* FPU settings */
  #if (__FPU_PRESENT == 1U) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
  #endif

  /* configure system clock */
  //system_clock_16m_irc8m();
  #ifdef  VECT_TAB_OFFSET
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, VECT_TAB_OFFSET);
  #endif
}
