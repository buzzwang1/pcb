#ifndef _C_SAFETY_HW_H
#define _C_SAFETY_HW_H

#include "TYPEDEF.h"
#include "MemTools.h"


class cSafetyHw
{
  public:

  static u8 u8CheckSum8(const u8* lpu8Data, u32 lu32Count, u8 lu8InitValue = 0)
  {
    u8 lu8Ret = lu8InitValue;

    while (lu32Count)
    {
      lu8Ret += *lpu8Data++;
      lu32Count--;
    }
    return lu8Ret;
  }

  static u16 u16CheckSum16(const u8* lpu8Data, u32 lu32Count, u16 lu16InitValue = 0)
  {
    u16 lu16Ret = lu16InitValue;

    while (lu32Count)
    {
      lu16Ret += *lpu8Data++;
      lu32Count--;
    }
    return lu16Ret;
  }

  static u32 u32CheckSum32(const u8* lpu8Data, u32 lu32Count, u32 lu32InitValue = 0)
  {
    u16 lu32Ret = lu32InitValue;

    while (lu32Count)
    {
      lu32Ret += *lpu8Data++;
      lu32Count--;
    }
    return lu32Ret;
  }

  // ------------------------------- CRC32 -------------------------------------
  // CRC32 code copied from https://github.com/Michaelangel007/crc32
  /*static u32 u32Crc32(const u8* lpu8Data, u32 lu32Count, u32 lu32Poly = 0x04C11DB7, u32 lu32InitValue = 0xFFFFFFFF)
  {
    u32 lu32Ret = lu32InitValue;

    if (lpu8Data == NULL) return 0xffffffff;

    while (lu32Count--)
    {
      lu32Ret ^= (*lpu8Data++ << 24);
      for (u8 lu8k = 0; lu8k < 8; lu8k++)
      {
        if (lu32Ret & 0x80000000) lu32Ret = (lu32Ret << 1) ^ lu32Poly;
                             else lu32Ret = lu32Ret << 1;
      }
    }

    return ~lu32Ret;
  }*/

  // u32 lu32Poly = 0x04C11DB7, u32 lu32InitValue = 0xFFFFFFFF
  static u32 u32Crc32(const u8* lpu8Data, u32 lu32Count)
  {
    u32 lu32Ret = ~0;

    while (lu32Count--)
    {
      lu32Ret ^= *lpu8Data++;

      for (int i = 0; i < 8; i++)
      {
        u32 t = ~((lu32Ret & 1) - 1); lu32Ret = (lu32Ret >> 1) ^ (0xEDB88320 & t);
      }
    }

    return ~lu32Ret;
  }




  // ------------------------------- CRC16 -------------------------------------
  // CRC16 code copied from https://stackoverflow.com/questions/17196743/crc-ccitt-implementation
  static u16 u16Crc16(const u8* lpu8Data, u32 lu32Count, u16 lu16Poly = 0x1021, u16 lu16InitValue = 0)
  {
    u16 lu16Ret = lu16InitValue;

    if (lpu8Data == NULL) return lu16InitValue;

    while (lu32Count--)
    { 
      lu16Ret ^= (*lpu8Data++ << 8);
      for (u8 lu8k = 0; lu8k < 8; lu8k++)
      {
        if (lu16Ret & 0x8000) lu16Ret = (lu16Ret << 1) ^ lu16Poly;
                         else lu16Ret = lu16Ret << 1;
      }
    }

    return lu16Ret;
  }


  // ------------------------------- CRC8 -------------------------------------
  // CRC8 code copied from https://stackoverflow.com/questions/51752284/how-to-calculate-crc8-in-c

