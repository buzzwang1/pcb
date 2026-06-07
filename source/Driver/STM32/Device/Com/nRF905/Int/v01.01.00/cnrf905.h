#ifndef _cNRF905_H
#define _cNRF905_H


#include "TYPEDEF.h"
#include "ComDat.h"
#include "tGPPin.h"
#include "nRF905_Spi.h"

//
// Preample: 200us
// Address : 4 Byte = > 640us
// x Bytes : x Byte = > x * 160us
//
// 10 Byte = > 2440us
// 32 Byte = > 5960us
//
// FlKa_20251201: DR Empfänger kommt 50us bis max. 250us verzögert zu DR Sender
//                   -  55us bei 32Byte
//                   -  95us bei 25Byte
//                   - 236us bei  2Byte


  // Workout channel from frequency & band
#define NRF905_CALC_CHANNEL(f, b)	((((f) / (1 + b)) - 422400000UL) / 100000UL)

class cNRF905
{
  public:

  // Radio Mode
  typedef enum
  {
    NRF905_RADIO_MODE_INIT,
    NRF905_RADIO_MODE_PWR_DOWN,   /* Power down */
    NRF905_RADIO_MODE_IDLE,       /* standby */
    NRF905_RADIO_MODE_TX,
    NRF905_RADIO_MODE_RX,
    NRF905_RADIO_MODE_TX_PULSE
  }tenRF905_Mode;

  // Frequency bands.
  typedef enum
  {
  // When using NRF905_BAND_868 and NRF905_BAND_915 for calculating channel (NRF905_CALC_CHANNEL(f, b)) they should be value 0x01,
  // but when using them for setting registers their value should be 0x02.
  // They're defined as 0x02 here so when used for calculating channel they're right shifted by 1
    NRF905_BAND_433 = 0x00, /* 433MHz band */
    NRF905_BAND_868 = 0x01, /* 868MHz band */
    NRF905_BAND_915 = 0x01  /* 915MHz band */
  }tenRF905_band;

  // Output power (n means negative, n10 = -10).
  typedef enum
  {
    NRF905_PWR_n10 = 0x00,  /* -10dBm = 100uW */
    NRF905_PWR_n2  = 0x01,  /* -2dBm = 631uW */
    NRF905_PWR_6   = 0x02,  /* 6dBm = 4mW */
    NRF905_PWR_10  = 0x03   /* 10dBm = 10mW */
  }tenRF905_pwr;

  // Output power (n means negative, n10 = -10).
  typedef enum
  {
    NRF905_CHANNEL_433_2 = 108,
    NRF905_CHANNEL_433_3 = 109,
    NRF905_CHANNEL_433_4 = 110,
    NRF905_CHANNEL_433_5 = 111,
    NRF905_CHANNEL_433_6 = 112,
    NRF905_CHANNEL_433_7 = 113,
    NRF905_CHANNEL_433_8 = 114,
    NRF905_CHANNEL_433_9 = 115,
    NRF905_CHANNEL_434_0 = 116,
    NRF905_CHANNEL_434_1 = 117,
    NRF905_CHANNEL_434_2 = 118,
    NRF905_CHANNEL_434_3 = 119,
    NRF905_CHANNEL_434_4 = 120,
    NRF905_CHANNEL_434_5 = 121,
    NRF905_CHANNEL_434_6 = 122,
    NRF905_CHANNEL_434_7 = 123
  }tenRF905_channel;

  // Save a few mA by reducing receive sensitivity.
  typedef enum
  {
    NRF905_LOW_RX_DISABLE = 0x0,  /* Disable low power receive */
    NRF905_LOW_RX_ENABLE  = 0x1   /* Enable low power receive */
  }tenRF905_low_rx;

  // Constantly retransmit payload while in transmit mode.
  // Can be useful in areas with lots of interference, but you'll need to make sure you can differentiate between re-transmitted packets and new packets (like an ID number).
  // Other transmissions will be blocked if collision avoidance is enabled.
  typedef enum
  {
    NRF905_AUTO_RETRAN_DISABLE = 0x0, /* Disable auto re-transmit */
    NRF905_AUTO_RETRAN_ENABLE  = 0x1  /* Enable auto re-transmit */
  }tenRF905_auto_retran;

  // Output a clock signal on pin 3 of IC.
  typedef enum
  {
    NRF905_OUTCLK_DISABLE = 0x00,  /* Disable output clock */
    NRF905_OUTCLK_ENABLE  = 0x01,  /* Enable output clock */
    NRF905_OUTCLK_4MHZ    = 0x00,  /* 4MHz clock */
    NRF905_OUTCLK_2MHZ    = 0x01,  /* 2MHz clock */
    NRF905_OUTCLK_1MHZ    = 0x02,  /* 1MHz clock */
    NRF905_OUTCLK_500KHZ  = 0x03,  /* 500KHz clock */
  } nRF905_outclk;

  // CRC Checksum.
  typedef enum
  {
    NRF905_CRC_DISABLE = 0x00, // Disable CRC
    NRF905_CRC_ENABLE  = 0x01, // Enable CRC
    NRF905_CRC_8       = 0x00, // 8bit CRC
    NRF905_CRC_16      = 0x01, // 16bit CRC
  } nRF905_crc_t;

  // Address size.
  typedef enum
  {
    NRF905_ADDR_SIZE_1 = 0x01,  /* 1 byte */
    NRF905_ADDR_SIZE_4 = 0x04,  /* 4 bytes */
  }tenRF905_addr_size;

  // Instructions
  typedef enum
  {
    NRF905_CMD_NOP            = 0xFF,
    NRF905_CMD_W_CONFIG       = 0x00,
    NRF905_CMD_W_CONFIG_RX_PW = 0x03,
    NRF905_CMD_W_CONFIG_TX_PW = 0x04,
    NRF905_CMD_R_CONFIG       = 0x10,
    NRF905_CMD_W_TX_PAYLOAD   = 0x20,
    NRF905_CMD_R_TX_PAYLOAD   = 0x21,
    NRF905_CMD_W_TX_ADDRESS   = 0x22,
    NRF905_CMD_R_TX_ADDRESS   = 0x23,
    NRF905_CMD_R_RX_PAYLOAD   = 0x24,
    NRF905_CMD_CHAN_CONFIG    = 0x80
  }tenRF905_Cmds;

