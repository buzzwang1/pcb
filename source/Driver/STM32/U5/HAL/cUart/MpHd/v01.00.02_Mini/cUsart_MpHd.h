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


class cComNodeUsartMpHd_Slave : public cComNode
{
public:
  cComDatMsg mpcMsg;

  cComNodeUsartMpHd_Slave(uint32 lui32TxDataSize, uint32 lui32RxDataSize)
  {
    mpcMsg.vMemAlloc(lui32TxDataSize, lui32RxDataSize);
  };

  //~cComNodeI2c_Slave() {};

  inline uint32 SizeOf()
  {
    return mpcMsg.SizeOf() -
           sizeof(mpcMsg) +
           sizeof(cComNodeUsartMpHd_Slave);
  }
};


class cComNodeListUsartMpHd_Master :public cComNodeList
{
  public:
    cComNodeListUsartMpHd_Master(uint32 lui32SlavesCnt)
      :cComNodeList(lui32SlavesCnt)
    {}

  inline uint32 SizeOf()
  {
    return cComNodeList::SizeOf() +
           sizeof(cComNodeListUsartMpHd_Master);
  }
};


class cUartMpHd
{
  public:

  cComNode::tenState   mSm;
  tenComDeviceStatus   munDeviceStatus;
  tenComBusStatus      munBusStatus;
  tenComStatus         munComStatus;

  u16                  mu16TickCounter;
  u16                  mu16DmaCounter2ms_Old;

  u32                  mu32Baudrate;

  //Single Wire, only Tx Pin
  cGpPin        *mPinTx;
  USART_TypeDef *mUsart;

  u8    mu8DmaCh;
  DMA_Channel_TypeDef* mstDmaCh;
  u32   mu32DmaRxTr1;
  u32   mu32DmaRxTr2;
  u32   mu32DmaRxPeriph;
  u32   mu32DmaTxTr1;
  u32   mu32DmaTxTr2;
  u32   mu32DmaTxPeriph;

  uint8 mui8Adr;
  bool  mbMaster;
  cComNode::tenConsts menOption;

