#ifndef __CLASS_RING_BUFFER_H__
#define __CLASS_RING_BUFFER_H__

#include "typedef.h"

template <typename tycRingBufT, typename tycRingBufTIndex>
class cRingBufT
{
public:
  tycRingBufT*     mtyBuf;
  tycRingBufTIndex mtySize;
  tycRingBufTIndex mtyHead;
  tycRingBufTIndex mtyCnt;
  tycRingBufTIndex mtyTail;

  void inc(tycRingBufTIndex &lVal)
  {
    lVal++;
    if (lVal >= mtySize) lVal = 0;
  }


  cRingBufT(const tycRingBufT* lptyBuf, const tycRingBufTIndex ltySize)
  {
    reset();
    mtySize = ltySize;
    mtyBuf  = (tycRingBufT*)lptyBuf;
  }

  void reset()
  {
    mtyHead = mtyTail = mtyCnt = 0;
  }

  bool isEmpty()
  {
    return mtyCnt == 0;
  }

  tycRingBufTIndex cnt()
  {
    return mtyCnt;
  }

  tycRingBufTIndex space_left()
  {
    return (mtySize - mtyCnt);
  }

  bool isFull()
  {
    return mtyCnt == mtySize;
  }

  void put_unsafe(tycRingBufT ltyItem)
  {
    if (mtyCnt < mtySize)
    {
      mtyBuf[mtyHead] = ltyItem;
      inc(mtyHead);
      mtyCnt++;
    }
  }

  void put(tycRingBufT ltyItem)
  {
    _dai();
    put_unsafe(ltyItem);
    _eai();
  }


  void put(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    while (lu32Cnt > 0)
    {
      put_unsafe(*ltyItem);
      ltyItem++;
      lu32Cnt--;
    }
    _eai();
  }


  tycRingBufT get_unsafe(void)
  {
    tycRingBufT ltyZwerg;

    if (isEmpty())
    {
      return 0;
    }

    //Read data and advance the tail (we now have a free space)
    ltyZwerg = mtyBuf[mtyTail];
    inc(mtyTail);
    mtyCnt--;

    return ltyZwerg;
  }

  tycRingBufT get(void)
  {
    tycRingBufT ltyZwerg;
    _dai();
    ltyZwerg = get_unsafe();
    _eai();

    return ltyZwerg;
  }

  void get(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    while (lu32Cnt > 0)
    {
      *ltyItem = get_unsafe();
      ltyItem++;
      lu32Cnt--;
    }
    _eai();
  }


  void move(cRingBufT *lcDest, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    while (lu32Cnt > 0)
    {
      lcDest->put_unsafe(get_unsafe());
      lu32Cnt--;
    }
    _eai();
  }
};





#endif //__CLASS_RING_BUFFER_H__
