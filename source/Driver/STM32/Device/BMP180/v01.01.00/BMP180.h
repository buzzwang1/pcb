#ifndef __BMP180_H__
#define __BMP180_H__

#include "Typedef.h"
#include "cI2C.h"


/* BMP180 I2C address */
#define nBMP180_I2C_ADDR       (0x77)


class cBMP180: public cComNode
{
  public:
    /* resolutions */
    typedef enum : u8
    {
      BMP180_ULTRALOWPOWER = 0x00, //low power             mode, oss0
      BMP180_STANDARD      = 0x01, //standard              mode, oss1
      BMP180_HIGHRES       = 0x02, //high resolution       mode, oss2
      BMP180_ULTRAHIGHRES  = 0x03  //ultra high resolution mode, oss3
    } tenResolution;

    /* calibration registers */
    typedef enum : u8
    {
      BMP180_CAL_AC1_REG = 0xAA,  //ac1 pressure    computation
      BMP180_CAL_AC2_REG = 0xAC,  //ac2 pressure    computation
      BMP180_CAL_AC3_REG = 0xAE,  //ac3 pressure    computation
      BMP180_CAL_AC4_REG = 0xB0,  //ac4 pressure    computation
      BMP180_CAL_AC5_REG = 0xB2,  //ac5 temperature computation
      BMP180_CAL_AC6_REG = 0xB4,  //ac6 temperature computation
      BMP180_CAL_B1_REG  = 0xB6,  //b1  pressure    computation
      BMP180_CAL_B2_REG  = 0xB8,  //b2  pressure    computation
      BMP180_CAL_MB_REG  = 0xBA,  //mb
      BMP180_CAL_MC_REG  = 0xBC,  //mc  temperature computation
      BMP180_CAL_MD_REG  = 0xBE   //md  temperature computation
    } tenCalReg;

    typedef enum : u8
    {
      BMP180_GET_ID_REG           = 0xD0,   //device id register
      BMP180_GET_VERSION_REG      = 0xD1,   //device version register

      BMP180_SOFT_RESET_REG       = 0xE0,   //soft reset register
      BMP180_SOFT_RESET_CTRL      = 0xB6,   //soft reset control

      BMP180_START_MEASURMENT_REG = 0xF4,   //start measurment  register
      BMP180_READ_ADC_MSB_REG     = 0xF6,   //read adc msb  register
      BMP180_READ_ADC_LSB_REG     = 0xF7,   //read adc lsb  register
      BMP180_READ_ADC_XLSB_REG    = 0xF8,   //read adc xlsb register
    } tenReg;

    typedef enum : u8
    {
      /* BMP180_START_MEASURMENT_REG controls */
      BMP180_GET_TEMPERATURE_CTRL   = 0x2E,   //get temperature control 4.5ms
      BMP180_GET_PRESSURE_OSS0_CTRL = 0x34,   //get pressure oversampling 1 time/oss0 control 4.5ms
      BMP180_GET_PRESSURE_OSS1_CTRL = 0x74,   //get pressure oversampling 2 time/oss1 control 7.5ms
      BMP180_GET_PRESSURE_OSS2_CTRL = 0xB4,   //get pressure oversampling 4 time/oss2 control 13.5ms
      BMP180_GET_PRESSURE_OSS3_CTRL = 0xF4,   //get pressure oversampling 8 time/oss3 control 25.5ms

      // BMP180_GET_TEMPERATURE_CTRL   = 0x2E => 00.1.01110 => 14
      // BMP180_GET_PRESSURE_OSS0_CTRL = 0x34 => 00.1.10100 => 20
      // BMP180_GET_PRESSURE_OSS1_CTRL = 0x74 => 01.1.10100 => 20
      // BMP180_GET_PRESSURE_OSS2_CTRL = 0xB4 => 10.1.10100 => 20
      // BMP180_GET_PRESSURE_OSS3_CTRL = 0xF4 => 11.1.10100 => 20

      /* misc */
      BMP180_ADDRESS                = 0x77,   //i2c address
      BMP180_CHIP_ID                = 0x55,   //id number

      BMP180_ERROR                  = 255,    //returns 255, if communication error is occurred
    } tenDiv;

    /* to store calibration coefficients */
    typedef struct
    {
      i16  bmpAC1;
      i16  bmpAC2;
      i16  bmpAC3;
      u16  bmpAC4;
      u16  bmpAC5;
      u16  bmpAC6;

      i16  bmpB1;
      i16  bmpB2;

      i16  bmpMB;
      i16  bmpMC;
      i16  bmpMD;
    } tstBMP180_CalCoeff;

    typedef struct
    {
      u8   cntrl : 5;
      u8   sco   : 1;
      u8   oss   : 2;
    } tstBMP180_Cntrl;

    typedef union
    {
      u8              Byte;
      tstBMP180_Cntrl Bits;
    } tunBMP180_Cntrl;


  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,
    enCmdInit4,

    enCmdSetControl,
    enCmdSetControl2,

