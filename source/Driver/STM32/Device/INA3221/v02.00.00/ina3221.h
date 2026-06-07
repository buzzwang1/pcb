#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "ina3221_RegMap.h"
#include "cI2C.h"

/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/
#define INA3221_I2C_ADDRESS_CONF_0       (0x40)     // A0 = GND, A1 = GND
#define INA3221_I2C_ADDRESS_CONF_1       (0x41)     // A0 = VS+, A1 = GND
#define INA3221_I2C_ADDRESS_CONF_2       (0x42)     // A0 = SDA, A1 = GND
#define INA3221_I2C_ADDRESS_CONF_3       (0x43)     // A0 = SCL, A1 = GND
#define INA3221_I2C_ADDRESS_DEFAULT      INA3221_I2C_ADDRESS_CONF_0

/*=========================================================================*/

class cINA3221 : public cComSeqHdlI2c<cIna3221_RegisterMap>
{
  public:

  cINA3221(cI2cMaster* lpcI2C, u8 lu8Adr)
    : cComSeqHdlI2c(lpcI2C, lu8Adr << 1)
  {
  }

  float fGetV_mv(u8 lu8Chn)
  {
    u16 u16Value = 0;

    // Das Datenblatt besagt: 13-Bit Wert, Bit 0-2 sind 0.
    switch (lu8Chn)
    {
      case 1: u16Value = (cIna3221_RegisterMap::unCh1Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh1Bus.stByte.Lb; break;
      case 2: u16Value = (cIna3221_RegisterMap::unCh2Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh2Bus.stByte.Lb; break;
      case 3: u16Value = (cIna3221_RegisterMap::unCh3Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh3Bus.stByte.Lb; break;
    }
    return (float)u16Value;
  }

  float fGetI_mA(u8 lu8Chn, float lfShunt_Ohm)
  {
    i16 i16Value = 0;

    // Das Datenblatt besagt: 13-Bit Wert, Bit 0-2 sind 0.
    switch (lu8Chn)
    {
      case 1: i16Value = (i16)((cIna3221_RegisterMap::unCh1Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh1Bus.stByte.Lb); break;
      case 2: i16Value = (i16)((cIna3221_RegisterMap::unCh2Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh2Bus.stByte.Lb); break;
      case 3: i16Value = (i16)((cIna3221_RegisterMap::unCh3Bus.stByte.Hb << 8) + cIna3221_RegisterMap::unCh3Bus.stByte.Lb); break;
    }

    // Jedes Bit entspricht 40 µV
    return (float)(i16Value * (0.04f / 8.0f)) / lfShunt_Ohm;
  }

};

