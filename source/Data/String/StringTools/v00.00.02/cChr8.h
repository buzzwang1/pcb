

#ifndef _CCHR8_H
  #define _CCHR8_H

  #include "TYPEDEF.h"

  #define CCHR8_INT            int
  #define CCHR8_UINT           unsigned int

  #define cChr8_isdigit cStrTools::i8IsDigit
  #define cChr8_isspace cStrTools::i8IsSpace

  class cChr8Tools
  {
    public:
      inline static bool IsSpace(char8 lchSign)       {return ((lchSign >= 0x09 && lchSign <= 0x0D) || (lchSign == 0x20));}
      inline static bool IsLetter(char8 lchSign)      {return ((((lchSign >= 'A') && (lchSign <= 'Z')) ||
                                                                ((lchSign >= 'a') && (lchSign <= 'z'))));}

      inline static bool IsDigit(char8 lchSign)       {return (lchSign >= '0' && lchSign <= '9');}
      inline static bool IsHexDigit(char8 lchSign)    {return (((lchSign >= 'A') && (lchSign <= 'F')) ||
                                                               ((lchSign >= 'a') && (lchSign <= 'f')) ||
                                                               (IsDigit(lchSign)));}

      inline static char8 ToDigit(u8 lu8Value)
      {
        if (lu8Value < 10)  return ('0' + lu8Value);
        return '?';
      }

      inline static char8 ToHexDigit(u8 lu8Value)
      {
        if (lu8Value < 10)  return ('0' + lu8Value);
        lu8Value -= 10;
        if (lu8Value < 16)  return ('A' + lu8Value);
        return '?';
      }

      inline static bool IsCharAs(char8* lszStr, char8 lchSign)
      {
        if (lszStr != null)
        {
          while (*lszStr)
          {
            if (*lszStr != lchSign)
            {
              return True;
            }
            lszStr++;
          }
        }
        return False;
      }
  };

  class cChr8
  {
    public:
      char8           mi8Data;

      inline cChr8(void)                             {mi8Data = 0;};
      inline cChr8(char8  li8Char)                   {mi8Data = li8Char;};

      inline cChr8& operator=(char8  li8Char)        {mi8Data = li8Char; return *this;};
      inline cChr8& operator=(cChr8 const &lcChr)    {mi8Data = lcChr.mi8Data; return *this;};

      inline cChr8& operator+=(char8 li8Char)        {mi8Data += li8Char; return *this;};
      inline cChr8& operator+=(cChr8 const &lcChr)   {mi8Data += lcChr.mi8Data; return *this;};

      bool  operator== (const char8 &li8Char)        {return (mi8Data == li8Char);};
      bool  operator== (const cChr8 &lcChr)          {return (mi8Data == lcChr.mi8Data);};

      inline bool IsSpace()                 {return cChr8Tools::IsSpace(mi8Data);};
      inline bool IsLetter()                {return cChr8Tools::IsLetter(mi8Data);};
      inline bool IsDigit()                 {return cChr8Tools::IsDigit(mi8Data);};
      inline bool IsHexDigit()              {return cChr8Tools::IsHexDigit(mi8Data);};
      inline bool IsCharAs(char8* lszStr)   {return cChr8Tools::IsCharAs(lszStr, mi8Data);}

      inline void ToDigit(u8 lu8Value)      {mi8Data = cChr8Tools::IsHexDigit(lu8Value);}
      inline void ToHexDigit(u8 lu8Value)   {mi8Data = cChr8Tools::IsHexDigit(lu8Value);}
  };

  //char8 operator=(cChr8 const &lcChr)        {return lcChr.mi8Data;};

#endif // _CCHR8_H
