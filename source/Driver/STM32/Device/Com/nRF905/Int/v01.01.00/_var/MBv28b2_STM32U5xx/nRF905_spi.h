#ifndef NRF905_SPI_H_
#define NRF905_SPI_H_

#ifndef _WIN32

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#include "stm32u5xx_ll_spi.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_exti.h"

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
    : mSCK( GPIO_MODE_AF_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMISO(GPIO_MODE_AF_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIO_MODE_AF_PP, GPIO_PULLDOWN, GPIO_SPEED_FREQ_VERY_HIGH, 0)
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

  uint8 mui8Dummy; // Dummy für Rx
  u8    mu8ModeTx;

  u8    mu8DmaChRx;
  u8    mu8DmaChTx;
  DMA_Channel_TypeDef* mDmaChRx;
  DMA_Channel_TypeDef* mDmaChTx;

  cNRF905_Spi()
  : mPins()
  {
    vInit();
  }

  void vInit()
  {
    SPI_HandleTypeDef lhSpi = {};

    mSPI = SPI1;
    mu8DmaChRx = 8;
    mu8DmaChTx = 9;
    mDmaChRx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[mu8DmaChRx]));
    mDmaChTx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[mu8DmaChTx]));

    mui8Dummy = 0x55;

    __HAL_RCC_SPI1_CLK_ENABLE();

    /* SD_SPI Config */
    lhSpi.Instance         = mSPI;
    lhSpi.Init.Mode        = SPI_MODE_MASTER;
    lhSpi.Init.Direction   = SPI_DIRECTION_2LINES;
    lhSpi.Init.DataSize    = SPI_DATASIZE_8BIT;
    lhSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    lhSpi.Init.CLKPhase    = SPI_PHASE_1EDGE;
    lhSpi.Init.NSS         = SPI_NSS_SOFT;
    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 80000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; break;     //80Mhz   /  16 =>   5.0 Mhz
      case 32000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  break;     //32Mhz   /   8 =>   4.0 Mhz
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

    mSPI->CFG1 |= (SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);
    //mSPI->CFG2 |= (0xF << SPI_CFG2_MIDI_Pos); // verzögert EOT

    HAL_NVIC_SetPriority(SPI1_IRQn, 9, 8); // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(SPI1_IRQn);

    //----------------------------------------------------------

    __HAL_RCC_GPDMA1_CLK_ENABLE();

    /* DMA node configuration declaration */
    LL_DMA_InitTypeDef           lstDmaInit;

    LL_DMA_StructInit(&lstDmaInit);

    lstDmaInit.BlkHWRequest  = LL_DMA_HWREQUEST_SINGLEBURST;
    lstDmaInit.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;

    lstDmaInit.SrcBurstLength  = 1;
    lstDmaInit.DestBurstLength = 1;

    lstDmaInit.SrcDataWidth  = LL_DMA_SRC_DATAWIDTH_BYTE;
    lstDmaInit.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;

    lstDmaInit.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
    lstDmaInit.BlkDataLength = 0;

    lstDmaInit.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;

    // -Port 0 should be allocated for transfers toand from
    //  peripherals, because there is a direct hardware datapath
    //  between this portand the APB peripherals, outside the  AHB matrix
    // -Port 1 should be allocated for transfers toand from
    //  memory, which are performed through the AHB interconnect.

    if (mSPI == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_RX;
    if (mSPI == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_RX;
    if (mSPI == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_RX;
    //if (mSPI == SPI4) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI4_RX;
    lstDmaInit.SrcAddress  = (u32)&(mSPI->RXDR);
    lstDmaInit.DestAddress = 0;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_FIXED;
    lstDmaInit.DestIncMode = LL_DMA_DEST_INCREMENT;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT0; // Source is periph
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1; // Dest is memory
    LL_DMA_Init(GPDMA1, mu8DmaChRx, &lstDmaInit);


    if (mSPI == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_TX;
    if (mSPI == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_TX;
    if (mSPI == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_TX;
    //if (mSPI == SPI4) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI4_TX;
    lstDmaInit.SrcAddress  = 0;
    lstDmaInit.DestAddress = (u32) & (mSPI->TXDR);
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_INCREMENT;
    lstDmaInit.DestIncMode = LL_DMA_DEST_FIXED;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT1; // Source is memory
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0; // Dest is periph
    LL_DMA_Init(GPDMA1, mu8DmaChTx, &lstDmaInit);
  }

  //Disable + All interrupts disable
  #define cNRF905_Spi_vStopDMA() mDmaChRx->CCR  &= ~0x0000FF01; mDmaChTx->CCR &= ~0x0000FF01; mDmaChRx->CFCR = 0xFFFF; mDmaChTx->CFCR = 0xFFFF;

  void vStartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
  {
    cNRF905_Spi_vStopDMA();

    SPI1->IFCR = 0xFFFF; // Clear all flags
    MODIFY_REG(SPI1->CR2, SPI_CR2_TSIZE, BufferSize);

    // Disable => Enable sollte Fifo flushen
    SPI1->CR1 |= (SPI_CR1_SPE);
    SPI1->CR1 |= SPI_CR1_CSTART;
    SPI1->IER |= SPI_IER_EOTIE;


    /* Initialize the DMA with the new parameters */
    if (Direction == SPI_DIRECTION_TX)
    {
      mu8ModeTx = 1;

      mDmaChTx->CBR1  = BufferSize;
      mDmaChTx->CSAR  = (uint32)pBuffer;
      mDmaChTx->CTR1 |= DMA_CTR1_SINC;  // Memory Inc
      mDmaChTx->CCR  |= (DMA_CCR_EN);
    }
    else /* Reception */
    {
      mu8ModeTx = 0;

      mDmaChTx->CBR1 = BufferSize;
      mDmaChTx->CSAR = (uint32)(&mui8Dummy);
      mDmaChTx->CTR1&= ((u32)(~DMA_CTR1_SINC)); // No Memory Inc

      mDmaChRx->CBR1 = BufferSize;
      mDmaChRx->CDAR = (uint32)pBuffer;
      mDmaChRx->CCR |= (DMA_CCR_EN);
      mDmaChTx->CCR |= (DMA_CCR_EN);
    }
  }


  inline void vWaitBussy()
  {
  }

  inline void vWaitEOT()
  {
    while ((SPI1->SR & SPI_FLAG_EOT) == 0);
    SPI1->IFCR = 0xFFFF;
  }


  inline void vClearRxBuf()
  {
    // Disable/Enable flushes the Fifos
    // CLEAR_BIT(SPI1->CR1 , SPI_CR1_SPE);
    // SET_BIT(SPI1->CR1 , SPI_CR1_SPE);
    while ((SPI1->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)))
    {
      LL_SPI_ReceiveData8(SPI1);
    }
  }

  void vWrite(uint8 *lui8Data, uint32 lui32Len)
  {
    SPI1->IER &= ~SPI_IER_EOTIE;
    MODIFY_REG(SPI1->CR2, SPI_CR2_TSIZE, lui32Len);
    SPI1->CR1 |= (SPI_CR1_SPE);
    LL_SPI_StartMasterTransfer(SPI1);

    while (lui32Len)
    {
      while((SPI1->SR & SPI_FLAG_TXP) == 0); // Tx Fifo has space ?
      LL_SPI_TransmitData8(SPI1, *lui8Data);
      lui8Data++;
      lui32Len--;
    }
    vWaitEOT();
    SPI1->CR1 &= ~(SPI_CR1_SPE);
  }

  void vRead(uint8 *lui8Data, uint32 lui32RxLen)
  {
    u32 lui32TxLen = lui32RxLen;

    SPI1->IER &= ~SPI_IER_EOTIE;
    MODIFY_REG(SPI1->CR2, SPI_CR2_TSIZE, lui32RxLen);
    SPI1->CR1 |= (SPI_CR1_SPE);
    vClearRxBuf();
    LL_SPI_StartMasterTransfer(SPI1);

    while ((lui32TxLen) || (lui32RxLen))
    {
      if ((SPI1->SR & SPI_FLAG_TXP) && (lui32TxLen))
      {
        LL_SPI_TransmitData8(SPI1, 0xFF);
        lui32TxLen--;
      }

      if ((SPI1->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) && (lui32RxLen))
      {
        *lui8Data = LL_SPI_ReceiveData8(SPI1);
        lui8Data++;
        lui32RxLen--;
      }
    }
    vWaitEOT();
    SPI1->CR1 &= ~(SPI_CR1_SPE);
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
  tcGpPin<GPIOE_BASE,  0> mTRx_Cn; // Chip select
  tcGpPin<GPIOB_BASE,  8> mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  tcGpPin<GPIOE_BASE,  1> mPWR;    // Power
  tcGpPin<GPIOE_BASE,  6> mCS_SPI; // Chip select SPI

  //Inputs
  tcGpPin<GPIOE_BASE,  3> mCD;  // Carrier Detected // CD and AM are n.A. So just put it to DR.
  tcGpPin<GPIOE_BASE,  4> mAM;  // Adress Match
  tcGpPin<GPIOE_BASE,  5> mDR;  // Data Ready*/

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
    // DR (Data Ready) is connected to PE05

    // Tell system that you will use PE05 for EXTI_Line5
    // EXTI->EXTICR[0] EXTI0..3
    // EXTI->EXTICR[0] EXTI4..7
    // EXTI->EXTICR[0] EXTI8..11
    // EXTI->EXTICR[0] EXTI12..15
    //
    //
    // EXTI_Line5 => EXTI->EXTICR[1] 2nd Byte
    // PA = 0; PB = 1, PC = 2, PD = 3
    // => PE = 4

    //u32 lu32Temp;
    //lu32Temp  = EXTI->EXTICR[1] & 0xFFFF00FF;
    //lu32Temp &= 4 << 8;
    //EXTI->EXTICR[1] = lu32Temp;

    LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTE, LL_EXTI_EXTI_LINE5);

    ////
    ////// PE05 is connected to EXTI_Line5
    LL_EXTI_InitTypeDef EXTI_InitStruct;
    LL_EXTI_StructInit(&EXTI_InitStruct);

    EXTI_InitStruct.Line_0_31   = LL_EXTI_LINE_5;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    // Add IRQ vector to NVIC
    HAL_NVIC_SetPriority(EXTI5_IRQn, 9, 8);  // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(EXTI5_IRQn);
  }

  inline void vEnableIrqDR()
  {
    EXTI->IMR1 |= (1<<5);
  }

  inline void vDisableIrqDR()
  {
    EXTI->IMR1 &= ~(1<<5);
    LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_5);
  }
};


#endif  //_WIN32

#endif /* NRF905_SPI_H_ */

