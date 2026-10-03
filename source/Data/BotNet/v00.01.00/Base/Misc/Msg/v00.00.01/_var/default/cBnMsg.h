#ifndef __BOTNET_MSG_H__
#define __BOTNET_MSG_H__

#include "Typedef.h"
#include "ComDat.h"
#include "cBnAdr.h"


// -4 -3 -2 -1  0
// DA DA SA SA CC

class cBotNetMsg_Base : public cBarryPtrT<u16>
{
  public:
  typedef enum
  {
    enMessage = 0,
    enStream,
    enPacket,
    enMultiMessage,
  }tenType;

  #ifdef __GNUC__
    typedef struct
    {
      uint8  MsgType  : 3;
      uint8  OneWay   : 1;
      uint8  NoCheck  : 1;
      uint8  Reserved : 1;
      uint8  AdrExtOn : 1;
      uint8  AdrIntOn : 1;
    }tstControl;
  #else
    typedef struct
    {
      u8  AdrIntOn : 1;
      u8  AdrExtOn : 1; // (not implemented)
      u8  Reserved : 1;
      u8  NoCheck  : 1; // Reduced Msg Format(Busy Flag, Checksum and Ack is ignored)
      u8  OneWay   : 1; // No Ack, from Master to Slave, for Broadcast/Multicast (not implemented)
      u8  MsgType  : 3; // 0: Msg Format
                        // 1: Stream Format
                        // 2: Package Format (not implemented)
                        // 3: Multi Msg Format Format (not implemented)
                        // 4: ---
                        // 5: ---
                        // 6: ---
                        // 7: Ack-Msg (not implemented)
    }tstControl;
  #endif

  typedef union
  {
    u8 Byte;
    tstControl Bits;
  }tunControl;

  cBotNetMsg_Base()
    : cBarryPtrT<u16>()
  {}

  cBotNetMsg_Base(cBarryPtrT<u16> &lcBAry)
    : cBarryPtrT<u16>(lcBAry)
  {}

  cBotNetMsg_Base(u8* lpaArray, u16 luiSize)
    : cBarryPtrT<u16>(lpaArray, luiSize)
  {}

  tunControl GetControl()
  {
    // Controlbyte is the last Byte
    tunControl lunCtrl;
    lunCtrl.Byte = this->mpu8Data[this->muLen - 1];
    return lunCtrl;
  }

  void SetControl(tunControl lunCtrl)
  {
    // Controlbyte is the last Byte
    this->mpu8Data[this->muLen - 1] = lunCtrl.Byte;
  }

  void vSetNoCheck(u8 lu8State) { tunControl lunCtrl = GetControl();
                                  lunCtrl.Bits.NoCheck = lu8State;
                                  SetControl(lunCtrl);
                                }
  bool isNoCheck()              { return GetControl().Bits.NoCheck; }

  void vSetOneWay(u8 lu8State)  { tunControl lunCtrl = GetControl();
                                  lunCtrl.Bits.OneWay = lu8State;
                                  SetControl(lunCtrl);
                                }
  bool isOneWay()               { return GetControl().Bits.OneWay; }

  void vAddFrame(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, tunControl lunCtrl)
  {
    u8* lu8Adr = this->mpu8Data + this->muLen;

    if ((lcSAdr.isSet()) || (lcDAdr.isSet()))
    {
      lunCtrl.Bits.AdrIntOn = 1;
    }
    else
    {
      lunCtrl.Bits.AdrIntOn = 0;
    }

    // -4 -3 -2 -1  0
    // DA DA SA SA CC
    if (lunCtrl.Bits.AdrIntOn)
    {
      lu8Adr  = lcDAdr.Encode(lu8Adr);
      lu8Adr  = lcSAdr.Encode(lu8Adr);
    }
    *lu8Adr++ = lunCtrl.Byte;
    this->muLen = lu8Adr - this->mpu8Data;
  }

  void vSetAdr(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr)
  {
    u8* lu8Adr = this->mpu8Data + this->muLen - u8GetBnFrameSize();

    tunControl lunCtrl = GetControl();
    if ((lcSAdr.isSet()) || (lcDAdr.isSet()))
    {
      lunCtrl.Bits.AdrIntOn = 1;
    }
    else
    {
      lunCtrl.Bits.AdrIntOn = 0;
    }

    // -4 -3 -2 -1  0
    // DA DA SA SA CC
    if (lunCtrl.Bits.AdrIntOn)
    {
      lu8Adr = lcDAdr.Encode(lu8Adr);
      lu8Adr = lcSAdr.Encode(lu8Adr);
    }
    *lu8Adr++ = lunCtrl.Byte;
    this->muLen = lu8Adr - this->mpu8Data;
  }

  cBotNetAdress cGetDAdr()
  {
    cBotNetAdress lcAdr;
    tunControl lunCtrl = GetControl();
    if (lunCtrl.Bits.AdrIntOn)
    {
      u8* lu8Adr = &this->mpu8Data[this->muLen - 5];
      lcAdr.Decode(lu8Adr);
    }
    return lcAdr;
  };

