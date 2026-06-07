#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_usart.h"
#include "stm32u5xx_ll_lpuart.h"
#include "stm32u5xx_ll_dma.h"

#include "cUartMpHdTimer.h"

#include "tGPPin.h"
#include "ComDat.h"

#include "ClockInfo.h"


class cUartMpHd
{
  public:
  u16                    mu16DmaCounter2ms_Old;
  u8                     mu8SelfTimer;

  //Single Wire, only Tx Pin
  cGpPin        *mPinTx;
  USART_TypeDef*        mUsart;

  u8    mu8DmaCh;
  DMA_Channel_TypeDef* mstDmaCh;
  u32   mu32DmaRxTr1;
  u32   mu32DmaRxTr2;
  u32   mu32DmaRxPeriph;
  u32   mu32DmaTxTr1;
  u32   mu32DmaTxTr2;
  u32   mu32DmaTxPeriph;

  u32                   mChnTxFlagTc;
  u32                   mChnRxFlagTc;
  IRQn_Type             mIrqChannelUsart;
  IRQn_Type             mIrqChannelDMA;



  cUartMpHd(USART_TypeDef *lstUsart, cGpPin* lcRxTx, u8 lu8DmaCh)
  {
    mUsart = lstUsart;
    mu8SelfTimer = 0;
	
    mPinTx   = lcRxTx;

    mu8DmaCh = lu8DmaCh;
    mstDmaCh = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[mu8DmaCh]));	

    if (mUsart == USART1)
    {
      ////mChnTxFlagTc = DMA_FLAG_TC6;
      ////mChnRxFlagTc = DMA_FLAG_TC7;

      ////mIrqChannelDMA =   DMA2_Channel7_IRQn; //USART1 connect to channel 7 of DMA2
      mIrqChannelUsart = USART1_IRQn;
    }
    else
    if (mUsart == USART2)
    {
      ////mChnTxFlagTc = DMA_FLAG_TC7;
      ////mChnRxFlagTc = DMA_FLAG_TC6;

      ////mIrqChannelDMA   = DMA1_Channel6_IRQn;
      mIrqChannelUsart = USART2_IRQn;
    }
    else
    if (mUsart == USART3)
    {
      ////mChnTxFlagTc = DMA_FLAG_TC2;
      ////mChnRxFlagTc = DMA_FLAG_TC3;

      ////mIrqChannelDMA = DMA1_Channel3_IRQn;
      mIrqChannelUsart = USART3_IRQn;
    }
  }

  void  vStopDMA()
  {
    LL_USART_DisableDMAReq_RX(mUsart);
    LL_USART_DisableDMAReq_TX(mUsart);
    mstDmaCh->CCR &= ~DMA_CCR_EN;
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return (mstDmaCh->CBR1 & 0xFFFF);
  }

  void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
      mu16DmaCounter2ms_Old = BufferSize;
      mstDmaCh->CFCR = DMA_CFCR_TCF;
      mstDmaCh->CCR &= ~DMA_CCR_EN;
      mstDmaCh->CBR1 = BufferSize;
      mstDmaCh->CSAR = mu32DmaRxPeriph;
      mstDmaCh->CDAR = (uint32)pBuffer;
      mstDmaCh->CTR1 = mu32DmaRxTr1;
      mstDmaCh->CTR2 = mu32DmaRxTr2;
      mstDmaCh->CCR |= DMA_CCR_EN;
      LL_USART_EnableDMAReq_RX(mUsart);

  }

  void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
      mstDmaCh->CFCR = DMA_CFCR_TCF;
      mstDmaCh->CCR &= ~DMA_CCR_EN;
      mstDmaCh->CBR1 = BufferSize;
      mstDmaCh->CSAR = (uint32)pBuffer;
      mstDmaCh->CDAR = mu32DmaTxPeriph;
      mstDmaCh->CTR1 = mu32DmaTxTr1;
      mstDmaCh->CTR2 = mu32DmaTxTr2;
      mstDmaCh->CCR |= DMA_CCR_EN;
      LL_USART_EnableDMAReq_TX(mUsart);
  }

  void  vInitHw(u8 lui8Adr, u32 lu32Baudrate, bool lbMaster)
  {
    /* Disable USART clock */
    if (mUsart == USART1)      { __HAL_RCC_USART1_FORCE_RESET(); __HAL_RCC_USART1_RELEASE_RESET();}
    else if (mUsart == USART2) { __HAL_RCC_USART2_FORCE_RESET(); __HAL_RCC_USART2_RELEASE_RESET(); }
    else if (mUsart == USART3) { __HAL_RCC_USART3_FORCE_RESET(); __HAL_RCC_USART3_RELEASE_RESET(); }

    // Enable DMA
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    vStopDMA();

    //SDA and SCL must be pulled high
    if (lbMaster)
    {
      if (!bCheckPins()) return;
    }

    //Single wire, only TX line
    if (mUsart == USART1)      { mPinTx->vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART1);}
    else if (mUsart == USART2) { mPinTx->vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART2);}
    else if (mUsart == USART3) { mPinTx->vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART3);}

    // Usart Periph clock enable
    if (mUsart == USART1) __HAL_RCC_USART1_CLK_ENABLE();
    else if (mUsart == USART2) __HAL_RCC_USART2_CLK_ENABLE();
    else if (mUsart == USART3) __HAL_RCC_USART3_CLK_ENABLE();

    // Usart Peripheral Disable
    LL_USART_Disable(mUsart);

    LL_USART_InitTypeDef lstInit;
    LL_USART_StructInit(&lstInit);

    lstInit.BaudRate            = lu32Baudrate;
    lstInit.DataWidth           = LL_USART_DATAWIDTH_9B;
    lstInit.StopBits            = LL_USART_STOPBITS_1;
    lstInit.Parity              = LL_USART_PARITY_NONE;
    lstInit.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    lstInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    lstInit.OverSampling        = LL_USART_OVERSAMPLING_8;

    LL_USART_Init(mUsart, &lstInit);

    LL_USART_EnableHalfDuplex(mUsart);

    if (!lbMaster)
    {
      LL_USART_ConfigNodeAddress(mUsart, LL_USART_ADDRESS_DETECT_4B, lui8Adr);
      LL_USART_EnableMuteMode(mUsart);
      LL_USART_SetWakeUpMethod(mUsart, LL_USART_WAKEUP_ADDRESSMARK);
    }

    // Usart Peripheral Enable
    LL_USART_Enable(mUsart);
    mUsart->ICR = 0xFFFFFFFF;

    //if (!mbMaster)
    //{
    //  LL_USART_RequestEnterMuteMode(mUsart);
    //}

    mUsart->CR1 |= LL_USART_CR1_TCIE;
    mUsart->CR3 |= LL_USART_CR3_EIE;

    // ----------- DMA ---------
    LL_DMA_InitTypeDef           lstDmaInit;

    LL_DMA_StructInit(&lstDmaInit);

    lstDmaInit.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
    lstDmaInit.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;

    lstDmaInit.SrcBurstLength = 1;
    lstDmaInit.DestBurstLength = 1;

    lstDmaInit.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
    lstDmaInit.BlkDataLength = 0;

    lstDmaInit.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;

    // -Port 0 should be allocated for transfers toand from
    //  peripherals, because there is a direct hardware datapath
    //  between this portand the APB peripherals, outside the  AHB matrix
    // -Port 1 should be allocated for transfers toand from
    //  memory, which are performed through the AHB interconnect.

    if (mUsart == USART1) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART1_RX;
    if (mUsart == USART2) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART2_RX;
    if (mUsart == USART3) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART3_RX;
    //if (mUsart == USART4) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART4_RX;
    lstDmaInit.SrcAddress  = LL_USART_DMA_GetRegAddr(mUsart, LL_USART_DMA_REG_DATA_RECEIVE);
    lstDmaInit.DestAddress = 0;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_FIXED;
    lstDmaInit.DestIncMode = LL_DMA_DEST_INCREMENT;
    lstDmaInit.SrcDataWidth = LL_DMA_DEST_DATAWIDTH_HALFWORD; // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lstDmaInit.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;  
    lstDmaInit.Direction   = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT0; // Source is periph
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1; // Dest is memory
    LL_DMA_Init(GPDMA1, mu8DmaCh, &lstDmaInit);

    mu32DmaRxTr1    = mstDmaCh->CTR1;
    mu32DmaRxTr2    = mstDmaCh->CTR2;
    mu32DmaRxPeriph = mstDmaCh->CSAR;

    if (mUsart == USART1) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART1_TX;
    if (mUsart == USART2) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART2_TX;
    if (mUsart == USART3) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART3_TX;
    //if (mUsart == USART4) lstDmaInit.Request = LL_GPDMA1_REQUEST_USART4_TX;
    lstDmaInit.SrcAddress  = 0;
    lstDmaInit.DestAddress = LL_USART_DMA_GetRegAddr(mUsart, LL_USART_DMA_REG_DATA_TRANSMIT);
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_INCREMENT;
    lstDmaInit.DestIncMode = LL_DMA_DEST_FIXED;
    lstDmaInit.SrcDataWidth  = LL_DMA_DEST_DATAWIDTH_BYTE;
    lstDmaInit.DestDataWidth = LL_DMA_DEST_DATAWIDTH_HALFWORD;  // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lstDmaInit.Direction   = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT1; // Source is memory
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0; // Dest is periph
    LL_DMA_Init(GPDMA1, mu8DmaCh, &lstDmaInit);

    mu32DmaTxTr1    = mstDmaCh->CTR1;
    mu32DmaTxTr2    = mstDmaCh->CTR2;
    mu32DmaTxPeriph = mstDmaCh->CDAR;

    // DMA interrupt -- TX DMA interrupt wird nicht benötigt

    HAL_NVIC_SetPriority(mIrqChannelUsart, 6, 8); // Höhere Prio,wegen Asynchron
    HAL_NVIC_SetPriority(mIrqChannelDMA, 7, 8);  // Höhere Prio,wegen Asynchron
    vEnableIrq();
  }

  void vEnableIrq()
  {
    HAL_NVIC_EnableIRQ(mIrqChannelUsart);
    HAL_NVIC_EnableIRQ(mIrqChannelDMA);
  }

  void vDisableIrq()
  {
    HAL_NVIC_DisableIRQ(mIrqChannelUsart);
    HAL_NVIC_DisableIRQ(mIrqChannelDMA);
  }

  bool  bCheckBusy()
  {
    if (mUsart->ISR & USART_ISR_BUSY)
    {
      return False;
    }
    return True;
  }

  bool  bCheckPins()
  {
    //mPinTx.vSetMode(GPIO_MODE_INPUT);
    //mPinSDA->vSetMode(GPIO_MODE_INPUT);

    //Tx must be pulled high
    if (mPinTx->ui8Get() == 0)
    {
      return False;
    }
    return True;
  }
};