  static u8 u8Crc8(const u8* lpu8Data, u32 lu32Count, u8 lu8Poly = 0x31, u8 lu8InitValue = 0)
  {
    u8 lu8Ret = lu8InitValue;
    
    if (lpu8Data == NULL) return lu8InitValue;

    while (lu32Count--)
    {
      lu8Ret ^= *lpu8Data++;
      for (u8 lu8k = 0; lu8k < 8; lu8k++)
      {
        if (lu8Ret & 0x80) lu8Ret = (lu8Ret << 1) ^ lu8Poly;
                      else lu8Ret = lu8Ret << 1;
      }
    }

    return lu8Ret;
  }


  static u8 u8Crc8b(const u8* lpu8Data, u32 lu32Count, u8 lu8InitValue = 0) 
  {
    static unsigned char const nCrc8_Table_Poly0x31[] = {
      0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d,
      0x4c, 0x1f, 0x2e, 0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb,
      0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d, 0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20,
      0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8, 0xc5, 0xf4, 0xa7, 0x96,
      0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb, 0x3d,
      0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71,
      0x22, 0x13, 0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5,
      0x94, 0x03, 0x32, 0x61, 0x50, 0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c,
      0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95, 0xf8, 0xc9, 0x9a, 0xab, 0x3c,
      0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6, 0x7a, 0x4b,
      0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65,
      0x54, 0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3,
      0x44, 0x75, 0x26, 0x17, 0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45,
      0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2, 0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a,
      0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91, 0x47, 0x76, 0x25,
      0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
      0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79,
      0x48, 0x1b, 0x2a, 0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49,
      0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef, 0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24,
      0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac};

    u8 lu8Ret = lu8InitValue;
    
    if (lpu8Data == NULL) return 0xff;

    while (lu32Count--)
    {
      lu8Ret = nCrc8_Table_Poly0x31[lu8Ret ^ *lpu8Data++];
    }

    return lu8Ret;
  }




  //-------------------------------- MD5 --------------------------------------

  // MD5 code copied from https://gist.github.com/creationix/4710780
  // leftrotate function definition
  inline static u32 u32Md5LeftRotate(u32 x, u32 c) 
  {
    return (((x) << (c)) | ((x) >> (32 - (c))));
  }
 
  static void vMd5(const u8* lpu8Data, u32 lu32Count, u8 lpu8Result[16]) 
  {
      // Message (to prepare)
      u8 *msg = NULL;

      // These vars will contain the hash
      u32 h[4];

 
      // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating 
      // r specifies the per-round shift amounts
      u32 r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

      // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
      u32 k[] = {
          0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
          0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
          0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
          0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
          0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
          0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
          0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
          0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
          0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
          0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
          0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
          0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
          0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
          0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
          0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
          0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
 
      h[0] = 0x67452301;
      h[1] = 0xefcdab89;
      h[2] = 0x98badcfe;
      h[3] = 0x10325476;
 
      // Pre-processing: adding a single 1 bit
      //append "1" bit to message    
      /* Notice: the input bytes are considered as bits strings,
         where the first bit is the most significant bit of the byte.[37] */
 
      // Pre-processing: padding with zeros
      //append "0" bit until message length in bit ≡ 448 (mod 512)
      //append length mod (2 pow 64) to message
 
      i32 new_len = ((((lu32Count + 8) / 64) + 1) * 64) - 8;
 
      msg = new u8[new_len + 64]; // also appends "0" bits 
                                  // (we alloc also 64 extra bytes...)
      cMemTools::vMemSet(msg, 0, new_len + 64);

      cMemTools::vMemCpy(msg, (u8*)lpu8Data, lu32Count);
      msg[lu32Count] = 128; // write the "1" bit
 
      u32 bits_len = 8 * lu32Count; // note, we append the len    
      cMemTools::vMemCpy(msg + new_len, (u8*)&bits_len, 4);           // in bits at the end of the buffer
 
      // Process the message in successive 512-bit chunks:
      //for each 512-bit chunk of message:
      i32 offset;
      for(offset=0; offset<new_len; offset += (512/8))
      {
        // break chunk i32o sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        u32 *w = (u32 *) (msg + offset);

        // Initialize hash value for this chunk:
        u32 a = h[0];
        u32 b = h[1];
        u32 c = h[2];
        u32 d = h[3];
 
        // Main loop:
        u32 i;
        for(i = 0; i<64; i++) 
        {
          u32 f, g;
 
          if (i < 16) 
          {
              f = (b & c) | ((~b) & d);
              g = i;
          } 
          else if (i < 32) 
          {
              f = (d & b) | ((~d) & c);
              g = (5*i + 1) % 16;
          } 
          else if (i < 48) 
          {
              f = b ^ c ^ d;
              g = (3*i + 5) % 16;
          } 
          else 
          {
              f = c ^ (b | (~d));
              g = (7*i) % 16;
          }

          u32 temp = d;
          d = c;
          c = b;
          b = b + u32Md5LeftRotate((a + f + k[i] + w[g]), r[i]);
          a = temp;
        }
 
        // Add this chunk's hash to result so far:
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
      } 
      // cleanup
      delete [] msg;
      
      cMemTools::vMemCpy(lpu8Result, (u8*)h, 16);
    }
};

class cShaTools
{
public:
  inline static u32 min(u32 a, u32 b)      { return a > b ? b : a;}

