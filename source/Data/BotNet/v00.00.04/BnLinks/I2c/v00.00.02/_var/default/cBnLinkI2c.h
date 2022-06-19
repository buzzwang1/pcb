#ifndef __BOTNET_LINK_I2C_H__
#define __BOTNET_LINK_I2C_H__

#include "Typedef.h"

#include "cI2C.h"
#include "cStrT.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"


class cBotNet_ComLinkI2c: public cBotNet_SyncedLinkBase
{
public:
  uint8 mAdrSync;
  uint8 mAdrData;
  uint8 mAdrAck;

  cBotNet_ComLinkI2c(uint16 lui16Adr, u32 lu32RxComBufSize, u32 lu32TxComBufSize)
    : cBotNet_SyncedLinkBase(lui16Adr, lu32RxComBufSize, lu32TxComBufSize)
  {
    uint8 lui8Adr;

    lui8Adr = mcDAdr.GetIdx();
    lui8Adr <<= 1; // für I2C

    mAdr = lui8Adr;
    mAdrSync = lui8Adr;
    mAdrData = lui8Adr + (0x20 << 1);
    mAdrAck = lui8Adr + (0x40 << 1);
  }
};


class cBotNet_DownLinkI2c: public cBotNet_ComLinkI2c
{
  public:
  cI2cMaster*  mcDownLink;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgData;
  cComDatMsg mpcMsgAck;

  u16        mu16TimeoutCounter;

  u8 mu8Sync;

  cBotNet_SyncedLinkBase::tenStates   menSm;
  cBotNet_SyncedLinkBase::tenStates   menSmLast;

  cBotNet_DownLinkI2c(uint16 lui16SlaveAdr, cI2cMaster* lcDownLink)
    : cBotNet_ComLinkI2c(lui16SlaveAdr, cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize)
  {
    uint8 lui8Adr;

    mcDAdr.Set(lui16SlaveAdr);
    mcSAdr.Set(mcDAdr.GetMasterAdr());

    lui8Adr = mcDAdr.GetIdx();
    lui8Adr <<= 1; // für I2C

    mcDownLink = lcDownLink;

    mAdr = lui8Adr;
    mAdrSync = lui8Adr;
    mAdrData = lui8Adr + (0x20 << 1);
    mAdrAck  = lui8Adr + (0x40 << 1);

    //Flka: +1 für Checksumme scheint zu fehlen bei MsgData ?
    mpcMsgSync.vMemAlloc(2,  2);
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);
    mpcMsgAck.vMemAlloc(0,  1);

    menSm         = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    menSmLast     = cBotNet_SyncedLinkBase::tenStates::enStIdle;

    mStatus.IsInit         = 1;
    mu16TimeoutCounter     = 0;

