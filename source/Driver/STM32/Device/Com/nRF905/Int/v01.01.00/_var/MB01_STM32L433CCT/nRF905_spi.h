#ifndef NRF905_SPI_H_
#define NRF905_SPI_H_

#ifndef _WIN32

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_dma.h"

#include "ClockInfo.h"


#define SPI_DIRECTION_TX 0
#define SPI_DIRECTION_RX 1

class cNRF905_SpiPins
{
  public:

  //SPI1 Outputs
  tcGpPin<GPIOB_BASE, 3> mSCK;
  tcGpPin<GPIOB_BASE, 4> mMISO;
  tcGpPin<GPIOB_BASE, 5> mMOSI;

  cNRF905_SpiPins()
    : mSCK( GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMISO(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    vInit();
  };

  void vInit()
  {
    mSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MOSI
  }
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
    vInit();
  }

  void vInit()
  {
    SPI_HandleTypeDef lhSpi = {};

    mSPI = SPI1;
    mDMA_Tx = DMA1_Channel3;
    mDMA_Rx = DMA1_Channel2;

    mui8Dummy = 0x55;

    __HAL_RCC_SPI1_CLK_ENABLE();

    /* SD_SPI Config */
    lhSpi.Instance = mSPI;
    lhSpi.Init.Mode = SPI_MODE_MASTER;
    lhSpi.Init.Direction = SPI_DIRECTION_2LINES;
    lhSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    lhSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    lhSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    lhSpi.Init.NSS = SPI_NSS_SOFT;
    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 80000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; break;     //80Mhz   /  16 =>   5.0 Mhz
      case 16000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  break;     //16Mhz   /   8 =>   2.0 Mhz
      default: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; break;
    }

    lhSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lhSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    lhSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    lhSpi.Init.CRCPolynomial = 7;
    lhSpi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    lhSpi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(&lhSpi) != HAL_OK)
    {
      //Error_Handler();
    }

    mSPI->CR1 |= (SPI_CR1_SPE);

    mSPI->CR2 |= (SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

    //----------------------------------------------------------
    DMA_HandleTypeDef lhDma = {};

    __HAL_RCC_DMA1_CLK_ENABLE();

    lhDma.Instance = mDMA_Rx;

    /* Configure DMA Channel data length */
    lhDma.Instance->CNDTR = 1;
    /* Configure DMA Channel destination address */
    lhDma.Instance->CPAR = (uint32_t) & (mSPI->DR);
    /* Configure DMA Channel source address */
    lhDma.Instance->CMAR = 0;

    lhDma.Init.Request = DMA_REQUEST_1;
    lhDma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    lhDma.Init.PeriphInc = DMA_PINC_DISABLE;
    lhDma.Init.MemInc = DMA_MINC_ENABLE;
    lhDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    lhDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    lhDma.Init.Mode = DMA_NORMAL;
    lhDma.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&lhDma) != HAL_OK)
    {
      //Error_Handler();
    }
    __HAL_DMA_ENABLE_IT(&lhDma, (DMA_IT_TC /*| DMA_IT_HT | DMA_IT_TE*/));

    lhDma.Instance = mDMA_Tx;
    /* Configure DMA Channel data length */
    lhDma.Instance->CNDTR = 1;
    /* Configure DMA Channel destination address */
    lhDma.Instance->CPAR = (uint32_t) & (mSPI->DR);
    /* Configure DMA Channel source address */
    lhDma.Instance->CMAR = 0;
    lhDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    if (HAL_DMA_Init(&lhDma) != HAL_OK)
    {
      //Error_Handler();
    }
    __HAL_DMA_ENABLE_IT(&lhDma, (DMA_IT_TC /*| DMA_IT_HT | DMA_IT_TE*/));

    /* DMA interrupt init */
    /* DMA1_Channel2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 9, 8); // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    /* DMA1_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 9, 8); // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  }

  //Disable + All interrupts dissable
  #define cNRF905_Spi_vStopDMA() mDMA_Rx->CCR  &= ~15; mDMA_Tx->CCR &= ~15; DMA1->IFCR = (DMA_ISR_GIF2 | DMA_ISR_HTIF2 | DMA_ISR_TCIF2 | DMA_ISR_GIF3 | DMA_ISR_HTIF3 | DMA_ISR_TCIF3)

  void vStartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
  {
    cNRF905_Spi_vStopDMA();
    /* Initialize the DMA with the new parameters */
    if (Direction == SPI_DIRECTION_TX)
    {
      DMA1_Channel3->CNDTR = BufferSize;
      DMA1_Channel3->CMAR  = (uint32)pBuffer;
      DMA1_Channel3->CCR |= (DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN); // Memory Inc
    }
    else /* Reception */
    {
      vClearRxBuf();

      DMA1_Channel3->CNDTR = BufferSize;
      DMA1_Channel3->CMAR = (uint32)(&mui8Dummy);
      DMA1_Channel3->CCR &= (0xFFFF & (~DMA_CCR_MINC)); // No Memory Inc

      DMA1_Channel2->CNDTR = BufferSize;
      DMA1_Channel2->CMAR  = (uint32)pBuffer;

      DMA1_Channel2->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
      DMA1_Channel3->CCR |= DMA_CCR_EN;
    }
  }


  inline void vWaitBussy()
  {
    while(mSPI->SR & SPI_FLAG_BSY);
  }


  inline void vClearRxBuf()
  {
    // FIFO Buffer is 4 Byte
    volatile u8 lu8Dummy;
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    UNUSED(lu8Dummy);
  }

  void vWrite(uint8 *lui8Data, uint32 lui32Len)
  {
    static u8* spiDrPtr = (uint8_t*)&SPI1->DR;

    while((mSPI->SR & SPI_FLAG_TXE) == 0); // TX Empty (Front Test)

    while (lui32Len >= 1)
    {
      *spiDrPtr = *lui8Data;
      lui8Data++;
      lui32Len--;
      vWaitBussy();
    }
  }

  void vRead(uint8 *lui8Data, uint32 lui32Len)
  {
    static u8* spiDrPtr = (uint8_t*)&SPI1->DR;
    vClearRxBuf();

    while((mSPI->SR & SPI_FLAG_TXE) == 0); // TX Empty (Front Test)

    while (lui32Len >= 1)
    {
      *spiDrPtr = 0xFF;
      vWaitBussy();
      while((mSPI->SR & SPI_FLAG_RXNE) == 0); // TX Empty (Front Test)
      *lui8Data = *spiDrPtr;
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
    //  Tim7: The counter counts from 0 to the auto-reload value (contents of the TIMx_ARR register),
    //  then restarts from 0 and generates a counter overflow event.
    TIM7->ARR = lui16Time_us;
    TIM7->CNT = 0;

    TIM7->CR1 |= 8; // OPM: One Pulse Mode: Counter stops counting at the next update event (clearing the CEN bit).

    TIM7->CR1 |= TIM_CR1_CEN;
  }

  void vStop()
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }

  void vInit(void)
  {
    TIM_HandleTypeDef TimHandle = {};

    __HAL_RCC_TIM7_CLK_ENABLE();

    cClockInfo::Update();

    TimHandle.Instance = TIM7;
    TimHandle.Init.Period            = 0; // No Auto reload
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) -1; //us
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_DOWN;
    TimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
      /* Initialization Error */
      //Error_Handler();
    }

    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->DIER |= TIM_DIER_UIE;

    // Enable the TIM7 global Interrupt
    HAL_NVIC_SetPriority(TIM7_IRQn, 9, 8);  // Niedere Prio, wegen busy waiting
    /* Enable the TIM7 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
};

class cNRF905_Pins
{
  public:

  //Outputs
  tcGpPin<GPIOA_BASE, 11> mTRx_Cn; // Chip select
  tcGpPin<GPIOB_BASE, 14> mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  tcGpPin<GPIOB_BASE, 15> mPWR;    // Power
  tcGpPin<GPIOA_BASE, 12> mCS_SPI; // Chip select SPI

  //Inputs
  tcGpPin<GPIOB_BASE, 13> mCD;  // Carrier Detected // CD and AM are n.A. So just put it to DR.
  tcGpPin<GPIOB_BASE, 13> mAM;  // Adress Match
  tcGpPin<GPIOB_BASE, 13> mDR;  // Data Ready*/

  cNRF905_Pins()
    : mTRx_Cn(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mTx_En( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPWR(   GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mCS_SPI(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1),

      mCD( GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mAM( GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mDR( GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    vInit();
  };


  void vInit()
  {
    LL_EXTI_InitTypeDef EXTI_InitStruct;

    // Enable clock for SYSCFG
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // Tell system that you will use PB13 for EXTI_Line13
    SYSCFG->EXTICR[3] = 0x0010;
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);

    // PB13 is connected to EXTI_Line13
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
    EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    // Add IRQ vector to NVIC
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 9, 8);  // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  }

  inline void vEnableIrqDR()
  {
    EXTI->IMR1 |= (1<<15);
  }

  inline void vDisableIrqDR()
  {
    EXTI->IMR1 &= ~(1<<15);
    LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_15);
  }
};


#endif  //_WIN32

#endif /* NRF905_SPI_H_ */

