#ifndef __BOTNET_LINK_NRF905_H__
#define __BOTNET_LINK_NRF905_H__

#include "Typedef.h"
// Driver
#include "cnRF905.h"

#ifdef cNRF905_SYNC_DEBUG
  #include "stdlib.h"
  #include "cUsart.h"
#endif

// Decomment for Debug
//#define cNRF905_SYNC_DEBUG

#include "cBnLinkBase.h"

class Botnet;

class cBotNet_ComLinknRf905: public cBotNet_SyncedLinkBase
{
  public:

  typedef enum
  {
    StIdle = 0,

    // Master
    StMasterSync_Start,
    StMasterSync_Start_Done,
    StMasterSync_WaitForSlave_Start,
    StMasterSync_WaitForSlave_Done,

    StMasterData_Tx_Data_TxStart,
    StMasterData_Tx_Data_TxProcess,
    StMasterData_Tx_Data_TxDone,
    StMasterData_Tx_WaitForSlave_Ack,

    StMasterData_Rx_Data_WaitForSlave_RxStart,
    StMasterData_Rx_Data_WaitForSlave_RxProcess,
    StMasterData_ConfirmData,

    // Slave
    StSlaveSync_Start,
    StSlaveSync_WaitForMaster,
    StSlaveSync_Send_Start,
    StSlaveSync_Send_Done,

    StSlaveData_Rx_Data_WaitForMaster_RxStart,
    StSlaveData_Rx_Data_WaitForMaster_RxProcess,
    StSlaveData_Rx_Ack_WaitForSend,

    StSlaveData_Tx_Data_TxStart,
    StSlaveData_Tx_Data_TxProcess,
    StSlaveData_Tx_Ack_WaitMaster,
    StSlaveData_ConfirmData,
  }tenStates;

  cNRF905   *mcNRF905;
  tenStates  mSm;

  u16 mu16RxIdx;
  u16 mu16TxIdx;

  #ifdef cNRF905_SYNC_DEBUG
    tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcUart;
  #endif

  cComDatMsg mpcMsgData;

  cBotNet_ComLinknRf905(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType, cNRF905 *lcNRF905)
    : cBotNet_SyncedLinkBase(lu32RxComBufSize, lu32TxComBufSize, lenType)
  {
    mcNRF905 = lcNRF905;
  }

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_SyncedLinkBase::vReInit(lbHandleHardware);

    if (lbHandleHardware)
    {
      //mcNRF905->vReInit();

      bool mbnRF905 = mcNRF905->bInit();

      if (!mbnRF905)
      {
        mStatus.IsInit = 0;
        mStatus.IsOnline = 0;
      }
      else
      {
        mStatus.IsInit = 1;
      }
    }
    else
    {
      mStatus.IsInit = 0;
      mStatus.IsOnline = 0;
    }
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);

    bool mbnRF905 = mcNRF905->bInit();

    if (!mbnRF905)
    {
      mStatus.IsInit = 0;
      mStatus.IsOnline = 0;
      return False;
    }
    mStatus.IsInit = 1;
    return True;
  }
};

class cBotNet_DownLinknRf905:public cBotNet_ComLinknRf905
{
  public:

