#ifndef __BOTNET_MSG_H__
#define __BOTNET_MSG_H__

#include "Typedef.h"
#include "ComDat.h"
#include "cBnAdr.h"


#define cBotNetMsg_Static_MsgProt_Create(MsgName, lui16Size) u8 MsgName##_Buf[lui16Size]; \
                                                             cbArrayExtT<uint16> MsgName##_BAry(MsgName##_Buf, lui16Size); \
                                                             cBotNetMsg_MsgProt MsgName(&MsgName##_BAry);

#define cBotNetMsg_Static_MsgProt_Create_Prepare(MsgName, lui16Size, lu16SrcAdr, lu16DstAdr, lu16Idx) \
                                                            cBotNetMsg_Static_MsgProt_Create(MsgName, lui16Size); \
                                                            MsgName.vPrepare(lu16SrcAdr, lu16DstAdr, lu16Idx);


#define cBotNetMsg_Static_MsgProt_Create_Copy(AdrSrc_Msg, Dst_MsgName, lui16Size) \
                                                             cBotNetMsg_Static_MsgProt_Create(Dst_MsgName, lui16Size); \
                                                             Dst_MsgName.CopyFrom(AdrSrc_Msg);


#define cBotNetMsg_Static_MsgProt_Create_Clone(AdrSrc_Msg, Dst_MsgName, lui16Size) \
                                                             cBotNetMsg_Static_MsgProt_Create(Dst_MsgName, AdrSrc_Msg->Size()); \
                                                             Dst_MsgName.CopyFrom(AdrSrc_Msg);



#ifdef __GNUC__
class __attribute__((packed)) cBotNetMsgFrame
#else
class cBotNetMsgFrame
#endif
{
public:

  typedef enum
  {
    enMessage = 0,
    enStream,
    enPacket,
    enMultiMessage,
    enRMessage
  }tenType;

  #ifdef __GNUC__
    typedef struct
    {
      uint8  MsgType : 3;
      uint8  Reserved : 3;
      uint8  AdrExtOn : 1;
      uint8  AdrIntOn : 1;
    }tstControl;
  #else
    typedef struct
    {
      uint8  AdrIntOn : 1;
      uint8  AdrExtOn : 1;
      uint8  Reserved : 3;
      uint8  MsgType : 3;
    }tstControl;
  #endif


  //u16                       mu16DDAdr;  // 2Byte DeviceAdress, nur für externe Kommunikation
  //u16                       mu16SDAdr;  // 2Byte DeviceAdress, nur für externe Kommunikation
  cBotNetAdress               mcDAdr;     // 2Byte Interne Adresse
  cBotNetAdress               mcSAdr;     // 2Byte Interne Adresse
  cBotNetMsgFrame::tstControl mstControl; // 1Byte

  cBotNetMsgFrame()
  : mcDAdr(), mcSAdr()
  {
    *((u8*)(&mstControl)) = 0;
  }

  uint8 u8Len()
  {
    u8 lu8Ret;
    lu8Ret = 1; // 1Byte ControlByte
    if (mstControl.AdrIntOn) lu8Ret += 4; // 2x 16Bit (Source und DestAdresse)
    if (mstControl.AdrExtOn) lu8Ret += 4; // 2x 16Bit (Source und DestDeviceAdresse)
    return (lu8Ret);
  }


  void vSet(uint16 lui16SourceAdr, uint16 lui16DestAdr)
  {
    if ((lui16SourceAdr) || (lui16DestAdr))
    {
      mstControl.AdrIntOn = 1;
    }
    else
    {
      mstControl.AdrIntOn = 0;
    }
    mcSAdr.Set(lui16SourceAdr);
    mcDAdr.Set(lui16DestAdr);
  }

  inline cBotNetAdress cGetDAdr() {return mcDAdr;};
  inline cBotNetAdress cGetSAdr() {return mcSAdr;};
};


class cBotNetMsg_BaseDyn : public cbArrayExtDynT<uint16>
{
public:

  cBotNetMsg_BaseDyn(uint16 lui16Size)
    : cbArrayExtDynT<uint16>(lui16Size)
  {
  }
};

class cBotNetMsg_Base : public cbArrayExtT<uint16>
{
  public:
    cBotNetMsgFrame mcFrame; // Dekodierter Frame der Nachricht
    u16             mu16Age; // Alter der Nachricht, damit alte Nachrichten ggf. gelöscht werden können.

  cBotNetMsg_Base(cbArrayExtT<uint16> *lcBAry)
  {
    vFromBAry(lcBAry);
  }

  inline void vFromBAry(cbArrayExtT<uint16>* lcBAry)
  {
    this->mpu8Data = lcBAry->mpu8Data;
    this->muiLen   = lcBAry->muiLen;
    this->muiSize  = lcBAry->muiSize;
    mu16Age        = 0;
  }

  inline cBotNetMsgFrame::tenType enGetMsgType()
  {
    return (cBotNetMsgFrame::tenType)mcFrame.mstControl.MsgType;
  }

  inline uint8 u8GetBnFrameSize()
  {
     return mcFrame.u8Len();
  }

  inline cBotNetAdress cGetDAdr()
  {
    return mcFrame.mcDAdr;
  }

  inline cBotNetAdress cGetSAdr()
  {
    return mcFrame.mcSAdr;
  }

  inline bool bHasAdress()
  {
    return (mcFrame.mstControl.AdrIntOn != 0);
  }

  inline u8 u8Len()
  {
    return (u8)(this->muiLen);
  }

  inline u8 iGetIdxPayload()
  {
    return 0;
  }

  inline u8 iGetLenPayload()
  {
    return (u8)(u8Len() - mcFrame.u8Len());
  }

  inline void GetPayload(cbArrayT<uint16>* lpcBAry)
  {
    lpcBAry->muiSize = this->muiLen - u8GetBnFrameSize();
    lpcBAry->mpu8Data = (uint8*)(this->mpu8Data + 1);
  }

  inline void vClearAge()
  {
    mu16Age = 0;
  }

  inline bool bDoAgeing()
  {
    if (mu16Age < 200)
    {
      mu16Age++;
      return False;
    }
    return True;
  }

  void vDecode()
  {
    // Decode von hinten nach vorne
    // Letztes Byte ist das Kontroll-Byte
    u8* lu8Src = this->mpu8Data + (u8Len() - 1);  // Ende der Nachricht
    u8* lu8Dst = (u8*)(&mcFrame) + (sizeof(mcFrame) - 1); // Ende der Frame-Info

    //KontrollByte kopieren
    *lu8Dst = *lu8Src;

    // Daten in Abhängingkeit vom KontrollByte kopieren
    if (mcFrame.mstControl.AdrIntOn)
    {
      lu8Src -= 2;
      mcFrame.mcSAdr.Decode(*((u16*)lu8Src));
      lu8Src -= 2;
      mcFrame.mcDAdr.Decode(*((u16*)lu8Src));
    }
  }

  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    vDecode();

    // Daten in Abhängingkeit vom KontrollByte kopieren
    if (!mcFrame.mstControl.AdrIntOn)
    {
      mcFrame.mcSAdr.Set(lu16SAdr);
      mcFrame.mcDAdr.Set(lu16DAdr);
    }
  }


  void vEncode(u8 lu8PayloadSize)  // __attribute__((optimize("O0")))
  {
    u8 lu8Len = u8GetBnFrameSize();

    this->muiLen = lu8PayloadSize + lu8Len;

    // Daten in Abhängingkeit vom KontrollByte kopieren
    u8* lu8Dst = this->mpu8Data +  (u8Len() - 1);
    u8* lu8Src = (u8*)(&mcFrame) + (sizeof(mcFrame) - 1);

    //KontrollByte kopieren
    *lu8Dst = *lu8Src;

    // Daten in Abhängingkeit vom KontrollByte kopieren
    if (mcFrame.mstControl.AdrIntOn)
    {
      lu8Dst-= 2;
      *((u16*)lu8Dst) = mcFrame.mcSAdr.Encode();
      lu8Dst-= 2;
      *((u16*)lu8Dst) = mcFrame.mcDAdr.Encode();
    }
  }

  void vAdressAdd(uint16 lui16SourceAdr, uint16 lui16DestAdr)
  {
    u8 lu8PayloadSize = this->muiLen - u8GetBnFrameSize();
    mcFrame.vSet(lui16SourceAdr, lui16DestAdr);
    vEncode(lu8PayloadSize);
  }

  void vAdressRemove()
  {
    u8 lu8PayloadSize = this->muiLen - u8GetBnFrameSize();
    mcFrame.vSet(0, 0);
    vEncode(lu8PayloadSize);
  }
};


