#pragma once

/**
 * Created by Gemini, 2026-06-01.
 * Resources: XPT2046 Datasheet / ADS7846 Datasheet.
 *
 * @class CXPT2046
 * @brief Hardware-Abstraktion für den XPT2046 Touch-Controller via SPI.
 * Bildet das 8-Bit Steuerwort (Control Byte) als Bitfeld ab.
 */

#include "TypeDef.h"

// --- Hauptklasse cSIC45X ---
struct cNRF905_Defs
{
  /** @brief Sendeleistung (Output Power) Definitionen */
  enum tenOutputPower : u8
  {
    nPWR_Minus10dBm = 0x00, ///< -10 dBm (100 µW)
    nPWR_Minus2dBm  = 0x01, ///< -2 dBm (630 µW)
    nPWR_Plus6dBm   = 0x02, ///< +6 dBm (4 mW)
    nPWR_Plus10dBm  = 0x03  ///< +10 dBm (10 mW)
  };

  /** @brief Frequenzband-Auswahl (433 MHz vs 868/915 MHz) */
  enum tenFrequencyBand : u8
  {
    nBAND_433MHz     = 0x00,    ///< 433 MHz Band
    nBAND_868_915MHz = 0x01 ///< 868/915 MHz Band
  };

  // Radio Mode
  enum tenMode : u8
  {
    nRADIO_MODE_INIT = 0,
    nRADIO_MODE_PWR_DOWN,   /* Power down */
    nRADIO_MODE_IDLE,       /* standby */
    nRADIO_MODE_TX,
    nRADIO_MODE_RX,
    nRADIO_MODE_TX_PULSE
  };

  // Output power (n means negative, n10 = -10).
  enum tenChannelNo : u8
  {
    nCHANNEL_433_2 = 108,
    nCHANNEL_433_3 = 109,
    nCHANNEL_433_4 = 110,
    nCHANNEL_433_5 = 111,
    nCHANNEL_433_6 = 112,
    nCHANNEL_433_7 = 113,
    nCHANNEL_433_8 = 114,
    nCHANNEL_433_9 = 115,
    nCHANNEL_434_0 = 116,
    nCHANNEL_434_1 = 117,
    nCHANNEL_434_2 = 118,
    nCHANNEL_434_3 = 119,
    nCHANNEL_434_4 = 120,
    nCHANNEL_434_5 = 121,
    nCHANNEL_434_6 = 122,
    nCHANNEL_434_7 = 123
  };

  // Save a few mA by reducing receive sensitivity.
  enum tenLowPowerRx : u8
  {
    nLOW_RX_DISABLE = 0x0,  /* Disable low power receive */
    nLOW_RX_ENABLE  = 0x1   /* Enable low power receive */
  };

  // Constantly retransmit payload while in transmit mode.
  // Can be useful in areas with lots of interference, but you'll need to make sure you can differentiate between re-transmitted packets and new packets (like an ID number).
  // Other transmissions will be blocked if collision avoidance is enabled.
  enum tenAutoRetran : u8
  {
    nAUTO_RETRAN_DISABLE = 0x0, /* Disable auto re-transmit */
    nAUTO_RETRAN_ENABLE  = 0x1  /* Enable auto re-transmit */
  };

  // Output a clock signal on pin 3 of IC.
  enum tenOutclk : u8
  {
    nOUTCLK_DISABLE = 0x00,  /* Disable output clock */
    nOUTCLK_ENABLE  = 0x01,  /* Enable output clock */
    nOUTCLK_4MHZ    = 0x00,  /* 4MHz clock */
    nOUTCLK_2MHZ    = 0x01,  /* 2MHz clock */
    nOUTCLK_1MHZ    = 0x02,  /* 1MHz clock */
    nOUTCLK_500KHZ  = 0x03,  /* 500KHz clock */
  };

  // CRC Checksum.
  enum tenCrc : u8
  {
    nCRC_DISABLE = 0x00, // Disable CRC
    nCRC_ENABLE  = 0x01, // Enable CRC
    nCRC_8       = 0x00, // 8bit CRC
    nCRC_16      = 0x01, // 16bit CRC
  };

  // Address size.
  enum tenAddrSize : u8
  {
    nADDR_SIZE_1 = 0x01,  /* 1 byte */
    nADDR_SIZE_4 = 0x04,  /* 4 bytes */
  };

  // Clock options
  enum tenClockOpt : u8
  {
    nCLK_4MHZ  = 0x00,
    nCLK_8MHZ  = 0x01,
    nCLK_12MHZ = 0x02,
    nCLK_16MHZ = 0x03,
    nCLK_20MHZ = 0x04
  };


  // Bit positions
  enum tenStatusBits : u8
  {
    nSTATUS_DR = 5,
    nSTATUS_AM = 7
  };

  // --- Register-Instanzen (Unions) ---

  union tunAddressBuffer 
  { 
    u8  au8Byte[4];
    u32 u32Adr;
  };

  union tunReg00_Config
  { 
    u8 au8Byte[10]; 
    struct 
    { 
      // Byte 9
      u8 CRC_Mode       : 1;
      u8 CRC_En         : 1;
      u8 XOF            : 3;
      u8 UP_CLK_EN      : 1; 
      u8 UP_CLK_FREQ    : 2; 
      
