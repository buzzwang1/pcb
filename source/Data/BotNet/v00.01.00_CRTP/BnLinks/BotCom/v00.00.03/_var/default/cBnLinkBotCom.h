#ifndef __BOTNET_LINK_BOTCOM_H__
#define __BOTNET_LINK_BOTCOM_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"

#include "cUsart.h"

// -------------------------------------------------------------------------------------------

class cBotNet_LinkBotCom : public cBotNet_LinkBase
{
  public:

  cBotNetMsg_Base mpcMsgDataR;
  cBotNetMsg_Base mpcMsgDataT;

  u16 mu16BytesPer1ms;
  u8 mu8TxByteIdx;

  u8  mu8TxChksum;

  u8 mu8RxChecksum;
  u8 mu8RxDataCount;
  u8 mu8RxLastCmd;

  u8 mu8PoolIdxTx;
  u8 mu8PoolIdxRx;

  struct tstMode
  {
    u8 WaitforAcknowledge  : 1;
    u8 SendAcknowledge     : 1;
    u8 SendMessageCounter  : 1;
  };

  tstMode mstMode;

  cUart* mcUart;

  cBotNet_LinkBotCom(cUart* lcUart, u32 lu32BytePerSync = 4)
    // lu32BytePerSync: If Sync is called in 1ms context
    //    lu32BytePerSync = (lu32Baudrate / 10000) + 1;
    : cBotNet_LinkBase(cBotNet_LinkBase::enSideLink)
  {
    mStatus.IsInit    = 1;
    mStatus.IsOnline  = 1;
    mStatus.IsEnabled = 1;

    mcUart = lcUart;

    mu8TxByteIdx = 0;

    mu8RxChecksum  = 0;
    mu8RxDataCount = 0;
    mu8RxLastCmd   = 0xFF;

    mstMode.WaitforAcknowledge = 0;
    mstMode.SendMessageCounter = 1;
    mstMode.SendAcknowledge    = 1;

    mu8PoolIdxTx = 0;
    mu8PoolIdxRx = 0;

    mu16BytesPer1ms = lu32BytePerSync;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    return cBotNet_LinkBase::bAddedToBn(lu16Adr);
  }

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
  }

  void vBotCom_SendStart(u8 u8Len)
  {
    mu8TxByteIdx = 0;
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
    mu8TxByteIdx = 0;
  }

  void vBotCom_SendEnd()
  {
    mcUart->mcUartDataOut.put((u8)((0 << 5) + (mu8TxChksum & 0x1F)));
  }

  void vBotCom_SendData(u8 u8Data)
  {
    mu8TxChksum += u8Data;

    mcUart->vSend((u8)((1 << 5) + (u8Data & 0x1F)));

    if (u8Data > 31)
    {
      mcUart->vSend((u8)((2 << 5) + (u8Data >> 5)));
    }
    mu8TxByteIdx++;
  }

  void vSync() override
  {
    // Encode data to send

    u16 lu16DataOutCnt = 0;

    while (1)
    {
      if (mcUart->mcUartDataOut.space_left() > 8)
      {
        if (mu8PoolIdxTx == 0)
        {
          mu8PoolIdxTx = mcTxComBuf.get();

          if (mu8PoolIdxTx)
          {
            cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, mu8PoolIdxTx);
            mpcMsgDataT.From(lcMsg);
            vBotCom_SendStart((u8)mpcMsgDataT.Len());
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
      while (mu8TxByteIdx < mpcMsgDataT.Len())
      {
        vBotCom_SendData(mpcMsgDataT.mpu8Data[mu8TxByteIdx]);
        lu16DataOutCnt++;

        if ((mcUart->mcUartDataOut.space_left() < 8) ||
            (lu16DataOutCnt > mu16BytesPer1ms))
        {
          break;
        }
      }

      if (mu8TxByteIdx == mpcMsgDataT.Len())
      {
        vBotCom_SendEnd();
        cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);
		mu8PoolIdxTx = 0;
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

      if ((mu8PoolIdxRx == 0) && (lu8Cmd == 7))
      {
        cBotNetMsg_Base lcMsg; cBnMsgPool::vReqMsg(lcMsg, mu8PoolIdxRx, 64);
        mpcMsgDataR.From(lcMsg);
      }

      if (!mu8PoolIdxRx) return;

      switch (lu8Cmd)
      {
        case 0: // 000 : Übertragung Ende: Checksumme in den unteren 5 Bits
          {
            bool isValid   = True;
            bool isNoSpace = False;

            if (mpcMsgDataR.Len() != mu8RxDataCount)
            {
              isValid = False;
            }

            // Message Counter überprüfen
            if (mstMode.SendAcknowledge)
            {
              // Ist noch genügend Platz im Buffer ?
              if (mcRxComBuf.space_left() == 0)
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
                  mcRxComBuf.put(mu8PoolIdxRx);
                  cBnMsgPool::vPutMsg(mu8PoolIdxRx);
                  cBnMsgPool::vSetLen(mu8PoolIdxRx, mpcMsgDataR.Len());

                  mu8MsgCntRx_Last = mu8MsgCntRx;
                }
              }
              else
              {
                mcRxComBuf.put(mu8PoolIdxRx);
                cBnMsgPool::vPutMsg(mu8PoolIdxRx);
                cBnMsgPool::vSetLen(mu8PoolIdxRx, mpcMsgDataR.Len());
              }
            }

            cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
            mu8PoolIdxRx = 0;

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
          mpcMsgDataR.Add(lu8Data);
          break;
        case 2: // 010 : Daten Zusatz: Die oberen  3 Bits
          switch (mu8RxLastCmd)
          {
            case 1: // Daten
              mpcMsgDataR.mpu8Data[mpcMsgDataR.Len() - 1] += (u8)((lu8Data & 0x7) << 5);
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
          break;
      }
      mu8RxLastCmd = lu8Cmd;
    }
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError); /* avoid warning */
    UNUSED(lenState); /* avoid warning */
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
  }

  void vComDone() override
  {
  }

  void vAddChecksum(u8 lu8PoolIdx) override
  {
    UNUSED(lu8PoolIdx);
  }
};


#endif // __BOTNET_LINK_BOTCOM_H__
