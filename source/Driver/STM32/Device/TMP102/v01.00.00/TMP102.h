#ifndef __I2C_TMP102_H__
#define __I2C_TMP102_H__

#include "Typedef.h"
#include "cI2C.h"


//   TA = −25°C to 100°C
//   TA = −55°C to 125°C

/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

#define TMP102_DEFAULT_ADDRESS0      ((0x90) + (0 << 1)) // A0 to Gnd
#define TMP102_DEFAULT_ADDRESS1      ((0x90) + (1 << 1)) // A0 to V+
#define TMP102_DEFAULT_ADDRESS2      ((0x90) + (2 << 1)) // A0 to SDA
#define TMP102_DEFAULT_ADDRESS3      ((0x90) + (3 << 1)) // A0 to SCL
#define TMP102_I2C_ADDRESS_DEFAULT   TMP102_DEFAULT_ADDRESS0

class cTmp102 : public cComNode
{
  public:

  typedef enum
  {
    enRegTemp = 0,
    enRegConf = 1,
    enRegHyst = 2,
    enRegOverTempShutdown = 3,
  }tenReg;


  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdReadTemp,
    enCmdReadTemp2,
  }tenCmd;

  i16        mi16Temp_digit;

  cI2cMaster*   mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenCmd     menCmd;

  bool mbInit;
  bool mbReadTemp;


  cTmp102(cI2cMaster* lpcI2C, u8 lui8Adr)
  {
    UNUSED(lui8Adr);

    mI2C = lpcI2C;
    mAdr = lui8Adr;


    mpcMsgWrite.vMemAlloc(2, 0);
    mpcMsgRead.vMemAlloc(1, 2);

    menCmd = enCmdIdle;
    mStatus.IsInit = false;

    vCmdSetup();

    //Default auf 25 grad
    mi16Temp_digit = 25 << 8;

    mI2C->vAddSlave((cComNode*)this);
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);

    menCmd = enCmdIdle;
    //mStatus.IsInit = false;
  }

  void vComStart(cComNode::tenEvent lenEvent)
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
      else if (mbReadTemp)
      {
        mbReadTemp = false;
        menCmd = enCmdReadTemp;
        vComDone();
      }
    }

    mStatus.IsStartRequested = (mbInit |
                                mbReadTemp);
  }

  void vComDone()
  {
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdInit:
          mStatus.IsInit = true;
          menCmd = enCmdIdle;
        break;


      case enCmdReadTemp:
        menCmd = enCmdReadTemp2;

        mpcMsgRead.cTxData.mpu8Data[0] = enRegTemp;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
        break;
      case enCmdReadTemp2:
        mi16Temp_digit = ((mpcMsgRead.cRxData.mpu8Data[0] << 8)  + mpcMsgRead.cRxData.mpu8Data[1]);
        menCmd = enCmdIdle;
        break;

      default:
        break;
    }

    mStatus.IsStartRequested = (mbInit |
                                mbReadTemp);
  }

  void vTick10ms(void)
  {
  }


  void vCmdSetup()
  {
    mbInit = true;
    mStatus.IsStartRequested = 1;
  }

  void vCmdReadTemp()
  {
    if (menCmd == enCmdIdle)
    {
      mbReadTemp = true;
      mStatus.IsStartRequested = 1;
    }
  }

  // /128: Resolution in 0.5°C
  // /256: Resolution in 1.0°C
  i16 i16GetTemp()
  {
    return (mi16Temp_digit / 256);
  }

  float fGetTemp()
  {
    return (mi16Temp_digit / 256.0f);
  }
};



#endif // __I2C_TMP102_H__
