
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB


RCC_ClocksTypeDef RCC_Clocks;
u32 TimingDelay = 0;

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


void Delay(u32 nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay > 0)
  {
    TimingDelay--;
  }
  else
  {
    TimingDelay = 0;
  }
}

void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}



tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcPcLinkUart(115200, GPIO_AF_7, 128, 128);




//Display
//void DMA1_Channel5_IRQHandler(void)
//{
//  // SPI TX
//  DMA1_Channel5->CCR &= ~DMA_CCR_EN;
//  DMA1->IFCR          = DMA1_FLAG_GL5 | DMA1_FLAG_HT5 | DMA1_FLAG_TC5;
//  mcDisplay.vEvDmaTc();
//}

//void EXTI15_10_IRQHandler(void)
//{
//  // Handle PC15 interrupt
//  // Make sure that interrupt flag is set
//  if (EXTI_GetITStatus(EXTI_Line15) != RESET)
//  {
//    // Clear interrupt flag
//    EXTI_ClearITPendingBit(EXTI_Line15);
//    mcTargetLink.bEventHandler(cNRF905::NRF905_EvDataReady);
//  }
//}



//void TIM4_IRQHandler(void)
//{
//  if(TIM4->SR & TIM_SR_UIF) // if UIF flag is set
//  {
//    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
//    TIM_Cmd(TIM4, DISABLE);
//    mcTargetLink.bEventHandler(cNRF905::NRF905_EvTimer);
//  }
//}


void USART1_IRQHandler(void)
{
  mcPcLinkUart.vIRQHandler();
}



void MAIN_vTick1msHp(void)
{
}

void MAIN_vTick10msHp(void)
{

}

void MAIN_vTick10msLp(void)
{
}

void MAIN_vTick100msLp(void)
{
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  // SysTick end of count event each 10ms
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);


  //----------  Liniensensor ------------------------------

  //PA3 - Pxcl_Clock
  tcGpPin<GPIOA_BASE, 3> lcPA03(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 0);
  lcPA03.vSetAF(GPIO_AF_9); // TIM15_CH2

  //PA6 - Start Pulse
  tcGpPin<GPIOA_BASE, 6> lcPA06(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1);
  lcPA06.vSetAF(GPIO_AF_1); // TIM16_CH1

  //TIM15 - Pxcl_Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
  TIM15->CCMR1 = 0x7000;
  TIM15->CCER  = 0x10;
  TIM15->ARR   = 17; // 72Mhz / 18 = 4Mhz;
  TIM15->BDTR  = 0x8000;
  TIM15->PSC   = 31;
  TIM15->CCR2  = 9;
  TIM15->CR1   = 1;


  //TIM16 - Pxcl_Start
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
  TIM16->CCMR1 = 0x0070;
  TIM16->CCER  = 0x01;
  TIM16->ARR   = 36000-1; // 72Mhz => 500ms;
  TIM16->BDTR  = 0x8000;
  TIM16->PSC   = 31;
  TIM16->CCR1  = 20;
  TIM16->CR1   = 0x1;


  //LaserDiode
  //PA12 - Laser Pulse
  tcGpPin<GPIOA_BASE, 12> lcPA12(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1);
  lcPA12.vSetAF(GPIO_AF_1); // TIM16_CH1


  tcGpPin<GPIOA_BASE, 11> lcPA11(GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1);
  tcGpPin<GPIOB_BASE, 12> lcPb12(GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1);


  //DAC auf 2V
  tcGpPin<GPIOA_BASE, 4> lcPA04(GPIO_Mode_AN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 0);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC1, ENABLE);
  DAC1->DHR12R1 = 0xFFF; //=2440d = 1,96V
  DAC1->CR      = 1;





  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                MAIN_vTick1000msHp /*1s_HP*/,

                NULL               /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