  // Clock options
  typedef enum
  {
    NRF905_CLK_4MHZ  = 0x00,
    NRF905_CLK_8MHZ  = 0x01,
    NRF905_CLK_12MHZ = 0x02,
    NRF905_CLK_16MHZ = 0x03,
    NRF905_CLK_20MHZ = 0x04
  }tenRF905_Clock_Opt;


  // Bit positions
  typedef enum
  {
    NRF905_STATUS_DR = 5,
    NRF905_STATUS_AM = 7
  }tenRF905_Status_Bits;


  // Cmds
  typedef enum
  {
    NRF905_StIdle = 0,

    NRF905_StWriteConfig,
    NRF905_StWriteConfig2,
    NRF905_StWriteConfig3,
    NRF905_StWriteConfig4,
    NRF905_StWriteConfig5,
    NRF905_StWriteConfig6,
    NRF905_StWriteConfig7,
    NRF905_StWriteConfig8,

    NRF905_StReceive,
    NRF905_StReceive2,
    NRF905_StReceive3,
    NRF905_StReceive4,
    NRF905_StReceive5,
    NRF905_StReceive6,
    NRF905_StReceive7,
    NRF905_StReceive8,

    NRF905_StTransmit,
    NRF905_StTransmit2,
    NRF905_StTransmit3,
    NRF905_StTransmit4,
    NRF905_StTransmit5,
    NRF905_StTransmit6,
    NRF905_StTransmit7,
    NRF905_StTransmit8,
    NRF905_StTransmit9
  }tenCmdState;

  // Events
  typedef enum
  {
    NRF905_EvDummy = 0,
    NRF905_EvError,
    NRF905_EvDone,
    NRF905_EvLoop,
    NRF905_EvStart,
    NRF905_EvStartCmd,
    NRF905_EvDataReady,
    NRF905_EvSpiDmaRxReady,
    NRF905_EvSpiDmaTxReady,
    NRF905_EvTimer
  }tenCmdEvents;


  // Consts
  typedef enum
  {
    NRF905_MAX_PAYLOAD            = 32,    // Maximum payload size
    NRF905_CFG_REG_SIZE           = 10,    // Configuration register count
    NRF905_WAIT_PWR_DWN_2_ST_BY   = 3000,  // 3ms
    NRF905_WAIT_STBY_2_COM        = 650,   // 650us
    NRF905_WAIT_COM_2_COM         = 550,   // 550us
    NRF905_WAIT_TIMEOUT_25ms      = 25000, // 25ms
    NRF905_WAIT_TIMEOUT_10ms      = 10000, // 10ms
    NRF905_WAIT_TIMEOUT_1ms       = 1000,  // 1ms
    NRF905_WAIT_300us             = 300,   // 300us
    NRF905_WAIT_250us             = 250,   // 250us
    NRF905_WAIT_200us             = 200,   // 200us
    NRF905_WAIT_100us             = 100,   // 100us
    NRF905_WAIT_50us              = 50,    // 50us
    NRF905_WAIT_10us              = 10,    // 10us
    NRF905_WAIT_5us               = 5,     // 5us

    NRF905_WAIT_TX_MODE_WAIT      = NRF905_WAIT_5us,
    NRF905_WAIT_TX_DELAY          = NRF905_WAIT_250us
  }tenRF905_Consts;

  typedef struct __attribute__((packed))
  {
    uint8 Ch_No:8;

    uint8 Ch_No8:1;
    uint8 Hfreq_Pll:1;
    uint8 Output_Power:2;
    uint8 Rx_Reduced_Power:1;
    uint8 Auto_Retransmit:1;
    uint8 NotUsed1:2;

    uint8  Rx_Transmit_Width:3;
    uint8  NotUsed2:1;
    uint8  Tx_Transmit_Width:3;
    uint8  NotUsed3:1;

    uint8  Rx_Payload_Width;//:6;
    //uint8  NotUsed4:2;

    uint8  Tx_Payload_Width;//:6;
    //uint8  NotUsed5:2;

    uint32 RxAdress;

    uint8  Output_Clock_Frequency:2;
    uint8  Output_Clock_Enabled:1;
    uint8  Crystal_Osc_Freq:3;
    uint8  CRC_Check_enable:1;
    uint8  CRC_Mode:1;

  }tstNRF905_Config;

  typedef struct
  {
    uint8  NotUsed1:5;
    uint8  DR:1;
    uint8  CD:1;
    uint8  AM:1;
  }tstNRF905_Status;

  typedef struct __attribute__((packed))
  {
    // Write is in the right sequence. Cmd + Data
    // Then the Tx Spi data can be easily send with one DMA-action
    uint8               u8SpiCmdCfg;
    tstNRF905_Config    mstConfig;
    uint8               u8SpiCmdTxPl;
    uint8               mui8TxPayLoad[NRF905_MAX_PAYLOAD];
    uint8               u8SpiCmdTxAdr;
    uint32              mui32TxAdress;
    uint8               u8SpiCmdRxPl;
    uint8               mui8RxPayLoad[NRF905_MAX_PAYLOAD];
    uint8               u8SpiCmdTxPlW;
    uint8               mui8TxPayLoadWidth;
    uint8               u8SpiCmdRxPlW;
    uint8               mui8RxPayLoadWidth;
    tstNRF905_Status    mstStatus;
  }tstNRF905;

  tstNRF905     mstNRF905;

  tenRF905_Mode menMode;

  cNRF905_Pins  mPins;
  cNRF905_Spi   mSpi;
  cNRF905_Timer mTimer;
  u8            mu8DmaCmdBuf;

  tenCmdState   mSm;

  uint32 mui32TxCnt;
  uint32 mui32RxCnt;

