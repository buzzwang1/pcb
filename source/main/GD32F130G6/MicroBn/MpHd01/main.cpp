
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

LED<GPIOB, 1>       mcLedRed;

//.text              17924 // orig
//.text              17772 // mit Sync-Checksumme, ohne daten Checksumme
//.text              17692 // Busy-Handling entfernt
//.text              17668 // Ack-Handling entfernt
//.text              17632 // Link kann nur noch Rx oder Tx
//.text              17336 // cComDatMsg vereinfacht und statisch gemacht
//.text              17236 // cComDatMsg vereinfacht und statisch gemacht
//.text              17128 // interne Nachrichten Fehlerbehandlung entfernt
//.text              16624 // Cli History entfernt
//.text              13068 // Cli kastiert, keine Sonderzeichen, keine Texte, eine Commandlist
//.text              12068 // Ringbuf minimiert


class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    UNUSED(lcMsg);
    UNUSED(lpcBn);
    //switch (lcMsg.mu16Idx)
    //{
    //  default:
    //  break;
    //}
  }
};


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBn_MsgProcess mcBn_MsgProcess;
cBotNet mcBn(&mcMyBotNetCfg, &mcBn_MsgProcess);




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
  // USART1 TX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(25);
      //gpio_bit_reset(GPIOB, GPIO_PIN_1);
    #endif
    dma_channel_disable(DMA_CH1);
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vReset();
      cTimeStampArray::vSetTimeStamp(0);
      //gpio_bit_set(GPIOB, GPIO_PIN_1);
    #endif

    dma_channel_disable(DMA_CH2);
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(1);
    #endif
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
    u8 lu8LedEnable = 1; //(u8)mcMotor.u32GetRomConstLedEnable();
  #endif

  while (1)
  {
    if (MAIN_u32Counter_1000ms_old != MAIN_u32Counter_1000ms)
    {
      MAIN_u32Counter_1000ms_old = MAIN_u32Counter_1000ms;
      //mcMotor.vTick1ms();
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