  cUartMpHd(USART_TypeDef *lstUsart, u32 lu32Baudrate, cGpPin* lcRxTx, u8 lu8DmaCh, bool  lbMaster)
  {
    mUsart = lstUsart;

    mu32Baudrate = lu32Baudrate;

    mPinTx   = lcRxTx;

    mu8DmaCh = lu8DmaCh;
    mstDmaCh = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[mu8DmaCh]));

    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;

    mbMaster        = lbMaster;
    menOption       = cComNode::tenConsts::enNop;
  }

  void  vStopDMA()
  {
    LL_USART_DisableDMAReq_RX(mUsart);
    LL_USART_DisableDMAReq_TX(mUsart);
    mstDmaCh->CCR &= ~DMA_CCR_EN;
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    return (mstDmaCh->CBR1 & 0xFFFF);
  }

  void  vStartDMA(uint8* pBuffer, uint32 BufferSize, cComNode::tenEvent lenDirection)
  {
    // Initialize the DMA with the new parameters
    if (lenDirection == cComNode::tenEvent::enEvPrepareForTx)
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
    else // Reception
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
  }

  void  vInitHw()
  {
    /* Disable USART clock */
    if (mUsart == USART1)      { __HAL_RCC_USART1_FORCE_RESET(); __HAL_RCC_USART1_RELEASE_RESET();}
    else if (mUsart == USART2) { __HAL_RCC_USART2_FORCE_RESET(); __HAL_RCC_USART2_RELEASE_RESET(); }
    else if (mUsart == USART3) { __HAL_RCC_USART3_FORCE_RESET(); __HAL_RCC_USART3_RELEASE_RESET(); }

    // Enable DMA
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    vStopDMA();

    //SDA and SCL must be pulled high
    if (mbMaster)
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

    lstInit.BaudRate            = mu32Baudrate;
    lstInit.DataWidth           = LL_USART_DATAWIDTH_9B;
    lstInit.StopBits            = LL_USART_STOPBITS_1;
    lstInit.Parity              = LL_USART_PARITY_NONE;
    lstInit.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    lstInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    lstInit.OverSampling        = LL_USART_OVERSAMPLING_8;

    LL_USART_Init(mUsart, &lstInit);

    LL_USART_EnableHalfDuplex(mUsart);
    LL_USART_ConfigNodeAddress(mUsart, LL_USART_ADDRESS_DETECT_4B, mui8Adr);

    if (!mbMaster)
    {
      LL_USART_EnableMuteMode(mUsart);
      LL_USART_SetWakeUpMethod(mUsart, LL_USART_WAKEUP_ADDRESSMARK);
    }

    // Usart Peripheral Enable
    LL_USART_Enable(mUsart);
    mUsart->ICR = 0xFFFFFFFF;

    if (!mbMaster)
    {
      LL_USART_RequestEnterMuteMode(mUsart);
    }

    if (isErrorActive())
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
      return;
    }

    if (mUsart->ISR & USART_ISR_BUSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      return;
    }

    // USART TC interrupt
    IRQn_Type lnNVIC_IRQChannel;
    if (mUsart == USART1) lnNVIC_IRQChannel = USART1_IRQn;
    else if (mUsart == USART2) lnNVIC_IRQChannel = USART2_IRQn;
    else /*if (mUsart == USART3)*/ lnNVIC_IRQChannel = USART3_IRQn;

    // Set the priority
    HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 6, 8); // Höhere Prio,wegen Asynchron
    // Enable the TIMx global Interrupt
    HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);

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

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption)
  {
    menOption = lenOption;
  }

  void vTick10ms()
  {
    cComNode::tenConsts lenOption;
    lenOption = menOption;
    if (lenOption != cComNode::tenConsts::enNop)
    {
      if (lenOption == cComNode::enResetHwFull)
      {
        vInitHw();
      }
      else  // enResetHwDma
      {
        // Usart Peripheral Disnable
        LL_USART_Disable(mUsart);
        vStopDMA();

        if (mbMaster)
        {
          //SDA and SCL must be pulled high
          if (!bCheckPins()) return;

          mPinTx->vSetMode(GPIO_MODE_AF_OD);
        }

        mUsart->ICR = 0xFFFFFFFF;

        // Usart Peripheral Enable
        LL_USART_Enable(mUsart);

        munDeviceStatus = tenComDeviceStatus::enDeviceReady;
        munBusStatus    = tenComBusStatus::enBusNoError;
        munComStatus    = tenComStatus::enStatusIdle;
        mSm             = cComNode::tenState::enStIdle;
      }

      if (isErrorActive())
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
        munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
        return;
      }

      if (mUsart->ISR & USART_ISR_BUSY)
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
        munBusStatus    = tenComBusStatus::enBusNotReady;
        return;
      }

      if (munDeviceStatus == tenComDeviceStatus::enDeviceReady)
      {
        menOption = cComNode::tenConsts::enNop;
      }
    }
  }

  bool  bCheckBus()
  {
    if (mUsart->ISR & USART_ISR_BUSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      mSm = cComNode::enStError;
      return False;
    }
    return True;
  }

  bool  bCheckPins()
  {
    mPinTx->vSetMode(GPIO_MODE_INPUT);
    //mPinSDA->vSetMode(GPIO_MODE_INPUT);

    //Tx must be pulled high
    if (mPinTx->ui8Get() == 0)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorPins;
      mSm             = cComNode::enStError;
      return False;
    }
    return True;
  }
  uint8 isErrorActive()
  {
    return 0;
  }
};



class cUartMpHdMaster : public cUartMpHd
{
  public:

  uint16             mu16ReInitTicks;
  uint16             mu16ComStuckTicks;
  uint16             mu16ComStuckTicksReload;


  cComNodeListUsartMpHd_Master  mcSlaves;
  cComNode                      *mpcActiveSlave;
  cComDatMsg                    *mpcActiveMsg;

  cComNode::tenState   menComState;
  uint8               *mpui8ComBuf;
  uint32               mui32ComByteCnt;

  u8                   mu8Lock;
  u8                   mu8SelfTimer;

  cBotNet_DownLinkUsartMpHd_Timer mTimer;

  cUartMpHdMaster(USART_TypeDef *lstUsart, uint32 lui32Slaves, u32 lu32Baudrate, cGpPin* lcTxRx, u8 lu8DmaCh)
    : cUartMpHd(lstUsart, lu32Baudrate, lcTxRx, lu8DmaCh, True), mcSlaves(lui32Slaves)
  {
    vInit();
  }

  void vInit()
  {
    mSm = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;

    mpcActiveSlave = NULL;
    mpcActiveMsg = NULL;

    mui8Adr = 0;
    mu8Lock = 0;
    mu8SelfTimer = 0;

    mu16ReInitTicks = 100;
    mu16ComStuckTicksReload = 2000;
    mu16ComStuckTicks = mu16ComStuckTicksReload;

    vInitHw();
  }

