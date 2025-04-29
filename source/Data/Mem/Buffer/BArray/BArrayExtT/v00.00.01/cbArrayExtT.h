
#ifndef __CLASS_BYTE_ARRAY_EXTENDED_H__
#define __CLASS_BYTE_ARRAY_EXTENDED_H__


#include "typedef.h"
#include "cbArrayT.h"

#define cbArrayExtT_pui8GETMEM(x)  (new uint8[x])
#define cbArrayExtT_vFREEMEM(x)    (delete[] x)
#define cbArrayExtT_vMemCpy        cMemTools::vMemCpy
#define cbArrayExtT_iMemCmp        cMemTools::iMemCmp

#define cbArrayExtT_UINTMAX        ((1 << (sizeof(tyArrayExtIdxT) * 8)) - 1)
                                // uint8 => (1 << (sizeof(uint8) * 8)) - 1
                                // uint8 => (1 << 8) - 1 => 255

enum cbArrayExtT_enErr
{
  cbArrayExtT_nOK            = FRET_OK,
  cbArrayExtT_nErr_No        = cbArrayExtT_nOK,
  cbArrayExtT_nErr_General   = FRET_ERR_GENERAL,
  cbArrayExtT_nErr_Parameter = FRET_ERR_PARAMETER,
  cbArrayExtT_nErr_OutofMem,
  cbArrayExtT_nErr_OutofBounds
};

