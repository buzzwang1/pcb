
#include "main.h"



// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 24KB


RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

LED<GPIOB_BASE, 9> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);
cLSM303DLHC_MAG mcMag(&mcI2C1);
cLSM303DLHC_ACC mcAcc(&mcI2C1);

cClockInfo mcClkInfo;
cGpPin *lcUsartRx;
cGpPin *lcUsartTx;

uint16 lui16ActualPosMot1 = 0;
uint16 lui16TargetPosMot1 = 0;
uint16 lui16ActualPosMot2 = 0;
uint16 lui16TargetPosMot2 = 0;


//Digi Power
cGpPin lcDigi1 = cGpPin(GPIOB_BASE, 14, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);
cGpPin lcDigi2 = cGpPin(GPIOB_BASE, 15, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);

//ana Power


#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G          mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


/*cGPPIN lcSM1A = cGPPIN(RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP, 0, 0, GPIO_Speed_10MHz, 1);
cGPPIN lcSM1B = cGPPIN(RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_7, GPIO_Mode_Out_PP, 0, 0, GPIO_Speed_10MHz, 0);
cGPPIN lcSM1C = cGPPIN(RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP, 0, 0, GPIO_Speed_10MHz, 0);
cGPPIN lcSM1D = cGPPIN(RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP, 0, 0, GPIO_Speed_10MHz, 0);*/


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
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
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

void DMA1_Channel4_IRQHandler(void) // I2C2 TX
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC4);
  }
}

void DMA1_Channel5_IRQHandler(void) // I2C2 RX
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC5);
  }
}

void DMA1_Channel6_IRQHandler(void) // I2C1 TX
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
  {
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC6);

    mcI2C1.I2C_EV_DMAHandler();
  }
}

void DMA1_Channel7_IRQHandler(void) // I2C1 RX
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC7);

    mcI2C1.I2C_EV_DMAHandler();
  }
}


/*
TIM1_CH1 PA8
TIM1_CH2 PA9
TIM1_CH3 PA10
TIM1_CH4 PA11

TIM3_CH1 PA6
TIM3_CH2 PA7
TIM3_CH3 PB0
TIM3_CH4 PB1
*/

