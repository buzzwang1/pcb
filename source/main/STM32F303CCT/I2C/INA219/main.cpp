
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);
cINA219      mcINA219_A(&mcI2C1, INA219_I2C_ADDRESS_CONF_0, 100);
cINA219      mcINA219_B(&mcI2C1, INA219_I2C_ADDRESS_CONF_1, 100);

tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE,  10> mcComPort(9600, GPIO_AF_7, 64, 64);
//tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort(9600, GPIO_AF_7, 64, 64);

#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G          mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


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


void DMA1_Channel4_IRQHndlr(void)
{
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    /* Disable the DMA1 Channel 4 */
    DMA_Cmd(DMA1_Channel4, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC4);
  }
}

void DMA1_Channel5_IRQHndlr(void)
{
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    /* Disable the DMA1 Channel 5 */
    DMA_Cmd(DMA1_Channel5, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC5);
  }
}

void DMA1_Channel6_IRQHndlr(void)
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

void DMA1_Channel7_IRQHndlr(void)
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



void DMA1_Stream0_IRQHandler(void) {}
void DMA1_Stream1_IRQHandler(void) {}
void DMA1_Stream2_IRQHandler(void) {}

void DMA1_Channel1_IRQHandler(void) {}
void DMA1_Channel2_IRQHandler(void) {}
void DMA1_Channel3_IRQHandler(void) {}

void DMA1_Channel4_IRQHandler(void) {DMA1_Channel4_IRQHndlr();}
void DMA1_Channel5_IRQHandler(void) {DMA1_Channel5_IRQHndlr();}
void DMA1_Channel6_IRQHandler(void) {DMA1_Channel6_IRQHndlr();}
void DMA1_Channel7_IRQHandler(void) {DMA1_Channel7_IRQHndlr();}

void DMA1_Stream3_IRQHandler(void) {DMA1_Channel4_IRQHndlr();}
void DMA1_Stream4_IRQHandler(void) {DMA1_Channel5_IRQHndlr();}
void DMA1_Stream5_IRQHandler(void) {DMA1_Channel6_IRQHndlr();}
void DMA1_Stream6_IRQHandler(void) {DMA1_Channel7_IRQHndlr();}
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
  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }
}

void MAIN_vTick100msLp(void)
{
  mcINA219_A.i8ReadVShunt_digit();
  mcINA219_A.i8ReadVBus_digit();

  mcINA219_B.i8ReadVShunt_digit();
  mcINA219_B.i8ReadVBus_digit();
}

void MAIN_vTick1000msLp(void)
{
  int32 li32IShunt = 0;
  int32 li32VBus   = 0;

  char lszValue[16] = "";
  char lszGrad[] = " mA";
  char lszProz[] = " mV";

  //mcHTU21D->i8ReadUser_digit();

  mcLedRed.Toggle();

  mcScreen1.vFill(0);

  li32IShunt = mcINA219_A.i32CalcIShunt_uA() / 1000;
  li32VBus   = mcINA219_A.i32CalcVBus_uV()   / 1000;

  cStrTools::uixItoa(li32IShunt, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszGrad);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 10, (char8*)"I:",  &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 10, lszValue,      &mcScreen1);

  mcComPort.vAdd((char8*)"I:");mcComPort.vAdd(lszValue);

  cStrTools::uixItoa(li32VBus, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszProz);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 20, (char8*)"V:",   &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 20, lszValue,       &mcScreen1);

  mcComPort.vAdd((char8*)" | V:");mcComPort.vAdd(lszValue);

  li32IShunt = mcINA219_B.i32CalcIShunt_uA() / 1000;
  li32VBus   = mcINA219_B.i32CalcVBus_uV()   / 1000;

  cStrTools::uixItoa(li32IShunt, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszGrad);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 40, (char8*)"I:",  &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 40, lszValue,      &mcScreen1);

  mcComPort.vAdd((char8*)" | I:");mcComPort.vAdd(lszValue);

  cStrTools::uixItoa(li32VBus, lszValue, 10);
  cStrTools::szStrCat(lszValue, lszProz);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 50, (char8*)"V:",   &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60 - cStrTools::uixStrLen(lszValue) * 4, 50, lszValue,       &mcScreen1);

  mcComPort.vAdd((char8*)" | V:");mcComPort.vSend(lszValue);

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
}



void TIM3_vInit();
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


  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(100);
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  mcComPort.vSend("Start\n\r");
  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}

