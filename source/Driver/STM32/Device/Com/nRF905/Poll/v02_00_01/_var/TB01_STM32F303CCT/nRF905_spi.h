#ifndef NRF905_SPI_H_
#define NRF905_SPI_H_

#ifndef _WIN32

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_dma.h"


class cNRF905_SpiPins
{
  public:

  //Outputs
  cGPPIN mSCK;
  cGPPIN mMISO;
  cGPPIN mMOSI;

  cNRF905_SpiPins()
    : mSCK( RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_5, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMISO(RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_6, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMOSI(RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_7, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0)
  {
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5); // SCK
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

  cNRF905_Spi();

  void vWaitBussy();
  void vClearRxBuf();
  void vDmaWrite(uint8 *lui8Data, uint32 lui32Len);
  void vDmaRead(uint8 *lui8Data, uint32 lui32Len);
  void vWrite(uint8 *lui8Data, uint32 lui32Len);
  void vRead(uint8 *lui8Data, uint32 lui32Len);
};


#endif  //_WIN32

#endif /* NRF905_SPI_H_ */

