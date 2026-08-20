
#ifndef __CLASS_BYTE_ARRAY_H__
#define __CLASS_BYTE_ARRAY_H__

#include "typedef.h"
#include "MemTools.h"
#include <type_traits>


//#define cBarryPtrT_pu8GETMEM(x)  (new u8[x])
//#define cBarryPtrT_vFREEMEM(x)    (delete[] x)
#define cBarryPtrT_vMemCpy        cMemTools::vMemCpy
#define cBarryPtrT_iMemCmp        cMemTools::iMemCmp

#define cBarryPtrT_UINTMAX        ((1 << (sizeof(tyTyp) * 8)) - 1)
// u8 => (1 << (sizeof(u8) * 8)) - 1
// u8 => (1 << 8) - 1 => 255

enum cBarryPtrT_enErr
{
  cBarryPtrT_nOK = FRET_OK,
  cBarryPtrT_nErr_No = cBarryPtrT_nOK,
  cBarryPtrT_nErr_General = FRET_ERR_GENERAL,
  cBarryPtrT_nErr_Parameter = FRET_ERR_PARAMETER,
  cBarryPtrT_nErr_OutofMem,
  cBarryPtrT_nErr_OutofBounds
};

template <typename tyTyp>
class cBarryPtrT
{
private:

public:
  u8*    mpu8Data;
  tyTyp  muLen;
  tyTyp  muSize;

  cBarryPtrT()
  {
    From(null, 0, 0);
  };

  cBarryPtrT(u8* lpaArray, tyTyp luSize)
  {
    From(lpaArray, 0, luSize);
  };

  cBarryPtrT(u8* lpaArray, tyTyp luLen, tyTyp luSize)
  {
    From(lpaArray, luLen, luSize);
  };

  cBarryPtrT(cBarryPtrT<u8>& lcBAryP)
  {
    From(lcBAryP);
  };


  cBarryPtrT(cBarryPtrT<u16>& lcBAryP)
  {
    From(lcBAryP);
  };

  void From(cBarryPtrT<u8>& lcBAryP)
  {
    From(lcBAryP.mpu8Data, lcBAryP.muLen, lcBAryP.muSize);
  };


  void From(cBarryPtrT<u16>& lcBAryP)
  {
    if constexpr (std::is_same_v<tyTyp, u16>)
    {
      u32* lThis = (u32*)this;
      u32* lSrc  = (u32*)&lcBAryP;
      *lThis++   = *lSrc++;
      *lThis     = *lSrc;
    }
    else
    {
      From(lcBAryP.mpu8Data, lcBAryP.muLen, lcBAryP.muSize);
    }
  };

  void From(u8* lpaArray, tyTyp luLen, tyTyp luSize)
  {
    mpu8Data = lpaArray;
    muLen    = luLen;
    muSize   = luSize;
  };

  inline void Len(tyTyp luLen)
  {
    muLen = luLen;
  };

  inline tyTyp Size(void)
  {
    return muSize;
  };

  inline tyTyp Len(void)
  {
    return muLen;
  };

  inline u8* Data(void)
  {
    return mpu8Data;
  };

  inline void Clear()
  {
    muLen = 0;
  };

  cBarryPtrT& Set(const u8* lpaArray, tyTyp luLen)
  {
    if (luLen > muSize) muLen = muSize;  // Source is to big -> Clip
                   else muLen = luLen; // Source fits
    cBarryPtrT_vMemCpy(mpu8Data, (u8*)lpaArray, muLen);
    return *this;
  };

  cBarryPtrT& Set(cBarryPtrT<u8> const& lcData)
  {
    return Set(lcData.mpu8Data, lcData.muLen);
  }

  cBarryPtrT& Set(cBarryPtrT<u16> const& lcData)
  {
    return Set(lcData.mpu8Data, lcData.muLen);
  };

  cBarryPtrT& Set(u8 lu8Byte)
  {
    mpu8Data[0] = lu8Byte;
    muLen      = 1;
    return *this;
  };

  cBarryPtrT& Add(u8* lpaArray, tyTyp luLen)
  {
    if (muLen < muSize)
    {
      tyTyp luLenOld = muLen;
      if ((luLen + muLen) > muSize) muLen = muSize;           // Source is to big -> Clip
                                 else muLen = (luLen + muLen); // Source fits
      cBarryPtrT_vMemCpy(mpu8Data + luLenOld, lpaArray, muLen - luLenOld);
    }
    return *this;
  };

