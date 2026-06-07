#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_lpuart.h"
#include "stm32l4xx_ll_dma.h"

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
  USART_TypeDef*        mUsart;
  cGpPin                mPinTx;
  DMA_TypeDef*          mDma;
  DMA_Channel_TypeDef*  mChnTx;
  DMA_Channel_TypeDef*  mChnRx;
  u32                   mChnTxFlagTc;
  u32                   mChnRxFlagTc;
  IRQn_Type             mIrqChannelUsart;
  IRQn_Type             mIrqChannelDMA;



  cUartMpHd(USART_TypeDef *lstUsart)
  {
    mUsart = lstUsart;
    mu8SelfTimer = 0;

    if (mUsart == USART1)
    {
      mDma = DMA2;
      mChnTx = DMA2_Channel6;
      mChnRx = DMA2_Channel7;
      mChnTxFlagTc = DMA_FLAG_TC6;
      mChnRxFlagTc = DMA_FLAG_TC7;

      mIrqChannelDMA =   DMA2_Channel7_IRQn; //USART1 connect to channel 7 of DMA2
      mIrqChannelUsart = USART1_IRQn;
    }
    else
    if (mUsart == USART2)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel7;
      mChnRx = DMA1_Channel6;
      mChnTxFlagTc = DMA_FLAG_TC7;
      mChnRxFlagTc = DMA_FLAG_TC6;

      mIrqChannelDMA   = DMA1_Channel6_IRQn;
      mIrqChannelUsart = USART2_IRQn;
    }
    else
    if (mUsart == USART3)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel2;
      mChnRx = DMA1_Channel3;
      mChnTxFlagTc = DMA_FLAG_TC2;
      mChnRxFlagTc = DMA_FLAG_TC3;

      mIrqChannelDMA = DMA1_Channel3_IRQn;
      mIrqChannelUsart = USART3_IRQn;
    }
  }

  void  vStopDMA()
  {
    LL_USART_DisableDMAReq_RX(mUsart);
    LL_USART_DisableDMAReq_TX(mUsart);
    mChnTx->CCR &=  ~DMA_CCR_EN; // USART1_TX
    mChnRx->CCR &=  ~DMA_CCR_EN; // USART1_RX
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return mChnRx->CNDTR;
  }

  void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mu16DmaCounter2ms_Old = BufferSize;
    mDma->IFCR = mChnRxFlagTc;
    mChnRx->CNDTR = BufferSize;
    mChnRx->CMAR = (uint32)pBuffer;
    mChnRx->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
    LL_USART_EnableDMAReq_RX(mUsart);
  }

  void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    mDma->IFCR = mChnTxFlagTc;
    mChnTx->CNDTR = BufferSize;
    mChnTx->CMAR  = (uint32)pBuffer;
    // Ende der Übertragung wird durch  Usart.TC interrupt erledigt
    // Tx DMA Interrupt wird nicht benötigt
    mChnTx->CCR  |=  (DMA_CCR_EN /*| DMA_CCR_TCIE*/);
    LL_USART_EnableDMAReq_TX(mUsart);
  }

  void  vInitHw(u8 lui8Adr, u32 lu32Baudrate, bool lbMaster)
  {
    // Setup Pins
    if (mUsart == USART1) mPinTx.vInit(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART2) mPinTx.vInit(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART3) mPinTx.vInit(GPIOB_BASE,10, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);


    /* Restart USART clock */
    if (mUsart == USART1)      { __HAL_RCC_USART1_FORCE_RESET(); __HAL_RCC_USART1_RELEASE_RESET(); __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_SYSCLK); }
    else if (mUsart == USART2) { __HAL_RCC_USART2_FORCE_RESET(); __HAL_RCC_USART2_RELEASE_RESET(); __HAL_RCC_USART2_CONFIG(RCC_USART2CLKSOURCE_SYSCLK); }
    else if (mUsart == USART3) { __HAL_RCC_USART3_FORCE_RESET(); __HAL_RCC_USART3_RELEASE_RESET(); __HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_SYSCLK); }

    // Enable DMA
    if (mUsart == USART1) __HAL_RCC_DMA2_CLK_ENABLE();
    else if (mUsart == USART2) __HAL_RCC_DMA1_CLK_ENABLE();
    else if (mUsart == USART3) __HAL_RCC_DMA1_CLK_ENABLE();

    vStopDMA();

    //SDA and SCL must be pulled high
    if (lbMaster)
    {
      if (!bCheckPins()) return;
    }

    //Single wire, only TX line
    if (mUsart == USART1) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART1);}
    else if (mUsart == USART2) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART2);}
    else if (mUsart == USART3) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART3);}

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
    // DMA Rx
    DMA_HandleTypeDef lhDMA = {};

    lhDMA.Instance = mChnRx;

    lhDMA.Init.Request             = DMA_REQUEST_2;
    lhDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&lhDMA);

    // Configure DMA Channel data length
    lhDMA.Instance->CNDTR = 0;
    // Configure DMA Channel destination address
    lhDMA.Instance->CPAR = (uint32_t)&(mUsart->RDR);
    // Configure DMA Channel source address
    lhDMA.Instance->CMAR = 0;


    // DMA Tx
    lhDMA.Instance = mChnTx;

    lhDMA.Init.Request             = DMA_REQUEST_2;
    lhDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&lhDMA);

    // Configure DMA Channel data length
    lhDMA.Instance->CNDTR = 0;
    // Configure DMA Channel destination address
    lhDMA.Instance->CPAR = (uint32)&(mUsart->TDR);
    // Configure DMA Channel source address
    lhDMA.Instance->CMAR = 0;

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
    if (mPinTx.ui8Get() == 0)
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

  cUartMpHdMaster(USART_TypeDef* lstUsart, u32 lu32Baudrate, u16 luInitDelay_ms = 1)
    : cUartMpHd(lstUsart),
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
          mChnRx->CCR &= ~DMA_CCR_EN;
          mDma->IFCR   = mChnRxFlagTc;
          vSm(lenEvent);
          break;
        case cComNode::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          mChnTx->CCR &= ~DMA_CCR_EN;
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

  cUartMpHdSlave(USART_TypeDef *lstUsart, u32 lu32Baudrate, u16 luInitDelay_ms = 1)
  : cUartMpHd(lstUsart),
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
          mChnRx->CCR &= ~DMA_CCR_EN;
          mDma->IFCR = mChnRxFlagTc;
          vSm(lenEvent);
          break;
        case cComNode::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          mChnTx->CCR &= ~DMA_CCR_EN;
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