void vInit_PWM(void)
{
  TIM_TimeBaseInitTypeDef lstTim;
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  // --- Ports

  cGpPin lcPWM1A = cGpPin(GPIOA_BASE,  8,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM1B = cGpPin(GPIOA_BASE,  9,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM1C = cGpPin(GPIOA_BASE, 10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM1D = cGpPin(GPIOA_BASE, 11, GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);

  cGpPin lcPWM2A = cGpPin(GPIOA_BASE, 6,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM2B = cGpPin(GPIOA_BASE, 7,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM2C = cGpPin(GPIOB_BASE, 0,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  cGpPin lcPWM2D = cGpPin(GPIOB_BASE, 1,  GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);

  // --- Timer
  // 8Bit PWM, PWM 64us => 64us / 256 = 0,25us => 4Mhz

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&lstTim);
  lstTim.TIM_Prescaler = (cClockInfo::mstClocks.PCLK2_Frequency / 4000000) - 1;  // 0,25us
  lstTim.TIM_CounterMode = TIM_CounterMode_Up;
  lstTim.TIM_Period = 256 - 1; // 256 / 4 = 64us

  lstTim.TIM_ClockDivision = TIM_CKD_DIV1;
  lstTim.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &lstTim);
  TIM_TimeBaseInit(TIM3, &lstTim);

  // --- Output Compare
  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_Pulse        = 31;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);


  TIM_Cmd(TIM1, ENABLE);
  TIM_Cmd(TIM3, ENABLE);

  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
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
  mcI2C1.vStartNext();
}



const uint8 Sine_Table[128]  = { 10, 18, 26, 34, 42, 50, 58, 66, 74, 84,
                                 92, 102, 112, 124, 136, 148, 162, 176, 190, 202,
                                 216, 226, 236, 242, 248, 250, 252 ,254, 254, 252,
                                 252, 252, 252, 252, 252, 252, 252, 254, 254, 252,
                                 252, 252, 250, 248, 246, 242, 238, 230, 222, 212,
                                 198, 184, 166, 148, 130, 110, 92, 74, 58, 42,
                                 30, 18, 8, 0, 10, 18, 26, 34, 42, 50,
                                 58, 66, 74, 84, 92, 102, 112, 124, 136, 148,
                                 162, 176, 190, 202, 216, 226, 236, 242, 248, 250,
                                 252, 254, 254, 252, 252, 252, 252, 252, 252, 252,
                                 252, 254, 254, 252, 252, 252, 250, 248, 246, 242,
                                 238, 230, 222, 212, 198, 184, 166, 148, 130, 110,
                                 92, 74, 58, 42, 30, 18, 8, 0 };

const uint8 Cos_Table[128] = { 218, 222, 224, 228, 230, 234, 236, 238, 242, 242,
                               244, 244, 242, 238, 234, 226, 216, 202, 188, 170,
                               152, 130, 110, 90, 70, 52, 36, 20, 8, 4,
                               14, 22, 30, 38, 44, 50, 58, 64, 70, 74,
                               80, 86, 92, 98, 104, 110, 118, 126, 136, 144,
                               154, 164, 172, 180, 188, 194, 200, 202, 206, 208,
                               210, 212, 214, 216, 218, 222, 224, 228, 230, 234,
                               236, 238, 242, 242, 244, 244, 242, 238, 234, 226,
                               216, 202, 188, 170, 152, 130, 110, 90, 70, 52,
                               36, 20, 8, 4, 14, 22, 30, 38, 44, 50,
                               58, 64, 70, 74, 80, 86, 92, 98, 104, 110,
                               118, 126, 136, 144, 154, 164, 172, 180, 188, 194,
                               200, 202, 206, 208, 210, 212, 214, 216 };


void vDrive(uint16 &lui16ActualPos, uint16 &lui16TargetPos, TIM_TypeDef* lstTIMx)
{
  uint16 lui16Index;

  if (lui16ActualPos < lui16TargetPos)
  {
    lui16ActualPos++;
  }

  if (lui16ActualPos > lui16TargetPos)
  {
    lui16ActualPos--;
  }

  lui16Index = lui16ActualPos % 128;

  if (lui16Index < 32)
  {
    TIM_SetCompare1(lstTIMx, Sine_Table[lui16Index]);
    TIM_SetCompare2(lstTIMx, 0);
    TIM_SetCompare3(lstTIMx, Cos_Table[lui16Index]);
    TIM_SetCompare4(lstTIMx, 0);
  }
  if ((lui16Index  > 31) &&  (lui16Index  < 64))
  {
    TIM_SetCompare1(lstTIMx, Sine_Table[lui16Index]);
    TIM_SetCompare2(lstTIMx, 0);
    TIM_SetCompare3(lstTIMx, 0);
    TIM_SetCompare4(lstTIMx, Cos_Table[lui16Index]);
  }
  if ((lui16Index  > 63) &&  (lui16Index  < 96))
  {
    TIM_SetCompare1(lstTIMx, 0);
    TIM_SetCompare2(lstTIMx, Sine_Table[lui16Index]);
    TIM_SetCompare3(lstTIMx, 0);
    TIM_SetCompare4(lstTIMx, Cos_Table[lui16Index]);
  }
  if ((lui16Index  > 95) &&  (lui16Index  < 128))
  {
    TIM_SetCompare1(lstTIMx, 0);
    TIM_SetCompare2(lstTIMx, Sine_Table[lui16Index]);
    TIM_SetCompare3(lstTIMx, Cos_Table[lui16Index]);
    TIM_SetCompare4(lstTIMx, 0);
  }
}


void MAIN_vTick1msHp(void)
{
  vDrive(lui16ActualPosMot1, lui16TargetPosMot1, TIM1);
  vDrive(lui16ActualPosMot2, lui16TargetPosMot2, TIM3);
}

void MAIN_vTick10msLp(void)
{
}

void MAIN_vTick100msLp(void)
{
  mcMag.vReadMag();
  mcAcc.vReadAcc();

  char8 lszValue[12];

  mcLedRed.Toggle();

  mcScreen1.vFill(0);

  cPaint::vRect(4, 4, 120, 58, 1, &mcScreen1);

  cStrTools::i8Itoa(mcMag.i16GetMagX(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, lszValue, &mcScreen1);
  cStrTools::i8Itoa(mcMag.i16GetMagY(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszValue, &mcScreen1);
  cStrTools::i8Itoa(mcMag.i16GetMagZ(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 40, lszValue, &mcScreen1);

  cStrTools::i8Itoa(mcAcc.i16GetAccX(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 20, lszValue, &mcScreen1);
  cStrTools::i8Itoa(mcAcc.i16GetAccY(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 30, lszValue, &mcScreen1);
  cStrTools::i8Itoa(mcAcc.i16GetAccZ(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 40, lszValue, &mcScreen1);

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
}


int8 kbhit(void)
{
  return (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET);
}

void setch(int8 lszChar)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  // warten, falls Buffer voll
  USART_SendData(USART2, lszChar);     // Byte senden
}


int8 getch(void)
{
  return (USART_ReceiveData(USART2));
}


void PrintString(const char *lszString)
{
  while (*lszString)
  {
    setch(*lszString++);     // Byte senden
  }
}

void MAIN_vTick1000msLp(void)
{
  /*char8 lszValue[12];

  lcLedRed->Toggle();

  BPP1_1G_Tools_i8ClearScreen(0, MAIN_stDisplay);
  BPP1_1G_Paint_i8CRect(4, 4, 120, 58, 1, MAIN_stDisplay);

  StrTools_i8Itoa(mcMag->i16GetMagX(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(10, 20, lszValue,  MAIN_stRFont01, MAIN_stDisplay);
  StrTools_i8Itoa(mcMag->i16GetMagY(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(10, 30, lszValue,  MAIN_stRFont01, MAIN_stDisplay);
  StrTools_i8Itoa(mcMag->i16GetMagZ(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(10, 40, lszValue,  MAIN_stRFont01, MAIN_stDisplay);

  StrTools_i8Itoa(mcAcc->i16GetAccX(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(60, 20, lszValue,  MAIN_stRFont01, MAIN_stDisplay);
  StrTools_i8Itoa(mcAcc->i16GetAccY(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(60, 30, lszValue,  MAIN_stRFont01, MAIN_stDisplay);
  StrTools_i8Itoa(mcAcc->i16GetAccZ(), lszValue, 10);
  BPP1_1G_RFont_i8PutStringXY(60, 40, lszValue,  MAIN_stRFont01, MAIN_stDisplay);


  mcSSD1306->vShowScreen(MAIN_stDisplay);
  mcSSD1306->Update();*/
}


void TIM2_vInit();
void MAIN_vInitSystem(void)
{
  USART_InitTypeDef USART_InitStructure;

  // Construct function
  SystemInit();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

  mcClkInfo.Update();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  


  vInit_PWM();

	/* USARTx configured as follow:
		- BaudRate = 19200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/

  lcUsartTx = (cGpPin*)  new cGpPin(GPIOA_BASE, 2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  lcUsartRx = (cGpPin*)  new cGpPin(GPIOA_BASE, 3, GPIO_Mode_IPD,   GPIO_Speed_50MHz, 0);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  USART_InitStructure.USART_BaudRate   = 19200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits   = USART_StopBits_1;
  USART_InitStructure.USART_Parity     = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);

  mcI2C1.vStartNext();
  mcClkInfo.Delay_ms(100);

  mcMag.vSetConfig(LSM303DLHC_MAG_TEMPSENSOR_ENABLE, LSM303DLHC_MAG_ODR_15_HZ, LSM303DLHC_MAG_FS_8_1_GA, LSM303DLHC_MAG_CONTINUOS_CONVERSION);  
  mcAcc.vSetControl(LSM303DLHC_ACC_ODR_25_HZ | LSM303DLHC_ACC_NORMAL_MODE | LSM303DLHC_ACC_X_ENABLE | LSM303DLHC_ACC_Y_ENABLE | LSM303DLHC_ACC_Z_ENABLE,
                    LSM303DLHC_ACC_HPM_NORMAL_MODE | LSM303DLHC_ACC_HPFCF_32,
                    0, // No Ints
                    LSM303DLHC_ACC_BlockUpdate_Continous | LSM303DLHC_ACC_FULLSCALE_2G, 
                    0,
                    0
                    );

  mcI2C1.vStartNext();
  mcClkInfo.Delay_ms(100);

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,


                NULL  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}



int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    if (kbhit())
    {
      setch(getch());
    }
    __asm("wfi");
  }
}