  cBarryPtrT& Add(cBarryPtrT<u8> const& lcData)
  {
    return Add(lcData.mpu8Data, lcData.muLen);
  }

  cBarryPtrT& Add(cBarryPtrT<u16> const& lcData)
  {
    return Add(lcData.mpu8Data, lcData.muLen);
  };

  cBarryPtrT& Add(u8 lu8Byte)
  {
    if (muLen < muSize)
    {
      mpu8Data[muLen] = lu8Byte;
      muLen++;
    }
    return *this;
  };


  u8 operator[](tyTyp luiPos) const
  {
    if (luiPos < muLen)
    {
      return mpu8Data[luiPos];
    }
    return 0;
  };

  u8& operator[](tyTyp luiPos)
  {
    if (luiPos < muSize)
    {
      if (luiPos >= muLen)
      {
        muLen = luiPos + 1;
      }
      return mpu8Data[luiPos];
    }
    #ifdef _WIN32
      else
      {
        throw cBarryPtrT_nErr_OutofBounds;
      }
    #endif
    return mpu8Data[0];
  };

  cBarryPtrT& operator=(u8 lu8Byte)                     {return Set(lu8Byte);};
  cBarryPtrT& operator=(cBarryPtrT<u8> const& lcData)   {return Set(lcData);};
  cBarryPtrT& operator=(cBarryPtrT<u16> const& lcData)  {return Set(lcData);};

  cBarryPtrT& operator+=(u8 lu8Byte)                    {return Add(lu8Byte);};
  cBarryPtrT& operator+=(cBarryPtrT<u8> const& lcData)  {return Add(lcData);};
  cBarryPtrT& operator+=(cBarryPtrT<u16> const& lcData) {return Add(lcData);};


  cBarryPtrT& Insert(tyTyp luiPos, u8* lpaArray, tyTyp luLen)
  {
    if (luLen)
    {
      if (luiPos >= muLen)
      {
        Add(lpaArray, luLen);
      }
      else
      {
        if ((luLen + muLen) > muSize) luLen = (muSize - muLen);

        if (luLen > 0)
        {
          u8* lpu8Read = &(mpu8Data[luiPos]);
          u8* lpu8Write = &(mpu8Data[luiPos + luLen]);
          u8* lpu8End = &(mpu8Data[luLen + muLen]);

          while (lpu8Write < lpu8End)
          {
            *lpu8Write = *lpu8Read;
            *lpu8Read = *lpaArray;

            lpu8Write++;
            lpu8Read++;
            lpaArray++;
            luLen--;
          }

          while ((luLen) && (lpu8Read < lpu8End))
          {
            *lpu8Read = *lpaArray;
            lpu8Read++;
            lpaArray++;
            luLen--;
          }

          muLen += luLen;
        }
      }
    }
    return *this;
  }

  cBarryPtrT& Insert(tyTyp luiPos, u8 lu8Byte)
  {
    return Insert(luiPos, &lu8Byte, 1);
  }

  cBarryPtrT& Cut(tyTyp luiPos, tyTyp luLen)
  {
    if (luLen)
    {
      u8* lu8PtrWrite;
      u8* lu8PtrRead;
      tyTyp luiCnt;

      //Eigentlicher Funktionsrumpf für cBarryPtrT_nMid
      if ((luiPos >= 0) && (luLen > 0) && (luiPos < muLen))
      {
        //Ggf. Länge clippen
        if ((luiPos + luLen) > muLen)
        {
          luLen = muLen - luiPos;
        }

        luiCnt = muLen - luiPos - luLen;

        lu8PtrWrite = mpu8Data + luiPos;
        lu8PtrRead = mpu8Data + luiPos + luLen;

        //Daten kopieren
        while (luiCnt)
        {
          *lu8PtrWrite = *lu8PtrRead;
          lu8PtrWrite++;
          lu8PtrRead++;
          luiCnt--;
        }

        muLen -= luLen;
      }
    }

    return *this;
  }