class cUartMpHdMaster : public cUartMpHd, public cComNodeMasterMulti
{
  public:
  cUsartMpHd_Master_Timer mTimer;

  cUartMpHdMaster(USART_TypeDef* lstUsart, u32 lu32Baudrate, cGpPin* lcTxRx, u8 lu8DmaCh, u16 luInitDelay_ms = 1)
    : cUartMpHd(lstUsart, lcTxRx, lu8DmaCh),
      cComNodeMasterMulti(luInitDelay_ms)
  {
    mu32Baudrate = lu32Baudrate;
  }

  cComNode::tenError enInitHw() override
  {
    cUartMpHd::vInitHw(0, mu32Baudrate, True);
    return cComNode::tenError::enNoError;
  }

  bool bCheckBusy() override
  {
    return True;
  }

  void vResetCom()
  {
    if (mSm != cComNode::tenState::enStIdle)
    {
      // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
      LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
      LL_USART_RequestRxDataFlush(mUsart);
      vStopDMA();

      mTimer.vStop();
      mUsart->ICR = 0xFFFF;

      mSm = cComNode::tenState::enStIdle;
      mError = cComNode::enNoError;
    }
  }

  // Start wird sowohl vom Interrupt wie auch von der Main benutzt
  // Um keine Laufzeit Konflikte zu bekommen, wird solange der Interrupt gesperrt
  void vPreStart() override
  {
    mUsart->ICR = 0xFFFF;
    cUartMpHd::vDisableIrq();
  }