  cBotNetAdress cGetSAdr()
  {
    cBotNetAdress lcAdr;
    if (GetControl().Bits.AdrIntOn)
    {
      u8* lu8Adr = &this->mpu8Data[this->muLen - 3];
      lcAdr.Decode(lu8Adr);
    }
    return lcAdr;
  };

  tenType enGetMsgType()
  {
    return (tenType)GetControl().Bits.MsgType;
  }

  bool bHasAdress()
  {
    return (GetControl().Bits.AdrIntOn != 0);
  }

  u8 u8GetBnFrameSize()
  {
    u8 lu8Ret;
    lu8Ret = 1; // 1Byte ControlByte
    if (GetControl().Bits.AdrIntOn) lu8Ret += 4; // 2x 16Bit (Source und DestAdresse)
    if (GetControl().Bits.AdrExtOn) lu8Ret += 4; // 2x 16Bit (Source und DestDeviceAdresse)
    return (lu8Ret);
  }

  u8 u8GetFramePos()
  {
    u8 lu8Ret;
    lu8Ret = (u8)(this->muLen) - 1; // 1Byte ControlByte
    if (GetControl().Bits.AdrIntOn) lu8Ret -= 4; // 2x 16Bit (Source und DestAdresse)
    if (GetControl().Bits.AdrExtOn) lu8Ret -= 4; // 2x 16Bit (Source und DestDeviceAdresse)
    return (lu8Ret);
  }


  void vFlipAdress(cBotNetMsg_Base &lcMsg)
  {
    cBotNetAdress lcDAdr;
    cBotNetAdress lcSAdr;
    if (lcMsg.bHasAdress())
    {
      lcDAdr = lcMsg.cGetSAdr();
      lcSAdr = lcMsg.cGetDAdr();
    }
    vSetAdr(lcSAdr, lcDAdr);
  }

  u8 u8Len()
  {
    return (u8)(this->muLen);
  }

  u8 iGetPayloadPos()
  {
    return 0;
  }

  u8 iGetLenPayload()
  {
    return (u8Len() - u8GetBnFrameSize());
  }

  cBarryPtrT<u16>& GetPayload(cBarryPtrT<u16> &lpcBAry, u8 lu8Offset)
  {
    u8 lu8Size = this->muLen - u8GetBnFrameSize() - lu8Offset;
    lpcBAry.From(this->mpu8Data + lu8Offset, lu8Size, lu8Size);
    return lpcBAry;
  }

  void vAdressAdd(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr)
  {
    vSetAdr(lcSAdr, lcDAdr);
  }

  void vAdressRemove()
  {
    vSetAdr(0, 0);
  }
};


class cBotNetMsg_MsgProt : public cBotNetMsg_Base
{
  public:

  struct ID
  {
    // MI = Message ID
    // S1 = Sub ID
    // S2 = Sub ID
    enum tenType : u8
    {
      nRequest  = 0,
      nSet      = 1,
      nResponse = 2,
    };
  };

  struct ID8 : public ID
  {
    u8 mu8Value;
    constexpr ID8(tenType lenType, u8 lu8MI)  :mu8Value(((u8)lenType << 6) + (lu8MI & 0x3F)) {}
    constexpr operator u8() const { return (static_cast<u8>(mu8Value)); }
  };

  struct ID16 : public ID
  {
    u16 mu16Value;
    constexpr ID16(tenType lenType, u8 lu8MI, u8 lu8S1) : mu16Value(((u16)(((u8)lenType << 6) | (lu8MI & 0x3F)) << 8) + lu8S1) {}
    constexpr operator u16() const { return (static_cast<u16>(mu16Value)); }
  };

  struct ID24 : public ID
  {
    u32 mu32Value;
    constexpr ID24(tenType lenType, u8 lu8MI, u8 lu8S1, u8 lu8S2)
      : mu32Value(((u32)(((u8)lenType << 6) | (lu8MI & 0x3F)) << 16) + ((u32)lu8S1 << 8) + lu8S2)
    {}
    constexpr operator u32() const { return (static_cast<u32>(mu32Value)); }
  };

  cBarryPtrT<u16> mcPayload;
  // [Last Msg Byte Index] MI1[MI2] MD[0..DA] [Botnet Footer]
  // MI: Message Index 8Bit or 16Bit
  // MD: Message Data / Payload

  cBotNetMsg_MsgProt()
    : cBotNetMsg_Base()
  {}

  cBotNetMsg_MsgProt(cBarryPtrT<u16> &lcBAry)
    : cBotNetMsg_Base(lcBAry)
  {}

  cBotNetMsg_MsgProt(u8* lpaArray, u16 luiSize)
    : cBotNetMsg_Base(lpaArray, luiSize)
  {}


