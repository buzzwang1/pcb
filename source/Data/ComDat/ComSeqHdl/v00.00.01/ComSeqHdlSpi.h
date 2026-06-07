#pragma once

#include "Typedef.h"
#include "ComDat.h"
#include "ComRegMap.h"
#include "cComComp.h"
#include "cComSeq.h"


// bCsPerEntry: CS toggled per Command Message (True) or per Sequence (False)

template <bool bCsPerCmd, typename mtcRegMap>
class cComSeqHdlSpi : public cComNode, public cComComp, public mtcRegMap
{
  public:

  typedef enum
  {
    enCmdIdle = 0,
    enCmdReadRegDone,
    enCmdReadDataDone,
    enCmdWriteRegDone,
    enCmdWriteDataDone,
    enCmdReadRegAll,
    enCmdReadDataAll,
    enCmdWriteRegAll,
    enCmdWriteDataAll,
  }tenCmd;

  cComNodeMaster*  mComNode;

  cComMsg<u16> mpcMsgRxData;
  cComMsgS<u16, 1> mpcMsgTxCmd;
  cComMsg<u16> mpcMsgTxData;
  cComMsg<u16> mpcMsgBlockTx;

  u16 muRestart_ms;
  u16 mu16Delay_ms;
  tenCmd menCmd;

  u8  mEntryIdx;
  u8  mRegIdx; 
  i8  mActiveSequence;
  i8  mActiveSequenceToContinue;

  cComSeqHdlSpi(cComNodeMaster* lpcNode, u8 lu8Adr)
    : mpcMsgRxData(cComNode::tenDirection::enIsRx),
      mpcMsgTxCmd(cComNode::tenDirection::enIsTx),
      mpcMsgTxData(cComNode::tenDirection::enIsTx),
      mpcMsgBlockTx(cComNode::tenDirection::enIsTx)
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
      menCmd = enCmdReadRegDone;
    
