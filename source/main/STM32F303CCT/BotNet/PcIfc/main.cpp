
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
LED<GPIOE_BASE, 15> lcLedRed;

cBotNetCfg mcBnCfg((const char8*)"Botnet Switch", 7, 0xE000);

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


void Delay(__IO uint32_t nTime)
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


cBotNetSwitch *mcBnSwitch;

tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcPcLinkUart(9600, GPIO_AF_7, 128, 128);

cBotNet_LinkBotCom         mcPcLnk(0xE000, (cUart*)&mcPcLinkUart);

cNRF905                    mcNRF905(0x00010100, 0x00010110);
cBotNet_DownLinknRf905     mcTargetLink(0x1000, &mcNRF905);



void I2C2_EV_IRQHandler(void)
{
}

void I2C2_ER_IRQHandler(void)
{
}

void I2C1_EV_IRQHandler(void)
{
}

void I2C1_ER_IRQHandler(void)
{
}

void EXTI15_10_IRQHandler(void)
{
  // Handle PC15 interrupt
  // Make sure that interrupt flag is set
  if (EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    // Clear interrupt flag
    EXTI_ClearITPendingBit(EXTI_Line15);
    mcTargetLink.bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}


void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC2);
  mcTargetLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA_Cmd(DMA1_Channel3, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC3);
  mcTargetLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
}


void TIM4_IRQHandler(void)
{
  if(TIM4->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM_Cmd(TIM4, DISABLE);
    mcTargetLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}


void USART1_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcPcLnk.mcUart->vIRQHandler();

  //mcComPort2.mcConIo.vIRQHandler_Ev();
}

void USART3_IRQHandler(void)
{
  //lcLedRed.Toggle();
  //mcComPort3.mcConIo.vIRQHandler_Ev();
}


void MAIN_vTick1msHp(void)
{
  mcBnSwitch->vTickHp1ms();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vTick10msLp(void)
{
  mcBnSwitch->vTickLp10ms();
}

void MAIN_vTick100msLp(void)
{
  static u8 lu8Cnt = 0;
  if ((mcTargetLink.IsOnline()) || (lu8Cnt == 0))
  {
    lcLedRed.Toggle();
  }

  lu8Cnt++;
  if (lu8Cnt > 5) lu8Cnt = 0;
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

  mcBnSwitch  = new cBotNetSwitch(&mcBnCfg, null);

  mcBnSwitch->bSetUpLink((cBotNet_LinkBase*)&mcPcLnk);
  mcBnSwitch->bSetDownLink((cBotNet_LinkBase*)&mcTargetLink);


  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                MAIN_vTick1000msHp /*1s_HP*/,

                NULL               /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


void vCleanUp()
{
  delete mcBnSwitch;
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

