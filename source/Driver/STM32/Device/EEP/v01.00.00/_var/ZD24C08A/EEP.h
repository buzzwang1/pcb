#ifndef __I2C_EEP_H__
#define __I2C_EEP_H__

#include "Typedef.h"
#include "ComDat.h"
#include "cI2c.h"


class cI2cEep : public cComNode
{
  public:
  // Measurements @400khz:
  //   Default Adresse: 0xA0 (already left shifted, 8Bit value)
  //   Time required after VCC is stable before the device can accept commands. 100 μs
  // 
  //   Read 16 Byte => 0,4556ms
  //   Read 1024 Bytes (64*16Byte) => 29ms
  //
  //   Write 1024 Bytes (64*16Byte) => 1x Page every 10ms => 640ms
  //

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdReadByte,
    enCmdReadByte2,
    enCmdReadBlock,
    enCmdReadBlock2,
    enCmdWriteByte,
    enCmdWriteByte2,
    enCmdWriteAll,
    enCmdWriteAll2,
    enCmdWriteAll3,
  }tenCmd;

  u8         mu8RamMirrow[1024];
  u16        mu16Adress;
  u16        mu16AdressEnd;  // End Adresse für Block Read
  u8         mu8WriteWaitTime;


  cI2cMaster*   mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenCmd     menCmd;
  u8         mIdx;

  typedef struct
  {
    u8 mbInit             :1;
    u8 mbReadByte         :1;
    u8 mbReadBlock        :1;
    u8 mbWriteByte        :1;
    u8 mbWriteAll         :1;
    u8 mbWriteAllContinue :1;
  }tstStartFlags;

  typedef union
  { 
    u8            u8FlagAll;
    tstStartFlags stFlags;
  }tunStartFlags;

  tunStartFlags munStartFlags;

  cI2cEep(cI2cMaster* lpcI2C, u8 lui8Adr, u16 lu16AdrEnd = 1023)
  {
    UNUSED(lui8Adr);

    mI2C = lpcI2C;
    mu8WriteWaitTime = 0;

    mpcMsgWrite.vMemAlloc(1 + 16, 0); // 1 Adress Byte + 16 Byte Page
    mpcMsgRead.vMemAlloc(1, 16); // 16 Byte Page

    menCmd = enCmdIdle;
    munStartFlags.u8FlagAll = 0;
    mStatus.IsInit = false;

    mu16AdressEnd = lu16AdrEnd;

    vCmdSetup();

    mI2C->vAddSlave((cComNode*)this);
  }

  u32 u32GetSize() {return sizeof(mu8RamMirrow);}

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);

    menCmd = enCmdIdle;
    mStatus.IsInit = false;
  }

  void vComStart(cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEvent);
    if (menCmd == enCmdIdle)
    {
      if (munStartFlags.stFlags.mbInit)
      {
        munStartFlags.stFlags.mbInit = false;
        menCmd = enCmdInit;
        vComDone();
      }
      else if (munStartFlags.stFlags.mbWriteByte)
      {
        munStartFlags.stFlags.mbWriteByte = false;
        menCmd = enCmdWriteByte;
        vComDone();
      }
      else if (munStartFlags.stFlags.mbReadByte)
      {
        munStartFlags.stFlags.mbReadByte = false;
        menCmd = enCmdReadByte;
        vComDone();
      }
      else if (munStartFlags.stFlags.mbWriteAll)
      {
        munStartFlags.stFlags.mbWriteAll = false;
        menCmd = enCmdWriteAll;
        vComDone();
      }
      else if (munStartFlags.stFlags.mbReadBlock)
      {
        munStartFlags.stFlags.mbWriteAll = false;
        menCmd = enCmdReadBlock;
        vComDone();
      }
    }
    else
    {
      if (munStartFlags.stFlags.mbWriteAllContinue)
      {
        munStartFlags.stFlags.mbWriteAllContinue = False;
        menCmd = enCmdWriteAll3;
        vComDone();
      }
    }

    mStatus.IsStartRequested = (munStartFlags.stFlags.mbInit |
                                munStartFlags.stFlags.mbReadByte |
                                munStartFlags.stFlags.mbWriteByte |
                                munStartFlags.stFlags.mbWriteAll);
  }

  void vAdrInc()
  {
    // 16 - byte Page size

    //  The internal data word address counter maintains the last address accessed during the last read or write
    //  operation, incremented by one.This address stays valid between operations as long as the VCC is maintained
    //  to the part.The address “roll over” during read is from the last byte of the last page to the first byte of the first
    //  page of the memory.

    u16 mu16AdressLow  = (mu16Adress + 1) & 0x000F;
    u16 mu16AdressHigh = mu16Adress & 0xFFF0;

    mu16Adress = mu16AdressHigh | mu16AdressLow;
  }

  void vComDone()
  {
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdInit:
        // EEPROM komplett einlesen
        menCmd = enCmdInit2;

        if (mu16AdressEnd == 0)
        {
          mStatus.IsInit = true;
          menCmd = enCmdIdle;
        }
        else
        {
          vReadBlockStart();
        }
        break;
      case enCmdInit2:
        vReadBlockProcess();

        if (mu16Adress > mu16AdressEnd)
        {
          mStatus.IsInit = true;
          menCmd = enCmdIdle;
        }
        break;
          
      case enCmdReadBlock:
        // EEPROM komplett einlesen
        menCmd = enCmdReadBlock2;

        vReadBlockStart();
        if (mu16Adress > mu16AdressEnd)
        {
          menCmd = enCmdIdle;
        }
        break;
      case enCmdReadBlock2:
        vReadBlockProcess();

        if (mu16Adress > mu16AdressEnd)
        {
          menCmd = enCmdIdle;
        }
        break;

      case enCmdWriteByte:
        menCmd = enCmdWriteByte2;

        // 10bit Adress 1024 * 8 = 8k
        // upper 2bit are locate in bit 2 and 1 in Byte 1
        // lower 8bit are located in Byte2

        // 1010.DAA0 AAAA.AAAA BBBB.BBBB
        //   D: HW Device Adress: 2 Eeprom can be adressed
        //   A: Memeory Adress
        //   B: Byte to be written
        mpcMsgWrite.cTxData.muiLen = 2;
        mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
        mpcMsgWrite.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
        mpcMsgWrite.cTxData.mpu8Data[1] = mu8RamMirrow[mu16Adress];
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
        break;
      case enCmdWriteByte2:
        menCmd = enCmdIdle;
        break;

      case enCmdReadByte:
        menCmd = enCmdReadByte2;

        // Write Adress + 1Byte einlesen
        mpcMsgRead.cTxData.muiLen = 1;
        mpcMsgRead.cRxData.muiLen = 1;

        mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
        mpcMsgRead.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
        break;
      case enCmdReadByte2:
        mu8RamMirrow[mu16Adress] = mpcMsgRead.cRxData.mpu8Data[0];
        menCmd = enCmdIdle;
        break;

      case enCmdWriteAll:
        // EEPROM komplett schreiben
        // Page by Page
        menCmd = enCmdWriteAll2;
        mu16Adress = 0;
        // Write Adress + 16Byte einlesen
        mpcMsgWrite.cTxData.muiLen = 17;

        mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
        mpcMsgWrite.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
        for (u8 lu8i = 1; lu8i < 17; lu8i++)
        {
          mpcMsgWrite.cTxData.mpu8Data[lu8i] = mu8RamMirrow[mu16Adress];
          mu16Adress++;
        }
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
        break;
      case enCmdWriteAll2:
        // tWR Write Cycle Time 5 ms
        menCmd = enCmdWriteAll3;
        if (mu16Adress == 16) mu8WriteWaitTime = 20;
                        else  mu8WriteWaitTime = 10;

        break;
      case enCmdWriteAll3:
        if (mu16Adress < 1023)
        {
          menCmd = enCmdWriteAll2;
          // Nächsten Daten schreiben
          mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
          mpcMsgWrite.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
          for (u8 lu8i = 1; lu8i < 17; lu8i++)
          {
            mpcMsgWrite.cTxData.mpu8Data[lu8i] = mu8RamMirrow[mu16Adress];
            mu16Adress++;
          }
          mpcMsgWrite.vStart();
          mI2C->vStartMsg(&mpcMsgWrite, this);
        }
        else
        {
          mu16Adress = 0;
          menCmd = enCmdIdle;
        }
        break;

      default:
        break;
    }

    mStatus.IsStartRequested = (munStartFlags.stFlags.mbInit |
                                munStartFlags.stFlags.mbReadByte |
                                munStartFlags.stFlags.mbReadBlock |
                                munStartFlags.stFlags.mbWriteByte |
                                munStartFlags.stFlags.mbWriteAll);
  }


  void vTick10ms(void)
  {
    if ((mu8WriteWaitTime) && (menCmd == enCmdWriteAll3))
    {
      if (mu8WriteWaitTime > 10)
      {
        mu8WriteWaitTime -= 10;
      }
      else
      {
        mu8WriteWaitTime = 0;
        munStartFlags.stFlags.mbWriteAllContinue = true;
        mStatus.IsStartRequested = 1;
      }
    }
  }

  void vReadBlockStart()
  {
    if (mu16Adress <= mu16AdressEnd)
    {
      u16 lu16Len = mu16AdressEnd - mu16Adress + 1;

      if (lu16Len > 16) lu16Len = 16;

      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cRxData.muiLen = lu16Len;

      mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
      mpcMsgRead.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
    }
  }

  void vReadBlockProcess()
  {
    for (u8 lu8i = 0; lu8i < mpcMsgRead.cRxData.muiLen; lu8i++)
    {
      mu8RamMirrow[mu16Adress] = mpcMsgRead.cRxData.mpu8Data[lu8i];
      mu16Adress++;
    }

    if (mu16Adress <= mu16AdressEnd)
    {
      u16 lu16Len = mu16AdressEnd - mu16Adress + 1;
      if (lu16Len > 16) lu16Len = 16;

      // Nächsten Daten lesen
      mpcMsgRead.cRxData.muiLen = lu16Len;
      mAdr = 0x0A0 + ((mu16Adress >> 8) << 1);
      mpcMsgRead.cTxData.mpu8Data[0] = mu16Adress & 0xFF;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
    }
  }

  void vCmdSetup()
  {
    mu16Adress = 0;
    munStartFlags.stFlags.mbInit = true;
    mStatus.IsError = false;
    mStatus.IsStartRequested = 1;
  }

  void vCmdReadByte(u16 lu16Adress)
  {
    if (menCmd == enCmdIdle)
    {
      mu16Adress = lu16Adress;
      munStartFlags.stFlags.mbReadByte = true;
      mStatus.IsStartRequested = 1;
    }
  }

  void vCmdReadBlock(u16 lu16AdrStart, u16 lu16AdrEnd)
  {
    if (menCmd == enCmdIdle)
    {
      mu16Adress    = lu16AdrStart;
      mu16AdressEnd = lu16AdrEnd;
      munStartFlags.stFlags.mbReadBlock = true;
      mStatus.IsStartRequested = 1;
    }
  }

  void vCmdWriteByte(u16 lu16Adress)
  {
    if (menCmd == enCmdIdle)
    {
      mu16Adress = lu16Adress;
      munStartFlags.stFlags.mbWriteByte = true;
      mStatus.IsStartRequested = 1;
    }
  }


  void vCmdWriteByte(u16 lu16Adress, u8 lu8Data)
  {
    if (menCmd == enCmdIdle)
    {
      mu8RamMirrow[lu16Adress] = lu8Data;
      vCmdWriteByte(lu16Adress);
    }
  }

  void vCmdWriteAll()
  {
    if (menCmd == enCmdIdle)
    {
      munStartFlags.stFlags.mbWriteAll = true;
      mStatus.IsStartRequested = 1;
    }
  }

  bool isIdle()
  {
    return (menCmd == enCmdIdle);
  }

  bool isBusy()
  {
    return (!isIdle());
  }
};



#endif // __I2C_EEP_H__
