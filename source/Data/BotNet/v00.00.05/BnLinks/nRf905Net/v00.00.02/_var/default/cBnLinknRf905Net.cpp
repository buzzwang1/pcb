#include "cBnLinknRf905Net.h"
#include "cBotnet.h"

cBotNet_ComLinknRf905Net::cBotNet_ComLinknRf905Net(cBotNet_ComLinknRf905* lcLinkNRF905, cBotNet_LinkBase::tenType lenType, cBotNet* lcBotNet)
  : cBotNet_SyncedLinkBase(0/*lu32RxComBufSize*/, 0/*lu32TxComBufSize*/, lenType)
{
  mu16SessionDAdr  = 0;
  mu16SessionBnAdr = 0;
  mu16SessionKanal = 0;
  mbSessionStart   = False;
  mbSessionStop    = False;

  mcLinkNRF905 = lcLinkNRF905;
  mcNRF905     = mcLinkNRF905->mcNRF905;
  mcBotnet     = lcBotNet;
}


bool cBotNet_UpLinknRf905Net::bEventHandler(cNRF905::tenCmdEvents lenEvent)
{
  // im Remote Modus weiterleiten zum Uplink
  if (mSmMain == cBotNet_ComLinknRf905Net::StRemoteUp)
  {
    // Setzt dem Timer zurück
    vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
    return mcUpLinknRf905->bEventHandler(lenEvent);
  }
  else
  {
    do
    {
      if (lenEvent == cNRF905::NRF905_EvLoop)
      {
        lenEvent = cNRF905::NRF905_EvDummy;
      }

      if (!mcNRF905->bEventHandler(lenEvent))
      {
        switch(mSmRadio)
        {
          case StAlive_UpSendPingCfgStart:
            mSmRadio = StAlive_UpSendPingCfgStart_Done;

            vResetStatusFlags();
            mcUpLinknRf905->vResetCom(True);

            mcNRF905->mstNRF905.mstConfig.RxAdress = 0x00000000;
            mcNRF905->mstNRF905.mui32TxAdress      = (u32)(MdNonSessionAdr << 16);

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

            mcNRF905->SetChannel_433(108, False);
            mcNRF905->SetCrc(16);

            mcNRF905->bStartUpdateCfg();
            break;

          case StAlive_UpSendPingCfgStart_Done:
            {
              mSmRadio = StAlive_UpSendPingTxStart;

              if (mu8SendStatus)
              {
                mSmRadio = StAlive_UpSendStatusTxStart;
              }

              u32 u32Delay = mcNRF905->u32Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_TX);
              if (u32Delay)
              {
                mcNRF905->mTimer.vStart(u32Delay);
              }
              else
              {
                lenEvent = cNRF905::NRF905_EvLoop;
              }
            }
            break;

          case StAlive_UpSendStatusTxStart:
            {
              mSmRadio = StAlive_UpSendStatusTxStart_Done;
              mcNRF905->mTimer.vStop();

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;
              //Bn-Msg-Header
              *lu8Tx++ = 0x11;
              //Bn-Msg
              *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress

              *lu8Tx++ = mu8SendStatusIdx; // SubId
              cMemTools::vMemCpy(lu8Tx, mu8SendStatusData, 16);
              lu8Tx += 16;

              //Bn-Frame
              *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
              *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
              *lu8Tx++ = 0x80;

              mcNRF905->vStartTransmit();
            }
            break;

          case StAlive_UpSendStatusTxStart_Done:
            mSmRadio = StAlive_UpSendPingTxStart;
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_25ms);
            break;


          case StAlive_UpSendPingTxStart:
            {
              mSmRadio = StAlive_UpSendPingTxStart_Done;
              mcNRF905->mTimer.vStop();

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;
              //Bn-Msg-Header
              *lu8Tx++ = 0x11;
              //Bn-Msg
              *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress
              *lu8Tx++ = 0x00; // SubId
              *lu8Tx++ = (123 >> 8); *lu8Tx++ = (123 & 0xFF); //Channel
              cMemTools::vMemCpy(lu8Tx, mu8SendStatusPing, 14);
              lu8Tx += 14;

              //Bn-Frame
              *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
              *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
              *lu8Tx++ = 0x80;

              mcNRF905->vStartTransmit();
            }
            break;

          case StAlive_UpSendPingTxStart_Done:
            {
              mSmRadio = StAlive_UpWaitPing;

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              mcNRF905->mstNRF905.mstConfig.RxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);

              mcNRF905->SetChannel_433(123, False);
              mcNRF905->SetCrc(0);

              mcNRF905->bStartUpdateCfg();
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            }
            break;

          case StAlive_UpWaitPing:
            mSmRadio = StAlive_UpWaitSessionStart;
            mcNRF905->mTimer.vStop();
            mcNRF905->vStartReceive();
            break;

          case StAlive_UpWaitSessionStart:
            if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 25) &&
                (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 0x11) &&
                (mcNRF905->mstNRF905.mui8RxPayLoad[3] == 0x01))
            {
              vOnSync();

              mSmRadio = StAlive_UpWaitSessionSync_DoneAck;
              lenEvent = cNRF905::NRF905_EvLoop;

              u16 lu16BnAdr = (mcNRF905->mstNRF905.mui8RxPayLoad[22] << 8) +
                               mcNRF905->mstNRF905.mui8RxPayLoad[23];
              u16 lu16BnDId = (mcNRF905->mstNRF905.mui8RxPayLoad[1] << 8) +
                               mcNRF905->mstNRF905.mui8RxPayLoad[2];

              mcNRF905->mstNRF905.mui32TxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);
              mcNRF905->SetCrc(0);
              mcNRF905->bStartUpdateCfg();
            }
            break;

          case StAlive_UpWaitSessionSync_DoneAck:
            {
              mSmRadio = StRemoteUp;
              mSmMain  = StRemoteUp;
              mcUpLinknRf905->vSlaveStartSync();
            }
            break;

          case StAlive_UpWaitSessionSync_DoneNack:
            mSmRadio = StAlive_UpWait;

            if (mu8KeepReceiverOnWhileWaiting)
            {
              mSmRadio = StAlive_UpWaitSessionStatusCfgStart;
            }

            lenEvent = cNRF905::NRF905_EvLoop;
            break;

          case StRemoteUp:
            break;

          case StAlive_UpWaitSessionStatusCfgStart:
            {
              mSmRadio = StAlive_UpWaitSessionStatusCfgDone;

              mcNRF905->mstNRF905.mstConfig.RxAdress = (u32)(MdNonSessionAdr << 16);

              mcNRF905->SetChannel_433(108, False);
              mcNRF905->SetCrc(16);

              mcNRF905->bStartUpdateCfg();
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            }
            break;
          case StAlive_UpWaitSessionStatusCfgDone:
            mSmRadio = StAlive_UpWaitStatusStart;
            mcNRF905->mTimer.vStop();
            lenEvent = cNRF905::NRF905_EvLoop;
            break;
          case StAlive_UpWaitStatusStart:
            mSmRadio = StAlive_UpWaitStatusDone;
            mcNRF905->vStartReceive();
            break;
          case StAlive_UpWaitStatusDone:
            cMemTools::vMemCpy(mcMsgRx.mpu8Data, mcNRF905->mstNRF905.mui8RxPayLoad, 25);
            mcMsgRx.muiLen = 25;
            mcMsgRx.vDecode(0, 0);
            mcBotnet->mcpMsgProcess->vMsg(mcMsgRx, mcBotnet);
            break;

          case StAlive_UpWait:
            mcNRF905->u32Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_PWR_DOWN);
            break;

          default:
            break;
        }
      }
    } while (lenEvent == cNRF905::NRF905_EvLoop);

    return False;
  }
}

