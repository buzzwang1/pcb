
#include "main.h"



// STM32F103C8T
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 64KB
// Ram 24KB


RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;

LED<GPIOC_BASE, 13> mcLedRed;
//LED<GPIOB_BASE, 9> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_Mode_AF_OD, GPIO_Speed_50MHz, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);

cClockInfo mcClkInfo;
cGpPin *lcUsartRx;
cGpPin *lcUsartTx;



//Digi Power
cGpPin lcDigi1 = cGpPin(GPIOA_BASE, 10, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);
cGpPin lcDigi2 = cGpPin(GPIOA_BASE, 11, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);
cGpPin lcDigi3 = cGpPin(GPIOA_BASE, 12, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);

//ana Power
cGpPin lcAna1 = cGpPin(GPIOB_BASE, 0, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);
cGpPin lcAna2 = cGpPin(GPIOB_BASE, 1, GPIO_Mode_Out_PP, GPIO_Speed_2MHz, 1);


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


// ADC
typedef enum
{
  // Events für allgemeine Verwenung
  MAIN_nEvReset = 0,
  MAIN_nEvStart,
  MAIN_nEvStop,

  MAIN_nEvDummy
}MAIN_tenEvents;


typedef enum 
{
  MAIN_nSmIdle = 0,

  // States für ADC
  MAIN_nSmAdcRun,

  MAIN_nSmDummy
}MAIN_tenStates;


typedef enum
{
  MAIN_nChnLdrL    = ADC_Channel_1,
  MAIN_nChnLdrR    = ADC_Channel_2,
  MAIN_nChnLdrO    = ADC_Channel_3,
  MAIN_nChnLdrH    = ADC_Channel_4,
  MAIN_nChnLdrU    = ADC_Channel_5,
  MAIN_nChnVSol    = ADC_Channel_6,

  MAIN_nADCResolution10 =  1024, //1 << 10, // 10 Bit
  MAIN_nADCResolution12 =  4096, //1 << 12, // 12 Bit
  MAIN_nADCResolution16 = 65536, //1 << 16, // 16 Bit
  MAIN_nADCResolution   = MAIN_nADCResolution12,
  MAIN_nADCRefmV        = 3300,

  MAIN_nVDummy
}MAIN_tenConsts;


MAIN_tenStates menSmMain_Adc = MAIN_nSmIdle;

#define ADC_MEASURE_CNT 6
#define ADC_MEASURE_AVG (MAIN_nADCResolution16 / MAIN_nADCResolution)

typedef struct tstAdc_type
{
  uint8  ui8AdcConvDone;
  uint8  ui8ChlIdx;
  uint8  ui8ChlList[ADC_MEASURE_CNT];
  uint16 ui16Value[ADC_MEASURE_CNT];
  uint16 ui16ValueRaw[ADC_MEASURE_CNT];
  uint16 ui16ValueAvg[ADC_MEASURE_CNT];
  uint8  ui8Cnt[ADC_MEASURE_CNT];

}tstAdc;

tstAdc stAdc;


typedef struct tstADCValues_type
{
  uint16 ui16LdrL;
  uint16 ui16LdrR;
  uint16 ui16LdrO;
  uint16 ui16LdrH;
  uint16 ui16LdrU;
  uint16 ui16VSol;

}tstADCValues;

tstADCValues *pstADCValuesAvg = (tstADCValues *)stAdc.ui16ValueAvg;
tstADCValues *pstADCValuesRaw = (tstADCValues *)stAdc.ui16ValueRaw;


