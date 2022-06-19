
#include "main.h"



// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, (72 MHz max)
// Rom 32KB
// Ram  4KB



//RCC_ClocksTypeDef RCC_Clocks;
LED<GPIOB, 1> lcLedRed;


void NMI_Handler(void)
{
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
}

void DebugMon_Handler(void)
{
}


void PendSV_Handler(void)
{
}


static volatile u32 delay;


void systick_config(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1);
    }
    NVIC_SetPriority(SysTick_IRQn, 0x00);
}

void delay_1ms(uint32_t count)
{
    delay = count;
    while(0 != delay);
}

void delay_decrement(void)
{
    if (0 != delay)
    { 
        delay--;
    }
}


void SysTick_Handler(void)
{
  delay_decrement();
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


void MAIN_vTick100msLp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  // Construct function
  //SystemInit();
  /* SysTick end of count event each 10ms */
  //RCC_GetClocksFreq(&RCC_Clocks);
  //SystemCoreClockUpdate();

  systick_config();

  //CycCall_Start(NULL /*1ms_HP*/,
  //              NULL /*10ms_HP*/,
  //              NULL /*100ms_HP*/,
  //              NULL /*1s_HP*/,
  //
  //              NULL               /*1ms_LP*/,
  //              NULL               /*10ms_LP*/,
  //              MAIN_vTick100msLp /*100ms_LP*/,
  //              MAIN_vTick1000msLp /*1s_LP*/);
}



int main(void)
{
  MAIN_vInitSystem();

  fwdgt_config(0x0FFF, FWDGT_PSC_DIV256);
  while (1)
  {
    
    fwdgt_counter_reload();
    lcLedRed.On();
    delay_1ms(500);

    fwdgt_counter_reload();
    lcLedRed.Off();
    delay_1ms(500);
    //CycCall_vIdle();
    //__asm("wfi");
  }
}

