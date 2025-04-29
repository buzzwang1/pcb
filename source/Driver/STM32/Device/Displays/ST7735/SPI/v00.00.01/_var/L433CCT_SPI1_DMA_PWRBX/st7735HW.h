
#ifndef __ST7735__HW_H__
#define __ST7735__HW_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_spi.h"

#include "ClockInfo.h"
#include "tGpPin.h"

class cST7735_HW
{
  public: 

  SPI_TypeDef* mSpi;
  DMA_Channel_TypeDef* mDMA_Tx;

  tcGpPin<GPIOA_BASE, 5> mSCK;
  tcGpPin<GPIOA_BASE, 7> mMOSI;
  tcGpPin<GPIOA_BASE, 2> mPinA0;  // ST7735 A0 (Data/Command select) pin
  tcGpPin<GPIOA_BASE, 3> mPinRst; // ST7735 RST (Reset) pin
  // tcGpPin<GPIOA_BASE, 4> mPinCs;  // ST7735 CS (Chip Select) pin
  tcGpPin<GPIOA_BASE, 1> mPinCs;  // ST7735 CS (Chip Select) pin

  cST7735_HW()
    : mSCK( GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),

      mPinA0( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinRst(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinCs( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    __HAL_RCC_SPI1_CLK_ENABLE();

    mSpi = SPI1;
    mDMA_Tx  = DMA1_Channel3;

    mSCK.vSetAF(GPIO_MODE_AF_PP,  GPIO_AF5_SPI1);  // SCK
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MOSI

    // Configure and enable SPI
    cClockInfo::Update();

    LL_SPI_InitTypeDef SPI;
    SPI.Mode              = LL_SPI_MODE_MASTER;
    SPI.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
    SPI.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
    SPI.ClockPolarity     = LL_SPI_POLARITY_LOW;
    SPI.ClockPhase        = LL_SPI_PHASE_1EDGE;
    SPI.NSS               = LL_SPI_NSS_SOFT;  
    SPI.BitOrder          = LL_SPI_MSB_FIRST;
    SPI.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
    SPI.CRCPoly           = 7;

    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 80000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_8; break;     //80Mhz   /   8 =>  10.0 Mhz
      case 16000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_2;  break;    //16Mhz   /   2 =>   8.0 Mhz
      default: SPI.BaudRate = SPI_BAUDRATEPRESCALER_4; break;
    }

    LL_SPI_Init(mSpi, &SPI);  
    LL_SPI_Enable(mSpi);

    mSpi->CR2 |= (SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

    //----------------------------------------------------------
    DMA_HandleTypeDef lhDma = {};

    __HAL_RCC_DMA1_CLK_ENABLE();

    lhDma.Instance = mDMA_Tx;

    /* Configure DMA Channel data length */
    lhDma.Instance->CNDTR = 0;
    /* Configure DMA Channel destination address */
    lhDma.Instance->CPAR = (uint32_t)&(mSpi->DR);
    /* Configure DMA Channel source address */
    lhDma.Instance->CMAR = 0;

    lhDma.Init.Request    = DMA_REQUEST_1;
    lhDma.Init.Direction  = DMA_MEMORY_TO_PERIPH;
    lhDma.Init.PeriphInc  = DMA_PINC_DISABLE;
    lhDma.Init.MemInc     = DMA_MINC_ENABLE;
    lhDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    lhDma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDma.Init.Mode     = DMA_NORMAL;
    lhDma.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&lhDma) != HAL_OK)
    {
      //Error_Handler();
    }
    
    __HAL_DMA_ENABLE_IT(&lhDma, (DMA_IT_TC /*| DMA_IT_HT | DMA_IT_TE*/));
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 8, 8);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  };


  bool isReady()
  {
    //return (((DMA1->ISR & DMA_ISR_TCIF3) == 0) && (!LL_SPI_IsActiveFlag_BSY(mSpi)));
    return ((DMA1_Channel3->CNDTR == 0) && (!LL_SPI_IsActiveFlag_BSY(mSpi)));
  }

  void write(uint8 *lui8Data, uint32 lui32Len)
  {
    while (lui32Len >= 1)
    {
      write(*lui8Data++);
      lui32Len--;
    }
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
  }

  void writeDma(uint8 *lui8Data, uint32 lui32Len)
  {
    DMA1_Channel3->CCR &= ~15; 
    DMA1->IFCR = (DMA_IFCR_CGIF3 | DMA_IFCR_CHTIF3 | DMA_IFCR_CTCIF3);

    DMA1_Channel3->CNDTR = lui32Len;
    DMA1_Channel3->CMAR  = (uint32)lui8Data;
    DMA1_Channel3->CCR |= (DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN); // Memory Inc
  }

  void write(uint8_t data) 
  {
    while (!LL_SPI_IsActiveFlag_TXE(mSpi));
    LL_SPI_TransmitData8(mSpi, data);
  }

  void cmd(uint8_t cmd) 
  {
    mPinA0.vSet0();
    write(cmd);
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
  }

  void data(uint8_t data) {
    mPinA0.vSet1();
    write(data);
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
  }

  void cmd2(uint8_t cmd) 
  {
    mPinCs.vSet0();
    mPinA0.vSet0();
    write(cmd);
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
    mPinCs.vSet1();
  }

  void data2(uint8_t data) {
    mPinCs.vSet0();
    mPinA0.vSet1();
    write(data);
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
    mPinCs.vSet1();
  }
};



#endif // __ST7735__HW_H__
