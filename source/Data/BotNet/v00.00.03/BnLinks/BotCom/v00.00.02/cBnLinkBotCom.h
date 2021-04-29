#ifndef __BOTNET_LINK_BOTCOM_H__
#define __BOTNET_LINK_BOTCOM_H__

#include "Typedef.h"
#include "cUsart.h"

#include "cBnLinkBase.h"


class cBotNet_LinkBotCom: public cBotNet_LinkBase
{
  public:

  cComDatMsg mpcMsgData;

  i16 mi16TxIdx;
  u8  mu8TxChksum;

  u8 mu8RxChecksum;
  u8 mu8RxDataCount;
  u8 mu8RxLastCmd;

  cUart* mcUart;

  cBotNet_LinkBotCom(uint16 lui16Adr, cUart* lcUart)
    : cBotNet_LinkBase(lui16Adr, cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize)
  {
    mStatus.IsInit   = 1;
    mStatus.IsOnline = 1;

    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    mcUart = lcUart;

    mi16TxIdx = -1;

    mu8RxChecksum  = 0;
    mu8RxDataCount = 0;
    mu8RxLastCmd   = 0xFF;
  }

  //~cBotNet_I2c_MaSl_Slave_Slave()
  //{
  //}


  virtual uint32 SizeOf()
  {
    /*uint32 lui32Size;

    lui32Size = sizeof(cBotNet_UplinkBotCom);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsg.SizeOf();
    lui32Size -= 3 * sizeof(cComDatMsg);

    return lui32Size;*/
    return 0;
  }

  void vBotCom_SendStart(u8 u8Len)
  {
    mi16TxIdx = 0;
    mu8TxChksum = 1;

    mcUart->vSend((u8)((7 << 5) + (u8Len & 0x1F)));
    if (u8Len > 31)
    {
      mcUart->vSend((u8)((2 << 5) + (u8Len >> 5)));
    }
  }

  void vBotCom_SendEnd()
  {
    mcUart->mcUartDataOut.put((u8)((0 << 5) + (mu8TxChksum & 0x1F)));
    mi16TxIdx = -1;
  }

  void vBotCom_SendData(u8 u8Data)
  {
     mu8TxChksum += u8Data;

    mcUart->vSend((u8)((1 << 5) + (u8Data & 0x1F)));

    if (u8Data > 31)
    {
      mcUart->vSend((u8)((2 << 5) + (u8Data >> 5)));
    }
    mi16TxIdx++;
  }


  void vSync()
  {
    // Encode data to send
    while (1)
    {
      if (mcUart->mcUartDataOut.space_left() > 8)
      {
        if (mi16TxIdx < 0)
        {
          if (mcTxComBuf.isMsgIn())
          {
            mcTxComBuf.get(&mpcMsgData.cTxData);
            vBotCom_SendStart((u8)mpcMsgData.cTxData.Len());
          }
          else
          {
            break;
          }
        }
      }
      else
      {
        break;
      }

      // Gibt es was zu senden
      // Ist Platz im Uart Puffer
      while ((mcUart->mcUartDataOut.space_left() > 8) && (mi16TxIdx < mpcMsgData.cTxData.Len()))
      {
        vBotCom_SendData(mpcMsgData.cTxData[mi16TxIdx]);
      }

      if (mi16TxIdx == mpcMsgData.cTxData.Len())
      {
        vBotCom_SendEnd();
      }

      if (mcUart->mcUartDataOut.space_left() < 8)
      {
        break;
      }
    }

    // Decode received data
    while (mcUart->mcUartDataIn.cnt() > 0)
    {
      u8 lu8DataIn = mcUart->mcUartDataIn.get();
      u8 lu8Data = (u8)(lu8DataIn & 0x1F);
      u8 lu8Cmd  = (u8)(lu8DataIn >> 5);
      switch (lu8Cmd)
      {
        case 0: // 000 : Übertragung Ende: Checksumme in den unteren 5 Bits
          if (mpcMsgData.cRxData.Len() == mu8RxDataCount)
          {
            mcRxComBuf.put(&mpcMsgData.cRxData);
          }
          break;
        case 1: // 001 : Daten Start: Start Daten. Die unteren 5 Bits
          mu8RxChecksum += lu8Data;
          mpcMsgData.cRxData.Add(lu8Data);
          break;
        case 2: // 010 : Daten Zusatz: Die oberen  3 Bits 
          switch (mu8RxLastCmd)
          {
            case 1: // Daten
              mpcMsgData.cRxData[mpcMsgData.cRxData.muiLen - 1] += (u8)(lu8Data << 5);
              mu8RxChecksum += (u8)(lu8Data << 5);
              break;
            case 7: // Anzahl Daten
              mu8RxDataCount += (u8)(lu8Data << 5);
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
          mu8RxChecksum = 1;
          mu8RxDataCount = lu8Data;
          mpcMsgData.cRxData.muiLen = 0;
          break;
      }
      mu8RxLastCmd = lu8Cmd;
    }
  }

  void vComError(cComNode::tenComNodeError lenError, tenComNodeState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
     mStatus.IsThisTxOk = 0;
  }

  void vResetCom(bool bAlways) override
  {
    cBotNet_LinkBase::vResetCom(bAlways);
    if ((!IsOnline()) || (bAlways))
    {
      vResetStatusFlags();

      mStatus.IsInit   = 1;
      mStatus.IsOnline = 1;
    }
  }

  void vComStart(tenComNodeStartState lenState)
  {
    (void)lenState;
  }

  void  vComDone()
  {
  }

};


#endif // __BOTNET_LINK_BOTCOM_H__