  inline static u32 rotlFixed(u32 x, u32 y) { return (x << y) | (x >> (sizeof(y) * 8 - y)); }
  inline static u32 rotrFixed(u32 x, u32 y) { return (x >> y) | (x << (sizeof(y) * 8 - y)); }

  inline static u32 ByteReverseWord(u32 value)
  {
    /* 6 instructions with rotate instruction, 8 without */
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    return rotlFixed(value, 16U);
  }

  inline static void ByteReverseWords(u32* out, const u32* in, u32 byteCount)
  {
    u32 count = byteCount / sizeof(u32), i;

    for (i = 0; i < count; i++)
    {
      out[i] = ByteReverseWord(in[i]);
    }
  }

  inline static void ByteReverseBytes(u8* out, const u8* in, u32 byteCount)
  {
    u32* op = (u32*)out;
    const u32* ip = (const u32*)in;
    ByteReverseWords(op, ip, byteCount);
  }

  inline static u32 Ch(u32 x, u32 y, u32 z)   { return  (z ^ (x & (y ^ z))); }
  inline static u32 Maj(u32 x, u32 y, u32 z)  { return  (((x | y) & z) | (x & y)); }
  inline static u32 S(u32 x, u32 n)           { return  rotrFixed(x, n); }
  inline static u32 R(u32 x, u32 n)           { return  (((x) & 0xFFFFFFFFL) >> (n)); }
  inline static u32 Sigma0(u32 x)             { return  (S(x, 2) ^ S(x, 13) ^ S(x, 22)); }
  inline static u32 Sigma1(u32 x)             { return  (S(x, 6) ^ S(x, 11) ^ S(x, 25)); }
  inline static u32 Gamma0(u32 x)             { return  (S(x, 7) ^ S(x, 18) ^ R(x, 3)); }
  inline static u32 Gamma1(u32 x)             { return  (S(x, 17) ^ S(x, 19) ^ R(x, 10)); }
};

class cSha1
{
public:
  // SHA1 code copied from https://github.com/CausticUC/STM32-Bootloader/blob/master/ExtLibraries/FreeRTOSV7.4.2/FreeRTOS-Plus/Source/CyaSSL/ctaocrypt/src/sha.c

  /* in bytes */
  enum
  {
    SHA1 = 1,   /* hash type unique */
    SHA1_BLOCK_SIZE = 64,
    SHA1_DIGEST_SIZE = 20,
    SHA1_PAD_SIZE = 56
  };

  u32  mBuffLen;   /* in bytes          */
  u32  mLoLen;     /* length in bytes   */
  u32  mHiLen;     /* length in bytes   */
  u32  mDigest[SHA1_DIGEST_SIZE / sizeof(u32)];
  u32  mBuffer[SHA1_BLOCK_SIZE  / sizeof(u32)];

