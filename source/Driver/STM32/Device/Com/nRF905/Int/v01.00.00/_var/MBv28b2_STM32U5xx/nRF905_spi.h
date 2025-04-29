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

  u8    mu8DmaChRx;
  u8    mu8DmaChTx;
  DMA_Channel_TypeDef* mDmaChRx;
  DMA_Channel_TypeDef* mDmaChTx;

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

    mSPI->CR1  |= (SPI_CR1_SPE);
    mSPI->CFG1 |= (SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

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




    //__HAL_DMA_ENABLE_IT(&lhDma, (DMA_IT_TC /*| DMA_IT_HT | DMA_IT_TE*/));


    IRQn_Type lenDmaIrqChnl = GPDMA1_Channel0_IRQn;

    switch (mu8DmaChRx)
    {
      case  0: lenDmaIrqChnl = GPDMA1_Channel0_IRQn; break;
      case  1: lenDmaIrqChnl = GPDMA1_Channel1_IRQn; break;
      case  2: lenDmaIrqChnl = GPDMA1_Channel2_IRQn; break;
      case  3: lenDmaIrqChnl = GPDMA1_Channel3_IRQn; break;
      case  4: lenDmaIrqChnl = GPDMA1_Channel4_IRQn; break;
      case  5: lenDmaIrqChnl = GPDMA1_Channel5_IRQn; break;
      case  6: lenDmaIrqChnl = GPDMA1_Channel6_IRQn; break;
      case  7: lenDmaIrqChnl = GPDMA1_Channel7_IRQn; break;
      case  8: lenDmaIrqChnl = GPDMA1_Channel8_IRQn; break;
      case  9: lenDmaIrqChnl = GPDMA1_Channel9_IRQn; break;
      case 10: lenDmaIrqChnl = GPDMA1_Channel10_IRQn; break;
      case 11: lenDmaIrqChnl = GPDMA1_Channel11_IRQn; break;
      case 12: lenDmaIrqChnl = GPDMA1_Channel12_IRQn; break;
      case 13: lenDmaIrqChnl = GPDMA1_Channel13_IRQn; break;
      case 14: lenDmaIrqChnl = GPDMA1_Channel14_IRQn; break;
      case 15: lenDmaIrqChnl = GPDMA1_Channel15_IRQn; break;
    }
    HAL_NVIC_SetPriority(lenDmaIrqChnl, 9, 8); // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(lenDmaIrqChnl);

    switch (mu8DmaChTx)
    {
      case  0: lenDmaIrqChnl = GPDMA1_Channel0_IRQn; break;
      case  1: lenDmaIrqChnl = GPDMA1_Channel1_IRQn; break;
      case  2: lenDmaIrqChnl = GPDMA1_Channel2_IRQn; break;
      case  3: lenDmaIrqChnl = GPDMA1_Channel3_IRQn; break;
      case  4: lenDmaIrqChnl = GPDMA1_Channel4_IRQn; break;
      case  5: lenDmaIrqChnl = GPDMA1_Channel5_IRQn; break;
      case  6: lenDmaIrqChnl = GPDMA1_Channel6_IRQn; break;
      case  7: lenDmaIrqChnl = GPDMA1_Channel7_IRQn; break;
      case  8: lenDmaIrqChnl = GPDMA1_Channel8_IRQn; break;
      case  9: lenDmaIrqChnl = GPDMA1_Channel9_IRQn; break;
      case 10: lenDmaIrqChnl = GPDMA1_Channel10_IRQn; break;
      case 11: lenDmaIrqChnl = GPDMA1_Channel11_IRQn; break;
      case 12: lenDmaIrqChnl = GPDMA1_Channel12_IRQn; break;
      case 13: lenDmaIrqChnl = GPDMA1_Channel13_IRQn; break;
      case 14: lenDmaIrqChnl = GPDMA1_Channel14_IRQn; break;
      case 15: lenDmaIrqChnl = GPDMA1_Channel15_IRQn; break;
    }
    HAL_NVIC_SetPriority(lenDmaIrqChnl, 9, 8); // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(lenDmaIrqChnl);
  }

  //Disable + All interrupts disable
  #define cNRF905_Spi_vStopDMA() mDmaChRx->CCR  &= ~0x0000FF01; mDmaChTx->CCR &= ~0x0000FF01; mDmaChRx->CFCR = 0xFFFF; mDmaChTx->CFCR = 0xFFFF;

  void vStartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
  {
    cNRF905_Spi_vStopDMA();
    /* Initialize the DMA with the new parameters */
    if (Direction == SPI_DIRECTION_TX)
    {
      mDmaChTx->CBR1 = BufferSize;
      mDmaChTx->CSAR = (uint32)pBuffer;
      mDmaChTx->CTR1|= DMA_CTR1_SINC;  // Memory Inc
      mDmaChTx->CCR |= (DMA_CCR_TCIE | DMA_CCR_EN);
    }
    else /* Reception */
    {
      vClearRxBuf();

      mDmaChTx->CBR1 = BufferSize;
      mDmaChTx->CSAR = (uint32)(&mui8Dummy);
      mDmaChTx->CTR1&= ((u32)(~DMA_CTR1_SINC)); // No Memory Inc

      mDmaChRx->CBR1 = BufferSize;
      mDmaChRx->CDAR = (uint32)pBuffer;
      mDmaChRx->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
    }
  }


  inline void vWaitBussy()
  {
    //while(mSPI->SR & SPI_FLAG_BSY);
  }


  inline void vClearRxBuf()
  {
    // SPI1&2 FIFO Buffer is 16 Byte
    // SPI3 FIFO Buffer is 8 Byte
    // volatile u8 lu8Dummy;
    // lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    // lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    // lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    // lu8Dummy = *(__IO uint8_t *) ((uint32_t)mSPI + 0x0C);
    // UNUSED(lu8Dummy);
  }

  void vWrite(uint8 *lui8Data, uint32 lui32Len)
  {
    static u8* spiDrPtr = (uint8_t*)&SPI1->TXDR;

    while((mSPI->SR & SPI_FLAG_TXP) == 0); // TX Empty (Front Test)

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
    static u8* spiRxDrPtr = (uint8_t*)&SPI1->RXDR;
    static u8* spiTxDrPtr = (uint8_t*)&SPI1->TXDR;
    vClearRxBuf();

    while((mSPI->SR & SPI_FLAG_TXP) == 0); // TX Empty (Front Test)

    while (lui32Len >= 1)
    {
      *spiTxDrPtr = 0xFF;
      vWaitBussy();
      while((mSPI->SR & SPI_FLAG_RXP) == 0); // TX Empty (Front Test)
      *lui8Data = *spiRxDrPtr;
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
  tcGpPin<GPIOE_BASE,  6> mTRx_Cn; // Chip select
  tcGpPin<GPIOB_BASE,  8> mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  tcGpPin<GPIOE_BASE,  1> mPWR;    // Power
  tcGpPin<GPIOE_BASE,  0> mCS_SPI; // Chip select SPI

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
    ////LL_EXTI_InitTypeDef EXTI_InitStruct;
    ////
    ////// Enable clock for SYSCFG
    ////__HAL_RCC_SYSCFG_CLK_ENABLE();
    ////
    ////// Tell system that you will use PB13 for EXTI_Line13
    ////SYSCFG->EXTICR[3] = 0x0010;
    //////SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);
    ////
    ////// PB13 is connected to EXTI_Line13
    ////EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
    ////EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    ////EXTI_InitStruct.LineCommand = ENABLE;
    ////EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    ////EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    ////LL_EXTI_Init(&EXTI_InitStruct);
    ////
    ////// Add IRQ vector to NVIC
    ////HAL_NVIC_SetPriority(EXTI15_10_IRQn, 9, 8);  // Niedere Prio, wegen busy waiting
    ////HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  }

  inline void vEnableIrqDR()
  {
    ////EXTI->IMR1 |= (1<<15);
  }

  inline void vDisableIrqDR()
  {
    ////EXTI->IMR1 &= ~(1<<15);
    ////LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_15);
  }
};


#endif  //_WIN32

#endif /* NRF905_SPI_H_ */

