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

  // https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/itoa.c
  static int8 i8Itoan(int num, char8* str, int len, int base)
  {
    int sum;
    int i = 0;
    int digit;

    if (len == 0)
    {
      return -1;
    }

    if (num < 0)
    {
      num = -num;
      str[i++] = '-';
    }

    sum = num;

    if (i == (len - 1) && sum)
    {
      return -1;
    }

    do
    {
      digit = sum % base;

      if (digit < 0xA)
      {
        str[i++] = '0' + digit;
      }
      else
      {
        str[i++] = 'A' + digit - 0xA;
      }

      sum /= base;

    }
    while (sum && (i < (len - 1)));

    if (i == (len - 1) && sum)
    {
      return -1;
    }

    str[i] = '\0';
    if (str[0] == '-')
      vStrRev(&str[1], i-1);
    else
      vStrRev(str, i);

    return 0;
  }

  static int8 i8Itoa(int num, char8* str, int base)
  {
    return  i8Itoan(num, str, 0xFF, base);
  }

  // https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/itoa.c
  static char8* i8Ftoan(float num, char8* str, int len, int precision)
  {
    int i = 0;
    int inum;
    int precisionn;
    char8* str1 = str;

    if (len == 0)
    {
      return 0;
    }


    inum = int(num);

    if (inum == 0)
    {
      *str++ = '0';
      len -= 2;
    }
    else
    {
      int point;
      i8Itoan(inum, str, len, 10);
      point = uixStrLen(str);
      len -= (point + 1);
      str += point;
      num -= float(inum);
    }


    if (precision)
    {
      precisionn = 1;
      for (i = 0; i < precision; i++)
      {
        precisionn *= 10;
      }
    }
    else
    {
      precisionn = 0;
    }

    if ((precisionn) && (len > 0))
    {
      *str++  = '.';
      *str    = 0; // str = "1.\0"
      len--;
      num *= precisionn;
      inum = int(num);

      if (inum > 0)
      {
        //Count and insert leading '0'
        precisionn /= 10;
        while ((inum < precisionn) && (len > 0))
        {
          precisionn /= 10;
          *str++ = '0';
          *str = 0; // str = "1.\0"
          len--;
        }

        i8Itoan(inum, str, len, 10);
      }
      else
      {
        *str++ = '0';
        *str = 0;
        len--;
        precision--;
        if (precision)
        {
          for (i = 0; ((i < precision) && (len > 0)); i++)
          {
            *str++ = '0';
            *str = 0;
            len--;
          }
        }
      }
    }

    return str1;
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
