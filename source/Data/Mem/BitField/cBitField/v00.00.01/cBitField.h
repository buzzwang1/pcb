#ifndef _BITFIELD_H
#define _BITFIELD_H

#include "typedef.h"

template <typename T>
class tcBitField
{
public:
  T mtData;
  tcBitField() : mtData(0) {}

  // Setzt ein spezifisches Bit (0-31)
  void vSet(u8 lu8Bit) { mtData |= (1UL << lu8Bit); }

  // Löscht ein spezifisches Bit (0-31)
  void vClear(u8 lu8Bit) { mtData &= ~(1UL << lu8Bit); }

  // Fragt ab, ob ein spezifisches Bit gesetzt ist
  bool isSet(u8 lu8Bit) const { return (mtData & (1UL << lu8Bit)) != 0; }

  // Löscht alle Bits (setzt auf 0)
  void vClearAll() { mtData = 0; }

  // Gibt den Index des ersten gesetzten Bits zurück (LSB zuerst)
  // Gibt -1 zurück, wenn kein Bit gesetzt ist.
  i8 i8GetFirst() const
  {
    // Nutzt Compiler-Builtin für Effizienz (GCC/Clang)
    // __builtin_ctz zählt die Nullen von rechts (Trailing Zeros)
    // Returns the number of trailing 0 - bits in x, starting at the least significant bit position.If x is 0, the result is undefined.
    if (mtData) return static_cast<i8>(__builtin_ctz(mtData));
    return -1;
  }

  // Operator für den Schreibzugriff auf den Rohwert (Zuweisung)
  tcBitField<T>& operator=(T ltData)
  {
    mtData = ltData;
    return *this;
  }

  // Operator für den Lesezugriff auf den Rohwert (Implizite Konvertierung)
  operator T() const { return mtData; }

  // Operator für den Lesezugriff auf den Rohwert (Implizite Konvertierung)
  operator bool() const { return (mtData != 0); }
};

template <u8 mu8Cnt>
class tcBitFieldArray
{
public:
  tcBitField<u32> mtcaBf[mu8Cnt];
  tcBitFieldArray()
  {
    vClearAll();
  }

  // Setzt ein spezifisches Bit (0-31)
  void vSet(u8 lu8Bit) 
  { 
    u8 lu8Byte = lu8Bit >> 5;
    mtcaBf[lu8Byte].vSet(lu8Bit - lu8Byte * 32);
  }

  // Löscht ein spezifisches Bit (0-31)
  void vClear(u8 lu8Bit) 
  {
    u8 lu8Byte = lu8Bit >> 5;
    mtcaBf[lu8Byte].vClear(lu8Bit - lu8Byte * 32);
  }

  // Fragt ab, ob ein irgendein Bit gesetzt ist
  bool isSet() const
  {
    for (u8 i = 0; i < mu8Cnt; i++)
    {
      if (mtcaBf[i].mtData) return True;
    }
    return False;
  }


  // Fragt ab, ob ein spezifisches Bit gesetzt ist
  bool isSet(u8 lu8Bit) const 
  { 
    u8 lu8Byte = lu8Bit >> 5;
    return mtcaBf[lu8Byte].isSet(lu8Bit - lu8Byte * 32);
  }

  // Löscht alle Bits (setzt auf 0)
  void vClearAll() 
  { 
    for (u8 i = 0; i < mu8Cnt; i++) mtcaBf[i].vClearAll();
  }

  // Gibt den Index des ersten gesetzten Bits zurück (LSB zuerst)
  // Gibt -1 zurück, wenn kein Bit gesetzt ist.
  i8 i8GetFirst() const
  {
    for (u8 i = 0; i < mu8Cnt; i++)
    {
      i8 li8Ret = mtcaBf[i].i8GetFirst();
      if (li8Ret >= 0) return li8Ret + i * 32;
    }
    return -1;
  }

  // Operator für den Schreibzugriff auf den Rohwert (Zuweisung)
  tcBitFieldArray<mu8Cnt>& operator=(tcBitFieldArray<mu8Cnt> ltData)
  {
    for (u8 i = 0; i < mu8Cnt; i++)
    {
      mtcaBf[i].mtData = ltData[i].mtData;
    }
    return *this;
  }

  // Operator für den Lesezugriff auf den Rohwert (Implizite Konvertierung)
  operator bool() const 
  {
    for (u8 i = 0; i < mu8Cnt; i++)
    {
      if (mtcaBf[i].mtData != 0) return True;
    }
    return False;
  }
};


class cBitField32: public tcBitField<u32>
{
  public:
  cBitField32() : tcBitField() {}
};

class cBitField16: public tcBitField<u16>
{
  public:
  cBitField16() : tcBitField() {}
};

class cBitField8: public tcBitField<u8>
{
  public:
  cBitField8() : tcBitField() {}
};

#endif /* _BITFIELD_H */
