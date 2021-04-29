#ifndef __BOTNET_MSG_H__
#define __BOTNET_MSG_H__

#include "Typedef.h"
#include "cBnAdr.h"

class cBotNetHeader
{
public:

  typedef enum
  {
    enBnHdTranstype_Reserved = 0,
    enBnHdTranstype_ShortNoAck,
    enBnHdTranstype_ShortAck,
    enBnHdTranstype_FullNoAck,
    enBnHdTranstype_FullAck,
    enBnHdTranstype_FullPlusNoAck,
    enBnHdTranstype_FullPlusAck,
    enBnHdTranstype_Ack
  }tenBotNetHeader_TransType;


  typedef enum
  {
    enBnHdMsgtype_Message = 0,
    enBnHdMsgtype_Stream,
    enBnHdMsgtype_Packet,
    enBnHdMsgtype_MultiMessage
  }tenBotNetHeader_MsgType;

  #ifdef __GNUC__
    typedef struct
    {
      uint8  MsgType : 3;
      uint8  Reserved : 2;
      uint8  AckOn : 1;
      uint8  AgeOn : 1;
      uint8  AdressOn : 1;
    }tstBotNetHeaderType;
  #else
    typedef struct
    {
      uint8  AdressOn : 1;
      uint8  AgeOn : 1;
      uint8  AckOn : 1;
      uint8  Reserved : 2;
      uint8  MsgType : 3;
    }tstBotNetHeaderType;
  #endif

  #ifdef __GNUC__
    typedef struct
    {
      uint8  MsgType : 3;
      uint8  Reserved : 2;
      uint8  AckOn : 1;
      uint8  AdressAgeOn : 2;
    }tstBotNetHeaderType2;
  #else
    typedef struct
    {
      uint8  AdressAgeOn : 2;
      uint8  AckOn : 1;
      uint8  Reserved : 2;
      uint8  MsgType : 3;
    }tstBotNetHeaderType2;
  #endif


