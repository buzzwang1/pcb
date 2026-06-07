#ifndef __SIC45X_H__
#define __SIC45X_H__

#include "Typedef.h"
#include "SIC45X_RegMap.h"




/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/


#define SIC45X_I2C_ADDRESS_0k845   0b0010000
#define SIC45X_I2C_ADDRESS_1k3     0b0010001
#define SIC45X_I2C_ADDRESS_1k78    0b0010010
#define SIC45X_I2C_ADDRESS_2k32    0b0010011
#define SIC45X_I2C_ADDRESS_2k87    0b0010100
#define SIC45X_I2C_ADDRESS_3k48    0b0010101
#define SIC45X_I2C_ADDRESS_4k12    0b0010110
#define SIC45X_I2C_ADDRESS_4k75    0b0010111
#define SIC45X_I2C_ADDRESS_5k49    0b0011000
#define SIC45X_I2C_ADDRESS_6k19    0b0011001
#define SIC45X_I2C_ADDRESS_6k98    0b0011010
#define SIC45X_I2C_ADDRESS_7k87    0b0011011
#define SIC45X_I2C_ADDRESS_8k87    0b0011100
#define SIC45X_I2C_ADDRESS_10k0    0b0011101
#define SIC45X_I2C_ADDRESS_11k0    0b0011110

#define SIC45X_I2C_ADDRESS_DEFAULT      SIC45X_I2C_ADDRESS_10k0

/*=========================================================================*/


class cSIC45X: public cComNode, public cSIC45X_RegisterMap
{
  public:

  typedef enum
  {
    enCmdIdle = 0,
    enCmdSequence1,
    enCmdSequence2,
    enCmdSequence3,
    enCmdReadDone,
    enCmdWriteDone,
    enCmdReadAll,
    enCmdReadAll2,
    enCmdReadAll3,
    enCmdWriteAll,
    enCmdWriteAll2,
    enCmdWriteAll3,
  }tenCmd;

  cI2cMaster* mI2C;

  cComDatMsgDyn mpcMsg;

  u8  mEntryIdx;
  u8  mRegIdx;
  u8  mu8Delay;
  i8  mActiveSequence;
  tenCmd menCmd;

  cSIC45X(cI2cMaster* lpcI2C, uint8 lui8Adr)
  {
    lui8Adr <<= 1;
    mI2C   = lpcI2C;
    mAdr   = lui8Adr;
    menCmd = enCmdIdle;

    mu8Delay = 0;
    mActiveSequence = -1;

    mpcMsg.vMemAlloc(24, 0);

    //i8Setup();
    mI2C->vAddSlave((cComNode*)this);
  }

  #if _WIN32
    ~cSIC45X()
    {
      mpcMsg.vMemFree();
    }
  #endif // _WIN32


  u8 u8StartReadReg()
  {
    cSequence* lcSeq = pcGetSequence(mActiveSequence);
    cSequence::cEntry lcEntry = lcSeq->pacGet()[mEntryIdx];
    mRegIdx = lcEntry.u8GetReg();

    u8 mRegAdr = u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = u8GetRegLen(mRegIdx);

    if (lu8RegLen != 0)
    {
      menCmd = enCmdReadDone;

      mpcMsg.cRxData.muiLen   = lu8RegLen;
      mpcMsg.cRxData.mpu8Data = pu8GetMem(mRegIdx);
      mpcMsg.cTxData.muiLen = 1;
      mpcMsg.cTxData.mpu8Data[0] = mRegAdr;

      mpcMsg.vStart();
      mI2C->vStartMsg(&mpcMsg);
      return 1;
    }
    return 0;
  }

