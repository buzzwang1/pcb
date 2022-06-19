
#include "main.h"



// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, (72 MHz max)
// Rom 32KB
// Ram  4KB
// TIM1(32Bit)
// TIM0, TIM2, TIM13, TIM15, TIM16


// Servo:
//
// PB0: LED:          Tim2_Ch2(AF1); Tim0_Ch1_On(AF2)
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



// Heap start: 0x20000440
// Heap end:   0x20000b48 (bei cBotNet_UpLinkComBufSize = 8)
// -----------------------
// Heap sum   1800 Byte
// 
// Heap start: 0x20000440
// Heap end:   0x20000948 (bei cBotNet_UpLinkComBufSize = 4)
// -----------------------
// Heap sum   1288 Byte
// 
// 
// Sram:  2048 (enthält 1024 IAP Buffer)
// Heap:  1536 (Heap)
// Stack:  512 (Stack)
//
//
//
// 2000 0000
// 2000 0658 Heapbase
// 2000 0D00 Stackbase
// 2000 0FFF


LED<GPIOB, 1>       mcLedRed;


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cBotNet mcBn(&mcMyBotNetCfg);


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


/*
static volatile u32 mu32SysTick_Delay;

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
    dma_channel_disable(DMA_CH1);
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    dma_channel_disable(DMA_CH2);
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
  }
}

void USART0_IRQHandler(void)
{
  if (USART_CTL0(USART0) & USART_CTL0_UEN)
  {
    if (USART_STAT(USART0) & USART_STAT_TC)
    {
      USART_INTC(USART0) = USART_STAT_TC;
      mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
    }

    if (USART_STAT(USART0) & 0xF)
    {
      mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    //usart_deinit(USART0);
    mcBn.mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErUnknown);
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
u32 MAIN_u32Counter_100ms;

void TIMER2_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_10ms++;
    MAIN_u32Counter_100ms++;
    mcBn.vTickHp1ms();
  }
}


void vInitTim2()
{
  rcu_periph_clock_enable(RCU_TIMER2);

  MAIN_u32Counter_10ms = 0;
  MAIN_u32Counter_100ms = 0;


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
  TIMER_PSC(TIMER2) = 7;
  TIMER_CAR(TIMER2) = 0x3E7;
  TIMER_CTL0(TIMER2) = 0x081;

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

  vInitTim2();
}


void MAIN_vGotoApp(void *p)
{
  /*unsigned rst;
  unsigned stk;
  unsigned vtor;
  rst = *(unsigned *)((u8*)p + 4);
  stk = *(unsigned *)p;
  vtor = (unsigned)p & 0x1FFFFF80;
  vtor = (vtor & ~0xffe00000) | (rst & 0xffe0000);
  SCB->VTOR = vtor;*/

  //Ersten 4Byte = Stack
  //Zweite 4Byte = Reset Anfang vom Startp Code.

  unsigned stk;
  unsigned rst;
  stk = *(unsigned *)p;
  rst = *(unsigned *)((u8*)p + 4);
  SCB->VTOR = (u32)p;

  __set_MSP(stk);
  __asm("bx %0" : : "r" (rst));
}


int main(void)
{
  // 12k = 12.288Byte

  if (!RomConst_IsValid())
  {
    // RomConst Error
    while (1) {}
  }

  volatile u32 mu32ResetReason = RCU_RSTSCK;
  // 29 FWDGTRSTF:  Set by hardware when a Free Watchdog timer generated. Reset by writing 1 to the RSTFC
  // 28 SWRSTF:     Set by hardware when a software reset generated. Reset by writing 1 to the RSTFC bit.
  // 27 PORRSTF:    Set by hardware when a Power reset generated. Reset by writing 1 to the RSTFC b

  rcu_all_reset_flag_clear();

  fwdgt_counter_reload();
  InAppProg_Platform_vStartWdog();

  if (mu32ResetReason & RCU_RSTSCK_FWDGTRSTF)
  {
    //Bei WatchDog im Bootloader bleiben
  }
  else
  if (mu32ResetReason & RCU_RSTSCK_SWRSTF)
  {
    //Bei SW-Reset nur im Bootloader bleiben
    //wenn der SW-Reset nicht vom Bootloader kam
    //sonst in die App springen
    u32 lu32Zwerg = ('I' << 24) + ('A' << 16) + ('P' << 8) + 'R';
    if (InAppProg_Platform_u32ReadBackupData(0) == lu32Zwerg)
    {
      InAppProg_Platform_vWriteBackupData(0, 0);
      MAIN_vGotoApp(IAP_PARTITION_START_ADRESS_APP);
    }
  }
  else
  {
    // Bei Power on reset direkt in die App
    MAIN_vGotoApp(IAP_PARTITION_START_ADRESS_APP);
  }


  MAIN_vInitSystem();

  while (1)
  {
    fwdgt_counter_reload();

    if (MAIN_u32Counter_10ms >= 10)
    {
      mcBn.vTickLp10ms();
      MAIN_u32Counter_10ms = 0;
    }
    if (MAIN_u32Counter_100ms >= 100)
    {
      mcLedRed.Toggle();
      MAIN_u32Counter_100ms = 0;
    }

    mcBn.vProcess10ms();

    fwdgt_counter_reload();

    __asm("wfi");
  }
}

