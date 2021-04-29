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
  typedef enum
  {
    enCmdIdle = 0,
    enCmdSync,
    enCmdData,
    enCmdWaitForSlave,
    enCmdAck,
    enCmdEnd
  }tenI2cMem_Cmd;

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
  cI2cMaster*      mI2C;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgData;
  cComDatMsg mpcMsgAck;

  u8 mu8Sync;

  tenI2cMem_Cmd   menCmd;
  tenI2cMem_Cmd   menCmdLast;

  cBotNet_DownLinkI2c(uint16 lui16SlaveAdr, cI2cMaster* lpcI2C)
    : cBotNet_ComLinkI2c(lui16SlaveAdr, cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize)
  {
    uint8 lui8Adr;

    mcDAdr.Set(lui16SlaveAdr);
    mcSAdr.Set(mcDAdr.GetMasterAdr());

    lui8Adr = mcDAdr.GetIdx();
    lui8Adr <<= 1; // für I2C

    mI2C     = lpcI2C;

    mAdr = lui8Adr;
    mAdrSync = lui8Adr;
    mAdrData = lui8Adr + (0x20 << 1);
    mAdrAck  = lui8Adr + (0x40 << 1);

    mpcMsgSync.vMemAlloc(2,  2);
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);
    mpcMsgAck.vMemAlloc(0,  1);

    menCmd         = enCmdIdle;
    menCmdLast     = enCmdIdle;

    mStatus.IsInit         = 1;

    mI2C->vAddSlave((cComNode*)this);
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

  void vComError(cComNode::tenComNodeError lenError, cComNode::tenComNodeState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);

    /* Wird auch nach 100ms offline gesetzt
    if ((lenState == cComNode::tenComNodeState::enStAdress) &&
        (menCmdLast == enCmdSync))
    {
      mStatus.IsOnline = 0;
    }*/

    vResetStatusComFlags();
    menCmd         = enCmdIdle;

    if (mI2C->mSm == cI2cMaster::enStWait)
    {
      mI2C->mSm = cI2cMaster::enStIdle;
    }
  }

  void vResetCom(bool bAlways) override
  {
    cBotNet_LinkBase::vResetCom(bAlways);
    if ((!IsOnline()) || (bAlways))
    {
      menCmd = enCmdIdle;
    }
  }

  void vComStart(tenComNodeStartState lenState) override // __attribute__((optimize("-O0")))
  {
    GPIOA->BSRR = 1;
    if (lenState == cComNode::enStPrepareToSendData)
    {
      if (menCmd == enCmdIdle)
      {
        if (mu8Sync)
        {
          mu8Sync = 0;
          menCmd = enCmdSync;
          vComDone();
        }
      }
      mStatus.IsStartRequested = mu8Sync;
    }
    else if (lenState == cComNode::enStMisc)
    {
      // Misc istder Timer Interrupt vom I2C Treiber
      vComDone();
    }
    GPIOA->BRR = 1;
  }

  void vComDone() override  // __attribute__((optimize("-O0")))
  {
    // GPIOB->BSRR = (1 << 13); // Test
    //GPIOA->BSRR = 1; // Test

    menCmdLast = menCmd;
    switch (menCmd)
    {
      case enCmdIdle:
        break;

      case enCmdSync:
        {
          // Schauen, ob noch Platz ist, ansonsten Sync überspringen
          menCmd = enCmdIdle;
          if (mcRxComBuf.space_left() > (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
          {
            // Falls vorherige Daten übertragen werden konnten
            // dann nächste Daten laden
            // sonst vorherige Daten beibehalten
            mStatus.IsLastTxOk = mStatus.IsThisTxOk;
            mAdr = mAdrSync;

            if ((mpcMsgSync.cTxData.Len() == 0) || (mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
            {
              mStatus.IsThisTxOk = 0;

              // Überprüfen, obes Daten zu senden gibt
              mcTxComBuf.get(&mpcMsgData.cTxData);
              mStatus.IsFirstTx = 0;
              if (mpcMsgData.cTxData.Len())
              {
                mu8MsgCntTx++;
              }

              // Ggf. Adresse entfernen
              if (mpcMsgData.cTxData.Len())
              {
                //Überprüfen, ob der Empfänger direkter Slave ist, wenn ja, dann kann Adressinformation entfernt werden
                cBotNetMsg_Base lcMsg(&mpcMsgData.cTxData);
                lcMsg.vDecode();
                if (lcMsg.mcFrame.mcSAdr.isDirectMasterOf(lcMsg.mcFrame.mcDAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgData.cTxData.muiLen = lcMsg.muiLen;
                }
              }

              vCreateSync(mpcMsgSync.cTxData.mpu8Data, 0, (uint8)mpcMsgData.cTxData.Len());

              // Ggf. Checksumme berechnen und anfügen
              if (mpcMsgData.cTxData.Len())
              {
                mpcMsgData.cTxData.Add(mpcMsgData.u8TxChecksum());
              }
            }
            menCmd = enCmdData;
            mpcMsgSync.vStart();
            mI2C->vStartMsg(&mpcMsgSync);
          }
        }
        break;

      case enCmdData:
        {
          /* Rx-Len an Sync anpassen */
          vOnSync();
          menCmd = enCmdIdle;
          //Sync Checksumme ?
          if (IsSyncCheckOk(mpcMsgSync.cRxData.mpu8Data))
          {
            // Kein Bussy Flag beim Slave gesetzt ?
            if ((mpcMsgSync.cRxData[1] & 128) == 0)
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
                if (mpcMsgData.cTxData.Len() > mpcMsgData.cRxData.Len())
                {
                  // Dem Slave Zeit geben, damit Daten verarbeitet werden können
                  menCmd = enCmdWaitForSlave; // Empfange Daten Ack'en
                }
                else
                {
                  menCmd = enCmdAck; // Empfange Daten Ack'en
                }
                mpcMsgData.vStart();
                mI2C->vStartMsg(&mpcMsgData);
                mI2C->mTimer.vStart(5000); // Timeout
              }
              else
              {
                // Nichts zum Austauschen, dann in idle
                mStatus.IsThisTxOk = 1;
              }
            }
          }
        }
        break;

      case enCmdWaitForSlave:
        {
          // GPIOB->BSRR = (1 << 13);  // Test
          // Warten bis Slave die Daten verarbeitet hat.
          menCmd = enCmdAck;
          // Auf Wait stellen, damit I2C nicht StartNext macht
          mI2C->mSm = cI2cMaster::enStWait;
          mI2C->mTimer.vStart(10 + mpcMsgData.cTxData.Len() - mpcMsgData.cRxData.Len());
        }
      break;

      case enCmdAck:
        {
          bool lbSendAck;

          mAdr = mAdrAck;
          mI2C->mTimer.vStop();
          menCmd = enCmdIdle;

          mI2C->mSm = cI2cMaster::enStIdle; // Nur um sicher zu sein

          // GPIOB->BRR = (1 << 13);  // Test

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
            menCmd = enCmdEnd;
            mpcMsgAck.vStart();
            mI2C->vStartMsg(&mpcMsgAck);
            mpcMsgAck.cRxData[0] = 0xFF;
            mI2C->mTimer.vStart(5000); // Timeout
          }
        }
        break;

      case enCmdEnd:
        menCmd       = enCmdIdle;
        mI2C->mTimer.vStop();

        if (mpcMsgAck.cRxData.Len())
        {
          if (mpcMsgAck.cRxData[0] == 0xA)
          {
            mStatus.IsThisTxOk = 1;

            /* Rx Daten abholen und in den Rx-Buffer schreiben */
            if (mpcMsgData.cRxData.Len())
            {
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
        }
        break;

      default:
        menCmd = enCmdIdle;
        break;
    }
    //GPIOA->BRR = 1;  // Test
  }

  void vSync()
  {
    mu8Sync           = 1;
    mStatus.IsStartRequested = 1;
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkI2c:public cBotNet_ComLinkI2c
{
  public:
  cI2cSlave *mcI2cSlave;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgData;
  cComDatMsg mpcMsgAck;

  cBotNet_UpLinkI2c(uint16 lu16DestAdr, cI2cSlave* lcI2cSlave)
  : cBotNet_ComLinkI2c(lu16DestAdr, cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize)
  {
    mStatus.IsInit         = 1;

    vSet(lcI2cSlave);

    mpcMsgSync.vMemAlloc(2,  2);
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);
    mpcMsgAck.vMemAlloc(1,  0);

    mcI2cSlave->vSetMsg(&mpcMsgSync);

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

  void vSet(cI2cSlave *lcI2cSlave)
  {
    mcI2cSlave = lcI2cSlave;
  }

  void vComError(cComNode::tenComNodeError lenError, tenComNodeState lenState) override // __attribute__((optimize("-O0")))
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vResetCom(bool bAlways) override
  {
    static u8 lu8t = 0;

    cBotNet_LinkBase::vResetCom(bAlways);

    if (!IsOnline())
    {
      if (lu8t > 9)
      {
        mcI2cSlave->vReInitHw(cI2c::enResetHwDma, False);
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

  // ComStart wird nach I2C AdrMatch aufgerufen
  void vComStart(tenComNodeStartState lenState) override // __attribute__((optimize("-O0")))
  {
    //GPIOB->BSRR = (1 << 4);  // Test
    if (mAdr == mAdrSync) //Sync
    {
      vOnSync();
      mcI2cSlave->vSetMsg(&mpcMsgSync);
    }
    else if (mAdr == mAdrData) //Data
    {
      mcI2cSlave->vSetMsg(&mpcMsgData);
    }
    else if (mAdr == mAdrAck) //Ack
    {
      mcI2cSlave->vSetMsg(&mpcMsgAck);
    }

    if (lenState == cComNode::enStPrepareForTx)
    {
      vComStartTx();
    }
    else if (lenState == cComNode::enStPrepareForRx)
    {
      vComStartRx();
    }
    //GPIOB->BRR = (1 << 4);  // Test
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

      mStatus.IsLastTxOk = mStatus.IsThisTxOk;
      mStatus.IsThisTxOk = 0;
      if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
      {
        mStatus.IsFirstTx  = 0;

        // Daten holen
        mcTxComBuf.get(&mpcMsgData.cTxData);
        if (mpcMsgData.cTxData.muiLen)
        {
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
        }

        // Ggf. Checksumme berechnen und anfügen
        if (mpcMsgData.cTxData.Len())
        {
          mpcMsgData.cTxData.Add(mpcMsgData.u8TxChecksum());
        }
      }

      uint8 lu8Bussy = 0;
      // Schauen, ob noch Platz ist, ansonsten Bussy setzen
      if (mcRxComBuf.space_left() < (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
      {
        lu8Bussy = 1;
      }
      if (mpcMsgData.cTxData.Len())
      {
        vCreateSync(mpcMsgSync.cTxData.mpu8Data, lu8Bussy, (uint8)mpcMsgData.cTxData.Len() - 1); // -1 Wegen checksumme
      }
      else
      {
        vCreateSync(mpcMsgSync.cTxData.mpu8Data, lu8Bussy, 0);
      }
    }
  }

  void vComStartTx()  // __attribute__((optimize("-O0")))
  {
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    // GPIOB->BSRR = (1 << 5);  // Test
    if (mAdr == mAdrSync)
    {
      // Sync
      mpcMsgData.cRxData.muiLen = mpcMsgSync.cRxData[0];

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
          mcI2cSlave->vSetAdr2(mAdrData);
        }
        else
        {
          // Nein, auf nächsten Sync warten
          mcI2cSlave->vSetAdr2(0);
          mStatus.IsThisTxOk = 1;
        }
      }
      else
      {
        //Checksumme schlecht
        mcI2cSlave->vSetAdr2(0); // Nicht antworten auf Daten NACK'en
      }
    }
    else if (mAdr == mAdrData)
    {
      // Daten
      mcI2cSlave->vSetAdr2(mAdrAck);
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
    else if (mAdr == mAdrAck)
    {
      if (mpcMsgAck.cTxData[0] == 0xA)
      {
        mStatus.IsThisTxOk = 1;
        // Checksumme ok
        if (mpcMsgData.cRxData.muiLen)
        {
          /* Wenn Daten empfangen wurden, dann
              Rx Daten abholen und in den Rx-Buffer schreiben
          */
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

    //GPIOB->BRR = (1 << 5);  // Test
  }
};


#endif // __BOTNET_LINK_I2C_H__
