//#include <string.h>

//#include "nRF905_spi.h"
#include "cnRF905.h"


// Workout channel from frequency & band
#define NRF905_CALC_CHANNEL(f, b)	((((f) / (1 + b)) - 422400000UL) / 100000UL)

cNRF905::cNRF905(uint32 lui32RxAdress, uint32 lui32TxAdress)
: mPins(), mSpi(), mClock()
{
  mstConfig.Ch_No             = NRF905_CALC_CHANNEL(433200000UL, NRF905_BAND_433) & 0xFF;
  mstConfig.Ch_No8            = NRF905_CALC_CHANNEL(433200000UL, NRF905_BAND_433) >> 8;

  mstConfig.Hfreq_Pll         = NRF905_BAND_433;
  mstConfig.Output_Power      = NRF905_PWR_10;
  mstConfig.Rx_Reduced_Power  = NRF905_LOW_RX_DISABLE;
  mstConfig.Auto_Retransmit   = NRF905_AUTO_RETRAN_DISABLE;

  mstConfig.Rx_Transmit_Width = NRF905_ADDR_SIZE_4;

  mstConfig.Tx_Transmit_Width = NRF905_ADDR_SIZE_4;


  mstConfig.Rx_Payload_Width  = NRF905_MAX_PAYLOAD;


  mstConfig.Tx_Payload_Width  = NRF905_MAX_PAYLOAD;

  mstConfig.RxAdress          = lui32RxAdress;

  mstConfig.Output_Clock_Frequency = NRF905_OUTCLK_DISABLE;
  mstConfig.Output_Clock_Enabled   = NRF905_OUTCLK_DISABLE;
  mstConfig.Crystal_Osc_Freq       = NRF905_CLK_16MHZ;
  mstConfig.CRC_Check_enable       = NRF905_CRC_ENABLE;
  mstConfig.CRC_Mode               = NRF905_CRC_16;

  mui32TxAdress = lui32TxAdress;

  menMode   = NRF905_RADIO_MODE_INIT;
  vSet_Mode(NRF905_RADIO_MODE_PWR_DOWN);

  mui32TxCnt = 0;
  mui32RxCnt = 0;
}

uint8 cNRF905::ui8Init()
{
  uint32 lui32TxAdress = mui32TxAdress;

  vWrite_TxAdress();
  mui32TxAdress = 0xFFFFFFFF;
  vRead_TxAdress();

  //Check if write => read is working
  if (mui32TxAdress == lui32TxAdress)
  {
    vWrite_Config();
    vRead_Config();
    vRead_Status();
    return 0;
  }

  return 1;
}

void cNRF905::vSet_Mode(tenRF905_Mode lenNewMode)
{
  if (menMode != lenNewMode)
  {
    switch (lenNewMode)
    {
      case NRF905_RADIO_MODE_PWR_DOWN:
        mPins.mPWR.vSet0();
        mPins.mCS.vSet0();
        mPins.mTXE.vSet0();
        break;
      case NRF905_RADIO_MODE_IDLE:
        mPins.mPWR.vSet1();
        mPins.mCS.vSet0();
        mPins.mTXE.vSet0();
        switch (menMode)
        {
          case NRF905_RADIO_MODE_PWR_DOWN:
            mClock.Delay_us(NRF905_WAIT_PWR_DWN_2_ST_BY);
            break;
          default:
            break;
        }
        break;
      case NRF905_RADIO_MODE_TX:
        mPins.mPWR.vSet1();
        mPins.mCS.vSet1();
        mPins.mTXE.vSet1();
        switch (menMode)
        {
          case NRF905_RADIO_MODE_PWR_DOWN:
            mClock.Delay_us(NRF905_WAIT_PWR_DWN_2_ST_BY);
            mClock.Delay_us(NRF905_WAIT_STBY_2_COM);
            break;
          case NRF905_RADIO_MODE_IDLE:
            mClock.Delay_us(NRF905_WAIT_STBY_2_COM);
            break;
          case NRF905_RADIO_MODE_RX:
            mClock.Delay_us(NRF905_WAIT_COM_2_COM);
            break;
          default:
            break;
        }
        break;
      case NRF905_RADIO_MODE_RX:
        mPins.mPWR.vSet1();
        mPins.mCS.vSet1();
        mPins.mTXE.vSet0();
        switch (menMode)
        {
          case NRF905_RADIO_MODE_PWR_DOWN:
            mClock.Delay_us(NRF905_WAIT_PWR_DWN_2_ST_BY);
            mClock.Delay_us(NRF905_WAIT_STBY_2_COM);
            break;
          case NRF905_RADIO_MODE_IDLE:
            mClock.Delay_us(NRF905_WAIT_STBY_2_COM);
            break;
          case NRF905_RADIO_MODE_TX:
            mClock.Delay_us(NRF905_WAIT_COM_2_COM);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    menMode = lenNewMode;
  }
}

void cNRF905::vRead_Config()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_R_CONFIG;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vRead((uint8*)&mstConfig, NRF905_CFG_REG_SIZE);
  }
  mPins.mCS_SPI.vSet1();
}
void cNRF905::vWrite_Config()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_W_CONFIG;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vWrite((uint8*)&mstConfig, NRF905_CFG_REG_SIZE);
  }
  mPins.mCS_SPI.vSet1();
}