  ~cUartMpHdMaster()
  {
  }

  inline uint32 SizeOf()
  {
    return mcSlaves.SizeOf() + sizeof(cUartMpHdMaster);
  }

  inline void SetLock()   {mu8Lock = 1;}
  inline void ResetLock() {mu8Lock = 0;}

  void TIM_EV_IRQHandler()
  {
    if (mpcActiveSlave)
    {
      if (mu8SelfTimer)
      {
        mu8SelfTimer = 0;
        u16 lu16DmaCounter = u16GetDmaCounter();
        if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
        {
          mu16TickCounter = 5;
          vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);
        }
      }
      else
      {
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvMisc);
      }
    }
  }

  void ComIrqHandler(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    if (lenEventType == cComNode::tenEventType::enEvTyError)
    {
      mSm = cComNode::tenState::enStIdle;
      vStopDMA();
      mu8Lock = 0;
      mTimer.vStop();
      mUsart->ICR = 0xFFFFFFFF;
      if (mpcActiveMsg) mpcActiveMsg->vDone();
      if (mpcActiveSlave) mpcActiveSlave->vComError(cComNode::enErOverrun, menComState);
    }
    else
    {
      vSm(lenEventType, lenEvent);
    }

    if (mSm == cComNode::tenState::enStIdle)
    {
      bStartNext();
    }
  }

  void vSetReInitTicks(u16 lu16ReInitTicks)
  {
    if (!mu16ReInitTicks)
    {
      mu16ReInitTicks = lu16ReInitTicks;
    }
  }

  void vTick1ms()
  {
    mu16TickCounter++;
    vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);

    if (mSm == cComNode::enStError)
    {
      if (mu16ReInitTicks)
      {
        mu16ReInitTicks--;

        if (!mu16ReInitTicks)
        {
          mpcActiveMsg   = NULL;
          mu8Lock        = 0;
          this->vReInitHw(cComNode::tenConsts::enResetHwFull);
          this->vReInitAllSlave();
        }
      }
    }
    else
    {
      mu16ReInitTicks   = 100;
    }

    if (munComStatus != tenComStatus::enStatusStuck)
    {
      if (mu16ComStuckTicks)
      {
        if (mSm != cComNode::tenState::enStIdle)
        {
          mu16ComStuckTicks--;
        }
      }
      else
      {
        mSm = cComNode::enStError;
        munDeviceStatus   = tenComDeviceStatus::enDeviceReady;
        munBusStatus      = tenComBusStatus::enBusNoError;
        munComStatus      = tenComStatus::enStatusStuck;
        mu16ComStuckTicks = mu16ComStuckTicksReload;
        mu16ReInitTicks   = 100;
      }
    }
  }


  void vAddSlave(cComNode *lpcSlave)
  {
    mcSlaves.vAdd(lpcSlave);
  }

  void vRemoveSlave(cComNode *lpcSlave)
  {
    mcSlaves.vRemove(lpcSlave);
  }

  void vReInitAllSlave()
  {
    mcSlaves.vSetErrorAll(cComNode::tenError::enErReInit, cComNode::cComNode::tenState::enStIdle);
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    cComNode::tenState mSm_Old;

    UNUSED(lenEventType);

    menComState = cComNode::tenState::enStIdle;

    mSm_Old = mSm;
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvStart:
            //TX
            if (mpcActiveMsg->cTxData.muiLen > 0)
            {
              menComState = cComNode::tenState::enStAdressAndData;

              // Rx ausschalten
              mUsart->CR1 &= ~USART_CR1_RE;
              mUsart->CR1 |= USART_CR1_TE;

              // Adresse nur senden wenn != 0xFF
              if (mpcActiveSlave->mAdr != 0xFF)
              {
                mUsart->TDR = mpcActiveSlave->mAdr | 0x100;
              }
              vStartDMA(mpcActiveMsg->cTxData.mpu8Data, mpcActiveMsg->cTxData.muiLen, cComNode::enEvPrepareForTx);
              // Soll nach dem Senden noch was empfangen werden
              if (mpcActiveMsg->cRxData.muiLen > 0)
              {
                mSm = cComNode::tenState::enStTx;
                vStartDMA(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen, cComNode::enEvPrepareForRx);
              }
              else
              {
                mSm = cComNode::tenState::enStEnd;
              }
            }
            else
            if (mpcActiveMsg->cRxData.muiLen > 0)
            {
              mu16TickCounter = 0;
              menComState = cComNode::tenState::enStWaitData;
              // Rx einschalten
              mUsart->CR1 |= USART_CR1_RE;
              vStartDMA(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen, cComNode::enEvPrepareForRx);
              mSm = cComNode::tenState::enStWait;
              mu8SelfTimer = 1;
              mTimer.vStart(250);
            }
            else
            {
              mSm = cComNode::tenState::enStIdle;
              mpcActiveMsg->vDone();
              mpcActiveSlave->vComDone();
            }
            break;
          default:
            break;
        }
        break;
      case cComNode::tenState::enStTx:
        {
          if (lenEvent == cComNode::tenEvent::enEvUsartTc)
          {
             // Rx einschalten
             mUsart->CR1 |= USART_CR1_RE;
             mu16TickCounter = 0;
             menComState = cComNode::tenState::enStWaitData;
             mSm = cComNode::tenState::enStWait;
             mu8SelfTimer = 1;
             mTimer.vStart(250);
          }
        }
        break;
      case cComNode::tenState::enStWait:
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvTick:
              {
                if (mu16TickCounter >= 3) // Überprüfen, ob in den letzten 2ms etwas empfangen wurde
                {
                  u16 lu16DmaCounter = u16GetDmaCounter();

                  if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
                  {
                    mSm = cComNode::tenState::enStIdle;
                    menComState = cComNode::tenState::enStIdle;
                    mpcActiveMsg->vDone();
                    mpcActiveSlave->vComError(cComNode::enErTimeout, menComState);
                    mu8Lock = 0;
                    vStopDMA();
                    bStartNext();
                  }
                  else
                  {
                    mu16TickCounter = 0;
                    mu16DmaCounter2ms_Old = lu16DmaCounter;
                  }
                }
              }
            break;
            case cComNode::tenEvent::enEvDmaRxTc:
              {
                // Rx ausschalten
                mUsart->CR1 &= ~USART_CR1_RE;
                mSm = cComNode::tenState::enStIdle;
                menComState = cComNode::tenState::enStIdle;
                if (mu8SelfTimer)
                {
                  mu8SelfTimer = 0;
                  mTimer.vStop();
                }
                vStopDMA();
                mpcActiveMsg->vDone();
                mpcActiveSlave->vComDone();
              }
            break;
            default:
            break;
          }
        }
        break;
      case cComNode::tenState::enStEnd:
        {
          if (lenEvent == cComNode::tenEvent::enEvUsartTc)
          {
            // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
            mUsart->CR1 &= ~USART_CR1_RE;
            mSm = cComNode::tenState::enStIdle;
            menComState = cComNode::tenState::enStIdle;
            if (mu8SelfTimer)
            {
              mu8SelfTimer = 0;
              mTimer.vStop();
            }
            vStopDMA();
            mpcActiveMsg->vDone();
            mpcActiveSlave->vComDone();
          }
        }
        break;
      default:
        break;
    }

    if (mSm_Old != mSm)
    {
      mu16ComStuckTicks = mu16ComStuckTicksReload;
    }
  }

  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (mpcActiveMsg != NULL)
      {
        if (mpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        lbStart = bCheckBus();
      }

      if (lbStart)
      {
        mpcActiveMsg = lpcActiveMsg;
        mpcActiveMsg->vBusy();
        vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
      }
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if ((mSm == cComNode::tenState::enStIdle) && (!mu8Lock))
    {
      bool lbStart;

      lbStart = true;

      if (mpcActiveMsg != NULL)
      {
        if (mpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        lbStart &= (!mcSlaves.isEmpty());
      }

      if (lbStart)
      {
        mpcActiveSlave = (cComNode*)mcSlaves.cGetNext();
        mpcActiveSlave->mStatus.IsStartRequested = 0;
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
      }
    }
    return True;
  }
};