    mcDownLink->vAddSlave((cComNode*)this);
  }

  //~cBotNet_DownLinkI2c()
  //{
  //}

  virtual uint32 SizeOf()
  {
    uint32 lui32Size;

    lui32Size  = sizeof(cBotNet_DownLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();

    lui32Size -= 3 * sizeof(cComDatMsg);

    return lui32Size;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);

    // Wird auch nach 100ms von cBotNet_SyncedLinkBase.vTick10ms offline gesetzt
    if ((menSmLast == cBotNet_SyncedLinkBase::tenStates::enStSync) && (mStatus.IsLastTxOk == 0))
    {
      mStatus.IsOnline = 0;
    }

    vResetStatusComFlags();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;

    if (mcDownLink->mSm == cComNode::tenState::enStWait)
    {
      mcDownLink->mSm = cComNode::tenState::enStIdle;
    }
  }

  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    cBotNet_SyncedLinkBase::vOnEnterOffline();
    mStatus.IsFirstTx  = 0;
  }

  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    mStatus.IsFirstTx  = 1;
    // Daten verwerfen
    mpcMsgData.cTxData.muiLen = 0;
    mcTxComBuf.reset();
  }

  void vResetCom(bool bAlways) override
  {
    cBotNet_LinkBase::vResetCom(bAlways);
    if ((!IsOnline()) || (bAlways))
    {
      menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    }
  }

  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOA->BSRR = 1;
    if (lenEvent == cComNode::tenEvent::enEvPrepareToSendData)
    {
      if (menSm == cBotNet_SyncedLinkBase::tenStates::enStIdle)
      {
        if (mu8Sync)
        {
          mu8Sync = 0;
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSync;
          vComDone();
        }
      }
      mStatus.IsStartRequested = mu8Sync;
    }
    else if (lenEvent == cComNode::tenEvent::enEvTimer)
    {
      // Misc ist der Timer Interrupt
      if (menSm == cBotNet_SyncedLinkBase::tenStates::enStAck)
      {
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
      }
      vComDone();
    }
    //GPIOA->BRR = 1;
  }

  void vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    //GPIOA->BSRR = (1 << 5); // Test

    menSmLast = menSm;
    do
    {
      lbLoop     = False;
      switch (menSm)
      {
        case cBotNet_SyncedLinkBase::tenStates::enStIdle:
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSync:
          {
            bool lbPrepareChecksum = False;
            // Schauen, ob noch Platz ist
            if (mcRxComBuf.space_left() > (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
            {
              mBussy = 0;
            }
            else
            {
              mBussy = 1;
            }

            // Schauen, ob noch Platz ist, ansonsten Sync überspringen
            menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;

            // Falls vorherige Daten übertragen werden konnten
            // dann nächste Daten laden
            // sonst vorherige Daten beibehalten
            mStatus.IsLastTxOk = mStatus.IsThisTxOk;
            mAdr = mAdrSync;

            if ((mpcMsgData.cTxData.Len() == 0) || (mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
            {
              // Überprüfen, obes Daten zu senden gibt
              mcTxComBuf.get(&mpcMsgData.cTxData);
              if (mpcMsgData.cTxData.Len())
              {
                mStatus.IsThisTxOk = 0;
                mStatus.IsFirstTx  = 0;
                mu8MsgCntTx++;
              }

              // Ggf. Adresse entfernen
              if (mpcMsgData.cTxData.Len())
              {
                //Überprüfen, ob der Empfänger direkter Slave ist, wenn ja, dann kann Adressinformation entfernt werden
                //Dauert ca. 30us @16MHz
                cBotNetMsg_Base lcMsg(&mpcMsgData.cTxData);
                lcMsg.vDecode();
                if (lcMsg.mcFrame.mcSAdr.isDirectMasterOf(lcMsg.mcFrame.mcDAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgData.cTxData.muiLen = lcMsg.muiLen;
                }
              }

              vCreateSync(mpcMsgSync.cTxData.mpu8Data, mBussy, (uint8)mpcMsgData.cTxData.Len());

              lbPrepareChecksum = True;
            }
            menSm = cBotNet_SyncedLinkBase::tenStates::enStData;
            mpcMsgSync.vStart();
            mcDownLink->SetLock();
            mcDownLink->vStartMsg(&mpcMsgSync);

            // Checksumme erst hier berechnen, solange SyncAdresse schon auf dem Bus gesendet wird
            if (lbPrepareChecksum)
            {
              // Ggf. Checksumme berechnen und anfügen
              // Dauert ca. 30us bei 60Byte @16MHz
              if (mpcMsgData.cTxData.Len())
              {
                mpcMsgData.cTxData.Add(mpcMsgData.u8TxChecksum());
              }
            }
            mpcMsgAck.cRxData[0] = 0x0F;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStData:
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            //Sync Checksumme ?
            if (IsSyncCheckOk(mpcMsgSync.cRxData.mpu8Data))
            {
              u8 lu8Bussy;
              vOnSync();

              // Kein Bussy Flag beim Slave gesetzt ?
              lu8Bussy = mBussy;
              if (mpcMsgSync.cRxData[1] & 128) lu8Bussy+=2;

              switch (lu8Bussy)
              {
                case 0:  // Master und Slave haben genug Platz
                  {
                    mpcMsgData.cRxData.muiLen = mpcMsgSync.cRxData[0];

                    if ((mpcMsgData.cRxData.Len()) || (mpcMsgData.cTxData.Len()))
                    {
                      mAdr = mAdrData;

                      // Gibt es Daten zu empfangen
                      if (mpcMsgData.cRxData.Len())
                      {
                        mpcMsgData.cRxData.muiLen++;
                      }

                      // Weiter mit Ack-Nachricht, Slave kann Ack-Nachricht Nack'en, um zu zeigen
                      // das Daten nicht richtig angekommen sind
                      menSm = cBotNet_SyncedLinkBase::tenStates::enStAck; // Empfange Daten Ack'en
                      mpcMsgData.vStart();
                      mcDownLink->vStartMsg(&mpcMsgData);
                      mu16TimeoutCounter = 4; // Timeout
                    }
                    else
                    {
                      // Nichts zum Austauschen, dann in idle
                      mStatus.IsThisTxOk = 1;
                      lbLoop = True;
                    }
                  }
                  break;
                case 1:  // Nur Slave hat genug Platz
                  {
                    mpcMsgData.cRxData.muiLen = 0;

                    if (mpcMsgData.cTxData.Len())
                    {
                      mAdr = mAdrData;

                      // Weiter mit Ack-Nachricht, Slave kann Ack-Nachricht Nack'en, um zu zeigen
                      // das Daten nicht richtig angekommen sind
                      menSm = cBotNet_SyncedLinkBase::tenStates::enStAck; // Empfange Daten Ack'en
                      mpcMsgData.vStart();
                      mcDownLink->vStartMsg(&mpcMsgData);
                      mu16TimeoutCounter = 4; // Timeout
                    }
                    else
                    {
                      // Nichts zum Austauschen, dann in idle
                      mStatus.IsThisTxOk = 1;
                      lbLoop = True;
                    }
                  }
                  break;
                case 2: // Nur Master hat genug Platz
                  {
                    u16 lu16LenSave;

                    lu16LenSave = mpcMsgData.cTxData.muiLen;
                    mpcMsgData.cRxData.muiLen = mpcMsgSync.cRxData[0];

                    if (mpcMsgData.cRxData.Len())
                    {
                      mAdr = mAdrData;

                      // Gibt es Daten zu empfangen
                      if (mpcMsgData.cRxData.Len())
                      {
                        mpcMsgData.cRxData.muiLen++;
                      }

                      // Weiter mit Ack-Nachricht, Slave kann Ack-Nachricht Nack'en, um zu zeigen
                      // das Daten nicht richtig angekommen sind
                      menSm = cBotNet_SyncedLinkBase::tenStates::enStAck; // Empfange Daten Ack'en
                      mpcMsgData.cTxData.muiLen = 0;
                      mpcMsgData.vStart();
                      mcDownLink->vStartMsg(&mpcMsgData);
                      mpcMsgData.cTxData.muiLen = lu16LenSave;
                      mu16TimeoutCounter = 4; // Timeout
                    }
                    else
                    {
                      // Nichts zum Austauschen, dann in idle
                      mStatus.IsThisTxOk = 1;
                      lbLoop = True;
                    }
                  }
                  break;
                case 3: // Keiner hat genug Platz
                  {
                    // Nichts zum Austauschen, dann in idle
                    mStatus.IsThisTxOk = 1;
                    lbLoop = True;
                  }
                  break;
              }
            }
            else
            {
              // Nichts zum Austauschen, dann in idle
              mStatus.IsThisTxOk = 1;
              lbLoop = True;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStAck:
          {
            bool lbSendAck;

            mAdr = mAdrAck;
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;

            mcDownLink->mSm = cComNode::tenState::enStIdle; // Nur um sicher zu sein

            lbSendAck = True;

            // Falls Daten empfangen wurden, Checksumme überprüfen
            if (mpcMsgData.cRxData.Len())
            {
              uint8 lui8ChkSum;

              mpcMsgData.cRxData.muiLen--;
              lui8ChkSum = mpcMsgData.u8RxChecksum();

              if (mpcMsgData.cRxData.mpu8Data[mpcMsgData.cRxData.Len()] != lui8ChkSum)
              {
                // Dann kein Ack-schicken
                lbSendAck = False;
              }
            }

            if (lbSendAck)
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
              mpcMsgAck.vStart();
              mcDownLink->vStartMsg(&mpcMsgAck);
              mpcMsgAck.cRxData[0] = 0xFF;
              mu16TimeoutCounter = 4; // Timeout
            }
            else
            {
              lbLoop = True;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStEnd:
          menSm       = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mcDownLink->ResetLock();

          if (mpcMsgAck.cRxData[0] == 0xA)
          {
            // Falls der Slave die Daten empfangen konnte, dann quittieren
            // ansonsten nochmals senden
            if (!(mpcMsgSync.cRxData[1] & 128) )
            {
              mStatus.IsThisTxOk = 1;
            }

            if (mpcMsgData.cRxData.Len())
            {
              /* Rx Daten abholen und in den Rx-Buffer schreiben */
              if (IsSyncNewData())
              {
                // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von einem Sklaven geschickt
                // Senderadresse == Sklavenadresse Zieladresse == meine Adresse (ich) dazufügen
                cBotNetMsg_Base lcMsg(&mpcMsgData.cRxData);
                lcMsg.vDecode();

                if (!lcMsg.bHasAdress())
                {
                  lcMsg.vAdressAdd(mcDAdr.Get(), mcSAdr.Get());
                  mpcMsgData.cRxData.muiLen = lcMsg.muiLen;
                }

                mcRxComBuf.put(&mpcMsgData.cRxData);
              }
            }
          }
          break;

        default:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mcDownLink->ResetLock();
          break;
      }
    } while (lbLoop);
    //GPIOA->BRR = (1 << 5); // Test
  }

  void vTick1ms() override
  {
    mcDownLink->vTick1ms();
    mcDownLink->bStartNext();
    if (mu16TimeoutCounter)
    {
      mu16TimeoutCounter--;
      if (!mu16TimeoutCounter)
      {
        vComStart(cComNode::tenEvent::enEvTimer);
      }
    }
  }

  void vSync() override
  {
    mu8Sync           = 1;
    mStatus.IsStartRequested = 1;
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkI2c:public cBotNet_ComLinkI2c
{
  public:
  cI2cSlave *mcUpLink;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgData;
  cComDatMsg mpcMsgAck;

  bool mbPrepareChecksum;

  cBotNet_UpLinkI2c(uint16 lu16DestAdr, cI2cSlave* lcUpLink)
  : cBotNet_ComLinkI2c(lu16DestAdr, cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize)
  {
    mStatus.IsInit         = 1;

    vSet(lcUpLink);

    mpcMsgSync.vMemAlloc(2,  2);
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);
    mpcMsgAck.vMemAlloc(1,  0);

    mcUpLink->vSetMsg(&mpcMsgSync);

    mcSAdr.Set(0);
    mcDAdr.Set(lu16DestAdr);

    mAdrSync = 0;
    mAdrData = 0;
    mAdrAck  = 0;
  }

  void vAddedToBn(cBotNet* lcBotNet, cBotNet_LinkBase::tenType lenType) override;

  //~cBotNet_I2c_MaSl_Slave_Slave()
  //{
  //}


  virtual uint32 SizeOf()
  {
    uint32 lui32Size;

    lui32Size = sizeof(cBotNet_UpLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();
    lui32Size -= 3 * sizeof(cComDatMsg);

    return lui32Size;
  }

  void vSet(cI2cSlave *lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);
    vResetStatusComFlags();
  }

  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    cBotNet_SyncedLinkBase::vOnEnterOffline();
    mStatus.IsFirstTx  = 0;
  }

  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    mStatus.IsFirstTx  = 1;
    // Daten verwerfen
    mpcMsgData.cTxData.muiLen = 0;
    mcTxComBuf.reset();
  }

  void vResetCom(bool bAlways) override
  {
    static u8 lu8t = 0;

    cBotNet_LinkBase::vResetCom(bAlways);

    if (!IsOnline())
    {
      if (lu8t > 9)
      {
        mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwDma, False);
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
    //GPIOA->BSRR = (1 << 9);  // Test
    if (mAdr == mAdrSync) //Sync
    {
      vOnSync();
      mcUpLink->vSetMsg(&mpcMsgSync);
    }
    else if (mAdr == mAdrData) //Data
    {
      mcUpLink->vSetMsg(&mpcMsgData);
    }
    else if (mAdr == mAdrAck) //Ack
    {
      mcUpLink->vSetMsg(&mpcMsgAck);
    }

    switch (lenEvent)
    {
      case cComNode::tenEvent::enEvPrepareForTx:
        {
          vComStartTx();
        }
        break;
      case cComNode::tenEvent::enEvPrepareForRx:
        {
          vComStartRx();
        }
        break;
      case cComNode::tenEvent::enEvAfterTxStarted:
        {
          vComAfterTxStarted();
        }
        break;
      case cComNode::tenEvent::enEvAfterRxStarted:
        {
          vComAfterRxStarted();
        }
        break;
      default:
      break;

    }
    //GPIOA->BRR = (1 << 9);  // Test
  }

  void vComStartTx()
  {
    if (mAdr == mAdrAck) //Ack
    {
      /* Checksumme der empfangenen Daten überprüfen */
      /* Falls ok, dann wird die Ack Nachricht geACK't
          ansonsten geNACK't */
      if (mpcMsgData.cRxData.Len())
      {
        uint8 lui8ChkSum;

        mpcMsgData.cRxData.muiLen--; //Checksumme abschneiden
        lui8ChkSum = mpcMsgData.u8RxChecksum();

        if (mpcMsgData.cRxData.mpu8Data[mpcMsgData.cRxData.Len()] == lui8ChkSum)
        {
          // Slave Ack
          mpcMsgAck.cTxData[0] = 0xA; //Acknowledge
        }
        else
        {
          // Slave Nack
          mpcMsgAck.cTxData[0] = 0xF; // Fail
        }
      }
      else
      {
        // Falls es Daten nur zu senden gab, dann immer Ack
        // Slave Ack
        mpcMsgAck.cTxData[0] = 0xA; //Acknowledge
      }
    }
  }

  void vComAfterTxStarted()
  {
  }

  void vComStartRx() // __attribute__((optimize("-O0")))
  {
    if (mAdr == mAdrSync) //Sync
    {
      // Nach Adress (Match) fängt der Master direkt an 2 Byte zu senden (wird vom DMA erledigt)
      // bevor der Mastere direkt (ohne zu warten) einen Read-Request macht
      // 2 Byte @ 400Khz ~ 16*2,5us = 40us
      // Daher sind etwa (nach dem AdrMatch) 40us Zeit, um die Slave Tx Daten bereitzustellen
      // (slave kann master nicht warten lassen)
      mbPrepareChecksum = False;
      mStatus.IsLastTxOk = mStatus.IsThisTxOk;
      if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
      {
        // Daten holen
        mcTxComBuf.get(&mpcMsgData.cTxData);
        if (mpcMsgData.cTxData.muiLen)
        {
          mStatus.IsThisTxOk = 0;
          mu8MsgCntTx++;
        }

        // Ggf. Adresse entfernen
        if (mpcMsgData.cTxData.Len())
        {
          //Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
          cBotNetMsg_Base lcMsg(&mpcMsgData.cTxData);
          lcMsg.vDecode();
          if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
          {
            lcMsg.vAdressRemove();
            mpcMsgData.cTxData.muiLen = lcMsg.muiLen;
          }
          // Ggf. Checksumme berechnen und anfügen. Wurde in vComAfterRxStarted verschoben
          mbPrepareChecksum = True;
        }
      }

      mBussy = 0;
      // Schauen, ob noch Platz ist, ansonsten Bussy setzen
      if (mcRxComBuf.space_left() < (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
      {
        mBussy = 1;
      }
      if (mpcMsgData.cTxData.Len())
      {
        if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
        {
          vCreateSync(mpcMsgSync.cTxData.mpu8Data, mBussy, (uint8)mpcMsgData.cTxData.Len());
        }
        else
        {
          vCreateSync(mpcMsgSync.cTxData.mpu8Data, mBussy, (uint8)mpcMsgData.cTxData.Len() - 1); // -1 für die Checksumme
        }
        mStatus.IsFirstTx  = 0;
      }
      else
      {
        vCreateSync(mpcMsgSync.cTxData.mpu8Data, mBussy, 0);
      }
    }
  }

  void vComAfterRxStarted() // __attribute__((optimize("-O0")))
  {
    if (mAdr == mAdrSync) //Sync
    {
      if (mbPrepareChecksum)
      {
        // Während der Sync Laufzeit die Checksumme berechnen
        // Ggf. Checksumme berechnen und anfügen
        // Dauert ca. 30us bei 60Byte @16MHz
        mpcMsgData.cTxData.Add(mpcMsgData.u8TxChecksum());
      }
    }
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    //GPIOA->BSRR = (1 << 9);  // Test

    do
    {
      lbLoop = False;
      if (mAdr == mAdrSync)
      {
        // Sync
        vOnSync();
        if (mBussy)
        {
          mpcMsgData.cRxData.muiLen = 0;
        }
        else
        {
          mpcMsgData.cRxData.muiLen = mpcMsgSync.cRxData[0];
        }

        // Sync Checksumme ?
        if (IsSyncCheckOk(mpcMsgSync.cRxData.mpu8Data))
        {
          if (mpcMsgData.cRxData.Len())
          {
            mpcMsgData.cRxData.muiLen++; // +1 für Checksumme
          }

          // Gibt es zu übertragen ?
          if ((mpcMsgData.cRxData.Len()) || (mpcMsgData.cTxData.Len()))
          {
            // Ja, dann auf Daten warten
            mcUpLink->vSetAdr2(mAdrData);
          }
          else
          {
            // Nein, auf nächsten Sync warten
            mcUpLink->vSetAdr2(0);
            mStatus.IsThisTxOk = 1;
          }
        }
        else
        {
          //Checksumme schlecht
          mcUpLink->vSetAdr2(0); // Nicht antworten auf Daten NACK'en
        }
      }
      else if (mAdr == mAdrData)
      {
        // Daten
        mcUpLink->vSetAdr2(mAdrAck);
      }
      else if (mAdr == mAdrAck)
      {
        if (mpcMsgAck.cTxData[0] == 0xA)
        {
          // Falls der Master die Daten empfangen konnte, dann quittieren
          // ansonsten nochmals senden
          if (!(mpcMsgSync.cRxData[1] & 128) )
          {
            mStatus.IsThisTxOk = 1;
          }

          // Checksumme ok
          if (mpcMsgData.cRxData.muiLen)
          {
            // Wenn Daten empfangen wurden, dann
            // Rx Daten abholen und in den Rx-Buffer schreiben
            if (IsSyncNewData())
            {
              // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht vonm direkten Master
              // Senderadresse == Masteradresse  Zieladresse == meine Adresse (ich) dazufügen
              cBotNetMsg_Base lcMsg(&mpcMsgData.cRxData);
              lcMsg.vDecode();

              if (!lcMsg.bHasAdress())
              {
                lcMsg.vAdressAdd(mcDAdr.Get(), mcSAdr.Get());
                mpcMsgData.cRxData.muiLen = lcMsg.muiLen;
              }

              mcRxComBuf.put(&mpcMsgData.cRxData);
            }
          }
        }
      }
    } while (lbLoop);
    //GPIOA->BRR = (1 << 9);  // Test
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
  }
};


#endif // __BOTNET_LINK_I2C_H__
