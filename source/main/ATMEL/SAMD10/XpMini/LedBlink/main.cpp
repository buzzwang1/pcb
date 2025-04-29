
#include "main.h"



// SAMD14D
// ARM®-based Cortex®-M0 32b MCU, (48 MHz max)
// Rom 16KB
// Ram  4KB



LED<(u32)&PORTA, 9> lcLedRed;


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
  //SysTick_Config(system_gclk_gen_get_hz(GCLK_GENERATOR_0));
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




void TC2_Handler(void)
{
  static u16 test = 0;
  REG_TC2_INTFLAG = TC_INTFLAG_OVF;

  test++;
  if (test == 1000)
  {
    test = 0;
    MAIN_vTick1000msLp();
  }
}


void Tim2_vInit()
{
  // Enable TC2,TC3 clock from GCLK1
  //REG_GCLK_PCHCTRL31 = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;

// Configure asynchronous clock source
  GCLK->CLKCTRL.reg  = GCLK_CLKCTRL_ID_TC1_TC2;  // select TC2 peripheral channel
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;   // select source GCLK_GEN[0]
  GCLK->CLKCTRL.bit.CLKEN = 1;                   // enable TC2 generic clock


// Configure synchronous bus clock
  PM->APBCSEL.bit.APBCDIV = 0;              // no prescaler
  PM->APBCMASK.bit.TC2_ = 1;                // enable TC2 interface

  REG_TC2_CTRLA = TC_CTRLA_MODE_COUNT8 | TC_CTRLA_PRESCALER_DIV8;
  REG_TC2_COUNT8_PER = 125;

  TC2->COUNT8.INTENSET.bit.OVF = 1; // set overflow interrupt
  NVIC_EnableIRQ(TC2_IRQn); // enable interrupts for TC2

  TC2->COUNT8.CTRLA.bit.ENABLE = 1; // enable TC2

  while (TC2->COUNT8.CTRLA.bit.ENABLE == 1); // wait for TC2 to be enabled
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  vSysTickInit();

  Tim2_vInit();
}

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {

    //lcLedRed.Toggle();

    //CycCall_vIdle();
    //__asm("wfi");
  }
}


extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/* This is done before the initialize the C/C++ library */
void PreMain(void)
{
  /* Change default QOS values to have the best performance and correct USB behaviour */
  SBMATRIX->SFR[SBMATRIX_SLAVE_HMCRAMC0].reg = 2;
  DMAC->QOSCTRL.bit.DQOS = 2;
  DMAC->QOSCTRL.bit.FQOS = 2;
  DMAC->QOSCTRL.bit.WRBQOS = 2;

  /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
  NVMCTRL->CTRLB.bit.MANW = 1;

  SystemInit();
}

