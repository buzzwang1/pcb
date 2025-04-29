#ifndef _cNRF905_SYNC_H
#define _cNRF905_SYNC_H

// Decomment for Debug
//#define cNRF905_SYNC_DEBUG

#include "TYPEDEF.h"

// Driver
#include "cnRF905.h"

#ifdef cNRF905_SYNC_DEBUG
  #include "stdlib.h"
  #include "cUsart.h"
#endif

#include "cbArrayExtT.h"
#include "cRingBufT.h"

class cNRF905_Sync
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
  tenStates mSm;

  #ifdef cNRF905_SYNC_DEBUG
    tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcUart;
  #endif

  cRingBufDynT<u8, u16> mcRingBufTx;
  cRingBufDynT<u8, u16> mcRingBufRx;

  cbArrayExtDynT<uint16> mcMsgTx;
  cbArrayExtDynT<uint16> mcMsgRx;

  uint8                  mu8RxMsgCnt;
  uint8                  mu8RxMsgCntLast;
  uint8                  mu8TxMsgCnt;


  uint16                 mu16RxIdx;
  uint16                 mu16TxIdx;

  u8                     mu8LastTransmitValid;

  cNRF905_Sync(u16 lu16MsgSize, u16 lu16RingBufSize, cNRF905 *lcNRF905)
    #ifdef cNRF905_SYNC_DEBUG
     : mcUartDataTx(lu16RingBufSize), mcUartDataRx(lu16RingBufSize),
       mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       mcMsgTx(lu16MsgSize + 1), mcMsgRx(lu16MsgSize + 1) // +1 für Checksumme
    #else
     : mcRingBufTx(lu16RingBufSize), mcRingBufRx(lu16RingBufSize),
       mcMsgTx(lu16MsgSize + 1), mcMsgRx(lu16MsgSize + 1) // +1 für Checksumme
    #endif
  {
    mcNRF905 = lcNRF905;
    mSm = StIdle;
    mu8LastTransmitValid = 1;
    mu8RxMsgCnt     = 0;
    mu8RxMsgCntLast = 0xFF;
    mu8TxMsgCnt     = 0;
  }

  void vSlaveStartSynv()
  {
    mSm = StSlaveSync_Start;
    mu16RxIdx = 0;
    mu16TxIdx = 0;

    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  void vMasterStartSynv()
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

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent)
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

            if (mu8LastTransmitValid)
            {
              u8  lu8Data;
              u16 lu16DataCnt;
              u16 lu16DataIdx;

              mu8LastTransmitValid = 0;
              lu16DataCnt = mcRingBufTx.cnt();
              if (lu16DataCnt > mcMsgTx.Size())
              {
                lu16DataCnt = mcMsgTx.Size();
              }

              lu8CheckSum = 1;
              mcMsgTx.muiLen = 0;
              for (lu16DataIdx = 0; lu16DataIdx < lu16DataCnt; lu16DataIdx++)
              {
                lu8Data = mcRingBufTx.get();
                mcMsgTx.Add(lu8Data);
                lu8CheckSum += lu8Data;
              }
              // Checksumme am Ende hinzufügen
              mcMsgTx.mpu8Data[mcMsgTx.muiLen] = lu8CheckSum;
            }

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 4;
            mcNRF905->mstNRF905.mui8TxPayLoad[0] = 16;
            mcNRF905->mstNRF905.mui8TxPayLoad[1] = mu8TxMsgCnt;
            mcNRF905->mstNRF905.mui8TxPayLoad[2] = mcMsgTx.muiLen;
            mcNRF905->mstNRF905.mui8TxPayLoad[3] = mcMsgTx.muiLen + mu8TxMsgCnt + 16 + 1;
            mcNRF905->vStartTransmit();
            break;

          case StMasterSync_Start_Done: // Sync wurde gesendet, auf Antwort warten
            mSm = StMasterSync_WaitForSlave_Start;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 4;
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
              if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 4) &&
                  (((mcNRF905->mstNRF905.mui8RxPayLoad[1] + 
                     mcNRF905->mstNRF905.mui8RxPayLoad[2] + 
                     16 + 1) & 0xFF) == mcNRF905->mstNRF905.mui8RxPayLoad[3]))
              {
                mu16RxIdx   = 0;
                mu8RxMsgCnt    = mcNRF905->mstNRF905.mui8RxPayLoad[1];
                mcMsgRx.muiLen = mcNRF905->mstNRF905.mui8RxPayLoad[2];

                // Gibt es Daten ? Ansonsten in Idle
                if ((mcMsgTx.muiLen) || (mcMsgRx.muiLen))
                {
                  // Gibt es Tx Daten ? Ansonsten in direkt in Rx
                  if (mcMsgTx.muiLen)
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
                  mu8LastTransmitValid = 1;
                }
              }
            }

            break;

          // ----------------------- Master Data -----------------------

          // ----------------------- Master Data TX -----------------------
          case StMasterData_Tx_Data_TxStart:
            mSm = StMasterData_Tx_Data_TxProcess;

            lui16Data2Copy = (mcMsgTx.muiLen + 1) - mu16TxIdx;   // Len+1 für Checksumme
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
              mcNRF905->mstNRF905.mui8TxPayLoad[lui16DataIdx + 1] = mcMsgTx.mpu8Data[mu16TxIdx + lui16DataIdx];
            }
            mu16TxIdx += lui16Data2Copy;

            mcNRF905->vStartTransmit();
            break;

          case StMasterData_Tx_Data_TxProcess:
            mSm = StMasterData_Tx_Data_TxDone;

            if (mu16TxIdx < (mcMsgTx.muiLen + 1))   // +1 wg. Checksumme
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
                if (mcMsgRx.muiLen)
                {
                  mSm = StMasterData_Rx_Data_WaitForSlave_RxStart;
                  lenEvent = cNRF905::NRF905_EvLoop;
                }
                else
                {
                  mu8LastTransmitValid = 1;
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

              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 1 + mcMsgRx.muiLen + 1; // 1+ für Identifier +1 für Checksumme
              if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width > 32)
              {
                mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 32;
              }

              // Initialisierung
              lui16Data2Copy = (mcMsgRx.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
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
                  mcMsgRx.mpu8Data[mu16RxIdx + lui16DataIdx] = mcNRF905->mstNRF905.mui8RxPayLoad[lui16DataIdx + 1];
                }

                mu16RxIdx  += lui16Data2Copy;

                // Alle Daten empfangen
                if (mu16RxIdx < (mcMsgRx.muiLen + 1))   // +1 wg. Checksumme
                {

                  lui16Data2Copy = (mcMsgRx.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
                  if (lui16Data2Copy > 31) lui16Data2Copy = 31;
                  mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

                  mSm = StMasterData_Rx_Data_WaitForSlave_RxProcess;
                  mcNRF905->vStartReceive();
                  mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
                }
                else
                {
                  // Empfangene Checksumme am berechnen
                  for (lu8t = 0; lu8t < mcMsgRx.muiLen; lu8t++)
                  {
                    lu8CheckSum += mcMsgRx.mpu8Data[lu8t];
                  }

                  if (mcMsgRx.mpu8Data[mcMsgRx.muiLen] == lu8CheckSum)
                  {
                    mSm = StMasterData_ConfirmData;
                    mcNRF905->mstNRF905.mui8TxPayLoad[0] = 64;    // Antworte mit Msg-Ack
                  }
                  else
                  {
                    mcNRF905->mstNRF905.mui8TxPayLoad[0] = 0xFE;  // Antworte mit Msg-Nack
                  }
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
            mu8TxMsgCnt++;
            mu8LastTransmitValid = 1;


            if (mu8RxMsgCntLast != mu8RxMsgCnt)
            {
              u16 lu16DataIdx;

              for (lu16DataIdx = 0; lu16DataIdx < mcMsgRx.muiLen; lu16DataIdx++)
              {
                mcRingBufRx.put(mcMsgRx[lu16DataIdx]);
              }
            }

            mu8RxMsgCntLast  = mu8RxMsgCnt;

            lenEvent = cNRF905::NRF905_EvLoop;
            break;



          // ----------------------- Slave Sync -----------------------
          case StSlaveSync_Start:
            // Empänger einschalten und DR-DataReady Interrupt aktivieren
            mSm = StSlaveSync_WaitForMaster;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 4;
            mcNRF905->vStartReceive();
            break;

          case StSlaveSync_WaitForMaster:
            mSm = StSlaveSync_Start;

            if (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 16)
            {
              if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 4)
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
            if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 4) &&
                (((mcNRF905->mstNRF905.mui8RxPayLoad[1] + 
                   mcNRF905->mstNRF905.mui8RxPayLoad[2] + 
                   16 + 1) & 0xFF) == mcNRF905->mstNRF905.mui8RxPayLoad[3]))
            {
              // Antworte mit Sync
              if (mu8LastTransmitValid)
              {
                u8  lu8Data;
                u16 lu16DataCnt;
                u16 lu16DataIdx;

                //mu8LastTransmitValid = 0;
                lu16DataCnt = mcRingBufTx.cnt();
                if (lu16DataCnt > mcMsgTx.Size())
                {
                  lu16DataCnt = mcMsgTx.Size();
                }

                mcMsgTx.muiLen = 0;
                lu8CheckSum = 1;
                for (lu16DataIdx = 0; lu16DataIdx < lu16DataCnt; lu16DataIdx++)
                {
                  lu8Data = mcRingBufTx.get();
                  lu8CheckSum += lu8Data;
                  mcMsgTx.Add(lu8Data);
                }
              // Checksumme am Ende hinzufügen
                mcMsgTx.mpu8Data[mcMsgTx.muiLen] = lu8CheckSum;
              }

              mu16TxIdx = 0;
              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 4;
              mcNRF905->mstNRF905.mui8TxPayLoad[0] = 16;
              mcNRF905->mstNRF905.mui8TxPayLoad[1] = mu8TxMsgCnt;
              mcNRF905->mstNRF905.mui8TxPayLoad[2] = mcMsgTx.muiLen;
              mcNRF905->mstNRF905.mui8TxPayLoad[3] = mcMsgTx.muiLen + mu8TxMsgCnt + 16 + 1;

              mcNRF905->vStartTransmit();
              #ifdef cNRF905_SYNC_DEBUG
                mcUart.vSend((const char*)"\nSl_Sync");
              #endif

              mSm = StSlaveSync_Send_Done;
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

            mu8RxMsgCnt    = mcNRF905->mstNRF905.mui8RxPayLoad[1];
            mcMsgRx.muiLen = mcNRF905->mstNRF905.mui8RxPayLoad[2];

            // Gibt es Daten ? Ansonsten in Idle
            if ((mcMsgTx.muiLen) || (mcMsgRx.muiLen))
            {
              // Gibt es Rx Daten ? Ansonsten in direkt in Tx
              if (mcMsgRx.muiLen)
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


          // ----------------------- Slave Data -----------------------
          case StSlaveData_Rx_Data_WaitForMaster_RxStart:
            mSm = StSlaveData_Rx_Data_WaitForMaster_RxProcess;

            mu16RxIdx   = 0;
            lu8CheckSum = 1;

            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 1 + mcMsgRx.muiLen + 1; // 1+ für Identifier +1 für Checkcsumme
            if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width > 32)
            {
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 32;
            }

            // Initialisierung
            lui16Data2Copy = (mcMsgRx.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
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
                mcMsgRx.mpu8Data[mu16RxIdx + lui16DataIdx] = mcNRF905->mstNRF905.mui8RxPayLoad[lui16DataIdx + 1];
              }

              mu16RxIdx  += lui16Data2Copy;

              // Alle Daten empfangen
              if (mu16RxIdx < (mcMsgRx.muiLen + 1))   // +1 wg. Checksumme
              {

                lui16Data2Copy = (mcMsgRx.muiLen + 1) - mu16RxIdx;   // Len+1 für Checksumme
                if (lui16Data2Copy > 31) lui16Data2Copy = 31;
                mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui16Data2Copy + 1; // +1  für Identifier

                mSm = StSlaveData_Rx_Data_WaitForMaster_RxProcess;
                mcNRF905->vStartReceive();
              }
              else
              {
                // Empfangene Checksumme am berechnen
                for (lu8t = 0; lu8t < mcMsgRx.muiLen; lu8t++)
                {
                  lu8CheckSum += mcMsgRx.mpu8Data[lu8t];
                }

                if (mcMsgRx.mpu8Data[mcMsgRx.muiLen] == lu8CheckSum)
                {
                  #ifdef cNRF905_SYNC_DEBUG
                    mcUart.vSend((const char*)"Ok");
                  #endif

                  // Antworte mit sync
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
                  #ifdef cNRF905_SYNC_DEBUG
                    mcUart.vSend((const char*)"NOk");
                  #endif
                }

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

            // Gibt es Tx Daten ? Ansonsten empfangene Daten übernehmen
            if (mcMsgTx.muiLen)
            {
              mSm = StSlaveData_Tx_Data_TxStart;
            }
            break;

          case StSlaveData_Tx_Data_TxStart:
            // Ack gesendet, dann sende Daten
            mSm = StSlaveData_Tx_Data_TxProcess;

            lui16Data2Copy = (mcMsgTx.muiLen + 1) - mu16TxIdx;   // Len+1 für Checksumme
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
              mcNRF905->mstNRF905.mui8TxPayLoad[lui16DataIdx + 1] = mcMsgTx.mpu8Data[mu16TxIdx + lui16DataIdx];
            }
            mu16TxIdx += lui16Data2Copy;

            mcNRF905->vStartTransmit();
            break;

          case StSlaveData_Tx_Data_TxProcess:
            mSm = StSlaveData_Tx_Ack_WaitMaster;

            if (mu16TxIdx < (mcMsgTx.muiLen + 1))   // +1 wg. Checksumme
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
            mu8TxMsgCnt++;

            #ifdef cNRF905_SYNC_DEBUG
              mcUart.vSend("Sl_Cnf Data");
            #endif

            mu8LastTransmitValid = 1;
            if (mu8RxMsgCntLast != mu8RxMsgCnt)
            {
              u16 lu16DataIdx;

              for (lu16DataIdx = 0; lu16DataIdx < mcMsgRx.muiLen; lu16DataIdx++)
              {
                mcRingBufRx.put(mcMsgRx[lu16DataIdx]);
              }
            }
            mu8RxMsgCntLast  = mu8RxMsgCnt;

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


#endif // __cNRF905_SYNC_H
