#ifndef __BQ25798_H__
#define __BQ25798_H__

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "BQ25798_RegMap.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define BQ25798_I2C_ADDRESS_CONF_0       (0x6B)
  #define BQ25798_I2C_ADDRESS_DEFAULT      BQ25798_I2C_ADDRESS_CONF_0
/*=========================================================================*/



class cBQ25798: public cComSeqHdlI2c<cBQ25798_RegisterMap>
{
  public:

  cBQ25798(cI2cMaster* lpcI2C, u8 lu8Adr)
    : cComSeqHdlI2c(lpcI2C, lu8Adr << 1)
  {
  }

  float fGetSetting(u8 lu8Chn, u8 lu8Value)
  {
    // nREG00_MIN_SYS_VOLTAGE    = 0, // Minimal System Voltage
    // nREG01_CHG_VOLTAGE_LIMIT,   // Charge Voltage Limit (2 Bytes)
    // nREG03_CHG_CURRENT_LIMIT,   // Charge Current Limit (2 Bytes)
    // nREG05_INPUT_VOLTAGE_LIMIT, // Input Voltage Limit
    // nREG06_INPUT_CURRENT_LIMIT, // Input Current Limit (2 Bytes)
    // nREG08_PRECHARGE_CTRL,      // Precharge Control
    // nREG09_TERM_CTRL,           // Termination Control
    // nREG0A_RECHARGE_CTRL,       // Re-charge Control
    // nREG0B_VOTG_REGULATION,     // VOTG regulation (2 Bytes)
    // nREG0D_IOTG_REGULATION,     // IOTG regulation

    switch ((cBQ25798_RegisterMap::tenRegIndex)lu8Chn)
    {
      case cBQ25798_RegisterMap::nREG00_MIN_SYS_VOLTAGE:
        return (float)((u16)cBQ25798_RegisterMap::tunREG00_Minimal_System_Voltage.stBits.VSYS_MIN) * 250 + 2500;
        break;
      case cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG01_Charge_Voltage_Limit.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG01_Charge_Voltage_Limit.stBits.Lb) * 10;
        break;
      case cBQ25798_RegisterMap::nREG03_CHG_CURRENT_LIMIT:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG03_Charge_Current_Limit.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG03_Charge_Current_Limit.stBits.Lb) * 10;
        break;