  cNRF905(uint32 lui32RxAdress, uint32 lui32TxAdress)
    : mPins(False), mSpi(False), mTimer(False)
  {
    mstNRF905.mstConfig.Rx_Transmit_Width = NRF905_ADDR_SIZE_4;
    mstNRF905.mstConfig.Tx_Transmit_Width = NRF905_ADDR_SIZE_4;

    mstNRF905.mstConfig.Rx_Payload_Width  = NRF905_MAX_PAYLOAD;
    mstNRF905.mstConfig.Tx_Payload_Width  = NRF905_MAX_PAYLOAD;

    mstNRF905.mstConfig.RxAdress          = lui32RxAdress;

    mstNRF905.mui32TxAdress = lui32TxAdress;

    mstNRF905.mstConfig.Output_Clock_Frequency = NRF905_OUTCLK_DISABLE;
    mstNRF905.mstConfig.Output_Clock_Enabled   = NRF905_OUTCLK_DISABLE;
    mstNRF905.mstConfig.Crystal_Osc_Freq       = NRF905_CLK_16MHZ;

    SetChannel_433((u16)NRF905_CHANNEL_433_2, False);
    SetCrc(0);

    mstNRF905.mstConfig.CRC_Check_enable       = NRF905_CRC_DISABLE; //NRF905_CRC_ENABLE;
    mstNRF905.mstConfig.CRC_Mode               = NRF905_CRC_16;


    mstNRF905.u8SpiCmdCfg   = NRF905_CMD_W_CONFIG;
    mstNRF905.u8SpiCmdTxPl  = NRF905_CMD_W_TX_PAYLOAD;
    mstNRF905.u8SpiCmdTxAdr = NRF905_CMD_W_TX_ADDRESS;
    mstNRF905.u8SpiCmdRxPl  = NRF905_CMD_R_RX_PAYLOAD;

    mstNRF905.u8SpiCmdTxPlW = NRF905_CMD_W_CONFIG_TX_PW;
    mstNRF905.mui8TxPayLoadWidth = mstNRF905.mstConfig.Tx_Payload_Width;

    mstNRF905.u8SpiCmdRxPlW = NRF905_CMD_W_CONFIG_RX_PW;
    mstNRF905.mui8RxPayLoadWidth = mstNRF905.mstConfig.Rx_Payload_Width;
  }

  cComNode::tenError vInitHw()
  {
    mPins.vInit();
    mSpi.vInit();
    mTimer.vInit();

    uint32 lui32TxAdress = mstNRF905.mui32TxAdress;

    // Device Switching Times: PWR_DWN -> ST_BY mode: 3 ms
    // Von daher 3ms lang probieren
    // Bei 3Mhz SW-SPI (kein DMA) dauert eine Tx->Rx Sequenz ca. 45us (10*8Byte ca. 80Byte)
    mTimer.vDisableIrq();
    mTimer.vStart(3000);

    do
    {
      mstNRF905.mui32TxAdress = lui32TxAdress;
      vWrite_TxAdress();
      mstNRF905.mui32TxAdress = 0xFFFFFFFF;
      vRead_TxAdress();
    } while ((mstNRF905.mui32TxAdress != lui32TxAdress) && (!mTimer.isDone()));

    mTimer.vStop();
    mTimer.vEnableIrq();

    //Check if write => read is working
    if (mstNRF905.mui32TxAdress == lui32TxAdress)
    {
      vWrite_Config();
      //vRead_Config();
      vRead_Status();
      return cComNode::tenError::enNoError;
    }
    return cComNode::tenError::enErDeviceInitErrorDevice;
  }

  bool bStartUpdateCfg()
  {
    if (mSm == NRF905_StIdle)
    {
      mSm = NRF905_StWriteConfig;
      bEventHandler(cNRF905::NRF905_EvDummy);
      return True;
    }
    return False;
  }

