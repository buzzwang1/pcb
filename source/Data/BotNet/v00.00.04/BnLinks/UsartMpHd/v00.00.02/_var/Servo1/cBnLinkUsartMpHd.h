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
  u8 mDAdr;

  typedef enum
  {
    enCnstWait50us = 50,
    enCnstWait75us = 75,
    enCnstWait100us = 100,
    enCnstWaitCom = enCnstWait75us,
    enCnstWaitEnd = enCnstWait50us,
  }tenConsts;

  cUartMpHdSlave *mcUpLink;

  cComDatMsg mpcMsgSyncRx;
  cComDatMsg mpcMsgSyncTx;

  cComDatMsg mpcMsgDataRx;
  cComDatMsg mpcMsgDataTx;

  cComDatMsg mpcMsgAckRx;
  cComDatMsg mpcMsgAckTx;

  cBotNet_SyncedLinkBase::tenStates  menSm;
  //cBotNet_SyncedLinkBase::tenStates  menSmActive; //Keine Funktion, Debugging Hilfe.

  #ifdef TEST_BnLinkUsartMpHd
    cTimeStampArray mcTimeStampArray;
  #endif

  cBotNet_UpLinkUsartMpHd(uint16 lu16DestAdr, cUartMpHdSlave* lcUpLink)
  : cBotNet_SyncedLinkBase(lu16DestAdr, cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize)
  {
    mDAdr = mAdr = mcDAdr.GetIdx();
    mStatus.IsInit         = 1;

    vSet(lcUpLink);

    mpcMsgSyncTx.vMemAlloc(2,  0);
    mpcMsgSyncRx.vMemAlloc(0,  2 + 1); // +1 wegen Id/Adressbyte

    mpcMsgDataTx.vMemAlloc(enCnstMaxDataPlusCheckSum,  0);
    mpcMsgDataRx.vMemAlloc(0,  enCnstMaxDataPlusCheckSum);

    mpcMsgAckTx.vMemAlloc(1,  0);
    mpcMsgAckRx.vMemAlloc(0,  1);

    mcSAdr.Set(0);
    mcDAdr.Set(lu16DestAdr);

    mDAdr = lu16DestAdr;
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;

    vSetOnlineTimeout(500);
  }

  void vAddedToBn(cBotNet* lcBotNet) override;

  //~cBotNet_I2c_MaSl_Slave_Slave()
  //{
  //}


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
    cBotNet_SyncedLinkBase::vOnEnterOffline();
    mStatus.IsFirstTx  = 0;
  }

  // Kommt von cBotNet_SyncedLinkBase
  // Nach 10s offline wirddas hier aufgerufen
  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    mStatus.IsFirstTx  = 1;
    // Daten verwerfen
    mpcMsgDataTx.cTxData.muiLen = 0;
    mcTxComBuf.reset();
    cBotNet_LinkBase::vResetCom(True);
    mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwFull);
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
  }


  // Wird in der Regel 1 mal pro Sekunde vom Hauptprogramm aufgerufen
  void vResetCom(bool bAlways) override
  {
    static u8 lu8t = 0;

    // Falls Links offline
    //  - Setzt die Kommunikation zurück
    //  - Löscht die Buffer
    cBotNet_LinkBase::vResetCom(bAlways);

    if (!IsOnline())
    {
      if (lu8t > 9)
      {
        mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwDma);
        lu8t = 0;
      }
      else
      {
        lu8t++;
      }
    }
    else
    {
      lu8t = 10;
    }
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
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStEndWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStAckWaitForTimer))
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

      mStatus.IsLastTxOk = mStatus.IsThisTxOk;
      if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx) || (mpcMsgDataTx.cTxData.muiLen == 0))
      {
        // Daten holen
        mcTxComBuf.get(&mpcMsgDataTx.cTxData);
        if (mpcMsgDataTx.cTxData.muiLen)
        {
          mStatus.IsFirstTx  = 0;
          mStatus.IsThisTxOk = 0;
          mu8MsgCntTx++; // Nachrichtenzähler
        }

        // Ggf. Adresse entfernen
        if (mpcMsgDataTx.cTxData.Len())
        {
          //Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
          cBotNetMsg_Base lcMsg(&mpcMsgDataTx.cTxData);
          lcMsg.vDecode();
          if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
          {
            lcMsg.vAdressRemove();
            mpcMsgDataTx.cTxData.muiLen = lcMsg.muiLen;
          }
        }

        // Ggf. Checksumme berechnen und anfügen
        if (mpcMsgDataTx.cTxData.Len())
        {
          mpcMsgDataTx.cTxData.Add(mpcMsgDataTx.u8TxChecksum());
        }
      }


      // Schauen, ob noch Platz ist, ansonsten Bussy setzen
      if (mcRxComBuf.space_left() < (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
      {
        mBussy = 1;
      }
      else
      {
        mBussy = 0;
      }

      if (mpcMsgDataTx.cTxData.Len())
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, mBussy, (uint8)mpcMsgDataTx.cTxData.Len() - 1); // -1 Wegen checksumme
      }
      else
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, mBussy, 0);
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

            if (mpcMsgSyncRx.cRxData.mpu8Data[0] == mAdr)
            {
              // Sync
              vOnSync();
              mpcMsgDataRx.cRxData.muiLen = mpcMsgSyncRx.cRxData.mpu8Data[1];

              #ifdef TEST_BnLinkUsartMpHd
                cTimeStampArray::vSetTimeStamp(6);
                //cTimeStampArray::vDtwStop();
              #endif

              // Sync Checksumme ?
              if (IsSyncCheckOk((mpcMsgSyncRx.cRxData.mpu8Data+1), mAdr))
              {
                if (mpcMsgDataRx.cRxData.Len())
                {
                  mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
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
            u8 lu8Bussy = mBussy;
            if (mpcMsgSyncRx.cRxData.mpu8Data[2] & 128) lu8Bussy+=2;

            #ifdef TEST_BnLinkUsartMpHd
              cTimeStampArray::vSetTimeStamp(15);
            #endif


            switch (lu8Bussy)
            {
              case 0:  // Master und Slave haben genug Platz
                {
                  if (mpcMsgDataRx.cRxData.Len()) // Gibt es was zu empfangen
                  {
                    menSm = enStDataWait;
                    mcUpLink->vStartMsg(&mpcMsgDataRx);
                  }
                  else if (mpcMsgDataTx.cTxData.Len()) // Gibt es nur was zu senden
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
              case 1:  // Nur Master hat genug Platz
                {
                  if (mpcMsgDataTx.cTxData.Len()) // Gibt es nur was zu senden
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
              case 2: //  Nur Slave hat genug Platz
                {
                  if (mpcMsgDataRx.cRxData.Len()) // Gibt es was zu empfangen
                  {
                    menSm = enStDataWait;
                    mcUpLink->vStartMsg(&mpcMsgDataRx);
                  }
                  else // Es werden keine Daten transferiert
                  {
                    menSm = enStEnd;
                    lbLoop = True;
                  }
                }
                break;

              case 3: // Keiner hat genug Platz
                {
                  menSm = enStEnd;
                  lbLoop = True;
                }
                break;
            }

            #ifdef TEST_BnLinkUsartMpHd
              cTimeStampArray::vSetTimeStamp(20);
            #endif

          }
          break;

        // ----------------- Master Tx Slave Ack

        case enStDataWait:
          {
            // Direkt senden, auf Master muss nicht gewartet werden
            mcUpLink->vStartMsg(&mpcMsgAckTx);
            menSm = enStAckDone;

            // Checksumme der empfangenen Daten überprüfen
            // Falls ok, dann wird die Ack Nachricht geACK't
            // ansonsten geNACK't
            {
              uint8 lui8ChkSum;

              mpcMsgDataRx.cRxData.muiLen--; //Checksumme abschneiden
              lui8ChkSum = mpcMsgDataRx.u8RxChecksum();

              if (mpcMsgDataRx.cRxData.mpu8Data[mpcMsgDataRx.cRxData.Len()] == lui8ChkSum)
              {
                // Slave Ack
                mpcMsgAckTx.cTxData.mpu8Data[0] = 0xA; //Acknowledge
              }
              else
              {
                // Slave Nack
                mpcMsgAckTx.cTxData.mpu8Data[0] = 0xF; // Fail
              }
            }

            // Wenn Daten empfangen wurden, dann
            // Rx Daten abholen und in den Rx-Buffer schreiben
            //
            if (IsSyncNewData())
            {
              // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht vonm direkten Master
              // Senderadresse == Masteradresse  Zieladresse == meine Adresse (ich) dazufügen
              cBotNetMsg_Base lcMsg(&mpcMsgDataRx.cRxData);
              lcMsg.vDecode();

              if (!lcMsg.bHasAdress())
              {
                lcMsg.vAdressAdd(mcDAdr.Get(), mcSAdr.Get());
                mpcMsgDataRx.cRxData.muiLen = lcMsg.muiLen;
              }

              mcRxComBuf.put(&mpcMsgDataRx.cRxData);
            }
          }
          break;

        case enStAckDone:
          {
            u8 lu8Bussy = (mpcMsgSyncRx.cRxData.mpu8Data[2] & 128);

            if (!lu8Bussy)
            {
              if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
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
            else
            {
              menSm = enStEnd;
              lbLoop = True;
            }
          }
          break;

        // ----------------- Slave Tx Master Ack

        case enStDataWaitForTimer:
          {
            mcUpLink->vStartMsg(&mpcMsgDataTx);
            menSm = enStDataDone;
          }
          break;

        case enStDataDone:
          {
            menSm = enStAckWait;
            mcUpLink->vStartMsg(&mpcMsgAckRx);
          }
          break;

        case enStAckWait:
          {
            menSm = enStEnd;
            lbLoop = True;

            if (mpcMsgAckRx.cRxData.mpu8Data[0] == 0xA)
            {
              mStatus.IsThisTxOk = 1;
            }
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