      mpcMsgTxCmd.Set(mRegAdr);
      mpcMsgRxData.muLen = lu8RegLen;
      mpcMsgRxData.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);
      vCmdStart();
      mComNode->vStartMsg(&mpcMsgTxCmd);
      return 1;
    }
    return 0;
  }

  u8 u8StartWriteReg()
  {
    cComSeq* lcSeq = mtcRegMap::pcGetSequence(mActiveSequence);
    cComSeq::cEntry lcEntry = lcSeq->pacGet()[mEntryIdx];
    mRegIdx = lcEntry.u8GetReg();

    menCmd = enCmdWriteRegDone;

    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);

    //mpcMsgTxCmd.vPrepareTx(mRegAdr, lu8RegLen, mtcRegMap::pu8GetMem(mRegIdx));
    mpcMsgTxCmd.Set(mRegAdr);
    mpcMsgTxData.muLen = lu8RegLen;
    mpcMsgTxData.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);

    vCmdStart();
    mComNode->vStartMsg(&mpcMsgTxCmd);
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
      menCmd = enCmdReadRegAll;
       
      mpcMsgTxCmd.Set(mRegAdr);
      mpcMsgRxData.muLen = lu8RegLen;
      mpcMsgRxData.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);
      vCmdStart();
      mComNode->vStartMsg(&mpcMsgTxCmd);
      // Alles Ok
      return 1;
    }
    return 0;
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
    menCmd = enCmdWriteRegAll;
    
    u8 mRegAdr   = mtcRegMap::u8GetRegAdr(mRegIdx);
    u8 lu8RegLen = mtcRegMap::u16GetRegLen(mRegIdx);
    
    mpcMsgTxCmd.Set(mRegAdr);
    mpcMsgTxData.muLen = lu8RegLen;
    mpcMsgTxData.mpu8Data = mtcRegMap::pu8GetMem(mRegIdx);

    vCmdStart();
    mComNode->vStartMsg(&mpcMsgTxCmd);
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
      return 0;
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
            menCmd = enCmdWriteDataDone;

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
          u16 lu16Len = lacEntryList[mEntryIdx].munEntry.u8Value << 8;
          mEntryIdx++;
          lu16Len += lacEntryList[mEntryIdx].munEntry.u8Value;
          mEntryIdx++;

          u32 lu32Ptr = lacEntryList[mEntryIdx].munEntry.u8Value << 24;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value << 16;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value << 8;
          mEntryIdx++;
          lu32Ptr += lacEntryList[mEntryIdx].munEntry.u8Value;

          if ((lu16Len > 0) && (lu32Ptr))
          {
            menCmd = enCmdWriteDataDone;

            mpcMsgBlockTx.muLen = lu16Len;
            mpcMsgBlockTx.mpu8Data = (u8*)(lu32Ptr);

            vDataStart();
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

  virtual void vSequenceStart(u8 lu8SeqIdx)
  {
    UNUSED(lu8SeqIdx);
  }

  virtual void vSequenceDone(u8 lu8SeqIdx)
  {
    UNUSED(lu8SeqIdx);
  }

  void vSequenceStart()
  {
    mtcRegMap::vCsOn();
    vSequenceStart(mActiveSequence);
  }

  void vSequenceDone()
  {
    vSequenceDone(mActiveSequence);
    mtcRegMap::vCsOff();
  }

  void vDataStart()
  {
    mtcRegMap::vModeData();
  }

  void vCmdStart()
  {
    if (bCsPerCmd)
    {
      mtcRegMap::vCsOn();
    }

    mtcRegMap::vModeCmd();
  }

  void vMsgDone()
  {
    if (bCsPerCmd)
    {
      mtcRegMap::vCsOff();
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
              vSequenceStart();

              mEntryIdx = 0;

              if (bStartEntry()) return;
            }
            else
            {
              // kein Inhalt
              // direkt in Done setzen und
              // weiter zur nächsten Sequenz
              vDoneSet(mActiveSequence);
              vSequenceDone();
              mActiveSequence = -1;
            }
          }
          else
          {
            // nicht implementiert
            // direkt in Done setzen und
            // weiter zur nächsten Sequenz
            vDoneSet(mActiveSequence);
            vSequenceDone();
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
        vSequenceDone();
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

      case enCmdReadRegDone:
        if (mpcMsgRxData.Len())
        {
          menCmd = enCmdReadDataDone;
          vDataStart();
          mComNode->vStartMsg(&mpcMsgRxData);
        }
        else
        {
          menCmd = enCmdIdle;
          vDoSeqence();
        }
        break;

      case enCmdReadDataDone:
        menCmd = enCmdIdle;
        vMsgDone();
        vDoSeqence();
        break;

      case enCmdWriteRegDone:
        if (mpcMsgTxData.Len())
        {
          menCmd = enCmdWriteDataDone;
          vDataStart();
          mComNode->vStartMsg(&mpcMsgTxData);
        }
        else
        {
          menCmd = enCmdIdle;
          vDoSeqence();
        }
        break;

      case enCmdWriteDataDone:
        menCmd = enCmdIdle;
        vMsgDone();
        vDoSeqence();
        break;

      case enCmdReadRegAll:
        if (mpcMsgRxData.Len())
        {
          menCmd = enCmdReadDataAll;
          vDataStart();
          mComNode->vStartMsg(&mpcMsgRxData);
        }
        else
        {
          if (!bContinueReadRegAll())
          {
            menCmd = enCmdIdle;
            vDoSeqence();
          }
        }
        break;

      case enCmdReadDataAll:
        vMsgDone();
        if (!bContinueReadRegAll())
        {
          menCmd = enCmdIdle;
          vDoSeqence();
        }
        break;

      case enCmdWriteRegAll:
        if (mpcMsgTxData.Len())
        {
          menCmd = enCmdWriteDataAll;
          vDataStart();
          mComNode->vStartMsg(&mpcMsgTxData);
        }
        else
        {
          if (!bContinueReadRegAll())
          {
            menCmd = enCmdIdle;
            vDoSeqence();
          }
        }
        break;

      case enCmdWriteDataAll:
        vMsgDone();
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





