#ifndef NRF905_SPI_H_
#define NRF905_SPI_H_

#ifndef _WIN32

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_syscfg.h"

#include "ClockInfo.h"


#define SPI_DIRECTION_TX 0
#define SPI_DIRECTION_RX 1

class cNRF905_SpiPins
{
  public:

  //SPI1 Outputs
  tcGPin<GPIOA_BASE, 5> mSCK;
  tcGPin<GPIOA_BASE, 6> mMISO;
  tcGPin<GPIOA_BASE, 7> mMOSI;

  cNRF905_SpiPins()
    : mSCK( GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMISO(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMOSI(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0)
  {
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5);  // SCK
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5);  // MISO
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);  // MOSI
  };
};

class cNRF905_Spi
{
  public:

  cNRF905_SpiPins mPins;

  SPI_TypeDef* mSPI;
  DMA_Channel_TypeDef* mDMA_Tx;
  DMA_Channel_TypeDef* mDMA_Rx;

  uint8 mui8Dummy; // Dummy für Rx

  cNRF905_Spi()
  : mPins()
  {
    mSPI     = SPI1;
    mDMA_Tx  = DMA1_Channel3;
    mDMA_Rx  = DMA1_Channel2;

    mui8Dummy = 0x55;

    cClockInfo::Update();

    SPI_InitTypeDef   SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);      // Enable DMA1 clock

    /* SD_SPI Config */
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;

    //8Mhz   /   2 =>   4  Mhz
    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 72000000: SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; break;
      case 16000000: SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; break;
      default: SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; break;
    }
    

    SPI_InitStructure.SPI_FirstBit       = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial  = 7;
    SPI_Init(mSPI, &SPI_InitStructure);

    SPI_RxFIFOThresholdConfig(mSPI, SPI_RxFIFOThreshold_QF);

    SPI_Cmd(mSPI, ENABLE); /* SD_SPI enable */

    //----------------------------------------------------------
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // Enable DMA1 clock

    DMA_DeInit(mDMA_Rx);
    DMA_DeInit(mDMA_Tx);


    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_M2M  = DMA_M2M_Disable;            // M2M Disabled- Peripheral mode (requires timer trigger)
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;            // Normal mode
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;    // Medium priority
    DMA_InitStruct.DMA_DIR      = DMA_DIR_PeripheralDST;  // Memory to Peripheral

    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     // 8-bit Register
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       // Always write to same register
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32)&mSPI->DR;       // Output data for SPI peripheral

    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             // 8-bit array
    DMA_InitStruct.DMA_MemoryInc      = DMA_MemoryInc_Enable;                // No Increment through array
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;                                   // Initialize later

    DMA_InitStruct.DMA_BufferSize = 1;                                       // Initialize later

    DMA_Init(mDMA_Tx, &DMA_InitStruct);                // Initialize TX DMA


    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_M2M  = DMA_M2M_Disable;            // M2M Disabled- Peripheral mode (requires timer trigger)
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;            // Normal mode
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;    // Medium priority
    DMA_InitStruct.DMA_DIR      = DMA_DIR_PeripheralSRC;  // Memory to Peripheral

    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     // 8-bit Register
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       // Always write to same register
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32)&mSPI->DR;       // Output data for SPI peripheral

    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             // 8-bit array
    DMA_InitStruct.DMA_MemoryInc      = DMA_MemoryInc_Enable;                // Increment through array
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;                                   // Initialize later

    DMA_InitStruct.DMA_BufferSize = 1;                                       // Initialize later

    DMA_Init(mDMA_Rx, &DMA_InitStruct);            // Initialize RX DMA

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    DMA_ITConfig(DMA1_Channel2, /*DMA_IT_TE |*/ DMA_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(DMA1_Channel3, /*DMA_IT_TE |*/ DMA_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_Init(&NVIC_InitStructure);
  }

  //Disable + All interrupts dissable
  #define cNRF905_Spi_vStopDMA() DMA1_Channel2->CCR  &= ~15; DMA1_Channel3->CCR &= ~15; DMA1->IFCR = (DMA1_FLAG_GL2 | DMA1_FLAG_HT2 | DMA1_FLAG_TC2 | DMA1_FLAG_GL3 | DMA1_FLAG_HT3 | DMA1_FLAG_TC3)

  void vStartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
  {
    cNRF905_Spi_vStopDMA();
    /* Initialize the DMA with the new parameters */
    if (Direction == SPI_DIRECTION_TX)
    {
      DMA1_Channel3->CNDTR = BufferSize;
      DMA1_Channel3->CMAR  = (uint32)pBuffer;
      DMA1_Channel3->CCR |= (DMA_MemoryInc_Enable | DMA_CCR_TCIE | DMA_CCR_EN); // Memory Inc
    }
    else /* Reception */
    {
      vClearRxBuf();

      DMA1_Channel3->CNDTR = BufferSize;
      DMA1_Channel3->CMAR = (uint32)(&mui8Dummy);
      DMA1_Channel3->CCR &= (0xFFFF & (~DMA_MemoryInc_Enable)); // No Memory Inc

      DMA1_Channel2->CNDTR = BufferSize;
      DMA1_Channel2->CMAR  = (uint32)pBuffer;

      DMA1_Channel2->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
      DMA1_Channel3->CCR |= DMA_CCR_EN;
    }
  }


  inline void vWaitBussy()
  {
    while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_BSY));
  }


  inline void vClearRxBuf()
  {
    // FIFO Buffer is 4 Byte
    volatile u8 lu8Dummy;
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)SPI1 + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)SPI1 + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)SPI1 + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)SPI1 + 0x0C);
    UNUSED(lu8Dummy);
  }

  void vWrite(uint8 *lui8Data, uint32 lui32Len)
  {
    while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_TXE) == RESET); // TX Empty (Front Test)

    while (lui32Len >= 1)
    {
      SPI_SendData8(mSPI, *lui8Data);
      lui8Data++;
      lui32Len--;
      vWaitBussy();
    }
  }

  void vRead(uint8 *lui8Data, uint32 lui32Len)
  {
    vClearRxBuf();

    while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_TXE) == RESET); // TX Empty (Front Test)

    while (lui32Len >= 1)
    {
      SPI_SendData8(mSPI, 0xFF);
      vWaitBussy();
      while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_RXNE) == RESET); //
      *lui8Data = SPI_ReceiveData8(mSPI);
      lui8Data++;
      lui32Len--;
    }
  }
};