  #ifdef __GNUC__
    typedef struct __attribute__ ((__packed__))
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
    }tstBotNetHeaderBase;

    typedef struct __attribute__ ((__packed__))
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      cBotNetAdress        cAdr;    // 2Byte
    }tstBotNetHeaderBaseAdr;

    typedef struct __attribute__ ((__packed__))
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      uint8                u8Age;   // 1Byte
    }tstBotNetHeaderBaseAge;


    typedef struct __attribute__ ((__packed__))
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      cBotNetAdress        cDAdr;   // 2Byte
      cBotNetAdress        cSAdr;   // 2Byte
      uint8                u8Age;   // 1Byte
    }tstBotNetHeaderBaseAdrAge;
  #else
    typedef struct
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
    }tstBotNetHeaderBase;

    typedef struct
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      cBotNetAdress        cAdr;    // 2Byte
    }tstBotNetHeaderBaseAdr;

    typedef struct
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      uint8                u8Age;   // 1Byte
    }tstBotNetHeaderBaseAge;


    typedef struct
    {
      tstBotNetHeaderType  stType;  // 1Byte
      uint8                u8Len;   // 1Byte
      cBotNetAdress        cDAdr;   // 2Byte
      cBotNetAdress        cSAdr;   // 2Byte
      uint8                u8Age;   // 1Byte
    }tstBotNetHeaderBaseAdrAge;
  #endif

  inline static uint8 u8Len(tstBotNetHeaderBaseAdrAge* lstHeader)
  {
    u8 lu8Ret;
    lu8Ret = 2;
    if (((tstBotNetHeaderType*)lstHeader)->AdressOn) lu8Ret += 4; // 16Bit Source und DestAdresse
    if (((tstBotNetHeaderType*)lstHeader)->AgeOn)    lu8Ret += 1; // 1Byte Alterungszähler
    return (lu8Ret);
  }

  inline static void GetPayload(tstBotNetHeaderBaseAdrAge* lstHeader, cbArrayT<uint16>* lpcBAry)
  {
    uint8 lu8Len = u8Len(lstHeader);
    lpcBAry->muiSize = lstHeader->u8Len - lu8Len;
    lpcBAry->mpu8Data = ((uint8*)(lstHeader)) + lu8Len;
  }
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
  cBotNetHeader::tstBotNetHeaderBaseAdrAge mstHeader; // Dekodierter (Byteorder) Header der Nachricht

  cBotNetMsg_Base(cbArrayExtT<uint16> *lcBAry)
  {
    this->mpu8Data          = lcBAry->mpu8Data;
    this->muiLen            = lcBAry->muiLen;
    this->muiSize           = lcBAry->muiSize;
  }

  inline cBotNetHeader::tstBotNetHeaderBaseAdrAge *cpGetBnHeader()
  {
     return (cBotNetHeader::tstBotNetHeaderBaseAdrAge*)this->mpu8Data;
  }

  inline uint8 u8GetBnHeaderSize()
  {
     return cBotNetHeader::u8Len(&mstHeader);
  }

  inline cBotNetAdress cGetDAdr()
  {
    return mstHeader.cDAdr;
  }

  inline cBotNetAdress cGetSAdr()
  {
    return mstHeader.cSAdr;
  }

  inline bool bHasAdress()
  {
    return (mstHeader.stType.AdressOn != 0);
  }

  // Dekodierung des Headers aus dem BAry 
  //     [0]   [1]  [2]   [4]    [6]
  // ABC0.0SSS PS  [DA DA SA SA] [AG]   mit Adresse
  // ABC0.0SSS PS  [AG]                 ohne Adresse

  // Wenn keine Adresse    in der Nachricht, dann wird der übergebene Wert benutzt
  // Wenn keine AgeCounter in der Nachricht, dann wird er  auf 0 gesetzt
  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    cBotNetHeader::tstBotNetHeaderBaseAdrAge *lstBnHe;
    lstBnHe = (cBotNetHeader::tstBotNetHeaderBaseAdrAge*)this->mpu8Data;

    //Kopieren der ersten 2 Byte (Flags, Type, Größe)
    *((uint16*)&mstHeader) = *((uint16*)lstBnHe);

    if (mstHeader.stType.AdressOn)
    {
      mstHeader.cDAdr.Decode(lstBnHe->cDAdr.Get());
      mstHeader.cSAdr.Decode(lstBnHe->cSAdr.Get());
    }
    else
    {
      mstHeader.cSAdr.Set(lu16SAdr);
      mstHeader.cDAdr.Set(lu16DAdr);
    }

    if (mstHeader.stType.AgeOn)
    {
      if (mstHeader.stType.AdressOn)
      {
        mstHeader.u8Age = ((uint8*)lstBnHe)[6];
      }
      else
      {
        mstHeader.u8Age = ((uint8*)lstBnHe)[2];
      }
    }
    else
    {
      mstHeader.u8Age = 0;
    }
  }

  void vSetHeader(bool lbAckOn, bool lbAgeOn, bool lbAdressOn, uint16 lui16SourceAdr, uint16 lui16DestAdr, u8 lui8Age)
  {
    mstHeader.stType.AgeOn = lbAgeOn;
    mstHeader.stType.AdressOn = lbAdressOn;
    mstHeader.stType.AckOn = lbAckOn;
    mstHeader.cSAdr.Set(lui16SourceAdr);
    mstHeader.cDAdr.Set(lui16DestAdr);
    mstHeader.u8Age = lui8Age;
  }

  void vEncode()
  {
    cBotNetHeader::tstBotNetHeaderBaseAdrAge* lstBnHeader;
    lstBnHeader = cpGetBnHeader();
    
    //Kopieren der ersten 2 Byte (Flags, Type, Größe)
    *((uint16*)lstBnHeader) = *((uint16*)&mstHeader);

    if (mstHeader.stType.AdressOn)
    {
      lstBnHeader->cSAdr.Set(mstHeader.cSAdr.Encode());
      lstBnHeader->cDAdr.Set(mstHeader.cDAdr.Encode());
    }

    if (mstHeader.stType.AgeOn)
    {
      if (mstHeader.stType.AdressOn)
      {
        ((uint8*)lstBnHeader)[6] = mstHeader.u8Age;
      }
      else
      {
        ((uint8*)lstBnHeader)[2] = mstHeader.u8Age;
      }
    }

    muiLen = mstHeader.u8Len;
  }
};