  inline void AddLength(u32 len)
  {
    u32 tmp = mLoLen;
    if ((mLoLen += len) < tmp)
    {
      mHiLen++;                       /* carry low to high */
    }
  }

  cSha1()
  {
    Init();
  }


  void Init()
  {
    mDigest[0] = 0x67452301L;
    mDigest[1] = 0xEFCDAB89L;
    mDigest[2] = 0x98BADCFEL;
    mDigest[3] = 0x10325476L;
    mDigest[4] = 0xC3D2E1F0L;

    mBuffLen = 0;
    mLoLen = 0;
    mHiLen = 0;
  }

  inline u32 blk0(u32 i, u32* W) {return (W[i] = mBuffer[i]);}
  inline u32 blk1(u32 i, u32* W) {return (W[i&15] = cShaTools::rotlFixed(W[(i+13)&15]^W[(i+8)&15]^W[(i+2)&15]^W[i&15],1));}

  inline u32 f1(u32 x, u32 y, u32 z) {return (z^(x &(y^z)));}
  inline u32 f2(u32 x, u32 y, u32 z) {return (x^y^z);}
  inline u32 f3(u32 x, u32 y, u32 z) {return ((x&y)|(z&(x|y)));}
  inline u32 f4(u32 x, u32 y, u32 z) {return (x^y^z);}

  /* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
  inline void R0(u32 v, u32 &w, u32 x, u32 y, u32 &z, u32 i, u32* W) {z+= f1(w,x,y) + blk0(i, W) + 0x5A827999 + cShaTools::rotlFixed(v,5); w = cShaTools::rotlFixed(w,30);}
  inline void R1(u32 v, u32 &w, u32 x, u32 y, u32 &z, u32 i, u32* W) {z+= f1(w,x,y) + blk1(i, W) + 0x5A827999 + cShaTools::rotlFixed(v,5); w = cShaTools::rotlFixed(w,30);}
  inline void R2(u32 v, u32 &w, u32 x, u32 y, u32 &z, u32 i, u32* W) {z+= f2(w,x,y) + blk1(i, W) + 0x6ED9EBA1 + cShaTools::rotlFixed(v,5); w = cShaTools::rotlFixed(w,30);}
  inline void R3(u32 v, u32 &w, u32 x, u32 y, u32 &z, u32 i, u32* W) {z+= f3(w,x,y) + blk1(i, W) + 0x8F1BBCDC + cShaTools::rotlFixed(v,5); w = cShaTools::rotlFixed(w,30);}
  inline void R4(u32 v, u32 &w, u32 x, u32 y, u32 &z, u32 i, u32* W) {z+= f4(w,x,y) + blk1(i, W) + 0xCA62C1D6 + cShaTools::rotlFixed(v,5); w = cShaTools::rotlFixed(w,30);}

  void Transform()
  {
    u32 W[SHA1_BLOCK_SIZE / sizeof(u32)];

    /* Copy context->state[] to working vars */
    u32 a = mDigest[0];
    u32 b = mDigest[1];
    u32 c = mDigest[2];
    u32 d = mDigest[3];
    u32 e = mDigest[4];

    /* nearly 1 K bigger in code size but 25% faster  */
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a, b, c, d, e, 0,  W); R0(e, a, b, c, d, 1,  W); R0(d, e, a, b, c, 2,  W); R0(c, d, e, a, b, 3,  W);
    R0(b, c, d, e, a, 4,  W); R0(a, b, c, d, e, 5,  W); R0(e, a, b, c, d, 6,  W); R0(d, e, a, b, c, 7,  W);
    R0(c, d, e, a, b, 8,  W); R0(b, c, d, e, a, 9,  W); R0(a, b, c, d, e, 10, W); R0(e, a, b, c, d, 11, W);
    R0(d, e, a, b, c, 12, W); R0(c, d, e, a, b, 13, W); R0(b, c, d, e, a, 14, W); R0(a, b, c, d, e, 15, W);

