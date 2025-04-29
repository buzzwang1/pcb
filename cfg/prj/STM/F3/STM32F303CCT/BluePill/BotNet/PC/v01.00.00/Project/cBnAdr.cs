using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using uint64 = System.UInt64;
using uint32 = System.UInt32;
using uint16 = System.UInt16;
using uint8 = System.Byte;
using u64 = System.UInt64;
using u32 = System.UInt32;
using u16 = System.UInt16;
using u8 = System.Byte;

using int64 = System.Int64;
using int32 = System.Int32;
using int16 = System.Int16;
using int8 = System.SByte;
using i64 = System.Int64;
using i32 = System.Int32;
using i16 = System.Int16;
using i8 = System.SByte;

public class cBotNetAdress
{
  /* 16 Bit v4 Adresse, wobei

      Adr1:Adr2:Adr3:Adr4

      0x1234: Adr1 = 1: Adr2 = 2: Adr3 = 3: Adr4 = 4

      0 = Master, z.B. 0x1230 => Master 1.2.3.0 Slaves 1.2.3.1..1.2.3.14
  */

  public uint8 AdrD1;
  public uint8 AdrD2;
  public uint8 AdrD3;
  public uint8 AdrD4;

  uint16 u16Adr;

  public void Set(uint16 lu16Adr)
  {
    u16Adr = lu16Adr;

    AdrD1 = (u8)((lu16Adr >> 12) & 0xF);
    AdrD2 = (u8)((lu16Adr >> 8) & 0xF);
    AdrD3 = (u8)((lu16Adr >> 4) & 0xF);
    AdrD4 = (u8)((lu16Adr >> 0) & 0xF);
  }

  public uint16 Get()
  {
    return u16Adr;
  }

  public void Decode(uint16 lui16Adr)
  {
    //Swap High-Byte an Low-Byte
    Set((u16)((lui16Adr << 8) + (lui16Adr >> 8)));
  }

  public uint16 Encode()
  {
    return ((u16)((u16Adr << 8) + (u16Adr >> 8)));
  }

  // 1.2.3.4 => 4
  // 1.2.3.0 => 3
  // 1.2.0.0 => 2
  // 1.0.0.0 => 1
  public uint8 GetIdx()
  {
    uint16 lu16Adr;

    lu16Adr = Get();

    if ((lu16Adr & 0x000F) > 0) return (u8)((lu16Adr & 0x000F) >> 0);
    if ((lu16Adr & 0x00F0) > 0) return (u8)((lu16Adr & 0x00F0) >> 4);
    if ((lu16Adr & 0x0F00) > 0) return (u8)((lu16Adr & 0x0F00) >> 8);
    if ((lu16Adr & 0xF000) > 0) return (u8)((lu16Adr & 0xF000) >> 12);

    return 0;
  }

