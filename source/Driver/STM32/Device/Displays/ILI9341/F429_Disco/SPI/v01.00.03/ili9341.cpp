//###############################
    /*ILI9341*/
//###############################

#include "ili9341.h"


#ifndef  _WIN32

DMA_InitTypeDef mstDMA;
uint8 mui8FirstTx;

void ILI9341_Delay(volatile unsigned int delay)
{
  for (; delay != 0; delay--);
}


void ILI9341_SPI_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;

  //Common settings for all pins
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

  //Enable clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  //Pinspack nr. 1        SCK          MISO         MOSI
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

  SPI_StructInit(&SPI_InitStruct);
  //SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI5, &SPI_InitStruct);
  SPI_Cmd(SPI5, ENABLE);
}

uint8_t ILI9341_SPI_Send(SPI_TypeDef* SPIx, uint8_t data)
{
  //Fill output buffer with data
  SPIx->DR = data;
  //Wait for transmission to complete
  while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));
  //Wait for received data to complete
  while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
  //Wait for SPI to be ready
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY));
  //Return data from buffer
  return SPIx->DR;
}

void ILI9341_SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint8_t *dataIn, uint16_t count)
{
  uint8_t i;
  for (i = 0; i < count; i++)
  {
    dataIn[i] = ILI9341_SPI_Send(SPIx, dataOut[i]);
  }
}

void ILI9341_SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint16_t count)
{
  uint16_t i;
  for (i = 0; i < count; i++)
  {
    ILI9341_SPI_Send(SPIx, dataOut[i]);
  }
}

void ILI9341_SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t *dataIn, uint8_t dummy, uint16_t count)
{
  uint16_t i;
  for (i = 0; i < count; i++)
  {
    dataIn[i] = ILI9341_SPI_Send(SPIx, dummy);
  }
}


void ILI9341_InitDMA(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  DMA_StructInit(&mstDMA);
  //==Configure DMA2 – Channel7: Stream6 (SPI5_TX) => (memory -> SPI5)
  mstDMA.DMA_Channel = DMA_Channel_7;

  //DMA_ClearFlag(DMA2_FLAG_GL5);
  //DMA_ClearFlag(DMA_FLAG_HTIF6);
  DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);


  //1.Set the peripheral register address in the DMA_CPARx register. The data will be moved from/ to this address to/ from the memory after the peripheral event.
  //Address of peripheral the DMA must map to
  mstDMA.DMA_PeripheralBaseAddr = (uint32)&SPI5->DR;

  //4. Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
  mstDMA.DMA_Priority           = DMA_Priority_High;

  //5. Configure data transfer direction, circular mode, peripheral & memory incremented mode, peripheral & memory data size, and interrupt after half and/or full transfer in the DMA_CCRx register
  mstDMA.DMA_FIFOMode        = DMA_FIFOMode_Enable;
  mstDMA.DMA_FIFOThreshold   = DMA_FIFOThreshold_Full;
  mstDMA.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
  mstDMA.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;


  //Src Config
  mstDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  mstDMA.DMA_MemoryInc      = DMA_MemoryInc_Enable;


  //Dest Config
  mstDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  mstDMA.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; //DMA_PeripheralInc_Enable;

  // Mode
  mstDMA.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
  mstDMA.DMA_Mode               = DMA_Mode_Normal;

  DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);
  SPI_I2S_DMACmd(SPI5, SPI_I2S_DMAReq_Tx, ENABLE);


  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  mui8FirstTx = 1;
}


uint8 ILI9341_u8DMA_TX_Complete(void)
{
  uint8 lui8Result;

  lui8Result = 1;
  //At the end of the transfer, the Transfer Complete Flag (TCIF) is set
  //This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the DMA_IFCR register.

  if (!mui8FirstTx)
  {
    lui8Result = DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6);
  }
  return lui8Result;
}

uint32 ILI9341_DMA_Data_Left = 0;


void ILI9341_DMA_TC(void)
{
  DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);

  if (ILI9341_DMA_Data_Left > 0)
  {
    DMA_Cmd(DMA2_Stream6, DISABLE);

    //2. Set the memory address in the DMA_CMARx register. The data will be written to or read from this memory after the peripheral event.
    //Variable from which data will be transmitted
    mstDMA.DMA_Memory0BaseAddr += 64000;

    //3. Configure the total number of data to be transferred in the DMA_CNDTRx register. After each peripheral event, this value will be decremented.
    //Buffer size
    if (ILI9341_DMA_Data_Left > 64000)
    {
      mstDMA.DMA_BufferSize  = 64000;
      ILI9341_DMA_Data_Left -= 64000;
    }
    else
    {
      mstDMA.DMA_BufferSize = ILI9341_DMA_Data_Left;
      ILI9341_DMA_Data_Left = 0;
    }

    DMA_Init(DMA2_Stream6, &mstDMA); //Initialise the DMA
    DMA_Cmd(DMA2_Stream6, ENABLE); //Enable the DMA1 – Channel5
  }
  else
  {
    DMA_Cmd(DMA2_Stream6, DISABLE);
  }
}


