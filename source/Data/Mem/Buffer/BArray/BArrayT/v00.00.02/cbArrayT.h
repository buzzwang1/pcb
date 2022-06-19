
#ifndef __CLASS_BYTE_ARRAY_H__
#define __CLASS_BYTE_ARRAY_H__


#include "typedef.h"
#include "MemTools.h"

#define cbArrayT_pui8GETMEM(x)  (new uint8[x])
#define cbArrayT_vFREEMEM(x)    (delete[] x)
#define cbArrayT_memcpy         MEMTOOLS_vMemCpy
#define cbArrayT_memcmp         MEMTOOLS_iMemCmp

#define cbArrayT_UINTMAX        ((1 << (sizeof(tybArrayIdxT) * 8)) - 1)
                                // uint8 => (1 << (sizeof(uint8) * 8)) - 1
                                // uint8 => (1 << 8) - 1 => 255

enum cbArrayT_enErr
{
  cbArrayT_nOK            = FRET_OK,
  cbArrayT_nErr_No        = cbArrayT_nOK,
  cbArrayT_nErr_General   = FRET_ERR_GENERAL,
  cbArrayT_nErr_Parameter = FRET_ERR_PARAMETER,
  cbArrayT_nErr_OutofMem,
  cbArrayT_nErr_OutofBounds
};