template <typename tyArrayExtIdxT>
#ifdef __GNUC__
class __attribute__((packed)) cbArrayExtT : public cbArrayT<tyArrayExtIdxT>
#else
class cbArrayExtT : public cbArrayT<tyArrayExtIdxT>
#endif
{
  private:

  public:
    tyArrayExtIdxT  muiLen;

    cbArrayExtT()
    {
      cbArrayT<tyArrayExtIdxT>::mpu8Data = null;
      muiLen  = 0;
      cbArrayT<tyArrayExtIdxT>::muiSize = 0;
    };

    cbArrayExtT(uint8 *lpaArray, tyArrayExtIdxT luiSize)
    {
      cbArrayT<tyArrayExtIdxT>::mpu8Data = lpaArray;
      muiLen   = 0;
      cbArrayT<tyArrayExtIdxT>::muiSize  = luiSize;
    };

    cbArrayExtT(uint8 *lpaArray, tyArrayExtIdxT luiLen, tyArrayExtIdxT luiSize)
    {
      cbArrayT<tyArrayExtIdxT>::mpu8Data = lpaArray;
      muiLen   = luiLen;
      cbArrayT<tyArrayExtIdxT>::muiSize  = luiSize;
    };

    inline uint32 SizeOf()
    {
      return cbArrayT<tyArrayExtIdxT>::muiSize + sizeof(cbArrayExtT<tyArrayExtIdxT>);
    }


    inline tyArrayExtIdxT Len(void)
    {
      return this->muiLen;
    };

    inline void Clear()
    {
      muiLen = 0;
    };

    cbArrayExtT& Set(uint8 *lpaArray, tyArrayExtIdxT luiSize)
    {
      tyArrayExtIdxT luiFreeByteCnt;

      luiFreeByteCnt = cbArrayT<tyArrayExtIdxT>::muiSize;

      if (luiFreeByteCnt)
      {
        if (luiSize > luiFreeByteCnt)
        {
          luiSize = luiFreeByteCnt;
        }
        cbArrayExtT_vMemCpy(cbArrayT<tyArrayExtIdxT>::mpu8Data, lpaArray, luiSize);
        muiLen = luiSize;
      }
      return *this;
    };
    
    inline cbArrayExtT&  Set(uint8 lbByte)
    {
      if (cbArrayT<tyArrayExtIdxT>::muiSize)
      {
        cbArrayT<tyArrayExtIdxT>::mpu8Data[0] = lbByte;
        muiLen = 1;
      }
      return *this;
    };

    cbArrayExtT& Set(cbArrayExtT const &lcSource)
    {
      tyArrayExtIdxT luiFreeByteCnt;
      tyArrayExtIdxT luiSize;

      luiFreeByteCnt = cbArrayT<tyArrayExtIdxT>::muiSize;

      if (luiFreeByteCnt)
      {
        luiSize = lcSource.muiLen;
        if (luiSize > luiFreeByteCnt)
        {
          luiSize = luiFreeByteCnt;
        }
        cbArrayExtT_vMemCpy(cbArrayT<tyArrayExtIdxT>::mpu8Data, lcSource.mpu8Data, luiSize);
        muiLen = luiSize;
      }
      return *this;
    };

    uint8 operator[](tyArrayExtIdxT luiPos) const
    {
      if (luiPos < muiLen)
      {
        return cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos];
      }
      return 0;
    };

    uint8& operator[](tyArrayExtIdxT luiPos)
    {
      if (luiPos < cbArrayT<tyArrayExtIdxT>::muiSize)
      {
        if (luiPos >= muiLen) 
        {
          muiLen = luiPos + 1;
        }
        return cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos];
      }
      #ifdef _WIN32
        else
        {
          throw cbArrayExtT_nErr_OutofBounds;
        }
      #endif
      return cbArrayT<tyArrayExtIdxT>::mpu8Data[0];
    };

    cbArrayExtT& operator=(uint8 lui8Byte)
    {
      Set(lui8Byte);
      return *this;
    };

    cbArrayExtT& operator=(cbArrayExtT const &lpcbArray)
    {
      Set(lpcbArray);
      return *this;
    };

    cbArrayExtT& Add(uint8 *lpaArray, tyArrayExtIdxT luiSize)
    {
      tyArrayExtIdxT luiFreeByteCnt;

      luiFreeByteCnt = cbArrayT<tyArrayExtIdxT>::muiSize - muiLen;

      if (luiFreeByteCnt)
      {
        if (luiSize > luiFreeByteCnt)
        {
          luiSize = luiFreeByteCnt;
        }
        cbArrayExtT_vMemCpy(cbArrayT<tyArrayExtIdxT>::mpu8Data + muiLen, lpaArray, luiSize);
        muiLen += luiSize;
      }

      return *this;
    };


    cbArrayExtT& Add(uint8 lui8Byte)
    {
      tyArrayExtIdxT luiFreeByteCnt;

      luiFreeByteCnt = cbArrayT<tyArrayExtIdxT>::muiSize - muiLen;

      if (luiFreeByteCnt)
      {
        cbArrayT<tyArrayExtIdxT>::mpu8Data[muiLen] = lui8Byte;
        muiLen++;
      }

      return *this;
    };

    cbArrayExtT& Add(cbArrayExtT const &lpcbArray)
    {
      tyArrayExtIdxT luiFreeByteCnt;
      tyArrayExtIdxT luiSize;

      luiFreeByteCnt = cbArrayT<tyArrayExtIdxT>::muiSize - muiLen;

      if (luiFreeByteCnt)
      {
        luiSize = lpcbArray.muiLen;
        if (luiSize > luiFreeByteCnt)
        {
          luiSize = luiFreeByteCnt;
        }
        cbArrayExtT_vMemCpy(cbArrayT<tyArrayExtIdxT>::mpu8Data + muiLen, lpcbArray.mpu8Data, luiSize);
        muiLen += luiSize;
      }
      return *this;
    };
    
    cbArrayExtT& operator+=(uint8 lui8Byte)
    {
      Add(lui8Byte);
      return *this;
    };

    cbArrayExtT& operator+=(cbArrayExtT const &lpcbArray)
    {
      Add(lpcbArray);
      return *this;
    };

    cbArrayExtT& Insert(tyArrayExtIdxT luiPos, uint8 *lpaArray, tyArrayExtIdxT luiSize)
    {
      if (luiSize)
      {
        if (luiPos >= muiLen) 
        {
          Add(lpaArray, luiSize);
        }
        else
        {
          if ((luiSize + muiLen) > cbArrayT<tyArrayExtIdxT>::muiSize) luiSize = (cbArrayT<tyArrayExtIdxT>::muiSize - muiLen);

          if (luiSize > 0)
          {
            uint8 *lpui8Read   = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos]);
            uint8 *lpui8Write  = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos + luiSize]);
            uint8 *lpui8End    = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiSize + muiLen]);

            while (lpui8Write < lpui8End)
            {
              *lpui8Write = *lpui8Read;
              *lpui8Read  = *lpaArray;

              lpui8Write++;
              lpui8Read++;
              lpaArray++;
              luiSize--;
            }

            while ((luiSize) && (lpui8Read < lpui8End))
            {
              *lpui8Read  = *lpaArray;
              lpui8Read++;
              lpaArray++;
              luiSize--;
            }

            muiLen += luiSize;
          }
        }
      }
      return *this;
    }

    cbArrayExtT& Insert(tyArrayExtIdxT luiPos, uint8 lui8Byte)
    {
      if (cbArrayT<tyArrayExtIdxT>::muiSize)
      {
        if (luiPos >= muiLen) 
        {
          Add(lui8Byte);
        }
        else
        {
          if (muiLen < cbArrayT<tyArrayExtIdxT>::muiSize)
          {
            uint8 *lpui8Read   = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos]);
            uint8 *lpui8Write  = lpui8Read + 1;
            uint8 *lpui8End    = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[muiLen + 1]);

            /* Platz machen */
            while (lpui8Write < lpui8End)
            {
              *lpui8Write = *lpui8Read;

              lpui8Write++;
              lpui8Read++;
            }

            cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos] = lui8Byte;

            muiLen++;
          }
        }
      }
      return *this;
    }

    cbArrayExtT& Cut(tyArrayExtIdxT luiPos, tyArrayExtIdxT luiLen)
    {
      if (luiLen)
      {
        uint8  *lui8PtrWrite;
        uint8  *lui8PtrRead;
        tyArrayExtIdxT luiCnt;

        //Eigentlicher Funktionsrumpf für cbArrayExtT_nMid
        if ((luiPos >= 0) && (luiLen > 0) && (luiPos < muiLen)) 
        {
          //Ggf. Länge clippen
          if ((luiPos + luiLen) > muiLen)
          {
            luiLen = muiLen - luiPos;
          }

          luiCnt = muiLen - luiPos - luiLen;


          lui8PtrWrite = cbArrayT<tyArrayExtIdxT>::mpu8Data + luiPos;
          lui8PtrRead  = cbArrayT<tyArrayExtIdxT>::mpu8Data + luiPos + luiLen;

          //Daten kopieren
          while (luiCnt)
          {
            *lui8PtrWrite = *lui8PtrRead;
            lui8PtrWrite++;
            lui8PtrRead++;
            luiCnt--;
          }

          muiLen -= luiLen;
        }
      }

      return *this;
    }

    cbArrayExtT& Rev(void)
    {
      tyArrayExtIdxT luii;
      tyArrayExtIdxT luij;
      uint8 li8a;

      for (luii = 0, luij = muiLen - 1; luii < luij; luii++, luij--) 
      { 
        li8a = cbArrayT<tyArrayExtIdxT>::mpu8Data[luii]; 
        cbArrayT<tyArrayExtIdxT>::mpu8Data[luii] = cbArrayT<tyArrayExtIdxT>::mpu8Data[luij]; 
        cbArrayT<tyArrayExtIdxT>::mpu8Data[luij] = li8a; 
      }

      return *this;
    }

    cbArrayExtT& Replace(uint8 lui8ByteToFind, uint8 lui8ByteToSet)
    {
      uint8 *lszPtr;
      tyArrayExtIdxT luilen;

      if (muiLen > 0)
      {
        luilen = muiLen;
        lszPtr = cbArrayT<tyArrayExtIdxT>::mpu8Data;

        while (luilen)
        {
          if (*lszPtr == lui8ByteToFind) *lszPtr = lui8ByteToSet;
          lszPtr++;
          luilen--;
        }
      }
      return *this;
    }


    cbArrayExtT& Remove(uint8 lui8ByteToFind)
    {
      uint8 *lui8PtrWrite;
      uint8 *lui8PtrRead;
      tyArrayExtIdxT luilen;

      if (muiLen > 0)
      {
        luilen = muiLen;
        lui8PtrWrite = lui8PtrRead = cbArrayT<tyArrayExtIdxT>::mpu8Data;

        while (luilen)
        {
          if (*lui8PtrRead != lui8ByteToFind)
          {
            *lui8PtrWrite = *lui8PtrRead;
            lui8PtrWrite++;
          }
          lui8PtrRead++;
          luilen--;
        }    
        muiLen = lui8PtrWrite - cbArrayT<tyArrayExtIdxT>::mpu8Data;
      }
      return *this;
    }


    tyArrayExtIdxT CountOf(uint8 lui8ByteToFind)
    {
      uint8 *lui8PtrRead;
      tyArrayExtIdxT luilen;
      tyArrayExtIdxT luiCnt;

      luiCnt = 0;

      if (muiLen > 0)
      {
        luilen = muiLen - 1;
        lui8PtrRead = cbArrayT<tyArrayExtIdxT>::mpu8Data;

        while (luilen)
        {
          if (*lui8PtrRead == lui8ByteToFind)
          {
            luiCnt++;
          }
          lui8PtrRead++;
          luilen--;
        }
      }
      return luiCnt;
    }


    int8  Cmp(const cbArrayExtT &lcbArray)
    {
      int8 li8Ret;

      li8Ret = 1;

      if ((!lcbArray.muiLen) || (!muiLen)) return li8Ret;

      if (muiLen == lcbArray.muiLen)
      {
        li8Ret = cbArrayExtT_iMemCmp(cbArrayT<tyArrayExtIdxT>::mpu8Data, lcbArray.mpu8Data, muiLen);
      }

      return li8Ret;
    }

    bool  operator== (const cbArrayExtT &lcbArray)
    {
      return (this->Cmp(lcbArray) == 0);
    }

    tyArrayExtIdxT IndexOf(tyArrayExtIdxT luiPos, uint8 lui8ByteToFind)
    {
      for (;luiPos < muiLen; luiPos++)
      {
        if (cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos] == lui8ByteToFind)
        {
          return luiPos;
        }
      }

      return cbArrayExtT_UINTMAX;
    }

    tyArrayExtIdxT IndexOf(tyArrayExtIdxT luiPos, uint8* laui8Token, tyArrayExtIdxT luiSize)
    {
      uint8         *lpaArraying;

      if (luiPos >= muiLen) 
      {
        return cbArrayExtT_UINTMAX;
      }

      for (;luiPos <= (muiLen - luiSize); luiPos++)
      {
        lpaArraying = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos]);

        if (cbArrayExtT_iMemCmp(lpaArraying, laui8Token, luiSize) == 0)
        {
          return luiPos;
        }
      }
      return cbArrayExtT_UINTMAX;
    }

    tyArrayExtIdxT IndexOf(tyArrayExtIdxT luiPos, cbArrayExtT &lcToken)
    {
      uint8        *lpaArraying;
      tyArrayExtIdxT luiTokenLen;

      if (luiPos >= muiLen) 
      {
        return cbArrayExtT_UINTMAX;
      }

      luiTokenLen = lcToken.muiLen;

      for (;luiPos < (muiLen - luiTokenLen); luiPos++)
      {
        lpaArraying = &(cbArrayT<tyArrayExtIdxT>::mpu8Data[luiPos]);

        if (cbArrayExtT_iMemCmp(lpaArraying, lcToken.mpu8Data, luiTokenLen) == 0)
        {
          return luiPos;
        }
      }
      return cbArrayExtT_UINTMAX;
    }


    /* Byte-Stream Interface */
    inline uint32 u32SizeOf()
    {
      return (sizeof(tyArrayExtIdxT) + muiLen);
    }

    void vToByteArray(uint8* laui8ArrayExt)
    {
      *((tyArrayExtIdxT*)laui8ArrayExt) = muiLen;
      ((tyArrayExtIdxT*)laui8ArrayExt)++;
      cbArrayExtT_vMemCpy(laui8ArrayExt, cbArrayT<tyArrayExtIdxT>::mpu8Data, muiLen);
    }

    void vFromByteArray(uint8* laui8ArrayExt)
    {
      muiLen = *((tyArrayExtIdxT*)laui8ArrayExt);
      if (muiLen > cbArrayT<tyArrayExtIdxT>::muiSize) muiLen = cbArrayT<tyArrayExtIdxT>::muiSize;
      ((tyArrayExtIdxT*)laui8ArrayExt)++;
      cbArrayExtT_vMemCpy(cbArrayT<tyArrayExtIdxT>::mpu8Data, laui8ArrayExt, muiLen);
    }

    void vParseStart(uint32 &lui32Pos)
    {
      lui32Pos = 0;
    }

    uint8 u8Parse(uint32 &lui32Pos)
    {
      lui32Pos++;
      if (lui32Pos >= muiLen) lui32Pos = 0;
      return cbArrayT<tyArrayExtIdxT>::mpu8Data[lui32Pos];
    }
};

