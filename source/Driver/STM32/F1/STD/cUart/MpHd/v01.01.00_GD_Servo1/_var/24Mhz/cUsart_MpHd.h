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
  cUsartMpHd_HwTimer     mTimer;
  u16                    mu16DmaCounter2ms_Old;
  u8                     mu8SelfTimer;
  //Single Wire, only Tx Pin
  //cGpPin               mPinTx;

  cUartMpHd()
  {
    mu8SelfTimer = 0;
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
  u16 u16GetDmaCounterRx()
  {
    return DMA_CH2CNT;
  }

  u16 u16GetDmaCounterTx()
  {
    return DMA_CH1CNT;
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

  void vInitHwUart(u8 lui8Adr)
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
    usart_address_config(USART0, lui8Adr);
    USART_CTL2(USART0) = 0x00000009;
    //USART_BAUD(USART0) = 0x00000340; //  19200 Baud @8Mhz  // 1.0Mhz/52.0 =  19230,77
    USART_BAUD(USART0) = 0x000000F0;   // 200000 Baud @8Mhz  // 1.0Mhz/ 5.0 = 200000,00 0x00000050
                                       // 200000 Baud @12Mhz // 1.5Mhz/ 7.5 = 200000,00 0x00000078
                                       // 200000 Baud @16Mhz // 2.0Mhz/10.0 = 200000,00 0x000000A0
                                       // 200000 Baud @24Mhz // 3.0Mhz/15.0 = 200000,00 0x000000F0

    // Usart Peripheral Enable
    usart_enable(USART0);

    USART_INTC(USART0) = 0xFFFF;
    nvic_irq_enable(USART0_IRQn, 1, 1);

    //Ist oben schon drin
    //usart_interrupt_enable(USART0, USART_INT_TC);
    //usart_interrupt_enable(USART0, USART_INT_ERR);
  }

  void  vInitHw(u8 lui8Adr)
  {
    // Setup Pins
    //PA09
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_bit_set(GPIOA, GPIO_PIN_9);
    GPIO_OMODE(GPIOA) |= 1 << (9);      // Open drain
    GPIO_OSPD(GPIOA)  |= 1 << (9 * 2);  // 10MHz
    GPIO_PUD(GPIOA)   |= 1 << (9 * 2);  // Pull Up
    GPIO_CTL(GPIOA)   |= 2 << (9 * 2);  // Af Mode
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    vStopDMA();

    vInitHwUart(lui8Adr);

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
  }

  void vEnableIrq()
  {
    nvic_irq_enable(USART0_IRQn, 1, 1);
    nvic_irq_enable(DMA_Channel1_2_IRQn, 1, 1);
  }

  void vDisableIrq()
  {
    nvic_irq_disable(USART0_IRQn);
    nvic_irq_disable(DMA_Channel1_2_IRQn);
  }


  bool bCheckBusy()
  {
    return (True);
  }

  bool bCheckPins()
  {
    return True;
  }
};


// Dummy Class
class cUartMpHdMaster : public cUartMpHd, public cComNodeMaster
{
  public:

  cUartMpHdMaster(u16 luInitDelay_ms)
    : cComNodeMaster(luInitDelay_ms)
  {
  }

  cComNode::tenError enInitHw() override
  {
    return cComNode::enNoError;
  }

  bool bCheckBusy() override
  {
    return cUartMpHd::bCheckBusy();
  }


  inline void SetLock(u8 lu8Val = 1)   {UNUSED(lu8Val);}
  inline void ResetLock() {}

  void vResetCom()
  {
  }

  void vSupressErrorHandling()
  {
  }

  // Start wird sowohl vom Interrupt wie auch von der Main benutzt
  // Um keine Laufzeit Konflikte zu bekommen, wird solange der Interrupt gesperrt
  void vPreStart() override
  {
  }

  void vPostStart() override
  {
  }


  void vComError() override
  {
  }


  void TIM_EV_IRQHandler()
  {
  }

  void ComIrqHandler(cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEvent);
  }

  void vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEvent);
  }
};



class cUartMpHdSlave : public cUartMpHd, public cComNodeSlave
{
  public:
  cUartMpHdSlave(u16 luInitDelay_ms = 1)
  : cUartMpHd(),
    cComNodeSlave(luInitDelay_ms)
  {
    mSm = cComNode::tenState::enStIdle;
    mu32Baudrate = 200000;
  }

  cComNode::tenError enInitHw() override
  {
    cUartMpHd::vInitHw(muAdr);
    return cComNode::tenError::enNoError;
  }

  void vHwStop() override
  {
    vDisableIrq();
    USART_INTC(USART0) = 0xFFFF;
    usart_disable(USART0);
  }

  void vHwContinue() override
  {
    usart_enable(USART0);
    USART_INTC(USART0) = 0xFFFF;
    vEnableIrq();
  }

  void vResetCom()
  {
    if (mSm != cComNode::tenState::enStIdle)
    {
      // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
      USART_CTL0(USART0) &= ~USART_CTL0_REN; // Rx ausschalten
      USART_CMD(USART0)  |= USART_CMD_RXFCMD;
      vStopDMA();

      mTimer.vStop();
      USART_INTC(USART0) = 0xFFFF;

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
    if (USART_STAT(USART0) & 0xF)
    {
      USART_INTC(USART0) = USART_STAT_ORERR;
      mError = cComNode::enErOverrun;
      vComError();
    }
    else
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvDmaRxTc:
          DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
          DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
          vSm(lenEvent);
          break;
        case cComNode::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          DMA_CHCTL(DMA_CH1) &= ~DMA_CHXCTL_CHEN;
          USART_INTC(USART0) = USART_STAT_TC;
          vSm(lenEvent);
          break;
        default:
          break;
      }
    }
  }

  inline void vMute()
  {
    USART_CMD(USART0) |= USART_CMD_MMCMD;
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART_CTL0(USART0) &= ~USART_CTL0_REN;  // Rx ausschalten
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
        if (!(USART_STAT(USART0) & USART_STAT_RWU))    // Nicht im Mute modus
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
            case cComNode::tenEvent::enEvStartSkipAdr:
              // Rx
              if (mpcActiveMsg->isRx())
              {
                USART_CMD(USART0)  |= USART_CMD_RXFCMD;
                USART_CTL0(USART0) |= USART_CTL0_REN; // Alles einschalten
                USART_INTC(USART0) = USART_INTC_AMC;
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
                  USART_CTL0(USART0) &= ~USART_CTL0_REN; // Rx ausschalten
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
                USART_CMD(USART0)  |= USART_CMD_RXFCMD;
                USART_CTL0(USART0) |= USART_CTL0_REN; // Alles einschalten
                USART_INTC(USART0) = USART_INTC_AMC;
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
        if (USART_STAT(USART0) & USART_STAT_AMF)
        {
          USART_INTC(USART0) = USART_INTC_AMC;
        }

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
