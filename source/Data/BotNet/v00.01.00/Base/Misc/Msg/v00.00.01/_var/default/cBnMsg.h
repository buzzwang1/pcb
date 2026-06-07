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
      lu8Adr  = lcDAdr.Encode(lu8Adr);
      lu8Adr  = lcSAdr.Encode(lu8Adr);
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