template <typename tyArrayExtIdxT>
class cbArrayExtStaticT: public cbArrayExtT<tyArrayExtIdxT>
{
  public:
    cbArrayExtStaticT(uint8* maui8bArray, tyArrayExtIdxT ltySize)
    : cbArrayExtT<tyArrayExtIdxT>(maui8bArray, ltySize)
    {
    }
};

template <typename tyArrayExtIdxT>
class cbArrayExtDynT: public cbArrayExtT<tyArrayExtIdxT>
{
  private:

  public:
    cbArrayExtDynT()
    {
      cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
      Create(0);
    }

    cbArrayExtDynT(tyArrayExtIdxT luiArrayMaxSize)
    {
      cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
      Create(luiArrayMaxSize);
    }

    cbArrayExtDynT(cbArrayExtDynT<tyArrayExtIdxT> const& lcbArray)
    {
      cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
      Create(lcbArray.muiSize);
      Set(lcbArray);
    }

    cbArrayExtDynT(uint8 *lpaArray, tyArrayExtIdxT luiSize)
    {
      cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
      Create(luiSize);
      Set(lpaArray, luiSize);
    }

    ~cbArrayExtDynT()
    {
      if (cbArrayExtT<tyArrayExtIdxT>::mpu8Data != null)
      {
        cbArrayExtT_vFREEMEM(cbArrayExtT<tyArrayExtIdxT>::mpu8Data);
        cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
      }
    }