  void vPostStart() override
  {
    cUartMpHd::vEnableIrq();
  }


  void vComError() override
  {
    cComNode::tenState lSm = mSm;
    cComNode::tenError lError = mError;

    vResetCom();

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(lError, lSm);
    }
  }


  void TIM_EV_IRQHandler()
  {
    if (mpcActiveSlave)
    {
      if (mu8SelfTimer)
      {
        u16 lu16DmaCounter = u16GetDmaCounterRx();
        if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
        {
          mu8SelfTimer = 0;
          mError = cComNode::enErHwTimerTimeout;
          vComError();
          vStart();
        }
        else  // Timer neu starten
        {
          mu8SelfTimer = 1;
          mu16DmaCounter2ms_Old = lu16DmaCounter;
          mTimer.vStart(250);
        }
      }
      else
      {
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvTimer);
      }
    }
  }

  void vStartTimer(u16 luTime_us)
  {
    mTimer.vStart(luTime_us);
  };

  void IrqHandler(cComNode::tenEvent lenEvent)
  {
    uint32 lui32ISR = mUsart->ISR;
    if (lui32ISR & 0xF)
    {
      mUsart->ICR = 0xF;
      mError = cComNode::enErOverrun;
      vComError();
    }
    else
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvDmaRxTc:
          mstDmaCh->CCR &= ~DMA_CCR_EN;
          ////GPDMA1->IFCR   = mChnRxFlagTc;
          vSm(lenEvent);
          break;
        case cComNode::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          mstDmaCh->CCR &= ~DMA_CCR_EN;
          mUsart->ICR  = LL_USART_ISR_TC;
          vSm(lenEvent);
          break;
        default:
          break;
      }
    }
    vStart();
  }

  void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
    mSm = cComNode::tenState::enStIdle;
    mpcActiveSlave->vComDone();
  }

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEvent);

    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
            case cComNode::tenEvent::enEvStartSkipAdr:
              // Rx
              if (mpcActiveMsg->isRx())
              {
                LL_USART_RequestRxDataFlush(mUsart);
                LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten

                vStartDMARx(mpcActiveMsg->mpu8Data, mpcActiveMsg->muLen);
                mu8SelfTimer = 1;
                mTimer.vStart(250);
                mSm = cComNode::tenState::enStWait;
              }
              else
              {
                // Tx
                if (mpcActiveMsg->isTx())
                {
                  LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                  // Ggf. Adresse senden
                  if (lenEvent == cComNode::tenEvent::enEvStart)
                  {
                    mUsart->TDR = mpcActiveSlave->mAdr | 0x100;
                  }
                  vStartDMATx(mpcActiveMsg->mpu8Data, mpcActiveMsg->muLen);
                  mSm = cComNode::tenState::enStEnd;
                }
                else
                {
                  vDone();
                }
              }
              break;
            default:
              break;
          }
        }
        break;
      case cComNode::tenState::enStWait:
        {
          if (lenEvent == cComNode::tenEvent::enEvDmaRxTc)
          {
            vDone();
          }
        }
        break;
      case cComNode::tenState::enStEnd:
        {
          if (lenEvent == cComNode::tenEvent::enEvUsartTc)
          {
            vDone();
          }
        }
        break;
      default:
        break;
    }
  }
};