  cBotNet_DownLinknRf905(cNRF905 *lcNRF905)
    #ifdef cNRF905_SYNC_DEBUG
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enDownLink, lcNRF905)
    #else
     : cBotNet_ComLinknRf905(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enDownLink, lcNRF905)
    #endif
  {
    mSm = StIdle;
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);
  }

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_ComLinknRf905::vReInit(lbHandleHardware);
    mSm = StIdle;
      
    if (lbHandleHardware)
    {
      //mcNRF905->vReInit();

      bool mbnRF905 = mcNRF905->bInit();

      if (!mbnRF905)
      {
        mStatus.IsInit = 0;
        mStatus.IsOnline = 0;
      }
      else
      {
        mStatus.IsInit = 1;
      }
    }
    else
    {
      mStatus.IsInit = 0;
      mStatus.IsOnline = 0;
    }
  }


  virtual uint32 SizeOf()
  {
    /*uint32 lui32Size;

    lui32Size  = sizeof(cBotNet_DownLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();

    lui32Size -= 3 * sizeof(cComDatMsg);*/

    return 0; //lui32Size;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    mStatus.IsThisTxOk = 0;
  }

  void vOnResetCom() override
  {
    mSm = StIdle;
  }

  void vComStart(tenEvent lenEvent)
  {
    (void)lenEvent;
  }

  void  vComDone()
  {
  }

  void vMasterStartSync()
  {
    static uint8 lui8Retry = 0;

    if (mSm != StIdle)
    {
      lui8Retry++;
    }

    if ((mSm == StIdle) || (lui8Retry > 100))
    {
      lui8Retry = 0;
      mSm = StMasterSync_Start;
      mu16RxIdx = 0;
      mu16TxIdx = 0;
      bEventHandler(cNRF905::NRF905_EvDummy);
    }
  }

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent) // __attribute__((optimize("-O0")))
  {
    static uint8 lu8CheckSum;
    static uint16 lui16Data2Copy;

    uint8 lu8t;
    uint16 lui16DataIdx;

    #ifdef cNRF905_SYNC_DEBUG
      static char lszStr[16];
    #endif

    do
    {
      if (lenEvent == cNRF905::NRF905_EvLoop)
      {
        lenEvent = cNRF905::NRF905_EvDummy;
      }

      if (!mcNRF905->bEventHandler(lenEvent))
      {
        switch(mSm)
        {
          // ----------------------- Master Sync -----------------------
          case StMasterSync_Start:
            mSm = StMasterSync_Start_Done;

            mu16TxIdx = 0;
            mcNRF905->mTimer.vStop();

            if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
            {
              u16 lu16DataCnt;

              lu16DataCnt = mcTxComBuf.cnt();

              if (lu16DataCnt > 0)
              {
                mStatus.IsLastTxOk = 0;
                mStatus.IsFirstTx  = 0;
              }

              // FlKa:  der Code scheint nutzlos zu sein
              if (lu16DataCnt > mpcMsgData.cTxData.Size())
              {
                lu16DataCnt = mpcMsgData.cTxData.Size();
              }

              mcTxComBuf.get(&mpcMsgData.cTxData);

              if (mpcMsgData.cTxData.Len())
              {
                mu8MsgCntTx++;
              }

              lu8CheckSum = mpcMsgData.u8TxChecksum();
              // Checksumme am Ende hinzufügen
              mpcMsgData.cTxData.mpu8Data[mpcMsgData.cTxData.muiLen] = lu8CheckSum;
            }

            // Sync Nachricht: 3Byte
            // 0001 0000 | DDDD DDDD | BCCC MMMM
            //  D: 8Bit: Anzahl Daten zu senden 0..64
            //  B: 1Bit: Busy Flag:
            //              0: kann Daten emmpfangen
            //              1: kann keine Daten empfangen
            //  C: 3Bit: Checksumme: D + M + B + 1
            //  M: 4Bit: MessageCounter
            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 3;
            mcNRF905->mstNRF905.mui8TxPayLoad[0] = 16; // Sync Identifier

            // Flka: Das ID Byte ist nicht teil der Checksumme
            vCreateSync(&mcNRF905->mstNRF905.mui8TxPayLoad[1], (uint8)mpcMsgData.cTxData.Len(), 0);
            mcNRF905->vStartTransmit();
            break;

          case StMasterSync_Start_Done: // Sync wurde gesendet, auf Antwort warten
            mSm = StMasterSync_WaitForSlave_Start;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 3;
            mcNRF905->vStartReceive();

            //Timeout setzen
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            break;

          case StMasterSync_WaitForSlave_Start:
            mSm = StIdle;

            mcNRF905->mTimer.vStop();
            // Timeout ?
            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 3) &&
                  (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 16) &&
                  // Flka: Das ID Byte ist nicht teil der Checksumme
                  (IsSyncCheckOk(&mcNRF905->mstNRF905.mui8RxPayLoad[1])))
              {
                vOnSync();
                mu16RxIdx   = 0;
                mpcMsgData.cRxData.muiLen = mcNRF905->mstNRF905.mui8RxPayLoad[1];

                // Gibt es Daten ? Ansonsten in Idle
                if ((mpcMsgData.cTxData.muiLen) || (mpcMsgData.cRxData.muiLen))
                {
                  // Gibt es Tx Daten ? Ansonsten in direkt in Rx
                  if (mpcMsgData.cTxData.muiLen)
                  {
                    mSm = StMasterData_Tx_Data_TxStart;

                    //Timer fürs Senden
                    mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY); // Warten für Slave
                  }
                  else
                  {
                    mSm = StMasterData_Rx_Data_WaitForSlave_RxStart;
                    lenEvent = cNRF905::NRF905_EvLoop;
                  }
                }
                else
                {
                  mStatus.IsLastTxOk = 1;
                }
              }
            }

            break;

          // ----------------------- Master Data -----------------------

          // ----------------------- Master Data TX -----------------------
          case StMasterData_Tx_Data_TxStart:
            mSm = StMasterData_Tx_Data_TxProcess;

            lui16Data2Copy = (mpcMsgData.cTxData.muiLen + 1) - mu16TxIdx;   // Len+1 für Checksumme
            if (lui16Data2Copy > 31) lui16Data2Copy = 31;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier
            if (mu16TxIdx == 0)
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[0] = 0;
            }
            else
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[0]++;
            }

            for (lui16DataIdx = 0; lui16DataIdx < lui16Data2Copy; lui16DataIdx++)
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[lui16DataIdx + 1] = mpcMsgData.cTxData.mpu8Data[mu16TxIdx + lui16DataIdx];
            }
            mu16TxIdx += lui16Data2Copy;

            mcNRF905->vStartTransmit();
            break;

          case StMasterData_Tx_Data_TxProcess:
            mSm = StMasterData_Tx_Data_TxDone;

            if (mu16TxIdx < (mpcMsgData.cTxData.muiLen + 1))   // +1 wg. Checksumme
            {
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
              mSm = StMasterData_Tx_Data_TxStart;
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
            break;

          case StMasterData_Tx_Data_TxDone:
            mSm = StMasterData_Tx_WaitForSlave_Ack;
            // Auf Msg-Ack warten
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 3;
            mcNRF905->vStartReceive();
            //Timeout setzen
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            break;

          case StMasterData_Tx_WaitForSlave_Ack:
            mSm = StIdle;

            mcNRF905->mTimer.vStop();
            // Timeout ?
            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 3) &&
                  (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 64))
              {
                // Gibt es Rx Daten ? Ansonsten Idle
                if (mpcMsgData.cRxData.muiLen)
                {
                  mSm = StMasterData_Rx_Data_WaitForSlave_RxStart;
                  lenEvent = cNRF905::NRF905_EvLoop;
                }
                else
                {
                  mStatus.IsLastTxOk = 1;
                }
              }
              else
              {
                #ifdef cNRF905_SYNC_DEBUG
                  mcUart.vSend((const char*)"Ma_Sl Nack. Discard Data");
                #endif
              }
            }
            else
            {
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend((const char*)"Ma_Sl TOut. Discard Data");
              #endif
            }
            break;

          // ----------------------- Master Data RX -----------------------

          case StMasterData_Rx_Data_WaitForSlave_RxStart:
            mSm = StIdle;

            mcNRF905->mTimer.vStop();

            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              mu16RxIdx   = 0;
              lu8CheckSum = 1;

              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 1 + mpcMsgData.cRxData.muiLen + 1; // 1+ für Identifier +1 für Checksumme
              if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width > 32)
              {
                mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 32;
              }

              // Initialisierung
              lui16Data2Copy = (mpcMsgData.cRxData.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
              if (lui16Data2Copy > 31) lui16Data2Copy = 31;
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

              mcNRF905->vStartReceive();
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
              mSm = StMasterData_Rx_Data_WaitForSlave_RxProcess;
            }
            break;

          case StMasterData_Rx_Data_WaitForSlave_RxProcess:
            mSm = StIdle;

            mcNRF905->mTimer.vStop();

            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              if (mcNRF905->mstNRF905.mui8RxPayLoad[0] < 16)
              {
                // Daten holen
                for (lui16DataIdx = 0; lui16DataIdx < lui16Data2Copy; lui16DataIdx++)
                {
                  mpcMsgData.cRxData.mpu8Data[mu16RxIdx + lui16DataIdx] = mcNRF905->mstNRF905.mui8RxPayLoad[lui16DataIdx + 1];
                }

                mu16RxIdx  += lui16Data2Copy;

                // Alle Daten empfangen
                if (mu16RxIdx < (mpcMsgData.cRxData.muiLen + 1))   // +1 wg. Checksumme
                {

                  lui16Data2Copy = (mpcMsgData.cRxData.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
                  if (lui16Data2Copy > 31) lui16Data2Copy = 31;
                  mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

                  mSm = StMasterData_Rx_Data_WaitForSlave_RxProcess;
                  mcNRF905->vStartReceive();
                  mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
                }
                else
                {
                  // Empfangene Checksumme am berechnen
                  for (lu8t = 0; lu8t < mpcMsgData.cRxData.muiLen; lu8t++)
                  {
                    lu8CheckSum += mpcMsgData.cRxData.mpu8Data[lu8t];
                  }

                  if (mpcMsgData.cRxData.mpu8Data[mpcMsgData.cRxData.muiLen] == lu8CheckSum)
                  {
                    mSm = StMasterData_ConfirmData;
                    mcNRF905->mstNRF905.mui8TxPayLoad[0] = 64;    // Antworte mit Msg-Ack
                  }
                  else
                  {
                    mcNRF905->mstNRF905.mui8TxPayLoad[0] = 0xFE;  // Antworte mit Msg-Nack
                  }

                  // FlKa: Eigentlich müsste man vor dem Senden noch die NRF905_WAIT_TX_DELAY einbauen ?
                  //       Warscheinlich dauert aber die Ausführung des Code davor so lange, dass genug Delay da ist
                  //       und es bislang funktioniert hat.
                  mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 3;
                  mcNRF905->mstNRF905.mui8TxPayLoad[1] = 0;
                  mcNRF905->mstNRF905.mui8TxPayLoad[2] = 0;
                  mcNRF905->vStartTransmit();
                }
              }
            }
            break;

          case StMasterData_ConfirmData:
            mSm = StIdle;
            mStatus.IsLastTxOk = 1;

            /* Rx Daten in den Rx-Buffer schreiben */
            if (mpcMsgData.cRxData.Len())
            {
              if (IsSyncNewData())
              {
                mcRxComBuf.put(&mpcMsgData.cRxData);
              }
            }

            lenEvent = cNRF905::NRF905_EvLoop;
            break;

          default:
            break;
        }
      }
    } while (lenEvent == cNRF905::NRF905_EvLoop);

    return False;
  }
};