    inline uint32 SizeOf()
    {
      return cbArrayExtT<tyArrayExtIdxT>::SizeOf();
    }

    void  Create(tyArrayExtIdxT luiArrayLen)
    {
      if (luiArrayLen > 0)
      {
        #ifdef _WIN32
        if (luiArrayLen > (cbArrayExtT_UINTMAX-1))
        {
          throw cbArrayExtT_nErr_Parameter;
        }
        #endif

        //Eigentlicher Funktionsrumpf für Reuse
        // Falls der bestehende String zu klein ist, wieder löschen und neu anlegen
        if ((cbArrayExtT<tyArrayExtIdxT>::mpu8Data != null) && 
            (cbArrayExtT<tyArrayExtIdxT>::muiSize < luiArrayLen))
        {
          this->~cbArrayExtDynT();
        }

        if (cbArrayExtT<tyArrayExtIdxT>::muiSize == 0)
        {
          cbArrayExtT<tyArrayExtIdxT>::mpu8Data = (uint8*)cbArrayExtT_pui8GETMEM(luiArrayLen);

          if (cbArrayExtT<tyArrayExtIdxT>::mpu8Data != null)
          {
            // Speicher konnte angefordert werden
            cbArrayExtT<tyArrayExtIdxT>::muiSize     = luiArrayLen;
            cbArrayExtT<tyArrayExtIdxT>::muiLen      = 0;
          }
          #ifdef _WIN32
          else
          {
            // Falls kein Speicher angefordert werden konnte
            throw cbArrayExtT_nErr_OutofMem;
          }
          #endif
        }
      }
      else
      {
        this->~cbArrayExtDynT();
        cbArrayExtT<tyArrayExtIdxT>::mpu8Data = null;
        cbArrayExtT<tyArrayExtIdxT>::muiLen  = 0;
        cbArrayExtT<tyArrayExtIdxT>::muiSize = 0;
      }
    }

