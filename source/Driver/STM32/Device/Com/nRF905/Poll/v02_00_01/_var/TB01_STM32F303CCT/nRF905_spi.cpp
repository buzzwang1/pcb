
#include "nRF905_spi.h"

#ifndef  _WIN32


cNRF905_Spi::cNRF905_Spi()
: mPins()
{
  mSPI     = SPI1;
  mDMA_Tx  = DMA1_Channel3;
  mDMA_Rx  = DMA1_Channel2;

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
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

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
  DMA_InitStruct.DMA_MemoryInc      = DMA_MemoryInc_Disable;               // No Increment through array
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
}


void cNRF905_Spi::vWaitBussy()
{
  while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_BSY));
}

void cNRF905_Spi::vDmaWrite(uint8 *lui8Data, uint32 lui32Len)
{
  // Clear DMA Flags
  DMA_ClearFlag(DMA1_FLAG_GL2 | DMA1_FLAG_HT2 | DMA1_FLAG_TC2 | DMA1_FLAG_GL3 | DMA1_FLAG_HT3 | DMA1_FLAG_TC3);

  mDMA_Tx->CMAR  = (uint32)lui8Data;
  mDMA_Tx->CNDTR = lui32Len;
  mDMA_Tx->CCR  |= DMA_MemoryInc_Enable; // Memory Inc

  // Enable SPI DMA TX Requests
  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Tx, ENABLE);

  // Enable DMA SPI TX Stream
  DMA_Cmd(mDMA_Tx, ENABLE);

   // Waiting the end of Data transfer
   while ((DMA_GetFlagStatus(DMA1_FLAG_TC3)==RESET))
   {
   }

  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Tx, DISABLE);

  DMA_Cmd(mDMA_Tx, DISABLE);
}

void cNRF905_Spi::vDmaRead(uint8 *lui8Data, uint32 lui32Len)
{
  uint8 lui8Dummy = 0xFF;

  // Clear DMA Flags
  DMA_ClearFlag(DMA1_FLAG_GL2 | DMA1_FLAG_HT2 | DMA1_FLAG_TC2 | DMA1_FLAG_GL3 | DMA1_FLAG_HT3 | DMA1_FLAG_TC3);

  mDMA_Tx->CMAR  = (uint32)(&lui8Dummy);
  mDMA_Tx->CNDTR = lui32Len;
  mDMA_Tx->CCR  &= (0xFFFF & (~DMA_MemoryInc_Enable)); // No Memory Inc

  mDMA_Rx->CMAR  = (uint32)lui8Data;
  mDMA_Rx->CNDTR = lui32Len;


  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Rx, ENABLE); // Enable SPI DMA RX Requests
  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Tx, ENABLE); // Enable SPI DMA TX Requests

  DMA_Cmd(mDMA_Rx, ENABLE); // Enable DMA SPI RX Stream
  DMA_Cmd(mDMA_Tx, ENABLE); // Enable DMA SPI TX Stream

   // Waiting the end of Data transfer
   while ((DMA_GetFlagStatus(DMA1_FLAG_TC2)==RESET))
   {
   }

  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Rx, DISABLE);
  SPI_I2S_DMACmd(mSPI, SPI_I2S_DMAReq_Tx, DISABLE);

  DMA_Cmd(mDMA_Rx, DISABLE);
  DMA_Cmd(mDMA_Tx, DISABLE);
}



void cNRF905_Spi::vClearRxBuf()
{
  // Clear RX Buffer
  while (SPI_GetReceptionFIFOStatus(mSPI))
  {
    SPI_ReceiveData8(mSPI);
  }
}

void cNRF905_Spi::vWrite(uint8 *lui8Data, uint32 lui32Len)
{
  while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_TXE) == RESET); // TX Empty (Front Test)

  if (lui32Len == 1)
  {
    SPI_SendData8(mSPI, *lui8Data);
    vWaitBussy();
  }
  else
  if (lui32Len > 1)
  {
    vDmaWrite(lui8Data, lui32Len);
  }
}

void cNRF905_Spi::vRead(uint8 *lui8Data, uint32 lui32Len)
{
  vClearRxBuf();

  while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_TXE) == RESET); // TX Empty (Front Test)

  if (lui32Len == 1)
  {
    SPI_SendData8(mSPI, 0xFF);
    vWaitBussy();
    while(SPI_I2S_GetFlagStatus(mSPI, SPI_I2S_FLAG_RXNE) == RESET); //
    *lui8Data = SPI_ReceiveData8(mSPI);
  }
  else
  if (lui32Len > 1)
  {
    vDmaRead(lui8Data, lui32Len);
  }
}


#endif  //_WIN32