  cBarryPtrT& Rev(void)
  {
    tyTyp luii;
    tyTyp luij;
    u8 li8a;

    for (luii = 0, luij = muLen - 1; luii < luij; luii++, luij--)
    {
      li8a = mpu8Data[luii];
      mpu8Data[luii] = mpu8Data[luij];
      mpu8Data[luij] = li8a;
    }
    return *this;
  }

  cBarryPtrT& Replace(u8 lu8ByteToFind, u8 lu8ByteToSet)
  {
    u8* lszPtr;
    tyTyp luLen;

    if (muLen > 0)
    {
      luLen = muLen;
      lszPtr = mpu8Data;

      while (luLen)
      {
        if (*lszPtr == lu8ByteToFind) *lszPtr = lu8ByteToSet;
        lszPtr++;
        luLen--;
      }
    }
    return *this;
  }


  cBarryPtrT& Remove(u8 lu8ByteToFind)
  {
    u8* lu8PtrWrite;
    u8* lu8PtrRead;
    tyTyp luLen;

    if (muLen > 0)
    {
      luLen = muLen;
      lu8PtrWrite = lu8PtrRead = mpu8Data;

      while (luLen)
      {
        if (*lu8PtrRead != lu8ByteToFind)
        {
          *lu8PtrWrite = *lu8PtrRead;
          lu8PtrWrite++;
        }
        lu8PtrRead++;
        luLen--;
      }
      muLen = lu8PtrWrite - mpu8Data;
    }
    return *this;
  }


  tyTyp CountOf(u8 lu8ByteToFind)
  {
    u8* lu8PtrRead;
    tyTyp luLen;
    tyTyp luiCnt;

    luiCnt = 0;

    if (muLen > 0)
    {
      luLen = muLen - 1;
      lu8PtrRead = mpu8Data;

      while (luLen)
      {
        if (*lu8PtrRead == lu8ByteToFind)
        {
          luiCnt++;
        }
        lu8PtrRead++;
        luLen--;
      }
    }
    return luiCnt;
  }


  int8  Cmp(const cBarryPtrT& lcbArray)
  {
    int8 li8Ret;

    li8Ret = 1;

    if ((!lcbArray.muLen) || (!muLen)) return li8Ret;

    if (muLen == lcbArray.muLen)
    {
      li8Ret = cBarryPtrT_iMemCmp(mpu8Data, lcbArray.mpu8Data, muLen);
    }
    return li8Ret;
  }

  bool  operator== (const cBarryPtrT& lcbArray)
  {
    return (this->Cmp(lcbArray) == 0);
  }

  tyTyp IndexOf(tyTyp luiPos, u8 lu8ByteToFind)
  {
    for (; luiPos < muLen; luiPos++)
    {
      if (mpu8Data[luiPos] == lu8ByteToFind)
      {
        return luiPos;
      }
    }
    return cBarryPtrT_UINTMAX;
  }

  tyTyp IndexOf(tyTyp luiPos, u8* lau8Token, tyTyp luLen)
  {
    u8* lpaArraying;

    if (luiPos >= muLen)
    {
      return cBarryPtrT_UINTMAX;
    }

    for (; luiPos <= (muLen - luLen); luiPos++)
    {
      lpaArraying = &(mpu8Data[luiPos]);

      if (cBarryPtrT_iMemCmp(lpaArraying, lau8Token, luLen) == 0)
      {
        return luiPos;
      }
    }
    return cBarryPtrT_UINTMAX;
  }

  tyTyp IndexOf(tyTyp luiPos, cBarryPtrT& lcToken)
  {
    return IndexOf(luiPos, lcToken.mpu8Data, lcToken.muLen);
  }


  void vSerialize(u8* lau8ArrayExt)
  {
    *((tyTyp*)lau8ArrayExt) = muLen;
    ((tyTyp*)lau8ArrayExt)++;
    cBarryPtrT_vMemCpy(lau8ArrayExt, mpu8Data, muLen);
  }

  void vDeSerialize(u8* lau8ArrayExt)
  {
    muLen = *((tyTyp*)lau8ArrayExt);
    if (muLen > muSize) muLen = muSize;
    ((tyTyp*)lau8ArrayExt)++;
    cBarryPtrT_vMemCpy(mpu8Data, lau8ArrayExt, muLen);
  }

  void vParseStart(cBarryPtrT& luiPos)
  {
    luiPos = 0;
  }