class cBotNetMsg_MsgProt : public cBotNetMsg_Base
{
  public:

  
  // [Botnet Header] MI1[MI2] MD[0..DA]
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
    mstHeader = lpcBase->mstHeader;
  }

  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vDecode(lu16SAdr, lu16DAdr);

    //Offset-Überspringen vom Botnet-Header
    lpu8Msg   = &(this->mpu8Data[u8GetBnHeaderSize()]);

    //8Bit oder 16Bit Index ?
    if (*lpu8Msg & 128)
    {
      //lang Idx 16Bit
      mu16Idx = (*lpu8Msg & 0x7F) << 8;
      lpu8Msg++;
      mu16Idx += *(lpu8Msg);
    }
    else
    {
      //kurz Idx 8Bit
      mu16Idx = *lpu8Msg;
    }

    lpu8Msg++;
    mcPayload.muiLen   = mstHeader.u8Len - (uint16)(lpu8Msg - this->mpu8Data); // Länge Payload. Gesamtlänge - Offset vom Start der Payload
    mcPayload.muiSize  = this->muiSize   - (uint16)(lpu8Msg - this->mpu8Data); // Größe Payload. Gesamtgröße - Offset vom Start der Payload
    mcPayload.mpu8Data = lpu8Msg; // Daten
  }

  void vPrepare(bool lbAckOn, bool lbAgeOn, bool lbAdressOn, uint16 lui16SourceAdr, uint16 lui16DestAdr, uint16 lui16MsgIdx, uint8 lui16Age)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vSetHeader(lbAckOn, lbAgeOn, lbAdressOn, lui16SourceAdr, lui16DestAdr, lui16Age);

    lpu8Msg = &(mpu8Data[u8GetBnHeaderSize()]);
    mu16Idx = lui16MsgIdx;

    //8Bit oder 16Bit Index ?
    if (mu16Idx > 127)
    {
      //lang Idx 16Bit
      *lpu8Msg = (uint8)((mu16Idx >> 8) | 128);
      lpu8Msg++;
      *lpu8Msg = (uint8)mu16Idx;

      mcPayload.muiSize = muiSize - (u8GetBnHeaderSize() + 2);
    }
    else
    {
      //kurz Idx 8Bit
      *lpu8Msg = (uint8)mu16Idx;

      mcPayload.muiSize = muiSize - (u8GetBnHeaderSize() + 1);
    }

    lpu8Msg++;
    mcPayload.muiLen   = 0;
    mcPayload.mpu8Data = lpu8Msg;

    mstHeader.stType.MsgType = cBotNetHeader::tenBotNetHeader_MsgType::enBnHdMsgtype_Message;
  }

  void vEncode()
  {
    // Gesamtlänge bestimmen

    //8Bit oder 16Bit Index ?
    if (mu16Idx > 127)
    {
      //lang Idx 16Bit
      mstHeader.u8Len = u8GetBnHeaderSize() + mcPayload.muiLen + 2;
    }
    else
    {
      //kurz Idx 8Bit
      mstHeader.u8Len = u8GetBnHeaderSize() + mcPayload.muiLen + 1;
    }
    cBotNetMsg_Base::vEncode();
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
    mstHeader = lpcBase->mstHeader;
  }

  void vDecode(uint16 lu16SAdr, uint16 lu16DAdr)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vDecode(lu16SAdr, lu16DAdr);

    lpu8Msg   = &(this->mpu8Data[u8GetBnHeaderSize()]);

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
    mcPayload.muiLen   = mstHeader.u8Len - (uint16)(lpu8Msg - this->mpu8Data); // Länge Payload. Gesamtlänge - Offset vom Start der Payload
    mcPayload.muiSize  = this->muiSize   - (uint16)(lpu8Msg - this->mpu8Data); // Größe Payload. Gesamtgröße - Offset vom Start der Payload
    mcPayload.mpu8Data = lpu8Msg; // Daten
  }


  void vPrepareCmd(bool lbAckOn, bool lbAgeOn, bool lbAdressOn,
                   uint16 lui16SourceAdr, uint16 lui16DestAdr, uint8 lui16Age,
                   uint8 lui8Cmd, uint8 lui8Stream)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vSetHeader(lbAckOn, lbAgeOn, lbAdressOn, lui16SourceAdr, lui16DestAdr, lui16Age);

    lpu8Msg = &(mpu8Data[u8GetBnHeaderSize()]);

    *lpu8Msg = (lui8Stream & 0x0F) | (lui8Cmd << 4) | 128;
    mcPayload.muiSize = muiSize - (u8GetBnHeaderSize() + 2);

    lpu8Msg++;
    mcPayload.muiLen = 0;
    mcPayload.mpu8Data = lpu8Msg;
    mstHeader.stType.MsgType = cBotNetHeader::tenBotNetHeader_MsgType::enBnHdMsgtype_Stream;
  }

  void vPrepareData(bool lbAckOn, bool lbAgeOn, bool lbAdressOn,
                    uint16 lui16SourceAdr, uint16 lui16DestAdr, uint8 lui16Age,
                    uint8 lui8Stream)
  {
    uint8* lpu8Msg;

    cBotNetMsg_Base::vSetHeader(lbAckOn, lbAgeOn, lbAdressOn, lui16SourceAdr, lui16DestAdr, lui16Age);

    lpu8Msg = &(mpu8Data[u8GetBnHeaderSize()]);

    *lpu8Msg = lui8Stream & 0x0F;
    mcPayload.muiSize = muiSize - (u8GetBnHeaderSize() + 2);

    lpu8Msg++;
    mcPayload.muiLen = 0;
    mcPayload.mpu8Data = lpu8Msg;
    mstHeader.stType.MsgType = cBotNetHeader::tenBotNetHeader_MsgType::enBnHdMsgtype_Stream;
  }

  void vEncode()
  {
    // Gesamtlänge bestimmen
    mstHeader.u8Len = u8GetBnHeaderSize() + mcPayload.muiLen + 1;

    cBotNetMsg_Base::vEncode();
  }
};



#endif // __BOTNET_MSG_H__
