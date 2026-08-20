#pragma once

#include "cSpi.h"
#include "cNRF905Bn_Tim.h"
#include "cNRF905Bn_Hw.h"
#include "cNRF905_Defs.h"


class cNRF905BnSlave : public cSpiTemplate<cComNodeSlave>, public cNRF905BnHw, public cNRF905_Defs
{
  public:

  // Cmds
  enum tenCmdState : u8
  {
    nStIdle = 0,

    nStWriteConfig,
    nStWriteConfig2,
    nStWriteConfig3,
    nStWriteConfig4,
    nStWriteConfig5,
    nStWriteConfig6,
    nStWriteConfig7,
    nStWriteConfig8,

    nStReceive,
    nStReceive2,
    nStReceive3,
    nStReceive4,
    nStReceive5,
    nStReceive6,
    nStReceive7,
    nStReceive8,

    nStTransmit,
    nStTransmit2,
    nStTransmit3,
    nStTransmit4,
    nStTransmit5,
    nStTransmit6,
    nStTransmit7,
    nStTransmit8,
    nStTransmit9
  };

  // Events
  enum tenCmdEvents : u8
  {
    nEvDummy = 0,
    nEvError,
    nEvDone,
    nEvLoop,
    nEvStart,
    nEvStartCmd,
    nEvDataReady,
    nEvSpiDmaRxReady,
    nEvSpiDmaTxReady,
    nEvTimer
  };



  struct __attribute__((__packed__)) cNrf905Fast
  { 
    u8 IdxCfg = 0;
    tunReg00_Config Cfg;


    // Byte 4
    u8 IdxTxPlWidth = 4;
    u8 TX_PayloadWidth;
    u8 TX_PayloadWidth_Old;
      
    // Byte 3
    u8 IdxRxPlWidth = 3;
    u8 RX_PayloadWidth;
    u8 RX_PayloadWidth_Old;

    u8 IdxTxAdr = 24;
    u32 TxAdress;

    u8 IdxRxBuf = 24;
    u8 au8RxBuf[32];

    u8 IdxTxBuf = 21;
    u8 au8TxBuf[32];
  };

  cNRF905BnTim mTimer;
  
  cNRF905_Defs::tenMode menMode;
  tenCmdState   mSm;
  cNrf905Fast   mData;

  cNRF905BnSlave(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx,
                 u16 lu32Prescaler = 256)
    : cSpiTemplate<cComNodeSlave>(lpstSpi, lu8DmaChTx, lu8DmaChRx, lu32Prescaler, 0),
      cNRF905BnHw(), cNRF905_Defs()
  {
    menMode = cNRF905_Defs::tenMode::nRADIO_MODE_INIT;
    mSm = tenCmdState::nStIdle;
  }


  // ---------------------------------------------

  bool bStartUpdateCfg()
  {
    if (mSm == tenCmdState::nStIdle)
    {
      mSm = tenCmdState::nStWriteConfig;
      bEventHandler(tenCmdEvents::nEvDummy);
      return True;
    }
    return False;
  }