      case cBQ25798_RegisterMap::nREG05_INPUT_VOLTAGE_LIMIT:
        return (float)((u16)cBQ25798_RegisterMap::tunREG05_Input_Voltage_Limit.stBits.VINDPM) * 100;
        break;
      case cBQ25798_RegisterMap::nREG06_INPUT_CURRENT_LIMIT:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG06_Input_Current_Limit.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG06_Input_Current_Limit.stBits.Lb) * 10;
        break;

      case cBQ25798_RegisterMap::nREG08_PRECHARGE_CTRL:
        switch (lu8Value)
        {
          case 0:
            switch (cBQ25798_RegisterMap::tunREG08_Precharge_Control.stBits.VBAT_LOWV)
            {
              case 0:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT,0) * 0.15f;
                break;
              case 1:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 0.622f;
                break;
              case 2:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 0.667f;
                break;
              default:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 0.714f;
            }            
            break;
          default:
            return (float)((u16)cBQ25798_RegisterMap::tunREG08_Precharge_Control.stBits.IPRECHG) * 40;
        }        
        break;

      case cBQ25798_RegisterMap::nREG09_TERM_CTRL:
        return (float)((u16)cBQ25798_RegisterMap::tunREG09_Termination_Control.stBits.ITERM) * 40;
        break;

      case cBQ25798_RegisterMap::nREG0A_RECHARGE_CTRL:
        switch (lu8Value)
        {
          case 0:
            return (float)((u16)cBQ25798_RegisterMap::tunREG0A_Re_charge_Control.stBits.CELL) + 1;
            break;
          case 1:
            switch (cBQ25798_RegisterMap::tunREG0A_Re_charge_Control.stBits.TRECHG)
            {
              case 0:
                return 64.0f;
                break;
              case 1:
                return 256.0f;
                break;
              case 2:
                return 1024.0f;
                break;
              default:
                return 2048.0f;
            }
            break;
          default:
            return (float)((u16)cBQ25798_RegisterMap::tunREG0A_Re_charge_Control.stBits.VRECHG) * 50 + 50;
        }

      case cBQ25798_RegisterMap::nREG10_CHARGER_CTRL_1:
        switch (lu8Value)
        {
          case 0:
            switch (cBQ25798_RegisterMap::tunREG10_Charger_Control_1.stBits.VBUS_BACKUP)
            {
              case 0:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT,0) * 0.4f;
                break;
              case 1:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 0.6f;
                break;
              case 2:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 0.8f;
                break;
              default:
                return (float)fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0) * 1.0f;
            }
            break;
          case 1:
            switch (cBQ25798_RegisterMap::tunREG10_Charger_Control_1.stBits.VAC_OVP)
            {
              case 0:
                return (float)26000;
                break;
              case 1:
                return (float)22000;
                break;
              case 2:
                return (float)12000;
                break;
              default:
                return (float)7000;
            }
            break;
          case 3:
            switch (cBQ25798_RegisterMap::tunREG10_Charger_Control_1.stBits.WD_MODE)
            {
              case 0: return (float)0;
              case 1: return (float)0.5f;
              case 2: return (float)1;
              case 3: return (float)2;
              case 4: return (float)20;
              case 5: return (float)40;
              case 6: return (float)80;
              case 7: return (float)160;
            }
            break;
          default:
            return (float)((u16)cBQ25798_RegisterMap::tunREG10_Charger_Control_1.stBits.WD_RST);
        }

      case cBQ25798_RegisterMap::nREG15_MPPT_CTRL:
        switch (lu8Value)
        {
          case 0:
            switch (cBQ25798_RegisterMap::tunREG15_MPPT_Control.stBits.VOC_PCT)
            {
              case 0: return (float)0.5625f;
              case 1: return (float)0.625f;
              case 2: return (float)0.6875f;
              case 3: return (float)0.75f;
              case 4: return (float)0.8125f;
              case 5: return (float)0.875f;
              case 6: return (float)0.9375f;
              case 7: return (float)1;
            }
            break;
          case 1:
            switch (cBQ25798_RegisterMap::tunREG15_MPPT_Control.stBits.VOC_DLY)
            {
              case 0: return (float)50;
              case 1: return (float)300;
              case 2: return (float)2;
              case 3: return (float)5;
            }
            break;
          case 2:
            switch (cBQ25798_RegisterMap::tunREG15_MPPT_Control.stBits.VOC_RATE)
            {
              case 0: return (float)0.5f;
              case 1: return (float)2;
              case 2: return (float)10;
              case 3: return (float)30;
            }
            break;
          default:
            return (float)((u16)cBQ25798_RegisterMap::tunREG15_MPPT_Control.stBits.EN_MPPT);
        }
      default:
        return (float)0.0f;
    }    
  }

  float fGetAdc(u8 lu8Chn)
  {
    //  cBQ25798_RegisterMap::nREG43_DPLUS_ADC
    //  cBQ25798_RegisterMap::nREG45_DMINUS_ADC

    switch ((cBQ25798_RegisterMap::tenRegIndex)lu8Chn)
    {
      case cBQ25798_RegisterMap::nREG31_IBUS_ADC:
        return (float)((i16)(cBQ25798_RegisterMap::tunREG31_IBUS_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG31_IBUS_ADC.stBits.Lb);
        break;
      case cBQ25798_RegisterMap::nREG35_VBUS_ADC: 
        return (float)((u16)(cBQ25798_RegisterMap::tunREG35_VBUS_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG35_VBUS_ADC.stBits.Lb);
        break;

      case cBQ25798_RegisterMap::nREG33_IBAT_ADC:
        return (float)((i16)(cBQ25798_RegisterMap::tunREG33_IBAT_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG33_IBAT_ADC.stBits.Lb);
        break;
      case cBQ25798_RegisterMap::nREG3B_VBAT_ADC:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG3B_VBAT_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG3B_VBAT_ADC.stBits.Lb);
        break;

      case cBQ25798_RegisterMap::nREG37_VAC1_ADC:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG37_VAC1_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG37_VAC1_ADC.stBits.Lb);
        break;
      case cBQ25798_RegisterMap::nREG39_VAC2_ADC:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG39_VAC2_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG39_VAC2_ADC.stBits.Lb);
        break;

      case cBQ25798_RegisterMap::nREG3D_VSYS_ADC:
        return (float)((u16)(cBQ25798_RegisterMap::tunREG3D_VSYS_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG3D_VSYS_ADC.stBits.Lb);
        break;

      case cBQ25798_RegisterMap::nREG3F_TS_ADC:
        return (float)(((u16)(cBQ25798_RegisterMap::tunREG3F_TS_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG3F_TS_ADC.stBits.Lb)) * 0.0976563f;
        break;
      case cBQ25798_RegisterMap::nREG41_TDIE_ADC:
        return (float)(((i16)(cBQ25798_RegisterMap::tunREG41_TDIE_ADC.stBits.Hb << 8) + cBQ25798_RegisterMap::tunREG41_TDIE_ADC.stBits.Lb)) * 0.5f;
        break;
      default:
        return (float)0.0f;
    }    
  }


};

#endif // __BQ25798_H__
