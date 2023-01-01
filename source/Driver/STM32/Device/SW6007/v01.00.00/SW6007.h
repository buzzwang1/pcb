#ifndef __SW6007_H__
#define __SW6007_H__

#include "Typedef.h"
#include "cI2C.h"


/* APDS-SW6007 I2C address */
#define nSW6007_I2C_ADDR       (0x3C << 1)




class cSW6007: public cComNode
{
  public:

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,

    enCmdSetId,
    enCmdSetId2,

    enCmdSet0x32PlugInOutDetectConfig,
    enCmdSet0x32PlugInOutDetectConfig2,

  }tenAPDS9960_Cmd;


  cI2cMaster*       mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;
  cComDatMsg mpcMsgReadData;

  tenAPDS9960_Cmd    menCmd;
  u16                mIdx;

  bool mbInit;
  bool mbId;
  bool mbSet0x32PlugInOutDetectConfig;

  u8   mu8Mem[256];

  cSW6007(cI2cMaster* lpcI2C, uint8 lui8Adr)
  {
    mI2C   = lpcI2C;
    mAdr   = lui8Adr;

    mpcMsgRead.vMemAlloc(1, 10);
    mpcMsgReadData.vMemAlloc(1, 0);
    mpcMsgWrite.vMemAlloc(2, 0);

    menCmd         = enCmdIdle;
    mStatus.IsInit = false;

    i8Setup();
    
    mbId = false;
    mbSet0x32PlugInOutDetectConfig = false;

    i8Set0x32PlugInOutDetectConfig(0x50);

    vSetStartRequest();

    mI2C->vAddSlave((cComNode*)this);
  }

  ~cSW6007()
  {
    mpcMsgRead.vMemFree();
    mpcMsgWrite.vMemFree();
  }

  inline void vSetStartRequest()
  {
    mStatus.IsStartRequested = (uint8)(mbInit |
                                       mbId | 
                                       mbSet0x32PlugInOutDetectConfig);
  }


  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError);
    UNUSED(lenState);
    mStatus.IsStartRequested = false;
    menCmd                   = enCmdIdle;
    mStatus.IsInit           = false;
    /*mAdr += 2;
    if (mAdr > 255) mAdr = 2;*/
    i8Setup();
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
      else if (mbId)
      {
        mbId = false;
        menCmd = enCmdSetId;
        vComDone();
      }
      else if (mbSet0x32PlugInOutDetectConfig)
      {
        mbSet0x32PlugInOutDetectConfig = false;
        menCmd = enCmdSet0x32PlugInOutDetectConfig;
        vComDone();
      }
    }

    vSetStartRequest();
  }

  void vComDone() override
  {
    mpcMsgRead.cTxData.muiLen = 1;
    mpcMsgRead.cRxData.muiLen = 1;
    mpcMsgWrite.cTxData.muiLen = 2;

    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdInit:
        mIdx    = 0;
        menCmd = enCmdInit2;
        //Read ID
        mpcMsgRead.cTxData.mpu8Data[0] = mIdx;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
        break;
      case enCmdInit2:
        if  (mIdx < 255)
        {
          mu8Mem[mIdx] = mpcMsgRead.cRxData.mpu8Data[0];
          mIdx++;
          mpcMsgRead.cTxData.mpu8Data[0] = mIdx;
          mpcMsgRead.vStart();
          mI2C->vStartMsg(&mpcMsgRead);
        }
        else
        {
          mStatus.IsInit = true;
          menCmd    = enCmdIdle;
        }
        break;
      case enCmdInit3:
        break;


      case enCmdSetId: // REG 0x26: Version I
        menCmd  = enCmdSetId2;
        //Read ID
        mpcMsgRead.cTxData.mpu8Data[0] = 0x26;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
        break;
      case enCmdSetId2:
        menCmd     = enCmdIdle;
        break;

      case enCmdSet0x32PlugInOutDetectConfig:
        menCmd  = enCmdSet0x32PlugInOutDetectConfig2;
        //Read ID
        mpcMsgWrite.cTxData.mpu8Data[0] = 0x32;
        mpcMsgWrite.cTxData.mpu8Data[1] = 0x50;
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
        break;
      case enCmdSet0x32PlugInOutDetectConfig2:
        menCmd     = enCmdIdle;
        break;

      default:
        break;
    }
    vSetStartRequest();
  }





  int8 i8Setup()
  {
    int li8Error = FRET_OK;

    mbInit          = true;
    mStatus.IsStartRequested = 1;

    return li8Error;
  }

  int8 i8ReadId()
  {
    mbId           = true;
    mStatus.IsStartRequested = 1;

    return FRET_OK;
  }

  int8 i8Set0x32PlugInOutDetectConfig(u8 lu8Value)
  {
    mbSet0x32PlugInOutDetectConfig = true;
    mu8Mem[0x32] = lu8Value;
    mStatus.IsStartRequested       = 1;

    return FRET_OK;
  }
};

#endif // __SW6007_H__
