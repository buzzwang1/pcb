#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "TMP102_RegMap.h"
#include "cI2C.h"

/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/
#define TMP102_I2C_ADDRESS0      (0x48 + 0) // A0 to Gnd
#define TMP102_I2C_ADDRESS1      (0x48 + 1) // A0 to V+
#define TMP102_I2C_ADDRESS2      (0x48 + 2) // A0 to SDA
#define TMP102_I2C_ADDRESS3      (0x48 + 3) // A0 to SCL
#define TMP102_I2C_ADDRESS_DEFAULT   TMP102_DEFAULT_ADDRESS0

/*=========================================================================*/

class cTMP102 : public cComSeqHdlI2c<cTMP102_RegisterMap>
{
  public:

  cTMP102(cI2cMaster* lpcI2C, u8 lu8Adr)
    : cComSeqHdlI2c(lpcI2C, lu8Adr << 1)
  {
  }

  void vCmdReadTemp()
  {
    vRequest(cTMP102_RegisterMap::nSEQ07_ReadTemp);
  }

  // The on-chip 12-bit ADC offers resolutions down to 0.0625°C.
  // Das 1 Byte enthält den Vorkomma-Temperaturwert
  // Das 2 Byte enthält den Nachkomma-Temperaturwert
  i16 i16GetTemp()
  {
    u16 lu16Temp;
    lu16Temp  = cTMP102_RegisterMap::unTemperature.u8Raw[0] << 8;
    lu16Temp += cTMP102_RegisterMap::unTemperature.u8Raw[1];

    return (lu16Temp / 256);
  }

  float fGetTemp()
  {
    u16 lu16Temp;
    lu16Temp =  cTMP102_RegisterMap::unTemperature.u8Raw[0] << 8;
    lu16Temp += cTMP102_RegisterMap::unTemperature.u8Raw[1];

    return (lu16Temp / 256.0f);
  }
};