class cBotNet_UpLinknRf905:public cBotNet_ComLinknRf905
{
  public:

  cBotNet_UpLinknRf905(cNRF905 *lcNRF905)
    #ifdef cNRF905_SYNC_DEBUG
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink, lcNRF905)
    #else
     : cBotNet_ComLinknRf905(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink, lcNRF905)
    #endif
  {
    mSm = StIdle;

    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    mStatus.IsInit   = 1;
  }

  void vReInit(bool lbHandleHardware)
  {
    cBotNet_ComLinknRf905::vReInit(lbHandleHardware);
    mSm = StIdle;
    mStatus.IsInit = 1;
  }

  virtual uint32 SizeOf()
  {
    /*uint32 lui32Size;

    lui32Size  = sizeof(cBotNet_DownLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();

    lui32Size -= 3 * sizeof(cComDatMsg);*/

    return 0; //lui32Size;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
     mStatus.IsThisTxOk = 0;
  }

  void vOnResetCom() override
  {
    mSm = StIdle;
  }

  void vComStart(tenEvent lenEvent) override
  {
    (void)lenEvent;
  }

  void  vComDone() override
  {
  }

  void vSlaveStartSync()
  {
    mSm = StSlaveSync_Start;
    mu16RxIdx = 0;
    mu16TxIdx = 0;
    vSyncStart();
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent)  // __attribute__((optimize("-O0")))
  {
    static uint8 lu8CheckSum;
    static uint16 lui16Data2Copy;

    uint8 lu8t;
    uint16 lui16DataIdx;

    #ifdef cNRF905_SYNC_DEBUG
      static char lszStr[16];
    #endif

    do
    {
      if (lenEvent == cNRF905::NRF905_EvLoop)
      {
        lenEvent = cNRF905::NRF905_EvDummy;
      }

      if (!mcNRF905->bEventHandler(lenEvent))
      {
        switch(mSm)
        {
          // ----------------------- Slave Sync -----------------------
          case StSlaveSync_Start:
            // Empänger einschalten und DR-DataReady Interrupt aktivieren
            mSm = StSlaveSync_WaitForMaster;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 3;
            mcNRF905->vStartReceive();
            break;

          case StSlaveSync_WaitForMaster:
            mSm = StSlaveSync_Start;

            if (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 16)
            {
              if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 3)
              {
                #ifdef cNRF905_SYNC_DEBUG
                  mcUart.vAdd((const char*)"\nMa_Sync: ");
                  itoa(mcNRF905->mstNRF905.mui8RxPayLoad[1], lszStr, 10);
                  mcUart.vAdd(lszStr);
                  mcUart.vPrintMem(':', mcNRF905->mstNRF905.mui8RxPayLoad, mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width);
                  mcUart.vSend((const char*)" ");
                #endif

                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY); // Warten damit Master in Rx wechsel kann
                mSm = StSlaveSync_Send_Start;
              }
              else
              {
                lenEvent = cNRF905::NRF905_EvLoop;
              }
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
            break;

          case StSlaveSync_Send_Start:
            mSm = StSlaveSync_Start;

            mcNRF905->mTimer.vStop();
            if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 3) &&
                (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 16) &&
                // Flka: Das ID Byte ist nicht teil der Checksumme
                (IsSyncCheckOk(&mcNRF905->mstNRF905.mui8RxPayLoad[1])))
            {
              vOnSync();

              // Antworte mit Sync
              if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
              {
                u16 lu16DataCnt;

                lu16DataCnt = mcTxComBuf.cnt();

                if (lu16DataCnt > 0)
                {
                  mStatus.IsLastTxOk = 0;
                  mStatus.IsFirstTx  = 0;
                }

                // FlKa: der Code scheint nutzlos zu sein
                if (lu16DataCnt > mpcMsgData.cTxData.Size())
                {
                  lu16DataCnt = mpcMsgData.cTxData.Size();
                }

                mcTxComBuf.get(&mpcMsgData.cTxData);

                if (mpcMsgData.cTxData.Len())
                {
                  mu8MsgCntTx++;
                }

                lu8CheckSum = mpcMsgData.u8TxChecksum();
                // Checksumme am Ende hinzufügen
                mpcMsgData.cTxData.mpu8Data[mpcMsgData.cTxData.muiLen] = lu8CheckSum;
              }

              mu16TxIdx = 0;
              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 3;
              mcNRF905->mstNRF905.mui8TxPayLoad[0] = 16;

              mStatus.IsBusy = 0;
              // Schauen, ob noch Platz ist, ansonsten Busy setzen
              if (mcRxComBuf.space_left() < (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
              {
                mStatus.IsBusy = 1;
              }
              
              // Flka: Das ID Byte ist nicht teil der Checksumme
              vCreateSync(&mcNRF905->mstNRF905.mui8TxPayLoad[1], (uint8)mpcMsgData.cTxData.Len(), 0);

              mcNRF905->vStartTransmit();
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend((const char*)"\nSl_Sync");
              #endif

              if (!mStatus.IsBusy)
              {
                mSm = StSlaveSync_Send_Done;
              }
            }
            else
            {
              // Antworte mit NACK
              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 3;
              mcNRF905->mstNRF905.mui8TxPayLoad[0] = 0xFF;
              mcNRF905->mstNRF905.mui8TxPayLoad[1] = 0;
              mcNRF905->mstNRF905.mui8TxPayLoad[2] = 0;
              mcNRF905->vStartTransmit();
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend((const char*)"\nSl_NACK");
              #endif

              lenEvent = cNRF905::NRF905_EvLoop;
            }
            break;

          case StSlaveSync_Send_Done:
            mSm = StSlaveSync_Start;
            lenEvent = cNRF905::NRF905_EvLoop;

            mpcMsgData.cRxData.muiLen = mcNRF905->mstNRF905.mui8RxPayLoad[1];

            // Gibt es Daten ? Ansonsten in Idle
            if ((mpcMsgData.cTxData.muiLen) || (mpcMsgData.cRxData.muiLen))
            {
              // Gibt es Rx Daten ? Ansonsten in direkt in Tx
              if (mpcMsgData.cRxData.muiLen)
              {
                mSm = StSlaveData_Rx_Data_WaitForMaster_RxStart;
                lenEvent = cNRF905::NRF905_EvLoop;
              }
              else
              {
                mSm = StSlaveData_Tx_Data_TxStart;
                lenEvent = cNRF905::NRF905_EvDummy;

                //Timer fürs senden, damit Master Zeit hat, um in Rx-Mode zu gehen
                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
              }
            }
            break;


          // ----------------------- Slave Data Rx -----------------------
          case StSlaveData_Rx_Data_WaitForMaster_RxStart:
            mSm = StSlaveData_Rx_Data_WaitForMaster_RxProcess;

            mu16RxIdx   = 0;
            lu8CheckSum = 1;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 1 + mpcMsgData.cRxData.muiLen + 1; // 1+ für Identifier +1 für Checkcsumme
            if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width > 32)
            {
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 32;
            }

            // Initialisierung
            lui16Data2Copy = (mpcMsgData.cRxData.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
            if (lui16Data2Copy > 31) lui16Data2Copy = 31;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

            mcNRF905->vStartReceive();

            break;

          case StSlaveData_Rx_Data_WaitForMaster_RxProcess:
            mSm = StSlaveSync_Start;

            if (mcNRF905->mstNRF905.mui8RxPayLoad[0] < 16)
            {
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vAdd("Data: ");
                itoa(mcNRF905->mstNRF905.mui8RxPayLoad[0], lszStr, 10);
                mcUart.vAdd(lszStr);
                mcUart.vAdd(": ");
                itoa(lui16Data2Copy, lszStr, 10);
                mcUart.vSend(lszStr);
              #endif

              // Daten holen
              for (lui16DataIdx = 0; lui16DataIdx < lui16Data2Copy; lui16DataIdx++)
              {
                mpcMsgData.cRxData.mpu8Data[mu16RxIdx + lui16DataIdx] = mcNRF905->mstNRF905.mui8RxPayLoad[lui16DataIdx + 1];
              }

              mu16RxIdx  += lui16Data2Copy;

              // Alle Daten empfangen
              if (mu16RxIdx < (mpcMsgData.cRxData.muiLen + 1))   // +1 wg. Checksumme
              {
                // es kommt noch was
                lui16Data2Copy = (mpcMsgData.cRxData.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
                if (lui16Data2Copy > 31) lui16Data2Copy = 31;
                mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

                mSm = StSlaveData_Rx_Data_WaitForMaster_RxProcess;
                mcNRF905->vStartReceive();
              }
              else
              {
                // Alle Daten empfangen
                // Empfangene Checksumme am berechnen
                for (lu8t = 0; lu8t < mpcMsgData.cRxData.muiLen; lu8t++)
                {
                  lu8CheckSum += mpcMsgData.cRxData.mpu8Data[lu8t];
                }

                if (mpcMsgData.cRxData.mpu8Data[mpcMsgData.cRxData.muiLen] == lu8CheckSum)
                {
                  #ifdef cNRF905_SYNC_DEBUG
                    mcUart.vSend((const char*)"Ok");
                  #endif

                  // Antworte mit Ack
                  // FlKa: Eigentlich müsste man vor dem Senden noch die NRF905_WAIT_TX_DELAY einbauen ?
                  //       Warscheinlich dauert aber die Ausführung des Code davor so lange, dass genug Delay da ist
                  //       und es bislang funktioniert hat.
                  mu16TxIdx = 0;
                  mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 3;
                  mcNRF905->mstNRF905.mui8TxPayLoad[0] = 64;
                  mcNRF905->mstNRF905.mui8TxPayLoad[1] = 0;
                  mcNRF905->mstNRF905.mui8TxPayLoad[2] = 0;

                  #ifdef cNRF905_SYNC_DEBUG
                    mcUart.vSend((const char*)"Data Ack");
                  #endif

                  mu16TxIdx = 0;

                  mcNRF905->vStartTransmit();
                  mSm = StSlaveData_Rx_Ack_WaitForSend;
                }
                else
                {
                  // FlKa: Das scheint falsch zu sein. Event wird kein Nack gesendet.
                  //       Vgl. zu Master Ack
                  #ifdef cNRF905_SYNC_DEBUG
                    mcUart.vSend((const char*)"NOk");
                  #endif
                }

                // FlKa: Das scheint falsch zu sein. Event wird von rf905 ausgelöst
                //       Vgl. zu Master Ack
                lenEvent = cNRF905::NRF905_EvLoop;
              }
            }
            else
            {
              lenEvent = cNRF905::NRF905_EvLoop;
            }
            break;

          case StSlaveData_Rx_Ack_WaitForSend:
            mSm = StSlaveData_ConfirmData;
            lenEvent = cNRF905::NRF905_EvLoop;

            #ifdef cNRF905_SYNC_DEBUG
              mcUart.vSend("Sl_Cnf Data");
            #endif

            /* Rx Daten abholen und in den Rx-Buffer schreiben */
            if (mpcMsgData.cRxData.Len())
            {
              if (IsSyncNewData())
              {
                mcRxComBuf.put(&mpcMsgData.cRxData);
              }
            }

            // Gibt es Tx Daten ? 
            if (mpcMsgData.cTxData.muiLen)
            {
              mSm = StSlaveData_Tx_Data_TxStart;
            }
            break;

          // ----------------------- Slave Data Tx -----------------------
          case StSlaveData_Tx_Data_TxStart:
            // Ack gesendet, dann sende Daten
            mSm = StSlaveData_Tx_Data_TxProcess;

            lui16Data2Copy = (mpcMsgData.cTxData.muiLen + 1) - mu16TxIdx;   // Len+1 für Checksumme
            if (lui16Data2Copy > 31) lui16Data2Copy = 31;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier
            if (mu16TxIdx == 0)
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[0] = 0;
            }
            else
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[0]++;
            }

            #ifdef cNRF905_SYNC_DEBUG
              mcUart.vAdd((const char*)"Sl_Data: ");
              itoa(mcNRF905->mstNRF905.mui8TxPayLoad[0], lszStr, 10);
              mcUart.vAdd(lszStr);
              mcUart.vAdd(":");
              itoa(lui16Data2Copy, lszStr, 10);
              mcUart.vSend(lszStr);
            #endif

            for (lui16DataIdx = 0; lui16DataIdx < lui16Data2Copy; lui16DataIdx++)
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[lui16DataIdx + 1] = mpcMsgData.cTxData.mpu8Data[mu16TxIdx + lui16DataIdx];
            }
            mu16TxIdx += lui16Data2Copy;

            mcNRF905->vStartTransmit();
            break;

          case StSlaveData_Tx_Data_TxProcess:
            mSm = StSlaveData_Tx_Ack_WaitMaster;

            if (mu16TxIdx < (mpcMsgData.cTxData.muiLen + 1))   // +1 wg. Checksumme
            {
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
              mSm = StSlaveData_Tx_Data_TxStart;
            }
            else // Sl_Wait_Ack
            {
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 3;
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend("Sl_Wait_Ack: ");
              #endif
              mcNRF905->vStartReceive();

              //Timeout setzen
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            }
            break;
          case StSlaveData_Tx_Ack_WaitMaster:
            mSm = StSlaveSync_Start;

            // Timeout ?
            mcNRF905->mTimer.vStop();
            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 3) &&
                  (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 64))
              {
                #ifdef cNRF905_SYNC_DEBUG
                  mcUart.vSend("Sl_Ma Ack.");
                #endif
                mSm = StSlaveData_ConfirmData;
                mStatus.IsLastTxOk = 1;
              }
              else
              {
                #ifdef cNRF905_SYNC_DEBUG
                  mcUart.vSend("Sl_Ma Nack. Discard Data");
                #endif
              }
            }
            else
            {
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend("Sl_Ma TOut. Discard Data");
              #endif
            }

            lenEvent = cNRF905::NRF905_EvLoop;
            break;

          case StSlaveData_ConfirmData:
            mSm = StSlaveSync_Start;

            lenEvent = cNRF905::NRF905_EvLoop;
            break;
          default:
            break;
        }
      }
    } while (lenEvent == cNRF905::NRF905_EvLoop);

    return False;
  }
};

#endif // __BOTNET_LINK_NRF905_H__
