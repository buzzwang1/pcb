#pragma once

#include "Typedef.h"
#include "ComDat.h"
#include "ComRegMap.h"
#include "cComComp.h"
#include "cComSeq.h"


template <typename mtcRegMap>
class cComSeqHdlI2c : public cComNode, public cComComp, public mtcRegMap
{
  public:

  typedef enum
  {
    enCmdIdle = 0,
    enCmdReadDone,
    enCmdWriteDone,
    enCmdReadAll,
    enCmdWriteAll,
  }tenCmd;

  cComNodeMaster*  mComNode;

  cComMsg<u16> mpcMsgRx;
  cComMsgS<u16, mtcRegMap::mu16GetRegMaxLen + 1> mpcMsgTx; // + 1 für Adresse
  cComMsg<u16> mpcMsgBlockTx;

  cComMsgI2c<u16> mpcMsgTxRx;

  u16 muRestart_ms;
  u16 mu16Delay_ms;
  tenCmd menCmd;

  u8  mEntryIdx;
  u8  mRegIdx; 
  i8  mActiveSequence;
  i8  mActiveSequenceToContinue;

  cComSeqHdlI2c(cComNodeMaster* lpcNode, u8 lu8Adr)
    : mpcMsgRx(cComNode::tenDirection::enIsRx),
      mpcMsgTx(cComNode::tenDirection::enIsTx),
      mpcMsgBlockTx(cComNode::tenDirection::enIsTx),
      mpcMsgTxRx(&mpcMsgTx, &mpcMsgRx)
  {
    mComNode = lpcNode;
    mAdr     = lu8Adr;
    menCmd   = enCmdIdle;

    mu16Delay_ms = 0;
    mActiveSequenceToContinue = mActiveSequence = -1;
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

    mu16Delay_ms = 0;
    mActiveSequence = -1;

    vRequest(cComSeq::nSEQ00Reset);
    vRequest(cComSeq::nSEQ01Init1);
    vRequest(cComSeq::nSEQ02Init2);
    vRequest(cComSeq::nSEQ04Enable);
  }

  u8 u8StartReadReg()
  {
    cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
    cComSeq::cEntry lcEntry = lcSeq->pacGet()[mEntryIdx];
    mRegIdx = lcEntry.u8GetReg();

    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);

