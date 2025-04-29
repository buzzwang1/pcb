#ifndef __BOTNET_LINK_USART_MP_HD_H__
#define __BOTNET_LINK_USART_MP_HD_H__

#include "Typedef.h"

#include "cUsart_MpHd.h"
#include "cStrT.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"

#ifdef TEST_BnLinkUsartMpHd
  #include "cTimeStampArray.h"
#endif

// -------------------------------------------------------------------------------------------
class cBotNet_UpLinkUsartMpHd:public cBotNet_SyncedLinkBase
{
  public:

  typedef enum
  {
    enCnstWait50us = 50,
    enCnstWait75us = 75,
    enCnstWait100us = 100,
    enCnstWaitCom = enCnstWait75us,
    enCnstWaitEnd = enCnstWait50us,
  }tenConsts;

  cUartMpHdSlave *mcUpLink;

  u8              mu8MsgSyncRxBuf[2];
  cComDatMsg mpcMsgSyncRx;
  u8              mu8MsgSyncTxBuf[2 + 1]; // +1 wegen Id/Adressbyte;
  cComDatMsg mpcMsgSyncTx;

  u8              mu8MsgDataRxBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataRx;
  u8              mu8MsgDataTxBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataTx;

  cBotNet_SyncedLinkBase::tenStates  menSm;

  #ifdef TEST_BnLinkUsartMpHd
    cTimeStampArray mcTimeStampArray;
  #endif

  cBotNet_UpLinkUsartMpHd(u16 lu16MyAdr, u16 lu16DAdr, cUartMpHdSlave* lcUpLink)
  : cBotNet_SyncedLinkBase(lu16MyAdr, lu16DAdr),
    mpcMsgSyncRx(mu8MsgSyncRxBuf, sizeof(mu8MsgSyncRxBuf), cComDatMsg::enRx),
    mpcMsgSyncTx(mu8MsgSyncTxBuf, sizeof(mu8MsgSyncTxBuf), cComDatMsg::enTx),
    mpcMsgDataRx(mu8MsgDataRxBuf, sizeof(mu8MsgDataRxBuf), cComDatMsg::enRx),
    mpcMsgDataTx(mu8MsgDataTxBuf, sizeof(mu8MsgDataTxBuf), cComDatMsg::enTx)
  {
    mStatus.IsInit  = 1;
    vSet(lcUpLink);

    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;

    vSetOnlineTimeout(500);
  }



  void vSet(cUartMpHdSlave* lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    (void)lenError; // avoid warning
    (void)lenState; // avoid warning
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    vComPrepareRx();
  }