bool cBotNet_DownLinknRf905Net::bEventHandler(cNRF905::tenCmdEvents lenEvent)
{
  // im Remote Modus weiterleiten zum Uplink
  if (mSmMain == cBotNet_ComLinknRf905Net::StRemoteDown)
  {
    return mcDownLinknRf905->bEventHandler(lenEvent);
  }
  else
  {
    do
    {
      if (lenEvent == cNRF905::NRF905_EvLoop)
      {
        lenEvent = cNRF905::NRF905_EvDummy;
      }

      if (!mcNRF905->bEventHandler(lenEvent))
      {
        switch(mSmRadio)
        {
          case StAlive_DownWaitPingCfgStart:
            mSmRadio = StAlive_DownWaitPingCfgStart_Done;

            mcDownLinknRf905->vResetCom(True);

            mcNRF905->mstNRF905.mstConfig.RxAdress = (u32)(MdNonSessionAdr << 16);
            mcNRF905->mstNRF905.mui32TxAdress      = 0x00000000;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

            mcNRF905->SetChannel_433(108, False);
            mcNRF905->SetCrc(16);

            mcNRF905->bStartUpdateCfg();
            break;

          case StAlive_DownWaitPingCfgStart_Done:
            mcNRF905->mTimer.vStop();
            mSmRadio = StAlive_DownWaitPingRx;
            mcNRF905->vStartReceive();
            break;

          case StAlive_DownWaitPingRx:
            mSmRadio = StAlive_DownWaitPingCfgStart_Done;

            if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 25)
            {
              cMemTools::vMemCpy(mpcMsgData.cRxData.mpu8Data, mcNRF905->mstNRF905.mui8RxPayLoad, 25);
              mpcMsgData.cRxData.muiLen = 25;
              mcDownLinknRf905->mcRxComBuf.put(&mpcMsgData.cRxData);
            }

            if (mbSessionStart)
            {
              // Überprüfen,obes eine Session Start Nachricht ist und zu meiner Session passt
              // Info:  11 | DH DL | 00 | KH.KL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
              // Start: 11 | DH DL | 01 | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
              if (mpcMsgData.cRxData.muiLen == 25)
              {
                // Info Nachricht ?
                if ((mpcMsgData.cRxData[24] == 0x80) &&
                    (mpcMsgData.cRxData[0]  == 0x11))
                {
                  //Start
                  if (mpcMsgData.cRxData[3] == 0x00)
                  {
                    u16 lu16BnAdr     = (u16)(((u16)mpcMsgData.cRxData[22] << 8) + (u16)mpcMsgData.cRxData[23]);
                    u16 lu16DeviceAdr = (u16)(((u16)mpcMsgData.cRxData[1]  << 8) + (u16)mpcMsgData.cRxData[2]);

                    if (lu16DeviceAdr == mu16SessionDAdr)
                    {
                      cBotNetAdress lcReceivedAdress(lu16BnAdr);
                      cBotNetAdress lcTargetAdress(mu16SessionBnAdr);

                      if ((lcReceivedAdress == lcTargetAdress) ||
                          (lcReceivedAdress.isMasterOf(lcTargetAdress)))
                      {
                        mu16SessionBnAdr = lu16BnAdr;
                        mu16SessionKanal = (u16)(((u16)mpcMsgData.cRxData[4] << 8) + (u16)mpcMsgData.cRxData[5]);
                        mbSessionStart   = False;
                        mSmRadio = StAlive_DownSendSessionStart_Init;
                      }
                    }
                  }
                }
              }
            }

            lenEvent = cNRF905::NRF905_EvLoop;
            break;

          case StAlive_DownSendSessionStart_Init:
            {
              mSmRadio = StAlive_DownSendSessionStart_Init2;

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              mcNRF905->mstNRF905.mstConfig.RxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);
              mcNRF905->mstNRF905.mui32TxAdress      = (mu16SessionDAdr << 16) + mu16SessionBnAdr;

              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

              mcNRF905->SetChannel_433(mu16SessionKanal, False);
              mcNRF905->SetCrc(0);

              mcNRF905->bStartUpdateCfg();
              //Timeout setzen
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            }
            break;

          case StAlive_DownSendSessionStart_Init2:
            mSmRadio = StAlive_DownSendSessionStart_Start;
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_1ms * 5);
            break;

          case StAlive_DownSendSessionStart_Start:
            {
              mSmRadio = StAlive_DownSendSessionStart_Wait;

              mcNRF905->mTimer.vStop();

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;

              //Bn-Msg-Header
              *lu8Tx++ = 0x11;

              //Bn-Msg
              *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress
              *lu8Tx++ = 0x01; // Start
              cMemTools::vMemSet(lu8Tx, 0, 16);
              lu8Tx += 16;

              //Bn-Frame
              *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
              *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
              *lu8Tx++ = 0x80;

              mcNRF905->vStartTransmit();
            }
            break;

          case StAlive_DownSendSessionStart_Wait:
            mSmRadio = StAlive_DownSendSessionStart_Done;
            //Kurz Warten, damit Slave umschalten kann
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_1ms * 5);
            break;

          case StAlive_DownSendSessionStart_Done:
            vOnSync();

            mSmRadio = StRemoteDown;
            mSmMain  = StRemoteDown;
            lenEvent = cNRF905::NRF905_EvLoop;
            break;

          case StRemoteDown:
            break;

          default:
            break;
        }
      }
    } while (lenEvent == cNRF905::NRF905_EvLoop);

    return False;
  }
}
