#ifndef __CLASS_RING_BUFFER_H__
#define __CLASS_RING_BUFFER_H__

// reference: https://embeddedartistry.com/blog/2017/4/6/circular-buffers-in-cc

#include "typedef.h"
#include "MemTools.h"

#define cbRingBufT_memcpy_sum     cMemTools::vMemCpyAndSum
#define cbRingBufT_memcpy         cMemTools::vMemCpy
#define cbRingBufT_memset         cMemTools::vMemSet

//tycRingBufT:       Buffer wird übergeben
//tycRingBufStaticT: Buffer wird statisch angelegt
//tycRingBufDynT:    Buffer wird dynamisch (Heap) angelegt

template <typename tycRingBufT, typename tycRingBufTIndex>
//#ifdef __GNUC__
//class __attribute__((packed)) cRingBufT
//#else
class cRingBufT
//#endif
{
public:
  tycRingBufT*     mtyBuf;
  tycRingBufTIndex mtySize;
  tycRingBufTIndex mtyHead;
  tycRingBufTIndex mtyCnt;
  tycRingBufTIndex mtyTail;

  inline void inc(tycRingBufTIndex &lVal)
  {
    lVal++;
    if (lVal >= mtySize) lVal = 0;
  }

  inline void dec(tycRingBufTIndex &lVal)
  {
    if (lVal == 0) lVal = mtySize;
    lVal--;
  }


  cRingBufT(const tycRingBufT* lptyBuf, const tycRingBufTIndex ltySize)
  {
    reset();
    mtySize = ltySize;
    mtyBuf  = (tycRingBufT*)lptyBuf;
  }

  cRingBufT(const tycRingBufTIndex ltySize)
  {
    reset();
    mtySize = ltySize;
  }

  inline uint32 SizeOf()
  {
    return sizeof(cRingBufT<tycRingBufT, tycRingBufTIndex>) + mtySize * sizeof(tycRingBufT);
  }

  inline void reset()
  {
    mtyHead = mtyTail = mtyCnt = 0;
  }

  inline bool isEmpty()
  {
    return mtyCnt == 0;
  }

  inline tycRingBufTIndex cnt()
  {
    return mtyCnt;
  }

  inline tycRingBufTIndex space_left()
  {
    return (mtySize - mtyCnt);
  }

  inline bool isFull()
  {
    return mtyCnt == mtySize;
  }

  void put_unsafe(tycRingBufT ltyItem, bool lbOverwrite)
  {
    if (mtyCnt < mtySize)
    {
      mtyBuf[mtyHead] = ltyItem;
      inc(mtyHead);
      mtyCnt++;
    }
    else
    {
      if (lbOverwrite)
      {
        mtyBuf[mtyHead] = ltyItem;
        inc(mtyHead);
        inc(mtyTail);
      }
    }
  }

  void put_unsafe(tycRingBufT ltyItem)
  {
    put_unsafe(ltyItem, True);
  }


  inline void put(tycRingBufT ltyItem, bool lbOverwrite)
  {
    _dai();
    put_unsafe(ltyItem, lbOverwrite);
    _eai();
  }

  inline void put(tycRingBufT ltyItem)
  {
    put(ltyItem, True);
  }

  void put_unsafe(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt, bool lbOverwrite)
  {  
    if (!lbOverwrite)
    {
      if (lu32Cnt > space_left())
      {
        lu32Cnt = space_left();
      }
    } 

    if ((mtyHead + lu32Cnt) < mtySize)
    {
      cbRingBufT_memcpy((uint8*)(&mtyBuf[mtyHead]), (uint8*)ltyItem, lu32Cnt * sizeof(tycRingBufT));
      mtyHead += lu32Cnt;
      mtyCnt  += lu32Cnt;
    }
    else
    {
      tycRingBufTIndex lu32SplitCnt;

      mtyCnt += lu32Cnt;
      lu32SplitCnt = mtySize - mtyHead;

      cbRingBufT_memcpy((uint8*)(&mtyBuf[mtyHead]), (uint8*)ltyItem, lu32SplitCnt * sizeof(tycRingBufT));
      lu32Cnt -= lu32SplitCnt;

      if (lu32Cnt >= (tycRingBufTIndex)(mtySize * 2))
      {
        tycRingBufTIndex luTemp = ((lu32Cnt / mtySize) - 1) * mtySize;
        lu32Cnt      -= luTemp;
        lu32SplitCnt += luTemp;
      }

      while (lu32Cnt >= mtySize)
      {
        cbRingBufT_memcpy((uint8*)(mtyBuf), (uint8*)(&ltyItem[lu32SplitCnt]), mtySize * sizeof(tycRingBufT));  
        lu32Cnt -= mtySize;
        lu32SplitCnt += mtySize;
      }

      if (lu32Cnt > 0)
      {
        cbRingBufT_memcpy((uint8*)(mtyBuf), (uint8*)(&ltyItem[lu32SplitCnt]), lu32Cnt * sizeof(tycRingBufT));
      }

      mtyHead = lu32Cnt;
    }
    if (mtyCnt > mtySize)
    {
      mtyCnt  = mtySize;
      mtyTail = mtyHead;
    }
  }

  void put_unsafe(tycRingBufT *ltyItem, uint32 lu32Cnt)
  {
    put_unsafe(ltyItem, lu32Cnt, True);
  }

  inline void put(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt, bool lbOverwrite)
  {
    _dai();
    put_unsafe(ltyItem, lu32Cnt, lbOverwrite);
    _eai();
  }

  inline void put(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    put(ltyItem, lu32Cnt, True);
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

  // Liest nur die, Daten ohne aber den Ringbuffer zu verändern
  void read_unsafe(tycRingBufT* ltyItem, tycRingBufTIndex lu32Cnt)
  {
    tycRingBufTIndex luRemaining;

    // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
    // später mit '0' auffüllen
    if (lu32Cnt > mtyCnt)
    {
      luRemaining = lu32Cnt - mtyCnt;
      lu32Cnt = mtyCnt;
    }
    else
    {
      luRemaining = 0;
    }

    // Überprüfen, ob der gewünschte Block in einem Stück im Ringbuffer liegt ...
    // ..oder, ob er in 2 Teile geteilt ist.
    if ((mtyTail + lu32Cnt) < mtySize)
    {
      // Block liegt komplett vor. Dann den kompletten Block lesen
      cbRingBufT_memcpy((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32Cnt * sizeof(tycRingBufT));
    }
    else
    {
      // Block ist geteilt
      u32 lu32SplitCnt;
      lu32SplitCnt = mtySize - mtyTail;

      // Dann den ersten Teil lesen
      cbRingBufT_memcpy((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32SplitCnt * sizeof(tycRingBufT));
      lu32Cnt -= lu32SplitCnt;

      if (lu32Cnt > 0)
      {
        // Dann den zweiten Teil lesen
        cbRingBufT_memcpy((uint8*)&ltyItem[lu32SplitCnt], (uint8*)mtyBuf, lu32Cnt * sizeof(tycRingBufT));
      }
    }

    if (luRemaining)
    {
      // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
      // mit '0' auffüllen
      cbRingBufT_memset((uint8*)&ltyItem[mtyCnt], 0, luRemaining);
    }
  }

  inline tycRingBufT get(void)
  {
    tycRingBufT ltyZwerg;
    _dai();
    ltyZwerg = get_unsafe();
    _eai();

    return ltyZwerg;
  }

  /*
  // Liest die Daten aus dem Ringbuffer und erzeugt gleich eine Summe der gelesenen Daten
  u32 getwithsum_unsafe(tycRingBufT* ltyItem, tycRingBufTIndex lu32Cnt)
  {
    tycRingBufTIndex luRemaining;
    u32 u32Ret = 0;

    // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
    // später mit '0' auffüllen
    if (lu32Cnt > mtyCnt)
    {
      luRemaining = lu32Cnt - mtyCnt;
      lu32Cnt = mtyCnt;
    }
    else
    {
      luRemaining = 0;
    }

    // Überprüfen, ob der gewünschte Block in einem Stück im Ringbuffer liegt ...
    // ..oder, ob er in 2 Teile geteilt ist.
    if ((mtyTail + lu32Cnt) < mtySize)
    {
      // Block liegt komplett vor. Dann den kompletten Block lesen
      u32Ret = cbRingBufT_memcpy_sum((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32Cnt * sizeof(tycRingBufT));
      mtyTail += lu32Cnt;
      mtyCnt -= lu32Cnt;
    }
    else
    {
      // Block ist geteilt
      u32 lu32SplitCnt;

      mtyCnt -= lu32Cnt;
      lu32SplitCnt = mtySize - mtyTail;

      // Dann den ersten Teil lesen
      u32Ret = cbRingBufT_memcpy_sum((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32SplitCnt * sizeof(tycRingBufT));
      lu32Cnt -= lu32SplitCnt;

      if (lu32Cnt > 0)
      {
        // Dann den zweiten Teil lesen
        u32Ret += cbRingBufT_memcpy_sum((uint8*)&ltyItem[lu32SplitCnt], (uint8*)mtyBuf, lu32Cnt * sizeof(tycRingBufT));
      }
      mtyTail = lu32Cnt;
    }

    if (luRemaining)
    {
      // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
      // mit '0' auffüllen
      cbRingBufT_memset((uint8*)&ltyItem[mtyCnt], 0, luRemaining);
    }
    
    return u32Ret;
  }

  inline u32 getwithsum(tycRingBufT* ltyItem, tycRingBufTIndex lu32Cnt)
  {
    u32 u32Ret;
    _dai();
    u32Ret = getwithsum_unsafe(ltyItem, lu32Cnt);
    _eai();
    return u32Ret;
  }*/

  void get_unsafe(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    tycRingBufTIndex luRemaining;

    // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
    // später mit '0' auffüllen
    if (lu32Cnt > mtyCnt)
    {
      luRemaining = lu32Cnt - mtyCnt;
      lu32Cnt = mtyCnt;
    }
    else
    {
      luRemaining = 0;
    }

    // Überprüfen, ob der gewünschte Block in einem Stück im Ringbuffer liegt ...
    // ..oder, ob er in 2 Teile geteilt ist.
    if ((mtyTail + lu32Cnt) < mtySize)
    {
      // Block liegt komplett vor. Dann den kompletten Block lesen
      cbRingBufT_memcpy((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32Cnt * sizeof(tycRingBufT));
      mtyTail += lu32Cnt;
      mtyCnt  -= lu32Cnt;
    }
    else
    {
      // Block ist geteilt
      u32 lu32SplitCnt;

      mtyCnt -= lu32Cnt;
      lu32SplitCnt = mtySize - mtyTail;

      // Dann den ersten Teil lesen
      cbRingBufT_memcpy((uint8*)ltyItem, (uint8*)(&mtyBuf[mtyTail]), lu32SplitCnt * sizeof(tycRingBufT));
      lu32Cnt -= lu32SplitCnt;

      if (lu32Cnt > 0)
      {
        // Dann den zweiten Teil lesen
        cbRingBufT_memcpy((uint8*)&ltyItem[lu32SplitCnt], (uint8*)mtyBuf, lu32Cnt * sizeof(tycRingBufT));
      }
      mtyTail = lu32Cnt;
    }

    if (luRemaining)
    {
      // Falls der angeforderte Block größer ist, als was im Ringbuffer liegt, dann ...
      // mit '0' auffüllen
      cbRingBufT_memset((uint8*)&ltyItem[mtyCnt], 0, luRemaining);
    }
  }

  inline void get(tycRingBufT *ltyItem, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    get_unsafe(ltyItem, lu32Cnt);
    _eai();
  }

  void move_unsafe(cRingBufT *lcDest, tycRingBufTIndex lu32Cnt)
  {
    if (lu32Cnt > lcDest->space_left()) lu32Cnt = lcDest->space_left();
    if (lu32Cnt > cnt()) lu32Cnt = cnt();

    if ((mtyTail + lu32Cnt) < mtySize)
    {
      lcDest->put_unsafe((uint8*)(&mtyBuf[mtyTail]), lu32Cnt * sizeof(tycRingBufT));
      mtyTail += lu32Cnt;
      mtyCnt  -= lu32Cnt;
    }
    else
    {
      u32 lu32SplitCnt;

      mtyCnt -= lu32Cnt;
      lu32SplitCnt = mtySize - mtyTail;

      lcDest->put_unsafe((uint8*)(&mtyBuf[mtyTail]), lu32SplitCnt * sizeof(tycRingBufT));
      lu32Cnt -= lu32SplitCnt;

      if (lu32Cnt > 0)
      {
        lcDest->put_unsafe((uint8*)mtyBuf, lu32Cnt * sizeof(tycRingBufT));
      }

      mtyTail = lu32Cnt;
    }
  }

  inline void move(cRingBufT *lcDest, tycRingBufTIndex lu32Cnt)
  {
    _dai();
    move_unsafe(lcDest, lu32Cnt);
    _eai();
  }

  void vParseStart(tycRingBufTIndex &lui32Pos)
  {
    lui32Pos = mtyTail;
  }

  tycRingBufT tyParse(tycRingBufTIndex &lui32Pos)
  {
    lui32Pos++;
    if (lui32Pos > mtyHead) lui32Pos = mtyTail;
    return mtyBuf[lui32Pos];
  }
};




template <typename tycRingBufT, typename tycRingBufTIndex>
#ifdef __GNUC__
class __attribute__((packed)) cRingBufDynT : public cRingBufT<tycRingBufT, tycRingBufTIndex>
#else
class cRingBufDynT : public cRingBufT<tycRingBufT, tycRingBufTIndex>
#endif
{
public:
  cRingBufDynT(tycRingBufTIndex tycRingBufTSize)
    :  cRingBufT<tycRingBufT, tycRingBufTIndex>(tycRingBufTSize)
  {
    if (tycRingBufTSize)
    {
      cRingBufT<tycRingBufT, tycRingBufTIndex>::mtyBuf = (tycRingBufT*) new tycRingBufT[tycRingBufTSize];
    }
    else
    {
      cRingBufT<tycRingBufT, tycRingBufTIndex>::mtyBuf = null;
    }
  }

  inline uint32 SizeOf()
  {
    return cRingBufT<tycRingBufT, tycRingBufTIndex>::SizeOf();
  }

  ~cRingBufDynT()
  {
    delete []cRingBufT<tycRingBufT, tycRingBufTIndex>::mtyBuf;
  }
};


#endif //__CLASS_RING_BUFFER_H__