  // Eine Art von Watchdog. Kommt von cBotNet_SyncedLinkBase
  // Wurde seit 200ms kein vOnSync aufgerufen, wird der Link deaktiviert
  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    // Daten verwerfen
    mpcMsgDataTx.cData.muiLen = 0;
    mcTxComBuf.reset();
    mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwFull);
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
  }



  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOB->BSRR = (1 << 4);  // Test
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait) //Sync
    {
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vReset();
        cTimeStampArray::vSetTimeStamp(0);
      #endif
      vComPrepareRx();
    }
    else if (lenEvent == cComNode::tenEvent::enEvMisc)
    {
      // Misc ist der Timer Interrupt
      if ((menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStEndWaitForTimer))
      {
        vComDone();
      }
    }
  }

  void vComPrepareRx() // __attribute__((optimize("-O0")))
  {
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait)
    {
      mcUpLink->vMute();
      // Master schickt Aresse + 2 Byte Sync Nachricht (wird vom DMA erledigt)
      mcUpLink->vStartMsg(&mpcMsgSyncRx);

      // Daten holen
      mcTxComBuf.get(&mpcMsgDataTx.cData);

      // Ggf. Adresse entfernen
      if (mpcMsgDataTx.cData.Len())
      {
        //Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
        cBotNetMsg_Base lcMsg(&mpcMsgDataTx.cData);
        lcMsg.vDecode();
        if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
        {
          lcMsg.vAdressRemove();
          mpcMsgDataTx.cData.muiLen = lcMsg.muiLen;
        }
      }

      if (mpcMsgDataTx.cData.Len())
      {
        vCreateSync(mpcMsgSyncTx.cData.mpu8Data, 0, (uint8)mpcMsgDataTx.cData.Len(), 0);
      }
      else
      {
        vCreateSync(mpcMsgSyncTx.cData.mpu8Data, 0, 0, 0);
      }
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(1);
      #endif
    }
  }


  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    // GPIOB->BSRR = (1 << 5);  // Test

    do
    {
      lbLoop = False;
      //menSmActive = menSm;
      switch (menSm)
      {
        case cBotNet_SyncedLinkBase::tenStates::enStSyncWait:
          {
            u8 lu8Error = 1;

            #ifdef TEST_BnLinkUsartMpHd
              cTimeStampArray::vSetTimeStamp(5);
              //cTimeStampArray::vDtwStart();
            #endif

            if (mpcMsgSyncRx.cData.mpu8Data[0] == mMyAdr)
            {
              // Sync
              vOnSync();
              mpcMsgDataRx.cData.muiLen = mpcMsgSyncRx.cData.mpu8Data[1];

              #ifdef TEST_BnLinkUsartMpHd
                cTimeStampArray::vSetTimeStamp(6);
                //cTimeStampArray::vDtwStop();
              #endif

              // Sync Checksumme ?
              if (IsSyncCheckOk((mpcMsgSyncRx.cData.mpu8Data+1), mMyAdr))
              {
                if (mpcMsgDataRx.cData.Len())
                {
                  mpcMsgDataRx.cData.muiLen++; // +1 für Checksumme
                }
                lu8Error = 0;

                // Direkt senden, auf Master muss nicht gewartet werden
                #ifdef TEST_BnLinkUsartMpHd
                  cTimeStampArray::vSetTimeStamp(7);
                #endif
                mcUpLink->vStartMsg(&mpcMsgSyncTx);
                #ifdef TEST_BnLinkUsartMpHd
                  cTimeStampArray::vSetTimeStamp(7);
                #endif
                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDone;
              }
            }

            #ifdef TEST_BnLinkUsartMpHd
              cTimeStampArray::vSetTimeStamp(10);
            #endif

            if (lu8Error)
            {
              menSm = enStEnd;
              lbLoop = True;
            }
          }
        break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDone:
          {
            #ifdef TEST_BnLinkUsartMpHd
              cTimeStampArray::vSetTimeStamp(15);
            #endif


            if (mpcMsgDataRx.cData.Len()) // Gibt es was zu empfangen
            {
              menSm = enStDataWaitRx;
              mcUpLink->vStartMsg(&mpcMsgDataRx);
            }
            else if (mpcMsgDataTx.cData.Len()) // Gibt es nur was zu senden
            {
              // Kurz Warten, damit Master vorbereiten kann
              menSm = enStDataWaitForTimer;
              mcUpLink->mTimer.vStart(enCnstWaitCom);
            }
            else // Es werden keine Daten transferiert
            {
              menSm = enStEnd;
              lbLoop = True;
            }
          }
          break;

        // ----------------- Master Tx
        case enStDataWaitRx:
          {
            if (mpcMsgDataTx.cData.Len()) // Gibt es was zu senden
            {
              // Kurz Warten, damit Master vorbereiten kann
              menSm = enStDataWaitForTimer;
              mcUpLink->mTimer.vStart(enCnstWaitCom);
            }
            else // Es werden keine Daten transferiert
            {
              menSm = enStEnd;
              lbLoop = True;
            }

            // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht vonm direkten Master
            // Senderadresse == Masteradresse  Zieladresse == meine Adresse (ich) dazufügen
            cBotNetMsg_Base lcMsg(&mpcMsgDataRx.cData);
            lcMsg.vDecode();

            if (!lcMsg.bHasAdress())
            {
              lcMsg.vAdressAdd(mcDAdr.Get(), mcMyAdr.Get());
              mpcMsgDataRx.cData.muiLen = lcMsg.muiLen;
            }
            mcRxComBuf.put(&mpcMsgDataRx.cData);
          }
          break;


        // ----------------- Slave Tx Master
        case enStDataWaitForTimer:
          {
            mcUpLink->vStartMsg(&mpcMsgDataTx);
            menSm = enStDataDoneTx;
          }
          break;

        case enStDataDoneTx:
          {
            menSm = enStEnd;
            lbLoop = True;
          }
          break;

        case enStEnd:
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
            vComPrepareRx();
          }
        break;

        default:
          break;
      }
    } while (lbLoop);
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
    mStatus.IsStartRequested = 1;
    mcUpLink->bStartNext();
  }
};


#endif // __BOTNET_LINK_USART_MP_HD_H__