class cBotNetMsg_MsgProt : public cBotNetMsg_Base
{
  public:


  // [Last Msg Byte Index] MI1[MI2] MD[0..DA] [Botnet Footer]
  // MI: Message Index 8Bit or 16Bit
  // MD: Message Data / Payload

  cbArrayExtT<uint16> mcPayload;
  uint16              mu16Idx;

  cBotNetMsg_MsgProt(cbArrayExtT<uint16> *lpcBAry)
  : cBotNetMsg_Base(lpcBAry)
  {}

  cBotNetMsg_MsgProt(cBotNetMsg_Base *lpcBase)
  : cBotNetMsg_Base(lpcBase)
  {
    mcFrame = lpcBase->mcFrame;
  }

  void CopyFrom(cBotNetMsg_MsgProt &lcMsg)
  {
    if (lcMsg.Len() <= this->Size())
    {
      this->Set(lcMsg);
      vDecode(0,0);
    }
  }

  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vDecode(lu16SAdr, lu16DAdr);

    //Offset-Überspringen
    lpu8Msg   = this->mpu8Data;

    //8Bit oder 16Bit Index ?
    if (*lpu8Msg & 128)
    {
      //lang Idx 16Bit
      mu16Idx = (*lpu8Msg & 0x7F) << 8;
      lpu8Msg++;
      mu16Idx += *(lpu8Msg);

      // Länge Payload. Gesamtlänge - FrameSize  - MsgProt-HeaderSize (1 oder 2 Byte)
      mcPayload.muiLen = this->muiLen - u8GetBnFrameSize() - 2;
    }
    else
    {
      //kurz Idx 8Bit
      mu16Idx = *lpu8Msg;

      // Länge Payload. Gesamtlänge - FrameSize  - MsgProt-HeaderSize (1 oder 2 Byte)
      mcPayload.muiLen = this->muiLen - u8GetBnFrameSize() - 1;
    }

