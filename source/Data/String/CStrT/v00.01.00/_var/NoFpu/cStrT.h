/* DString: Dynamischer String */
/* Dynamischer String
     - benötigt dynamsiche Speicherverarbeitung
     - String wird automatisch NUR vergrößter. Speicher wird nur mit Delete wieder freigegeben
*/

#ifndef _CSTR_H
  #define _CSTR_H

  #include <stdarg.h>
  #include "TYPEDEF.h"
  #include "cbArrayT.h"
  #include "cStringTools.h"
  #include "cChr8.h"

  #define CSTRT_FAIL            FRET_ERR_GENERAL
  #define CSTRT_pui8GETMEM(x)  (new char8[x])
  #define CSTRT_vFREEMEM(x)    (delete[] x)
  #define CSTRT_INT            int
  #define CSTRT_UINT           unsigned int

  #define CSTRT_UINTMAX        ((1 << (sizeof(tyCStrT) * 8)) - 1)
  #define CSTRT_DATA_UINT      tyCStrT

  #define cStrT_isspace cChr8Tools::IsSpace

  #define cStrT_strcpy  cStrTools::szStrCpy
  #define cStrT_strncpy cStrTools::szStrnCpy
  #define cStrT_strcat  cStrTools::szStrCat
  #define cStrT_strlwr  cStrTools::szStrLwr
  #define cStrT_strupr  cStrTools::szStrUpr
  #define cStrT_strcmp  cStrTools::i8StrCmp
  #define cStrT_strncmp cStrTools::i8StrnCmp
  #define cStrT_txtcmp  cStrTools::i8TextCmp
  #define cStrT_txtncmp cStrTools::i8TextnCmp
  #define cStrT_strlen  cStrTools::uixStrLen
  #define cStrT_strnlen cStrTools::uixStrnLen
  #define cStrT_Iton    cStrTools::uixIton
  #define cStrT_Itoan   cStrTools::uixItoan_open
  //#define cStrT_Fton    cStrTools::uixFton
  //#define cStrT_Ftoan   cStrTools::uixFtoan_open
  #define cStrT_Atoin   cStrTools::atoin

  #define cStr  cStrBaseT<u16>
  #define cSStr cStr(lu16Size)
  #define cDStr cStrDynT<u16>

  #define cStr_Create(Name, lu16Size)  char8 Name##_Buf[lu16Size];cStrBaseT<u16> Name(Name##_Buf, 0, lu16Size);
  #define cSStr_Create(Name, lu16Size) cStr(Name, lu16Size)


  enum CStrT_enErr
  {
    CStrT_nOK            = FRET_OK,
    CStrT_nErr_No        = CStrT_nOK,
    CStrT_nErr_General   = FRET_ERR_GENERAL,
    CStrT_nErr_Parameter = FRET_ERR_PARAMETER,
    CStrT_nErr_OutofMem,
    CStrT_nErr_OutofBounds
  };

  template <typename tyCStrT>
  class cStrBaseT : public cBarryPtrT<tyCStrT>
  {
    public:
    cStrBaseT()
    {
      cBarryPtrT<tyCStrT>::muLen = 0;
      cBarryPtrT<tyCStrT>::muSize = 0;
      cBarryPtrT<tyCStrT>::mpu8Data = null;
    };

    cStrBaseT(const char8* lszStr, CSTRT_UINT luLen, CSTRT_UINT luSize)
    {
      cBarryPtrT<tyCStrT>::muLen   = luLen;
      cBarryPtrT<tyCStrT>::muSize  = luSize;
      if (cBarryPtrT<tyCStrT>::muLen > cBarryPtrT<tyCStrT>::muSize) cBarryPtrT<tyCStrT>::muLen = cBarryPtrT<tyCStrT>::muSize;
      cBarryPtrT<tyCStrT>::mpu8Data = (uint8*)lszStr;
    };

    cStrBaseT(const char8* lszStr, CSTRT_UINT luSize)
    {
      cBarryPtrT<tyCStrT>::muLen   = (CSTRT_UINT)cStrT_strnlen(lszStr, luSize);
      cBarryPtrT<tyCStrT>::muSize  = luSize;
      if (cBarryPtrT<tyCStrT>::muLen > cBarryPtrT<tyCStrT>::muSize) cBarryPtrT<tyCStrT>::muLen = cBarryPtrT<tyCStrT>::muSize;
      cBarryPtrT<tyCStrT>::mpu8Data = (uint8*)lszStr;
    };

    cStrBaseT(const char8* lszStr)
    {
      From(lszStr);
    };

    cStrBaseT(cStr* lszStr)
    {
      From(lszStr);
    };

    inline void From(const char8* lszStr)
    {
      cBarryPtrT<tyCStrT>::muLen = (CSTRT_UINT)cStrT_strlen(lszStr);
      cBarryPtrT<tyCStrT>::muSize = cBarryPtrT<tyCStrT>::muLen;
      cBarryPtrT<tyCStrT>::mpu8Data = (uint8*)lszStr;
    }

    inline void From(cStr* lszStr)
    {
      cBarryPtrT<tyCStrT>::muLen = lszStr->muLen;
      cBarryPtrT<tyCStrT>::muSize   = lszStr->muSize;
      cBarryPtrT<tyCStrT>::mpu8Data  = lszStr->mpu8Data;
    }

    char8* ToString()
    {
      // Falls hinten schon eine Null dranhängt
      // dann kann der String direkt benutzt werden.
      if (cBarryPtrT<tyCStrT>::muLen > 0)
      {
        if (cBarryPtrT<tyCStrT>::mpu8Data[cBarryPtrT<tyCStrT>::muLen - 1] == 0)
        {
          return (char8*)cBarryPtrT<tyCStrT>::mpu8Data;
        }
      }
      *this += (char8)0;
      return (char8*)cBarryPtrT<tyCStrT>::mpu8Data;
    }

    inline uint32 SizeOf()
    {
      return cBarryPtrT<tyCStrT>::SizeOf() + sizeof(cStrBaseT<tyCStrT>);
    }

    inline CSTRT_UINT Len(void)
    {
      return cBarryPtrT<tyCStrT>::muLen;
    };


    inline CSTRT_UINT CheckStrlen(CSTRT_UINT luiOffset, uint32 lui32StrLen)
    {
      lui32StrLen += luiOffset;
      if (lui32StrLen > cBarryPtrT<tyCStrT>::muSize)
      {
        //throw CStr8_nErr_OutofBounds;
        //lui32StrLen = CSTRT_UINTMAX;
        lui32StrLen = cBarryPtrT<tyCStrT>::muSize;
      }
      return (CSTRT_UINT)lui32StrLen;
    };

    inline bool isStrValid(const char8* lpszStr)
    {
      if (lpszStr != null) // ""-String
      {
        if (*lpszStr != 0) // ""-String
        {
          return True;
        }
      }
      return False;
    }

    inline bool isStrValid(char8* lpszStr)
    {
      if (lpszStr != null) // ""-String
      {
        if (*lpszStr != 0) // ""-String
        {
          return True;
        }
      }
      return False;
    }

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Set(pcStr8 lpstStr, char8* lpszStr)
     *  \brief Schreibt den übergebenen String in den DString8. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
     *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Set(char8* lpszStr)
    {
      if (!isStrValid(lpszStr)) // ""-String
      {
        cBarryPtrT<tyCStrT>::muLen = 0;
      }
      else
      {
        cBarryPtrT<tyCStrT>::Set((uint8*)lpszStr, (CSTRT_UINT)cStrT_strnlen(lpszStr, cBarryPtrT<tyCStrT>::muSize));
      }
      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Set(pcStr8 lpstStr, const char8* lpszStr)
     *  \brief Schreibt den übergebenen String in den DString8. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
     *  \param lpszStr const char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Set(const char8* lpszStr)
    {
      if (!isStrValid((char8*)lpszStr)) // ""-String
      {
        cBarryPtrT<tyCStrT>::muLen = 0;
      }
      else
      {
         cBarryPtrT<tyCStrT>::Set((uint8*)lpszStr, (CSTRT_UINT)cStrT_strnlen(lpszStr, cBarryPtrT<tyCStrT>::muSize));
      }
      return *this;
    };


    cStrBaseT& Set(char8 lchSign)
    {
      cBarryPtrT<tyCStrT>::Set(lchSign);
      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Set(CStrT lcStrSource)
     *  \brief Kopiert lcStrSource. Der Zielstring wird bei Bedarf vergrößert aber nicht verkleinert
      *  \param lcStrSource CStrT: Source String
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Set(cStrBaseT const& lcStrSource)
    {
      cBarryPtrT<tyCStrT>::Set(lcStrSource);
      return *this;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Set(pcStr8 lpstStr, char8* lpszStr)
     *  \brief Schreibt einen formatierten String (vgl.bar zu printf) in den DString8. DString8 wird bei Bedarf vergrößert aber nicht verkleinert
     *         Basiert ursprünglich auf ChaN's xprintf
     *  \param lpstStr DString8*: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
     *  \param lpszStr char8*: '0'-terminierte formatierte Zeichenkette die geschrieben weden soll.
     *  \param lParList va_list: Parameterliste.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Setf(const char8* lpszfStr, ...)
    {
      /*----------------------------------------------*/
      /* Formatted string output                      */
      /*----------------------------------------------*/
      /*    lszStr.Setf("%d",   1234);           // "1234"
            lszStr.Setf("%6d,%3d%%", -200, 5);   // "  -200,  5%"
            lszStr.Setf("%-6u",    100);         // "100   "
            lszStr.Setf("%ld",     12345678L);   // "12345678"
            lszStr.Setf("%04x",    0xA3);        // "00a3"
            lszStr.Setf("%08LX",   0x123ABC);    // "00123ABC"
            lszStr.Setf("%016b",   0x550F);      // "0101010100001111"
            lszStr.Setf("%s",      "String");    // "String"
            lszStr.Setf("%-4s",    "abc");       // "abc "
            lszStr.Setf("%4s",     "abc");       // " abc"
            lszStr.Setf("%c",      'a');         // "a"

            lszStr.Setf("%f",      10.0);        // "10"
            lszStr.Setf("%.2f",    10.0);        // "10.0"
            lszStr.Setf("%.02f",   10.0);        // "10.00"
            lszStr.Setf("%.2f",    10.01);       // "10.01"
            lszStr.Setf("%8.3f",  -1.123);       // "  -1.123"
            lszStr.Setf("%08.3f", -1.123);       // "-001.123"
            lszStr.Setf("%-8.3f", -1.123);       // "-1.123  "
      */

      /* %[flags][width][.precision][length]specifier */

      u8 lui8Flags; /* Flag */
                    /*  1: '0'     => '0' padded   */
                    /*  2: '-'     => left justified   */
                    /*  4: 'l','L' => Prefix: Size is long int, not supported anymore */
                    /*  8: '-'     => signed */
                    /* 16: 'X'     => hex charaters in upper case */
                    /* 64: '0'     => '0' padded float number fractional part  */
      i16    li16Size;
      i16    li16Width;
      i16    li16Prec;
      i8     li8Spec;

      i16    li16Pos;
      char8* lszStr;

      u8     lui8Radix;

      i32    li32Value;
      //float  lfValue;

      char8  lc8Zeichen;

      va_list lParList;
      va_start(lParList, lpszfStr);

      cBarryPtrT<tyCStrT>::muLen = 0; // *this = "";

      while (1)
      {
        lc8Zeichen = *lpszfStr++;       /* Get a char */
        if (!lc8Zeichen) break;         /* End of format? */
        if (lc8Zeichen != '%')          /* Pass through it if not a % sequence */
        {
          *this += lc8Zeichen; continue;
        }


        lui8Flags = 0;
        lc8Zeichen = *lpszfStr++;       /* Get first char of the sequence */
        /* Flag: '0' padded */
        if (lc8Zeichen == '0')
        {
          lui8Flags = 1; lc8Zeichen = *lpszfStr++;
        }
        else
        {
          /* Flag: left justified */
          if (lc8Zeichen == '-')
          {
            lui8Flags = 2; lc8Zeichen = *lpszfStr++;
          }
        }

        /* Minimum width */
        for (li16Width = 0; (lc8Zeichen >= '0') && (lc8Zeichen <= '9'); lc8Zeichen = *lpszfStr++)
        {
          li16Width = li16Width * 10 + lc8Zeichen - '0';
        }

        /* Precision */
        li16Prec = 0;
        if (lc8Zeichen == '.') 
        {
          /* Flag: '0' padded fractional part*/
          lc8Zeichen = *lpszfStr++;
          if (lc8Zeichen == '0')
          {
            lui8Flags |= 64; lc8Zeichen = *lpszfStr++;
          }

          while (lc8Zeichen >= '0' && lc8Zeichen <= '9') 
          {
            li16Prec = li16Prec * 10 + lc8Zeichen - '0';
            lc8Zeichen = *lpszfStr++;
          }
        }

        if (lc8Zeichen == 'l' || lc8Zeichen == 'L')
        {  /* Prefix: Size is long int */
          lui8Flags |= 4; lc8Zeichen = *lpszfStr++;
        }

        /* End of format? */
        if (!lc8Zeichen) break;

        li8Spec = lc8Zeichen;
        if (li8Spec >= 'a') li8Spec -= 0x20; /* convert to upper case */

        /* Specifier is... */
        switch (li8Spec)
        {
          case 'S':          /* String */
            lszStr = va_arg(lParList, char8*);

            li16Pos = CheckStrlen(0, (CSTRT_UINT)cStrT_strlen(lszStr));
            if (!(lui8Flags & 2)) /* right justified, add padding */
            {
              while (li16Pos++ < li16Width) *this += ' ';
            }

            *this += lszStr;

            while (li16Pos++ < li16Width) *this += ' ';
            continue;

          case 'C':          /* Character */
            li16Pos = 1;
            if (!(lui8Flags & 2)) /* right justified, add padding */
            {
              while (li16Pos++ < li16Width) *this += ' ';
            }

            lc8Zeichen = (int8)va_arg(lParList, int);
            *this += lc8Zeichen;
            while (li16Pos++ < li16Width) *this += ' ';
            continue;

          case 'B':          /* Binary */
            lui8Radix = 2; break;

          case 'O':          /* Octal */
            lui8Radix = 8; break;

          case 'D':          /* Signed decimal */
          case 'U':          /* Unsigned decimal */
            lui8Radix = 10; break;

          case 'X':          /* Hexdecimal */
            lui8Radix = 16;
            if (lc8Zeichen == 'X') lui8Flags |= 16;
            break;

          //case 'F':          /* Float */
            //lui8Radix = 128;
            //break;

          default:            /* Unknown type (passthrough) */
            *this += lc8Zeichen;
            continue;
        }

        li16Pos = 0;
      
        if (cBarryPtrT<tyCStrT>::muLen < cBarryPtrT<tyCStrT>::muSize)
        {        
          #if 0
          u16 lu16TrailingZeros = 0;
          u16 lu16StartPosOfFloat;
          if (lui8Radix & 128)
          {
            lfValue = (float)va_arg(lParList, double);   // float

            if (li16Width)
            {
              if (lui8Flags & 1) /* leading '0' */
              {
                u8 lu8Dummy;
                li16Size = cStrT_Fton(lfValue, li16Prec, lu8Dummy);

                if (lui8Flags & 64) /* reserve space for trailing '0' */
                {
                  lu16TrailingZeros = li16Prec - lu8Dummy;
                  li16Size += lu16TrailingZeros;
                }

                while (li16Size++ < li16Width) {*this += '0';li16Pos++;}
              }
              else
              if (!(lui8Flags & 2)) /* right justified, add padding */
              {
                u8 lu8Dummy;
                li16Size = cStrT_Fton(lfValue, li16Prec, lu8Dummy);

                if (lui8Flags & 64) /* reserve space for trailing '0' */
                {
                  lu16TrailingZeros = li16Prec - lu8Dummy;
                  li16Size += lu16TrailingZeros;
                }

                while (li16Size++ < li16Width) {*this += ' ';li16Pos++;}
              }
              
            }
            else
            {
              if (lui8Flags & 64) /* reserve space for trailing '0' */
              {
                u8 lu8Dummy;
                li16Size = cStrT_Fton(lfValue, li16Prec, lu8Dummy);
                lu16TrailingZeros = li16Prec - lu8Dummy;
              }
            }

            lu16StartPosOfFloat = cBarryPtrT<tyCStrT>::muLen;
            lszStr = (char8*)(cBarryPtrT<tyCStrT>::mpu8Data + cBarryPtrT<tyCStrT>::muLen);
            li16Size = cStrT_Ftoan(lfValue, lszStr, cBarryPtrT<tyCStrT>::muSize - cBarryPtrT<tyCStrT>::muLen, li16Prec);
            cBarryPtrT<tyCStrT>::muLen += (CSTRT_UINT)(li16Size);

            if (lui8Flags & 64) /* add trailing '0' */
            {
              while (lu16TrailingZeros-- > 0) {*this += '0';li16Size++;}
            }

            if (lui8Flags & 1) /* exchange the '-' with a leading 0 */
            {
              if ((lfValue < 0) && (lu16StartPosOfFloat > 0))
              {
                lszStr = (char8*)(cBarryPtrT<tyCStrT>::mpu8Data);
                lszStr[0] = '-';
                lszStr[lu16StartPosOfFloat] = '0';
              }
            }
          }
          else
          #endif
          {
            /* Get an argument and put it in numeral */
            if (lui8Flags & 4)       li32Value = (u32)va_arg(lParList, u64);   // 64 Bit -> 32 Bit
            else if (li8Spec == 'D') li32Value = (u32)va_arg(lParList, i32);   // 32 Bit
            else                     li32Value = (u32)va_arg(lParList, u32);   // 32 Bit

            if (li16Width)
            {
              if (lui8Flags & 1) /* leading '0' */
              {
                li16Size = cStrT_Iton(li32Value, lui8Radix);
                while (li16Size++ < li16Width) {*this += '0';li16Pos++;}
              }
              else
              if (!(lui8Flags & 2)) /* right justified, add padding */
              {
                li16Size = cStrT_Iton(li32Value, lui8Radix);
                while (li16Size++ < li16Width) {*this += ' ';li16Pos++;}
              }
            }

            lszStr = (char8*)(cBarryPtrT<tyCStrT>::mpu8Data + cBarryPtrT<tyCStrT>::muLen);
            li16Size = cStrT_Itoan(li32Value, lszStr, cBarryPtrT<tyCStrT>::muSize - cBarryPtrT<tyCStrT>::muLen, lui8Radix, lui8Flags & 16);
            cBarryPtrT<tyCStrT>::muLen += (CSTRT_UINT)(li16Size);
          }
          
          /* right justified, add padding */
          li16Pos += li16Size;
          while (li16Pos++ < li16Width) *this += ' ';
        }
        else
        {
          break;
        }
      }

      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Add(const char8* lpszStr)
     *  \brief Fügt den übergebenen String dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Add(const char8* lpszStr)
    {
      if (isStrValid(lpszStr))
      {
        cBarryPtrT<tyCStrT>::Add((uint8*)lpszStr, (CSTRT_UINT)cStrT_strlen(lpszStr));
      }
      return *this;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Add(char8* lpszStr)
     *  \brief Fügt den übergebenen String dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Add(char8* lpszStr)
    {
      if (isStrValid(lpszStr))
      {
        cBarryPtrT<tyCStrT>::Add((uint8*)lpszStr, (CSTRT_UINT)cStrT_strlen(lpszStr));
      }
      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Add(char8 li8Char)
     *  \brief Fügt das übergebene Zeichen dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
     *  \param li8Char char8: Zeichen das geschrieben werden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Add(char8 li8Char)
    {
      cBarryPtrT<tyCStrT>::Add(li8Char);
      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::Add(char8* lpszStr)
     *  \brief Fügt den übergebenen String dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
     *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Add(cStrBaseT<tyCStrT> const& lpcStr)
    {
      cBarryPtrT<tyCStrT>::Add(lpcStr);
      return *this;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrStaticT<tyCStrT>::iInsert(uint16 luiPos, pcStr8 lpstStr, char8* lpszStr)
     *  \brief Fügt an der angegebenen Stelle den übergebenen String in den pcStr8 hinein.
     *  \param luiPos int8: Zielposition zum einfügen. Start bei 0
     *  \param lpstStr DString8: Zielstring, hier wird eingefügt
     *  \param lpszStr char8*: Zu einfügender String
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Insert(CSTRT_UINT luiPos, char8* lpszStr)
    {
      if (isStrValid(lpszStr))
      {
        cBarryPtrT<tyCStrT>::Insert(luiPos, (uint8*)lpszStr, (CSTRT_UINT)cStrT_strlen(lpszStr));
      }
      return *this;
    }

    cBarryPtrT<tyCStrT>& Insert(CSTRT_UINT luiPos, uint8 lui8Byte)
    {
      cBarryPtrT<tyCStrT>::Insert(luiPos, lui8Byte);
      return *this;
    }

    cStrBaseT& Mid(CSTRT_UINT luiPos, CSTRT_UINT luilen)
    {

      if ((CSTRT_UINT)luiPos > (CSTRT_UINT)((cBarryPtrT<tyCStrT>::muLen - 1)))
      {
        cBarryPtrT<tyCStrT>::muLen = 0;
        return *this;
      }

      //ggf. clippen
      if ((luiPos + luilen) > cBarryPtrT<tyCStrT>::muLen)
      {
        luilen = cBarryPtrT<tyCStrT>::muLen - luiPos;
      }

      cStrT_strncpy((char8*)&(cBarryPtrT<tyCStrT>::mpu8Data[0]), (char8*)&(cBarryPtrT<tyCStrT>::mpu8Data[luiPos]), luilen);
      cBarryPtrT<tyCStrT>::muLen = luilen;

      return *this;
    };

    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::iRemove(uint8 luiPos, uint8 luilen)
     *  \brief Entfernt ein Stück aus der Mitte
     *  \param luiPos int8: Startposition beginnend bei 0
     *  \param luilen int8: Anzahl zu entfernender Zeichen
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  -  sonst: allg. Fehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Cut(CSTRT_UINT luiPos, CSTRT_UINT luilen)
    {
      cBarryPtrT<tyCStrT>::Cut(luiPos, luilen);
      return *this;
    }

    cStrBaseT& Rev(void)
    {
      cBarryPtrT<tyCStrT>::Rev();
      return *this;
    };


    /****************************************************************************************/
    /*! FRet CStrT<tyCStrT>::iReplaceSign(pcStr8 lpstString, char8 lc8SignToFind, char8 lc8SignToSet)
     *  \brief Ersetzt das angegebene Zeichen durch ein anderes
     *  \param lpstString DString8*: String, in dem gesucht wird
     *  \param lc8SignToFind char8: Zu suchendes Zeichen
     *  \param lc8SignToSet  char8: Zu ersetztenden Zeichen
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 22.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Replace(char8 lc8SignToFind, char8 lc8SignToSet)
    {
      cBarryPtrT<tyCStrT>::Replace(lc8SignToFind, lc8SignToSet);
      return *this;
    };

    /****************************************************************************************/
    /*! FRet CStrT<tyCStrT>::iRemoveSign(pcStr8 lpstString, int8 lc8SignToFind)
     *  \brief Entfernt das angegebene Zeichen
     *  \param lpstString DString8: String, in dem gesucht wird
     *  \param lc8SignToFind int8: Zu suchendes Zeichen
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 08.07.2012
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Remove(char8 lc8SignToFind)
    {
      cBarryPtrT<tyCStrT>::Remove(lc8SignToFind);
      return *this;
    };

    CSTRT_INT CountOf(char8 lc8SignToFind)
    {
      return cBarryPtrT<tyCStrT>::CountOf(lc8SignToFind);
    };

    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::iLTrim(pcStr8 lpstString)
     *  \brief Entfernt die Leerzeichen vorne am String
     *  \param lpstString DString8: String, an dem die Leerzeichen entfernt werden sollen
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 22.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& LTrim()
    {
      CSTRT_DATA_UINT luiPos;

      if (cBarryPtrT<tyCStrT>::muLen > 0)
      {
        luiPos = 0;

        while ((cChr8Tools::IsSpace((char8)cBarryPtrT<tyCStrT>::mpu8Data[luiPos])) && (luiPos < cBarryPtrT<tyCStrT>::muLen))
        {
          luiPos++;
        }

        if (luiPos > 0)
        {
          if (luiPos < cBarryPtrT<tyCStrT>::muLen)
          {
            /* Lücke schließen */
            CSTRT_DATA_UINT luiPos2;
            luiPos2 = 0;
            while (luiPos < cBarryPtrT<tyCStrT>::muLen)
            {
              cBarryPtrT<tyCStrT>::mpu8Data[luiPos2] = cBarryPtrT<tyCStrT>::mpu8Data[luiPos];
              luiPos++;
              luiPos2++;
            }
            cBarryPtrT<tyCStrT>::muLen = luiPos2;
          }
          else
          {
            cBarryPtrT<tyCStrT>::muLen = 0;
          }
        }
      }

      return *this;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::iRTrim(pcStr8 lpstString)
     *  \brief Entfernt die Leerzeichen hinten am String
     *  \param lpstString DString8: String, an dem die Leerzeichen entfernt werden sollen
     *  \return int8
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler
     *  \author Florian Kaes
     *  \section history History
     *  \since 22.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& RTrim()
    {
      if (cBarryPtrT<tyCStrT>::muLen > 0)
      {
        while ((cChr8Tools::IsSpace(cBarryPtrT<tyCStrT>::mpu8Data[cBarryPtrT<tyCStrT>::muLen - 1])) && (cBarryPtrT<tyCStrT>::muLen > 0))
        {
          cBarryPtrT<tyCStrT>::muLen--;
        }
      }
      return *this;
    };



    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::iTrim(DString8* lpstString)
     *  \brief Entfernt die Leerzeichen vorne und hinten am String
     *  \param lpstString DString8*: String, an dem die Leerzeichen entfernt werden sollen
     *  -  0: kein Fehler
     *  -  1: allg. Fehler
     *  -  2: Übergabefehler *  \author Florian Kaes
     *  \section history History
     *  \since 22.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    cStrBaseT& Trim()
    {
      LTrim();
      RTrim();
      return *this;
    };

    cStrBaseT& Upper()
    {
      if (cBarryPtrT<tyCStrT>::muLen > 0)
      {
        cStrT_strupr((char8*)cBarryPtrT<tyCStrT>::mpu8Data);
      }
      return *this;
    };

    cStrBaseT& Lower()
    {
      if (cBarryPtrT<tyCStrT>::muLen > 0)
      {
        cStrT_strlwr((char8*)cBarryPtrT<tyCStrT>::mpu8Data);
      }
      return *this;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::Cmp(pcStr8 lpstStr1, pcStr8 lpstStr2)
     *  \brief Vergleicht die beiden String miteinander
     *  \param lpstStr1 DString8: zu vergleichender String oder Wildcard
     *  \param lpstStr2 lui8Like: Überprüft ob die Strings ähnlich sind (Groß und Kleinschreibung und umschließende Leerzeichen werden ignoriert, Wildcard '*' ist möglich).
     *  \return int8: < 0 string1 less than string2; == 0 string1 identical to string2; > 0 string1 greater than string2
     *  \author Florian Kaes
     *  \section history History
     *  \since 22.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    int8 Cmp(cStrBaseT &lpstStr)
    {
      return cStrT_strcmp((char8*)cBarryPtrT<tyCStrT>::mpu8Data, (char8*)lpstStr.cBarryPtrT<tyCStrT>::mpu8Data);
    };

    int8 nCmp(cStrBaseT &lpstStr)
    {
      return cStrT_strncmp((char8*)cBarryPtrT<tyCStrT>::mpu8Data, (char8*)lpstStr.cBarryPtrT<tyCStrT>::mpu8Data, cBarryPtrT<tyCStrT>::muLen);
    };

    int8 TxtCmp(cStrBaseT &lpstStr)
    {
      return cStrT_txtcmp((char8*)cBarryPtrT<tyCStrT>::mpu8Data, (char8*)lpstStr.cBarryPtrT<tyCStrT>::mpu8Data);
    };

    int8 nTxtCmp(cStrBaseT &lpstStr)
    {
      return cStrT_txtncmp((char8*)cBarryPtrT<tyCStrT>::mpu8Data, (char8*)lpstStr.cBarryPtrT<tyCStrT>::mpu8Data, cBarryPtrT<tyCStrT>::muLen);
    };


    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::uInstr(Sign(uint8 luiPos, pcStr8 lpstStr, int8 lc8SignToFind)
     *  \brief Gibt die Position eines Zeichens zurück
     *  \param luiPos uint8: Startposition, bei der die Suche begonnen wird
     *  \param lpstStr DString8: String, in dem gesucht wird
     *  \param lc8SignToFind int8: Zeichen, nach dem gesucht wird
     *  \return uint8: 0xFF nicht gefunden, sonst Position den gefundenen Zeichens.
     *  \author Florian Kaes
     *  \section history History
     *  \since 08.17.2012
     *  - Erstellt
     */
     /****************************************************************************************/
    CSTRT_UINT Instr(CSTRT_UINT luiPos, char8 lc8SignToFind)
    {
      return cBarryPtrT<tyCStrT>::IndexOf(luiPos, lc8SignToFind);
    };


    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::uInstr((uint16 luiPos, pcStr8 lpstStr, char8* lszToken)
     *  \brief Sucht die Position des übergegebenen Sub-String und gibt ggf. dessen Position zurück
     *  \param luiPos int8: Startposition, bei der die Suche begonnen wird
     *  \param lpstStr DString8: String, in dem gesucht wird
     *  \param lszToken char8*: Substring, nach dem gesucht wird
     *  \return uint8: 0xFF nicht gefunden sonst Position den gefundenen Strings.
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    CSTRT_UINT Instr(CSTRT_UINT luiPos, const char8* lszToken)
    {
      if (isStrValid(lszToken))
      {
        return cBarryPtrT<tyCStrT>::IndexOf(luiPos, (uint8*)lszToken, (CSTRT_DATA_UINT)cStrT_strlen(lszToken));
      }

      return CSTRT_UINTMAX;
    };


    /****************************************************************************************/
    /*! \fn FRet CStrT<tyCStrT>::uInstr((uint16 luiPos, pcStr8 lpstStr, char8* lszToken)
     *  \brief Sucht die Position des übergegebenen Sub-String und gibt ggf. dessen Position zurück
     *  \param luiPos int8: Startposition, bei der die Suche begonnen wird
     *  \param lpstStr DString8: String, in dem gesucht wird
     *  \param lszToken char8*: Substring, nach dem gesucht wird
     *  \return uint8: 0xFF nicht gefunden sonst Position den gefundenen Strings.
     *  \author Florian Kaes
     *  \section history History
     *  \since 17.11.2011
     *  - Erstellt
     */
     /****************************************************************************************/
    CSTRT_UINT Instr(CSTRT_UINT luiPos, cStrBaseT &lcszToken)
    {
      return cBarryPtrT<tyCStrT>::IndexOf(luiPos, lcszToken);
    };


    CSTRT_UINT StartOfText(CSTRT_UINT li32StartIdx = 0)
    {
      const char8* lpszTemp;
      for (lpszTemp = (const char8*)&cBarryPtrT<tyCStrT>::mpu8Data[li32StartIdx]; li32StartIdx < cBarryPtrT<tyCStrT>::muLen; lpszTemp++)
      {
        if (!(cStrT_isspace(*lpszTemp) || (*lpszTemp == '-'))) return li32StartIdx;
        li32StartIdx++;
      }
      return li32StartIdx;
    }

    CSTRT_UINT EndOfText(CSTRT_UINT li32StartIdx = 0)
    {
      const char8* lpszTemp = (const char8*)&cBarryPtrT<tyCStrT>::mpu8Data[li32StartIdx];
      if (cStrT_isspace(*lpszTemp) || (*lpszTemp == '-')) return li32StartIdx;
      for (; li32StartIdx < cBarryPtrT<tyCStrT>::muLen; lpszTemp++)
      {
        if (cStrT_isspace(*lpszTemp) || (*lpszTemp == '-')) return li32StartIdx - 1;
        li32StartIdx++;
      }
      return li32StartIdx;
    }

    CSTRT_INT Atoi(void)
    {
      return cStrT_Atoin((char8*)cBarryPtrT<tyCStrT>::mpu8Data, cBarryPtrT<tyCStrT>::muLen);
    };

    CSTRT_INT Atoi(CSTRT_INT offset)
    {
      if (offset < cBarryPtrT<tyCStrT>::muLen)
      {
        return cStrT_Atoin((char8*)cBarryPtrT<tyCStrT>::mpu8Data + offset, cBarryPtrT<tyCStrT>::muLen - offset);
      }
      else
      {
        return CSTRT_UINTMAX;
      }
    };

    cStrBaseT& Itoa(CSTRT_INT num, CSTRT_INT base)
    {
      u32 lu32Size;
      lu32Size = cStrT_Itoan(num, (char8*)cBarryPtrT<tyCStrT>::mpu8Data, cBarryPtrT<tyCStrT>::muSize, base);
      cBarryPtrT<tyCStrT>::muLen = (CSTRT_UINT)lu32Size;
      return *this;
    };

    cStrBaseT& Itoa(CSTRT_INT offset, CSTRT_INT num, CSTRT_INT base)
    {
      u32 lu32Size;
      lu32Size = cStrT_Itoan(num, (char8*)cBarryPtrT<tyCStrT>::mpu8Data + offset, cBarryPtrT<tyCStrT>::muSize - offset, base);

      if ((offset + lu32Size) > cBarryPtrT<tyCStrT>::muLen)
      {
        cBarryPtrT<tyCStrT>::muLen = (CSTRT_UINT)(offset + lu32Size);
      }
      return *this;
    };

    //---------------------- Operatoren -----------------------------------------

    bool operator== (const cStrBaseT &lszStr)
    {
      return (Cmp(lszStr) == 0);
    };

    // e.g. MyStr += "a"
    cStrBaseT& operator+=(char8* lpszStr)
    {
      return Add(lpszStr);
    };

    // e.g. MyStr += "a"
    cStrBaseT& operator+=(const char8* lpszStr)
    {
      return Add(lpszStr);
    };

    // e.g. MyStr += 'a'
    cStrBaseT& operator+=(char8 li8Char)
    {
      return Add(li8Char);
    };

    // e.g. myStr += myStr
    cStrBaseT& operator+=(cStrBaseT<tyCStrT> const& lpcStr)
    {
      return Add(lpcStr);
    };

    //----- Copy operator ------
    // e.g. MyStr = "a"
    cStrBaseT& operator=(char8* lpszStr)
    {
      return Set(lpszStr);
    };

    // e.g. MyStr = "a"
    cStrBaseT& operator=(const char8* lpszStr)
    {
      return Set(lpszStr);
    };

    // e.g. MyStr = 'a'
    cStrBaseT& operator=(char8 lchSign)
    {
      return Set(lchSign);
    };

    // e.g myStr1 = myStr2;
    cStrBaseT& operator=(cStrBaseT const& lpcStr)
    {
      return Set(lpcStr);
    };

    // ------------------------------

    // e.g. myStr--;
    cStrBaseT& operator++(int)
    {
      return Upper();
    };

    // e.g. myStr--;
    cStrBaseT& operator--(int)
    {
      return Lower();
    };


    // Indexing operator - Read
    char8 operator[](CSTRT_UINT luiPos) const
    {
      if (luiPos < cBarryPtrT<tyCStrT>::muLen)
      {
        return (char8)cBarryPtrT<tyCStrT>::mpu8Data[luiPos];
      }
      return 0;
    };

    // Indexing operator - Writable a[i] = x
    char8& operator[](CSTRT_UINT luiPos)
    {
      #ifdef _WIN32
        if (luiPos >= cBarryPtrT<tyCStrT>::muLen) throw CStrT_nErr_OutofBounds;
      #endif
      return (char8&)cBarryPtrT<tyCStrT>::mpu8Data[luiPos];
    };
  };


  #define CStrStaticT cStrBaseT
  #define CStrST CStrStaticT


  #define CStrDT cStrDynT

  template <typename tyCStrT>
  class cStrDynT: public cStrBaseT<tyCStrT>
  {
    private:

    public:
      cStrDynT()
      {
        Create(0);
      };

      cStrDynT(tyCStrT ltySize)
      {
        Create(ltySize);
      };

      cStrDynT(cStrDynT<uint16> &lszString)
      {
        Create(lszString.Len());
        Set(lszString);
      }

      cStrDynT(char8* lpszStr)
      {
        Create((CSTRT_UINT)cStrT_strlen(lpszStr));
        Set(lpszStr);
      };

      cStrDynT(int num, int base)
      {
        Create(10);
        cStrBaseT<tyCStrT>::Itoa(num, base);
      };

      ~cStrDynT()
      {
        if (cBarryPtrT<tyCStrT>::mpu8Data != null)
        {
          CSTRT_vFREEMEM((char8*)cBarryPtrT<tyCStrT>::mpu8Data);
        }
        Create(0);
      };

      void Create(CSTRT_UINT luiStrLen)
      {
        if (luiStrLen > 0)
        {
          cBarryPtrT<tyCStrT>::mpu8Data = null;
          Reuse(luiStrLen);
        }
        else
        {
          cBarryPtrT<tyCStrT>::mpu8Data = null;
          cBarryPtrT<tyCStrT>::muLen = 0;
          cBarryPtrT<tyCStrT>::muSize = 0;
        }
      };

      void Reuse(CSTRT_UINT luiStrLen)
      {
        //Parameterüberwachung für luiStrLen
        #ifdef _WIN32
        if ((luiStrLen == 0) || (luiStrLen > (CSTRT_UINTMAX - 1)))
        {
          throw CStrT_nErr_Parameter;
        }
        #endif

        //Eigentlicher Funktionsrumpf für cStrT_Create
        // Falls der bestehende String zu klein ist, wieder löschen und neu anlegen
        if ((cBarryPtrT<tyCStrT>::mpu8Data != null) &&
          (cBarryPtrT<tyCStrT>::muSize < luiStrLen))
        {
          this->~cStrDynT();
        }

        if (cBarryPtrT<tyCStrT>::mpu8Data == null)
        {
          // Auf vielfaches von 4 aufrunden
          luiStrLen = (luiStrLen + 3) & (~3);

          cBarryPtrT<tyCStrT>::mpu8Data = (uint8*)CSTRT_pui8GETMEM(luiStrLen);

          if (cBarryPtrT<tyCStrT>::mpu8Data != null)
          {
            // Speicher konnte angefordert werden
            cBarryPtrT<tyCStrT>::muSize = luiStrLen;
            cBarryPtrT<tyCStrT>::muLen  = 0;
          }
          #ifdef _WIN32
          else
          {
            // Falls kein Speicher angefordert werden konnte
            throw CStrT_nErr_OutofMem;
          }
          #endif
        }
      };

      char* Dupe()
      {
        char *lszStr = (char*) new char[cStrBaseT<tyCStrT>::Len() + 1];
        cBarryPtrT_memcpy((uint8*)lszStr, cBarryPtrT<tyCStrT>::mpu8Data, cStrBaseT<tyCStrT>::Len());
        lszStr[cStrBaseT<tyCStrT>::Len()] = 0;
        return lszStr;
      }

      char* DupeAll()
      {
        char *lszStr = (char*) new char[cStrBaseT<tyCStrT>::Len() + 1];

        for (uint32 i = 0; i < cStrBaseT<tyCStrT>::Len(); i++)
        {
          if (!cBarryPtrT<tyCStrT>::mpu8Data[i])
          {
            lszStr[i] = '_';
          }
          else
          {
            lszStr[i] = cBarryPtrT<tyCStrT>::mpu8Data[i];
          }

        }

        lszStr[cStrBaseT<tyCStrT>::Len()] = 0;
        return lszStr;
      }

      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Set(pcStr8 lpstStr, char8* lpszStr)
       *  \brief Schreibt den übergebenen String in den DString8. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
       *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
       *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Set(char8* lpszStr)
      {
        if (cStrBaseT<tyCStrT>::isStrValid(lpszStr))
        {
          Reuse((CSTRT_UINT)cStrT_strlen(lpszStr));
          cStrBaseT<tyCStrT>::Set(lpszStr);
        }
        return *this;
      };

      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Set(pcStr8 lpstStr, const char8* lpszStr)
       *  \brief Schreibt den übergebenen String in den DString8. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
       *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
       *  \param lpszStr const char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Set(const char8* lpszStr)
      {
        if (cStrBaseT<tyCStrT>::isStrValid((char8*)lpszStr))
        {
          Reuse((CSTRT_UINT)cStrT_strlen(lpszStr));
          cStrBaseT<tyCStrT>::Set(lpszStr);
        }
        return *this;
      };


      cStrDynT& Set(char8 lchSign)
      {
        Reuse(1);
        // Daten übernehmen
        cBarryPtrT<tyCStrT>::mpu8Data[0] = lchSign;
        cBarryPtrT<tyCStrT>::muLen = 1;

        return *this;
      };

      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Set(CStrT lcStrSource)
       *  \brief Kopiert lcStrSource. Der Zielstring wird bei Bedarf vergrößert aber nicht verkleinert
        *  \param lcStrSource CStrT: Source String
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Set(cStrDynT const& lcStrSource)
      {
        if (((cStrBaseT<tyCStrT>&)lcStrSource).Len() > 0)
        {
          Reuse(((cStrBaseT<tyCStrT>&)lcStrSource).Len());
          cStrBaseT<tyCStrT>::Set(lcStrSource);
        }
        return *this;
      };


      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Add(char8* lpszStr)
       *  \brief Fügt den übergebenen String dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
       *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Add(char8* lpszStr)
      {
        if (cStrBaseT<tyCStrT>::isStrValid(lpszStr))
        {
          Reuse(cBarryPtrT<tyCStrT>::muLen + (CSTRT_UINT)cStrT_strlen(lpszStr));
          cStrBaseT<tyCStrT>::Add(lpszStr);
        }
        return *this;
      };

      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Add(char8 li8Char)
       *  \brief Fügt das übergebene Zeichen dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
       *  \param lpstStr DString8: DString8, der geschrieben werden soll. Muss vorher mit Create angelegt worden sein
       *  \param li8Char char8: Zeichen das geschrieben werden soll.
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Add(char8 li8Char)
      {
        cStrBaseT<tyCStrT>::Add(li8Char);
        return *this;
      };

      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Add(char8* lpszStr)
       *  \brief Fügt den übergebenen String dem pcStr8 hinzu. pcStr8 wird bei Bedarf vergrößert aber nicht verkleinert
       *  \param lpszStr char8*: '0'-terminierte Zeichenkette die geschrieben weden soll.
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Add(cStrDynT<tyCStrT> const &lpcStr)
      {
        if (lpcStr.cBarryPtrT<tyCStrT>::muLen)
        {
          if ((cBarryPtrT<tyCStrT>::muLen + lpcStr.cBarryPtrT<tyCStrT>::muLen) > cBarryPtrT<tyCStrT>::muSize)
          {
            cStrDynT<tyCStrT> lcStrTemp(*this);
            Reuse(cBarryPtrT<tyCStrT>::muLen + lpcStr.cBarryPtrT<tyCStrT>::muLen);

            cStrBaseT<tyCStrT>::Add(lcStrTemp);
            cStrBaseT<tyCStrT>::Add(lpcStr);
          }
          else
          {
            cStrBaseT<tyCStrT>::Add(lpcStr);
          }
        }
        return *this;
      };


      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::iInsert(uint16 luiPos, pcStr8 lpstStr, char8* lpszStr)
       *  \brief Fügt an der angegebenen Stelle den übergebenen String in den pcStr8 hinein.
       *  \param luiPos int8: Zielposition zum einfügen. Start bei 0
       *  \param lpstStr DString8: Zielstring, hier wird eingefügt
       *  \param lpszStr char8*: Zu einfügender String
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Insert(CSTRT_UINT luiPos, char8* lpszStr)
      {
        if (cStrBaseT<tyCStrT>::isStrValid(lpszStr))
        {
          Reuse((CSTRT_UINT)cStrT_strlen(lpszStr) + cBarryPtrT<tyCStrT>::muLen);
          cStrBaseT<tyCStrT>::Insert(lpszStr);
        }
        return *this;
      }



      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::iMid(uint8 luiPos, uint8 luilen, pcStr8 lpstStrSource)
       *  \brief Kopiert ein Stück aus der Mitte in den lpstStrDest.
       *  \param luiPos int8: Startposition beginnend bei 0
       *  \param luilen int8: Anzahl zu kopierender Zeichen
       *  \param lpstStrDest DString8*: Zielstring
       *  \param lpstStrSource DString8*: Quellstring
       *  \return int8
       *  -  0: kein Fehler
       *  -  1: allg. Fehler
       *  -  2: Übergabefehler
       *  -  sonst: allg. Fehler
       *  \author Florian Kaes
       *  \section history History
       *  \since 17.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      cStrDynT& Mid(CSTRT_UINT luiPos, CSTRT_UINT luilen)
      {
        cStrDynT lszReturn;

        lszReturn = "";

        //Eigentlicher Funktionsrumpf für cStrT_Mid
        if ((luiPos >= 0) && (luilen > 0) && (luiPos < cBarryPtrT<tyCStrT>::muLen))
        {
          //Ggf. Länge clippen
          if ((luiPos + luilen) > cBarryPtrT<tyCStrT>::muLen)
          {
            luilen = cBarryPtrT<tyCStrT>::muLen - luiPos;
          }

          //Genug Speicher für String vorhanden ?
          if (luilen > lszReturn.cBarryPtrT<tyCStrT>::muSize)
          {
            //Größe anpassen
            lszReturn.Reuse(luilen);
          }

          //Daten kopieren
          cStrT_strncpy((char8*)lszReturn.cBarryPtrT<tyCStrT>::mpu8Data, (char8*)&(cBarryPtrT<tyCStrT>::mpu8Data[luiPos]), luilen);
          lszReturn.cBarryPtrT<tyCStrT>::mpu8Data[luilen] = 0;
          lszReturn.cBarryPtrT<tyCStrT>::muLen = luilen;
        }

        *this = lszReturn;

        return *this;
      };




      /****************************************************************************************/
      /*! \fn FRet cStrDynT<tyCStrT>::Cmp(pcStr8 lpstStr1, pcStr8 lpstStr2)
       *  \brief Vergleicht die beiden String miteinander
       *  \param lpstStr1 DString8: zu vergleichender String oder Wildcard
       *  \param lpstStr2 lui8Like: Überprüft ob die Strings ähnlich sind (Groß und Kleinschreibung und umschließende Leerzeichen werden ignoriert, Wildcard '*' ist möglich).
       *  \return int8: < 0 string1 less than string2; == 0 string1 identical to string2; > 0 string1 greater than string2
       *  \author Florian Kaes
       *  \section history History
       *  \since 22.11.2011
       *  - Erstellt
       */
       /****************************************************************************************/
      int8 Cmp(cStrDynT lpstStr, uint8 lui8Like)
      {
        int8 li8Ret;

        if (!lui8Like) //StrCmp
        {
          li8Ret = cStrT_strcmp((char8*)cBarryPtrT<tyCStrT>::mpu8Data, (char8*)lpstStr.cBarryPtrT<tyCStrT>::mpu8Data);
        }
        else //WildCard: Beispiel if (wildiCmp("bl?h.*", "blah.jpg"))
        {
          char8 *lszCp;
          char8 *lszMp;
          char8 *lszWild;
          char8 *lszString;

          cStrDynT lstStrTmp1;
          cStrDynT lstStrTmp2;

          lstStrTmp1 = lpstStr;
          lstStrTmp2 = *this;

          lstStrTmp1.Trim();
          lstStrTmp2.Trim();

          lstStrTmp1.Lower();
          lstStrTmp2.Lower();

          lszCp = null;
          lszMp = null;

          lszWild = lstStrTmp1.cBarryPtrT<tyCStrT>::mpu8Data;
          lszString = lstStrTmp2.cBarryPtrT<tyCStrT>::mpu8Data;

          li8Ret = 0;


          while ((*lszString) && (*lszWild != '*'))
          {
            if ((*lszWild != *lszString) && (*lszWild != '?'))
            {
              li8Ret = 1;
              break;
            }

            lszWild++;
            lszString++;
          }

          if (li8Ret == 0)
          {
            while (*lszString)
            {
              if (*lszWild == '*')
              {
                if (!*++lszWild)
                {
                  li8Ret = -1;
                  break;
                }

                lszMp = lszWild;
                lszCp = lszString + 1;

              }
              else if ((*lszWild == *lszString) || (*lszWild == '?'))
              {
                lszWild++;
                lszString++;
              }
              else
              {
                lszWild = lszMp;
                lszString = lszCp++;
              }
            }

            if (li8Ret == 0)
            {
              while (*lszWild == '*')
              {
                lszWild++;
              }
              li8Ret = !(!*lszWild);
            }
            else
            {
              li8Ret = 0;
            }
          }
        }

        return li8Ret;
      };



    //---------------------- Operatoren -----------------------------------------

    bool operator== (const cStrDynT<tyCStrT> &lszStr)
    {
      return (Cmp(lszStr) == 0);
    };

    // e.g. MyStr += "a"
    cStrDynT& operator+=(char8* lpszStr)
    {
      return Add(lpszStr);
    };

    // e.g. MyStr += 'a'
    cStrDynT& operator+=(char8 li8Char)
    {
      return Add(li8Char);
    };

    // e.g. myStr += myStr
    cStrDynT& operator+=(cStrDynT<tyCStrT> const& lpcStr)
    {
      return Add(lpcStr);
    };

    //----- Copy operator ------
    // e.g. MyStr = "a"
    cStrDynT& operator=(char8* lpszStr)
    {
      return Set(lpszStr);
    };

    // e.g. MyStr = "a"
    cStrDynT& operator=(const char8* lpszStr)
    {
      return Set(lpszStr);
    };

    // e.g. MyStr = 'a'
    cStrDynT& operator=(char8 lchSign)
    {
      return Set(lchSign);
    };

    // e.g myStr1 = myStr2;
    cStrDynT& operator=(cStrDynT const& lpcStr)
    {
      return Set(lpcStr);
    };

    // ------------------------------

    // e.g. myStr--;
    cStrDynT& operator++(int)
    {
      return cStrBaseT<tyCStrT>::Upper();
    };

    // e.g. myStr--;
    cStrDynT& operator--(int)
    {
      return cStrBaseT<tyCStrT>::Lower();
    };


    // Indexing operator - Read
    char8 operator[](CSTRT_UINT luiPos) const
    {
      if (luiPos < cBarryPtrT<tyCStrT>::muLen)
      {
        return (char8)cBarryPtrT<tyCStrT>::mpu8Data[luiPos];
      }
      return 0;
    };

    // Indexing operator - Writable a[i] = x
    char8 operator[](CSTRT_UINT luiPos)
    {
      #ifdef _WIN32
      if (luiPos >= cBarryPtrT<tyCStrT>::muLen) throw cBarryPtrT_nErr_OutofBounds;
      #endif
      return (char8)cBarryPtrT<tyCStrT>::mpu8Data[luiPos];
    };
  };


  template <typename tyCStrT>
  cStrDynT<tyCStrT> operator+(const cStrDynT<tyCStrT>& lpcStr1, const cStrDynT<tyCStrT>& lpcStr2)
  {
    cStrDynT<tyCStrT> lszTemp;
    lszTemp  = lpcStr1;
    lszTemp += lpcStr2;
    return lszTemp;
  }

  // e.g. myStr + "x"
  template <typename tyCStrT>
  cStrDynT<tyCStrT> operator+(const cStrDynT<tyCStrT>& lpcStr1, char8* lpcStr2)
  {
    cStrDynT<tyCStrT> lszTemp;
    lszTemp  = lpcStr1;
    lszTemp += lpcStr2;
    return lszTemp;
  }


  template <typename tyCStrT>
  cStrDynT<tyCStrT> operator+(char8* lpcStr1, const cStrDynT<tyCStrT>& lpcStr2)
  {
    cStrDynT<tyCStrT> lszTemp;
    lszTemp  = lpcStr1;
    lszTemp += lpcStr2;
    return lszTemp;
  }




#endif