class cUartMpHdSlave : public cUartMpHd
{
  public:
  cComDatMsg          *mpcMsg;

  cComNode            *mpcComNode;
  cComNode::tenState   menComState;
  cComNode::tenError   menErrorActive;

  cBotNet_DownLinkUsartMpHd_Timer mTimer;

  cUartMpHdSlave(USART_TypeDef *lstUsart, u32 lu32Baudrate, cGpPin* lcTxRx, u8 lu8DmaCh)
  : cUartMpHd(lstUsart, lu32Baudrate, lcTxRx, lu8DmaCh, False)
  {
    mSm         = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;
    mpcComNode  = NULL;
  }

  ~cUartMpHdSlave()
  {
  }

  void vTick1ms()
  {
    mu16TickCounter++;
    vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    mui8Adr    = mpcComNode->mAdr;
    if (mpcComNode != null)
    {
      vInitHw();
    }
  }

  void TIM_EV_IRQHandler()
  {
    if (mpcComNode)
    {
      mpcComNode->vComStart(cComNode::enEvMisc);
    }
  }

  void ComIrqHandler(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    if (lenEventType == cComNode::tenEventType::enEvTyError)
    {
      mSm = cComNode::tenState::enStIdle;
      vStopDMA();
      mTimer.vStop();
      mUsart->ICR = 0xFFFFFFFF;
      mpcMsg->vDone();
      mpcComNode->vComError(cComNode::enErOverrun, menComState);
    }
    else
    {
      vSm(lenEventType, lenEvent);
    }

    if (mSm == cComNode::tenState::enStIdle)
    {
      bStartNext();
    }
  }

