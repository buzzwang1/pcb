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

public class cBotNetMsgFrame
{
  public enum tenBotNetMsgType
  {
    enMessage = 0,
    enStream,
    enPacket,
    enMultiMessage
  }

  public bool bAdressIntOn;
  public bool bAdressExtOn;

  public tenBotNetMsgType enMsgType;

  public cBotNetAdress cDAdr;
  public cBotNetAdress cSAdr;


  public cBotNetMsgFrame()
  {
    cDAdr = new cBotNetAdress();
    cSAdr = new cBotNetAdress();
    vSetDefault();
  }

  public void vSetDefault()
  {
    vSet(0, 0);
    enMsgType = tenBotNetMsgType.enMessage;
  }

  public void vSet(uint16 lui16SourceAdr, uint16 lui16DestAdr)
  {
    if ((lui16SourceAdr != 0) || (lui16DestAdr != 0))
    {
      bAdressIntOn = true;
    }
    else
    {
      bAdressIntOn = false;
    }

    cSAdr.Set(lui16SourceAdr);
    cDAdr.Set(lui16DestAdr);
  }

  public u8 u8Len()
  {
    u8 lu8Ret;
    lu8Ret = 1;
    if (bAdressIntOn) lu8Ret += 4; // 16Bit Source und DestAdresse
    if (bAdressExtOn) lu8Ret += 4; // 1Byte Alterungszähler
    return (lu8Ret);
  }
};

public class cBotNetMsg_Base
{
  public cBotNetMsgFrame mFrame;
  public List<byte>  mMsgData;

  public cBotNetMsg_Base()
  {
    mMsgData = new List<byte>(cBnCfg.cBotNet_MsgSize);
    mFrame = new cBotNetMsgFrame();
  }

  public cBotNetMsg_Base(u8[] lByteArray)
  {
    mFrame = new cBotNetMsgFrame();

    FromByteArray(lByteArray);
  }

  public void FromByteArray(u8[] lByteArray)
  {
    mMsgData = new List<Byte>(lByteArray);

    vDecode();
  }

  public void vSetFrame(uint16 lui16SourceAdr, uint16 lui16DestAdr)
  {
    mFrame.vSet(lui16SourceAdr, lui16DestAdr);
  }

  public u8[] ToByteArray()
  {
    return mMsgData.ToArray();
  }

  public String toString()
  {
    String lszRet = ToByteArray().ToString();

    return lszRet;
  }


  public cBotNetMsg_Base(List<byte> lMsgData)
  {
    mMsgData = new List<byte>(lMsgData);
  }

  public cBotNetAdress cGetDAdr()
  {
    return mFrame.cDAdr;
  }

  public cBotNetAdress cGetSAdr()
  {
    return mFrame.cSAdr;
  }

  public bool bHasAdress()
  {
    return mFrame.bAdressIntOn;
  }

  public u8 u8GetBnFrameSize()
  {
    return (u8)(mFrame.u8Len());
  }

  public u8 u8Len()
  {
    return (u8)(mMsgData.Count);
  }

  public u8 iGetIdxPayload()
  {
    return 0;
  }

  public u8 iGetLenPayload()
  {
    return (u8)(u8Len() - mFrame.u8Len());
  }


  public void vDecode()
  {
    u8 lu8FrameIdx = (u8)(u8Len()-1);

    mFrame.vSetDefault();

    //  -4 -3   -2 -1   0
    // [DA DA] [SA SA] CB

    //Interne Adresse ?
    if ((mMsgData[lu8FrameIdx] & 128) == 128) 
    {
      mFrame.bAdressIntOn = true;

      mFrame.vSet((u16)(mMsgData[lu8FrameIdx-2] << 8 + mMsgData[lu8FrameIdx-1]), // Source
                  (u16)(mMsgData[lu8FrameIdx-4] << 8 + mMsgData[lu8FrameIdx-3])); // Destination
    }
    mFrame.enMsgType = (cBotNetMsgFrame.tenBotNetMsgType)(mMsgData[lu8FrameIdx] & 0x0F);
  }