    R1(e, a, b, c, d, 16, W); R1(d, e, a, b, c, 17, W); R1(c, d, e, a, b, 18, W); R1(b, c, d, e, a, 19, W);

    R2(a, b, c, d, e, 20, W); R2(e, a, b, c, d, 21, W); R2(d, e, a, b, c, 22, W); R2(c, d, e, a, b, 23, W);
    R2(b, c, d, e, a, 24, W); R2(a, b, c, d, e, 25, W); R2(e, a, b, c, d, 26, W); R2(d, e, a, b, c, 27, W);
    R2(c, d, e, a, b, 28, W); R2(b, c, d, e, a, 29, W); R2(a, b, c, d, e, 30, W); R2(e, a, b, c, d, 31, W);
    R2(d, e, a, b, c, 32, W); R2(c, d, e, a, b, 33, W); R2(b, c, d, e, a, 34, W); R2(a, b, c, d, e, 35, W);
    R2(e, a, b, c, d, 36, W); R2(d, e, a, b, c, 37, W); R2(c, d, e, a, b, 38, W); R2(b, c, d, e, a, 39, W);

    R3(a, b, c, d, e, 40, W); R3(e, a, b, c, d, 41, W); R3(d, e, a, b, c, 42, W); R3(c, d, e, a, b, 43, W);
    R3(b, c, d, e, a, 44, W); R3(a, b, c, d, e, 45, W); R3(e, a, b, c, d, 46, W); R3(d, e, a, b, c, 47, W);
    R3(c, d, e, a, b, 48, W); R3(b, c, d, e, a, 49, W); R3(a, b, c, d, e, 50, W); R3(e, a, b, c, d, 51, W);
    R3(d, e, a, b, c, 52, W); R3(c, d, e, a, b, 53, W); R3(b, c, d, e, a, 54, W); R3(a, b, c, d, e, 55, W);
    R3(e, a, b, c, d, 56, W); R3(d, e, a, b, c, 57, W); R3(c, d, e, a, b, 58, W); R3(b, c, d, e, a, 59, W);

    R4(a, b, c, d, e, 60, W); R4(e, a, b, c, d, 61, W); R4(d, e, a, b, c, 62, W); R4(c, d, e, a, b, 63, W);
    R4(b, c, d, e, a, 64, W); R4(a, b, c, d, e, 65, W); R4(e, a, b, c, d, 66, W); R4(d, e, a, b, c, 67, W);
    R4(c, d, e, a, b, 68, W); R4(b, c, d, e, a, 69, W); R4(a, b, c, d, e, 70, W); R4(e, a, b, c, d, 71, W);
    R4(d, e, a, b, c, 72, W); R4(c, d, e, a, b, 73, W); R4(b, c, d, e, a, 74, W); R4(a, b, c, d, e, 75, W);
    R4(e, a, b, c, d, 76, W); R4(d, e, a, b, c, 77, W); R4(c, d, e, a, b, 78, W); R4(b, c, d, e, a, 79, W);