  // 1.2.3.0 => 1.2.3.x
  // 1.2.0.0 => 1.2.x.0
  // 1.0.0.0 => 1.x.0.0
  public uint16 GetSlaveAdr(uint8 lui8SlaveIdx)
  {
    uint16 lu16Adr;

    lu16Adr = Get();

    //if ((lu16Adr & 0x000F) > 0)return ((lu16Adr & 0x000F) >> 0);
    if ((lu16Adr & 0x00F0) > 0) return (u16)(lu16Adr | (lui8SlaveIdx << 0));
    if ((lu16Adr & 0x0F00) > 0) return (u16)(lu16Adr | (lui8SlaveIdx << 4));
    if ((lu16Adr & 0xF000) > 0) return (u16)(lu16Adr | (lui8SlaveIdx << 8));

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
  public u16 u16GetMask(uint16 lu16Adr)
  {
    u16 lu16Mask;
    //Swap High-Byte an Low-Byte
    u16Adr = lu16Adr;

    lu16Mask = 0;

    if ((lu16Adr & 0x000F) > 0) lu16Mask = 0xFFFF;
    else
      if ((lu16Adr & 0x00F0) > 0) lu16Mask = 0xFFF0;
      else
        if ((lu16Adr & 0x0F00) > 0) lu16Mask = 0xFF00;
        else
          if ((lu16Adr & 0xF000) > 0) lu16Mask = 0xF000;

    return lu16Mask;
  }

  // 'Lowest' slaves cannot slaves
  public bool isHasNoSlaves()
  {
    return ((u16Adr & 0xFFF0) != 0);
  }

  public bool isMe(uint16 lui16Adr)
  {
    return (lui16Adr == u16Adr);
  }

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
  public bool isDirectMasterOrSlave(uint16 lu16Adr)
  {
    u16 lu16Mask = u16GetMask(u16Adr);

    if ((u16Adr & 0x000F) > 0)
    {
      if (((lu16Mask << 4) & u16Adr) == lu16Adr) return true;
    }
    else
      if ((u16Adr & 0x00F0) > 0)
      {
        if (((lu16Mask << 4) & u16Adr) == lu16Adr) return true;
        if ((0xFFF0 & lu16Adr) == u16Adr) return true;
      }
      else
        if ((u16Adr & 0x0F00) > 0)
        {
          if (((lu16Mask << 4) & u16Adr) == lu16Adr) return true;
          if ((0xFF0F & lu16Adr) == u16Adr) return true;
        }
        else
          if ((u16Adr & 0xF000) > 0)
          {
            if ((0xF0FF & lu16Adr) == u16Adr) return true;
          }

    return false;
  }

  // mAdr    | Mask    | Slave   | 
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | False
  // 1.2.3.0 | F.F.F.0 | 1.2.3.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | False
  // 1.2.0.0 | F.F.0.0 | 1.2.1.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | False
  // 1.2.0.0 | F.F.0.0 | 1.2.1.1 | True
  // 1.2.0.0 | F.F.0.0 | 1.3.1.1 | False
  public bool isMasterOf(uint16 lui16SlaveAdr)
  {
    u16 lu16Mask = u16GetMask(u16Adr);
    return ((lu16Mask & lui16SlaveAdr) == u16Adr);
  }

  // mAdr    | Mask    | Master  | 
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | False
  // 1.2.3.1 | F.F.F.F | 1.2.3.0 | True
  // 1.1.1.1 | F.F.F.F | 1.2.0.0 | False
  // 1.2.1.1 | F.F.F.F | 1.2.0.0 | True
  // 1.1.1.1 | F.F.F.F | 1.2.0.0 | False
  // 1.2.1.0 | F.F.F.0 | 1.2.0.0 | True
  // 1.3.1.1 | F.F.F.F | 1.2.0.0 | False
  public bool isSlaveOf(uint16 lui16MasterAdr)
  {
    u16 lu16Mask = u16GetMask(u16Adr);
    return (((lu16Mask << 4) & u16Adr) == lui16MasterAdr);
  }

  // mAdr    | Mask    | Slave   | 
  // 1.2.3.0 | F.F.F.0 | 1.2.3.0 | n.a.
  // 1.2.3.0 | F.F.F.0 | 1.2.3.1 | 1
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | n.a.
  // 1.2.0.0 | F.F.0.0 | 1.2.3.1 | 3
  // 1.2.0.0 | F.F.0.0 | 1.1.1.1 | n.a.
  // 1.2.0.0 | F.F.0.0 | 1.2.5.1 | 5
  // 1.2.0.0 | F.F.0.0 | 1.3.1.1 | n.a.
  public uint16 u16GetNextLevelSlaveIdx(uint16 lui16SlaveAdr)
  {
    lui16SlaveAdr &= u16GetMask(u16Adr);
    while (true)
    {
      if ((lui16SlaveAdr & 0xF0)  > 0)
        lui16SlaveAdr >>= 4;
      else
        return lui16SlaveAdr;
    }
  }

  public override String ToString()
  {
    return   AdrD1.ToString("x") + "." +
             AdrD2.ToString("x") + "." +
             AdrD3.ToString("x") + "." +
             AdrD4.ToString("x");
  }
};