  public void vEncode(List<byte> lMsgPayload)
  {
    mMsgData.Clear();

    //Payload
    for (int t = 0; t < lMsgPayload.Count; t++)
    {
      mMsgData.Add(lMsgPayload[t]);
    }

    //Frame Daten
    u8 lu8ControlByte = 0;
    lu8ControlByte = (u8)((u8)(mFrame.enMsgType) & 0x0F);
    if (mFrame.bAdressIntOn)
    {
      lu8ControlByte |= 128;

      mMsgData.Add((u8)(mFrame.cDAdr.Get() >> 8));
      mMsgData.Add((u8)(mFrame.cDAdr.Get() & 0xFF));

      mMsgData.Add((u8)(mFrame.cSAdr.Get() >> 8));
      mMsgData.Add((u8)(mFrame.cSAdr.Get() & 0xFF));
    }

    mMsgData.Add(lu8ControlByte);
  }

  public void vEncode(u8[] lau8Payload)
  {
    List<byte> lMsgPayload = new List<byte>(lau8Payload);
    vEncode(lMsgPayload);
  }
};



public class cBotNetMsg_MsgProt : cBotNetMsg_Base
{
  // [Botnet Header] MI1[MI2] MD[0..DA]
  // MI: Message Index 8Bit or 16Bit
  // MD: Message Data / Payload

  public List<byte>   mcPayload;
  public uint16       mu16Idx;

  public cBotNetMsg_MsgProt() 
  {
    mcPayload = new List<byte>(cBnCfg.cBotNet_MsgSize);
  }

  public cBotNetMsg_MsgProt(cBotNetMsg_Base lMsgBase)
  {
    mcPayload = new List<byte>(cBnCfg.cBotNet_MsgSize);
    base.mFrame = lMsgBase.mFrame;
    base.mMsgData = lMsgBase.mMsgData;
    vDecode();
  }

  public new void vDecode()
  {
    base.vDecode();

    //Offset-Überspringen vom Botnet-Header
    u8 u8MsgIdx = (u8)base.iGetIdxPayload();
    u8 lu8Data  = mMsgData[u8MsgIdx++];

    u8 lu8DataToCopy;
    lu8DataToCopy = (u8)(base.u8Len() - base.u8GetBnFrameSize());

    //8Bit oder 16Bit Index ?
    if ((lu8Data & 128) > 0)
    {
      //lang Idx 16Bit
      mu16Idx = (u16)((lu8Data & 0x7F) << 8);
      lu8Data  = mMsgData[u8MsgIdx++];
      mu16Idx += lu8Data;

      lu8DataToCopy -= 2;
    }
    else
    {
      //kurz Idx 8Bit
      mu16Idx = lu8Data;

      lu8DataToCopy -= 1;
    }

    for (int t = 0; t < lu8DataToCopy; t++)
    {
      if (t < mcPayload.Count)
      {
        mcPayload[t] = mMsgData[u8MsgIdx++];
      }
    }
  }

  public void vPrepare(uint16 lui16SourceAdr, uint16 lui16DestAdr, uint16 lui16MsgIdx)
  {
    base.mFrame.enMsgType =  cBotNetMsgFrame.tenBotNetMsgType.enMessage;
    base.vSetFrame(lui16SourceAdr, lui16DestAdr);

    mu16Idx = lui16MsgIdx;
    mcPayload.Clear();
  }

  public void vEncode()
  {
    List<byte>   lcData;
    lcData = new List<byte>(cBnCfg.cBotNet_MsgSize);

    // Gesamtlänge bestimmen
    //8Bit oder 16Bit Index ?
    if (mu16Idx > 127)
    {
      //lang Idx 16Bit
      lcData.Add((u8)((mu16Idx >> 8) | 128));
      lcData.Add((u8)(mu16Idx));
    }
    else
    {
      //kurz Idx 8Bit
      lcData.Add((u8)(mu16Idx));
    }
    lcData.AddRange(mcPayload);

    base.vEncode(lcData);
  }

  public new void vEncode(List<byte> lMsgPayload)
  {
    mcPayload = lMsgPayload;
    vEncode();
  }

  public new void vEncode(u8[] lau8Payload)
  {
    List<byte> lMsgPayload = new List<byte>(lau8Payload);
    vEncode(lMsgPayload);
  }


};

