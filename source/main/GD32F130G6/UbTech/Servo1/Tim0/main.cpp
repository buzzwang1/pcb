
#include "main.h"



// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, (72 MHz max)
// Rom 32KB
// Ram  4KB
// TIM1(32Bit)
// TIM0, TIM2, TIM13, TIM15, TIM16



LED<GPIOB, 1>       mcLedRed;



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


void SysTick_Handler(void)
{
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



u32 MAIN_u32Counter_10ms;
u32 MAIN_u32Counter_100ms;

void TIMER2_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_10ms++;
    MAIN_u32Counter_100ms++;
  }
}


void vInitTim2()
{
  rcu_periph_clock_enable(RCU_TIMER0);
  rcu_periph_clock_enable(RCU_TIMER2);
  rcu_periph_clock_enable(RCU_GPIOA);

  MAIN_u32Counter_10ms = 0;
  MAIN_u32Counter_100ms = 0;

  timer_parameter_struct timer_initpara;
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
  timer_enable(TIMER2);



  // Motor
  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_9);

  //Configure PA8(TIMER0_CH0) as alternate function
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
  gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_9);


  // ------------ Timer0_Config
  /* TIMER0 configuration: generate PWM signals with different duty cycles:
      timer0CLK = SystemCoreClock / 108 = 1MHz */
  rcu_periph_clock_enable(RCU_TIMER0);

  timer_oc_parameter_struct timer_ocintpara;

  timer_deinit(TIMER0);

  // TIMER0 configuration
  timer_initpara.prescaler = 3; // 0.5us
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1;    // 1 Mhz
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER0, &timer_initpara);

  // CH0 configuration in PWM mode
  timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
  timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
  timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
  timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
  timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
  timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
  timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);

  timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 0);
  timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
  timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

  timer_primary_output_config(TIMER0, ENABLE);
  // auto-reload preload enable
  timer_auto_reload_shadow_enable(TIMER0);
  timer_enable(TIMER0);

  timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 1);

  /*NVIC config*/
  nvic_irq_enable(TIMER2_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  vInitTim2();
}


int main(void)
{
  // 12k = 12.288Byte

  MAIN_vInitSystem();

  while (1)
  {
    if (MAIN_u32Counter_10ms >= 10)
    {
      MAIN_u32Counter_10ms = 0;
    }
    if (MAIN_u32Counter_100ms >= 100)
    {
      mcLedRed.Toggle();
      MAIN_u32Counter_100ms = 0;
    }

    __asm("wfi");
  }
}

void MainSystemInit(void)
{

}