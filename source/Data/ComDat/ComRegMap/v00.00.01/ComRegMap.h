#pragma once

/**
 * Created by Gemini 3 Flash, 2026-05-05.
 * Resources: TI INA3221 Datasheet (SBOS516B), DefaultArm Framework.
 *
 * @class CINA3221
 * @brief Professionelle Hardware-Abstraktion für den INA3221 Monitor.
 * Jedes Register ist als eigene Union mit Bitfeldern (LSB-first) abgebildet.
 */

#include "TypeDef.h"
#include "cI2C.h"
#include "cComSeq.h"


struct cRegisterMap
{
  public:
  virtual u8 u8GetRegCnt();

  virtual u8 u8GetRegAdr(u8 lu8Idx) = 0;
  virtual u16 u16GetRegLen(u8 lu8Idx) = 0;

  virtual u8* pu8GetMem(u8 lu8Idx) = 0;
  virtual cComSeq* pcGetSequence(u8 lu8SeqIdx) = 0;

  bool isReg(cComSeq::cEntry lcEntry)
  {
    return (lcEntry.isReg(u8GetRegCnt()));
  }

  bool isReadAll(cComSeq::cEntry lcEntry)
  {
    return (lcEntry.isRegAll(u8GetRegCnt()) && lcEntry.isRead());
  }

  bool isWriteAll(cComSeq::cEntry lcEntry)
  {
    return (lcEntry.isRegAll(u8GetRegCnt()) && lcEntry.isWrite());
  }

  bool isDelay(cComSeq::cEntry lcEntry)
  {
    return (lcEntry.isDelay(u8GetRegCnt()));
  }
};
