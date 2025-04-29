
#include "main.h"

// GD32F330G8
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 64KB
// Ram  8KB

//   - FBL  16k: 0x0800 0000 - 0x0800 3FFF
//   - APP  19k: 0x0800 4000 - 0x0800 FBFF
//   - DATA  1k: 0x0800 FC00 - 0x0800 FFFF
// Ram  8KB
//
// TIM0, TIM1(32Bit), TIM2, TIM13, TIM14, TIM15, TIM16


// Servo:
//
//
// PA00: LED             Tim0_Ch0(AF2)
//
// PA09: I2C0_SCL/U0_Tx: I2C0_SCL(AF4)/USART0_TX(AF1)
// PA10: I2C0_SDA:       I2C0_SCL(AF4)
//
// PA04: Ntc:            ADC_IN4
//
// PB01: Poti:           ADC_IN9
// PA02: Mot Strom:      ADC_IN2
//
// PA01: Mot Sleep:      1 = enable H-Bridge, 0 = disable H-Bridge
// PA05: Mot IN1:        TIMER1_CH0(AF2), ADC_Trigger(TIMER1_CH0)
// PA06: Mot IN2:        TIMER2_CH0(AF1), ADC_Trigger(TIMER2_TRGO)


//#define PCB_PROJECTCFG_Test

#ifdef PCB_PROJECTCFG_Test
  LED<GPIOA, 10>    mcPA10;
#endif


void vLedInit()
{
  // PA0
  rcu_periph_clock_enable(RCU_GPIOA);
  GPIO_CTL(GPIOA) |= 1;  // Output Mode
}

void vLedToggle()
{
  if (gpio_input_bit_get(GPIOA, GPIO_PIN_0))
  {
    gpio_bit_reset(GPIOA, GPIO_PIN_0);
  }
  else
  {
    gpio_bit_set(GPIOA, GPIO_PIN_0);
  }
}


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cUartMpHdSlave mcUartMpHdU0;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);
cBotNet_UpLinkUsartMpHd_RMsg mcUpLink(&mcUartMpHdU0);
cBotNet mcBn(&mcMyBotNetCfg);

#ifdef PCB_PROJECTCFG_Test
static void vDtwStart()
{
  DWT->CTRL &= 0xFF000000UL;
  DWT->CYCCNT = 0;
  DWT->CPICNT = 0;
  DWT->EXCCNT = 0;
  DWT->CTRL |= 0x07F0001UL;
}
#endif


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
  // Per DWT gemessen. Benötigte Cycles
  // --------------------------8Mhz---
  // -Os:     450 + 30: 480:   60us
  // -O1:     400 + 30: 430:   54us
  // -OFast:  340 + 30: 370:   45us
  // 
  // Aber nur in -Os reichts es vom Platz
  // ---------- Text   
  // -Os:     10856
  // -O1:     17412
  // -OFast:  21288
  // 
  // Daher habe ich den entschieden auf 12Mhz zu gehen


  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
    vDtwStart();
  #endif

  // USART1 TX
  // Ender der Übertragung wird durch  U0.TC interrupt erledigt
  // Tx DMA Interrupt wird nicht benötigt
  // if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  // {
  //   dma_channel_disable(DMA_CH1);
  //   DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
  //   mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  // }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
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
      USART_INTC(USART0) = USART_STAT_TC;
      mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
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

u32 MAIN_u32Counter_100ms;

void TIMER13_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (timer_interrupt_flag_get(TIMER13, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_100ms++;
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


void vInitTim13()
{
  rcu_periph_clock_enable(RCU_TIMER13);

  MAIN_u32Counter_100ms = 0;


  /*timer_parameter_struct timer_initpara;
  // TIMER configuration
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER13, &timer_initpara);
  timer_interrupt_enable(TIMER13, TIMER_INT_UP);

  // auto-reload preload enable
  timer_auto_reload_shadow_enable(TIMER13);
  // auto-reload preload enable
  timer_enable(TIMER13);*/


  // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
  // Braucht aber so 90Byte weniger
  TIMER_DMAINTEN(TIMER13) = 1;
  TIMER_PSC(TIMER13)      = 7;  //  15 für 16Mhz und 7 für 8Mhz
  TIMER_CAR(TIMER13)      = 0x3E7;
  TIMER_CTL0(TIMER13)     = 0x081;

  /*NVIC config*/
  nvic_irq_enable(TIMER13_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

  vLedInit();

  // Add Uplink
  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);

  vInitTim13();
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
  // 16k = 16.384Byte

  if (!RomConst_IsValid())
  {
    // RomConst Error
    mcBn.mcAdr.Set(0x1EE0);
  }

  volatile u32 mu32ResetReason = RCU_RSTSCK;
  // 29 FWDGTRSTF:  Set by hardware when a Free Watchdog timer generated. Reset by writing 1 to the RSTFC
  // 28 SWRSTF:     Set by hardware when a software reset generated. Reset by writing 1 to the RSTFC bit.
  // 27 PORRSTF:    Set by hardware when a Power reset generated. Reset by writing 1 to the RSTFC b

  rcu_all_reset_flag_clear();

  fwdgt_counter_reload();
  #ifndef PCB_PROJECTCFG_Test
    cBnSpop_vStartWdog();
  #endif

  #ifndef PCB_PROJECTCFG_Test
    if (mu32ResetReason & RCU_RSTSCK_FWDGTRSTF)
    {
      //Bei WatchDog im Bootloader bleiben
    }
    else
    if (mu32ResetReason & RCU_RSTSCK_SWRSTF)
    {
      // Bei SW-Reset nur im Bootloader bleiben,
      // wenn der SW-Reset nicht vom Applikation kam,
      // sonst in die App springen
      u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
      if (cBnSpop_u32ReadBackupData(0) != lu32Zwerg)
      {
        MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
      }
    }
    else
    {
      // Bei Power on reset direkt in die App
      MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
    }
  #endif

  MAIN_vInitSystem();

  u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
  if (cBnSpop_u32ReadBackupData(0) == lu32Zwerg)
  {
    u32 lcAdress = cBnSpop_u32ReadBackupData(1);

    cBnSpop_vWriteBackupData(0, 0);
    cBnSpop_vWriteBackupData(1, 0);

    mcBn.mcUpLink->mStatus.IsOnline = 1;
    mcBn.mcSpop.vEnter(lcAdress >> 16, (u16)lcAdress, &mcBn);
  }

  while (1)
  {
    
    fwdgt_counter_reload();

    if ((MAIN_u32Counter_100ms % 10) == 0)
    {
      mcBn.vTickLp10ms();
    }
    if (MAIN_u32Counter_100ms > 100)
    {
      vLedToggle();
      MAIN_u32Counter_100ms -= 100;
    }
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