  u8 u8StartWriteReg()
  {
    cSequence* lcSeq = pcGetSequence(mActiveSequence);
    cSequence::cEntry lcEntry = lcSeq->pacGet()[mEntryIdx];
    mRegIdx = lcEntry.u8GetReg();

    u8 mRegAdr = u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = u8GetRegLen(mRegIdx);

    menCmd = enCmdWriteDone;

    mpcMsg.cRxData.muiLen   = 0;
    mpcMsg.cRxData.mpu8Data = null;
    mpcMsg.cTxData.muiLen      = lu8RegLen + 2; // + 1 für register + 1 für PEC Byte
    mpcMsg.cTxData.mpu8Data[0] = mRegAdr;

    for (u8 i = 0; i < lu8RegLen; i++)
    {
      mpcMsg.cTxData.mpu8Data[i + 1] = pu8GetMem(mRegIdx)[i];
    }

    // PEC Byte CRC-8 Polynom x^8 + x^2 + x^1 + 1;  1.00000111
    //                         
    u8 lu8PEC = 0 ^ mAdr; // Initialwert ist 0
    for (u8 j = 0; j < 8; j++) 
    {
      if (lu8PEC & 0x80) {
        lu8PEC = (lu8PEC << 1) ^ 0x07; // Polynom 0x07
      }
      else {
        lu8PEC <<= 1;
      }
    }

    // + 1 für register aber ohne PEC Byte
    for (u8 i = 0; i < (lu8RegLen + 1); i++) 
    {
      lu8PEC ^= mpcMsg.cTxData.mpu8Data[i];
      for (u8 j = 0; j < 8; j++) {
        if (lu8PEC & 0x80) {
          lu8PEC = (lu8PEC << 1) ^ 0x07; // Polynom 0x07
        }
        else {
          lu8PEC <<= 1;
        }
      }
    }


    mpcMsg.cTxData.mpu8Data[lu8RegLen + 1] = lu8PEC;

    mpcMsg.vStart();
    mI2C->vStartMsg(&mpcMsg);
    return 1;
  }

  u8 u8ReadRegAll()
  {
    if (mRegIdx >= u8GetRegCnt()) return 2;

    u8 mRegAdr = u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = u8GetRegLen(mRegIdx);

    while ((lu8RegLen == 0) && (mRegIdx < u8GetRegCnt()))
    {
      mRegIdx++;
      mRegAdr = u8GetRegAdr(mRegIdx);
      lu8RegLen = u8GetRegLen(mRegIdx);
    }

    if (lu8RegLen == 0)
    {
      // Das letzte Register kann nicht gelesen werden
      return 2;
    }
    else
    {
      menCmd = enCmdReadAll;

      mpcMsg.cRxData.muiLen   = lu8RegLen;
      mpcMsg.cRxData.mpu8Data = pu8GetMem(mRegIdx);
      mpcMsg.cTxData.muiLen = 1;
      mpcMsg.cTxData.mpu8Data[0] = mRegAdr;

      mpcMsg.vStart();
      mI2C->vStartMsg(&mpcMsg);
      // Alles Ok
      return 1;
    }
  }

  bool bContinueReadRegAll()
  {
    mRegIdx++;
    switch (u8ReadRegAll())
    {
      case 1: return True;
      default: return False;
    }
  }

  u8 u8StartReadRegAll()
  {
    mRegIdx = 0;
    return u8ReadRegAll();
  }

  u8 u8WriteRegAll()
  {
    u8 mRegAdr = u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = u8GetRegLen(mRegIdx);

    menCmd = enCmdWriteAll;

    mpcMsg.cRxData.muiLen   = 0;
    mpcMsg.cRxData.mpu8Data = null;
    mpcMsg.cTxData.muiLen      = lu8RegLen + 1;
    mpcMsg.cTxData.mpu8Data[0] = mRegAdr;

    for (u8 i = 0; i < lu8RegLen; i++)
    {
      mpcMsg.cTxData.mpu8Data[i + 1] = pu8GetMem(mRegIdx)[i];
    }

    mpcMsg.vStart();
    mI2C->vStartMsg(&mpcMsg);
    return 1;
  }


  bool bContinueWriteRegAll()
  {
    mRegIdx++;
    switch (u8WriteRegAll())
    {
      case 1: return True;
      default: return False;
    }
  }

  u8 u8StartWriteRegAll()
  {
    mRegIdx = 0;
    return u8WriteRegAll();
  }


