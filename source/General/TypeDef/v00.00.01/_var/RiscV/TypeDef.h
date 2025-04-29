/****************************************************************************************/
/*! \file Typedef.h
 *  \brief Einige cdefs für PC, ggf. für entsprechendes System anpassen
 *  \author Florian Kaes
 *  \version 2007_04_08
 *  \date    08.04.2007
 *  \section history History
 *
 *  \since 08.04.2007
 *  - Erstellt
 */
/****************************************************************************************/

#ifndef _cdef_H
#define _cdef_H

#include <inttypes.h>
#include <stddef.h>

#include "Sections.h"
#include "Irq.h"

#define INLINE //inline
//#define NULL   null
#define nDefCoreType8   0
#define nDefCoreType16  1
#define nDefCoreType32  2
#define nDefCoreType64  3

#define nDefCoreType    nDefCoreType32


#define nDefMemLayoutTiny    0 // <4k
#define nDefMemLayoutSmall   1 // <64k
#define nDefMemLayoutMedium  2 // <512k
#define nDefMemLayoutLarge   3 // >=512k

#define nDefMemLayout   nDefMemLayoutSmall


#define BitPos(Pos) (1<<Pos)


typedef uint64_t       uint64;
typedef uint32_t       uint32;
typedef uint16_t       uint16;
typedef uint8_t        uint8;
typedef uint64_t       u64;
typedef uint32_t       u32;
typedef uint16_t       u16;
typedef uint8_t        u8;

typedef int64_t        int64;
typedef int32_t        int32;
typedef int16_t        int16;
typedef signed char    int8;
typedef int64_t        i64;
typedef int32_t        i32;
typedef int16_t        i16;
typedef signed char    i8;

//typedef signed char    bool;

//Function Return Values
typedef int8    FRet;
#define FRET_OK                0
#define FRET_ERR_NO            FRET_OK
#define FRET_ERR_GENERAL       1
#define FRET_ERR_PARAMETER     2


#if (nDefCoreType == nDefCoreType8)
  typedef uint16 tsize;
#endif
#if (nDefCoreType == nDefCoreType16)
  typedef uint16 tsize;
#endif
#if (nDefCoreType == nDefCoreType32)
  typedef uint32 tsize;
#endif
#if (nDefCoreType == nDefCoreType64)
  typedef uint64 tsize;
#endif

#define UINT32MAX 0xFFFFFFFF
#define UINT16MAX 0xFFFF
#define UINT8MAX  0xFF
#define INT32MAX 2147483647
#define INT16MAX 32767
#define INT8MAX  127
#define INT32MIN ((int32)(-2147483648))
#define INT16MIN ((int16)(-32768))
#define INT8MIN  ((int8)(-128))


// Punkt
typedef struct { float  x, y; }  stPxlf;
typedef struct { int16  x, y; }  stPxli16; // Pixel
typedef struct { int16  x, y; }  stPnti16; // Pnt


// Bitfeld für 8 Bit
typedef struct
{
  uint8 B0:1;
  uint8 B1:1;
  uint8 B2:1;
  uint8 B3:1;
  uint8 B4:1;
  uint8 B5:1;
  uint8 B6:1;
  uint8 B7:1;
}Bit8_struct;

// Union für Bitweiser Zugriff auf ein Byte
typedef union
{
  uint8        ui8Byte;
  Bit8_struct  stBits;
}tunBit8;


// Union für Byteweiser Zugriff auf uint32
typedef union
{
  uint32       ui32Data;
  uint8        ui8Byte[4];
}tun4Byte;


#define char8 char
typedef const char8* rsz; // Rom String zero ending 

#ifndef UNUSED
  #define UNUSED(x) (void)(x)
#endif
#define false ((bool)0)
#define true  (!false)
#define null 0
#ifndef NULL
  #define NULL null
#endif

#define True    (true)    /* Boolean value for True       */
#define False   (false)    /* Boolean value for False      */
#define sbi(port, bit) //(port) |= _BV(bit)
#define cbi(port, bit) //(port) &= ~(_BV(bit));

// https://stackoverflow.com/questions/72124439/reading-risc-v-csr-registers-using-c-asm
#define CSRR_READ(v, csr)                           \
/* CSRR_READ(v, csr):
 * csr: MUST be a compile time integer 12-bit constant (0-4095)
 */                                             \
     asm volatile("csrr %0, %1"             \
     : "=r" (v)                        \
     : "n" (csr)                       \
     : /* clobbers: none */);

//AVR portable
#define PROGMEM


#define MemWriteu8( u8ptr, u8Val)  ((u8*)u8ptr)[0] = (u8)((u8Val));
#define MemWriteu16(u8ptr, u16Val) ((u8*)u8ptr)[0] = ((u16Val)>>0); ((u8*)u8ptr)[1] = ((u16Val)>>8);

#define MemReadu8( u8ptr) (u8)((u8*)(u8ptr))[0]
#define MemReadu16(u8ptr) ((u16)(((u8*)(u8ptr))[0] << 0) + (u16)(((u8*)(u8ptr))[1] << 8))
#define MemReadu32(u8ptr) ((u32)(((u8*)(u8ptr))[0] << 0) + (u32)(((u8*)(u8ptr))[1] << 8) + (u32)(((u8*)(u8ptr))[2] << 16) + (u32)(((u8*)(u8ptr))[3] << 24))


#endif // cdef