void ILI9341_DMA_Start(uint32 *lpui8SourceAdr, uint32 lui16SizeofSource )
{

  if (ILI9341_DMA_Data_Left == 0)
  {
    ILI9341_DMA_Data_Left = lui16SizeofSource;

    DMA_Cmd(DMA2_Stream6, DISABLE);

    mui8FirstTx = 0;

    //DMA_ClearFlag(DMA2_Stream6, DMA_FLAG_TCIF6);

    //2. Set the memory address in the DMA_CMARx register. The data will be written to or read from this memory after the peripheral event.
    //Variable from which data will be transmitted
    mstDMA.DMA_Memory0BaseAddr = (uint32)lpui8SourceAdr;

    //3. Configure the total number of data to be transferred in the DMA_CNDTRx register. After each peripheral event, this value will be decremented.
    //Buffer size
    if (ILI9341_DMA_Data_Left > 64000)
    {
      mstDMA.DMA_BufferSize  = 64000;
      ILI9341_DMA_Data_Left -= 64000;
    }
    else
    {
      mstDMA.DMA_BufferSize = ILI9341_DMA_Data_Left;
      ILI9341_DMA_Data_Left = 0;
    }

    DMA_Init(DMA2_Stream6, &mstDMA); //Initialise the DMA
    DMA_Cmd(DMA2_Stream6, ENABLE); //Enable the DMA1 – Channel5
  }
}


void ILI9341_SendCommand(uint8_t data) {
  ILI9341_WRX_RESET;
  ILI9341_CS_RESET;
  ILI9341_SPI_Send(ILI9341_SPI, data);
  ILI9341_CS_SET;
}

void ILI9341_SendData(uint8_t data) {
  ILI9341_WRX_SET;
  ILI9341_CS_RESET;
  ILI9341_SPI_Send(ILI9341_SPI, data);
  ILI9341_CS_SET;
}

void ILI9341_SetArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
  ILI9341_SendData(x1 >> 8);
  ILI9341_SendData(x1 & 0xFF);
  ILI9341_SendData(x2 >> 8);
  ILI9341_SendData(x2 & 0xFF);

  ILI9341_SendCommand(ILI9341_PAGE_ADDR);
  ILI9341_SendData(y1 >> 8);
  ILI9341_SendData(y1 & 0xFF);
  ILI9341_SendData(y2 >> 8);
  ILI9341_SendData(y2 & 0xFF);
}


void ILI9341_Rotate(ILI9341_Orientation_t orientation, uint8 BGR)
{
  uint8 lui8Data;

  lui8Data = 0;
  if (orientation == ILI9341_Orientation_Portrait_1)
  {
    lui8Data = 0x50;
  }
  else if (orientation == ILI9341_Orientation_Portrait_2)
  {
    lui8Data = 0x80;
  }
  else if (orientation == ILI9341_Orientation_Landscape_1)
  {
    lui8Data = 0x20;
  }
  else if (orientation == ILI9341_Orientation_Landscape_2)
  {
    lui8Data = 0xE0;
  }

  if (lui8Data)
  {
    ILI9341_SendCommand(ILI9341_MAC);
    if (BGR) lui8Data |= 0x08;
    ILI9341_SendData(lui8Data);
  }
}