    if (lu8RegLen != 0)
    {
      menCmd = enCmdReadDone;

      mpcMsgRx.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);
      mpcMsgTxRx.vPrepareRx(&mpcMsgTx, &mpcMsgRx, mRegAdr, lu8RegLen);
      mComNode->vStartMsg(&mpcMsgTxRx);
      return 1;
    }
    return 0;
  }

  u8 u8StartWriteReg()
  {
    cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
    cComSeq::cEntry lcEntry = lcSeq->pacGet()[mEntryIdx];
    mRegIdx = lcEntry.u8GetReg();

    menCmd = enCmdWriteDone;

    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);

    mpcMsgTx.vPrepareTx(mRegAdr, lu8RegLen, mtcRegMap::pu8GetMem(mRegIdx));
    mComNode->vStartMsg(&mpcMsgTx);
    return 1;
  }

  u8 u8ReadRegAll()
  {
    if (mRegIdx >= mtcRegMap::u8GetRegCnt()) return 2;

    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);

    while ((lu8RegLen == 0) && (mRegIdx < mtcRegMap::u8GetRegCnt()))
    {
      mRegIdx++;
      mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
      lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);
    }

    if (lu8RegLen == 0)
    {
      // Das letzte Register kann nicht gelesen werden
      return 2;
    }
    else
    {
      menCmd = enCmdReadAll;

      mpcMsgRx.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);
      mpcMsgTxRx.vPrepareRx(&mpcMsgTx, &mpcMsgRx, mRegAdr, lu8RegLen);

      mComNode->vStartMsg(&mpcMsgTxRx);
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
    menCmd = enCmdWriteAll;

    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);

    mpcMsgTx.vPrepareTx(mRegAdr, lu8RegLen, mtcRegMap::pu8GetMem(mRegIdx));
    mComNode->vStartMsg(&mpcMsgTx);
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
    cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
    cComSeq::cEntry* lacEntryList = lcSeq->pacGet();
    cComSeq::cEntry lcEntry = lacEntryList[mEntryIdx];

    // Ein Register ...
    if (lcEntry.isReg(mtcRegMap::u8GetRegCnt()))
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
    if (lcEntry.isRegAll(mtcRegMap::u8GetRegCnt()))
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
    // Block ...
    if (lcEntry.isBlock(mtcRegMap::u8GetRegCnt()))
    {
      // ... lesen
      if (lcEntry.isRead())
      {
        // Nicht implementiert
        return 0;
      }
      else
      // ... schreiben
      //if (lcEntry.isWrite())
      {
        // Block
        // Byte 1: Block länge in Byte
        // Byte 2..: Data..
        mEntryIdx++;
        if ((mEntryIdx + 1) < lcSeq->u8GetLen())
        {
          u8 lu8Len = lacEntryList[mEntryIdx].munEntry.u8Value;
          mEntryIdx++;

          if (lu8Len > 0)
          {
            menCmd = enCmdWriteDone;

            mpcMsgBlockTx.muLen = lu8Len;
            mpcMsgBlockTx.mpu8Data = (u8*)(&lacEntryList[mEntryIdx]);

            mEntryIdx += (lu8Len - 1);

            mComNode->vStartMsg(&mpcMsgBlockTx);
            return 1;
          }
        }
      }
    }
    else
    // Ptr ...
    if (lcEntry.isPtr(mtcRegMap::u8GetRegCnt()))
    {
      // ... lesen
      if (lcEntry.isRead())
      {
        // Nicht implementiert
        return 0;
      }
      else
      // ... schreiben
      //if (lcEntry.isWrite())
      {
        // Block
        // Byte 1, 2:    Block länge in Byte
        // Byte 3,4,5,6: Pointer zu den Daten
        mEntryIdx++;
        if ((mEntryIdx + 1) < lcSeq->u8GetLen())
        {
          u16 lu8Len = lacEntryList[mEntryIdx].munEntry.u8Value << 8;
          mEntryIdx++;
          lu8Len += lacEntryList[mEntryIdx].munEntry.u8Value;
          mEntryIdx++;

          u32 lu32Ptr = lacEntryList[mEntryIdx].munEntry.u8Value << 24;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value << 16;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value << 8;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value;

          if ((lu8Len > 0) && (lu32Ptr))
          {
            menCmd = enCmdWriteDone;

            mpcMsgBlockTx.muLen = lu8Len;
            mpcMsgBlockTx.mpu8Data = (u8*)(lu32Ptr);

            mComNode->vStartMsg(&mpcMsgBlockTx);
            return 1;
          }
        }
      }
    }
    else
    if (lcEntry.isDelay(mtcRegMap::u8GetRegCnt()))
    {
      // Wartezeit einlesen
      mEntryIdx++;
      if ((mEntryIdx + 1) < lcSeq->u8GetLen())
      {
        mu16Delay_ms  = lacEntryList[mEntryIdx].munEntry.u8Value << 8;
        mEntryIdx++;
        mu16Delay_ms += lacEntryList[mEntryIdx].munEntry.u8Value;
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
    cComNode::vComError(lenError, lenState);
    mStatus.IsInit = false;
    muRestart_ms = 100;

    if (mActiveSequence == -1)
    {
      vDoneSet(mActiveSequence);
      vErrorSet(mActiveSequence);
      vProcessClear(mActiveSequence);
    }

    cComComp::mReq.vClearAll();
    mActiveSequence = -1;
  }



  bool bStartEntry()
  {
    cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
    while(1)
    {
      switch (u8StartEntry())
      {
        case 1: // kein Fehler
          return True;
          break;
        case 2: // kein Fehler, aber auch nicht starten können
                // Eintrag konnte nicht ausgeführt werden
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
          cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
          if (lcSeq)
          {
            // überprüfen, ob die Sequenz einen Inhalt halt
            u8 lu8Len = lcSeq->u8GetLen();
            if (lu8Len > 0)
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
      cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);

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
      }
    }
    mControl.StartRequest = isRequest();
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
    if (mActiveSequence == -1)
    {
      vDoSeqence();
    }
    else
    if (mActiveSequenceToContinue != -1)
    {
      mActiveSequenceToContinue = -1;
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

    if (mu16Delay_ms)
    {
      if (mu16Delay_ms > lu16Time_ms)
      {
        mu16Delay_ms -= lu16Time_ms;
      }
      else
      {
        mu16Delay_ms = 0;
        if (mActiveSequence != -1)
        {
          mActiveSequenceToContinue = mActiveSequence;
          mControl.StartRequest = isRequest();
        }
      }
    }
  };

  void vRequest(u8 lu8SeqIdx)
  {
    vRequestSet(lu8SeqIdx);
    mControl.StartRequest = isRequest();
  }

  bool isAnySeqError()
  {
    return cComComp::isError();
  }

  bool isAnySeqProcess()
  {
    return cComComp::isProcess();
  }

  bool isAnySeqStartRequest()
  {
    return cComComp::isRequest();
  }

  bool isAnySeqPending()
  {
    return ((isAnySeqStartRequest()) || (isAnySeqProcess()));
  }
};





