#ifndef _CSTRINGTOOLS_H
#define _CSTRINGTOOLS_H

#include "typedef.h"
#include "cChr8.h"

class cStrTools
{
  public:
  // http://www.ethernut.de/api/strcpy_8c_source.html
  static char8* szStrCpy(char8 *lpszDest, const char8 *lpszSrc)
  {
     char8* lpszSaved = lpszDest;
     while (*lpszSrc)
     {
         *lpszDest++ = *lpszSrc++;
     }
     *lpszDest = 0;
     return lpszSaved;
  }

  // http://www.ethernut.de/api/strncpy_8c_source.html
  static char8* szStrnCpy(char8 *lpszDest, const char8 *lpszSrc, tsize uLen)
  {
    char8* lpszSaved = lpszDest;
    while ((*lpszSrc) && (uLen > 0))
    {
      *lpszDest++ = *lpszSrc++;
      uLen--;
    }
    *lpszDest = 0;
    return lpszSaved;
  }

  // http://www.ethernut.de/api/strcmp_8c_source.html
  static int8 i8StrCmp(const char8 *lpszStr1, const char8 *lpszStr2)
  {
    while (*lpszStr1 == *lpszStr2++)
    {
      if (*lpszStr1++ == 0)
      {
        return (0);
      }
    }
    return (*(uint8 *) lpszStr1 - *(uint8 *) --lpszStr2);
  }

  // http://www.ethernut.de/api/strncmp_8c_source.html
  static int8 i8StrnCmp(const char8 *lpszStr1, const char8 *lpszStr2, tsize uLen)
  {
    if (uLen == 0) return 0;
    do
    {
      if (*lpszStr1 != *lpszStr2++)
        return (*(uint8*)lpszStr1 - *(uint8*)--lpszStr2);
      if (*lpszStr1++ == 0) break;
    } while (--uLen != 0);
    return 0;
  }

  static inline char cLcase(char lcChr)
  {
    if ((lcChr >= 'A') && (lcChr <= 'Z'))
    {
      return lcChr += ('a' - 'A');
    }
    return lcChr;
  }

  // Wie i8StrCmp nur nicht case sensetiv
  static int8 i8TextCmp(const char8 *lpszStr1, const char8 *lpszStr2)
  {
    while (cStrTools::cLcase(*lpszStr1) == cStrTools::cLcase(*lpszStr2++))
    {
      if (*lpszStr1++ == 0)
      {
        return (0);
      }
    }
    return (*(uint8 *) lpszStr1 - *(uint8 *) --lpszStr2);
  }

  // Wie i8StrCmp nur nicht case sensetiv
  static int8 i8TextnCmp(const char8 *lpszStr1, const char8 *lpszStr2, tsize uLen)
  {
    if (uLen == 0) return 0;
    do
    {
      if (cStrTools::cLcase(*lpszStr1) != cStrTools::cLcase(*lpszStr2++))
        return (*(uint8*)lpszStr1 - *(uint8*)--lpszStr2);
      if (*lpszStr1++ == 0) break;
    } while (--uLen != 0);
    return 0;
  }

  // http://www.ethernut.de/api/strcat_8c_source.html
  static char8 *szStrCat(char8 *lpszDest, const char8 *lpszSrc)
  {
    char8 *lpszRet = lpszDest;

    for (; *lpszDest; ++lpszDest);
    while ((*lpszDest++ = *lpszSrc++) != '\0');
    return lpszRet;
  }

  // http://www.cprogramming.com/snippets/source-code/how-to-implement-strlwr-in-c
  static char8 *szStrLwr(char8 *lpszSrc)
  {
    char8 *lpszTemp = lpszSrc;

    if (!lpszSrc)
    {
      return 0;
    }

    while ( *lpszTemp != '\0' )
    {
      if ((*lpszTemp >= 'A') && (*lpszTemp <= 'Z') )
      {
        *lpszTemp = *lpszTemp + ('a' - 'A');
      }
      lpszTemp++;
    }

    return lpszSrc;
  }

  static char8* szStrnLwr(char8* lpszSrc, tsize uLen)
  {
    char8* lpszTemp = lpszSrc;

    if ((!lpszSrc) || (uLen == 0))
    {
      return 0;
    }

    while ((*lpszTemp != '\0') && (uLen > 0))
    {
      if ((*lpszTemp >= 'A') && (*lpszTemp <= 'Z'))
      {
        *lpszTemp = *lpszTemp + ('a' - 'A');
      }
      lpszTemp++;
      uLen--;
    }

    return lpszSrc;
  }


  static char8 *szStrUpr(char8 *lpszSrc)
  {
    char8 *lpszTemp = lpszSrc;

    if (!lpszSrc)
    {
      return 0;
    }

    while ( *lpszTemp != '\0' )
    {
      if ((*lpszTemp >= 'a') && (*lpszTemp <= 'z') )
      {
        *lpszTemp = *lpszTemp - ('a' - 'A');
      }
      lpszTemp++;
    }

    return lpszSrc;
  }