void vMain_SmAdc(MAIN_tenEvents lenEvent)
{
  if (lenEvent == MAIN_nEvReset)
  {
    uint8 lui8t;
    for (lui8t = 0; lui8t < ADC_MEASURE_CNT; lui8t++)
    {
      stAdc.ui16Value[lui8t]    = 0;
      stAdc.ui16ValueRaw[lui8t] = 0;
      stAdc.ui16ValueAvg[lui8t] = 0;
      stAdc.ui8Cnt[lui8t]       = 0;
    }
    
    stAdc.ui8AdcConvDone = 0;

    stAdc.ui8ChlList[0] = MAIN_nChnLdrL;
    stAdc.ui8ChlList[1] = MAIN_nChnLdrR;
    stAdc.ui8ChlList[2] = MAIN_nChnLdrO;
    stAdc.ui8ChlList[3] = MAIN_nChnLdrH;
    stAdc.ui8ChlList[4] = MAIN_nChnLdrU;
    stAdc.ui8ChlList[5] = MAIN_nChnVSol;

    menSmMain_Adc = MAIN_nSmIdle;   
  }
  else
  if (lenEvent == MAIN_nEvStop)
  {
    #ifndef WIN32
      ADC_DeInit(ADC1);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
    #endif
    menSmMain_Adc = MAIN_nSmIdle;    
  }
  else
  {  
    switch (menSmMain_Adc)
    {
      case MAIN_nSmIdle:
        if (lenEvent == MAIN_nEvStart)
        {
          #ifndef WIN32
            ADC_InitTypeDef       ADC_InitStructure;
            GPIO_InitTypeDef      GPIO_InitStructure;

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

            /* PCLK2 is the APB2 clock */
            /* ADCCLK = PCLK2/6 = 24/6 = 4MHz*/
            RCC_ADCCLKConfig(RCC_PCLK2_Div6);

            //enable ADC1 clock
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

            
            //ADC1 configuration
            //select independent conversion mode (single)
            ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
            //We will convert single channel only
            ADC_InitStructure.ADC_ScanConvMode = DISABLE;
            //we will convert one time
            ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
            //select no external triggering
            ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
            //right 12-bit data alignment in ADC data register
            ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
            //single channel conversion
            ADC_InitStructure.ADC_NbrOfChannel = 1;
            //load structure values to control and status registers
            ADC_Init(ADC1, &ADC_InitStructure);
            //wake up temperature sensor
            ADC_TempSensorVrefintCmd(ENABLE);
          #endif

          stAdc.ui8ChlIdx = 0;

          #ifndef WIN32
            //we select 41.5 cycles conversion for channel16
            //and rank=1 which doesn't matter in single mode
            ADC_RegularChannelConfig(ADC1, stAdc.ui8ChlList[stAdc.ui8ChlIdx], 1, ADC_SampleTime_41Cycles5);
            //Enable ADC1
            ADC_Cmd(ADC1, ENABLE);
            //Enable ADC1 reset calibration register
            ADC_ResetCalibration(ADC1);
            //Check the end of ADC1 reset calibration register
            while(ADC_GetResetCalibrationStatus(ADC1));
            //Start ADC1 calibration
            ADC_StartCalibration(ADC1);
            //Check the end of ADC1 calibration
            while(ADC_GetCalibrationStatus(ADC1));
            //Start ADC1 Software Conversion
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
          #endif

          menSmMain_Adc = MAIN_nSmAdcRun;
        }
        break;

      case MAIN_nSmAdcRun:
        #ifndef WIN32
          if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
          {
            stAdc.ui16ValueRaw[stAdc.ui8ChlIdx]  = ADC_GetConversionValue(ADC1);
            stAdc.ui16Value[stAdc.ui8ChlIdx]    += stAdc.ui16ValueRaw[stAdc.ui8ChlIdx];
            if (stAdc.ui8Cnt[stAdc.ui8ChlIdx] < (ADC_MEASURE_AVG - 1))
            {
              stAdc.ui8Cnt[stAdc.ui8ChlIdx]++;
            }
            else
            {
              stAdc.ui16ValueAvg[stAdc.ui8ChlIdx] = stAdc.ui16Value[stAdc.ui8ChlIdx] / ADC_MEASURE_AVG;
              stAdc.ui16Value[stAdc.ui8ChlIdx] = 0;
              stAdc.ui8Cnt[stAdc.ui8ChlIdx]    = 0;
              stAdc.ui8AdcConvDone++;
            }

            stAdc.ui8ChlIdx++;
            if (stAdc.ui8ChlIdx == ADC_MEASURE_CNT) stAdc.ui8ChlIdx = 0;

            ADC_RegularChannelConfig(ADC1, stAdc.ui8ChlList[stAdc.ui8ChlIdx], 1, ADC_SampleTime_41Cycles5);
            ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear EOC flag
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
          }
        #else
          stAdc.ui16ValueRaw[stAdc.ui8ChlIdx]  = ((uint16)rand()) & (MAIN_nADCResolution - 1); //ADC_GetConversionValue(ADC1);
          stAdc.ui16Value[stAdc.ui8ChlIdx]    += stAdc.ui16ValueRaw[stAdc.ui8ChlIdx];
          if (stAdc.ui8Cnt[stAdc.ui8ChlIdx] < (ADC_MEASURE_AVG - 1))
          {
            stAdc.ui8Cnt[stAdc.ui8ChlIdx]++;
          }
          else
          {
            stAdc.ui16ValueAvg[stAdc.ui8ChlIdx] = stAdc.ui16Value[stAdc.ui8ChlIdx] / ADC_MEASURE_AVG;
            stAdc.ui16Value[stAdc.ui8ChlIdx] = 0;
            stAdc.ui8Cnt[stAdc.ui8ChlIdx]    = 0;
            if (stAdc.ui8AdcConvDone < UINT8MAX)
            {
              stAdc.ui8AdcConvDone++;
            }
          }

          stAdc.ui8ChlIdx++;
          if (stAdc.ui8ChlIdx == ADC_MEASURE_CNT) stAdc.ui8ChlIdx = 0;
        #endif
        break;
      default:
        break;

    }
  }
}



