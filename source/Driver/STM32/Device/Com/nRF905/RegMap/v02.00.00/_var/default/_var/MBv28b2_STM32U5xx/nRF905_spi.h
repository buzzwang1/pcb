#pragma once

#include "TYPEDEF.h"
#include "tGPPin.h"

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#include "stm32u5xx_ll_spi.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_exti.h"

#include "cDmaHelper.h"
#include "ClockInfo.h"

class cNRF905_SpiPins
{
  public:

  //SPI1 Outputs
  cGpPin mSCK;
  cGpPin mMISO;
  cGpPin mMOSI;

  cNRF905_SpiPins()
    : mSCK(GPIOB_BASE, 3),
      mMISO(GPIOB_BASE, 4),
      mMOSI(GPIOB_BASE, 5)
  {
  };

  void vInit()
  {
    mSCK.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMISO.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMOSI.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MOSI
  }
};

class cNRF905_Spi
{
  public:

  cNRF905_SpiPins mPins;

  u32   mu32Prescaler;

  SPI_TypeDef *mSpi;

  IRQn_Type mnNVIC_IRQChannelEv;

  // SPI ist Vollcuplex. Von daher 2 DMA Kanäle
  DMA_Channel_TypeDef* mstDmaChTx;
  DMA_Channel_TypeDef* mstDmaChRx;

  // Spi muss beim Empfangen Dummy Daten senden.
  u32 mu32DummyTx;

  // Sicherung von Register nach Init
  u32 mu32SPI_CFG1;
  u32 mu32DMATx_CRT1;
  u32 mu32DMARx_CRT1;

  u8 mu8DmaChTx;
  u8 mu8DmaChRx;

  u8 mu8DataWidth;