    cbArrayExtDynT Mid(tyArrayExtIdxT luiPos, tyArrayExtIdxT luiLen)
    {
      cbArrayExtDynT<tyArrayExtIdxT> lcReturn(cbArrayExtT<tyArrayExtIdxT>::muiSize);

      //Eigentlicher Funktionsrumpf für cbArrayExtT_nMid
      if ((luiPos >= 0) && (luiLen > 0) && (luiPos < cbArrayExtT<tyArrayExtIdxT>::muiLen)) 
      {
        //Ggf. Länge clippen
        if ((luiPos + luiLen) > cbArrayExtT<tyArrayExtIdxT>::muiLen)
        {
          luiLen = cbArrayExtT<tyArrayExtIdxT>::muiLen - luiPos;
        }

         //Daten kopieren
        cbArrayExtDynT_memcpy(lcReturn.mpu8Data, &(cbArrayExtT<tyArrayExtIdxT>::mpu8Data[luiPos]), luiLen);
        lcReturn.muiLen = luiLen;
      }

      return lcReturn;
    }
};


template <typename tyArrayExtIdxT>
cbArrayExtDynT<tyArrayExtIdxT> operator+(const cbArrayExtDynT<tyArrayExtIdxT>& lpcbArray1, const cbArrayExtDynT<tyArrayExtIdxT>& lpcbArray2)
{
  cbArrayExtDynT<tyArrayExtIdxT> lcTemp(lpcbArray1);
  lcTemp += lpcbArray2;
  return lcTemp;
}