  bool bEventHandler(tenCmdEvents lenEvent) // __attribute__((optimize("-O0")))
  {
    do
    {
      if (lenEvent == tenCmdEvents::nEvLoop)
      {
        lenEvent = tenCmdEvents::nEvDummy;
      }

      switch (mSm)
      {
        // ----------------------------------------------------------------------------------------
        case tenCmdState::nStWriteConfig:
          mSm = tenCmdState::nStWriteConfig2;
          {
            u16 u16Delay = u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_IDLE);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = tenCmdEvents::nEvLoop;
            }
          }
          break;

        case tenCmdState::nStWriteConfig2:
          // Config schreiben
          mSm = tenCmdState::nStWriteConfig3;
          mCS_SPI.vSet0();
          vStartDMATx((u8*)&mData.IdxTxAdr, 10 + 1);  // + 1 Wegen Indexbyte am Anfang
          break;

        case tenCmdState::nStWriteConfig3:
          mSm = tenCmdState::nStWriteConfig4;

          mCS_SPI.vSet1();
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        case tenCmdState::nStWriteConfig4:
          // TX Adresse schreiben
          mSm = tenCmdState::nStWriteConfig5;
          mCS_SPI.vSet0();
          vStartDMATx((u8*)&mData.IdxCfg, 4 + 1); // + 1 Wegen Indexbyte am Anfang
          break;

        case tenCmdState::nStWriteConfig5:
          mSm = tenCmdState::nStWriteConfig6;

          mCS_SPI.vSet1();

          // 5us warten bis einstellungen übernommen sind
          mTimer.vStart(5);
          break;

        case tenCmdState::nStWriteConfig6:
          mSm = tenCmdState::nStIdle;
          mTimer.vStop();
          break;

        // ----------------------------------------------------------------------------------------

        case tenCmdState::nStTransmit: // TX Payload Cnt Setzen Commando
          mSm = tenCmdState::nStTransmit2;

          if (mData.TX_PayloadWidth == 0)
          {
            mSm = tenCmdState::nStIdle;
          }
          else
          {
            u16 u16Delay = u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_TX);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
             lenEvent = tenCmdEvents::nEvLoop;
            }
          }
          break;

        case tenCmdState::nStTransmit2: // TX Payload Cnt Setzen Daten
          mSm = tenCmdState::nStTransmit3;

          if (mData.TX_PayloadWidth_Old != mData.TX_PayloadWidth)
          {
            mCS_SPI.vSet0();
            mData.TX_PayloadWidth_Old = mData.TX_PayloadWidth;
            vStartDMATx((u8*)&mData.IdxTxPlWidth, 1 + 1); // + 1 Wegen Indexbyte am Anfang
          }
          else
          {
            lenEvent = tenCmdEvents::nEvLoop;
          }
          break;

        case tenCmdState::nStTransmit3: // TX Payload Setzen Commando
          mSm = tenCmdState::nStTransmit4;

          mCS_SPI.vSet1();
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        case tenCmdState::nStTransmit4: // TX Payload Setzen Daten
          mSm = tenCmdState::nStTransmit5;
          mCS_SPI.vSet0();
          vStartDMATx((u8*)&mData.IdxTxBuf, mData.IdxTxPlWidth + 1); // + 1 Wegen Indexbyte am Anfang
          break;

        case tenCmdState::nStTransmit5: // in TX Modus aktivieren
          mSm = tenCmdState::nStTransmit6;

          mCS_SPI.vSet1();
          {
            u16 u16Delay = u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_TX_PULSE);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = tenCmdEvents::nEvLoop;
            }
          }
          break;

        case tenCmdState::nStTransmit6: // Senden und auf DR warten
          mSm = tenCmdState::nStTransmit7;
          {
            u16 u16Delay = u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_TX);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = tenCmdEvents::nEvLoop;
            }
          }
          break;

        case tenCmdState::nStTransmit7: // Senden und auf DR warten
          mSm = tenCmdState::nStTransmit8;

          mTimer.vStop();
          if (!isDr())
          {
            mTimer.vStart(10000);
            vEnableIrqDR();
          }
          else
          {
            lenEvent = tenCmdEvents::nEvLoop;
          }
          break;

        case tenCmdState::nStTransmit8: // in Idle Modus
          mSm = tenCmdState::nStIdle;

          mTimer.vStop();
          vDisableIrqDR();
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        case tenCmdState::nStTransmit9: // Fertig
          mSm = tenCmdState::nStIdle;
          break;

        // ----------------------------------------------------------------

        case tenCmdState::nStReceive:
          /*mSm = tenCmdState::nStReceive2;

          bStart_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_RX);
          if (mstNRF905.mstConfig.Rx_Payload_Width == 0)
          {
            mSm = tenCmdState::nStIdle;
          }
          else
          {
            mCS_SPI.vSet0();
            //Dummy Read to reset AM & DR
            vStartDMA(&mstNRF905.u8SpiCmdRxPl, 1, SPI_DIRECTION_TX); // 'Playload lesen' senden
          }*/

          mSm = tenCmdState::nStReceive4;
          //{
              /*u16 u16Delay =*/ u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_RX);
          //  if (u16Delay)
          //  {
          //    mTimer.vStart(u16Delay);
          //  }
          //  else
          //  {
              lenEvent = tenCmdEvents::nEvLoop;
          //  }
          //}
          break;

        case tenCmdState::nStReceive2:
          mSm = tenCmdState::nStReceive3;

          //Dummy Read to reset AM & DR
          vStartDMARx((u8*)&mData.IdxRxBuf, mData.RX_PayloadWidth_Old + 1);  // + 1 Wegen Indexbyte am Anfang
          break;

        case tenCmdState::nStReceive3:
          mSm = tenCmdState::nStReceive4;

          mCS_SPI.vSet1();
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        case tenCmdState::nStReceive4: // RX Payload Cnt Setzen Daten
          mSm = tenCmdState::nStReceive5;

          if (mData.RX_PayloadWidth_Old != mData.RX_PayloadWidth)
          {
            mCS_SPI.vSet0();
            mData.RX_PayloadWidth_Old = mData.RX_PayloadWidth;
            vStartDMATx((u8*)&mData.IdxRxPlWidth, 1 + 1);  // + 1 Wegen Indexbyte am Anfang
          }
          else
          {
            lenEvent = tenCmdEvents::nEvLoop;
          }
          break;

        case tenCmdState::nStReceive5: // Empfänger einschalten und auf DR warten
          mSm = tenCmdState::nStReceive6;

          mCS_SPI.vSet1();

          if (isDr())
          {
            lenEvent = tenCmdEvents::nEvLoop;
          }
          else
          {
            vEnableIrqDR();
            if (isDr())
            {
              vDisableIrqDR();
              lenEvent = tenCmdEvents::nEvLoop;
            }
          }
          break;

        case tenCmdState::nStReceive6:
          mSm = tenCmdState::nStIdle;

          if ((lenEvent == tenCmdEvents::nEvDataReady) || // Externer Interrupt
              (isDr()))
          {
            mSm = tenCmdState::nStReceive7;
            mCS_SPI.vSet0();
            vDisableIrqDR();
            vStartDMARx((u8*)&mData.IdxRxBuf, mData.RX_PayloadWidth + 1); // 'Playload lesen' senden
          }
          break;

        case tenCmdState::nStReceive7: // in Idle Modus
          mSm = tenCmdState::nStReceive8;
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        case tenCmdState::nStReceive8: // Fertig
          mSm = tenCmdState::nStIdle;

          mCS_SPI.vSet1();
          lenEvent = tenCmdEvents::nEvLoop;
          break;

        // ----------------------------------------------------------------
        default:
          break;
      }
    } while (lenEvent == tenCmdEvents::nEvLoop);

    return (mSm == tenCmdState::nStIdle);
  }

  void vStartTransmit()
  {
    mSm = tenCmdState::nStTransmit;
    bEventHandler(tenCmdEvents::nEvDummy);
  }

  void vStartReceive()
  {
    mSm = tenCmdState::nStReceive;
    bEventHandler(tenCmdEvents::nEvDummy);
  }



  // ----------------------------------------------


  u16 u16Start_Set_Mode(tenMode lenNewMode)
  {
    u16 lu16Ret_us = 0;

    if (menMode != lenNewMode)
    {
      switch (lenNewMode)
      {
        case tenMode::nRADIO_MODE_PWR_DOWN:
          cNRF905BnHw::vPwrOff();
          cNRF905BnHw::vEnOff();
          cNRF905BnHw::vCEOff();
          break;
        case tenMode::nRADIO_MODE_IDLE:
          cNRF905BnHw::vPwrOn();
          cNRF905BnHw::vCEOff();
          cNRF905BnHw::vEnOff();
          if (menMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000;
          break;
        case tenMode::nRADIO_MODE_RX:
          cNRF905BnHw::vPwrOn();
          cNRF905BnHw::vEnOff();
          cNRF905BnHw::vCEOn();
          if (menMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000 + 650;
          if (menMode == tenMode::nRADIO_MODE_IDLE)     lu16Ret_us = 650;
          if (menMode == tenMode::nRADIO_MODE_TX)       lu16Ret_us = 550;
          break;
        case tenMode::nRADIO_MODE_TX:
          cNRF905BnHw::vPwrOn();
          cNRF905BnHw::vCEOff();
          cNRF905BnHw::vEnOn();
          if (menMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000 + 650;
          if (menMode == tenMode::nRADIO_MODE_IDLE)     lu16Ret_us = 650;
          if (menMode == tenMode::nRADIO_MODE_RX)       lu16Ret_us = 550;
          break;
        case tenMode::nRADIO_MODE_TX_PULSE:
          cNRF905BnHw::vCEOn();
          lu16Ret_us = 5;
          break;
        default:
          break;
      }
      menMode = lenNewMode;
    }
    return lu16Ret_us;
  }



  void vStopCom()
  {
    mTimer.vStop();
    cComNodeSlave::mSm = cComNode::tenState::enStIdle;
    ////cNRF905::mSm = tenCmdState::nStIdle;
    mError = cComNode::enNoError;
  }

  void vPowerDown()
  {
    u16Start_Set_Mode(cNRF905_Defs::tenMode::nRADIO_MODE_PWR_DOWN);
  }


 void SetChannel_433(u16 lu16Channel, bool bIKnowWhatIDo)
  {
    // Sets center freq. together with HFREQ_PLL (default = 001101100b = 108d).
    //  - fRF = ( 422.4 + CH_NOd /10)*(1+HFREQ_PLLd) MHz

    //  Operating frequency HFREQ_PLL      CH_NO
    //  430.0 MHz              [0]      [001001100]
    //  433.1 MHz              [0]      [001101011]
    //  433.2 MHz              [0]      [001101100]
    //  434.7 MHz              [0]      [001111011]
    //
    //  862.0 MHz              [1]      [001010110]
    //  868.2 MHz              [1]      [001110101]
    //  868.4 MHz              [1]      [001110110]
    //  869.8 MHz              [1]      [001111101]
    //
    //  902.2 MHz              [1]      [100011111]
    //  902.4 MHz              [1]      [100100000]
    //  927.8 MHz              [1]      [110011111]

    // 433,05 MHz  -- 434,79 MHz
    //
    //   0:  422,4
    //  86:  431,0
    //  96:  432,0
    // 106:  433,0
    // 107:  433,1
    // 108:  433,2 - erlaubt
    // 109:  433,3 - erlaubt
    // 110:  433,4 - erlaubt
    // 111:  433,5 - erlaubt
    // 112:  433,6 - erlaubt
    // 113:  433,7 - erlaubt
    // 114:  433,8 - erlaubt
    // 115:  433,9 - erlaubt
    // 116:  434,0 - erlaubt
    // 117:  434,1 - erlaubt
    // 118:  434,2 - erlaubt
    // 119:  434,3 - erlaubt
    // 120:  434,4 - erlaubt
    // 121:  434,5 - erlaubt
    // 122:  434,6 - erlaubt
    // 123:  434,7 - erlaubt
    // 124:  434,8
    // 125:  434,9
    // 126:  435,0
    // 511:  473,5

    if (!bIKnowWhatIDo)
    {
      if (lu16Channel < 108) lu16Channel = 108;
      if (lu16Channel > 123) lu16Channel = 123;
    }

    mData.Cfg.stBits.ChannelNum_Low  = lu16Channel & 0xFF;
    mData.Cfg.stBits.ChannelNum_High = lu16Channel >> 8;

    // Sets PLL in 433 or 868/915 MHz mode (default = 0).
    //  '0' – Chip operating in 433MHz band
    //  '1' – Chip operating in 868 or 915 MHz band
    mData.Cfg.stBits.HFREQ_PLL         = tenFrequencyBand::nBAND_433MHz;

    // Output power (default = 00).
    //  '11'  +10dBm 11.0mA
    //  '00'  -10dBm 14.0mA
    //  '01'  -2dBm  20.0mA
    //  '10'  +6dBm  30.0mA
    mData.Cfg.stBits.PA_PWR = tenOutputPower::nPWR_Plus10dBm;

    // Reduces current in RX mode by 1.6mA. Sensitivity is reduced (default = 0).
    //  '0' – Normal operation
    //  '1' – Reduced power
    mData.Cfg.stBits.RX_RED_PWR = tenLowPowerRx::nLOW_RX_DISABLE;

    mData.Cfg.stBits.AutoRetransmit = tenAutoRetran::nAUTO_RETRAN_DISABLE;
  }

  void SetCrc(u8 lu8BitWidth) // 0, 8, 16
  {
    switch (lu8BitWidth)
    {
      case 0:
        mData.Cfg.stBits.CRC_En   = tenCrc::nCRC_DISABLE; //NRF905_CRC_ENABLE;
        mData.Cfg.stBits.CRC_Mode = tenCrc::nCRC_16;
        break;
      case 8:
        mData.Cfg.stBits.CRC_En   = tenCrc::nCRC_ENABLE;
        mData.Cfg.stBits.CRC_Mode = tenCrc::nCRC_8;
        break;
      case 16:
        mData.Cfg.stBits.CRC_En   = tenCrc::nCRC_ENABLE;
        mData.Cfg.stBits.CRC_Mode = tenCrc::nCRC_16;
        break;
    }
  }

  // Forward / Wrapper
  void vSetTxAdress(u32 lu32Adress) { mData.TxAdress = lu32Adress; }
  void vSetRxAdress(u32 lu32Adress) { mData.Cfg.stBits.unRxAdress.u32Adr = lu32Adress; }

  void vSetTxPayloadWidth(u8 lu8PayloadWidth) { mData.TX_PayloadWidth = mData.Cfg.stBits.TX_PayloadWidth = lu8PayloadWidth; }
  void vSetRxPayloadWidth(u8 lu8PayloadWidth) { mData.RX_PayloadWidth = mData.Cfg.stBits.RX_PayloadWidth = lu8PayloadWidth; }



};