void ILI9341_InitLCD(void)
{
  // ILI9341_RST_RESET;
  ILI9341_RST_SET;

  ILI9341_SendCommand(ILI9341_RESET);
  ILI9341_Delay(2000000);

  ILI9341_SendCommand(ILI9341_POWERA);
  ILI9341_SendData(0x39);
  ILI9341_SendData(0x2C);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x34);
  ILI9341_SendData(0x02);
  ILI9341_SendCommand(ILI9341_POWERB);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0xC1);
  ILI9341_SendData(0x30);
  ILI9341_SendCommand(ILI9341_DTCA);
  ILI9341_SendData(0x85);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x78);
  ILI9341_SendCommand(ILI9341_DTCB);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_POWER_SEQ);
  ILI9341_SendData(0x64);
  ILI9341_SendData(0x03);
  ILI9341_SendData(0x12);
  ILI9341_SendData(0x81);
  ILI9341_SendCommand(ILI9341_PRC);
  ILI9341_SendData(0x20);
  ILI9341_SendCommand(ILI9341_POWER1);
  ILI9341_SendData(0x23);
  ILI9341_SendCommand(ILI9341_POWER2);
  ILI9341_SendData(0x10);
  ILI9341_SendCommand(ILI9341_VCOM1);
  ILI9341_SendData(0x3E);
  ILI9341_SendData(0x28);
  ILI9341_SendCommand(ILI9341_VCOM2);
  ILI9341_SendData(0x86);
  ILI9341_SendCommand(ILI9341_MAC);
  ILI9341_SendData(0x20);
  ILI9341_SendCommand(ILI9341_PIXEL_FORMAT);
  ILI9341_SendData(0x55);
  ILI9341_SendCommand(ILI9341_FRC);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x18);
  ILI9341_SendCommand(ILI9341_DFC);
  ILI9341_SendData(0x08);
  ILI9341_SendData(0x82);
  ILI9341_SendData(0x27);
  ILI9341_SendCommand(ILI9341_3GAMMA_EN);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0xEF);
  ILI9341_SendCommand(ILI9341_PAGE_ADDR);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x01);
  ILI9341_SendData(0x3F);
  ILI9341_SendCommand(ILI9341_GAMMA);
  ILI9341_SendData(0x01);
  ILI9341_SendCommand(ILI9341_PGAMMA);
  ILI9341_SendData(0x0F);
  ILI9341_SendData(0x31);
  ILI9341_SendData(0x2B);
  ILI9341_SendData(0x0C);
  ILI9341_SendData(0x0E);
  ILI9341_SendData(0x08);
  ILI9341_SendData(0x4E);
  ILI9341_SendData(0xF1);
  ILI9341_SendData(0x37);
  ILI9341_SendData(0x07);
  ILI9341_SendData(0x10);
  ILI9341_SendData(0x03);
  ILI9341_SendData(0x0E);
  ILI9341_SendData(0x09);
  ILI9341_SendData(0x00);
  ILI9341_SendCommand(ILI9341_NGAMMA);
  ILI9341_SendData(0x00);
  ILI9341_SendData(0x0E);
  ILI9341_SendData(0x14);
  ILI9341_SendData(0x03);
  ILI9341_SendData(0x11);
  ILI9341_SendData(0x07);
  ILI9341_SendData(0x31);
  ILI9341_SendData(0xC1);
  ILI9341_SendData(0x48);
  ILI9341_SendData(0x08);
  ILI9341_SendData(0x0F);
  ILI9341_SendData(0x0C);
  ILI9341_SendData(0x31);
  ILI9341_SendData(0x36);
  ILI9341_SendData(0x0F);
  ILI9341_SendCommand(ILI9341_SLEEP_OUT);

  ILI9341_Delay(1000000);

  ILI9341_SendCommand(ILI9341_DISPLAY_ON);
  ILI9341_SendCommand(ILI9341_GRAM);
}

void ILI9341_Init()
{
  GPIO_InitTypeDef GPIO_InitDef;

  RCC_AHB1PeriphClockCmd(ILI9341_WRX_CLK, ENABLE);
  GPIO_InitDef.GPIO_Pin = ILI9341_WRX_PIN;
  GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
  GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ILI9341_WRX_PORT, &GPIO_InitDef);

  RCC_AHB1PeriphClockCmd(ILI9341_CS_CLK, ENABLE);
  GPIO_InitDef.GPIO_Pin = ILI9341_CS_PIN;
  GPIO_Init(ILI9341_CS_PORT, &GPIO_InitDef);

  RCC_AHB1PeriphClockCmd(ILI9341_RST_CLK, ENABLE);
  GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitDef.GPIO_Pin = ILI9341_RST_PIN;
  GPIO_Init(ILI9341_RST_PORT, &GPIO_InitDef);

  ILI9341_CS_SET;

  ILI9341_SPI_Init();
  ILI9341_InitDMA();

  ILI9341_InitLCD();
}


void ILI9341_Show(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  //uint32 lui32t;
  //uint16 lui16Col;

  ILI9341_SetArea(0, 0, ILI9341_HEIGHT - 1, ILI9341_WIDTH - 1);

  ILI9341_SendCommand(ILI9341_GRAM);

  ILI9341_WRX_SET;
  ILI9341_CS_RESET;

  /*for (lui32t = 0; lui32t < ILI9341_PIXEL * 2; lui32t++) {
    lui16Col = lpstScreen->aui8Data[lui32t];
    ILI9341_SPI_Send(ILI9341_SPI, lui16Col);
  }*/

  ILI9341_DMA_Start((uint32*)lpstBm->mpui8Data, (lpstBm->miGfxHeight * lpstBm->miGfxWidth * 2));

  return;
}


#endif  //_WIN32