  cNRF905_Spi(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx)
  : mPins()
  {
    mSpi = lpstSpi;

    mu8DmaChTx = lu8DmaChTx;
    mu8DmaChRx = lu8DmaChRx;

    mstDmaChTx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[lu8DmaChTx]));
    mstDmaChRx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[lu8DmaChRx]));

    mu32DummyTx = 0; // 0x55555555;
    mu8DataWidth = 8;
  }

  inline void vStopDMA()
  {
    mstDmaChTx->CCR &= ~DMA_CCR_EN;
    mstDmaChRx->CCR &= ~DMA_CCR_EN;
  }

  // Überprüft, ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return (mstDmaChRx->CBR1 & 0xFFFF);
  }

  // Überprüft, ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas gesendet wurde
  u16 u16GetDmaCounterTx()
  {
    return (mstDmaChTx->CBR1 & 0xFFFF);
  }


  void vStartDMATx(uint8* pBuffer, u16 BufferSize)
  {
    // Zuerst SPI dann DMA klar machen
    // Sonst hat bei der 2. Übertragung das 1 Byte fehlt
    LL_SPI_Disable(mSpi);

    // Is programmed in number of bytes / DataWidth
    switch (mu8DataWidth)
    {
      case 32: LL_SPI_SetTransferSize(mSpi, BufferSize / 4); break;
      case 16: LL_SPI_SetTransferSize(mSpi, BufferSize / 2); break;
      case  8:
      default: LL_SPI_SetTransferSize(mSpi, BufferSize); break;
    }
    
    // Auf Simplex gehen, um RX OVR Error zu vermeiden
    LL_SPI_SetTransferDirection(mSpi, LL_SPI_SIMPLEX_TX);
    LL_SPI_Enable(mSpi);
    LL_SPI_StartMasterTransfer(mSpi);

    mstDmaChTx->CFCR = DMA_CFCR_TCF;
    mstDmaChTx->CCR &= ~DMA_CCR_EN;

    // Is programmed in number of bytes
    mstDmaChTx->CBR1 = BufferSize;

    mstDmaChTx->CSAR = (uint32)pBuffer;
    mstDmaChTx->CTR1 |= DMA_CTR1_SINC;

    mstDmaChTx->CCR |= DMA_CCR_EN;
  }

  void vFlushRx()
  {
    // In case there is still something in the RX Register
    // Flush RXDR, else DMA will have offset
    if (mSpi->SR & SPI_SR_RXP)
    {
      volatile u8 lu8Dummy = mSpi->RXDR;
      UNUSED(lu8Dummy);
    }
  }

  void vStartDMARx(uint8* pBuffer, u16 BufferSize)
  {
    vFlushRx();

    // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, BufferSize);
    LL_SPI_SetTransferDirection(mSpi, LL_SPI_FULL_DUPLEX);
    LL_SPI_Enable(mSpi);
    LL_SPI_StartMasterTransfer(mSpi);

    mstDmaChRx->CFCR = DMA_CFCR_TCF;
    mstDmaChRx->CCR &= ~DMA_CCR_EN;

    mstDmaChTx->CFCR = DMA_CFCR_TCF;
    mstDmaChTx->CCR &= ~DMA_CCR_EN;

    // Is programmed in number of bytes
    switch (mu8DataWidth)
    {
      case 32: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize * 4; break;
      case 16: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize * 2; break;
      case  8:
      default: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize; break;
    }

    mstDmaChRx->CDAR = (uint32)pBuffer;
    mstDmaChTx->CSAR = (u32)(&mu32DummyTx); // Zum Empfangen Dummy Daten schicken

    mstDmaChTx->CTR1 &= ~DMA_CTR1_SINC;

    mstDmaChRx->CCR |= DMA_CCR_EN;
    mstDmaChTx->CCR |= DMA_CCR_EN;
  }

  u32 u32BaudToRegister(u16 lu16Pres)
  {
    // mu32Prescaler wird bei SPI als prescaler verwendet
    switch (lu16Pres)
    {
      case 0:    return LL_SPI_BAUDRATEPRESCALER_BYPASS;
      case 2:    return LL_SPI_BAUDRATEPRESCALER_DIV2;
      case 4:    return LL_SPI_BAUDRATEPRESCALER_DIV4;
      case 8:    return LL_SPI_BAUDRATEPRESCALER_DIV8;
      case 16:   return LL_SPI_BAUDRATEPRESCALER_DIV16;
      case 32:   return LL_SPI_BAUDRATEPRESCALER_DIV32;
      case 64:   return LL_SPI_BAUDRATEPRESCALER_DIV64;
      case 128:  return LL_SPI_BAUDRATEPRESCALER_DIV128;
      case 256:  return LL_SPI_BAUDRATEPRESCALER_DIV256;
    }

    return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }

  u8 isErrorActive()
  {
    uint32 lui32ErrorMask;
    lui32ErrorMask = SPI_SR_MODF | SPI_SR_TIFRE | SPI_SR_CRCE | SPI_SR_OVR | SPI_SR_UDR;
    if ((mSpi->SR & lui32ErrorMask)) return 1;
    return 0;
  }

  void vInitDma()
  {
    /* Enable DMA clock */
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    cDmaHelper::vSet(mu8DmaChTx);
    cDmaHelper::vSet(mu8DmaChRx);

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

    if (mSpi == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_RX;
    if (mSpi == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_RX;
    if (mSpi == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_RX;

    lstDmaInit.SrcAddress  = (u32)&mSpi->RXDR;
    lstDmaInit.DestAddress = 0;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_FIXED;
    lstDmaInit.DestIncMode = LL_DMA_DEST_INCREMENT;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT0; // Source is periph
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1; // Dest is memory
    LL_DMA_Init(GPDMA1, mu8DmaChRx, &lstDmaInit);

    if (mSpi == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_TX;
    if (mSpi == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_TX;
    if (mSpi == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_TX;
    lstDmaInit.SrcAddress  = 0;
    lstDmaInit.DestAddress = (u32)&mSpi->TXDR;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_INCREMENT;
    lstDmaInit.DestIncMode = LL_DMA_DEST_FIXED;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT1; // Source is memory
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0; // Dest is periph
    LL_DMA_Init(GPDMA1, mu8DmaChTx, &lstDmaInit);
  }

  cComNode::tenError enInitHw(bool lbMaster) // __attribute__((optimize("-O0")))
  {
    SPI_HandleTypeDef lhSpi = {};

    mPins.vInit();

    if (mSpi == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    if (mSpi == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    if (mSpi == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();

    vStopDMA();

    /* SPI Peripheral Disable */
    LL_SPI_Disable(mSpi);


    /* I2C configuration */
    /* I2C configuration */
    lhSpi.Instance = mSpi;


    lhSpi.Init.Mode                      = SPI_MODE_MASTER;
    lhSpi.Init.Direction                 = SPI_DIRECTION_2LINES;
    lhSpi.Init.DataSize                  = SPI_DATASIZE_8BIT;
    lhSpi.Init.CLKPolarity               = SPI_POLARITY_LOW;
    lhSpi.Init.CLKPhase                  = SPI_PHASE_1EDGE;
    lhSpi.Init.NSS                       = SPI_NSS_SOFT;
    //lhSpi.Init.NSSPMode =
    //lhSpi.Init.NSSPolarity =
    //lhSpi.Init.MasterSSIdleness =
    lhSpi.Init.BaudRatePrescaler         = SPI_BAUDRATEPRESCALER_256;
    lhSpi.Init.FirstBit                  = SPI_FIRSTBIT_MSB;
    lhSpi.Init.TIMode                    = SPI_TIMODE_DISABLE;
    lhSpi.Init.CRCCalculation            = SPI_CRCCALCULATION_DISABLE;
    //lhSpi.Init.CRCPolynomial             =
    //lhSpi.Init.CRCLength                 =
    //lhSpi.Init.TxCRCInitializationPatter =
    //lhSpi.Init.RxCRCInitializationPatter =
    lhSpi.Init.FifoThreshold             = SPI_FIFO_THRESHOLD_04DATA;
    lhSpi.Init.MasterInterDataIdleness   = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    lhSpi.Init.MasterReceiverAutoSusp    = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    lhSpi.Init.MasterKeepIOState         = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    lhSpi.Init.IOSwap                    = SPI_IO_SWAP_DISABLE;
    lhSpi.Init.ReadyMasterManagement     = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    lhSpi.Init.ReadyPolarity             = SPI_RDY_POLARITY_HIGH;


    /* Peripheral clock enable */
    HAL_SPI_Init(&lhSpi);

    LL_SPI_SetBaudRatePrescaler(mSpi, u32BaudToRegister(mu32Prescaler));

    mSpi->IFCR = 0xFFFF;

    LL_SPI_EnableDMAReq_RX(mSpi);
    LL_SPI_EnableDMAReq_TX(mSpi);

    if (isErrorActive())
    {
      return cComNode::enErDeviceInitErrorDevice;
    }

    if (mSpi == SPI1) mnNVIC_IRQChannelEv = SPI2_IRQn;
    if (mSpi == SPI2) mnNVIC_IRQChannelEv = SPI2_IRQn;
    if (mSpi == SPI3) mnNVIC_IRQChannelEv = SPI2_IRQn;

    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(mnNVIC_IRQChannelEv, 8, 8);
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(mnNVIC_IRQChannelEv);

    if (lbMaster)
    {
      //mSpi->IER |= SPI_IER_UDRIE | SPI_IER_OVRIE | SPI_IER_MODFIE;
      mSpi->IER |= SPI_IER_EOTIE;
    }

    vInitDma();

    mu32SPI_CFG1   = mSpi->CFG1;
    mu32DMATx_CRT1 = mstDmaChTx->CTR1;
    mu32DMARx_CRT1 = mstDmaChRx->CTR1;


    return cComNode::enNoError;
  }


  void vSetPrescaler(u16 lu16Pres)
  {
    LL_SPI_Disable(mSpi);
       LL_SPI_SetBaudRatePrescaler(mSpi, u32BaudToRegister(lu16Pres));
    LL_SPI_Enable(mSpi);
  }


  void vSetHalfWordExchange()
  {
    MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_DHX, LL_DMA_DEST_HALFWORD_EXCHANGE);
  }

  // 8
  // 16
  // 32
  void vSetDataWidth(u8 lu8DWidth)
  {
    mu8DataWidth = lu8DWidth;
    switch (lu8DWidth)
    {
      case 32:
        {
          LL_SPI_Disable(mSpi);
            mstDmaChTx->CCR &= ~DMA_CCR_EN;

            LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_32BIT);
            LL_SPI_SetFIFOThreshold(mSpi, LL_SPI_FIFO_TH_04DATA);

            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_WORD);
            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_WORD);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_WORD);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_WORD);

          LL_SPI_Enable(mSpi);
        }
        break;
      case 16:
        {
          LL_SPI_Disable(mSpi);
            mstDmaChTx->CCR &= ~DMA_CCR_EN;

            LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_16BIT);
            LL_SPI_SetFIFOThreshold(mSpi, LL_SPI_FIFO_TH_02DATA);

            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_HALFWORD);
            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_HALFWORD);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_HALFWORD);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_HALFWORD);

          LL_SPI_Enable(mSpi);
        }
        break;
      case 8:
      default:
        {
          LL_SPI_Disable(mSpi);
            mstDmaChTx->CCR &= ~DMA_CCR_EN;

            LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_8BIT);

            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_BYTE);
            MODIFY_REG(mstDmaChTx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_BYTE);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_SDW_LOG2, LL_DMA_SRC_DATAWIDTH_BYTE);
            MODIFY_REG(mstDmaChRx->CTR1, DMA_CTR1_DDW_LOG2, LL_DMA_DEST_DATAWIDTH_BYTE);

          LL_SPI_Enable(mSpi);
        }
        break;
    }
  }

  void vRestoreCfg()
  {
    mu8DataWidth = 8;
    LL_SPI_Disable(mSpi);
      mSpi->CFG1 = mu32SPI_CFG1;
    LL_SPI_Enable(mSpi);

    mstDmaChTx->CCR &= ~DMA_CCR_EN;
      mstDmaChTx->CTR1 = mu32DMATx_CRT1;
      mstDmaChRx->CTR1 = mu32DMARx_CRT1;
  }

  void vEnableIrq()
  {
    HAL_NVIC_EnableIRQ(mnNVIC_IRQChannelEv);
  }

  void vDisableIrq()
  {
    HAL_NVIC_DisableIRQ(mnNVIC_IRQChannelEv);
  }

  void vWaitUntilStop()
  {
  }


  inline void vWaitBussy()
  {
  }

  inline void vWaitEOT()
  {
    while ((mSpi->SR & SPI_FLAG_EOT) == 0);
    mSpi->IFCR = 0xFFFF;
  }


  inline void vClearRxBuf()
  {
    // Disable/Enable flushes the Fifos
    // CLEAR_BIT(SPI1->CR1 , SPI_CR1_SPE);
    // SET_BIT(SPI1->CR1 , SPI_CR1_SPE);
    while ((mSpi->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)))
    {
      LL_SPI_ReceiveData8(mSpi);
    }
  }

  void vWrite(uint8 *lui8Data, uint32 lui32Len)
  {
    mSpi->IER &= ~SPI_IER_EOTIE;
    MODIFY_REG(mSpi->CR2, SPI_CR2_TSIZE, lui32Len);
    mSpi->CR1 |= (SPI_CR1_SPE);
    LL_SPI_StartMasterTransfer(mSpi);

    while (lui32Len)
    {
      while((mSpi->SR & SPI_FLAG_TXP) == 0); // Tx Fifo has space ?
      LL_SPI_TransmitData8(mSpi, *lui8Data);
      lui8Data++;
      lui32Len--;
    }
    vWaitEOT();
    mSpi->CR1 &= ~(SPI_CR1_SPE);
  }

  void vRead(uint8 *lui8Data, uint32 lui32RxLen)
  {
    u32 lui32TxLen = lui32RxLen;

    mSpi->IER &= ~SPI_IER_EOTIE;
    MODIFY_REG(mSpi->CR2, SPI_CR2_TSIZE, lui32RxLen);
    mSpi->CR1 |= (SPI_CR1_SPE);
    vClearRxBuf();
    LL_SPI_StartMasterTransfer(mSpi);

    while ((lui32TxLen) || (lui32RxLen))
    {
      if ((mSpi->SR & SPI_FLAG_TXP) && (lui32TxLen))
      {
        LL_SPI_TransmitData8(mSpi, 0xFF);
        lui32TxLen--;
      }

      if ((mSpi->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) && (lui32RxLen))
      {
        *lui8Data = LL_SPI_ReceiveData8(mSpi);
        lui8Data++;
        lui32RxLen--;
      }
    }
    vWaitEOT();
    mSpi->CR1 &= ~(SPI_CR1_SPE);
  }
};

