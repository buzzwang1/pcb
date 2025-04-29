#include "I2cMem.h"

cI2cMem::cI2cMem(cI2cMaster* lpcI2C, uint8 lui8Adr)
{
  if (lui8Adr < I2CMEM_DEFAULT_ADDRESS)
  {
    lui8Adr <<= 1;
  }

  mI2C     = lpcI2C;
  mAdr     = lui8Adr;
  mAdrSync = lui8Adr;
  mAdrData = lui8Adr + (0x20 << 1);
  mAdrAck  = lui8Adr + (0x40 << 1);

  mpcMsgSync.vMemAlloc(2,  2);

  mpcMsgData.vMemAlloc(enCnstMaxData, enCnstMaxData);

  mpcMsgAck.vMemAlloc(1,  0);

  mcRxComBuf = (cComBuf*) new cComBuf(384);
  mcTxComBuf = (cComBuf*) new cComBuf(384);

  menCmd      = enCmdIdle;
  mbIsInit    = True;

  mbOnline     = False;
  mbLastSyncOk = True;
  mbThisSyncOk = True;

  mui16DataToTx = 0;

  mI2C->vAddSlave((cI2cMaster_Slave*)this);
}

cI2cMem::~cI2cMem()
{
  delete mcRxComBuf;
  delete mcTxComBuf;
}

void cI2cMem::vComError(cComNode::tenComNodeError lenError, cComNode::tenComNodeState lenState)
{

  if ((lenState == cComNode::tenComNodeState::enStAdress) &&
      (mAdr == mAdrSync))
  {
    mbOnline = False;
  }

  mbThisSyncOk = False;

  mui8StartRequest = 0;
  mbIsInit  = true;
  menCmd    = enCmdIdle;
}

void cI2cMem::vComStart()
{
  if (menCmd == enCmdIdle)
  {
    if (mbSync)
    {
      mbSync = false;
      menCmd = enCmdSync;
      vComDone();
    }
  }

  mui8StartRequest = (uint8)(mbSync);
}

void cI2cMem::vComDone()
{
  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdSync:
      {
        // Falls vorherige Daten übertragen werden konnten
        // dann nächste Daten laden
        // sonst vorherige Daten beibehalten
        mbLastSyncOk = mbThisSyncOk;
        mAdr = mAdrSync;
        if ((mui16DataToTx == 0) || (mbLastSyncOk))
        {
          mbThisSyncOk = False;
          // Überprüfen, obes Daten zu senden gibt
          mui16DataToTx = mcTxComBuf->cnt();
          if (mui16DataToTx > enCnstMaxData - 1) mui16DataToTx = enCnstMaxData - 1;

          if (mui16DataToTx)
          {
            mcTxComBuf->get(&mpcMsgData.cTxData.paui8Data[1], mui16DataToTx);

            mpcMsgData.cTxData.paui8Data[0] = 1;
            for (uint8 lui8t = 0; lui8t < mui16DataToTx; lui8t++)
            {
              mpcMsgData.cTxData.paui8Data[0] += mpcMsgData.cTxData.paui8Data[1+lui8t];
            }
          }
          mpcMsgSync.cTxData.paui8Data[0] = mui16DataToTx + 1;
          mpcMsgSync.cTxData.paui8Data[1] = mui16DataToTx;
        }
        else
        {
          mui16DataToTx = mpcMsgSync.cTxData.paui8Data[1];
        }

        menCmd = enCmdData;
        mpcMsgSync.vStart();
        mI2C->vStartMsg(&mpcMsgSync);
      }
      break;

    case enCmdData:
      {
        /* Rx-Len an Sync anpassen */
        mbOnline      = True;
        mui16DataToRx = mpcMsgSync.cRxData.paui8Data[1];

        //Checksumme ?
        if ((uint8)(mui16DataToRx + 1) == mpcMsgSync.cRxData.paui8Data[0])
        {
          if (mui16DataToRx > enCnstMaxData - 1) mui16DataToRx = enCnstMaxData - 1;

          if ((mui16DataToRx) || (mui16DataToTx))
          {
            mAdr = mAdrData;

            if (mui16DataToTx)
            {
              mpcMsgData.cTxData.ui32DataSize = mui16DataToTx + 1;
            }
            else
            {
              mpcMsgData.cTxData.ui32DataSize = 0;
            }

            if (mui16DataToRx)
            {
              mpcMsgData.cRxData.ui32DataSize = mui16DataToRx + 1;
            }
            else
            {
              mpcMsgData.cRxData.ui32DataSize = 0;
            }

            menCmd = enCmdAck;

            mpcMsgData.vStart();
            mI2C->vStartMsg(&mpcMsgData);
          }
          else
          {
            // Nichts zum Austauschen, dann in idle
            mbThisSyncOk = True;
            menCmd = enCmdIdle;
          }
        }
        else
        {
          //Checksumme schlecht
          menCmd = enCmdIdle;
        }
      }

      break;

    case enCmdAck:
      mAdr = mAdrAck;
      mpcMsgAck.cTxData.paui8Data[0] = 0x0F; // Ack Kennung

      if (mui16DataToRx)
      {
        uint8 lui8ChkSum;

        lui8ChkSum = 1;
        for (uint8 lui8t = 0; lui8t < mui16DataToRx; lui8t++)
        {
          lui8ChkSum += mpcMsgData.cRxData.paui8Data[1+lui8t];
        }

        if (mpcMsgData.cRxData.paui8Data[0] != lui8ChkSum)
        {
          mpcMsgAck.cTxData.paui8Data[0] = 0xF0; // Nack Kennung
        }
      }

      menCmd = enCmdEnd;
      mpcMsgAck.vStart();
      mI2C->vStartMsg(&mpcMsgAck);
      break;

    case enCmdEnd:
      menCmd       = enCmdIdle;
      mbThisSyncOk = True;
      /* Rx Daten abholen und in den Rx-Buffer schreiben */
      if (mui16DataToRx)
      {
        mcRxComBuf->put(&mpcMsgData.cRxData.paui8Data[1], mui16DataToRx);
      }
      break;

    default:
      menCmd = enCmdIdle;
      break;
  }
}

void cI2cMem::vSync()
{
  mbSync           = true;
  mui8StartRequest = 1;
}





