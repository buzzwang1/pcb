#ifndef _MESSAGE_H
#define _MESSAGE_H



#include "typedef.h"

class cMsg
{
public:
  enum tenMsgIDs
  {
    nCursorDummy = 0,

    //Cursor
    nCursor,
    nGfxElement,
    nTimestamp
  };

  uint8 mID;
  uint8 mSubID;
  uint8 mui8Data[8];

  cMsg()
  {
    uint32 *lui8Data = (uint32*)mui8Data;
    lui8Data[0] = 0;
    lui8Data[1] = 0;
  }

  void vClone(cMsg *lpcMsg)
  {
    uint32 *lui8DataSrc = (uint32*)mui8Data;
    uint32 *lui8DataDst = (uint32*)lpcMsg->mui8Data;
    lpcMsg->mID    = mID;
    lpcMsg->mSubID = mSubID;
    
    // 2x 32Bit => 8Byte
    lui8DataDst[0] = lui8DataSrc[0];
    lui8DataDst[1] = lui8DataSrc[1];
  }
};


class cMsgBox
{
public:

  cMsg  *mMsgBuf;
  uint32 mHead;
  uint32 mCnt;
  uint32 mSize;
  uint32 mTail;

  
  cMsgBox(uint32 lui32Size)
  {
    mMsgBuf = new cMsg[lui32Size];

    mHead = 0;
    mTail = 0;
    mCnt  = 0;
    mSize = lui32Size;
  }

  ~cMsgBox()
  {
    delete[]mMsgBuf;
  }

  bool isEmpty(void)
  {
    return mCnt == 0;
  }


  void vput(cMsg *lpcMsg)
  {
    lpcMsg->vClone(&mMsgBuf[mHead]);
    mHead = (mHead + 1) % mSize;

    if (mHead == mTail)
    {
      mTail = (mTail + 1) % mSize;
    }
    mCnt++;
  }

  void vget(cMsg *lpcMsg)
  {
    if (isEmpty())
    {
      return;
    }

    //Read data and advance the tail (we now have a free space)
    mMsgBuf[mTail].vClone(lpcMsg);
    mTail = (mTail + 1) % mSize;
    mCnt--;
  }
};




#endif /* _MESSAGE_H */
