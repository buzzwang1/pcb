
#ifndef __HX8347__HW_H__
#define __HX8347__HW_H__

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_misc.h"

#include "ClockInfo.h"

class cHx8347_HW
{
  public:
  tcGpPin<GPIOB_BASE, 13> mSCK;
  tcGpPin<GPIOB_BASE, 14> mMISO;
  tcGpPin<GPIOB_BASE, 15> mMOSI;


  tcGpPin<GPIOB_BASE, 12> mPinCs;
  tcGpPin<GPIOA_BASE, 8>  mPinPs;
  tcGpPin<GPIOA_BASE, 9>  mPinRst;
  tcGpPin<GPIOB_BASE, 1>  mPinPwr;

  SPI_TypeDef* mSpi;
  DMA_Channel_TypeDef* mDMA_Tx;
  DMA_Channel_TypeDef* mDMA_Rx;

  uint8 mui8Dummy; // Dummy für Rx

  cHx8347_HW()
    : mSCK( GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMISO(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mMOSI(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),

      mPinCs( GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 1),
      mPinPs( GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mPinRst(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mPinPwr(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0)
  {
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_5);  // SCK
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_5);  // MISO
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_5);  // MOSI

    mSpi = SPI2;
    mDMA_Tx = DMA1_Channel5;
    mDMA_Rx = DMA1_Channel4;

    mui8Dummy = 0x55;

    cClockInfo::Update();

    SPI_InitTypeDef   SPI_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);      // Enable DMA1 clock

    /* SD_SPI Config */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;

    //8Mhz   /   2 =>   4  Mhz
    switch (cClockInfo::mstClocks.HCLK_Frequency)
    {
      case 72000000: SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; break;
      case 16000000: SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; break;
      default:       SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; break;
    }


    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(mSpi, &SPI_InitStructure);

    SPI_RxFIFOThresholdConfig(mSpi, SPI_RxFIFOThreshold_QF);

    SPI_Cmd(mSpi, ENABLE); /* SD_SPI enable */

    //----------------------------------------------------------
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);      // Enable DMA1 clock

    DMA_DeInit(mDMA_Rx);
    DMA_DeInit(mDMA_Tx);


    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_M2M      = DMA_M2M_Disable;            // M2M Disabled- Peripheral mode (requires timer trigger)
    DMA_InitStruct.DMA_Mode     = DMA_Mode_Normal;            // Normal mode
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;    // Medium priority
    DMA_InitStruct.DMA_DIR      = DMA_DIR_PeripheralDST;  // Memory to Peripheral

    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     // 8-bit Register
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       // Always write to same register
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32)&mSpi->DR;       // Output data for SPI peripheral

    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             // 8-bit array
    DMA_InitStruct.DMA_MemoryInc      = DMA_MemoryInc_Enable;                // No Increment through array
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;                                   // Initialize later

    DMA_InitStruct.DMA_BufferSize = 0;                                       // Initialize later

    DMA_Init(mDMA_Tx, &DMA_InitStruct);                // Initialize TX DMA


    DMA_StructInit(&DMA_InitStruct);
    DMA_InitStruct.DMA_M2M      = DMA_M2M_Disable;            // M2M Disabled- Peripheral mode (requires timer trigger)
    DMA_InitStruct.DMA_Mode     = DMA_Mode_Normal;            // Normal mode
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;    // Medium priority
    DMA_InitStruct.DMA_DIR      = DMA_DIR_PeripheralSRC;  // Memory to Peripheral

    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     // 8-bit Register
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       // Always write to same register
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32)&mSpi->DR;       // Output data for SPI peripheral

    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             // 8-bit array
    DMA_InitStruct.DMA_MemoryInc      = DMA_MemoryInc_Enable;                // Increment through array
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;                                   // Initialize later

    DMA_InitStruct.DMA_BufferSize = 0;                                       // Initialize later

    DMA_Init(mDMA_Rx, &DMA_InitStruct);            // Initialize RX DMA

    SPI_I2S_DMACmd(mSpi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    DMA_ITConfig(mDMA_Rx, /*DMA_IT_TE |*/ DMA_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig(mDMA_Tx, /*DMA_IT_TE |*/ DMA_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_Init(&NVIC_InitStructure);
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
    return ((DMA1_Channel5->CNDTR == 0) && (!SPI_I2S_GetFlagStatus(mSpi, SPI_I2S_FLAG_BSY)));
  }

  inline void vWaitBussy()
  {
    while (SPI_I2S_GetFlagStatus(mSpi, SPI_I2S_FLAG_BSY));
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
    DMA1->IFCR = (DMA1_FLAG_GL4 | DMA1_FLAG_HT4 | DMA1_FLAG_TC4 |
                  DMA1_FLAG_GL5 | DMA1_FLAG_HT5 | DMA1_FLAG_TC5);

    DMA1_Channel5->CNDTR = lui32Len;
    DMA1_Channel5->CMAR  = (uint32)lui8Data;
    DMA1_Channel5->CCR  |= (DMA_MemoryInc_Enable | DMA_CCR_TCIE | DMA_CCR_EN); // Memory Inc
  }

  void write(u8 lui8Data)
  {
    while (SPI_I2S_GetFlagStatus(mSpi, SPI_I2S_FLAG_TXE) == RESET); // TX Empty (Front Test)
    SPI_SendData8(mSpi, lui8Data);
  }

  /*void cmd(uint8_t cmd)
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
  }*/
};



#endif // __HX8347__HW_H__
