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


class cUartMpHd
{
  public:

  cComNode::tenState   mSm;

  u16                  mu16TickCounter;
  u16                  mu16DmaCounter2ms_Old;

  //Single Wire, only Tx Pin
  cGpPin               mPinTx;

  uint8 mui8Adr;

  cUartMpHd()
  {
  }

  void  vStopDMA()
  {
    //usart_dma_receive_config(USART0,  USART_DENT_DISABLE);
    //usart_dma_transmit_config(USART0, USART_DENR_DISABLE);
    USART_CTL2(USART0) &= ~(USART_CTL2_DENR | USART_CTL2_DENT);
    dma_channel_disable(DMA_CH1);
    dma_channel_disable(DMA_CH2);
  }

  // Überpruft ob sich der Rx DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    return DMA_CH2CNT;
  }

  inline void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mu16DmaCounter2ms_Old = BufferSize;
    DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH2);
    DMA_CH2CNT    = BufferSize;
    DMA_CH2MADDR  = (uint32)pBuffer;
    DMA_CH2CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
    USART_CTL2(USART0) |= USART_DENR_ENABLE;
  }

  inline void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    USART_INTC(USART0) = USART_STAT_TC;
    DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH1);
    DMA_CH1CNT    = BufferSize;
    DMA_CH1MADDR  = (uint32)pBuffer;
    DMA_CH1CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
    USART_CTL2(USART0) |= USART_DENT_ENABLE;
  }

  /*
  void  vStartDMA(uint8* pBuffer, uint32 BufferSize, cComNode::tenEvent lenDirection)
  {
    // Initialize the DMA with the new parameters
    if (lenDirection == cComNode::tenEvent::enEvPrepareForTx)
    {
      DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH1);
      DMA_CH1CNT    = BufferSize;
      DMA_CH1MADDR  = (uint32)pBuffer;
      DMA_CH1CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      USART_CTL2(USART0) |= USART_DENT_ENABLE;
    }
    else // Reception
    {
      mu16DmaCounter2ms_Old = BufferSize;
      DMA_INTC      = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH2);
      DMA_CH2CNT    = BufferSize;
      DMA_CH2MADDR  = (uint32)pBuffer;
      DMA_CH2CTL   |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
      USART_CTL2(USART0) |= USART_DENR_ENABLE;
    }
  }*/

  void vInitHwUart()
  {
      // Usart Periph clock enable
    rcu_periph_clock_enable(RCU_USART0);

    // Usart Peripheral Disable
    usart_deinit(USART0);

    /*
    usart_oversample_config(USART0, USART_OVSMOD_8);
    usart_baudrate_set(USART0,    19200);
    usart_word_length_set(USART0, USART_WL_9BIT);

    usart_halfduplex_enable(USART0);
    usart_address_config(USART0, mui8Adr);
    usart_address_detection_mode_config(USART0, USART_ADDM_4BIT);

    usart_mute_mode_enable(USART0);
    usart_mute_mode_wakeup_config(USART0, USART_WM_ADDR);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    */

    //Das Gleiche wie oben, spart aber 600 Byte Code
    USART_CTL0(USART0) = 0x0000b848;
    USART_CTL1(USART0) = 0x01000000;
    usart_address_config(USART0, mui8Adr);
    USART_CTL2(USART0) = 0x00000009;
    //USART_BAUD(USART0) = 0x00000340; //  19200 Baud @8Mhz // 1Mhz/52 =  19230,77
    USART_BAUD(USART0) = 0x00000050; // 200000 Baud @8Mhz // 1Mhz/ 5 = 200000,00

    // Usart Peripheral Enable
    usart_enable(USART0);
    USART_CMD(USART0) |= USART_CMD_MMCMD;

    nvic_irq_enable(USART0_IRQn, 1, 1);

    //Ist oben schon drin
    //usart_interrupt_enable(USART0, USART_INT_TC);
    //usart_interrupt_enable(USART0, USART_INT_ERR);
  }

  void  vInitHw()
  {
    // Setup Pins
    mPinTx.vInit(GPIOA, 9, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, 1);
    mPinTx.vSetAF(GPIO_AF_1);


    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    vStopDMA();

    vInitHwUart();

    // ----------- DMA ---------
    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* dma_parameter_struct dma_init_struct;

    // configure the USART TX DMA channel
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_addr  = USART0+0x28;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, dma_init_struct);

    // configure the USART RX DMA channel
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.periph_addr  = USART0+0x24;
    dma_init(DMA_CH2, dma_init_struct);*/

    //Das Gleiche wie oben, spart aber 200 Byte Code
    DMA_CH1CTL   = 0x00003190;
    DMA_CH1PADDR = 0x40013828;

    DMA_CH2CTL   = 0x00003180;
    DMA_CH2PADDR = 0x40013824;

    nvic_irq_enable(DMA_Channel1_2_IRQn, 1, 1);

    mSm             = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption)
  {
    UNUSED(lenOption);
    // Immer cComNode::enResetHwFull, Spart ein paar Byte
    vInitHw();
  }
};



class cUartMpHdSlave : public cUartMpHd
{
  public:
  cComDatMsg     *mpcMsg;
  cComNode       *mpcComNode;

  cUsartMpHd_Slave_Timer mTimer;

  cUartMpHdSlave()
  : cUartMpHd()
  {
    mSm         = cComNode::tenState::enStIdle;
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
    mui8Adr    = mpcComNode->mMyAdr;
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
      if (lenEvent == cComNode::tenEvent::enEvUsartErOre)
      {
        // receiver abschalten, um weitere Overrun-Errors zuvermeiden.
        __asm("nop");
        //USART_CTL0(USART0) &= ~USART_CTL0_REN;
        //USART_INTC(USART0) = USART_STAT_ORERR;
        vInitHwUart();
        vStopDMA();
        mSm = cComNode::tenState::enStIdle;
        mpcComNode->vComError(cComNode::enErOverrun, cComNode::tenState::enStError);
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

  void vError(cComNode::tenError lenError)
  {
    vInitHw();
    mpcComNode->vComError(lenError, cComNode::tenState::enStError);
  }

  void vSetMsg(cComDatMsg* lpcMsg)
  {
    mpcMsg = lpcMsg;
  };

  inline void vMute()
  {
    USART_CMD(USART0) |= USART_CMD_MMCMD;
  }

  inline void vDone()
  {
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART_CTL0(USART0) &= ~USART_CTL0_REN;
    vStopDMA();
    mSm = cComNode::tenState::enStIdle;
    mpcComNode->vComDone();
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  //__attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvStart:
            if (mpcMsg->cData.muiLen > 0)
            {
              // Beim slave Rx vor Tx.
              // Rx
              if (mpcMsg->enDirection == cComDatMsg::enRx)
              {
                mu16TickCounter = 0;
                USART_CTL0(USART0) &= ~USART_CTL0_REN;
                vStartDMARx(mpcMsg->cData.mpu8Data, mpcMsg->cData.muiLen);
                USART_CTL0(USART0) |= USART_CTL0_REN;
                mSm = cComNode::tenState::enStWait;
              }
              else
              {
                // Tx
                USART_CTL0(USART0) &= ~USART_CTL0_REN;
                vStartDMATx(mpcMsg->cData.mpu8Data, mpcMsg->cData.muiLen);
                mSm = cComNode::tenState::enStEnd;
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
                      (!(USART_STAT(USART0) & USART_STAT_RWU)))    // Nicht im Mute modus
                  {
                    vError(cComNode::enErTimeout);
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
                vDone();
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
      mpcMsg = lpcActiveMsg;
      vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

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
