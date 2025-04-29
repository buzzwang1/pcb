#ifndef __BOTNET_ADR_H__
#define __BOTNET_ADR_H__

#include "Typedef.h"
#include "cChr8.h"

#ifdef __GNUC__
class __attribute__((__packed__)) cBotNetAdress
#else
class cBotNetAdress
#endif
{
public:

  /* 16 Bit v4 Adresse, wobei

      Adr1:Adr2:Adr3:Adr4

      0x1234: Adr1 = 1: Adr2 = 2: Adr3 = 3: Adr4 = 4

      0 = Master, z.B. 0x1230 => Master 1.2.3.0 Slaves 1.2.3.1..1.2.3.14
  */

  #ifdef __GNUC__
    typedef struct
    {
      uint8  AdrD4:4;
      uint8  AdrD3:4;
      uint8  AdrD2:4;
      uint8  AdrD1:4;
    }tstBotNetAdress;
  #else
    typedef struct
    {
      uint8  AdrD1:4;
      uint8  AdrD2:4;
      uint8  AdrD3:4;
      uint8  AdrD4:4;
    }tstBotNetAdress;
  #endif

  typedef union
  {
    uint16 u16Adr;
    tstBotNetAdress stAdr;

  }tunBotNetAdress;

  tunBotNetAdress mAdr;

  cBotNetAdress()
  {
    Set(0);
  }

  cBotNetAdress(uint16 lu16Adr)
  {
    Set(lu16Adr);
  }

  cBotNetAdress(u8 lu16Adr4, u8 lu16Adr3, u8 lu16Adr2, u8 lu16Adr1)
  {
    Set(lu16Adr1, lu16Adr2, lu16Adr3, lu16Adr4);
  }

  inline void Set(u8 lu16Adr4, u8 lu16Adr3, u8 lu16Adr2, u8 lu16Adr1)
  {
    mAdr.u16Adr = lu16Adr1 + (lu16Adr2 << 4) + (lu16Adr3 << 8) + (lu16Adr4 << 12);
  }

  inline void Set(uint16 lu16Adr)
  {
    mAdr.u16Adr = lu16Adr;
  }


  inline void Set(cBotNetAdress lcAdr)
  {
    Set(lcAdr.mAdr.u16Adr);
  }

  static u16 u16GetAdr(u8 lu16Adr4, u8 lu16Adr3, u8 lu16Adr2, u8 lu16Adr1)
  {
    return (lu16Adr1 + (lu16Adr2 << 4) + (lu16Adr3 << 8) + (lu16Adr4 << 12));
  }

  inline uint16 Get()
  {
    return mAdr.u16Adr;
  };

  inline void Decode(uint16 lui16Adr)
  {
    //Swap High-Byte an Low-Byte
    Set((lui16Adr << 8) + (lui16Adr >> 8));
  };

  inline uint16 Encode()
  {
    return ((mAdr.u16Adr << 8) + (mAdr.u16Adr >> 8));
  };


  // 1.2.3.4 => 4
  // 1.2.3.0 => 3
  // 1.2.0.0 => 2
  // 1.0.0.0 => 1
  uint8 GetIdx(u16 lu16Adr)
  {

    if (lu16Adr & 0x000F) return (lu16Adr & 0x000F) >> 0;
    if (lu16Adr & 0x00F0) return (lu16Adr & 0x00F0) >> 4;
    if (lu16Adr & 0x0F00) return (lu16Adr & 0x0F00) >> 8;
    if (lu16Adr & 0xF000) return (lu16Adr & 0xF000) >> 12;
    return 0;
  }

  // 1.2.3.4 => 4
  // 1.2.3.0 => 3
  // 1.2.0.0 => 2
  // 1.0.0.0 => 1
  uint8 GetIdx()
  {
    uint16 lu16Adr;

    lu16Adr = Get();

    if (lu16Adr & 0x000F) return (lu16Adr & 0x000F) >> 0;
    if (lu16Adr & 0x00F0) return (lu16Adr & 0x00F0) >> 4;
    if (lu16Adr & 0x0F00) return (lu16Adr & 0x0F00) >> 8;
    if (lu16Adr & 0xF000) return (lu16Adr & 0xF000) >> 12;
    return 0;
  }


  // 1.2.3.4 => 1.2.3.0
  // 1.2.3.0 => 1.2.0.0
  // 1.2.0.0 => 1.0.0.0
  // 1.0.0.0 => 0.0.0.0
  static uint16 GetMasterAdr(u16 lu16Adr)
  {
    if (lu16Adr & 0x000F) return (lu16Adr & 0xFFF0);
    if (lu16Adr & 0x00F0) return (lu16Adr & 0xFF00);
    if (lu16Adr & 0x0F00) return (lu16Adr & 0xF000);
    return 0x0000;
  }


  // 1.2.3.4 => 1.2.3.0
  // 1.2.3.0 => 1.2.0.0
  // 1.2.0.0 => 1.0.0.0
  // 1.0.0.0 => 0.0.0.0
  uint16 GetMasterAdr()
  {
    uint16 lu16Adr;

    lu16Adr = Get();

    if (lu16Adr & 0x000F) return (lu16Adr & 0xFFF0);
    if (lu16Adr & 0x00F0) return (lu16Adr & 0xFF00);
    if (lu16Adr & 0x0F00) return (lu16Adr & 0xF000);

    return 0x0000;
  }

  // 1.2.3.0 => 1.2.3.x
  // 1.2.0.0 => 1.2.x.0
  // 1.0.0.0 => 1.x.0.0
  static uint16 GetSlaveAdr(u16 lu16Adr, uint8 lui8SlaveIdx)
  {
    //if (lu16Adr & 0x000F) return (lu16Adr & 0x000F) >> 0;
    if (lu16Adr & 0x00F0) return lu16Adr | (lui8SlaveIdx << 0);
    if (lu16Adr & 0x0F00) return lu16Adr | (lui8SlaveIdx << 4);
    if (lu16Adr & 0xF000) return lu16Adr | (lui8SlaveIdx << 8);
    return 0;
  }

  // 1.2.3.0 => 1.2.3.x
  // 1.2.0.0 => 1.2.x.0
  // 1.0.0.0 => 1.x.0.0
  uint16 GetSlaveAdr(uint8 lui8SlaveIdx)
  {
    uint16 lu16Adr;

    lu16Adr = Get();

    //if (lu16Adr & 0x000F) return (lu16Adr & 0x000F) >> 0;
    if (lu16Adr & 0x00F0) return lu16Adr | (lui8SlaveIdx << 0);
    if (lu16Adr & 0x0F00) return lu16Adr | (lui8SlaveIdx << 4);
    if (lu16Adr & 0xF000) return lu16Adr | (lui8SlaveIdx << 8);

    return 0;
  }


  // Master  | Mask
  // 1.2.3.1 | F.F.F.F
  // 1.2.3.0 | F.F.F.0
  // 1.2.0.0 | F.F.0.0
  // 1.2.0.0 | F.F.0.0
  // 1.2.0.0 | F.F.0.0
  // 1.2.0.0 | F.F.0.0
  // 1.2.0.0 | F.F.0.0
  inline u16 u16GetMask(uint16 lu16Adr)
  {
    u16 lu16Mask;
    //Swap High-Byte an Low-Byte
    mAdr.u16Adr = lu16Adr;

    lu16Mask = 0;

    if (lu16Adr & 0x000F) lu16Mask = 0xFFFF;
    else
      if (lu16Adr & 0x00F0) lu16Mask = 0xFFF0;
      else
        if (lu16Adr & 0x0F00) lu16Mask = 0xFF00;
        else
          if (lu16Adr & 0xF000) lu16Mask = 0xF000;

    return lu16Mask;
  };

  // 'Lowest' slaves cannot have slaves
  bool isHasNoSlaves()
  {
    return ((mAdr.u16Adr & 0xFFF0) != 0);
  }

  inline bool isMe(uint16 lui16Adr)
  {
    return (mAdr.u16Adr == lui16Adr);
  }

  inline bool isMe(cBotNetAdress lcAdr)
  {
    return isMe(lcAdr.mAdr.u16Adr);
  }

  bool operator== (const uint16 lui16Adr)
  {
    return isMe(lui16Adr);
  };

  bool operator== (const cBotNetAdress lcAdr)
  {
    return isMe(lcAdr);
  };


  // mAdr     | Adr     |
  // 1.2.3.4  | 1.2.3.0 | True
  // 1.2.3.4  | 1.2.3.5 | False
  // 1.2.3.4  | 1.2.0.0 | False
  // 1.2.3.0  | 1.2.3.1 | True
  // 1.2.3.0  | 1.2.4.1 | False
  // 1.2.0.0  | 1.2.1.1 | False
  // 1.2.0.0  | 1.2.1.0 | True
  // 1.2.0.0  | 1.1.0.0 | False
  // 1.2.0.0  | 1.2.0.0 | False
  // 1.2.0.0  | 1.0.0.0 | True
  inline bool isDirectMasterOrSlave(uint16 lu16Adr)
  {
    u16 lu16Mask = u16GetMask(mAdr.u16Adr);

    if (mAdr.u16Adr & 0x000F)
    {
      if (((lu16Mask << 4) & mAdr.u16Adr) == lu16Adr) return True;
    }
    else
      if (mAdr.u16Adr & 0x00F0)
      {
        if (((lu16Mask << 4) & mAdr.u16Adr) == lu16Adr) return True;
        if ((0xFFF0 & lu16Adr) == mAdr.u16Adr) return True;
      }
      else
        if (mAdr.u16Adr & 0x0F00)
        {
          if (((lu16Mask << 4) & mAdr.u16Adr) == lu16Adr) return True;
          if ((0xFF0F & lu16Adr) == mAdr.u16Adr) return True;
        }
        else
          if (mAdr.u16Adr & 0xF000)
          {
            if ((0xF0FF & lu16Adr) == mAdr.u16Adr) return True;
          }

    return False;
  }

  inline bool isDirectMasterOrSlave(cBotNetAdress lcAdr)
  {
    return isDirectMasterOrSlave(lcAdr.mAdr.u16Adr);
  }

  // mAdr     | Adr     |
  // 1.2.3.4  | 1.2.3.0 | False
  // 1.2.3.4  | 1.2.3.5 | False
  // 1.2.3.4  | 1.2.0.0 | False
  // 1.2.3.0  | 1.2.3.1 | True
  // 1.2.3.0  | 1.2.4.1 | False
  // 1.2.0.0  | 1.2.1.1 | False
  // 1.2.0.0  | 1.2.1.0 | True
  // 1.2.0.0  | 1.1.0.0 | False
  // 1.2.0.0  | 1.2.0.0 | False
  // 1.2.0.0  | 1.0.0.0 | False
  inline bool isDirectMasterOf(uint16 lu16Adr)
  {
    if (mAdr.u16Adr & 0x000F) // Kann keine Slave haben
    {
      return False;
    }
    else
      if (mAdr.u16Adr & 0x00F0)
      {
        if ((0xFFF0 & lu16Adr) == mAdr.u16Adr) return True;
      }
      else
        if (mAdr.u16Adr & 0x0F00)
        {
          if ((0xFF0F & lu16Adr) == mAdr.u16Adr) return True;
        }
        else
          if (mAdr.u16Adr & 0xF000)
          {
            if ((0xF0FF & lu16Adr) == mAdr.u16Adr) return True;
          }

    return False;
  }

  inline bool isDirectMasterOf(cBotNetAdress lcAdr)
  {
    return isDirectMasterOf(lcAdr.mAdr.u16Adr);
  }

  // mAdr    | Mask    | Slave   |
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | False
  // 1.2.3.0 | F.F.F.0 | 1.2.3.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | False
  // 1.2.0.0 | F.F.0.0 | 1.2.1.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | False
  // 1.2.0.0 | F.F.0.0 | 1.2.1.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.3.1.1 | False
  inline bool isMasterOf(uint16 lui16SlaveAdr)
  {
    u16 lu16Mask = u16GetMask(mAdr.u16Adr);
    return ((lu16Mask & lui16SlaveAdr) == mAdr.u16Adr);
  }

  inline bool isMasterOf(cBotNetAdress lcAdr)
  {
    return isMasterOf(lcAdr.mAdr.u16Adr);
  }

  // mAdr    | Mask    | Master  |
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | False
  // 1.2.3.1 | F.F.F.F | 1.2.3.0 | True
  // 1.1.1.1 | F.F.F.F | 1.2.0.0 | False
  // 1.2.1.1 | F.F.F.F | 1.2.0.0 | True
  // 1.1.1.1 | F.F.F.F | 1.2.0.0 | False
  // 1.2.1.0 | F.F.F.0 | 1.2.0.0 | True
  // 1.3.1.1 | F.F.F.F | 1.2.0.0 | False
  inline bool isSlaveOf(uint16 lui16MasterAdr)
  {
    u16 lu16Mask = u16GetMask(mAdr.u16Adr);
    return (((lu16Mask << 4) & mAdr.u16Adr) == lui16MasterAdr);
  }

  inline bool isSlaveOf(cBotNetAdress lcAdr)
  {
    return isSlaveOf(lcAdr.mAdr.u16Adr);
  }


  // mAdr    | Mask    | Slave   |
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | n.a.
  // 1.2.3.0 | F.F.F.0 | 1.2.3.1 | 1
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | n.a.
  // 1.2.0.0 | F.F.0.0 | 1.2.3.1 | 3
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | n.a.
  // 1.2.0.0 | F.F.0.0 | 1.2.5.1 | 5
  // 1.2.0.0 | F.F.0.0 | 1.3.1.1 | n.a.
  inline u8 u8GetNextLevelSlaveIdx(uint16 lui16SlaveAdr)
  {
    u16 lu16Mask = u16GetMask(mAdr.u16Adr);

    if ((lui16SlaveAdr & lu16Mask) == mAdr.u16Adr)
    {
      lui16SlaveAdr &= (~lu16Mask);
      lui16SlaveAdr &= 0x0FFF;

      if (lui16SlaveAdr >> 8) return lui16SlaveAdr >> 8;
      if (lui16SlaveAdr >> 4) return lui16SlaveAdr >> 4;
      if (lui16SlaveAdr)      return lui16SlaveAdr;
    }
    return 0;
  }

  char* ToString()
  {
    static char8  lszStr[8] = "0.0.0.0";
    lszStr[0] = cChr8Tools::ToHexDigit(mAdr.stAdr.AdrD1);
    lszStr[2] = cChr8Tools::ToHexDigit(mAdr.stAdr.AdrD2);
    lszStr[4] = cChr8Tools::ToHexDigit(mAdr.stAdr.AdrD3);
    lszStr[6] = cChr8Tools::ToHexDigit(mAdr.stAdr.AdrD4);
    return lszStr;
  }


  inline bool isSet() {return (mAdr.u16Adr != 0);}

};


#endif // __BOTNET_ADR_H__
