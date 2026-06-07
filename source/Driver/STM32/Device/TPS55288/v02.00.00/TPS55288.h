#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "TPS55288_RegMap.h"
#include "cI2C.h"

/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/
#define TPS55288_I2C_ADDRESS_CONF_0       (0x74)
#define TPS55288_I2C_ADDRESS_CONF_1       (0x75)
#define TPS55288_I2C_ADDRESS_DEFAULT       TPS55288_I2C_ADDRESS_CONF_0

/*=========================================================================*/

class cTPS55288 : public cComSeqHdlI2c<cTPS55288_RegisterMap>
{
  public:

  cTPS55288(cI2cMaster* lpcI2C, u8 lu8Adr)
    : cComSeqHdlI2c(lpcI2C, lu8Adr << 1)
  {
  }

  void vSetVoltage_mV(u16 lu16Value_mV)
  {
    // Internal feedback ratio
    //   00b = Set internal feedback ratio to 1,129mV / 0.2256 =>   5mV/digit (45mV =>  200mV...5324mV
    //   01b = Set internal feedback ratio to 1,129mV / 0.1128 =>  10mV/digit (45mV =>  400mV..10429mV
    //   10b = Set internal feedback ratio to 1,129mV / 0.0752 =>  15mV/digit (45mV =>  600mV..15973mV
    //   11b = Set internal feedback ratio to 1,129mV / 0.0564 =>  20mV/digit (45mV =>  800mV..21298mV

    // U = (45mV + x * 1,129mV) / Ration

    u16 lu16Zwerg = 0;
    switch (cTPS55288_RegisterMap::unVoutFs.stBit.IntFb)
    {
      case 0: 
        {
          float lfZwerg = (((float)lu16Value_mV * 0.2256f) - 45.0f) / 1.129f;
          if (lfZwerg > 0) lu16Zwerg = lfZwerg;
        }
        break;
      case 1: 
        {
          float lfZwerg = (((float)lu16Value_mV * 0.1128f) - 45.0f) / 1.129f;
          if (lfZwerg > 0) lu16Zwerg = lfZwerg;
        }
        break;
      case 2: 
        {
          float lfZwerg = (((float)lu16Value_mV * 0.0752f) - 45.0f) / 1.129f;
          if (lfZwerg > 0) lu16Zwerg = lfZwerg;
        }
        break;
      case 3: 
        {
          float lfZwerg = (((float)lu16Value_mV * 0.0564f) - 45.0f) / 1.129f;
          if (lfZwerg > 0) lu16Zwerg = lfZwerg;
        }
        break;
    }

    if (lu16Zwerg > 1023) lu16Zwerg = 1023;

    cTPS55288_RegisterMap::unVrefLsb.u8Raw = (u8)lu16Zwerg;
    cTPS55288_RegisterMap::unVrefMsb.u8Raw = (u8)(lu16Zwerg >> 8);

    vRequest(cTPS55288_RegisterMap::nSEQ07SetOutput);
  }

  void vSetCurrent_mA(u16 lu16ILim_mA)
  {
    u16 lILim;
    // 10mOhm at VISP-VISN
    // ILIM =   0 =>    0 mA
    // ILIM =   1 =>   50 mA
    // ILIM =   2 =>  100 mA
    // ILIM =   3 =>  150 mA
    // ILIM =   4 =>  200 mA
    // ILIM =   5 =>  250 mA
    // ILIM = 127 => 6350 mA

    lILim = ((lu16ILim_mA + 49) / 50);
    if (lILim > 127) lILim = 127;

    cTPS55288_RegisterMap::unIoutLimit.stBit.IoutLimitEn = 1;
    cTPS55288_RegisterMap::unIoutLimit.stBit.IoutLimit   = lILim;

    vRequest(cTPS55288_RegisterMap::nSEQ07SetOutput);
  }

  void vSetOutput(u16 lu16Value_mV, u16 lu16ILim_mA)
  {
    vSetCurrent_mA(lu16ILim_mA);
    vSetVoltage_mV(lu16Value_mV);
  }


  void vSetMode(cTPS55288_RegisterMap::tunMode lunMode)
  {
    cTPS55288_RegisterMap::unMode.u8Raw = lunMode.u8Raw;

    vRequest(cTPS55288_RegisterMap::nSEQ08WriteMode);
  }

  void vCmdReadStatus()
  {
    vRequest(cTPS55288_RegisterMap::nSEQ09ReadStatus);
  }

  cTPS55288_RegisterMap::tunStatus unGetStatus()
  {
    return cTPS55288_RegisterMap::unStatus;
  }
};