  void  vError()
  {
  }

  void vSetMsg(cComDatMsg* lpcMsg)
  {
    mpcMsg = lpcMsg;
  };

  inline void vMute()
  {
    LL_USART_RequestEnterMuteMode(mUsart);
  }


  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
    if (isErrorActive())
    {
      vError();
      return;
    }

    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvStart:
            // Beim slave Rx vor Tx.
            // Rx
            if (mpcMsg->cRxData.muiLen > 0)
            {
              mu16TickCounter = 0;
              menComState = cComNode::tenState::enStWaitAdressAndData;
              LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten
              vStartDMA(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen, cComNode::enEvPrepareForRx);
              mSm = cComNode::tenState::enStWait;
            }
            else
            {
              // Tx
              if (mpcMsg->cTxData.muiLen > 0)
              {
                menComState = cComNode::tenState::enStData;
                LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                vStartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, cComNode::enEvPrepareForTx);
                mSm = cComNode::tenState::enStEnd;
              }
              else
              {
                mSm = cComNode::tenState::enStIdle;
                mpcMsg->vDone();
                mpcComNode->vComDone();
              }
            }
            break;
          default:
            break;
        }
        break;

      case cComNode::tenState::enStWait:
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvTick:
              {
                if (mu16TickCounter >= 3) // Überprüfen, ob in den letzten 3ms etwas empfangen wurde
                {
                  u16 lu16DmaCounter = u16GetDmaCounter();

                  if ((mu16DmaCounter2ms_Old == lu16DmaCounter) && // Counter hat sich nicht geändert
                      (!(mUsart->ISR & USART_ISR_RWU)))            // Nicht in Mute modus
                  {
                    mSm = cComNode::tenState::enStIdle;
                    vStopDMA();
                    menComState = cComNode::tenState::enStIdle;
                    mpcMsg->vDone();
                    mpcComNode->vComError(cComNode::enErTimeout, menComState);
                    bStartNext();
                  }
                  else
                  {
                    mu16TickCounter = 0;
                    mu16DmaCounter2ms_Old = lu16DmaCounter;
                  }
                }
              }
            break;
            case cComNode::tenEvent::enEvDmaRxTc:
              {
                // Soll nach dem Empfangen noch was gesendet werden
                if (mpcMsg->cTxData.muiLen > 0)
                {
                  menComState = cComNode::tenState::enStData;
                  LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                  vStartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, cComNode::enEvPrepareForTx);
                  mSm = cComNode::tenState::enStEnd;
                }
                else
                {
                  mSm = cComNode::tenState::enStIdle;
                  menComState = cComNode::tenState::enStIdle;
                  mpcMsg->vDone();
                  mpcComNode->vComDone();
                }
              }
            break;
            default:
            break;
          }
        }
        break;
      case cComNode::tenState::enStEnd:
        {
          if (lenEvent == cComNode::tenEvent::enEvUsartTc)
          {
            // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
            LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_NONE);
            mSm = cComNode::tenState::enStIdle;
            menComState = cComNode::tenState::enStIdle;
            mpcMsg->vDone();
            mpcComNode->vComDone();
          }
        }
        break;
      default:
        break;
    }
  }

  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (mpcMsg != NULL)
      {
        if (mpcMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        mpcMsg = lpcActiveMsg;
        mpcMsg->vBusy();
        vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
      }
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (mpcMsg != NULL)
      {
        if (mpcMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (mpcComNode != NULL)
      {
        if (!mpcComNode->mStatus.IsStartRequested)
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        mpcComNode->mStatus.IsStartRequested = 0;
        mpcComNode->vComStart(cComNode::enEvPrepareToReceiveData);
      }
    }
    return True;
  }
};


#endif // __cUSART_MPHD_H__