void cNRF905::vRead_TxPayLoad()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_R_TX_PAYLOAD;
    mSpi.vWrite(&lui8Cmd, 1);

    // Read full payload,that DR and AM are reseted
    //mSpi.vRead((uint8*)mui8TxPayLoad, mstConfig.Tx_Payload_Width);
    mSpi.vRead((uint8*)mui8TxPayLoad, NRF905_MAX_PAYLOAD);
  }
  mPins.mCS_SPI.vSet1();
}
void cNRF905::vWrite_TxPayLoad()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_W_TX_PAYLOAD;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vWrite((uint8*)mui8TxPayLoad, mstConfig.Tx_Payload_Width);

    mui32TxCnt += mstConfig.Tx_Payload_Width;
  }
  mPins.mCS_SPI.vSet1();
}

void cNRF905::vRead_TxAdress()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_R_TX_ADDRESS;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vRead((uint8*)&mui32TxAdress, 4);
  }
  mPins.mCS_SPI.vSet1();
}
void cNRF905::vWrite_TxAdress()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_W_TX_ADDRESS;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vWrite((uint8*)&mui32TxAdress, 4);
  }
  mPins.mCS_SPI.vSet1();
}

void cNRF905::vRead_RxPayLoad()
{
  mPins.mCS_SPI.vSet0();
  {
    uint8 lui8Cmd = NRF905_CMD_R_RX_PAYLOAD;
    mSpi.vWrite(&lui8Cmd, 1);

    mSpi.vRead((uint8*)mui8RxPayLoad, mstConfig.Rx_Payload_Width);
    mui32RxCnt += mstConfig.Rx_Payload_Width;
  }
  mPins.mCS_SPI.vSet1();
}


void cNRF905::vRead_Status_Spi()
{
  mPins.mCS_SPI.vSet0();
  {
    mSpi.vRead((uint8*)&mstStatus, 1);
    mstStatus.CD = 0;
  }
  mPins.mCS_SPI.vSet1();
}

void cNRF905::vRead_Status_Pin()
{
  mstStatus.DR = mPins.mDR.ui8Get();
  mstStatus.CD = mPins.mCD.ui8Get();
  mstStatus.AM = mPins.mAM.ui8Get();
}

void cNRF905::vRead_Status()
{
  //vRead_Status_Spi();
  vRead_Status_Pin();
}

/*
void nRF905_setChannel(nRF905_band_t band, uint16 channel)
{
  config.reg1 = (config.reg1 & NRF905_MASK_CHANNEL) | band | ((channel>>8) & 0x01);

  STANDBY;
  spiSelect();
  {
    NRF905_Spi_Transfer_nr(NRF905_CMD_W_CONFIG | NRF905_REG_CHANNEL);
    NRF905_Spi_Transfer_nr((uint8)channel);
    NRF905_Spi_Transfer_nr(config.reg1);
  }
  spiDeselect();
}


void nRF905_setFrequency(nRF905_band_t band, uint32 freq)
{
  nRF905_setChannel(band, (uint16)NRF905_CALC_CHANNEL(freq, band));
}
*/

void cNRF905::vTransmit(bool lboWait)
{
  //Activate Transmission
  vSet_Mode(NRF905_RADIO_MODE_TX);

  mstStatus.DR = 0;
  if (lboWait)
  {
    while (mstStatus.DR == 0)
    {
      vRead_Status();
    }

    vSet_Mode(NRF905_RADIO_MODE_IDLE);
  }
}

bool cNRF905::isDataReady()
{
  if (menMode != NRF905_RADIO_MODE_RX)
  {
    return False;
  }

  mstStatus.DR = 0;
  mstStatus.AM = 0;

  vRead_Status();

  if ((mstStatus.DR != 0) && (mstStatus.AM != 0))
  {
    return True;
  }
  return False;
}


