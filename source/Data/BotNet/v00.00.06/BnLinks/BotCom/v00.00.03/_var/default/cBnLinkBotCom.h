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
  u16 mu16BytesPer1ms;
  u8  mu8TxChksum;

  u8 mu8RxChecksum;
  u8 mu8RxDataCount;
  u8 mu8RxLastCmd;

  struct tstMode
  {
    u8 WaitforAcknowledge  : 1; // Not Implemented
    u8 SendAcknowledge     : 1; // Not Implemented
    u8 SendMessageCounter  : 1;
  };

  tstMode mstMode;

  cUart* mcUart;

  cBotNet_LinkBotCom(cUart* lcUart, u32 lu32BytePerSync = 4)
    // lu32BytePerSync: If Sync is called in 1ms context
    //    lu32BytePerSync = (lu32Baudrate / 10000) + 1;
    : cBotNet_LinkBase(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enSideLink)
  {
    mStatus.IsInit   = 1;
    mStatus.IsOnline = 1;

    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    mcUart = lcUart;

    mi16TxIdx = -1;

    mu8RxChecksum  = 0;
    mu8RxDataCount = 0;
    mu8RxLastCmd   = 0xFF;

    mstMode.WaitforAcknowledge = 0;
    mstMode.SendMessageCounter = 1;
    mstMode.SendAcknowledge    = 1;

    mu16BytesPer1ms = lu32BytePerSync;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    return cBotNet_LinkBase::bAddedToBn(lu16Adr);
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

    // case 7: // 111 : Übertragung Start: Anzahl Daten (Low 5-Bit) in den unteren 5 Bits
    mcUart->vSend((u8)((7 << 5) + (u8Len & 0x1F)));
    if (mstMode.SendMessageCounter)
    {
      // case 2: // 010 : Daten Zusatz: 2 Bit MessageCounter + Die oberen 3 Bits
      mcUart->vSend((u8)((2 << 5) + (u8)((mu8MsgCntTx & 0x3) << 3) + (u8Len >> 5)));
    }
    else
    {
      if (u8Len > 31)
      {
        // case 2: // 010 : Daten Zusatz: Die oberen  3 Bits
        mcUart->vSend((u8)((2 << 5) + (u8Len >> 5)));
      }
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

    u16 lu16DataOutCnt = 0;

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
            lu16DataOutCnt++;
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
      while (mi16TxIdx < mpcMsgData.cTxData.Len())
      {
        vBotCom_SendData(mpcMsgData.cTxData[mi16TxIdx]);
        lu16DataOutCnt++;

        if ((mcUart->mcUartDataOut.space_left() < 8) ||
            (lu16DataOutCnt > mu16BytesPer1ms))
        {
          break;
        }
      }

      if (mi16TxIdx == mpcMsgData.cTxData.Len())
      {
        vBotCom_SendEnd();
        lu16DataOutCnt++;
      }

      if ((mcUart->mcUartDataOut.space_left() < 8) ||
          (lu16DataOutCnt > mu16BytesPer1ms))
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
          {
            bool isValid   = True;
            bool isNoSpace = False;

            if (mpcMsgData.cRxData.Len() != mu8RxDataCount)
            {
              isValid = False;
            }

            // Message Counter überprüfen
            if (mstMode.SendAcknowledge)
            {
              // Ist noch genügend Platz im Buffer ?
              if (mcRxComBuf.space_left() < (mpcMsgData.cRxData.Len() + 2)) // +4 Buffer
              {
                isValid = False;
                isNoSpace = True;
              }
            }

            if (isValid)
            {
              // Message Counter überprüfen
              if (mstMode.SendAcknowledge)
              {
                // Neue Nachricht oder die gleiche wiederholt
                if (mu8MsgCntRx_Last != mu8MsgCntRx)
                {
                  mcRxComBuf.put(&mpcMsgData.cRxData);
                  mu8MsgCntRx_Last = mu8MsgCntRx;
                }
              }
              else
              {
                mcRxComBuf.put(&mpcMsgData.cRxData);
              }
            }

            if (mstMode.SendAcknowledge)
            {
              u8 luStatus = (u8)(0x03 << 5);
              if (isNoSpace) luStatus |= 2;
              if (isValid)   luStatus |= 1;
              mcUart->vSend(luStatus);
            }
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
              mpcMsgData.cRxData[mpcMsgData.cRxData.muiLen - 1] += (u8)((lu8Data & 0x7) << 5);
              mu8RxChecksum += (u8)(lu8Data << 5);
              break;
            case 7: // Anzahl Daten
              mu8RxDataCount += (u8)((lu8Data & 0x07) << 5);
              mu8MsgCntRx     = (u8)((lu8Data & 0x18) >> 3);
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

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
     mStatus.IsThisTxOk = 0;
  }

  /*void vOnResetCom() override
  {
    mStatus.IsInit   = 1;
    mStatus.IsOnline = 1;
  }*/

  void vComStart(cComNode::tenEvent lenEvent)
  {
    (void)lenEvent;
  }

  void  vComDone()
  {
  }

  void vTick1ms() override
  {
  }
};


#endif // __BOTNET_LINK_BOTCOM_H__