      // Byte 8,7,6,5
      tunAddressBuffer unRxAdress;

      // Byte 4
      u8 NotUsed1        : 2;
      u8 TX_PayloadWidth : 6;
      
      // Byte 3
      u8 NotUsed2        : 2;
      u8 RX_PayloadWidth : 6;

      // Byte 2
      u8 NotUsed3        : 1;
      u8 TX_AddressWidth : 3;
      u8 NotUsed4        : 1;
      u8 RX_AddressWidth : 3;

      // Byte 1
      u8 NotUsed5        : 2;
      u8 AutoRetransmit  : 1;
      u8 RX_RED_PWR      : 1;
      u8 PA_PWR          : 2;
      u8 HFREQ_PLL       : 1;
      u8 ChannelNum_High : 1;

      // Byte 0
      u8  ChannelNum_Low;
    } stBits; 
  };

  union tunReg80_ChCfg
  { 
    u8 au8Byte[2]; 
    struct 
    { 
      u16 CH_NO         : 9;
      u16 HFREQ_PLL     : 1;
      u16 PA_PWR        : 2;
      u16 Reserve       : 4;
    } stBits; 
  };
  
  // Puffer-Strukturen für TX/RX Daten und Adressen via SPI-Befehle
  union tunDataBuffer           { u8 au8Byte[32]; };


  void vSetChannel_433(tunReg00_Config& lunCfg, u16 lu16Channel, bool bIKnowWhatIDo)
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

    lunCfg.stBits.ChannelNum_Low  = lu16Channel & 0xFF;
    lunCfg.stBits.ChannelNum_High = lu16Channel >> 8;

    // Sets PLL in 433 or 868/915 MHz mode (default = 0).
    //  '0' – Chip operating in 433MHz band
    //  '1' – Chip operating in 868 or 915 MHz band
    lunCfg.stBits.HFREQ_PLL         = tenFrequencyBand::nBAND_433MHz;

    // Output power (default = 00).
    //  '11'  +10dBm 11.0mA
    //  '00'  -10dBm 14.0mA
    //  '01'  -2dBm  20.0mA
    //  '10'  +6dBm  30.0mA
    lunCfg.stBits.PA_PWR = tenOutputPower::nPWR_Plus10dBm;

    // Reduces current in RX mode by 1.6mA. Sensitivity is reduced (default = 0).
    //  '0' – Normal operation
    //  '1' – Reduced power
    lunCfg.stBits.RX_RED_PWR = tenLowPowerRx::nLOW_RX_DISABLE;

    lunCfg.stBits.AutoRetransmit = tenAutoRetran::nAUTO_RETRAN_DISABLE;
  }

  void vSetChannel_868(tunReg00_Config& lunCfg, u16 lu16Channel)
  {
    //  862.0 MHz              [1]      [001010110] => 86
    //  868.2 MHz              [1]      [001110101] => 117
    //  868.4 MHz              [1]      [001110110] => 118
    //  869.8 MHz              [1]      [001111101] => 125

    //  902.2 MHz              [1]      [100011111] => 287
    //  902.4 MHz              [1]      [100100000] => 288
    //  927.8 MHz              [1]      [110011111] => 415
    lunCfg.stBits.ChannelNum_Low  = lu16Channel & 0xFF;
    lunCfg.stBits.ChannelNum_High = lu16Channel >> 8;

    // Sets PLL in 433 or 868/915 MHz mode (default = 0).
    //  '0' – Chip operating in 433MHz band
    //  '1' – Chip operating in 868 or 915 MHz band
    lunCfg.stBits.HFREQ_PLL         = tenFrequencyBand::nBAND_868_915MHz;

    // Output power (default = 00).
    //  '11'  +10dBm 11.0mA
    //  '00'  -10dBm 14.0mA
    //  '01'  -2dBm  20.0mA
    //  '10'  +6dBm  30.0mA
    lunCfg.stBits.PA_PWR = tenOutputPower::nPWR_Plus10dBm;

    // Reduces current in RX mode by 1.6mA. Sensitivity is reduced (default = 0).
    //  '0' – Normal operation
    //  '1' – Reduced power
    lunCfg.stBits.RX_RED_PWR = tenLowPowerRx::nLOW_RX_DISABLE;

    lunCfg.stBits.AutoRetransmit = tenAutoRetran::nAUTO_RETRAN_DISABLE;
  }

  void vSetCrc(tunReg00_Config& lunCfg, u8 lu8BitWidth) // 0, 8, 16
  {
    switch (lu8BitWidth)
    {
      case 0:
        lunCfg.stBits.CRC_En   = tenCrc::nCRC_DISABLE; //NRF905_CRC_ENABLE;
        lunCfg.stBits.CRC_Mode = tenCrc::nCRC_16;
        break;
      case 8:
        lunCfg.stBits.CRC_En   = tenCrc::nCRC_ENABLE;
        lunCfg.stBits.CRC_Mode = tenCrc::nCRC_8;
        break;
      case 16:
        lunCfg.stBits.CRC_En   = tenCrc::nCRC_ENABLE;
        lunCfg.stBits.CRC_Mode = tenCrc::nCRC_16;
        break;
    }
  }
};
