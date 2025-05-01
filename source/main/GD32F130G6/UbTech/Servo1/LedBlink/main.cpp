
#include "main.h"



// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, (72 MHz max)
// Rom 32KB
// Ram  4KB


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


LED<GPIOB, 1> lcLedRed;

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
}


void SysTick_Handler(void)
{
  vDelay_Decrement();
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
void MAIN_vTick1000msLp(void)
{
  lcLedRed.Toggle();
}



void TIMER2_IRQHandler(void)
{
  static u16 test = 0;
  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    test++;
    if (test == 1000)
    {
      test = 0;
      MAIN_vTick1000msLp();
    }
  }
}


void Tim2_vInit()
{
  timer_parameter_struct timer_initpara;
  rcu_periph_clock_enable(RCU_TIMER2);
  timer_deinit(TIMER2);

  /* TIMER configuration */
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);

  /*NVIC config*/
  nvic_irq_enable(TIMER2_IRQn, 1, 1);

  /* auto-reload preload enable */
  timer_auto_reload_shadow_enable(TIMER2);
  /* auto-reload preload enable */
  timer_enable(TIMER2);
}



void MAIN_vInitSystem(void)
{
  //SystemInit();
  vSysTickInit();

#if 0

  CycCall_Start(NULL /*1ms_HP*/,
    NULL /*10ms_HP*/,
    NULL /*100ms_HP*/,
    NULL /*1s_HP*/,

    NULL               /*1ms_LP*/,
    NULL               /*10ms_LP*/,
    NULL               /*100ms_LP*/,
    MAIN_vTick1000msLp /*1s_LP*/);
#endif
  Tim2_vInit();
}



int main(void)
{
  //fwdgt_config(0x0FFF, FWDGT_PSC_DIV256);
  //fwdgt_counter_reload();

  //volatile u32 mu32ResetReason = RCU_RSTSCK;

  MAIN_vInitSystem();

  while (1)
  {
    //fwdgt_counter_reload();
    //lcLedRed.Toggle();

    //CycCall_vIdle();
    //__asm("wfi");
  }
}

void MainSystemInit(void)
{

}

