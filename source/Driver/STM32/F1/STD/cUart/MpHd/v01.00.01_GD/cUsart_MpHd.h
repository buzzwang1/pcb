#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_usart.h"
#include "gd32f1x0_dma.h"


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
  cGpPin             mPinTx;
  u32                mUsart;

  uint8 mui8Adr;

  cUartMpHd(u32 lu32Usart, u32 lu32Baudrate)
  {
    mUsart = lu32Usart;

    mu32Baudrate = lu32Baudrate;

    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
  }

  void  vStopDMA()
  {
    usart_dma_receive_config(mUsart,  USART_DENT_DISABLE);
    usart_dma_transmit_config(mUsart, USART_DENT_DISABLE);
    if (mUsart == USART0)
    {
      dma_channel_enable(DMA_CH1);
      dma_channel_enable(DMA_CH2);
    }
    else
    if (mUsart == USART1)
    {
      dma_channel_enable(DMA_CH3);
      dma_channel_enable(DMA_CH4);
    }
  }

  // Überpruft ob sich der Rx DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    if (mUsart == USART0) return DMA_CH2CNT;
    if (mUsart == USART1) return DMA_CH4CNT;
    return 0;
  }  
  
  void  vStartDMA(uint8* pBuffer, uint32 BufferSize, cComNode::tenEvent lenDirection)
  {
    // Initialize the DMA with the new parameters
    if (lenDirection == cComNode::tenEvent::enEvPrepareForTx)
    {
      if (mUsart == USART0)
      {
        DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH1);
        DMA_CH1CNT    = BufferSize;
        DMA_CH1MADDR  = (uint32)pBuffer;
        DMA_CH1CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      }
      else
      if (mUsart == USART1)
      {
        DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH3);
        DMA_CH3CNT    = BufferSize;
        DMA_CH3MADDR  = (uint32)pBuffer;
        DMA_CH3CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      }
       usart_dma_transmit_config(mUsart, USART_DENT_ENABLE);
    }
    else // Reception
    {
      mu16DmaCounter2ms_Old = BufferSize;
      if (mUsart == USART0)
      {
        DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH2);
        DMA_CH2CNT    = BufferSize;
        DMA_CH2MADDR  = (uint32)pBuffer;
        DMA_CH2CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      }
      else
      if (mUsart == USART1)
      {
        DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH4);
        DMA_CH4CNT    = BufferSize;
        DMA_CH4MADDR  = (uint32)pBuffer;
        DMA_CH4CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      }
      usart_dma_receive_config(mUsart,  USART_DENT_ENABLE);
    }
  }

  void  vInitHw(bool lbMaster)
  {
    // Setup Pins
    if (mUsart == USART0)      mPinTx.vInit(GPIOA, 9, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, 1);
    else if (mUsart == USART1) mPinTx.vInit(GPIOA, 2, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, 1);    
    mPinTx.vSetAF(GPIO_AF_2);    


    // Enable USART clock
    if (mUsart == USART0) rcu_periph_clock_disable(RCU_USART0);
    else if (mUsart == USART1) rcu_periph_clock_disable(RCU_USART1);

    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    vStopDMA();

    //Tx must be pulled high
    if (lbMaster)
    {
      if (!bCheckPins()) return;
    }


    // Usart Periph clock enable
    if (mUsart == USART0)      rcu_periph_clock_enable(RCU_USART0);
    else if (mUsart == USART1) rcu_periph_clock_enable(RCU_USART1);

    // Usart Peripheral Disable
    usart_deinit(mUsart);
    usart_baudrate_set(mUsart,mu32Baudrate);
    usart_transmit_config(mUsart, USART_TRANSMIT_ENABLE);
    usart_receive_config(mUsart,  USART_RECEIVE_ENABLE);

    usart_oversample_config(USART0, USART_OVSMOD_8);
    usart_halfduplex_enable(USART0);
    usart_address_config(USART0, mui8Adr);
    usart_address_detection_mode_config(USART0, USART_ADDM_4BIT);


    if (!lbMaster)
    {
      usart_mute_mode_enable(mUsart);
      usart_mute_mode_wakeup_config(mUsart, USART_WM_ADDR);
    }

    // Usart Peripheral Enable
    usart_enable(mUsart);
    USART_INTC(mUsart) = 0xFFFFFFFF;

    if (!lbMaster)
    {
      USART_CMD(mUsart) |= USART_CMD_MMCMD;
    }

    if (isErrorActive(lbMaster))
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
      return;
    }

    if (USART_STAT(mUsart) & USART_STAT_BSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      return;
    }


    // USART TC interrupt
    switch (mUsart)
    {
      case USART0: nvic_irq_enable(USART0_IRQn, 1, 1);break;
      case USART1: nvic_irq_enable(USART1_IRQn, 1, 1);break;
    }

    usart_interrupt_enable(mUsart, USART_INT_TC);
    usart_interrupt_enable(mUsart, USART_INT_ERR);


    // ----------- DMA ---------
    
    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    
    dma_channel_enum lenDMA_TX = DMA_CH1;
    dma_channel_enum lenDMA_RX = DMA_CH2;
    
    if (mUsart == USART1) 
    {
      lenDMA_TX = DMA_CH3;
      lenDMA_RX = DMA_CH4;
    }    
    dma_deinit(lenDMA_TX);
    dma_deinit(lenDMA_RX);
    
      
    dma_parameter_struct dma_init_struct;

    /* configure the USART TX DMA channel */
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_addr  = USART_TDATA(mUsart);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;    
    dma_init(lenDMA_TX, dma_init_struct);
    
    /* configure the USART RX DMA channel */
    dma_deinit(DMA_CH4);
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.periph_addr  = USART_RDATA(mUsart);
    dma_init(lenDMA_RX, dma_init_struct);    
    
    switch (mUsart)
    {
      case USART0: nvic_irq_enable(DMA_Channel1_2_IRQn, 1, 1);break;
      case USART1: nvic_irq_enable(DMA_Channel3_4_IRQn, 1, 1);break;            
    }

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption, bool lbMaster)
  {
    if (lenOption == cComNode::enResetHwFull)
    {
      vInitHw(lbMaster);
    }
    else  // enResetHwDma
    {
      // Usart Peripheral Disnable
      usart_disable(mUsart);
      vStopDMA();

      if (lbMaster)
      {
        //SDA and SCL must be pulled high
        if (!bCheckPins()) return;
      }

      USART_INTC(mUsart) = 0xFFFFFFFF;

      // Usart Peripheral Enable
      usart_enable(mUsart);

      munDeviceStatus = tenComDeviceStatus::enDeviceReady;
      munBusStatus    = tenComBusStatus::enBusNoError;
      munComStatus    = tenComStatus::enStatusIdle;
      mSm             = cComNode::tenState::enStIdle;
    }

    if (isErrorActive(lbMaster))
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
      return;
    }

    if (USART_STAT(mUsart) & USART_STAT_BSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      return;
    }
  }

  bool  bCheckBus()
  {
    if (USART_STAT(mUsart) & USART_STAT_BSY)
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
    mPinTx.vSetMode(GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);

    //Tx must be pulled high
    if (mPinTx.ui8Get() == 0)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorPins;
      mSm             = cComNode::enStError;
      lbRet = False;
    }
    
    mPinTx.vSetMode(GPIO_MODE_AF, GPIO_PUPD_PULLUP);
    mPinTx.vSetAF(GPIO_AF_2);
        
    return True;
  }
  uint8 isErrorActive(bool lbMaster)
  {
    UNUSED(lbMaster);
    return 0;
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

  cUartMpHdSlave(u32 lu32Usart, u32 lu32Baudrate)
  : cUartMpHd(lu32Usart, lu32Baudrate)
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
      vInitHw(False);
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
      USART_INTC(mUsart) = 0xFFFFFFFF;
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
    USART_CMD(mUsart) |= USART_CMD_MMCMD;
  }


  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
    if (isErrorActive(True))
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
              // Alles einschalten
              usart_receive_config(mUsart,  USART_RECEIVE_ENABLE);
              usart_transmit_config(mUsart, USART_TRANSMIT_ENABLE);
              vStartDMA(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen, cComNode::enEvPrepareForRx);
              mSm = cComNode::tenState::enStWait;
            }
            else
            {
              // Tx
              if (mpcMsg->cTxData.muiLen > 0)
              {
                menComState = cComNode::tenState::enStData;
                // Rx ausschalten
                usart_receive_config(mUsart,  USART_RECEIVE_DISABLE);
                usart_transmit_config(mUsart, USART_TRANSMIT_ENABLE);
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
                      (!(USART_STAT(mUsart) & USART_STAT_RWU)))    // Nicht in Mute modus
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
                  // Rx ausschalten
                  usart_receive_config(mUsart,  USART_RECEIVE_DISABLE);
                  usart_transmit_config(mUsart, USART_TRANSMIT_ENABLE);                  
                  vStartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, cComNode::enEvPrepareForTx);
                  mSm = cComNode::tenState::enStEnd;
                }
                else
                {
                  // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
                  usart_receive_config(USART0,  USART_RECEIVE_DISABLE);
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
          if ((lenEvent == cComNode::tenEvent::enEvDmaRxTc) || (lenEvent == cComNode::tenEvent::enEvUsartTc))
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