class cUartMpHdSlave : public cUartMpHd, public cComNodeSlave
{
  public:
  cUsartMpHd_Slave_Timer mTimer;

  cUartMpHdSlave(USART_TypeDef *lstUsart, u32 lu32Baudrate, cGpPin* lcTxRx, u8 lu8DmaCh, u16 luInitDelay_ms = 1)
  : cUartMpHd(lstUsart, lcTxRx, lu8DmaCh),
    cComNodeSlave(luInitDelay_ms)
  {
	  mu32Baudrate = lu32Baudrate;
  }

  cComNode::tenError enInitHw() override
  {
    cUartMpHd::vInitHw(muAdr, mu32Baudrate, False);
    return cComNode::tenError::enNoError;
  }

  void vResetCom()
  {
    if (mSm != cComNode::tenState::enStIdle)
    {
      // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
      LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
      LL_USART_RequestRxDataFlush(mUsart);
      vStopDMA();

      mTimer.vStop();
      mUsart->ICR = 0xFFFF;

      mSm    = cComNode::tenState::enStIdle;
      mError = cComNode::enNoError;
    }
  }

  void vComError() override
  {
    cComNode::tenState lSm = mSm;
    cComNode::tenError lError = mError;

    vResetCom();

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(lError, lSm);
    }
  }

  void TIM_EV_IRQHandler()
  {
    if (mpcActiveSlave)
    {
      if (mu8SelfTimer)
      {
        u16 lu16DmaCounter = u16GetDmaCounterRx();
        if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
        {
          mu8SelfTimer = 0;
          mError = cComNode::enErHwTimerTimeout;
        }
        else  // Timer neu starten
        {
          mu8SelfTimer = 1;
          mu16DmaCounter2ms_Old = lu16DmaCounter;
          mTimer.vStart(250);
        }
      }
      else
      {
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvTimer);
      }
    }
  }

  void IrqHandler(cComNode::tenEvent lenEvent)
  {
    uint32 lui32ISR = mUsart->ISR;
    if (lui32ISR & 0xF)
    {
      mUsart->ICR = 0xF;
      mError = cComNode::enErOverrun;
      vComError();
    }

    else
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvDmaRxTc:
          mstDmaCh->CCR &= ~DMA_CCR_EN;
          ////GPDMA1->IFCR = mChnRxFlagTc;
          vSm(lenEvent);
          break;
        case cComNode::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          mstDmaCh->CCR &= ~DMA_CCR_EN;
          mUsart->ICR = LL_USART_ISR_TC;
          vSm(lenEvent);
          break;
        default:
          break;
      }
    }
  }

  void vMute()
  {
    LL_USART_RequestEnterMuteMode(mUsart);
  }

  void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
    mSm = cComNode::tenState::enStIdle;
    mpcActiveSlave->vComDone();
  }

  bool bCheckBusy() override
  {
    return True;
  }

  void vStartTimer(u16 luTime_us)
  {
    mTimer.vStart(luTime_us);
  };

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
      {
        if (!(LL_USART_IsActiveFlag_RWU(mUsart)))    // Nicht im Mute modus
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
            case cComNode::tenEvent::enEvStartSkipAdr:
              // Rx
              if (mpcActiveMsg->isRx())
              {
		            LL_USART_RequestRxDataFlush(mUsart);
		            LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten

                vStartDMARx(mpcActiveMsg->mpu8Data, mpcActiveMsg->muLen);
                mu8SelfTimer = 1;
                mTimer.vStart(250);
                mSm = cComNode::tenState::enStWait;
              }
              else
              {
                // Tx
                if (mpcActiveMsg->isTx())
                {
                  LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                  vStartDMATx(mpcActiveMsg->mpu8Data, mpcActiveMsg->muLen);
                  mSm = cComNode::tenState::enStEnd;
                }
                else
                {
                  vDone();
                }
              }
              break;

            case cComNode::tenEvent::enEvStartWait:
              // Rx
              if (mpcActiveMsg->isRx())
              {
                vMute();
		            LL_USART_RequestRxDataFlush(mUsart);
		            LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten

                vStartDMARx(mpcActiveMsg->mpu8Data, mpcActiveMsg->muLen);
                mSm = cComNode::tenState::enStWaitAdress;
              }
              else
              {
                vDone();
              }
              break;
            default:
              break;
          }
        }
      }
      break;

      case cComNode::tenState::enStWaitAdress:
        // HW is Muted
        // Wait for adress match and resulting wake up
        // and DMA is finished
        // Before that, PrepareRx has to be called
        if (lenEvent == cComNode::tenEvent::enEvDmaRxTc)
        {
          vDone();
        }
        break;

      case cComNode::tenState::enStWait:
        {
          if (lenEvent == cComNode::tenEvent::enEvDmaRxTc)
          {
            vDone();
          }
        }
        break;
      case cComNode::tenState::enStEnd:
        {
          if (lenEvent == cComNode::tenEvent::enEvUsartTc)
          {
            vDone();
          }
        }
        break;
      default:
        break;
    }
  }
};


#endif // __cUSART_MPHD_H__