  u8 u8Parse(cBarryPtrT& luiPos)
  {
    u8 lu8Data = mpu8Data[luiPos];
    luiPos++;
    if (luiPos >= muLen) luiPos = 0;
    return lu8Data;
  }

  u32 u32Sum()
  {
    u32   luChkSum;
    u8*   lu8Buf = mpu8Data;
    tyTyp luCnt;

    luChkSum = 0;
    luCnt = muLen;

    while (luCnt > 4)
    {
      luChkSum += *lu8Buf++;
      luChkSum += *lu8Buf++;
      luChkSum += *lu8Buf++;
      luChkSum += *lu8Buf++;
      luCnt -= 4;
    }
    while (luCnt)
    {
      luChkSum += *lu8Buf++;
      luCnt--;
    }
    return luChkSum;
  }

  u8 u8Sum()
  {
    return (u8)u32Sum();
  }
};


template <typename tyTyp, tyTyp tySize>
class SBArry
{
public:
  tyTyp mLen;
  u8 mData[tySize];

  SBArry()
  {
    mLen = 0;
  }

  SBArry(const u8* lau8aData, tyTyp luLen)
  {
    Set(lau8aData, luLen);
  }

  SBArry& Set(const u8* lau8aData, tyTyp luLen)
  {
    cBarryPtrT<tyTyp> lcBArryP(*this->Get());
    lcBArryP.Set(lau8aData, luLen);
    mLen = lcBArryP.muLen;
    return *this;
  }

  SBArry& Set(cBarryPtrT<u8> const& lcData)
  {
    return Set(lcData.mpu8Data, lcData.muLen);
  }

  SBArry& Set(cBarryPtrT<u16> const& lcData)
  {
    return Set(lcData.mpu8Data, lcData.muLen);
  }

  SBArry& Set(u8 luData)
  {
    mData[0] = luData;
    mLen     = 1;
    return *this;
  }

  SBArry& Add(const u8* lau8aData, tyTyp luLen)
  {
    cBarryPtrT<tyTyp> lcBArryP(*this->Get());
    lcBArryP.Add(lau8aData, luLen);
    mLen = lcBArryP.muLen;
    return *this;
  }

  SBArry& Add(cBarryPtrT<u16> const& lcData)
  {
    return Add(lcData.mpu8Data, lcData.muLen);
  }

  SBArry& Add(cBarryPtrT<u8> const& lcData)
  {
    return Add(lcData.mpu8Data, lcData.muLen);
  }

  SBArry& Add(u8 luData)
  {
    if (mLen < tySize)
    {
      mData[mLen] = luData;
      mLen++;
    }
    return *this;
  }

  bool Contains(u8 lu8ByteToFind)
  {
    tyTyp luiPos = 0;
    for (; luiPos < mLen; luiPos++)
    {
      if (mData[luiPos] == lu8ByteToFind)
      {
        return True;
      }
    }
    return False;
  }

  void vRemove(u8 lu8ByteToFind)
  {
    u8* lu8PtrWrite;
    u8* lu8PtrRead;
    tyTyp luLen;

    if (mLen > 0)
    {
      luLen = mLen;
      lu8PtrWrite = lu8PtrRead = mData;

      while (luLen)
      {
        if (*lu8PtrRead != lu8ByteToFind)
        {
          *lu8PtrWrite = *lu8PtrRead;
          lu8PtrWrite++;
        }
        lu8PtrRead++;
        luLen--;
      }
      mLen = lu8PtrWrite - mData;
    }
  }

  tyTyp Size()
  {
    return tySize;
  }

  SBArry& operator=(u8 lu8Byte)                            {return Set(lu8Byte);};
  SBArry& operator=(cBarryPtrT<u8> const& lcData)          {return Set(lcData);};
  SBArry& operator=(cBarryPtrT<u16> const& lcData)         {return Set(lcData);};

  SBArry& operator+=(u8 lu8Byte)                           {return Add(lu8Byte);};
  SBArry& operator+=(cBarryPtrT<u8> const& lcData)         {return Add(lcData);};
  SBArry& operator+=(cBarryPtrT<u16> const& lcData)        {return Add(lcData);};
};


#endif //__CLASS_BYTE_ARRAY_H__
