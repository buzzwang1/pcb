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


public class cBotCom
{

  List<u8> llstDecode;
  u8       mMsgCounterTx;

  public cBotCom()
  {
    llstDecode = new List<byte>();
    mMsgCounterTx = 0;
  }

  /*
    DDDDD = Datenbits
    CCC   = Commandbits
    |||__
    ||__ |
    |__ ||
       |||
       000 :0: Übertragung Ende: Checksumme in den unteren 5 Bits
       001 :1: Daten Low  5-Bit = Die unteren 5 Bits
       010 :2: Daten High 3-Bit = Die oberen  3 Bits 
               - kommt nur nach einer 001 oder 111 kommen, falls der Wert >31 ist
       011 :3: NC
       100 :4: NC
       101 :5: NC
       110 :6: NC
       111 :7: Übertragung Start: Anzahl Daten (Low 5-Bit) in den unteren 5 Bits
  */
  public u8[] vEncode(u8[] lui8Data)
  {
    List<u8> llstOutput = new List<byte>();

    u8 lu8DataCount = (u8)lui8Data.Length;
    llstOutput.Add((u8)((7 << 5) + (lu8DataCount & 0x1F))); //Start
    llstOutput.Add((u8)((2 << 5) + ((mMsgCounterTx & 3) << 3) + (lu8DataCount >> 5)));
    
    mMsgCounterTx++;

    u8 lu8CheckSum = 1;
    for (int i = 0; i < lu8DataCount; i++)
    {
      lu8CheckSum += lui8Data[i];
      llstOutput.Add((u8)((1 << 5) + (lui8Data[i] & 0x1F)));

      if (lui8Data[i] > 31) //GGf obere 3 Bit
      {
        llstOutput.Add((u8)((2 << 5) + (lui8Data[i] >> 5)));
      }
    }
    llstOutput.Add((u8)((0 << 5) + (lu8CheckSum & 0x1F)));

    return llstOutput.ToArray();
  }


  u8 mu8Checksum;
  //u8 mu8DataByte;
  u8 mu8DataCount;
  u8 mu8LastCmd;

  public u8[] vDecode(u8 lui8DataIn)
  {
    u8[] lu8Ret = null;
    u8 lu8Data;
    u8 lu8Cmd;
    lu8Data = (u8)(lui8DataIn & 0x1F);
    lu8Cmd  = (u8)(lui8DataIn >> 5);
    switch (lu8Cmd)
    {
      case 0: // 000 : Übertragung Ende: Checksumme in den unteren 5 Bits
        if (llstDecode.Count == mu8DataCount)
        {
          if (llstDecode.Count > 0)
          {
            lu8Ret = llstDecode.ToArray();
          }
        }
        llstDecode.Clear();
        break;
      case 1: // 001 : Daten Start: Start Daten. Die unteren 5 Bits
        mu8Checksum += lu8Data;
        llstDecode.Add(lu8Data);
        break;
      case 2: // 010 : Daten Zusatz: Die oberen  3 Bits 
        switch (mu8LastCmd)
        {
          case 1: // Daten
            llstDecode[llstDecode.Count - 1] += (u8)(lu8Data << 5);
            mu8Checksum += (u8)(lu8Data << 5);
            break;
          case 7: // Anzahl Daten
            mu8DataCount += (u8)(lu8Data << 5);
            break;
          default:
            break;
        }
        break;
      case 3: // 011 : NC
        break;
      case 4: // 100 : NC
        break;
      case 5: // 101 : NC
        break;
      case 6: // 110 : NC
        break;
      case 7: // 111 : Übertragung Start: Anzahl Daten (Low 5-Bit) in den unteren 5 Bits
        mu8Checksum = 1;
        mu8DataCount = lu8Data;
        llstDecode.Clear();
        break;
    }

    mu8LastCmd = lu8Cmd;

    return lu8Ret;
  }
};


