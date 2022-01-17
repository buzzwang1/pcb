
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);

tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcUart(9600, GPIO_AF_7, 128, 128);


cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cAPDS9960    mcAPDS9960(&mcI2C1, nAPDS9960_I2C_ADDR, &mcUart);



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
  //1ms at 72Mhs
  uint32 lui32DecValue = 72000000 / cClockInfo::mstClocks.HCLK_Frequency;

  if (TimingDelay > lui32DecValue)
  {
    TimingDelay -= lui32DecValue;
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


void DMA1_Channel1_IRQHandler(void)
{
}

void DMA1_Channel2_IRQHandler(void)
{
}

void DMA1_Channel3_IRQHandler(void)
{

}

void DMA1_Channel4_IRQHandler(void)
{
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    /* Disable the DMA1 Channel 4 */
    DMA_Cmd(DMA1_Channel4, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC4);
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    /* Disable the DMA1 Channel 5 */
    DMA_Cmd(DMA1_Channel5, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC5);
  }
}

void DMA1_Channel6_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Tx

  //if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
  {
    /* Disable the DMA1 Channel 6 */
    DMA_Cmd(DMA1_Channel6, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC6);
  }
}

void DMA1_Channel7_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Rx
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    /* Disable the DMA1 Channel 7 */
    DMA_Cmd(DMA1_Channel7, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC7);
  }
}


void I2C1_EV_IRQHandler(void)
{
  mcI2C1.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1.I2C_ER_IRQHandler();
}


void MAIN_vTick10msHp(void)
{
  //mcHTU21D->vTick10ms();
  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }
}



void MAIN_vTick100msLp(void)
{
  mcAPDS9960.i8ReadGestureSensor();
}

void MAIN_vItoa_HUD21D(int num, char8* str)
{
  int32 li32Val1 = 0;
  int32 li32Val2 = 0;
  char lszVal1[8] = "";
  char lszVal2[8] = "";
  char lszPkt[] = ",";

  li32Val1 = num / 100;
  li32Val2 = num - (li32Val1 * 100);

  cStrTools::uixItoa(li32Val1, lszVal1, 10);

  if (li32Val2 < 10)
  {
    lszVal2[0] = '0';
    cStrTools::uixItoa(li32Val2, (char8*)(&lszVal2[1]), 10);
  }
  else
  {
    cStrTools::uixItoa(li32Val2, lszVal2, 10);
  }

  cStrTools::szStrCpy(str, lszVal1);
  cStrTools::szStrCat(str, lszPkt);
  cStrTools::szStrCat(str, lszVal2);
}


void MAIN_vTick1000msLp(void)
{
  mcAPDS9960.i8ReadGestureSensor();

  mcLedRed.Toggle();

  switch (mcAPDS9960.menDirection)
  {
    case cAPDS9960::nAPDS9960_DIR_DOWN:
      mcUart.vSend("D");
      break;
    case cAPDS9960::nAPDS9960_DIR_UP:
      mcUart.vSend("U");
      break;
    case cAPDS9960::nAPDS9960_DIR_RIGHT:
      mcUart.vSend("R");
      break;
    case cAPDS9960::nAPDS9960_DIR_LEFT:
      mcUart.vSend("L");
      break;
    default:
      mcUart.vSend("-");
      break;
  }
}

void MAIN_vInitSystem(void)
{
  SystemInit();
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);


  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_4);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_4);

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  //mcAPDS9960->i8EnableAmbientLight();
  mcAPDS9960.i8EnableProximitySensor();
  mcAPDS9960.i8EnableGestureSensor();

  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(100);
  }
}

void USART1_IRQHandler(void)
{
   mcUart.vIRQHandler();
}

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}