    /* Add the working vars back into digest state[] */
    mDigest[0] += a;
    mDigest[1] += b;
    mDigest[2] += c;
    mDigest[3] += d;
    mDigest[4] += e;
  }


  void Update(const u8* data, u32 len)
  {
    /* do block size increments */
    u8* local = (u8*)mBuffer;

    while (len)
    {
      u32 add = cShaTools::min(len, SHA1_BLOCK_SIZE - mBuffLen);
      cMemTools::vMemCpy((u8*)&local[mBuffLen], (u8*)data, add);

      mBuffLen += add;
      data += add;
      len -= add;

      if (mBuffLen == SHA1_BLOCK_SIZE)
      {
        #ifdef LITTLE_ENDIAN_ORDER
          ByteReverseBytes(local, local, SHA1_BLOCK_SIZE);
        #endif
        Transform();
        AddLength(SHA1_BLOCK_SIZE);
        mBuffLen = 0;
      }
    }
  }


  void Final(u8* hash)
  {
    u8* local = (u8*)mBuffer;

    AddLength(mBuffLen);               /* before adding pads */

    local[mBuffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (mBuffLen > SHA1_PAD_SIZE) {
      cMemTools::vMemSet((u8*)&local[mBuffLen], 0, SHA1_BLOCK_SIZE - mBuffLen);
      mBuffLen += SHA1_BLOCK_SIZE - mBuffLen;

      #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseBytes(local, local, SHA1_BLOCK_SIZE);
      #endif
      Transform();
      mBuffLen = 0;
    }
    cMemTools::vMemSet((u8*)&local[mBuffLen], 0, SHA1_PAD_SIZE - mBuffLen);

    /* put lengths in bits */
    mHiLen = (mLoLen >> (8 * sizeof(mLoLen) - 3)) + (mHiLen << 3);
    mLoLen = mLoLen << 3;

    /* store lengths */
    #ifdef LITTLE_ENDIAN_ORDER
      ByteReverseBytes(local, local, SHA1_BLOCK_SIZE);
    #endif
    /* ! length ordering dependent on digest endian type ! */
    cMemTools::vMemCpy((u8*)&local[SHA1_PAD_SIZE], (u8*)&mHiLen, sizeof(u32));
    cMemTools::vMemCpy((u8*)&local[SHA1_PAD_SIZE + sizeof(u32)], (u8*)&mLoLen, sizeof(u32));

    Transform();
    #ifdef LITTLE_ENDIAN_ORDER
      ByteReverseWords(mDigest, mDigest, SHA1_DIGEST_SIZE);
    #endif
    cMemTools::vMemCpy((u8*)hash, (u8*)mDigest, SHA1_DIGEST_SIZE);

    Init();  /* reset state */
  }
};


class cSha256
{
  public:  
  // SHA256 code copied from https://github.com/CausticUC/STM32-Bootloader/blob/master/ExtLibraries/FreeRTOSV7.4.2/FreeRTOS-Plus/Source/CyaSSL/ctaocrypt/src/sha256.c


  /* in bytes */
  enum 
  {
    SHA256              =  2,   /* hash type unique */
    SHA256_BLOCK_SIZE   = 64,
    SHA256_DIGEST_SIZE  = 32,
    SHA256_PAD_SIZE     = 56
  };


  inline void AddLength(u32 len)
  {
    u32 tmp = mLoLen;
    if ((mLoLen += len) < tmp)
    {
      mHiLen++;                       /* carry low to high */
    }
  }

  /* Sha1 mDigest */
  u32  mBuffLen;   /* in bytes          */
  u32  mLoLen;     /* length in bytes   */
  u32  mHiLen;     /* length in bytes   */
  u32  mDigest[SHA256_DIGEST_SIZE / sizeof(u32)];
  u32  mBuffer[SHA256_BLOCK_SIZE  / sizeof(u32)];

  cSha256()
  {
    Init();
  }

  void Init()
  {
    mDigest[0] = 0x6A09E667L;
    mDigest[1] = 0xBB67AE85L;
    mDigest[2] = 0x3C6EF372L;
    mDigest[3] = 0xA54FF53AL;
    mDigest[4] = 0x510E527FL;
    mDigest[5] = 0x9B05688CL;
    mDigest[6] = 0x1F83D9ABL;
    mDigest[7] = 0x5BE0CD19L;

    mBuffLen = 0;
    mLoLen   = 0;
    mHiLen   = 0;
  }

  static const u32 K[64];

  inline void RND(u32 a, u32 b, u32 c, u32 &d, u32 e, u32 f, u32 g, u32 &h, u32 i, u32 &t0, u32 &t1, u32* W)
  {
    t0 = h + cShaTools::Sigma1(e) + cShaTools::Ch(e, f, g) + K[i] + W[i];
    t1 = cShaTools::Sigma0(a) + cShaTools::Maj(a, b, c);
    d += t0;
    h  = t0 + t1;
  }

