
#ifndef __HX8347__HW_H__
#define __HX8347__HW_H__


#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_spi.h"

#include "ClockInfo.h"
#include "tGpPin.h"

class cHx8347_HW
{
  public: 

  SPI_TypeDef* mSpi;
  DMA_Channel_TypeDef* mDMA_Tx;
  DMA_Channel_TypeDef* mDMA_Rx;

  tcGpPin<GPIOB_BASE, 13> mSCK;
  tcGpPin<GPIOB_BASE, 14> mMISO;
  tcGpPin<GPIOB_BASE, 15> mMOSI;

  tcGpPin<GPIOB_BASE, 12> mPinCs;
  tcGpPin<GPIOA_BASE, 8>  mPinPs;
  tcGpPin<GPIOA_BASE, 9>  mPinRst;
  tcGpPin<GPIOB_BASE, 1>  mPinPwr;

  uint8 mui8Dummy; // Dummy für Rx

  cHx8347_HW()
    : mSCK( GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMISO(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),

      mPinCs( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1),
      mPinPs( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinRst(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinPwr(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    __HAL_RCC_SPI2_CLK_ENABLE();

    mSpi = SPI2;
    mDMA_Tx = DMA1_Channel5;
    mDMA_Rx = DMA1_Channel4;

    mSCK.vSetAF(GPIO_MODE_AF_PP,  GPIO_AF5_SPI2);  // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // MOSI

    mui8Dummy = 0x55;

    // Configure and enable SPI
    cClockInfo::Update();

    LL_SPI_InitTypeDef SPI;
    SPI.Mode              = LL_SPI_MODE_MASTER;

    /* SD_SPI Config */
    SPI.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
    SPI.ClockPolarity     = LL_SPI_POLARITY_HIGH;
    SPI.ClockPhase        = LL_SPI_PHASE_2EDGE;
    SPI.NSS               = LL_SPI_NSS_SOFT;  
    SPI.BitOrder          = LL_SPI_MSB_FIRST;
    SPI.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
    SPI.CRCPoly           = 7;

    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 80000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_16; break;    //80Mhz   /  16 =>   5.0 Mhz
      case 16000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_4;  break;    //16Mhz   /   4 =>   4.0 Mhz
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
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 8, 8);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  };


  inline void vPwrDisable() {mPinPwr.vSet0();};
  inline void vPwrEnable()  {mPinPwr.vSet1();};
  inline void vCsDisable()  {mPinCs.vSet1(); };
  inline void vCsEnable()   {mPinCs.vSet0(); };
  inline void vParallel()    {mPinPs.vSet1(); };
  inline void vSerial()      {mPinPs.vSet0(); };
  inline void vRstDisable()  {mPinRst.vSet1(); mPinRst.vSet0();}
  inline void vRstEnable()   {mPinRst.vSet1(); mPinRst.vSet1();}

  void vWait()
  {
    cClockInfo::Delay_us(25);
  }

  bool isReady()
  {
    return ((DMA1_Channel5->CNDTR == 0) && (!LL_SPI_IsActiveFlag_BSY(mSpi)));
  }

  inline void vWaitBussy()
  {
    while (LL_SPI_IsActiveFlag_BSY(mSpi));
  }


  void write(uint8 *lui8Data, uint32 lui32Len)
  {
    while (lui32Len >= 1)
    {
      write(*lui8Data++);
      lui32Len--;
    }
    vWaitBussy();
  }

  void writeDma(uint8 *lui8Data, uint32 lui32Len)
  {
    DMA1_Channel5->CCR &= ~15; 
    DMA1->IFCR = (DMA_IFCR_CGIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTCIF5);

    DMA1_Channel5->CNDTR = lui32Len;
    DMA1_Channel5->CMAR  = (uint32)lui8Data;
    DMA1_Channel5->CCR |= (DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN); // Memory Inc
  }

  void write(u8 data) 
  {
    while (!LL_SPI_IsActiveFlag_TXE(mSpi));
    LL_SPI_TransmitData8(mSpi, data);
  }

};



#endif // __HX8347__HW_H__