template <typename tybArrayIdxT>
#ifdef __GNUC__
class __attribute__((packed)) cbArrayT 
#else
class cbArrayT
#endif
{
  private:

  public:
    tybArrayIdxT  muiSize;
    uint8         *mpu8Data;

    cbArrayT()
    {
      mpu8Data = null;
      muiSize  = 0;
    };

    cbArrayT(uint8 *lpaArray, tybArrayIdxT luiSize)
    {
      mpu8Data = lpaArray;
      muiSize = luiSize;
    };

    inline uint32 SizeOf()
    {
      return muiSize + sizeof(cbArrayT<tybArrayIdxT>);
    }

    tybArrayIdxT Size(void)
    {
      return this->muiSize;
    };

    cbArrayT& Set(uint8 *lpaArray, tybArrayIdxT luiSize)
    {
      if (luiSize > muiSize) luiSize = muiSize;
      cbArrayT_memcpy(mpu8Data, lpaArray, luiSize);
      return *this;
    };


    cbArrayT&  Set(cbArrayT const &lcSource)
    {
      tybArrayIdxT luiSize = lcSource->muiSize;
      return Set(lcSource->mpu8Data, luiSize);
    };

    uint8 operator[](tybArrayIdxT luiPos) const
    {
      if (luiPos < muiSize)
      {
        return mpu8Data[luiPos];
      }
      return 0;
    };

    uint8& operator[](tybArrayIdxT luiPos)
    {
      if (luiPos < muiSize)
      {
        return mpu8Data[luiPos];
      }
      return mpu8Data[0];
    };

    cbArrayT& operator=(cbArrayT const &lpcArray)
    {
      return Set(lpcArray);
    };

    void vRev(void)
    {
      tybArrayIdxT luii;
      tybArrayIdxT luij;
      uint8 li8a;

      for (luii = 0, luij = muiSize - 1; luii < luij; luii++, luij--) 
      { 
        li8a = mpu8Data[luii]; 
        mpu8Data[luii] = mpu8Data[luij]; 
        mpu8Data[luij] = li8a; 
      }
    }

    void vReplace(uint8 lui8ByteToFind, uint8 lui8ByteToSet)
    {
      uint8 *lszPtr;
      tybArrayIdxT luiSize;

      if (muiSize > 0)
      {
        luiSize = muiSize;
        lszPtr = mpu8Data;

        while (luiSize)
        {
          if (*lszPtr == lui8ByteToFind) *lszPtr = lui8ByteToSet;
          lszPtr++;
          luiSize--;
        }
      }
    }

    tybArrayIdxT CountOf(uint8 lui8ByteToFind)
    {
      uint8 *lui8PtrRead;
      tybArrayIdxT luiSize;
      tybArrayIdxT luiCnt;

      luiCnt = 0;

      if (muiSize > 0)
      {
        luiSize = muiSize - 1;
        lui8PtrRead = mpu8Data;

        while (luiSize)
        {
          if (*lui8PtrRead == lui8ByteToFind)
          {
            luiCnt++;
          }
          lui8PtrRead++;
          luiSize--;
        }
      }
      return luiCnt;
    }


    int8  Cmp(const cbArrayT &lcArray)
    {
      int8 li8Ret;

      li8Ret = 1;

      if ((!lcArray.muiSize) || (!muiSize)) return li8Ret;

      if (muiSize == lcArray.muiSize)
      {
        li8Ret = cbArrayT_memcmp(mpu8Data, lcArray.mpu8Data, muiSize);
      }

      return li8Ret;
    }

    bool  operator== (const cbArrayT &lcArray)
    {
      return (this->Cmp(lcArray) == 0);
    }

    tybArrayIdxT IndexOf(tybArrayIdxT luiPos, uint8 lui8ByteToFind)
    {
      for (;luiPos < muiSize; luiPos++)
      {
        if (mpu8Data[luiPos] == lui8ByteToFind)
        {
          return luiPos;
        }
      }

      return cbArrayT_UINTMAX;
    }

    tybArrayIdxT IndexOf(tybArrayIdxT luiPos, uint8* laui8Token, tybArrayIdxT luiSize)
    {
      uint8         *lpaArraying;

      if (luiPos >= muiSize) 
      {
        return cbArrayT_UINTMAX;
      }

      for (;luiPos < (muiSize - luiSize); luiPos++)
      {
        lpaArraying = &(mpu8Data[luiPos]);

        if (cbArrayT_memcmp(lpaArraying, laui8Token, luiSize) == 0)
        {
          return luiPos;
        }
      }
      return cbArrayT_UINTMAX;
    }

    tybArrayIdxT IndexOf(tybArrayIdxT luiPos, cbArrayT lcToken)
    {
      uint8        *lpaArraying;
      tybArrayIdxT luiTokenSize;

      if (luiPos >= muiSize) 
      {
        return cbArrayT_UINTMAX;
      }

      luiTokenSize = lcToken.muiSize;

      for (;luiPos < (muiSize - luiTokenSize); luiPos++)
      {
        lpaArraying = &(mpu8Data[luiPos]);

        if (cbArrayT_memcmp(lpaArraying, lcToken.mpu8Data, luiTokenSize) == 0)
        {
          return luiPos;
        }
      }
      return cbArrayT_UINTMAX;
    }


    /* Byte-Stream Interface */
    inline uint32 u32SizeOf()
    {
      return (sizeof(tybArrayIdxT) + muiSize);
    }

    void vToByteArray(uint8* laui8BArray)
    {
      *((tybArrayIdxT*)laui8BArray) = muiSize;
      ((tybArrayIdxT*)laui8BArray)++;
      cbArrayT_memcpy(laui8BArray, mpu8Data, muiSize);
    }

    void vFromByteArray(uint8* laui8BArray)
    {
      tybArrayIdxT luiSize;

      luiSize = *((tybArrayIdxT*)laui8BArray);
      if (luiSize > muiSize) luiSize = muiSize;
      muiSize = luiSize;
      ((tybArrayIdxT*)laui8BArray)++;
      cbArrayT_memcpy(mpu8Data, laui8BArray, muiSize);
    }

    void vParseStart(uint32 &lui32Pos)
    {
      lui32Pos = 0;
    }

    uint8 u8Parse(uint32 &lui32Pos)
    {
      lui32Pos++;
      if (lui32Pos >= muiSize) lui32Pos = 0;
      return mpu8Data[lui32Pos];
    }
};

template <typename tybArrayIdxT>
class cbArrayStaticT: public cbArrayT<tybArrayIdxT>
{
  public:
    cbArrayStaticT(uint8* maui8bArray, tybArrayIdxT ltySize)
    : cbArrayT<tybArrayIdxT>(maui8bArray, ltySize)
    {
    }