public class cBotNetMsg_StreamProt : cBotNetMsg_Base
{
  // Header : Cxxx.IIII -> 1 Byte

  // Cxxx : Daten / Command 1 Bit
  //    0 = Daten xxx = 0
  //    1 = Command / Cfg xxx = Kommando

  //    IIII = Stream Index 0..15
  //      0 : Default für Diag / Konsole
  //      1..15 : frei


  public List<byte>          mcPayload;
  public uint8               mu8StreamIdx;
  public bool                mbCmd;
  public uint8               mu8Cmd;

  public cBotNetMsg_StreamProt()
  {
    mcPayload = new List<byte>(cBnCfg.cBotNet_MsgSize);
  }

  public cBotNetMsg_StreamProt(cBotNetMsg_Base lMsgBase)
  {
    mcPayload = new List<byte>(cBnCfg.cBotNet_MsgSize);
    base.mFrame = lMsgBase.mFrame;
    base.mMsgData = lMsgBase.mMsgData;
    //vDecode(0, 0);
    vDecode();
  }


  public new void vDecode()
  {
    //base.vDecode(lu16SAdr, lu16DAdr);
    base.vDecode();

    //Offset-Überspringen vom Botnet-Header
    u8 u8MsgIdx = (u8)base.iGetIdxPayload();
    u8 lu8Data  = mMsgData[u8MsgIdx++];

    u8 lu8DataToCopy;
    lu8DataToCopy = (u8)(base.iGetLenPayload());

    mu8StreamIdx = (u8)(lu8Data & 0x0F);

    if ((lu8Data & 128) > 0)
    {
      //Cmd
      mbCmd = true;
      mu8Cmd = (u8)((lu8Data & 0x70) >> 4);
    }
    else
    {
      //Daten
      mbCmd  = false;
      mu8Cmd = 0;
    }

    lu8DataToCopy -= 1;

    mcPayload.Clear();
    for (int t = 0; t < lu8DataToCopy; t++)
    {
      if (u8MsgIdx < mcPayload.Capacity)
      {
        mcPayload.Add(mMsgData[u8MsgIdx++]);
      }
    }
  }


  public void vPrepareCmd(bool lbAckOn, bool lbAgeOn, bool lbAdressOn,
                          uint16 lui16SourceAdr, uint16 lui16DestAdr, uint8 lui16Age,
                          uint8 lui8Cmd, uint8 lui8Stream)
  {
    base.mFrame.enMsgType = cBotNetMsgFrame.tenBotNetMsgType.enStream;
    base.mFrame.vSet(lui16SourceAdr, lui16DestAdr);

    mbCmd = true;
    mu8Cmd = lui8Cmd;
    mu8StreamIdx = lui8Stream;

    mcPayload.Clear();
  }

  public void vPrepareData(bool lbAckOn, bool lbAgeOn, bool lbAdressOn,
                           uint16 lui16SourceAdr, uint16 lui16DestAdr, uint8 lui16Age,
                           uint8 lui8Stream)
  {
    base.mFrame.enMsgType = cBotNetMsgFrame.tenBotNetMsgType.enStream;
    base.mFrame.vSet(lui16SourceAdr, lui16DestAdr);
    
    mbCmd = false;
    mu8StreamIdx = lui8Stream;

    mcPayload.Clear();
  }

  public void vEncode()
  {
    List<byte> lcData;
    lcData = new List<byte>(cBnCfg.cBotNet_MsgSize);

    if (mbCmd)
    {
      lcData.Add((u8)((mu8StreamIdx & 0x0F) | (mu8Cmd << 4) | 128));
    }
    else
    {
      lcData.Add((u8)(mu8StreamIdx & 0x0F));
    }
    lcData.AddRange(mcPayload);

    base.vEncode(lcData);
  }

  public new void vEncode(List<byte> lMsgPayload)
  {
    mcPayload = lMsgPayload;
    vEncode();
  }

  public new void vEncode(u8[] lau8Payload)
  {
    List<byte> lMsgPayload = new List<byte>(lau8Payload);
    vEncode(lMsgPayload);
  }

};