  u8 u8StartEntry()
  {
    cSequence* lcSeq = pcGetSequence(mActiveSequence);
    cSequence::cEntry* lacEntryList = lcSeq->pacGet();
    cSequence::cEntry lcEntry = lacEntryList[mEntryIdx];

    // Ein Register ...
    if (lcEntry.isReg(u8GetRegCnt()))
    {
      // ... lesen
      if (lcEntry.isRead())
      {
        return u8StartReadReg();
      }
      else
      // ... schreiben
      //if (lcEntry.isWrite())
      {
        return u8StartWriteReg();
      }
    }
    else
    // Alle Register ...
    if (lcEntry.isRegAll(u8GetRegCnt()))
    {
      // ... lesen
      if (lcEntry.isRead())
      {
        return u8StartReadRegAll();
      }
      else
      // ... schreiben
      //if (lcEntry.isWrite())
      {
        return u8StartWriteRegAll();
      }
    }
    else
    if (lcEntry.isDelay(u8GetRegCnt()))
    {
      // Wartezeit einlesen
      mEntryIdx++;
      if (mEntryIdx < lcSeq->u8GetLen())
      {
        mu8Delay = lacEntryList[mEntryIdx].munEntry.u8Value;
        return 1;
      }
      else // Fehler - Wartezeit fehlt
      {
      }
    }
    else // Fehler - Unbekannter eintrag
    {
    }

    return 0;
  }


  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError);
    UNUSED(lenState);

    if (mActiveSequence == -1)
    {
      vDoneSet(mActiveSequence);
      vErrorSet(mActiveSequence);
      vProcessClear(mActiveSequence);
    }

    mActiveSequence = -1;
    mStatus.IsStartRequested = isRequest();
  }


  bool bStartEntry()
  {
    cSequence* lcSeq = pcGetSequence(mActiveSequence);
    while(1)
    {
      switch (u8StartEntry())
      {
        case 1: // kein Fehler
          return True;
          break;
        case 2: // kein Fehler, aber auch nicht starten können
                // Eintrag könnte nicht ausgeführt werden
          mEntryIdx++;
          if (mEntryIdx >= lcSeq->u8GetLen())
          {
            vDoneSet(mActiveSequence);
            mActiveSequence = -1;
            vProcessClear(mActiveSequence);
            return False;
          }
          break;
        default: // Fehler
          vDoneSet(mActiveSequence);
          mActiveSequence = -1;
          vErrorSet(mActiveSequence);
          vProcessClear(mActiveSequence);
          return False;
          break;
        }
    }
  }

  void vDoSeqence()
  {
    if (mActiveSequence == -1)
    {
      while (1)
      {
        // Erste angeforderte Sequence nehmen
        mActiveSequence = i8GetRequest();
        if (mActiveSequence >= 0)
        {
          vRequestClear(mActiveSequence);

          // Überprüfen, ob die Sequenz implementiert ist
          cSequence* lcSeq = pcGetSequence(mActiveSequence);
          if (lcSeq)
          {
            // überprüfen, ob die Sequenz einen Inhalt halt
            if (lcSeq->u8GetLen() > 0)
            {
              vDoneClear(mActiveSequence);
              vProcessSet(mActiveSequence);

              mEntryIdx = 0;

              if (bStartEntry()) return;
            }
            else
            {
              // kein Inhalt
              // direkt in Done setzen und
              // weiter zur nächsten Sequenz
              vDoneSet(mActiveSequence);
              mActiveSequence = -1;
            }
          }
          else
          {
            // nicht implementiert
            // direkt in Done setzen und
            // weiter zur nächsten Sequenz
            vDoneSet(mActiveSequence);
            mActiveSequence = -1;
          }
        }
        else
        {
          // Es war nichts angefordert
          return;
        }
      }
    }
    else
    {
      // Es läuft gerade eine Sequenz
      cSequence* lcSeq = pcGetSequence(mActiveSequence);

      mEntryIdx++;

      if (mEntryIdx < lcSeq->u8GetLen())
      {
        if (bStartEntry()) return;
      }
      else
      {
        // Sequenz fertig
        vDoneSet(mActiveSequence);
        vProcessClear(mActiveSequence);
        mActiveSequence = -1;
        //vDoSeqence();
      }
    }
    mStatus.IsStartRequested = isRequest();
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
    if (mActiveSequence == -1)
    {
      vDoSeqence();
    }
  }

  void vComDone() override
  {
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdReadDone:
        menCmd = enCmdIdle;
        vDoSeqence();
        break;

      case enCmdWriteDone:
        menCmd = enCmdIdle;
        vDoSeqence();
        break;

      case enCmdReadAll:
        if (!bContinueReadRegAll())
        {
          menCmd = enCmdIdle;
          vDoSeqence();
        }
        break;

      case enCmdWriteAll:
        if (!bContinueWriteRegAll())
        {
          menCmd = enCmdIdle;
          vDoSeqence();
        }
        break;

      default:
        break;
    }
  }

  void vRequest(u8 lu8SeqIdx)
  {
    vRequestSet(lu8SeqIdx);
    mStatus.IsStartRequested = 1;
  }
};

#endif // __SIC45X_H__