    lpu8Msg++;
    mcPayload.muiSize =  mcPayload.muiLen;
    mcPayload.mpu8Data = lpu8Msg; // Daten
  }

  void vPrepare(uint16 lui16SourceAdr, uint16 lui16DestAdr, uint16 lui16MsgIdx)
  {
    uint8* lpu8Msg;

    this->mcFrame.vSet(lui16SourceAdr, lui16DestAdr);

    lpu8Msg = mpu8Data;

    mu16Idx = lui16MsgIdx;
    //8Bit oder 16Bit Index ?
    if (mu16Idx > 127)
    {
      //lang Idx 16Bit
      *lpu8Msg = (uint8)((mu16Idx >> 8) | 128);
      lpu8Msg++;
      *lpu8Msg = (uint8)mu16Idx;

      mcPayload.muiSize = muiSize - (u8GetBnFrameSize() + 2);
    }
    else
    {
      //kurz Idx 8Bit
      *lpu8Msg = (uint8)mu16Idx;

      mcPayload.muiSize = muiSize - (u8GetBnFrameSize() + 1);
    }

    lpu8Msg++;
    mcPayload.muiLen   = 0;
    mcPayload.mpu8Data = lpu8Msg;

    mcFrame.mstControl.MsgType = cBotNetMsgFrame::tenType::enMessage;
  }


  void vEncode()
  {
    // Gesamtlänge bestimmen

    //8Bit oder 16Bit Index ?
    if (mu16Idx > 127)
    {
      //lang Idx 16Bit
      cBotNetMsg_Base::vEncode(mcPayload.muiLen + 2);
    }
    else
    {
      //kurz Idx 8Bit
      cBotNetMsg_Base::vEncode(mcPayload.muiLen + 1);
    }
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


  cbArrayExtT<uint16> mcPayload;
  uint8               mu8StreamIdx;
  bool                mbCmd;
  uint8               mu8Cmd;

  cBotNetMsg_StreamProt(cbArrayExtT<uint16> *lpcBAry)
  : cBotNetMsg_Base(lpcBAry)
  {}

  cBotNetMsg_StreamProt(cBotNetMsg_Base *lpcBase)
  : cBotNetMsg_Base(lpcBase)
  {
    mcFrame= lpcBase->mcFrame;
  }

  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vDecode(lu16SAdr, lu16DAdr);

    lpu8Msg   = this->mpu8Data;

    mu8StreamIdx = (*lpu8Msg & 0x0F);

    if (*lpu8Msg & 128)
    {
      //Cmd
      mbCmd  = True;
      mu8Cmd = (*lpu8Msg & 0x70) >> 4;
    }
    else
    {
      //Daten
      mbCmd  = False;
      mu8Cmd = 0;
    }

    lpu8Msg++;
    mcPayload.muiLen   = u8Len() - u8GetBnFrameSize() - 1;
    mcPayload.muiSize  = mcPayload.muiLen;
    mcPayload.mpu8Data = lpu8Msg; // Daten
  }


  void vPrepareCmd(uint16 lui16SourceAdr, uint16 lui16DestAdr,
                   uint8 lui8Cmd, uint8 lui8Stream)
  {
    uint8* lpu8Msg;

    this->mcFrame.vSet(lui16SourceAdr, lui16DestAdr);

    lpu8Msg = mpu8Data + 1;

    *lpu8Msg = (lui8Stream & 0x0F) | (lui8Cmd << 4) | 128;
    mcPayload.muiSize = muiSize - (u8GetBnFrameSize());

    lpu8Msg++;
    mcPayload.muiLen = 0;
    mcPayload.mpu8Data = lpu8Msg;
    mcFrame.mstControl.MsgType = cBotNetMsgFrame::tenType::enStream;
  }

  void vPrepareData(uint16 lui16SourceAdr, uint16 lui16DestAdr,
                    uint8 lui8Stream)
  {
    uint8* lpu8Msg;

    this->mcFrame.vSet(lui16SourceAdr, lui16DestAdr);

    lpu8Msg = mpu8Data;

    *lpu8Msg = lui8Stream & 0x0F;
    mcPayload.muiSize = muiSize - (u8GetBnFrameSize());

    lpu8Msg++;
    mcPayload.muiLen = 0;
    mcPayload.mpu8Data = lpu8Msg;
    mcFrame.mstControl.MsgType = cBotNetMsgFrame::tenType::enStream;
  }

  void vEncode()
  {
    cBotNetMsg_Base::vEncode(mcPayload.muiLen + 1);
  }
};



#endif // __BOTNET_MSG_H__
