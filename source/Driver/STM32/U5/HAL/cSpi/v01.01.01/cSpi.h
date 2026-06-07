#pragma once

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#include "stm32u5xx_ll_spi.h"
#include "stm32u5xx_ll_dma.h"

#include "tGPPin.h"
#include "ComDat.h"
#include "cDmaHelper.h"
#include "ClockInfo.h"


class cSpi
{
  public:
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

  cSpi(SPI_TypeDef *lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx)
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
};

class cSpiMaster : public cSpi, public cComNodeMasterMulti
{
  public:

  // Für Nachrichten >255 Byte
  u16  mu16ComByteCnt;
  u8*  mpui8ComBuf;

  cSpiMaster(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx,
             u16 lu32Prescaler = 7, u16 luInitDelay_ms = 1)
    : cSpi(lpstSpi, lu8DmaChTx, lu8DmaChRx),
      cComNodeMasterMulti(luInitDelay_ms)
  {
    mu32Prescaler  = lu32Prescaler;
    mu32Baudrate   = lu32Prescaler;
  }

  cComNode::tenError enInitHw() override
  {
    return cSpi::enInitHw(True);
  }

  bool bCheckBusy() override
  {
    return True;
  }

  /*
    RXNE:   Receive buffer not empty
    TXIS:   Transmit buffer interrupt status
    STOPF:  Stop detection interrupt flag
    TC/TCR: Transfer Complete/ Transfer complete Reload
    ADDR:   Address matched
    NACKF:  NACK reception
  */
  void vIRQHandler()
  {
    if (cSpi::isErrorActive())
    {
      vComError();
    }
    else
    {
      vSm(cComNode::tenEvent::enEvIrq);
    }
    vStart();
  }


  void vResetCom()
  {
    if (mSm != cComNode::tenState::enStIdle)
    {
      // I2C Peripheral Disable
      vStopDMA();
      mSpi->IFCR = 0xFFFF;
      mpcActiveMsg   = NULL;
      mpcActiveSlave = NULL;

      mSm    = cComNode::tenState::enStIdle;
      mError = cComNode::enNoError;
    }
  }

  // Start wird sowohl vom Interrupt wie auch von der Main benutzt
  // Um keine Laufzeit Konflikte zu bekommen, wird solange der Interrupt gesperrt
  void vPreStart() override
  {
    mSpi->IFCR = 0xFFFF;
    vDisableIrq();
  }

  void vPostStart() override
  {
    vEnableIrq();
  }


  /*
    I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
    I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
    I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
    I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
  */
  void vComError() override
  {
    if (cSpi::isErrorActive())
    {
      if      (mSpi->SR & SPI_SR_UDR)   mError = cComNode::enErUnderrun;
      else if (mSpi->SR & SPI_SR_OVR)   mError = cComNode::enErOverrun;
      else if (mSpi->SR & SPI_SR_CRCE)  mError = cComNode::enErCrc;
      else if (mSpi->SR & SPI_SR_TIFRE) mError = cComNode::enErFrame;
      else if (mSpi->SR & SPI_SR_MODF)  mError = cComNode::enErMode;
      else mError = cComNode::enErUnknown;
    }

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(mError, mSm);
    }
    vRestoreCfg();
    vResetCom();
  }

  void vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
      case cComNode::tenState::enStLock:
      case cComNode::tenState::enStError:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvStart:
          case cComNode::tenEvent::enEvStartSkipAdr:
            switch (mpcActiveMsg->enDir())
            {
              case cComNode::tenDirection::enIsTxRx:
                mu16ComByteCnt = ((cComMsgI2c<u16>*)mpcActiveMsg)->GetLenTx();
                mpui8ComBuf     = ((cComMsgI2c<u16>*)mpcActiveMsg)->GetDataTx();
                break;
              default:
                mu16ComByteCnt = mpcActiveMsg->Len();
                mpui8ComBuf     = mpcActiveMsg->Data();
                break;
            }

            switch (mpcActiveMsg->enDir())
            {
              case cComNode::tenDirection::enIsTxRx:
                mSm = cComNode::tenState::enStTx;
                vStartDMATx(mpui8ComBuf, mu16ComByteCnt);
                break;
              case cComNode::tenDirection::enIsTx:
                mSm = cComNode::tenState::enStTx;
                vStartDMATx(mpui8ComBuf, mu16ComByteCnt);
                break;
              case cComNode::tenDirection::enIsRx:
                mSm = cComNode::tenState::enStRx;
                cSpi::vFlushRx();
                vStartDMARx(mpui8ComBuf, mu16ComByteCnt);
                break;
              default:
                mpcActiveSlave->vComDone();
                break;
            }
            break;
          case cComNode::tenEvent::enEvIrq:
              mError = cComNode::enErUnknown;
              vComError();
              mSpi->IFCR = 0xFFFF;
            break;
          default:
            mSpi->IFCR = 0xFFFF;
            break;
            break;
        }
        break;
      case cComNode::tenState::enStTx:
        {
          uint32 lui32ISR = mSpi->SR;
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvIrq:
              if (lui32ISR & SPI_SR_EOT)
              {
                switch (mpcActiveMsg->enDir())
                {
                  case cComNode::tenDirection::enIsTxRx:
                    mSm = cComNode::enStRx;
                    cSpi::vFlushRx();
                    mu16ComByteCnt = ((cComMsgI2c<u16>*)mpcActiveMsg)->GetLenRx();
                    mpui8ComBuf    = ((cComMsgI2c<u16>*)mpcActiveMsg)->GetDataRx();
                    vStartDMARx(mpui8ComBuf, mu16ComByteCnt);
                    break;
                  default:
                    mSpi->IFCR = 0xFFFF;
                    mSm = cComNode::tenState::enStIdle;
                    mpcActiveSlave->vComDone();
                    break;
                }
              }
              else
              {
                mError = cComNode::enErUnknown;
                vComError();
              }
              break;
            default:
              break;
          }
        }
        break;

      case cComNode::tenState::enStRx:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvIrq:
            if (mSpi->SR & SPI_SR_EOT)
            {
              /* Clear STOPF flag */
              mSpi->IFCR = 0xFFFF;
              mSm = cComNode::tenState::enStIdle;
              mpcActiveSlave->vComDone();
            }
            else
            {
              vComError();
            }
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
};


