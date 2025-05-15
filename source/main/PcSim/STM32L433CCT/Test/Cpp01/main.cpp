#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "cStrT.h"


#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

template <typename tybArrayIdxT>
class cbArrayT2
{
private:

public:
  tybArrayIdxT  muiSize;
  uint8* mpu8Data;

  cbArrayT2()
  {
    mpu8Data = null;
    muiSize = 0;
  };
};

template <typename tyArrayExtIdxT>
class cbArrayExtT2 : public cbArrayT2<tyArrayExtIdxT>
{
private:

public:
  tyArrayExtIdxT  muiLen;

  cbArrayExtT2()
  {
    cbArrayT2<tyArrayExtIdxT>::mpu8Data = null;
    muiLen = 0;
    cbArrayT2<tyArrayExtIdxT>::muiSize = 0;
  };
};

template <typename tyCStrT>
class cStrBaseT2 : public cbArrayExtT2<tyCStrT>
{
public:
  cStrBaseT2()
  {
    cbArrayExtT2<tyCStrT>::muiLen = 1;
    cbArrayT2<tyCStrT>::muiSize = 2;
    cbArrayT2<tyCStrT>::mpu8Data = null;
  };
};



template <typename tybArrayIdxT>
class cbArrayT3
{
private:

public:
  cbArrayT3(u8* lu8Mem)
  {
    (u8*)&this = 0;
    vSetSize(0);
  };

  cbArrayT3()
  {
  };

  void vSetSize(tybArrayIdxT lSize)
  {
    *((tybArrayIdxT*)this) = lSize;
  }

  tybArrayIdxT uGetSize()
  {
    return *((tybArrayIdxT*)this)
  }

  virtual u8* u8GetData()
  {
    return (((u8*)this) + sizeof(tybArrayIdxT));
  }
};

template <typename tyArrayExtIdxT>
class cbArrayExtT3 : public cbArrayT3<tyArrayExtIdxT>
{
private:

public:

  cbArrayExtT3(u8* lu8Mem)
    : cbArrayT3(lu8Mem)
  {
    vSetLen(0);
  };

  cbArrayExtT3()
    : cbArrayT3()
  {
  };


  void vSetLen(tyArrayExtIdxT lLen)
  {
    *(((tyArrayExtIdxT*)this) + 1) = lLen;
  }

  tyArrayExtIdxT uGetLen()
  {
    return *(((tyArrayExtIdxT*)this) + 1);
  }

  u8* u8GetData() override
  {
    return (((u8*)this) + sizeof(tyArrayExtIdxT) * 2);
  }
};

template <typename tyCStrT>
class cStrBaseT3 : public cbArrayExtT3<tyCStrT>
{
public:
  cStrBaseT3(u8* lu8Mem)
    : cbArrayExtT3(lu8Mem)
  {
    vSetSize(0x1234);
    vSetLen(0x5678);
  };

  cStrBaseT3()
    : cbArrayExtT3()
  {
  };
};


void main(void)
{
  cStrBaseT2<u16> lcTest;

  u8     lcTest3_Buf[32];
  cStrBaseT3<u16> *lcTest3 = (cStrBaseT3<u16>*)lcTest3_Buf;

  cStrBaseT3<u16> lcTest4((u8*)lcTest3_Buf);

  lcTest3->vSetSize(sizeof(lcTest3_Buf) - 4);
  lcTest3->vSetLen(0x1234);


  cStr_Create(lszStr, 32);

  lszStr.Setf((rsz)"cStrT size: %d\r\n", sizeof(lszStr));
  printf(lszStr.ToString());
  
  lszStr.Setf((rsz)"cStrT2 size: %d\r\n", sizeof(lcTest));
  printf(lszStr.ToString());

  lszStr.Setf((rsz)"cStrT3 size: %d\r\n", sizeof(lcTest3));
  printf(lszStr.ToString());
}