void MAIN_vTick10msLp(void)
{
}

void MAIN_vTick100msLp(void)
{
  vMain_SmAdc(MAIN_nEvDummy);
}


int8 kbhit(void)
{
  return (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET);
}

void setch(int8 lszChar)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  // warten, falls Buffer voll
  USART_SendData(USART1, lszChar);     // Byte senden
}


int8 getch(void)
{
  return (USART_ReceiveData(USART1));
}


void PrintString(const char *lszString)
{
  while (*lszString)
  {
    setch(*lszString++);     // Byte senden
  }
}


/*
void MAIN_vItoa_HUD21D(int num, char8* str)
{
  int32 li32Val1 = 0;
  int32 li32Val2 = 0;
  char lszVal1[8] = "";
  char lszVal2[8] = "";
  char lszPkt[] = ",";

  li32Val1 = num / 100;
  li32Val2 = num - (li32Val1 * 100);

  StrTools_i8Itoa(li32Val1, lszVal1, 10);

  if (li32Val2 < 10)
  {
    lszVal2[0] = '0';
    StrTools_i8Itoa(li32Val2, (char8*)(&lszVal2[1]), 10);
  }
  else
  {
    StrTools_i8Itoa(li32Val2, lszVal2, 10);
  }

  StrTools_szStrCpy(str, lszVal1);
  StrTools_szStrCat(str, lszPkt);
  StrTools_szStrCat(str, lszVal2);
}*/


void MAIN_vTick1000msLp(void)
{
  char lszVal1[8] = "";


  mcLedRed.Toggle();

  PrintString("Test");
  
  mcScreen1.vFill(0);

  for (uint8 lui8t = 0; lui8t < 6; lui8t++)
  {
    cStrTools::i8Itoa(stAdc.ui16ValueRaw[lui8t], lszVal1, 10);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 10 + lui8t * 10, lszVal1,  &mcScreen1);
  }

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
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

  //mcI2C1.mui16WaitCnt_Reload = 100;


  /* ADC */
  vMain_SmAdc(MAIN_nEvReset);
  vMain_SmAdc(MAIN_nEvStart);


	/* USARTx configured as follow:
		- BaudRate = 19200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/

  lcUsartTx = (cGpPin*)  new cGpPin(GPIOA_BASE, 9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
  //lcUsartRx = (cGPPIN*)  new cGPPIN(RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_10, GPIO_Mode_IPD,   GPIO_Speed_50MHz, 0);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_BaudRate   = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

  mcI2C1.vStartNext();
  mcClkInfo.Delay_ms(1000);

  CycCall_Start(NULL /*1ms_HP*/,
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

