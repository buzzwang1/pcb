
#include "main.h"



// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, (72 MHz max)
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



// 128, 2000 0660       : UpLink_ComBuf.Rx
// 128, 2000 06E8       : UpLink_ComBuf.Tx
//   2, 2000 0770:      : UpLink Sync.Tx
//   3, 2000 0778:   +8 : UpLink Sync.Rx
//  65, 2000 0788:  +16 : UpLink Data.Tx
//  65, 2000 07d0:  +72 : UpLink Data.Rx
//   1, 2000 0820:  +72 : UpLink Ack.Tx
//   1, 2000 0828:   +8 : UpLink Ack.Rx
// 128, 2000 0838       : StreamSys_ComBuf.Rx
// 128, 2000 08C0       : StreamSys_ComBuf.Tx
//  64, 2000 0948       : StreamSys_Msg.Rx
//  64, 2000 0990       : StreamSys_Msg.Tx
//                      : cCliHistory
//                      : cCliHistoryData
//                      : cCliCmdList
//  64, 2000 0A48:      : cBotnetMsgRx
//  64, 2000 0A90:      : cBotnetMsgTx
//  --  2000 0AD8:      : -----------
//
// 1144 Byte
//
//
// Sram:  2048
// Heap:  1536
// Stack:  512
//
//
//
// 2000 0000
// 2000 0658 Heapbase
// 2000 0D00 Stackbase
// 2000 0FFF


LED<GPIOB, 1>       mcLedRed;
//cServo1_Applikation mcMotor;


class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:

  void vSendStatus(cBotNet* lpcBn, u16 lu16DestAdress)
  {
    cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgStatusMsg, 24, lu16DestAdress, lpcBn->mcAdr.Get(), 0x90);

    i16* lpi16Data;
    lpi16Data = (i16*)lcMsgStatusMsg.mcPayload.mpu8Data;

    *lpi16Data++ = 0;
    *lpi16Data++ = 0;
    *lpi16Data++ = 0;
    *lpi16Data   = 0;
    lpi16Data[8] = 0;


    lcMsgStatusMsg.vEncode();
    lpcBn->bSendMsg(&lcMsgStatusMsg);
  }

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    UNUSED(lpcBn);
    switch (lcMsg.mu16Idx)
    {
      case 80: // i16 Sollpos + i16Strombegrenzung (optional)
        {
        }
        break;
      case 81: // i16 Sollpwm + i16Strombegrenzung (optional)
        {
        }
        break;
      case 82: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
        }
      break;
      default:
      break;
    }
  }
};


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cBn_MsgProcess mcBn_MsgProcess;


cBotNet mcBn(&mcMyBotNetCfg, &mcBn_MsgProcess);




class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lcCli);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);


      return True;
    }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lcCli);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);


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
      UNUSED(lcParam);
      UNUSED(lcCli);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);



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
  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vDtwStart();
      gpio_bit_set(GPIOB, GPIO_PIN_1);
      cTimeStampArray::vReset();
      cTimeStampArray::vSetTimeStamp(0);
    #endif

    DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

    #ifdef TEST_BnLinkUsartMpHd
      gpio_bit_reset(GPIOB, GPIO_PIN_1);
      cTimeStampArray::vDtwStop();
      cTimeStampArray::vSetTimeStamp(1);
    #endif
  }
  else
  // USART1 TX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(25);
    #endif
    DMA_CHCTL(DMA_CH1) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  }
}

void USART0_IRQHandler(void)
{
  if (USART_CTL0(USART0) & USART_CTL0_UEN)
  {
    if (USART_STAT(USART0) & USART_STAT_TC)
    {
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(26);
      #endif

      USART_INTC(USART0) = USART_STAT_TC;
      mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(27);
      #endif
    }

    if (USART_STAT(USART0) & 0xF)
    {
      USART_INTC(USART0) = USART_STAT_ORERR;
      mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    //usart_deinit(USART0);
    mcBn.mcUartMpHdU0.vInitHw();
  }
}

void TIMER16_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER16, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER16, TIMER_INT_FLAG_UP);
    timer_disable(TIMER16);
    mcBn.mcUartMpHdU0.TIM_EV_IRQHandler();
  }
}


u32 MAIN_u32Counter_10ms;
u32 MAIN_u32Counter_1000ms;
u32 MAIN_u32Counter_1000ms_old;

void TIMER2_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_10ms++;
    MAIN_u32Counter_1000ms++;
    mcBn.vTickHp1ms();
  }
}


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
  TIMER_PSC(TIMER2)      = 7;
  TIMER_CAR(TIMER2)      = 0x3E7;
  TIMER_CTL0(TIMER2)     = 0x081;

  /*NVIC config*/
  nvic_irq_enable(TIMER2_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

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
    u8 lu8LedEnable = (u8)mcMotor.u32GetRomConstLedEnable();
  #endif

  while (1)
  {
    if (MAIN_u32Counter_1000ms_old != MAIN_u32Counter_1000ms)
    {
      MAIN_u32Counter_1000ms_old = MAIN_u32Counter_1000ms;
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

        if (lu8LedEnable) mcLedRed.Toggle();
                     else mcLedRed.Off();
        MAIN_u32Counter_1000ms = 0;
      }
    #endif

    fwdgt_counter_reload();

    __asm("wfi");
  }
}