  bool bEventHandler(tenCmdEvents lenEvent) // __attribute__((optimize("-O0")))
  {
    do
    {
      if (lenEvent == cNRF905::NRF905_EvLoop)
      {
        lenEvent = cNRF905::NRF905_EvDummy;
      }

      switch (mSm)
      {
        // ----------------------------------------------------------------------------------------
        case NRF905_StWriteConfig:
          mSm = NRF905_StWriteConfig2;
          {
            u16 u16Delay = u16Start_Set_Mode(NRF905_RADIO_MODE_IDLE);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
          }
          break;

        case NRF905_StWriteConfig2:
          // Config schreiben
          mSm = NRF905_StWriteConfig3;
          mPins.mCS_SPI.vSet0();
          mSpi.vStartDMA((uint8*)&mstNRF905.u8SpiCmdCfg, sizeof(tstNRF905_Config) + 1, SPI_DIRECTION_TX); // + 1 Wegen Indexbyte am Anfang
          break;

        case NRF905_StWriteConfig3:
          mSm = NRF905_StWriteConfig4;
          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       bevor alles auf dem realen Bus fertig ist
          mSpi.vWaitBussy();
          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StWriteConfig4:
          // TX Adresse schreiben
          mSm = NRF905_StWriteConfig5;
          mPins.mCS_SPI.vSet0();
          mSpi.vStartDMA((uint8*)&mstNRF905.u8SpiCmdTxAdr, 4 + 1, SPI_DIRECTION_TX); // + 1 Wegen Indexbyte am Anfang
          break;

        case NRF905_StWriteConfig5:
          mSm = NRF905_StWriteConfig6;
          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       bevor alles auf dem realen Bus fertig ist
          mSpi.vWaitBussy();
          mPins.mCS_SPI.vSet1();

          // 5us warten bis einstellungen übernommen sind
          mTimer.vStart(5);
          break;

        case NRF905_StWriteConfig6:
          mSm = NRF905_StIdle;
          mTimer.vStop();
          break;

        // ----------------------------------------------------------------------------------------

        case NRF905_StTransmit: // TX Payload Cnt Setzen Commando
          mSm = NRF905_StTransmit2;

          if (mstNRF905.mstConfig.Tx_Payload_Width == 0)
          {
            mSm = NRF905_StIdle;
          }
          else
          {
            u16 u16Delay = u16Start_Set_Mode(NRF905_RADIO_MODE_TX);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
             lenEvent = cNRF905::NRF905_EvLoop;
            }
          }
          break;

        case NRF905_StTransmit2: // TX Payload Cnt Setzen Daten
          mSm = NRF905_StTransmit3;

          if (mstNRF905.mui8TxPayLoadWidth != mstNRF905.mstConfig.Tx_Payload_Width)
          {
            mPins.mCS_SPI.vSet0();
            mstNRF905.mui8TxPayLoadWidth = mstNRF905.mstConfig.Tx_Payload_Width;
            mSpi.vStartDMA((uint8*)&mstNRF905.u8SpiCmdTxPlW, 2, SPI_DIRECTION_TX);
          }
          else
          {
            lenEvent = cNRF905::NRF905_EvLoop;
          }
          break;

        case NRF905_StTransmit3: // TX Payload Setzen Commando
          mSm = NRF905_StTransmit4;

          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       before alles auf dem realen Bur fertig ist
          mSpi.vWaitBussy();

          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StTransmit4: // TX Payload Setzen Daten
          mSm = NRF905_StTransmit5;
          mPins.mCS_SPI.vSet0();
          mSpi.vStartDMA(&mstNRF905.u8SpiCmdTxPl, mstNRF905.mstConfig.Tx_Payload_Width + 1, SPI_DIRECTION_TX);
          break;

        case NRF905_StTransmit5: // in TX Modus aktivieren
          mSm = NRF905_StTransmit6;

          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       before alles auf dem realen Bur fertig ist
          mSpi.vWaitBussy();

          mPins.mCS_SPI.vSet1();
          {
            u16 u16Delay = u16Start_Set_Mode(NRF905_RADIO_MODE_TX_PULSE);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
          }
          break;

        case NRF905_StTransmit6: // Senden und auf DR warten
          mSm = NRF905_StTransmit7;

          {
            u16 u16Delay = u16Start_Set_Mode(NRF905_RADIO_MODE_TX);
            if (u16Delay)
            {
              mTimer.vStart(u16Delay);
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
          }
          break;

        case NRF905_StTransmit7: // Senden und auf DR warten
          mSm = NRF905_StTransmit8;

          mTimer.vStop();
          vRead_Status();
          if (mstNRF905.mstStatus.DR == 0)
          {
            mTimer.vStart(10000);
            mPins.vEnableIrqDR();
          }
          else
          {
            lenEvent = cNRF905::NRF905_EvLoop;
          }
          break;

        case NRF905_StTransmit8: // in Idle Modus
          mSm = NRF905_StIdle;

          mTimer.vStop();
          mPins.vDisableIrqDR();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StTransmit9: // Fertig
          mSm = NRF905_StIdle;
          break;

        // ----------------------------------------------------------------

        case NRF905_StReceive:
          /*mSm = NRF905_StReceive2;

          bStart_Set_Mode(NRF905_RADIO_MODE_RX);
          if (mstNRF905.mstConfig.Rx_Payload_Width == 0)
          {
            mSm = NRF905_StIdle;
          }
          else
          {
            mPins.mCS_SPI.vSet0();
            //Dummy Read to reset AM & DR
            mSpi.vStartDMA(&mstNRF905.u8SpiCmdRxPl, 1, SPI_DIRECTION_TX); // 'Playload lesen' senden
          }*/

          mSm = NRF905_StReceive4;
          //{
              /*u16 u16Delay =*/ u16Start_Set_Mode(NRF905_RADIO_MODE_RX);
          //  if (u16Delay)
          //  {
          //    mTimer.vStart(u16Delay);
          //  }
          //  else
          //  {
              lenEvent = cNRF905::NRF905_EvLoop;
          //  }
          //}
          break;

        case NRF905_StReceive2:
          mSm = NRF905_StReceive3;

          //Dummy Read to reset AM & DR
          mSpi.vStartDMA(mstNRF905.mui8RxPayLoad, mstNRF905.mstConfig.Rx_Payload_Width, SPI_DIRECTION_RX);
          break;

        case NRF905_StReceive3:
          mSm = NRF905_StReceive4;

          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       before alles auf dem realen Bur fertig ist
          mSpi.vWaitBussy();

          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StReceive4: // RX Payload Cnt Setzen Daten
          mSm = NRF905_StReceive5;

          if (mstNRF905.mui8RxPayLoadWidth != mstNRF905.mstConfig.Rx_Payload_Width)
          {
            mPins.mCS_SPI.vSet0();
            mstNRF905.mui8RxPayLoadWidth = mstNRF905.mstConfig.Rx_Payload_Width;
            mSpi.vStartDMA((uint8*)&mstNRF905.u8SpiCmdRxPlW, 2, SPI_DIRECTION_TX);
          }
          else
          {
            lenEvent = NRF905_EvLoop;
          }
          break;

        case NRF905_StReceive5: // Empfänger einschalten und auf DR warten
          mSm = NRF905_StReceive6;


          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       before alles auf dem realen Bur fertig ist
          mSpi.vWaitBussy();

          mPins.mCS_SPI.vSet1();

          vRead_Status();
          if (mstNRF905.mstStatus.DR)
          {
            lenEvent = NRF905_EvLoop;
          }
          else
          {
            mPins.vEnableIrqDR();
            vRead_Status();
            if (mstNRF905.mstStatus.DR)
            {
              mPins.vDisableIrqDR();
              lenEvent = NRF905_EvLoop;
            }
          }
          break;

        case NRF905_StReceive6:
          mSm = NRF905_StIdle;

          vRead_Status();

          if ((lenEvent == NRF905_EvDataReady) || // Externer Interrupt
              ((mstNRF905.mstStatus.DR)))
          {
            mSm = NRF905_StReceive7;
            mPins.mCS_SPI.vSet0();
            mPins.vDisableIrqDR();
            mSpi.vStartDMA(&mstNRF905.u8SpiCmdRxPl, 1, SPI_DIRECTION_TX); // 'Playload lesen' senden
          }
          break;

        case NRF905_StReceive7: // in Idle Modus
          mSm = NRF905_StReceive8;

          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       bevor alles auf dem realen Bus fertig ist
          mSpi.vWaitBussy();

          mSpi.vStartDMA(mstNRF905.mui8RxPayLoad, mstNRF905.mstConfig.Rx_Payload_Width, SPI_DIRECTION_RX);
          break;

        case NRF905_StReceive8: // Fertig
          mSm = NRF905_StIdle;

          // FlKa: Bussy Waiting in der Interrupt routine !
          //       Ging aber nicht anders, weil DMA und TXE kommen,
          //       before alles auf dem realen Bur fertig ist
          mSpi.vWaitBussy();

          mPins.mCS_SPI.vSet1();
          lenEvent = NRF905_EvLoop;
          break;

        // ----------------------------------------------------------------
        default:
          break;
      }
    } while (lenEvent == cNRF905::NRF905_EvLoop);

    return (mSm == NRF905_StIdle);
  }

