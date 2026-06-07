#pragma once

#include "Typedef.h"
#include "cBitField.h"

class cComComp
{
  public:
  cBitField32 mReq;
  cBitField32 mProcess;
  cBitField32 mDone;
  cBitField32 mError;

  void vClearAllRequest() { mReq.vClearAll(); }
  void vClearAllErrors() { mError.vClearAll(); }

  i8 i8GetRequest() { return mReq.i8GetFirst(); }

  void vRequestClear(u8 lu8Bit) { mReq.vClear(lu8Bit); }
  void vRequestSet(u8 lu8Bit) { mReq.vSet(lu8Bit); }
  bool isRequest() { return mReq; };

  void vProcessClear(u8 lu8Bit) { mProcess.vClear(lu8Bit); }
  void vProcessSet(u8 lu8Bit) { mProcess.vSet(lu8Bit); }
  bool isProcess() { return mProcess; };

  void vDoneClear(u8 lu8Bit) { mDone.vClear(lu8Bit); }
  void vDoneSet(u8 lu8Bit) { mDone.vSet(lu8Bit); }
  bool isDone(u8 lu8Bit) { return mDone.isSet(lu8Bit); }

  void vErrorClear(u8 lu8Bit) { mError.vClear(lu8Bit); }
  void vErrorSet(u8 lu8Bit) { mError.vSet(lu8Bit); }
  bool isError() { return mError; };
  bool isError(u8 lu8Bit) { return mError.isSet(lu8Bit); }
};

