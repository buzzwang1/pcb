#ifndef __I2C_LM75_H__
#define __I2C_LM75_H__

#include "Typedef.h"
#include "cI2C.h"


//   TA = −25°C to 100°C
//   TA = −55°C to 125°C

/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

#define LM75_DEFAULT_ADDRESS0      ((0x90) + (0 << 1))
#define LM75_DEFAULT_ADDRESS1      ((0x90) + (1 << 1))
#define LM75_DEFAULT_ADDRESS2      ((0x90) + (2 << 1))
#define LM75_DEFAULT_ADDRESS3      ((0x90) + (3 << 1))
#define LM75_DEFAULT_ADDRESS4      ((0x90) + (4 << 1))
#define LM75_DEFAULT_ADDRESS5      ((0x90) + (5 << 1))
#define LM75_DEFAULT_ADDRESS6      ((0x90) + (6 << 1))
#define LM75_DEFAULT_ADDRESS7      ((0x90) + (7 << 1))
#define LM75_I2C_ADDRESS_DEFAULT   LM75_DEFAULT_ADDRESS0

class cLm75 : public cComNode
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

  i16    mi16Temp_digit;
  u16    muRestart_ms;

  cComNodeMaster* mI2C;

  cComMsgS<u16, 8> mpcMsgWrite;
  cComMsgS<u16, 8> mpcMsgRead;

  tenCmd     menCmd;

  bool mbInit;
  bool mbReadTemp;


  cLm75(cComNodeMaster*  lpcI2C, u8 lui8Adr)
    : mpcMsgWrite(cComNode::enIsTx),
      mpcMsgRead(cComNode::enIsTxRx)
  {
    muRestart_ms = 0;
    mI2C = lpcI2C;
    mAdr = lui8Adr;
  }

  void vInit() override
  {
    vRestart();
  }

  void vRestart()
  {
    menCmd = enCmdIdle;
    mStatus.IsInit = false;

    vCmdSetup();
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError);
    UNUSED(lenState);

    muRestart_ms = 100;
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
        {
          menCmd = enCmdReadTemp2;

          u8 lu8Data = enRegTemp;
          mpcMsgRead.vPrepareTxRx(1, 2, (u8*)&lu8Data);
          mI2C->vStartMsg(&mpcMsgRead);
        }
        break;
      case enCmdReadTemp2:
        mi16Temp_digit = ((mpcMsgRead.mpu8Data[0] << 8)  + mpcMsgRead.mpu8Data[1]);
        menCmd = enCmdIdle;
        break;

      default:
        break;
    }

    mStatus.IsStartRequested = (mbInit |
                                mbReadTemp);
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
  };


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
    return (mi16Temp_digit / 128);
  }
};

#endif // __I2C_LM75_H__