template <typename tyArrayExtIdxT>
cbArrayExtDynT<tyArrayExtIdxT> operator+(const cbArrayExtDynT<tyArrayExtIdxT>& lpcbArray1, uint8* lpcbArray2)
{
  cbArrayExtDynT<tyArrayExtIdxT> lcTemp(lpcbArray1);
  lcTemp += lpcbArray2;
  return lcTemp;
}

template <typename tyArrayExtIdxT>
cbArrayExtDynT<tyArrayExtIdxT> operator+(uint8* lpcbArray1, const cbArrayExtDynT<tyArrayExtIdxT>& lpcbArray2)
{
  cbArrayExtDynT<tyArrayExtIdxT> lcTemp(lpcbArray1);
  lcTemp += lpcbArray2;
  return lcTemp;
}

/*
class cbArrayExtTest
{
  public:
  cbArrayExtTest()
  {
    uint8 data[] = {1,2,3,4,5};

    cbArrayExtDynT<uint16> mcTest1(4);
    cbArrayExtDynT<uint16> mcTest2(8);
    cbArrayExtDynT<uint16> mcTest3(data, 5);
    cbArrayExtDynT<uint16> mcTest4(mcTest3);
    cbArrayExtDynT<uint16> mcTest5(16);

    mcTest3 += mcTest3;
    mcTest5  = mcTest2 + mcTest4;
    mcTest5 += mcTest4[0];

    mcTest2 = mcTest4;

    if (mcTest2 == mcTest4)
    {
      if (mcTest3 == mcTest4)
      {
        return;
      }
    }

    mcTest2[2] = 8;
    if (mcTest2 == mcTest4)
    {
      return;
    }

  }
};*/


#endif //__CLASS_BYTE_ARRAY_EXTENDED_H__
