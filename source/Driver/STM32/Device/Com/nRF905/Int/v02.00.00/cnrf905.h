#pragma once


#include "TYPEDEF.h"
#include "ComDat.h"
#include "tGPPin.h"
#include "nRF905_Spi.h"

#include "cDmaHelper.h"

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
    NRF905_EvSpiTc,
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
  cNRF905_Timer mTimer;

  tenCmdState   mSm;

  u32   mu32Prescaler;

  SPI_TypeDef* mSpi;

  IRQn_Type mnNVIC_IRQChannelEv;

  // SPI ist Vollcuplex. Von daher 2 DMA Kanäle
  DMA_Channel_TypeDef* mstDmaChTx;
  DMA_Channel_TypeDef* mstDmaChRx;

  // Spi muss beim Empfangen Dummy Daten senden.
  u32 mu32DummyTx;

  // Sicherung von Register nach Init
  u32 mu32SPI_CFG1;
  u32 mu32DMATx_CRT1;
  u32 mu32DMARx_CRT1;

  u8 mu8DmaChTx;
  u8 mu8DmaChRx;

  u8 mu8DataWidth;

  cNRF905(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx)
    : mPins(), mTimer()
  {
    mSpi = lpstSpi;

    mu8DmaChTx = lu8DmaChTx;
    mu8DmaChRx = lu8DmaChRx;

    mstDmaChTx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[lu8DmaChTx]));
    mstDmaChRx = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[lu8DmaChRx]));

    mu32DummyTx = 0; // 0x55555555;
    mu8DataWidth = 8;
  }

  inline void vStopDMA()
  {
    mstDmaChTx->CCR &= ~DMA_CCR_EN;
    mstDmaChRx->CCR &= ~DMA_CCR_EN;
  }

  // Überprüft, ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return (mstDmaChRx->CBR1 & 0xFFFF);
  }

  // Überprüft, ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas gesendet wurde
  u16 u16GetDmaCounterTx()
  {
    return (mstDmaChTx->CBR1 & 0xFFFF);
  }


  void vStartDMATx(uint8* pBuffer, u16 BufferSize)
  {
    // Zuerst SPI dann DMA klar machen
    // Sonst hat bei der 2. Übertragung das 1 Byte fehlt
    LL_SPI_Disable(mSpi);

    // Is programmed in number of bytes / DataWidth
    switch (mu8DataWidth)
    {
      case 32: LL_SPI_SetTransferSize(mSpi, BufferSize / 4); break;
      case 16: LL_SPI_SetTransferSize(mSpi, BufferSize / 2); break;
      case  8:
      default: LL_SPI_SetTransferSize(mSpi, BufferSize); break;
    }

    // Auf Simplex gehen, um RX OVR Error zu vermeiden
    LL_SPI_SetTransferDirection(mSpi, LL_SPI_SIMPLEX_TX);
    LL_SPI_Enable(mSpi);
    LL_SPI_StartMasterTransfer(mSpi);

    mstDmaChTx->CFCR = DMA_CFCR_TCF;
    mstDmaChTx->CCR &= ~DMA_CCR_EN;

    // Is programmed in number of bytes
    mstDmaChTx->CBR1 = BufferSize;

    mstDmaChTx->CSAR = (uint32)pBuffer;
    mstDmaChTx->CTR1 |= DMA_CTR1_SINC;

    mstDmaChTx->CCR |= DMA_CCR_EN;
  }

  void vFlushRx()
  {
    // In case there is still something in the RX Register
    // Flush RXDR, else DMA will have offset
    if (mSpi->SR & SPI_SR_RXP)
    {
      volatile u8 lu8Dummy = mSpi->RXDR;
      UNUSED(lu8Dummy);
    }
  }

  void vStartDMARx(uint8* pBuffer, u16 BufferSize)
  {
    vFlushRx();

    // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, BufferSize);
    LL_SPI_SetTransferDirection(mSpi, LL_SPI_FULL_DUPLEX);
    LL_SPI_Enable(mSpi);
    LL_SPI_StartMasterTransfer(mSpi);

    mstDmaChRx->CFCR = DMA_CFCR_TCF;
    mstDmaChRx->CCR &= ~DMA_CCR_EN;

    mstDmaChTx->CFCR = DMA_CFCR_TCF;
    mstDmaChTx->CCR &= ~DMA_CCR_EN;

    // Is programmed in number of bytes
    switch (mu8DataWidth)
    {
      case 32: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize * 4; break;
      case 16: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize * 2; break;
      case  8:
      default: mstDmaChRx->CBR1 = mstDmaChTx->CBR1 = BufferSize; break;
    }

    mstDmaChRx->CDAR = (uint32)pBuffer;
    mstDmaChTx->CSAR = (u32)(&mu32DummyTx); // Zum Empfangen Dummy Daten schicken

    mstDmaChTx->CTR1 &= ~DMA_CTR1_SINC;

    mstDmaChRx->CCR |= DMA_CCR_EN;
    mstDmaChTx->CCR |= DMA_CCR_EN;
  }

  u32 u32BaudToRegister(u16 lu16Pres)
  {
    // mu32Prescaler wird bei SPI als prescaler verwendet
    switch (lu16Pres)
    {
      case 0:    return LL_SPI_BAUDRATEPRESCALER_BYPASS;
      case 2:    return LL_SPI_BAUDRATEPRESCALER_DIV2;
      case 4:    return LL_SPI_BAUDRATEPRESCALER_DIV4;
      case 8:    return LL_SPI_BAUDRATEPRESCALER_DIV8;
      case 16:   return LL_SPI_BAUDRATEPRESCALER_DIV16;
      case 32:   return LL_SPI_BAUDRATEPRESCALER_DIV32;
      case 64:   return LL_SPI_BAUDRATEPRESCALER_DIV64;
      case 128:  return LL_SPI_BAUDRATEPRESCALER_DIV128;
      case 256:  return LL_SPI_BAUDRATEPRESCALER_DIV256;
    }

    return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }

  u8 isErrorActive()
  {
    uint32 lui32ErrorMask;
    lui32ErrorMask = SPI_SR_MODF | SPI_SR_TIFRE | SPI_SR_CRCE | SPI_SR_OVR | SPI_SR_UDR;
    if ((mSpi->SR & lui32ErrorMask)) return 1;
    return 0;
  }

  void vInitDma()
  {
    /* Enable DMA clock */
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    cDmaHelper::vSet(mu8DmaChTx);
    cDmaHelper::vSet(mu8DmaChRx);

    /* DMA node configuration declaration */
    LL_DMA_InitTypeDef           lstDmaInit;

    LL_DMA_StructInit(&lstDmaInit);

    lstDmaInit.BlkHWRequest  = LL_DMA_HWREQUEST_SINGLEBURST;
    lstDmaInit.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;

    lstDmaInit.SrcBurstLength  = 1;
    lstDmaInit.DestBurstLength = 1;

    lstDmaInit.SrcDataWidth  = LL_DMA_SRC_DATAWIDTH_BYTE;
    lstDmaInit.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;


    lstDmaInit.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
    lstDmaInit.BlkDataLength = 0;

    lstDmaInit.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;


    // -Port 0 should be allocated for transfers toand from
    //  peripherals, because there is a direct hardware datapath
    //  between this portand the APB peripherals, outside the  AHB matrix
    // -Port 1 should be allocated for transfers toand from
    //  memory, which are performed through the AHB interconnect.

    if (mSpi == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_RX;
    if (mSpi == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_RX;
    if (mSpi == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_RX;

    lstDmaInit.SrcAddress  = (u32)&mSpi->RXDR;
    lstDmaInit.DestAddress = 0;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_FIXED;
    lstDmaInit.DestIncMode = LL_DMA_DEST_INCREMENT;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT0; // Source is periph
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1; // Dest is memory
    LL_DMA_Init(GPDMA1, mu8DmaChRx, &lstDmaInit);

    if (mSpi == SPI1) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI1_TX;
    if (mSpi == SPI2) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI2_TX;
    if (mSpi == SPI3) lstDmaInit.Request = LL_GPDMA1_REQUEST_SPI3_TX;
    lstDmaInit.SrcAddress  = 0;
    lstDmaInit.DestAddress = (u32)&mSpi->TXDR;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_INCREMENT;
    lstDmaInit.DestIncMode = LL_DMA_DEST_FIXED;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT1; // Source is memory
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0; // Dest is periph
    LL_DMA_Init(GPDMA1, mu8DmaChTx, &lstDmaInit);
  }

  cComNode::tenError enInitHw(bool lbMaster)
  {
    mstNRF905.mstConfig.Rx_Transmit_Width = NRF905_ADDR_SIZE_4;
    mstNRF905.mstConfig.Tx_Transmit_Width = NRF905_ADDR_SIZE_4;

    mstNRF905.mstConfig.Rx_Payload_Width  = NRF905_MAX_PAYLOAD;
    mstNRF905.mstConfig.Tx_Payload_Width  = NRF905_MAX_PAYLOAD;

    mstNRF905.mstConfig.RxAdress          = 0;

    mstNRF905.mui32TxAdress = 0;

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

    mPins.vInit();


    SPI_HandleTypeDef lhSpi = {};

    if (mSpi == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    if (mSpi == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    if (mSpi == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();

    vStopDMA();

    /* SPI Peripheral Disable */
    LL_SPI_Disable(mSpi);


    /* I2C configuration */
    /* I2C configuration */
    lhSpi.Instance = mSpi;


    lhSpi.Init.Mode                      = SPI_MODE_MASTER;
    lhSpi.Init.Direction                 = SPI_DIRECTION_2LINES;
    lhSpi.Init.DataSize                  = SPI_DATASIZE_8BIT;
    lhSpi.Init.CLKPolarity               = SPI_POLARITY_LOW;
    lhSpi.Init.CLKPhase                  = SPI_PHASE_1EDGE;
    lhSpi.Init.NSS                       = SPI_NSS_SOFT;
    //lhSpi.Init.NSSPMode =
    //lhSpi.Init.NSSPolarity =
    //lhSpi.Init.MasterSSIdleness =
    lhSpi.Init.BaudRatePrescaler         = SPI_BAUDRATEPRESCALER_256;
    lhSpi.Init.FirstBit                  = SPI_FIRSTBIT_MSB;
    lhSpi.Init.TIMode                    = SPI_TIMODE_DISABLE;
    lhSpi.Init.CRCCalculation            = SPI_CRCCALCULATION_DISABLE;
    //lhSpi.Init.CRCPolynomial             =
    //lhSpi.Init.CRCLength                 =
    //lhSpi.Init.TxCRCInitializationPatter =
    //lhSpi.Init.RxCRCInitializationPatter =
    lhSpi.Init.FifoThreshold             = SPI_FIFO_THRESHOLD_04DATA;
    lhSpi.Init.MasterInterDataIdleness   = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    lhSpi.Init.MasterReceiverAutoSusp    = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    lhSpi.Init.MasterKeepIOState         = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    lhSpi.Init.IOSwap                    = SPI_IO_SWAP_DISABLE;
    lhSpi.Init.ReadyMasterManagement     = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    lhSpi.Init.ReadyPolarity             = SPI_RDY_POLARITY_HIGH;


    /* Peripheral clock enable */
    HAL_SPI_Init(&lhSpi);

    LL_SPI_SetBaudRatePrescaler(mSpi, u32BaudToRegister(mu32Prescaler));

    mSpi->IFCR = 0xFFFF;

    LL_SPI_EnableDMAReq_RX(mSpi);
    LL_SPI_EnableDMAReq_TX(mSpi);

    if (isErrorActive())
    {
      return cComNode::enErDeviceInitErrorDevice;
    }

    if (mSpi == SPI1) mnNVIC_IRQChannelEv = SPI1_IRQn;
    if (mSpi == SPI2) mnNVIC_IRQChannelEv = SPI2_IRQn;
    if (mSpi == SPI3) mnNVIC_IRQChannelEv = SPI3_IRQn;

    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(mnNVIC_IRQChannelEv, 8, 8);
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(mnNVIC_IRQChannelEv);

    if (lbMaster)
    {
      //mSpi->IER |= SPI_IER_UDRIE | SPI_IER_OVRIE | SPI_IER_MODFIE;
      mSpi->IER |= SPI_IER_EOTIE;
    }

    vInitDma();

    mu32SPI_CFG1   = mSpi->CFG1;
    mu32DMATx_CRT1 = mstDmaChTx->CTR1;
    mu32DMARx_CRT1 = mstDmaChRx->CTR1;


    return cComNode::enNoError;
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
          vStartDMATx((uint8*)&mstNRF905.u8SpiCmdCfg, sizeof(tstNRF905_Config) + 1); // + 1 Wegen Indexbyte am Anfang
          break;

        case NRF905_StWriteConfig3:
          mSm = NRF905_StWriteConfig4;

          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StWriteConfig4:
          // TX Adresse schreiben
          mSm = NRF905_StWriteConfig5;
          mPins.mCS_SPI.vSet0();
          vStartDMATx((uint8*)&mstNRF905.u8SpiCmdTxAdr, 4 + 1); // + 1 Wegen Indexbyte am Anfang
          break;

        case NRF905_StWriteConfig5:
          mSm = NRF905_StWriteConfig6;

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
            vStartDMATx((uint8*)&mstNRF905.u8SpiCmdTxPlW, 2);
          }
          else
          {
            lenEvent = cNRF905::NRF905_EvLoop;
          }
          break;

        case NRF905_StTransmit3: // TX Payload Setzen Commando
          mSm = NRF905_StTransmit4;

          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StTransmit4: // TX Payload Setzen Daten
          mSm = NRF905_StTransmit5;
          mPins.mCS_SPI.vSet0();
          vStartDMATx(&mstNRF905.u8SpiCmdTxPl, mstNRF905.mstConfig.Tx_Payload_Width + 1);
          break;

        case NRF905_StTransmit5: // in TX Modus aktivieren
          mSm = NRF905_StTransmit6;

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
          mSm = NRF905_StReceive4;

          u16Start_Set_Mode(NRF905_RADIO_MODE_RX);
          lenEvent = cNRF905::NRF905_EvLoop;

          break;

        case NRF905_StReceive2:
          mSm = NRF905_StReceive3;

          //Dummy Read to reset AM & DR
          vStartDMARx(mstNRF905.mui8RxPayLoad, mstNRF905.mstConfig.Rx_Payload_Width);
          break;

        case NRF905_StReceive3:
          mSm = NRF905_StReceive4;

          mPins.mCS_SPI.vSet1();
          lenEvent = cNRF905::NRF905_EvLoop;
          break;

        case NRF905_StReceive4: // RX Payload Cnt Setzen Daten
          mSm = NRF905_StReceive5;

          if (mstNRF905.mui8RxPayLoadWidth != mstNRF905.mstConfig.Rx_Payload_Width)
          {
            mPins.mCS_SPI.vSet0();
            mstNRF905.mui8RxPayLoadWidth = mstNRF905.mstConfig.Rx_Payload_Width;
            vStartDMATx((uint8*)&mstNRF905.u8SpiCmdRxPlW, 2);
          }
          else
          {
            lenEvent = NRF905_EvLoop;
          }
          break;

        case NRF905_StReceive5: // Empfänger einschalten und auf DR warten
          mSm = NRF905_StReceive6;

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
            vStartDMATx(&mstNRF905.u8SpiCmdRxPl, 1); // 'Playload lesen' senden
          }
          break;

        case NRF905_StReceive7: // in Idle Modus
          mSm = NRF905_StReceive8;

          vStartDMARx(mstNRF905.mui8RxPayLoad, mstNRF905.mstConfig.Rx_Payload_Width);
          break;

        case NRF905_StReceive8: // Fertig
          mSm = NRF905_StIdle;

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

  //void vEnableIrq()
  //{
  //  mSpi.vEnableIrq();
  //  mTimer.vEnableIrq();
  //}
  //
  //void vDisableIrq()
  //{
  //  mSpi.vDisableIrq();
  //  mTimer.vDisableIrq();
  //}
};


class cNRF905Master : public cNRF905, public cComNodeMasterSingle
{
  public:
  u8 mu8SelfTimer;
  u8 mu8MsgIdx;
  u8 mu8Timeout_ms;

  cNRF905Master(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx,
                u16 lu32Prescaler = 256, u16 luInitDelay_ms = 1)
    : cNRF905(lpstSpi, lu8DmaChTx, lu8DmaChRx),
      cComNodeMasterSingle(luInitDelay_ms)
  {
    mu32Prescaler = lu32Prescaler;
    mu32Baudrate = 50000;
    mu8Timeout_ms = 0;
  }

  cComNode::tenError enInitHw() override
  {
    return cNRF905::enInitHw(True);
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

  void vIRQHandler(cComNode::tenEvent lenEvent)
  {
    if (cNRF905::mSm != cNRF905::NRF905_StIdle)
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvSpiTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiTc))
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
              switch (mpcActiveMsg->enDir())
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

  cNRF905Slave(SPI_TypeDef* lpstSpi, u8 lu8DmaChTx, u8 lu8DmaChRx,
               u16 lu32Prescaler = 256, u16 luInitDelay_ms = 1)
    : cNRF905(lpstSpi, lu8DmaChTx, lu8DmaChRx),
      cComNodeSlave(luInitDelay_ms)
  {
    mu32Prescaler = lu32Prescaler;
    mu32Baudrate = 50000;
    mu8Timeout_ms = 0;
  }

  cComNode::tenError enInitHw() override
  {
    return cNRF905::enInitHw(True);
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

  void vIRQHandler(cComNode::tenEvent lenEvent)
  {
    if (cNRF905::mSm != cNRF905::NRF905_StIdle)
    {
      switch (lenEvent)
      {
        case cComNode::tenEvent::enEvSpiTc:
          if (cNRF905::bEventHandler(cNRF905::NRF905_EvSpiTc))
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
              switch (mpcActiveMsg->enDir())
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