  // http://www.ethernut.de/api/strlen_8c_source.html
  static u32 uixStrLen(const char8 *lpszSrc)
  {
    const char8 *lpszTemp;

    for (lpszTemp = lpszSrc; *lpszTemp; ++lpszTemp);
    return (lpszTemp - lpszSrc);
  }

  // https://opensource.apple.com/source/Libc/Libc-1158.30.7/string/FreeBSD/strnlen.c.auto.html
  static u32 uixStrnLen(const char8* lpszSrc, u32 maxlen)
  {
    u32 len;
    for (len = 0; (*lpszSrc) && (len < maxlen); len++, lpszSrc++);
    return (len);
  }

  static u32 ui32StartOfText(const char8* lpszSrc, u32 li32StartIdx = 0)
  {
    const char8* lpszTemp;
    for (lpszTemp = &lpszSrc[li32StartIdx]; *lpszTemp; lpszTemp++)
    {
      if (!cChr8Tools::IsSpace(*lpszTemp)) return li32StartIdx;
      li32StartIdx++;
    }
    return li32StartIdx;
  }

  static u32 ui32EndOfText(const char8* lpszSrc, u32 li32StartIdx = 0)
  {
    const char8* lpszTemp= &lpszSrc[li32StartIdx];
    if (cChr8Tools::IsSpace(*lpszTemp)) return li32StartIdx;
    for (; *lpszTemp; lpszTemp++)
    {
      if (cChr8Tools::IsSpace(*lpszTemp)) return li32StartIdx - 1;
      li32StartIdx++;
    }
    return li32StartIdx;
  }

  //https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/string/strrev.c
  static void vStrRev(char8* str)
  {
    u32 i;
    u32 j;
    uint8 a;
    u32 len = uixStrLen((const char8 *)str);

    for (i = 0, j = len - 1; i < j; i++, j--)
    {
      a = str[i];
      str[i] = str[j];
      str[j] = a;
    }
  }

  static void vStrRev(char8* str, u32 len)
  {
    u32 i;
    u32 j;
    uint8 a;

    for (i = 0, j = len - 1; i < j; i++, j--)
    {
      a = str[i];
      str[i] = str[j];
      str[j] = a;
    }
  }


  // Gibt die Länge in Bytes zurück
  static u32 uixIton(int num, int base)
  {
    u32 luSize = 0;
    if (!num) return 1;
    switch (base)
    {
      case 2:  // Binär
        while ((u32)num) {num = (u32)num >> 1; luSize++;}; break;
      case 8:  // Oktal
        while ((u32)num) {num = (u32)num >> 3; luSize++;}; break;
      case 16: // Hex
        while ((u32)num) {num = (u32)num >> 4; luSize++;}; break;
      default:
        if (num < 0) // neg. Zahl nur bei dezimal, platz für ein '-' reservieren
        {
          luSize++;
        }
        while (num) {num /= base; luSize++;}; break;
    }
    return luSize;
  }

  // https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/itoa.c
  // Gibt die Länge in Bytes zurück
  // Wenn die Zahl zu lang ist für den string, z.B. Länge Zahl > len, dann gibt es kein korrektes clipping
  // und das Ergebnis wird eine falsche Zahl sein
  // Vorsicht: keine null terminierung
  static u32 uixItoan_open(int num, char8* str, int len, int base, u8 toupper)
  { 
    u8  Size;
    u8  digit;
    int i = 0;

    if (len == 0)
    {
      return i;
    }

    Size = uixIton(num, base);

    if (Size > len) Size = len;
    str = str + (Size - 1);
    i = Size;

    switch (base)
    {
      case 2:  // Binär
        while (i)
        {
          *str-- = '0' + (num & 1);
          num = (u32)num >> 1;
          i--;
        }
        break;
      case 8:  // Oktal
        while (i)
        {
          *str-- = '0' + (num & 7);
          num = (u32)num >> 3;
          i--;
        }
        break;
      case 16: // Hex
        while (i)
        {
          digit = (num & 15);
          if (digit < 0xA)
          {
            *str-- = '0' + digit;
          }
          else
          {
            if (toupper)
              *str-- = 'A' + digit - 0xA;
            else
              *str-- = 'a' + digit - 0xA;
          }
          num = (u32)num >> 4;
          i--;
        }
        break;
      case 10: // dezimal
        u8 sign;
        if (num < 0) // neg. Zahl nur bei dezimal, platz für ein '-' reservieren
        {
          i--;
          num = -num; 
          sign = 1;
        }
        else
        {
          sign = 0;
        }

        while (i)
        {
          *str-- = '0' + (num % 10);
          num /= 10;
          i--;
        }

        if (sign)
        {
          *str = '-';
        }
        break;
    }
    return Size;
  }
  
  static u32 uixItoan_open(int num, char8* str, int len, int base)
  {
    return  uixItoan_open(num, str, len, base, 1);
  }

  static u32 uixItoan(int num, char8* str, int len, int base)
  {
    u32 Size;
    
    if (len > 1)
    {
      Size = uixItoan_open(num, str, len - 1, base, 1);
      str += Size;
      *str = 0;
    }
    else
    {
      Size = 0;
      if (len > 0)
      {      
        *str = 0;
      }
    }

    return Size;
  }