  void vStartTransmit()
  {
    mSm = NRF905_StTransmit;
    bEventHandler(NRF905_EvDummy);
  }

  void vStartReceive()
  {
    mSm = NRF905_StReceive;
    bEventHandler(NRF905_EvDummy);
  }


  u16 u16Start_Set_Mode(tenRF905_Mode lenNewMode)  // __attribute__((optimize("-O0")))
  {
    u16 lu16Ret = 0;

    if (menMode != lenNewMode)
    {
      switch (lenNewMode)
      {
        case NRF905_RADIO_MODE_PWR_DOWN:
          mSm = NRF905_StIdle;
          mPins.mPWR.vSet0();
          mPins.mTx_En.vSet0();
          mPins.mTRx_Cn.vSet0();
          break;
        case NRF905_RADIO_MODE_IDLE:
          mPins.mPWR.vSet1();
          mPins.mTRx_Cn.vSet0();
          mPins.mTx_En.vSet0();
          if (menMode == NRF905_RADIO_MODE_PWR_DOWN) lu16Ret = 3000;
          break;
        case NRF905_RADIO_MODE_RX:
          mPins.mPWR.vSet1();
          mPins.mTx_En.vSet0();
          mPins.mTRx_Cn.vSet1();
          if (menMode == NRF905_RADIO_MODE_PWR_DOWN) lu16Ret = 3000 + 650;
          if (menMode == NRF905_RADIO_MODE_IDLE)     lu16Ret = 650;
          if (menMode == NRF905_RADIO_MODE_TX)       lu16Ret = 550;
          break;
        case NRF905_RADIO_MODE_TX:
          mPins.mPWR.vSet1();
          mPins.mTRx_Cn.vSet0();
          mPins.mTx_En.vSet1();
          if (menMode == NRF905_RADIO_MODE_PWR_DOWN) lu16Ret = 3000 + 650;
          if (menMode == NRF905_RADIO_MODE_IDLE)     lu16Ret = 650;
          if (menMode == NRF905_RADIO_MODE_RX)       lu16Ret = 550;
          break;
        case NRF905_RADIO_MODE_TX_PULSE:
          mPins.mTRx_Cn.vSet1();
          lu16Ret = NRF905_WAIT_TX_MODE_WAIT;
          break;
        default:
          break;
      }
      menMode = lenNewMode;
    }
    return lu16Ret;
  }

  void vRead_Config()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_R_CONFIG;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vRead((uint8*)&mstNRF905.mstConfig, NRF905_CFG_REG_SIZE);
    }
    mPins.mCS_SPI.vSet1();
  }
  void vWrite_Config()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_W_CONFIG;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vWrite((uint8*)&mstNRF905.mstConfig, NRF905_CFG_REG_SIZE);
    }
    mPins.mCS_SPI.vSet1();
  }

  /*void vRead_TxPayLoad()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_R_TX_PAYLOAD;
      mSpi.vWrite(&lui8Cmd, 1);

      // Read full payload,that DR and AM are reseted
      //mSpi.vRead((uint8*)mui8TxPayLoad, mstConfig.Tx_Payload_Width);
      mSpi.vRead((uint8*)mstNRF905.mui8TxPayLoad, NRF905_MAX_PAYLOAD);
    }
    mPins.mCS_SPI.vSet1();
  }*/

  /*void vWrite_TxPayLoad()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_W_TX_PAYLOAD;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vWrite((uint8*)mstNRF905.mui8TxPayLoad, mstNRF905.mstConfig.Tx_Payload_Width);

      mui32TxCnt += mstNRF905.mstConfig.Tx_Payload_Width;
    }
    mPins.mCS_SPI.vSet1();
  }*/

  void vRead_TxAdress()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_R_TX_ADDRESS;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vRead((uint8*)&mstNRF905.mui32TxAdress, 4);
    }
    mPins.mCS_SPI.vSet1();
  }
  void vWrite_TxAdress()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_W_TX_ADDRESS;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vWrite((uint8*)&mstNRF905.mui32TxAdress, 4);
    }
    mPins.mCS_SPI.vSet1();
  }

  /*void vRead_RxPayLoad()
  {
    mPins.mCS_SPI.vSet0();
    {
      uint8 lui8Cmd = NRF905_CMD_R_RX_PAYLOAD;
      mSpi.vWrite(&lui8Cmd, 1);

      mSpi.vRead((uint8*)mstNRF905.mui8RxPayLoad, 32);
      mui32RxCnt += mstNRF905.mstConfig.Rx_Payload_Width;
    }
    mPins.mCS_SPI.vSet1();
  }*/


  /*void vRead_Status_Spi()
  {
    mPins.mCS_SPI.vSet0();
    {
      mSpi.vRead((uint8*)&mstNRF905.mstStatus, 1);
      mstNRF905.mstStatus.CD = 0;
    }
    mPins.mCS_SPI.vSet1();
  }*/

  void vRead_Status_Pin()
  {
    mstNRF905.mstStatus.DR = mPins.mDR.ui8Get();
    mstNRF905.mstStatus.CD = mPins.mCD.ui8Get();
    mstNRF905.mstStatus.AM = mPins.mAM.ui8Get();
  }

  void vRead_Status()
  {
    //vRead_Status_Spi();
    vRead_Status_Pin();
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


    mstNRF905.mstConfig.Ch_No             = lu16Channel & 0xFF;
    mstNRF905.mstConfig.Ch_No8            = lu16Channel >> 8;

    // Sets PLL in 433 or 868/915 MHz mode (default = 0).
    //  '0' – Chip operating in 433MHz band
    //  '1' – Chip operating in 868 or 915 MHz band
    mstNRF905.mstConfig.Hfreq_Pll         = NRF905_BAND_433;

    // Output power (default = 00).
    //  '11'  +10dBm 11.0mA
    //  '00'  -10dBm 14.0mA
    //  '01'  -2dBm  20.0mA
    //  '10'  +6dBm  30.0mA
    mstNRF905.mstConfig.Output_Power    = NRF905_PWR_10;

    // Reduces current in RX mode by 1.6mA. Sensitivity is reduced (default = 0).
    //  '0' – Normal operation
    //  '1' – Reduced power
    mstNRF905.mstConfig.Rx_Reduced_Power  = NRF905_LOW_RX_DISABLE;

    mstNRF905.mstConfig.Auto_Retransmit   = NRF905_AUTO_RETRAN_DISABLE;
  }

  void SetChannel_868(u16 lu16Channel)
  {
    //  862.0 MHz              [1]      [001010110] => 86
    //  868.2 MHz              [1]      [001110101] => 117
    //  868.4 MHz              [1]      [001110110] => 118
    //  869.8 MHz              [1]      [001111101] => 125

    //  902.2 MHz              [1]      [100011111] => 287
    //  902.4 MHz              [1]      [100100000] => 288
    //  927.8 MHz              [1]      [110011111] => 415
    mstNRF905.mstConfig.Ch_No             = lu16Channel & 0xFF;
    mstNRF905.mstConfig.Ch_No8            = lu16Channel >> 8;

    // Sets PLL in 433 or 868/915 MHz mode (default = 0).
    //  '0' – Chip operating in 433MHz band
    //  '1' – Chip operating in 868 or 915 MHz band
    mstNRF905.mstConfig.Hfreq_Pll         = NRF905_BAND_868;

    // Output power (default = 00).
    //  '11'  +10dBm 11.0mA
    //  '00'  -10dBm 14.0mA
    //  '01'  -2dBm  20.0mA
    //  '10'  +6dBm  30.0mA
    mstNRF905.mstConfig.Output_Power    = NRF905_PWR_10;

    // Reduces current in RX mode by 1.6mA. Sensitivity is reduced (default = 0).
    //  '0' – Normal operation
    //  '1' – Reduced power
    mstNRF905.mstConfig.Rx_Reduced_Power  = NRF905_LOW_RX_DISABLE;

    mstNRF905.mstConfig.Auto_Retransmit   = NRF905_AUTO_RETRAN_DISABLE;
  }

  void SetCrc(u8 lu8BitWidth) // 0, 8, 16
  {
    switch (lu8BitWidth)
    {
      case 0:
        mstNRF905.mstConfig.CRC_Check_enable       = NRF905_CRC_DISABLE; //NRF905_CRC_ENABLE;
        mstNRF905.mstConfig.CRC_Mode               = NRF905_CRC_16;
        break;
      case 8:
        mstNRF905.mstConfig.CRC_Check_enable       = NRF905_CRC_ENABLE;
        mstNRF905.mstConfig.CRC_Mode               = NRF905_CRC_8;
        break;
      case 16:
        mstNRF905.mstConfig.CRC_Check_enable       = NRF905_CRC_ENABLE;
        mstNRF905.mstConfig.CRC_Mode               = NRF905_CRC_16;
        break;
    }
  }

  void vEnableIrq()
  {
    mSpi.vEnableIrq();
    mTimer.vEnableIrq();
  }

  void vDisableIrq()
  {
    mSpi.vDisableIrq();
    mTimer.vDisableIrq();
  }
};


