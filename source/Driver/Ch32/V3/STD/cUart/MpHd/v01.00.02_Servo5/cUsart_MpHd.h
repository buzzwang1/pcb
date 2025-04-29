#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"
#include "ch32v30x.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_usart.h"
#include "ch32v30x_dma.h"

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
    USART3->CTLR3 &= ~((1<<6) | (1<<7)); // ~(USART_CTL2_DENR | USART_CTL2_DENT);
    DMA1_Channel2->CFGR &= ~(1 << 0); // EN
    DMA1_Channel3->CFGR &= ~(1 << 0); // EN
  }

  // Überpruft ob sich der Rx DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    return DMA1_Channel3->CNTR;
  }

  inline void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mu16DmaCounter2ms_Old    = BufferSize;
    DMA1->INTFCR             = DMA1_IT_TC3;
    DMA1_Channel3->CNTR      = BufferSize;
    DMA1_Channel3->MADDR     = (uint32)pBuffer;
    DMA1_Channel3->CFGR     |=  ((1 << 0) | (1 << 1)); // EN, TCIE
    USART3->CTLR3           |= (1 << 6); // (USART_CTL2_DENR);
  }

  inline void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    USART3->STATR           &= ~(1 << 6);   // USART_STAT_TC;
    DMA1->INTFCR             = DMA1_IT_TC2;
    DMA1_Channel2->CNTR      = BufferSize;
    DMA1_Channel2->MADDR     = (uint32)pBuffer;
    // Ende der Übertragung wird durch  U0.TC interrupt erledigt
    // Tx DMA Interrupt wird nicht benötigt
    DMA1_Channel2->CFGR     |=  ((1 << 0) /* | DMA_CHXCTL_FTFIE */); // EN
    USART3->CTLR3           |=   (1 << 7); // (USART_CTL2_DENT);
  }


  void vInitHwUart()
  {
      // Usart Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // Usart Peripheral Disable
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);

    USART_InitTypeDef USART_InitStructure;

    // NAch Datenblatt hat das Teil kein Oversampling
    // USART_OverSampling8Cmd(USART3, ENABLE);

    USART_InitStructure.USART_BaudRate    = 200000;
    USART_InitStructure.USART_WordLength  = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits    = USART_StopBits_1;
    USART_InitStructure.USART_Parity      = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode        = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);
  
    USART_HalfDuplexCmd(USART3, ENABLE);

    USART_SetAddress(USART3, mui8Adr);
    USART_WakeUpConfig(USART3, USART_WakeUp_AddressMark);
    // Enter mute mode
    USART3->CTLR1 |= 2; // 2 = RWU Mute Mode


    //// //Das Gleiche wie oben, spart aber 600 Byte Code
    //// USART_CTL0(USART0) = 0x0000b848;
    //// USART_CTL1(USART0) = 0x01000000;
    //// usart_address_config(USART0, mui8Adr);
    //// USART_BAUD(USART0) = 0x00000028;   // 200000 Baud @8Mhz  // 0.5Mhz/ 2.5 = 200000,00 0x00000028
    ////                                    // 200000 Baud @16Mhz // 1.0Mhz/ 5.0 = 200000,00 0x00000050

    // Prio 0(höchste) 7(kleinste)
    NVIC_SetPriority(USART3_IRQn, (2 << 5) | (0x01 << 4));
    NVIC_EnableIRQ(USART3_IRQn);

    // ISR löschen bevor Interrupts aktiviert werden,
    // weil TC ist nach reset gesetzt und würde direkt einen Interrupt auslösen
    USART3->STATR = 0;

    //Ist oben schon drin
    USART_ITConfig(USART3, USART_IT_TC,  ENABLE);
    USART_ITConfig(USART3, USART_IT_ERR, ENABLE);

    // Usart Peripheral Enable
    USART_Cmd(USART3, ENABLE);
  }

  void  vInitHw()
  {
    // Setup Pins
  // PB10
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    // PB10 ist im High register
    GPIOB->CFGHR &= ~(0xF << ((10 & 7) * 4));
    // D = 1101 => Output AF OD 10Mhz
    // B = 1011 => Output AF PP 50Mhz
    GPIOB->CFGHR |= ((0xB) << ((10 & 7) * 4));


    /* enable the DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    vStopDMA();

    vInitHwUart();

    // ----------- DMA ---------
    DMA_InitTypeDef DMA_InitStructure;
    
    // configure the USART TX DMA channel
    // Channel2
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DATAR); /* USART2->DATAR:0x40004404 */
    DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)0;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    
    // configure the USART RX DMA channel
    // Channel3
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    //// //Das Gleiche wie oben, spart aber 200 Byte Code
    //// DMA_CH1CTL   = 0x00003190;
    //// DMA_CH1PADDR = 0x40013828;
    //// 
    //// DMA_CH2CTL   = 0x00003180;
    //// DMA_CH2PADDR = 0x40013824;
    
    // Prio 0(höchste) 7(kleinste)
    NVIC_SetPriority(DMA1_Channel3_IRQn, (2 << 5) | (0x01 << 4));
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
    USART3->CTLR1 &= ~(1 << 2); // USART_CTL0_REN
    
    //CH32V305 has no flush command, therefore dummy read
    //to ensure, that Rx register is empty
    volatile u8 u8Dummy = USART3->DATAR; UNUSED(u8Dummy);
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
    USART3->CTLR1 |= 2; // 2 = RWU Mute Mode
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART3->CTLR1 &= ~(1 << 2); // USART_CTL0_REN
    vStopDMA();
    mSm = cComNode::tenState::enStIdle;
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
              //CH32V305 has no flush command, therefore dummy read
              //to ensure, that Rx register is empty
              volatile u8 u8Dummy = USART3->DATAR; UNUSED(u8Dummy);
              USART3->CTLR1      |= (1 << 2); // USART_CTL0_REN
              vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);

              if (!(USART3->CTLR1 | 2))    // Nicht im Mute modus
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
                USART3->CTLR1 &= ~(1 << 2); // USART_CTL0_REN
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
                  USART3->CTLR1 &= ~(1 << 2); // USART_CTL0_REN
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