  static u32 uixItoa(int num, char8* str, int base)
  {
    return  uixItoan(num, str, 0xFF, base);
  }

  // Gibt die Länge in Bytes zurück
  // expon: länge des nachkommateils
  static u32 uixFton(float num, int precision, u8 &expon)
  {
    expon = 0;
    if (num != 0)
    {
      u32 luSize;
      int inum = int(num);

      luSize = uixIton(inum, 10);

      if ((inum == 0) && (num < 0)) // z.B. -0.5
      {
        luSize++; // für '-'
      }

      if (precision)
      {
        luSize++; // für '.'

        // Nachkommateil
        num -= inum;
        if (num < 0) num = -num;

        for (i8 t = 0; t < precision; t++)
        {
          num *= 10;
        }

        inum = int(num);

        // ggf. runden
        if ((num - inum) > 0.5f)
        {
          inum++;
        }

        if (inum)
        {
          while ((inum % 10) == 0) //Nullen entfernen
          {
            inum /= 10;
            precision--;
          }

          luSize += precision;
          expon += precision;
        }
        else
        {
          luSize++; // für '0'
          expon++;
        }
      }
      return luSize;
    }
    else
    {
      if (precision)
      {
        expon = 1;
        return 3;
      }
      else
      {
        return 1;
      }
    }
  }

  // https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/itoa.c
  // Gibt die Länge in Bytes zurück
  // Wenn die Zahl zu lang ist für den string, z.B. Länge Zahl > len, dann gibt es kein korrektes clipping
  // und das Ergebnis wird eine falsche Zahl sein
  // Vorsicht: keine null terminierung
  static u32 uixFtoan_open(float num, char8* str, int len, int precision)
  {
    i16 i;
    int inum;
    u8 expon;

    if (len == 0)
    {
      return 0;
    }

    if (precision > 9) precision = 9;

    uixFton(num, precision, expon);

    inum = int(num);

    if ((inum == 0) && (num < 0)) // z.B. -0.5
    {
      if (len > 1)
      {
        *str++ = '-';
        *str++ = '0';
        i = 2;
      }
      else
      {
        *str++ = '-';
        i = 1;
      }
    }
    else
    {
      i = uixItoan_open(inum, str, len, 10, 0);
      str += i;
    }

    len -= i;

    if ((expon > 0) && (len)) // Nachkommateil
    {
      *str++ = '.';
      len--;
      i++;

      if (len)
      {
        u8 t;
        i32 expoval = 1;
        num -= inum;
        if (num < 0) num = -num;

        for (t = 0; t < expon; t++)
        {
          num *= 10;
        }
        inum = int(num);

        if (inum)
        {
          // ggf. runden
          if ((num - inum) > 0.5f)
          {
            inum++;
          }

          // Führende nullen
          for (t = 0; expoval <= inum; t++)
          {
            expoval *= 10;
          }

          for (; t < expon; t++)
          {
            *str++ = '0';
            len--;
            i++;
          }

          i += uixItoan_open(inum, str, len, 10, 0);
        }
        else
        {
          if (len)
          {
            *str++ = '0';
            len--;
            i++;
          }
        }
      }
    }

    return i;
  }

  static u32 uixFtoan(float num, char8* str, int len, int precision)
  {
    u32 Size;
    
    if (len > 1)
    {
      Size = uixFtoan_open(num, str, len - 1, precision);
      str += Size;
      *str = 0;
    }
    else
    {
      Size = 0;
      if (len > 0)
      {      
        *str = 0;
      }
    }

    return Size;
  }


  // https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/atoi.c
  static int hexval(char8 c)
  {
    if (c >= '0' && c <= '9')      return c - '0';
    else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') return c - 'A' + 10;

    return 0;
  }

  static int atoi(const char8 *num)
  {
    long value = 0;
    int neg = 0;

    if (num[0] == '0' && num[1] == 'x')
    {
      // hex
      num += 2;
      while (*num && cChr8Tools::IsDigit(*num))
        value = value * 16 + hexval(*num++);
    }
    else
    {
      // decimal
      if (num[0] == '-')
      {
        neg = 1;
        num++;
      }
      while (*num && cChr8Tools::IsDigit(*num))
        value = value * 10 + *num++  - '0';
    }

    if (neg)
      value = -value;

    return value;
  }

  static int atoin(const char8 *num, int len)
  {
    long value = 0;
    int neg = 0;

    if (len == 0)
    {
      return 0;
    }

    if ((num[0] == '0') && (num[1] == 'x') && (len > 2))
    {
      // hex
      num += 2;
      while (*num && cChr8Tools::IsDigit(*num))
        value = value * 16 + hexval(*num++);
    }
    else
    {
      // decimal
      if (num[0] == '-')
      {
        neg = 1;
        num++;
        len--;
      }
      while ((*num) && (cChr8Tools::IsDigit(*num)) && (len > 0))
      {
        value = value * 10 + *num++  - '0';
        len--;
      }
    }

    if (neg)
      value = -value;

    return value;
  }

};



#endif /* _CSTRINGTOOLS_H */