class cNRF905Master : public cNRF905, public cComNodeMasterSingle
{
  public:
  u8 mu8SelfTimer;
  u8 mu8MsgIdx;
  u8 mu8Timeout_ms;

  cNRF905Master(u32 lui32RxAdress, u32 lui32TxAdress, u16 luInitDelay_ms = 1)
    : cNRF905(lui32RxAdress, lui32TxAdress),
      cComNodeMasterSingle(luInitDelay_ms)
  {
    mu32Baudrate = 50000;
    mu8Timeout_ms = 0;
  }

  cComNode::tenError enInitHw() override
  {
    return cNRF905::vInitHw();
  }

  void vPowerDown()
  {
    cNRF905::u16Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_PWR_DOWN);
  }

  void vResetCom() override
  {
    if (cComNodeMasterSingle::mSm != cComNode::tenState::enStIdle)
    {
      mTimer.vStop();
      cComNodeMasterSingle::mSm = cComNode::tenState::enStIdle;
      mError = cComNode::enNoError;
    }
    cNRF905::mSm = NRF905_StIdle;
  }

  void vComError() override
  {
    cComNode::tenState lSm = cComNodeMasterSingle::mSm;
    cComNode::tenError lError = mError;

    vResetCom();

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(lError, lSm);
    }
  }

  void vTick1ms()
  {
    cComNodeMasterSingle::vTick1ms();
    if (mu8Timeout_ms)
    {
      mu8Timeout_ms--;
      if (mu8Timeout_ms == 0)
      {
        mError = cComNode::enErTimeout;
        vComError();
      }
    }
  }

  void IrqHandler(cComNode::tenEvent lenEvent)
  {
    if (cNRF905::mSm != cNRF905::NRF905_StIdle)
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvDmaRxTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          lenEvent = cComNode::tenEvent::enEvDone;
          break;
        case cComNode::tenEvent::enEvDmaTxTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        case cComNode::tenEvent::enEvUsartTimer:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvTimer))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        case cComNode::tenEvent::enEvUsartExtiP1:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvDataReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        default:
          mError = cComNode::tenError::enErUnknown;
          vComError();
          return;
          break;
      }
    }

    if (lenEvent == cComNode::tenEvent::enEvUsartTimer)
    {
      if (mu8SelfTimer)
      {
        mu8SelfTimer = 0;
        vSm(lenEvent);
      }
      else
      {
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvTimer);
      }
    }
    //mError = cComNode::tenError::enErUnknown;
    //vComError();
  }


  void vDone()
  {
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    cComNodeMasterSingle::mSm = cComNode::tenState::enStIdle;
    mpcActiveSlave->vComDone();
  }

  bool bCheckBusy() override
  {
    return True;
  }

  void vStartTimer(u16 luTime_us)
  {
    cNRF905::mTimer.vStart(luTime_us);
  };

  void vWaitMsg(cComMsg<u16>* lpcActiveMsg)
  {
    if (bCheckBusy())
    {
      vResetComTimeout();
      mpcActiveMsg = lpcActiveMsg;
      vSm(cComNode::tenEvent::enEvStartWait);
    }
    else
    {
      mError = cComNode::enErStartWhileBusy;
      vErrorHdl();
    }
  }

  // Start wird sowohl vom Interrupt wie auch von der Main benutzt
  // Um keine Laufzeit Konflikte zu bekommen, wird solange der Interrupt gesperrt
  void vPreStart() override
  {
    cNRF905::vDisableIrq();
  }

  void vPostStart() override
  {
    cNRF905::vEnableIrq();
  }

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEvent);

    switch (cComNodeMasterSingle::mSm)
    {
      case cComNode::tenState::enStIdle:
      {
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
            case cComNode::tenEvent::enEvStartSkipAdr:
              switch (mpcActiveMsg->mDir)
              {
                case cComNode::tenDirection::enIsTx:
                  {
                    u8 u8Len = mpcActiveMsg->Len();
                    mu8MsgIdx = 0;
                    if (u8Len > 32)
                    {
                      u8Len = 32;
                    }
                    cComNodeMasterSingle::mSm = cComNode::tenState::enStEndTx;
                    cNRF905::mstNRF905.mstConfig.Tx_Payload_Width = u8Len;
                    cMemTools::vMemCpy(cNRF905::mstNRF905.mui8TxPayLoad, mpcActiveMsg->Data(), u8Len);
                    cNRF905::vStartTransmit();
                  }
                  break;
                case cComNode::tenDirection::enIsRx:
                  {
                    u8 u8Len = mpcActiveMsg->Len();
                    mu8MsgIdx = 0;
                    if (u8Len > 32)
                    {
                      u8Len = 32;
                    }
                    cComNodeMasterSingle::mSm = cComNode::tenState::enStEndRx;
                    cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
                    cNRF905::vStartReceive();
                    mu8Timeout_ms = 25;
                  }
                  break;
                case cComNode::tenDirection::enCfgWrite:
                  cComNodeMasterSingle::mSm = cComNode::tenState::enStEnd;
                  cNRF905::bStartUpdateCfg();
                  break;
                case cComNode::tenDirection::enCfgRead:
                  vDone();
                  break;
                default:
                  vDone();
                  break;
              }
              break;

            case cComNode::tenEvent::enEvStartWait:
              // Rx
              if (mpcActiveMsg->isRx())
              {
                u8 u8Len = mpcActiveMsg->Len();
                mu8MsgIdx = 0;
                if (u8Len > 32)
                {
                  u8Len = 32;
                }
                // In WaitAdress gibt es keinen Timeout
                cComNodeMasterSingle::mSm = cComNode::tenState::enStWaitAdress;
                cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
                cNRF905::vStartReceive();
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

    case cComNode::tenState::enStWaitAdress: // In WaitAdress gibt es keinen Timeout
    case cComNode::tenState::enStEndRx:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
        {
          mu8Timeout_ms = 0;
          u8 u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
          if (u8Len > 32)
          {
            u8Len = 32;
          }

          cMemTools::vMemCpy(mpcActiveMsg->Data() + mu8MsgIdx, cNRF905::mstNRF905.mui8RxPayLoad, u8Len);
          mu8MsgIdx += 32;

          // Alle Daten gesendet ?
          if (mpcActiveMsg->Len() <= mu8MsgIdx)
          {
            vDone();
          }
          else
          {
            //cComNodeSlave::mSm = cComNode::tenState::enStEndRx;
            u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
            if (u8Len > 32)
            {
              u8Len = 32;
            }
            cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
            cNRF905::vStartReceive();
            mu8Timeout_ms = 25;
          }
        }
      }
      break;

    case cComNode::tenState::enStTx: // Conseutive Msg
      {
        u8 u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
        if (u8Len > 32)
        {
          u8Len = 32;
        }
        cComNodeMasterSingle::mSm = cComNode::tenState::enStEndTx;
        cNRF905::mstNRF905.mstConfig.Tx_Payload_Width = u8Len;
        cMemTools::vMemCpy(cNRF905::mstNRF905.mui8TxPayLoad, mpcActiveMsg->Data() + mu8MsgIdx, u8Len);
        cNRF905::vStartTransmit();
      }
      break;


    case cComNode::tenState::enStEndTx:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
        {
          mu8MsgIdx += 32;
          // Alle Daten gesendet ?
          if (mpcActiveMsg->Len() <= mu8MsgIdx)
          {
            vDone();
          }
          else
          {
            mu8SelfTimer = 1;
            cComNodeMasterSingle::mSm = cComNode::tenState::enStTx;
            mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
          }
        }
      }
      break;

    case cComNode::tenState::enStEnd:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
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



class cNRF905Slave : public cNRF905, public cComNodeSlave
{
  public:
  u8 mu8SelfTimer;
  u8 mu8MsgIdx;
  u8 mu8Timeout_ms;

  cNRF905Slave(u32 lui32RxAdress, u32 lui32TxAdress, u16 luInitDelay_ms = 1)
    : cNRF905(lui32RxAdress, lui32TxAdress),
      cComNodeSlave(luInitDelay_ms)
  {
    mu32Baudrate = 50000;
    mu8Timeout_ms = 0;
  }

  cComNode::tenError enInitHw() override
  {
    return cNRF905::vInitHw();
  }

  void vStopCom()
  {
    mTimer.vStop();
    cComNodeSlave::mSm = cComNode::tenState::enStIdle;
    cNRF905::mSm = NRF905_StIdle;
    mError = cComNode::enNoError;
  }

  void vPowerDown()
  {
    cNRF905::u16Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_PWR_DOWN);
  }

  void vHwStop() override
  {
    vPowerDown();
  }

  void vHwContinue() override
  {
  }

  void vResetCom() override
  {
    if (cComNodeSlave::mSm != cComNode::tenState::enStIdle)
    {
      mTimer.vStop();
      cComNodeSlave::mSm = cComNode::tenState::enStIdle;
      mError = cComNode::enNoError;
    }
    cNRF905::mSm = NRF905_StIdle;
  }

  void vComError() override
  {
    cComNode::tenState lSm = cComNodeSlave::mSm;
    cComNode::tenError lError = mError;

    vResetCom();

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(lError, lSm);
    }
  }

  void vTick1ms()
  {
    cComNodeSlave::vTick1ms();
    if (mu8Timeout_ms)
    {
      mu8Timeout_ms--;
      if (mu8Timeout_ms == 0)
      {
        mError = cComNode::enErTimeout;
        vComError();
      }
    }
  }

  void IrqHandler(cComNode::tenEvent lenEvent)
  {
    if (cNRF905::mSm != cNRF905::NRF905_StIdle)
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvDmaRxTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          lenEvent = cComNode::tenEvent::enEvDone;
          break;
        case cComNode::tenEvent::enEvDmaTxTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        case cComNode::tenEvent::enEvUsartTimer:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvTimer))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        case cComNode::tenEvent::enEvUsartExtiP1:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvDataReady))
          {
            vSm(cComNode::tenEvent::enEvDone);
          }
          return;
          break;
        default:
          mError = cComNode::tenError::enErUnknown;
          vComError();
          return;
          break;
      }
    }

    if (lenEvent == cComNode::tenEvent::enEvUsartTimer)
    {
      if (mu8SelfTimer)
      {
        mu8SelfTimer = 0;
        vSm(lenEvent);
      }
      else
      {
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvTimer);
      }
    }
    //mError = cComNode::tenError::enErUnknown;
    //vComError();
  }


  void vDone()
  {
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    cComNodeSlave::mSm = cComNode::tenState::enStIdle;
    mpcActiveSlave->vComDone();
  }

  bool bCheckBusy() override
  {
    return True;
  }

  void vStartTimer(u16 luTime_us)
  {
    cNRF905::mTimer.vStart(luTime_us);
  };

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEvent);

    switch (cComNodeSlave::mSm)
    {
      case cComNode::tenState::enStIdle:
      {
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
            case cComNode::tenEvent::enEvStartSkipAdr:
              switch (mpcActiveMsg->mDir)
              {
                case cComNode::tenDirection::enIsTx:
                  {
                    u8 u8Len = mpcActiveMsg->Len();
                    mu8MsgIdx = 0;
                    if (u8Len > 32)
                    {
                      u8Len = 32;
                    }
                    cComNodeSlave::mSm = cComNode::tenState::enStEndTx;
                    cNRF905::mstNRF905.mstConfig.Tx_Payload_Width = u8Len;
                    cMemTools::vMemCpy(cNRF905::mstNRF905.mui8TxPayLoad, mpcActiveMsg->Data(), u8Len);
                    cNRF905::vStartTransmit();
                  }
                  break;
                case cComNode::tenDirection::enIsRx:
                  {
                    u8 u8Len = mpcActiveMsg->Len();
                    mu8MsgIdx = 0;
                    if (u8Len > 32)
                    {
                      u8Len = 32;
                    }
                    cComNodeSlave::mSm = cComNode::tenState::enStEndRx;
                    cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
                    cNRF905::vStartReceive();
                    mu8Timeout_ms = 25;
                  }
                  break;
                case cComNode::tenDirection::enCfgWrite:
                  cComNodeSlave::mSm = cComNode::tenState::enStEnd;
                  cNRF905::bStartUpdateCfg();
                  break;
                case cComNode::tenDirection::enCfgRead:
                  vDone();
                  break;
                default:
                  vDone();
                  break;
              }
              break;

            case cComNode::tenEvent::enEvStartWait:
              // Rx
              if (mpcActiveMsg->isRx())
              {
                u8 u8Len = mpcActiveMsg->Len();
                mu8MsgIdx = 0;
                if (u8Len > 32)
                {
                  u8Len = 32;
                }
                // In WaitAdress gibt es keinen Timeout
                cComNodeSlave::mSm = cComNode::tenState::enStWaitAdress;
                cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
                cNRF905::vStartReceive();
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

    case cComNode::tenState::enStWaitAdress: // In WaitAdress gibt es keinen Timeout
    case cComNode::tenState::enStEndRx:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
        {
          u8 u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
          mu8Timeout_ms = 0;
          if (u8Len > 32)
          {
            u8Len = 32;
          }

          cMemTools::vMemCpy(mpcActiveMsg->Data() + mu8MsgIdx, cNRF905::mstNRF905.mui8RxPayLoad, u8Len);
          mu8MsgIdx += 32;

          // Alle Daten gesendet ?
          if (mpcActiveMsg->Len() <= mu8MsgIdx)
          {
            vDone();
          }
          else
          {
            //cComNodeSlave::mSm = cComNode::tenState::enStEndRx;
            u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
            if (u8Len > 32)
            {
              u8Len = 32;
            }
            cNRF905::mstNRF905.mstConfig.Rx_Payload_Width = u8Len;
            cNRF905::vStartReceive();
            mu8Timeout_ms = 25;
          }
        }
      }
      break;

    case cComNode::tenState::enStTx: // Conseutive Msg
      {
        u8 u8Len = mpcActiveMsg->Len() - mu8MsgIdx;
        if (u8Len > 32)
        {
          u8Len = 32;
        }
        cComNodeSlave::mSm = cComNode::tenState::enStEndTx;
        cNRF905::mstNRF905.mstConfig.Tx_Payload_Width = u8Len;
        cMemTools::vMemCpy(cNRF905::mstNRF905.mui8TxPayLoad, mpcActiveMsg->Data() + mu8MsgIdx, u8Len);
        cNRF905::vStartTransmit();
      }
      break;


    case cComNode::tenState::enStEndTx:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
        {
          mu8MsgIdx += 32;
          // Alle Daten gesendet ?
          if (mpcActiveMsg->Len() <= mu8MsgIdx)
          {
            vDone();
          }
          else
          {
            mu8SelfTimer = 1;
            cComNodeSlave::mSm = cComNode::tenState::enStTx;
            mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
          }
        }
      }
      break;

    case cComNode::tenState::enStEnd:
      {
        if (lenEvent == cComNode::tenEvent::enEvDone)
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


#endif // __cNRF905_H