  void Transform()
  {
    u32 S[8], W[64], t0, t1;
    int i;

    /*#define RND(a,b,c,d,e,f,g,h,i) \
         t0 = h + cShaTools::Sigma1(e) + cShaTools::Ch(e, f, g) + K[i] + W[i]; \
         t1 = cShaTools::Sigma0(a) + cShaTools::Maj(a, b, c); \
         d += t0; \
         h  = t0 + t1;*/

    /* Copy context->state[] to working vars */
    for (i = 0;  i < 8;  i++)  S[i] = mDigest[i];
    for (i = 0;  i < 16; i++)  W[i] = mBuffer[i];
    for (i = 16; i < 64; i++)  W[i] = cShaTools::Gamma1(W[i-2]) + W[i-7] + cShaTools::Gamma0(W[i-15]) + W[i-16];

    for (i = 0; i < 64; i += 8) 
    {
      RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0,t0,t1,W);
      RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1,t0,t1,W);
      RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2,t0,t1,W);
      RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3,t0,t1,W);
      RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4,t0,t1,W);
      RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5,t0,t1,W);
      RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6,t0,t1,W);
      RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7,t0,t1,W);
    }

    /* Add the working vars back into mDigest state[] */
    for (i = 0; i < 8; i++) 
    { 
      mDigest[i] += S[i];
    }
  }


  void Update(const u8* data, u32 len)
  {
    /* do block size increments */
    u8* local = (u8*)mBuffer;

    while (len) 
    {
      u32 add = cShaTools::min(len, SHA256_BLOCK_SIZE - mBuffLen);
      cMemTools::vMemCpy((u8*)&local[mBuffLen], (u8*)data, add);

      mBuffLen += add;
      data            += add;
      len             -= add;

      if (mBuffLen == SHA256_BLOCK_SIZE) 
      {
        #ifdef LITTLE_ENDIAN_ORDER
          ByteReverseBytes(local, local, SHA256_BLOCK_SIZE);
        #endif
        Transform();
        AddLength(SHA256_BLOCK_SIZE);
        mBuffLen = 0;
      }
    }
  }

  void Final(u8* hash)
  {
    u8* local = (u8*)mBuffer;

    AddLength(mBuffLen);  /* before adding pads */

    local[mBuffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (mBuffLen > SHA256_PAD_SIZE) 
    {
      cMemTools::vMemSet(&local[mBuffLen], 0, SHA256_BLOCK_SIZE - mBuffLen);
      mBuffLen += SHA256_BLOCK_SIZE - mBuffLen;

      #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseBytes(local, local, SHA256_BLOCK_SIZE);
      #endif
      Transform();
      mBuffLen = 0;
    }
    cMemTools::vMemSet(&local[mBuffLen], 0, SHA256_PAD_SIZE - mBuffLen);

    /* put lengths in bits */
    mHiLen = (mLoLen >> (8*sizeof(mLoLen) - 3)) + (mHiLen << 3);
    mLoLen = mLoLen << 3;

    /* store lengths */
    #ifdef LITTLE_ENDIAN_ORDER
      ByteReverseBytes(local, local, SHA256_BLOCK_SIZE);
    #endif

    /* ! length ordering dependent on mDigest endian type ! */
    cMemTools::vMemCpy((u8*)&local[SHA256_PAD_SIZE], (u8*)&mHiLen, sizeof(u32));
    cMemTools::vMemCpy((u8*)&local[SHA256_PAD_SIZE + sizeof(u32)], (u8*)&mLoLen, sizeof(u32));

    Transform();
    #ifdef LITTLE_ENDIAN_ORDER
      ByteReverseWords(mDigest, mDigest, SHA256_DIGEST_SIZE);
    #endif
    cMemTools::vMemCpy((u8*)hash, (u8*)mDigest, SHA256_DIGEST_SIZE);

    Init();  /* reset state */
  }
};


#endif /* _C_SAFETY_HW_H */
