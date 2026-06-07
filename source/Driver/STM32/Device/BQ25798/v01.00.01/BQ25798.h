#ifndef __BQ25798_H__
#define __BQ25798_H__

#include "Typedef.h"
#include "BQ25798_RegMap.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define BQ25798_I2C_ADDRESS_CONF_0       (0x6B)
  #define BQ25798_I2C_ADDRESS_DEFAULT      BQ25798_I2C_ADDRESS_CONF_0
/*=========================================================================*/



class cBQ25798: public cComNode
{
  public:

  typedef struct
  {
    u8 mbInit         :1;
    u8 mbRead         :1;
    u8 mbWrite        :1;
    u8 mbKillHiz      :1;
  }tstStartFlags;

  typedef union
  {
    u8            u8FlagAll;
    tstStartFlags stFlags;
  }tunStartFlags;

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,
    enCmdInit4,
    enCmdWrite,
    enCmdWrite2,
    enCmdRead,
    enCmdRead2,

    enCmdKillHiz,
    enCmdKillHiz2,

  }tenBQ25798_Cmd;

  cBQ25798_RegisterMap mcRegisterMap;


  cI2cMaster* mI2C;
  u8          mPartInfo;

  u8          mRegIdx;
  u8          mRegIdxEnd;


  cComDatMsgDyn mpcMsgWrite;
  cComDatMsgDyn mpcMsgRead;

  tenBQ25798_Cmd  menCmd;

  tunStartFlags munStartFlags;

  cBQ25798(cI2cMaster* lpcI2C, uint8 lui8Adr)
  {

    lui8Adr <<= 1;

    mI2C   = lpcI2C;
    mAdr   = lui8Adr;

    mPartInfo = 0;

    mpcMsgRead.vMemAlloc(1, 24);
    mpcMsgWrite.vMemAlloc(3, 0);

    menCmd            = enCmdIdle;
    mStatus.IsInit    = false;
    munStartFlags.u8FlagAll = 0;

    i8Setup();

    mI2C->vAddSlave((cComNode*)this);
  }
  
  #if _WIN32
    ~cBQ25798()
    {
      mpcMsgRead.vMemFree();
      mpcMsgWrite.vMemFree();
    }
  #endif // _WIN32


  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);

    if (IsError())
    {
      mStatus.IsError = false;
      menCmd = enCmdIdle;
      mStatus.IsInit = false;
      i8Setup();
    }

    if (menCmd == enCmdIdle)
    {
      if (munStartFlags.stFlags.mbInit)
      {
        munStartFlags.stFlags.mbInit = false;
        menCmd = enCmdInit;
        vComDone();
      }
      else
      if (munStartFlags.stFlags.mbRead)
      {
        munStartFlags.stFlags.mbRead = false;
        menCmd = enCmdInit;
        vComDone();
      }
      else
      if (munStartFlags.stFlags.mbWrite)
      {
        munStartFlags.stFlags.mbWrite = false;
        menCmd = enCmdInit;
        vComDone();
      }
      else
      if (munStartFlags.stFlags.mbKillHiz)
      {
        munStartFlags.stFlags.mbKillHiz = false;
        menCmd = enCmdKillHiz;
        vComDone();
      }

      mStatus.IsStartRequested = (munStartFlags.u8FlagAll != 0);
    }
  }


  void vComDone() override
  {
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdInit:
        menCmd = enCmdInit2;

        if (bReadBlockStart() == False)
        {
          mStatus.IsInit = true;
          menCmd = enCmdIdle;
        }
        break;

      case enCmdInit2:
        if (bReadBlockProcess() == False)
        {
          mStatus.IsInit = true;
          //menCmd = enCmdIdle;

          // ADC einschalten
          menCmd = enCmdInit3;
          
          mcRegisterMap.REG2E_ADC_Ctrl.stBits.ADC_AVG = 1;
          mcRegisterMap.REG2E_ADC_Ctrl.stBits.ADC_SAMPLE = 1;
          mcRegisterMap.REG2E_ADC_Ctrl.stBits.ADC_EN = 1;
          mpcMsgWrite.cTxData.muiLen = 2;
          mpcMsgWrite.cTxData.mpu8Data[0] = (u8)cBQ25798_RegisterMap::nREG_ADC_CONTROL;
          mpcMsgWrite.cTxData.mpu8Data[1] = mcRegisterMap.REG2E_ADC_Ctrl.u8Byte;
          mpcMsgWrite.vStart();
          mI2C->vStartMsg(&mpcMsgWrite);
        }
        break;

      case enCmdInit3:
        // Temperaturüberwachung ausschalten
        menCmd = enCmdInit4;

        mcRegisterMap.REG18_NTC_Ctrl1.stBits.TS_IGNORE = 1;
        mpcMsgWrite.cTxData.muiLen = 2;
        mpcMsgWrite.cTxData.mpu8Data[0] = (u8)cBQ25798_RegisterMap::nREG_NTC_CTRL_1;
        mpcMsgWrite.cTxData.mpu8Data[1] = mcRegisterMap.REG18_NTC_Ctrl1.u8Byte;
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
        break;

      case enCmdInit4:
        menCmd = enCmdIdle;
        break;

      case enCmdRead:
        menCmd = enCmdRead2;

        if (bReadBlockStart() == False)
        {
          menCmd = enCmdIdle;
        }
        break;

      case enCmdRead2:
        if (bReadBlockProcess() == False)
        {
          menCmd = enCmdIdle;
        }
        break;

      case enCmdKillHiz:
        // Temperaturüberwachung ausschalten
        menCmd = enCmdInit4;

        mcRegisterMap.REG0F_CHG_Ctrl0.stBits.EN_HIZ = 0;
        mpcMsgWrite.cTxData.muiLen = 2;
        mpcMsgWrite.cTxData.mpu8Data[0] = (u8)cBQ25798_RegisterMap::nREG_CHARGER_CTRL_0;
        mpcMsgWrite.cTxData.mpu8Data[1] = mcRegisterMap.REG0F_CHG_Ctrl0.u8Byte;
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
        break;

      case enCmdKillHiz2:
        menCmd = enCmdIdle;
        break;

      default:
        break;
    }
  }

  bool bReadBlockStart()
  {
    if (mRegIdx <= mRegIdxEnd)
    {
      u8 lu8Len = mRegIdxEnd - mRegIdx + 1;

      if (lu8Len > 16) lu8Len = 16;

      mpcMsgRead.cRxData.muiLen = lu8Len;
      mpcMsgRead.cTxData.mpu8Data[0] = mRegIdx;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      return True;
    }
    return False;
  }

  bool bReadBlockProcess()
  {
    for (u8 lu8i = 0; lu8i < mpcMsgRead.cRxData.muiLen; lu8i++)
    {
      ((u8*)(&mcRegisterMap))[mRegIdx] = mpcMsgRead.cRxData.mpu8Data[lu8i];
      mRegIdx++;
    }

    return bReadBlockStart();
  }

  void vReadRegister(cBQ25798_RegisterMap::tenRegisterList lenReg)
  {
    mRegIdx = (u8)lenReg;
    mRegIdxEnd = mcRegisterMap.u8GetRegLen(lenReg) - 1;

    munStartFlags.stFlags.mbRead = True;
    mStatus.IsStartRequested = (munStartFlags.u8FlagAll != 0);
  }

  void vReadAll()
  {
    mRegIdx = 0;
    mRegIdxEnd = sizeof(mcRegisterMap) - 1;

    munStartFlags.stFlags.mbRead = True;
    mStatus.IsStartRequested = (munStartFlags.u8FlagAll != 0);
  }

  void vKillHiz()
  {
    munStartFlags.stFlags.mbKillHiz = True;
    mStatus.IsStartRequested = (munStartFlags.u8FlagAll != 0);
  }

  int8 i8Setup()
  {
    int li8Error = FRET_OK;

    mRegIdx = 0;
    mRegIdxEnd = sizeof(mcRegisterMap) - 1;
    
    munStartFlags.stFlags.mbInit = True;
    mStatus.IsStartRequested = (munStartFlags.u8FlagAll != 0);

    return li8Error;
  }
};

#endif // __BQ25798_H__