class cNRF905_Timer
{
  public:

  cNRF905_Timer()
  {
  }

  void vStart(u16 lui16Time_us)
  {
    //  Tim7: The counter counts from 0 to the auto-reload value (contents of the TIMx_ARR register),
    //  then restarts from 0 and generates a counter overflow event.
    TIM7->ARR = lui16Time_us;
    TIM7->CNT = 0;

    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 |= 8; // OPM: One Pulse Mode: Counter stops counting at the next update event (clearing the CEN bit).
    TIM7->CR1 |= TIM_CR1_CEN;
  }

  void vStop()
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }

  void vEnableIrq()
  {
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }

  void vDisableIrq()
  {
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  }

  bool isDone()
  {
    return ((TIM7->SR & TIM_SR_UIF) == TIM_SR_UIF);
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
  cGpPin mTRx_Cn; // Chip select
  cGpPin mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  cGpPin mPWR;    // Power
  cGpPin mCS_SPI; // Chip select SPI

  //Inputs
  cGpPin mCD;  // Carrier Detected // CD and AM are n.A. So just put it to DR.
  cGpPin mAM;  // Adress Match
  cGpPin mDR;  // Data Ready*/

  cNRF905_Pins()
    : mTRx_Cn(GPIOE_BASE, 0),
      mTx_En(GPIOB_BASE, 8),
      mPWR(GPIOE_BASE, 1),
      mCS_SPI(GPIOE_BASE, 6),

      //Inputs
      mCD(GPIOE_BASE, 3),
      mAM(GPIOE_BASE, 4),
      mDR(GPIOE_BASE, 5)
  {
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

    mTRx_Cn.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mTx_En.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,  GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mPWR.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,    GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mCS_SPI.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1),

    mCD.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mAM.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mDR.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)

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


