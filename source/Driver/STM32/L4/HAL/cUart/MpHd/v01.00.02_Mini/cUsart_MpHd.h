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
    return 0;
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
  cGpPin                mPinTx;
  USART_TypeDef*        mUsart;
  DMA_TypeDef*          mDma;
  DMA_Channel_TypeDef*  mChnTx;
  DMA_Channel_TypeDef*  mChnRx;
  u32                   mChnTxFlagTc;
  u32                   mChnRxFlagTc;


  uint8 mui8Adr;
  bool  mbMaster;
  cComNode::tenConsts menOption;

  cUartMpHd(USART_TypeDef *lstUsart, u32 lu32Baudrate, bool  lbMaster)
  {
    mUsart = lstUsart;

    mu32Baudrate = lu32Baudrate;

    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;

    mbMaster        = lbMaster;
    menOption       = cComNode::tenConsts::enNop;


    if (mUsart == USART1)
    {
      mDma = DMA2;
      mChnTx = DMA2_Channel6;
      mChnRx = DMA2_Channel7;
      mChnTxFlagTc = DMA_FLAG_TC6;
      mChnRxFlagTc = DMA_FLAG_TC7;
    }
    else
    if (mUsart == USART2)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel7;
      mChnRx = DMA1_Channel6;
      mChnTxFlagTc = DMA_FLAG_TC7;
      mChnRxFlagTc = DMA_FLAG_TC6;
    }
    else
    if (mUsart == USART3)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel2;
      mChnRx = DMA1_Channel3;
      mChnTxFlagTc = DMA_FLAG_TC2;
      mChnRxFlagTc = DMA_FLAG_TC3;
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
  u16 u16GetDmaCounter()
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

  void  vInitHw()
  {
    // Setup Pins
    if (mUsart == USART1) mPinTx.vInit(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART2) mPinTx.vInit(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART3) mPinTx.vInit(GPIOB_BASE,10, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);


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
    if (mbMaster)
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

    // DMA interrupt
    if (mUsart == USART1) lnNVIC_IRQChannel = DMA2_Channel7_IRQn; //USART1 connect to channel 7 of DMA2
    else if (mUsart == USART2) lnNVIC_IRQChannel = DMA1_Channel6_IRQn;
    else if (mUsart == USART3) lnNVIC_IRQChannel = DMA1_Channel3_IRQn;
    // Set the TIMx priority
    HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 7, 8);  // Höhere Prio,wegen Asynchron
    // Enable the TIMx global Interrupt
    HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);

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
    //if (mUsart == USART1) lnNVIC_IRQChannel = DMA2_Channel6_IRQn; //USART1 connect to channel 6 of DMA2
    //else if (mUsart == USART2) lnNVIC_IRQChannel = DMA1_Channel7_IRQn;
    //else if (mUsart == USART3) lnNVIC_IRQChannel = DMA1_Channel2_IRQn;
    //// Set the TIMx priority
    //HAL_NVIC_SetPriority(lnNVIC_IRQChannel, , 8);  // Höhere Prio,wegen Asynchron
    //// Enable the TIMx global Interrupt
    //HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);

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

          mPinTx.vSetMode(GPIO_MODE_AF_OD);
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
    mPinTx.vSetMode(GPIO_MODE_INPUT);
    //mPinSDA->vSetMode(GPIO_MODE_INPUT);

    //Tx must be pulled high
    if (mPinTx.ui8Get() == 0)
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

  cUartMpHdMaster(USART_TypeDef *lstUsart, uint32 lui32Slaves, u32 lu32Baudrate)
    : cUartMpHd(lstUsart, lu32Baudrate, True), mcSlaves(lui32Slaves)
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


  void vPrepareRx()
  {
    mu16TickCounter = 0;
    menComState = cComNode::tenState::enStWaitData;
    mSm = cComNode::tenState::enStWait;
    mu8SelfTimer = 1;
    vStartDMARx(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen);
    // Rx einschalten
    mUsart->CR1 |= USART_CR1_RE;
    mTimer.vStart(250);
  }

  void vHandleTimeout()
  {
    mSm = cComNode::tenState::enStIdle;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    mUsart->CR1 &= ~USART_CR1_RE;
    // flush Rx register
    mUsart->RQR |= USART_RQR_RXFRQ;
    vStopDMA();
    menComState = cComNode::tenState::enStIdle;
    mpcActiveMsg->vDone();
    mpcActiveSlave->vComError(cComNode::enErTimeout, menComState);
    mu8Lock = 0;
    bStartNext();
  }

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
          vHandleTimeout();
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

              // flush Rx register
              mUsart->RQR |= USART_RQR_RXFRQ;

              // Adresse nur senden wenn != 0xFF
              if (mpcActiveSlave->mAdr != 0xFF)
              {
                mUsart->TDR = mpcActiveSlave->mAdr | 0x100;
              }
              vStartDMATx(mpcActiveMsg->cTxData.mpu8Data, mpcActiveMsg->cTxData.muiLen);
              // Soll nach dem Senden noch was empfangen werden
              if (mpcActiveMsg->cRxData.muiLen > 0)
              {
                mSm = cComNode::tenState::enStTx;
              }
              else
              {
                mSm = cComNode::tenState::enStEnd;
              }
            }
            else
            if (mpcActiveMsg->cRxData.muiLen > 0)
            {
              vPrepareRx();
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
            vPrepareRx();
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
                    vHandleTimeout();
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
                mu8SelfTimer = 0;
                mTimer.vStop();
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

  cUsartMpHd_Slave_Timer mTimer;

  u8                   mu8SelfTimer;

  cUartMpHdSlave(USART_TypeDef *lstUsart, u32 lu32Baudrate)
  : cUartMpHd(lstUsart, lu32Baudrate, False)
  {
    mSm         = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;
    mpcComNode  = NULL;
    mpcMsg      = NULL;
    mu8SelfTimer = 0;
  }

  ~cUartMpHdSlave()
  {
  }

  void vTick1ms()
  {
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    if (mpcComNode != null)
    {
      mui8Adr = mpcComNode->mAdr;
      vInitHw();
    }
  }


  void vError(cComNode::tenError lenError)
  {
    mSm = cComNode::tenState::enStIdle;
    menErrorActive = lenError;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
    LL_USART_RequestRxDataFlush(mUsart);
    vStopDMA();
    cUartMpHd::vInitHw();
    mpcMsg->vDone();
    mpcComNode->vComError(lenError, cComNode::tenState::enStError);
  }

  void TIM_EV_IRQHandler()
  {
    if (mpcComNode)
    {
      if (mu8SelfTimer)
      {
        u16 lu16DmaCounter = u16GetDmaCounter();
        if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
        {
          mu8SelfTimer = 0;
          vError(cComNode::enErTimeout);
          bStartNext();
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
        mpcComNode->vComStart(cComNode::tenEvent::enEvMisc);
      }
    }
  }

  void ComIrqHandler(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    if (lenEventType == cComNode::tenEventType::enEvTyError)
    {
      if (lenEvent == cComNode::tenEvent::enEvUsartErOre)
      {
        // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
        vError(cComNode::enErOverrun);
      }
      else
      {
        vError(cComNode::enErUnknown);
      }
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



  void vSetMsg(cComDatMsg* lpcMsg)
  {
    mpcMsg = lpcMsg;
  };

  inline void vMute()
  {
    LL_USART_RequestEnterMuteMode(mUsart);
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
    vStopDMA();
    mSm = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;	
    mpcMsg->vDone();
    mpcComNode->vComDone();
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
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
              menComState = cComNode::tenState::enStWaitAdressAndData;
              LL_USART_RequestRxDataFlush(mUsart);
              LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten
              vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);

              if (!(LL_USART_IsActiveFlag_RWU(mUsart)))    // Nicht im Mute modus
              {
                mu8SelfTimer = 1;
                mTimer.vStart(250);
              }
              mSm = cComNode::tenState::enStWait;
            }
            else
            {
              // Tx
              if (mpcMsg->cTxData.muiLen > 0)
              {
                menComState = cComNode::tenState::enStData;
                LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
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
        break;

      case cComNode::tenState::enStWait:
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvDmaRxTc:
              {
                // Soll nach dem Empfangen noch was gesendet werden
                if (mpcMsg->cTxData.muiLen > 0)
                {
                  menComState = cComNode::tenState::enStData;
                  LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
                  vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
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

      if (!mpcComNode->mStatus.IsStartRequested)
      {
        lbStart = false;
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