class cNRF905_Timer
{
  public:

  cNRF905_Timer()
  {
    vInit();
  }

  void vStart(u16 lui16Time_us)
  {
    TIM_SetCounter(TIM4, lui16Time_us);
    TIM_Cmd(TIM4, ENABLE);
  }

  void vStop()
  {
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM_Cmd(TIM4, DISABLE);
  }

  void vInit(void)
  {
    TIM_TimeBaseInitTypeDef lstTim;
    NVIC_InitTypeDef NVIC_InitStructure;

    cClockInfo::Update();

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructInit(&lstTim);
    lstTim.TIM_Prescaler = (cClockInfo::mstClocks.TIM1CLK_Frequency / 1000000) -1;  // 1us
    lstTim.TIM_CounterMode = TIM_CounterMode_Down;
    lstTim.TIM_Period = 0xFFFF;

    lstTim.TIM_ClockDivision = TIM_CKD_DIV1;
    lstTim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &lstTim);
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag


    // Enable the TIM4 global Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // lowest priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;        // lowest priority
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable the TIM4 Interrupt Request
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  }
};

class cNRF905_Pins
{
  public:

  //Outputs
  tcGPin<GPIOA_BASE, 3> mTRx_Cn; // Chip select
  tcGPin<GPIOA_BASE, 2>  mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  tcGPin<GPIOC_BASE, 13>  mPWR;    // Power
  tcGPin<GPIOA_BASE, 1>  mCS_SPI; // Chip select SPI

  //Inputs
  tcGPin<GPIOC_BASE, 14>  mCD;  // Carrier Detected
  tcGPin<GPIOB_BASE, 3> mAM;  // Adress Match
  tcGPin<GPIOC_BASE, 15> mDR;  // Data Ready*/

  cNRF905_Pins()
    : mTRx_Cn(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mTx_En( GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mPWR(   GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mCS_SPI(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 1),

      mCD( GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mAM( GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mDR( GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0)
  {
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // Enable clock for SYSCFG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


    // Tell system that you will use PC15 for EXTI_Line15
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);

    // PC15 is connected to EXTI_Line15
    EXTI_InitStruct.EXTI_Line = EXTI_Line15;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStruct);

    // Add IRQ vector to NVIC
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
  };

  inline void vEnableIrqDR()
  {
    EXTI->IMR |= (1<<15);
  }

  inline void vDisableIrqDR()
  {
    EXTI->IMR &= ~(1<<15);
    EXTI_ClearITPendingBit(EXTI_Line15);
  }
};


#endif  //_WIN32

#endif /* NRF905_SPI_H_ */

