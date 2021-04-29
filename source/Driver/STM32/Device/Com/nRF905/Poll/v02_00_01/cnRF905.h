#ifndef _cNRF905_H
#define _cNRF905_H


#include "TYPEDEF.h"

#ifdef WIN32
  #define NRF905_INLINE
  #define NRF905_NO_INLINE
#else
  #define NRF905_INLINE     inline
  #define NRF905_NO_INLINE  __attribute__ ((__noinline__))

  #include "tGPPin.h"
  #include "ClockInfo.h"
  #include "nRF905_Spi.h"
#endif


class cNRF905_Pins
{
  public:

  //Outputs
  cGPPIN mCS;     // Chip select
  cGPPIN mTXE;    // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  cGPPIN mPWR;    // Power
  cGPPIN mCS_SPI; // Chip select SPI

  //Inputs
  cGPPIN mCD;  // Carrier Detected
  cGPPIN mAM;  // Adress Match
  cGPPIN mDR;  // Data Ready*/

  cNRF905_Pins()
    : mCS(    RCC_AHBPeriph_GPIOC, GPIOC, GPIO_Pin_14, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mTXE(   RCC_AHBPeriph_GPIOB, GPIOB, GPIO_Pin_5,  GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mPWR(   RCC_AHBPeriph_GPIOB, GPIOB, GPIO_Pin_4,  GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mCS_SPI(RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_4,  GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 1),

      mCD( RCC_AHBPeriph_GPIOB, GPIOB, GPIO_Pin_3,  GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mAM( RCC_AHBPeriph_GPIOC, GPIOC, GPIO_Pin_13, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0),
      mDR( RCC_AHBPeriph_GPIOC, GPIOC, GPIO_Pin_15, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0)
  {
  };
};


class cNRF905
{
  public:

  // Radio Mode
  typedef enum
  {
    NRF905_RADIO_MODE_INIT,
    NRF905_RADIO_MODE_PWR_DOWN, /* Power down */
    NRF905_RADIO_MODE_IDLE,     /* standby */
    NRF905_RADIO_MODE_TX,       /* Transmitting */
    NRF905_RADIO_MODE_RX        /* Receive mode */
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
    NRF905_CMD_NOP          = 0xFF,
    NRF905_CMD_W_CONFIG     = 0x00,
    NRF905_CMD_R_CONFIG     = 0x10,
    NRF905_CMD_W_TX_PAYLOAD = 0x20,
    NRF905_CMD_R_TX_PAYLOAD = 0x21,
    NRF905_CMD_W_TX_ADDRESS = 0x22,
    NRF905_CMD_R_TX_ADDRESS = 0x23,
    NRF905_CMD_R_RX_PAYLOAD = 0x24,
    NRF905_CMD_CHAN_CONFIG  = 0x80
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


  // Consts
  typedef enum
  {
    NRF905_MAX_PAYLOAD            = 32,   // Maximum payload size
    NRF905_CFG_REG_SIZE           = 10,   // Configuration register count
    NRF905_WAIT_PWR_DWN_2_ST_BY   = 3000, // 3ms
    NRF905_WAIT_STBY_2_COM        = 650,  // 650us
    NRF905_WAIT_COM_2_COM         = 550   // 550us
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

    uint8  Rx_Payload_Width:6;
    uint8  NotUsed4:2;

    uint8  Tx_Payload_Width:6;
    uint8  NotUsed5:2;

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

  tstNRF905_Config mstConfig;
  uint8            mui8TxPayLoad[NRF905_MAX_PAYLOAD];
  uint32           mui32TxAdress;
  uint8            mui8RxPayLoad[NRF905_MAX_PAYLOAD];
  tstNRF905_Status mstStatus;

  tenRF905_Mode    menMode;

  cNRF905_Pins mPins;
  cNRF905_Spi  mSpi;
  cClockInfo   mClock;

  cNRF905(uint32 lui32RxAdress, uint32 lui32TxAdress);

  uint8 ui8Init();

  uint32 mui32TxCnt;
  uint32 mui32RxCnt;

  void  vSet_Mode(tenRF905_Mode lenNewMode);

  void vRead_Config();
  void vWrite_Config();

  void vRead_TxPayLoad();
  void vWrite_TxPayLoad();

  void vRead_TxAdress();
  void vWrite_TxAdress();

  void vRead_RxPayLoad();

  void vRead_Status_Spi();
  void vRead_Status_Pin();
  void vRead_Status();

  void vTransmit(bool lboWait);
  bool isDataReady();
};

#endif // __cNRF905_H
