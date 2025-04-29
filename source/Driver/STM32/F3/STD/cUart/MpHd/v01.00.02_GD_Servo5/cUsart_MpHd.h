#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"
#include "gd32f3x0.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_usart.h"
#include "gd32f3x0_dma.h"

#include "cUartMpHdTimer.h"

#include "tGPPin.h"
#include "ComDat.h"

#include "ClockInfo.h"


class cUartMpHd
{
  public:

  cComNode::tenState   mSm;

  u16                  mu16DmaCounter2ms_Old;

  //Single Wire, only Tx Pin
  //cGpPin               mPinTx;

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
    DMA_INTC            = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH2);
    DMA_CH2CNT          = BufferSize;
    DMA_CH2MADDR        = (uint32)pBuffer;
    DMA_CH2CTL         |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
    USART_CTL2(USART0) |= USART_DENR_ENABLE;
  }

  inline void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    USART_INTC(USART0)  = USART_STAT_TC;
    DMA_INTC            = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH1);
    DMA_CH1CNT          = BufferSize;
    DMA_CH1MADDR        = (uint32)pBuffer;
    // Ende der Übertragung wird durch  U0.TC interrupt erledigt
    // Tx DMA Interrupt wird nicht benötigt
    DMA_CH1CTL         |=  (DMA_CHXCTL_CHEN /* | DMA_CHXCTL_FTFIE */);
    USART_CTL2(USART0) |= USART_DENT_ENABLE;
  }



  void vInitHwUart()
  {
      // Usart Periph clock enable
    rcu_periph_clock_enable(RCU_USART0);

    // Usart Peripheral Disable
    // usart_deinit(USART0);
    rcu_periph_reset_enable(RCU_USART0RST);
    rcu_periph_reset_disable(RCU_USART0RST);

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
    //USART_BAUD(USART0) = 0x00000340; //  19200 Baud @8Mhz  // 1.0Mhz/52.0 =  19230,77
    USART_BAUD(USART0) = 0x00000050;   // 200000 Baud @8Mhz  // 1.0Mhz/ 5.0 = 200000,00 0x00000050
                                       // 200000 Baud @12Mhz // 1.5Mhz/ 7.5 = 200000,00 0x00000074
                                       // 200000 Baud @16Mhz // 2.0Mhz/10.0 = 200000,00 0x000000A0

    // Usart Peripheral Enable
    usart_enable(USART0);
    // Enter mute mode
    USART_CMD(USART0) |= USART_CMD_MMCMD;

    nvic_irq_enable(USART0_IRQn, 1, 1);

    //Ist oben schon drin
    //usart_interrupt_enable(USART0, USART_INT_TC);
    //usart_interrupt_enable(USART0, USART_INT_ERR);
  }

  void  vInitHw()
  {
    // Setup Pins
    //PA09
    rcu_periph_clock_enable(RCU_GPIOA);


    gpio_bit_set(GPIOA, GPIO_PIN_9);
    GPIO_OMODE(GPIOA) |= 1 << (9);      // Open drain
    GPIO_OSPD0(GPIOA) |= 1 << (9 * 2);  // 10MHz
    GPIO_PUD(GPIOA)   |= 1 << (9 * 2);  // Pull Up
    GPIO_CTL(GPIOA)   |= 2 << (9 * 2);  // Af Mode
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

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

    mSm  = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption, bool lbMaster = False)
  {
    UNUSED(lenOption);
    UNUSED(lbMaster);
    // Immer cComNode::enResetHwFull, Spart ein paar Byte
    vInitHw();
  }

  void vTick10ms()
  {
  }
};


// Dummy Class
class cUartMpHdMaster : public cUartMpHd
{
  public:
    cUsartMpHd_Slave_Timer mTimer;


  cUartMpHdMaster()
    : cUartMpHd()
  {
  }

  ~cUartMpHdMaster()
  {
  }

  void vReInit() {}


  inline void SetLock()   {}
  inline void ResetLock() {}

  void TIM_EV_IRQHandler()
  {
  }

  void ComIrqHandler(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEventType);
    UNUSED(lenEvent);
  }

  void vSetReInitTicks(u16 lu16ReInitTicks)
  {
    UNUSED(lu16ReInitTicks);
  }

  void vTick1ms()
  {
  }


  void vAddSlave(cComNode *lpcSlave)
  {
    UNUSED(lpcSlave);
  }

  void vRemoveSlave(cComNode *lpcSlave)
  {
    UNUSED(lpcSlave);
  }

  void vReInitAllSlave()
  {
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
    UNUSED(lenEvent);
  }

  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    UNUSED(lpcActiveMsg);
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {

    return True;
  }
};



class cUartMpHdSlave : public cUartMpHd
{
  public:
  cComDatMsg          *mpcMsg;

  cComNode            *mpcComNode;

  cUsartMpHd_Slave_Timer mTimer;

  u8                   mu8SelfTimer;

  cUartMpHdSlave()
  : cUartMpHd()
  {
    mSm         = cComNode::tenState::enStIdle;
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
    mui8Adr    = mpcComNode->mAdr;
    if (mpcComNode != null)
    {
      vInitHw();
    }
  }


  void vError(cComNode::tenError lenError)
  {
    mSm = cComNode::tenState::enStIdle;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART_CTL0(USART0) &= ~USART_CTL0_REN;
    USART_CMD(USART0) |= USART_CMD_RXFCMD;
    vStopDMA();
    cUartMpHd::vInitHwUart();
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
    USART_CMD(USART0) |= USART_CMD_MMCMD;
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART_CTL0(USART0) &= ~USART_CTL0_REN;
    vStopDMA();
    mSm = cComNode::tenState::enStIdle;
    mpcMsg->vDone();
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
            // Beim slave Rx vor Tx.
            // Rx
            if (mpcMsg->cRxData.muiLen > 0)
            {
              USART_CMD(USART0)  |= USART_CMD_RXFCMD;
              USART_CTL0(USART0) |= USART_CTL0_REN;
              vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);

              if (!(USART_STAT(USART0) & USART_STAT_RWU))    // Nicht im Mute modus
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
                USART_CTL0(USART0) &= ~USART_CTL0_REN;
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
                  USART_CTL0(USART0) &= ~USART_CTL0_REN;
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