    enCmdRead,
    enCmdRead2,
    enCmdRead3,
  }tenBMP180_Cmd;

  cComNodeMaster* mI2C;

  cComMsgS<u16, 4> mpcMsgWrite;
  cComMsgS<u16, 8> mpcMsgRead;


  u16*                 mpCalCoeff;
  tstBMP180_CalCoeff   mstCalCoeff;

  float mfTemp;
  u32   muPressure;

  u32  mu32ReadValue;
  u32  mu32PressureValueRaw;
  i16  mi16TempValueRaw;

  u16    muRestart_ms;

  tenBMP180_Cmd    menCmd;
  tunBMP180_Cntrl  menCntrl;

  u8               mIdx;
  u8               mId;

  bool mbReadBusy;
  u8   mu8ReadDelay_ms;

  bool mbTempFirstValue;
  bool mbPressureFirstValue;

  bool mbInit;
  bool mbSetControl;
  bool mbRead;

  cBMP180(cComNodeMaster* lpcI2C, uint8 lui8Adr)
    : mpcMsgWrite(cComNode::enIsTx),
      mpcMsgRead(cComNode::enIsTxRx)
  {
    mI2C   = lpcI2C;
    mAdr   = lui8Adr;
  }

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
    vRestart();
  }

  void vRestart()
  {
    menCmd = enCmdIdle;
    mStatus.IsInit = false;
    mStatus.IsError = false;
    i8Setup();
  }

  int8 i8Setup()
  {
    int li8Error = FRET_OK;

    mbInit           = true;
    mControl.StartRequest = 1;

    mbTempFirstValue = False;
    mbPressureFirstValue = False;
    mbReadBusy = False;

    return li8Error;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError);
    UNUSED(lenState);
    cComNode::vComError(lenError, lenState);

    mStatus.IsInit = false;
    menCmd = enCmdIdle;

    muRestart_ms = 100;
  }


  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
    if (menCmd == enCmdIdle)
    {
      if (mbInit)
      {
        mbInit = false;
        menCmd = enCmdInit;
        vComDone();
      }
      else if (mbRead)
      {
        mbRead = false;
        menCmd = enCmdRead;
        vComDone();
      }
      else if (mbSetControl)
      {
        mbSetControl = false;
        menCmd = enCmdSetControl;
        vComDone();
      }
    }
  }


  void vComDone() override
  {
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdInit:
        {
          mIdx    = 0;
          menCmd = enCmdInit2;

          //Read ID
          u8 lu8Data = tenReg::BMP180_GET_ID_REG;
          mpcMsgRead.vPrepareTxRx(1, 1, (u8*)&lu8Data);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        break;
      case enCmdInit2:
        // when ID ok then read control
        if (mpcMsgRead.mpu8Data[0] == tenDiv::BMP180_CHIP_ID)
        {
          mId = mpcMsgRead.mpu8Data[0];
          menCmd = enCmdInit3;

          //Read Control
          u8 lu8Data = tenReg::BMP180_START_MEASURMENT_REG;
          mpcMsgRead.vPrepareTxRx(1, 1, (u8*)&lu8Data);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        else
        {
          vComError(cComNode::tenError::enErUnknown, cComNode::tenState::enStData);
        }
        break;
      case enCmdInit3:
        menCntrl.Byte = mpcMsgRead.mpu8Data[0];
        menCmd = enCmdInit4;

        //Read Calibration
        mpCalCoeff = (u16*)&mstCalCoeff;

        mIdx = tenCalReg::BMP180_CAL_AC1_REG;
        mpcMsgRead.vPrepareTxRx(1, 2, (u8*)&mIdx);
        mI2C->vStartMsg(&mpcMsgRead);
        break;

      case enCmdInit4:
        *mpCalCoeff++ = (mpcMsgRead.mpu8Data[0] << 8) + mpcMsgRead.mpu8Data[1];

        if (mIdx == tenCalReg::BMP180_CAL_MD_REG)
        {
          menCmd = enCmdIdle;
          mStatus.IsInit = True;
        }
        else
        {
          mIdx += 2;
          mpcMsgRead.vPrepareTxRx(1, 2, (u8*)&mIdx);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        break;


      case enCmdSetControl:
        menCmd = enCmdSetControl2;
        mpcMsgWrite.mpu8Data[0] = tenReg::BMP180_START_MEASURMENT_REG;
        mpcMsgWrite.mpu8Data[1] = this->menCntrl.Byte;
        mpcMsgWrite.muLen = 2;
        mI2C->vStartMsg(&mpcMsgWrite);
        break;
      case enCmdSetControl2:
        menCmd = enCmdIdle;
        switch (menCntrl.Bits.oss)
        {
          case 0: mu8ReadDelay_ms = 10; break;
          case 1: mu8ReadDelay_ms = 20; break;
          case 2: mu8ReadDelay_ms = 30; break;
          case 3: mu8ReadDelay_ms = 40; break;
        }
        break;

      case enCmdRead:
        {
          menCmd = enCmdRead2;

          mu32ReadValue = 0;
          u8 lu8Data = tenReg::BMP180_READ_ADC_MSB_REG;
          mpcMsgRead.vPrepareTxRx(1, 2, (u8*)&lu8Data);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        break;

      case enCmdRead2:
        mu32ReadValue = (mpcMsgRead.mpu8Data[0] << 8) + mpcMsgRead.mpu8Data[1];

        // Pressure value has 3 Bytes to read
        if (menCntrl.Bits.cntrl == 20)
        {
          menCmd = enCmdRead3;
          u8 lu8Data = tenReg::BMP180_READ_ADC_XLSB_REG;
          mpcMsgRead.vPrepareTxRx(1, 1, (u8*)&lu8Data);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        else
        {
          mbTempFirstValue = True;

          // Nach der Temp noch den Pressure lesen
          mbSetControl = True;
          menCntrl.Byte = tenDiv::BMP180_GET_PRESSURE_OSS0_CTRL;
          mu8ReadDelay_ms = 0;
          mControl.StartRequest = 1;
          mi16TempValueRaw = (i16)mu32ReadValue;

          menCmd = enCmdIdle;
        }
        break;
      case enCmdRead3:
        mu32ReadValue = (mu32ReadValue << 8) + mpcMsgRead.mpu8Data[0];
        mu32ReadValue >>= (8 - menCntrl.Bits.oss);
        mu32PressureValueRaw = mu32ReadValue;
        mbPressureFirstValue = True;
        mbReadBusy = False;
        menCmd = enCmdIdle;
        break;

      default:
        break;
    }
  }


  void vTick(u16 lu16Time_ms) override
  {
    if (muRestart_ms)
    {
      if (muRestart_ms > lu16Time_ms)
      {
        muRestart_ms -= lu16Time_ms;
      }
      else
      {
        muRestart_ms = 0;
        vRestart();
      }
    }

    if (mu8ReadDelay_ms)
    {
      if (mu8ReadDelay_ms > lu16Time_ms)
      {
        mu8ReadDelay_ms -= lu16Time_ms;
      }
      else
      {
        mu8ReadDelay_ms = 0;
        mbRead = True;
        mControl.StartRequest = 1;
      }
    }
  };

  int8 i8Start()
  {

    if (mbReadBusy) return FRET_ERR_GENERAL;

    mbSetControl = True;
    mbReadBusy = True;
    menCntrl.Byte = tenDiv::BMP180_GET_TEMPERATURE_CTRL;
    mu8ReadDelay_ms = 0;
    mControl.StartRequest = 1;
    return FRET_OK;
  }

  i32 i32ComputeB5(i32 li32UT)
  {
    i32 X1 = ((li32UT - (i32)mstCalCoeff.bmpAC6) * (i32)mstCalCoeff.bmpAC5) >> 15;
    i32 X2 = ((i32)mstCalCoeff.bmpMC << 11) / (X1 + (i32)mstCalCoeff.bmpMD);

    return X1 + X2;
  }

  void vCalc(void)
  {
    i32 UT       = 0;
    i32 UP       = 0;
    i32 B3       = 0;
    i32 B5       = 0;
    i32 B6       = 0;
    i32 X1       = 0;
    i32 X2       = 0;
    i32 X3       = 0;
    i32 pressure = 0;
    u32 B4       = 0;
    u32 B7       = 0;

    if (!mbTempFirstValue) return;

    UT = mi16TempValueRaw;
    B5 = i32ComputeB5(UT);

    mfTemp = (float)((B5 + 8) >> 4) / 10.0f;

    if (!mbPressureFirstValue) return;

    UP = mu32PressureValueRaw;


    /* pressure calculation */
    B6 = B5 - 4000;
    X1 = ((i32)mstCalCoeff.bmpB2 * ((B6 * B6) >> 12)) >> 11;
    X2 = ((i32)mstCalCoeff.bmpAC2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((i32)mstCalCoeff.bmpAC1 * 4 + X3) << menCntrl.Bits.oss) + 2) / 4;

    X1 = ((i32)mstCalCoeff.bmpAC3 * B6) >> 13;
    X2 = ((i32)mstCalCoeff.bmpB1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = ((u32)mstCalCoeff.bmpAC4 * (X3 + 32768L)) >> 15;
    B7 = (UP - B3) * (50000UL >> menCntrl.Bits.oss);

    if (B4 == 0) return;                                     //safety check, avoiding division by zero

    if   (B7 < 0x80000000) pressure = (B7 * 2) / B4;
    else                   pressure = (B7 / B4) * 2;

    X1 = (pressure >> 8) * (pressure >> 8);
    X1 = (X1 * 3038L) >> 16;
    X2 = (-7357L * pressure) >> 16;

    muPressure = pressure + ((X1 + X2 + 3791L) >> 4);
  }

  float fGetTemperature()
  {
    return mfTemp;
  }

  u32 u32GetPressure()
  {
    return muPressure;
  }
};

#endif // __BMP180_H__