  u16 u16GetIdx()
  {
    return *this->mpu8Data;
  }

  cBarryPtrT<u16>& GetPayload()
  {
    return cBotNetMsg_Base::GetPayload(mcPayload, 1);
  }

  cBarryPtrT<u16>& cPrepare(u16 luMsgIdx)
  {
    *this->mpu8Data = (uint8)luMsgIdx;
    mcPayload.From(this->mpu8Data + 1, 0, this->muSize - 1);
    return this->mcPayload;
  }

  void vEncode(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr)
  {
    // Ende vom Payload bestimmen
    this->muLen = 1 + mcPayload.muLen;
    tunControl lunCtrl = { .Byte = 0 };
    lunCtrl.Bits.MsgType = enMessage;
    vAddFrame(lcSAdr, lcDAdr, lunCtrl);
  }

  ID::tenType enGetType() { return (ID::tenType)((*this->mcPayload.mpu8Data) >> 6); }
  bool isRequest()    { return (enGetType() == ID::nRequest); }
  bool isSet()        { return (enGetType() == ID::nSet); }
  bool isResponse()   { return (enGetType() == ID::nResponse); }

  bool isId(ID8 lcID8)   { return (*this->mcPayload.mpu8Data == lcID8); }
  bool isId(ID16 lcID16) { return cMemTools::u16U8toU16(this->mcPayload.mpu8Data) == lcID16; }
  bool isId(ID24 lcID24) { return (cMemTools::u32U8toU32(this->mcPayload.mpu8Data) >> 8) == lcID24; }

  u32  u32GetId() { return (cMemTools::u32U8toU32(this->mcPayload.mpu8Data) >> 8); }

  bool isSetId(const u8 lu8MI, const u8 lu8S1, const u8 lu8S2)
  {
    return ((cMemTools::u32U8toU32(this->mcPayload.mpu8Data) >> 8) == ID24(ID::nSet, lu8MI, lu8S1, lu8S2));
  }

  static constexpr ID24 cId24(const u8 lu8MI, const u8 lu8S1, const u8 lu8S2)
  {
    return ID24(ID::nSet, lu8MI, lu8S1, lu8S2);
  }

  static void vRespId(const u8 lu8MI, const u8 lu8S1, const u8 lu8S2, u8* lpu8Dest)
  {
    lpu8Dest[0] = ID8(ID::nResponse, lu8MI);
    lpu8Dest[1] = lu8S1;
    lpu8Dest[2] = lu8S2;
  }

  static constexpr u8 u8RespId(const u8 lu8MI)
  {
    return ID8(ID::nResponse, lu8MI);
  }
};



class cBotNetMsg_StreamProt : public cBotNetMsg_Base
{
  public:

  // Header : Cxxx.IIII -> 1 Byte

  // Cxxx : Daten / Command 1 Bit
  //    0 = Daten xxx = 0
  //    1 = Command / Cfg xxx = Kommando

  //    IIII = Stream Index 0..15
  //      0 : Default für Diag / Konsole
  //      1..15 : frei
  cBarryPtrT<u16> mcPayload;

  cBotNetMsg_StreamProt()
    : cBotNetMsg_Base()
  {}

  cBotNetMsg_StreamProt(cBarryPtrT<u16> &lcBAry)
    : cBotNetMsg_Base(lcBAry)
  {}

  cBotNetMsg_StreamProt(u8* lpaArray, u16 luiSize)
    : cBotNetMsg_Base(lpaArray, luiSize)
  {}

  bool isCommand() { return ((*this->mpu8Data & 128) != 0); }
  bool isData()    { return ((*this->mpu8Data & 128) == 0); }

  u8   uGetCommand()   { return ((*this->mpu8Data & 0x70) >> 4); }
  u8   uGetStreamIdx() { return (*this->mpu8Data & 0x0F); }


  cBarryPtrT<u16>& GetPayload()
  {
    return cBotNetMsg_Base::GetPayload(mcPayload, 1);
  }

  cBarryPtrT<u16>& cPrepareCmd(u8 luCmd, u8 luStream)
  {
    *this->mpu8Data = (luStream & 0x0F) | (luCmd << 4) | 128;
    mcPayload.From(this->mpu8Data + 1, 0, this->muSize - 1);
    return this->mcPayload;
  }

  cBarryPtrT<u16>& cPrepareData(u8 luStream)
  {
    *this->mpu8Data = luStream & 0x0F;
    mcPayload.From(this->mpu8Data + 1, 0, this->muSize - 1);
    return this->mcPayload;
  }

  void vEncode(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr)
  {
    // Ende vom Payload bestimmen
    this->muLen = 1 + mcPayload.muLen;
    tunControl lunCtrl = { .Byte = 0 };
    lunCtrl.Bits.MsgType = enStream;
    vAddFrame(lcSAdr, lcDAdr, lunCtrl);
  }
};






#endif // __BOTNET_MSG_H__