    inline uint32 SizeOf()
    {
      return cbArrayT<tybArrayIdxT>::SizeOf() + sizeof(cbArrayStaticT<tybArrayIdxT>);
    }
};


template <typename tybArrayIdxT>
class cbArrayDynT: public cbArrayT<tybArrayIdxT>
{
  private:

  public:
    cbArrayDynT()
    {
      cbArrayT<tybArrayIdxT>::mpu8Data = null;
      Create(0);
    }


    cbArrayDynT(tybArrayIdxT luiSize)
    {
      cbArrayT<tybArrayIdxT>::mpu8Data = null;
      Create(luiSize);
    }

    cbArrayDynT(cbArrayDynT<tybArrayIdxT> const& lcArray)
    {
      cbArrayT<tybArrayIdxT>::mpu8Data = null;
      Create(lcArray.muiSize);
      Set(lcArray);
    }

    cbArrayDynT(uint8 *lpaArray, tybArrayIdxT luiSize)
    {
      cbArrayT<tybArrayIdxT>::mpu8Data = null;
      Create(luiSize);
      Set(lpaArray, luiSize);
    }

    ~cbArrayDynT()
    {
      if (cbArrayT<tybArrayIdxT>::mpu8Data != null)
      {
        cbArrayT_vFREEMEM(cbArrayT<tybArrayIdxT>::mpu8Data);
        cbArrayT<tybArrayIdxT>::mpu8Data = null;
      }
    }


    inline uint32 SizeOf()
    {
      return cbArrayT<tybArrayIdxT>::SizeOf() + sizeof(cbArrayDynT<tybArrayIdxT>);
    }

    void  Create(tybArrayIdxT luiSize)
    {
      if (luiSize > 0)
      {
        #ifdef _WIN32
        if (luiSize > (cbArrayT_UINTMAX-1))
        {
          throw cbArrayT_nErr_Parameter;
        }
        #endif

        // Falls der bestehende String zu klein ist, wieder löschen und neu anlegen
        if (cbArrayT<tybArrayIdxT>::mpu8Data != null)
        {
          this->~cbArrayDynT();
        }

        if (cbArrayT<tybArrayIdxT>::mpu8Data == null)
        {
          cbArrayT<tybArrayIdxT>::mpu8Data = (uint8*)cbArrayT_pui8GETMEM(luiSize);

          if (cbArrayT<tybArrayIdxT>::mpu8Data != null)
          {
            // Speicher konnte angefordert werden
            cbArrayT<tybArrayIdxT>::muiSize      = luiSize;
          }
          #ifdef _WIN32
          else
          {
            // Falls kein Speicher angefordert werden konnte
            throw cbArrayT_nErr_OutofMem;
          }
          #endif
        }
      }
      else
      {
        cbArrayT<tybArrayIdxT>::mpu8Data = null;
        cbArrayT<tybArrayIdxT>::muiSize  = 0;
      }
    }

    cbArrayDynT Mid(tybArrayIdxT luiPos, tybArrayIdxT luiSize)
    {
      cbArrayDynT<tybArrayIdxT> lcReturn(cbArrayT<tybArrayIdxT>::muiSize);

      //Eigentlicher Funktionsrumpf für cbArrayT_nMid
      if ((luiPos >= 0) && (luiSize > 0) && (luiPos < cbArrayT<tybArrayIdxT>::muiSize)) 
      {
        //Ggf. Länge clippen
        if ((luiPos + luiSize) > cbArrayT<tybArrayIdxT>::muiSize)
        {
          luiSize = cbArrayT<tybArrayIdxT>::muiSize - luiPos;
        }

         //Daten kopieren
        cbArrayDynT_memcpy(lcReturn.mpu8Data, &(cbArrayT<tybArrayIdxT>::mpu8Data[luiPos]), luiSize);
        lcReturn.muiSize = luiSize;
      }

      return lcReturn;
    }
};


#endif //__CLASS_BYTE_ARRAY_H__
