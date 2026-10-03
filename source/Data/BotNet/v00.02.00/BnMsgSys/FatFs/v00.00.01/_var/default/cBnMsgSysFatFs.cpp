#include "cBnMsgSysFatFs.h"
#include "cBotnet.h"


bool cBotNetMsgPortFatFs::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  if (lcMsg.u16GetIdx() != 7)
    return false;

  if (isBusy())
    return False;

  u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
  mcTxDAdr = lcMsg.cGetSAdr();

  if (isOpen())
  {
    switch (lcMsg.u8GetId())
    {
      case lcMsg.cId8Set(0x04): // BlockRead
      {
        bool lbOk = False;
        u8 lu8MsgSysSpaceLeft = 0;
        u8  lu8FlwCtr = lpu8PayloadRx[1];
        mu8MsgIdx = 0;

        if (lu8FlwCtr & 1) // SOT: Start of Transmission
        {
          u16 lu16DataIdx = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 3));

          mpcFsJob->cGetDataRead(mcTransfer, lu16DataIdx);

          if (mcTransfer.Len() != 0) // Posetiv response
          {
            mcTransfer.Size(mcTransfer.Len());
            mcTransfer.Len(0);

            lbOk = True;
            {
              u16 lu16DataToSend = mcTransfer.SpaceLeft();
              if (lu16DataToSend > 40)
              {
                lu16DataToSend = 40;
              }
              else
              {
                mu8MsgIdx = 0xFF;
              }

              //                                     Resp. Idx       FC    MI
              //                                                     FF
              u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x04), 0x01, mu8MsgIdx };
              mcTxMsgTx.Set(lau8Resp, sizeof(lau8Resp));

              mcTxMsgTx.Addu16Com(mcTransfer.Size()); // DCDC: 16Bit: Count of all Data to be transfered: only at SOT
              mcTxMsgTx.Addu16Com(0xCCCC);            // CSCS: 16Bit: Checksum of the data to be transfered: only at SOT and SOB

              mcTransfer.Take((mcTxMsgTx.Data() + mcTxMsgTx.Len()), lu16DataToSend);
              mcTxMsgTx.Len(mcTxMsgTx.Len() + lu16DataToSend);

              lu8MsgSysSpaceLeft = u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)mcTxMsgTx.Data(), mcTxMsgTx.Len());
            }
          }
        }
        else if (lu8FlwCtr & 2) // SOB: Start of Block
        {
          u16 lu16DataOft = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 3));

          if (lu16DataOft <= mcTransfer.Size())
          {
            mcTransfer.Len(lu16DataOft);

            if (mcTransfer.SpaceLeft()) // Posetiv response
            {
              lbOk = True;
              {
                u16 lu16DataToSend = mcTransfer.SpaceLeft();
                if (lu16DataToSend > 40)
                {
                  lu16DataToSend = 40;
                }
                else
                {
                  mu8MsgIdx = 0xFF;
                }

                //                                     Resp. Idx       FC    MI
                //                                                     CF
                u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x04), 0x02, mu8MsgIdx };
                mcTxMsgTx.Set(lau8Resp, sizeof(lau8Resp));

                mcTxMsgTx.Addu16Com(0xCCCC);            // CSCS: 16Bit: Checksum of the data to be transfered: only at SOT and SOB

                mcTransfer.Take((mcTxMsgTx.Data() + mcTxMsgTx.Len()), lu16DataToSend);
                mcTxMsgTx.Len(mcTxMsgTx.Len() + lu16DataToSend);

                lu8MsgSysSpaceLeft = u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)mcTxMsgTx.Data(), mcTxMsgTx.Len());
              }
            }
          }
        }

        if (lbOk)
        {
          // Versuchen direkt vier weiter Data-Nachrichten abzuschicken
          for (u8 lu8MsgCnt = 0; lu8MsgCnt < 4; lu8MsgCnt++)
          {
            if (mcTransfer.SpaceLeft() == 0) break;
            mu8MsgIdx++;

            u16 lu16DataToSend = mcTransfer.SpaceLeft();
            if (lu16DataToSend > 48)
            {
              lu16DataToSend = 48;
            }
            else
            {
              mu8MsgIdx = 0xFF;
            }

            //                                           Resp. Idx       FC    MI
            u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x04),       0x00, mu8MsgIdx};
            mcTxMsgTx.Set(lau8Resp, sizeof(lau8Resp));

            mcTransfer.Take((mcTxMsgTx.Data() + mcTxMsgTx.Len()), lu16DataToSend);
            mcTxMsgTx.Len(mcTxMsgTx.Len() + lu16DataToSend);

            lu8MsgSysSpaceLeft = u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)mcTxMsgTx.Data(), mcTxMsgTx.Len());
            if (!lu8MsgSysSpaceLeft)
            {
              // Kein Platz mehr im Nachrichtenpuffer
              break;
            };
          }

          if (mcTransfer.SpaceLeft())
          {
            cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobRead);
          }
        }
        else // Negativ response
        {
          const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x04), 0x00, 0x00, FR_INVALID_PARAMETER };
          u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
        }

        return True; // Consumed
      }
      break;

      case lcMsg.cId8Set(0x05): // BlockWrite
      {
        u8  lu8FlwCtr = lpu8PayloadRx[1];
        u8  lu8MsgIdx = lpu8PayloadRx[2];

        if (lu8FlwCtr & 1) // SOT: Start of Transmission
        {
          u16 lu16DataCnt = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 3));
          u16 lu16DataIdx = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 5));
          //u16 lu16DataChk = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 7));

          mpcFsJob->cGetDataWrite(mcTransfer, lu16DataIdx);

          if ((lu16DataCnt != 0) && (lu16DataCnt <= mcTransfer.Size())) // Posetiv response
          {
            mcTransfer.Size(lu16DataCnt);

            u16 lu16DataInMsg = lcMsg.mcPayload.Len() - 9;
            mcTransfer.Set((u8*)(lpu8PayloadRx + 9), lu16DataInMsg);

            if (lu16DataInMsg == lu16DataCnt) // Alle Daten in einer Nachricht ?
            {
              const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, FR_OK };
              u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            }
            else  // Es kommt nocht mehr
            {
            }
          }
          else // Negativ response
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, FR_INVALID_PARAMETER };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
          }
        }
        else if (lu8FlwCtr & 2) // SOB: Start of Block
        {
          //u16 lu16DataChk = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 3));
          u16 lu16DataOft = cMemTools::u16U8toU16((u8*)(lpu8PayloadRx + 5));

          mcTransfer.Len(lu16DataOft);

          u16 lu16DataInMsg = lcMsg.mcPayload.Len() - 7;
          mcTransfer.Set((u8*)(lpu8PayloadRx + 7), lu16DataInMsg);

          if ((mcTransfer.Len() + lu16DataInMsg) == mcTransfer.Size()) // Alle Daten in einer Nachricht ?
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, FR_OK };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
          }
          else  // Es kommt nocht mehr
          {
          }
        }
        else // Data Frame
        {
          u16 lu16DataInMsg = lcMsg.mcPayload.Len() - 3;

          if ((mcTransfer.Len() + lu16DataInMsg) <= mcTransfer.Size()) // Daten passen ?
          {
            mcTransfer.Add((u8*)(lpu8PayloadRx + 3), lu16DataInMsg);

            if (((mcTransfer.Len()) == mcTransfer.Size()) ||
                 (lu8MsgIdx == 0xFF)) // Alle Daten empfangen ?
            {
              const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, FR_OK };
              u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            }
            else  // Es kommt nocht mehr
            {
            }
          }
          else
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, FR_INVALID_PARAMETER };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
          }
        }

        return True; // Consumed
      }
      break;
    }


    switch (lcMsg.u32GetId())
    {
      // 02 SI 00:  Request Service: SI: Service Index
      case lcMsg.cId24(0x02, 0x00, 0x00): // Cmd Request Nop
        {
          const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x00, 0x00, FR_OK, 0x00 };
          u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
          return True;
        }
        break;

      case lcMsg.cId24(0x02, 0x01, 0x00): // Cmd Request Mkfs
        {
          if (lcMsg.GetPayload().Len() < (3 + 1 + 1 + 4 + 4 + 4))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x01, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstMkfs lstParam;

          lstParam.opt.fmt     = lpu8PayloadRx[3];  /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
          lstParam.opt.n_fat   = lpu8PayloadRx[4];  /* Number of FATs */
          lstParam.opt.align   = cMemTools::u32U8toU32(lpu8PayloadRx + 5);  /* Data area alignment (sector) */
          lstParam.opt.n_root  = cMemTools::u32U8toU32(lpu8PayloadRx + 9);  /* Number of root directory entries */
          lstParam.opt.au_size = cMemTools::u32U8toU32(lpu8PayloadRx + 13);  /* Cluster size (byte) */

          // Todo 
          mpcFsJob->cGetDataWrite(mcTransfer, 7);
          lstParam.work = mcTransfer.Data(); //(void*)cMemTools::u32U8toU32(lpu8PayloadRx + 17);  // Pointer to working buffer (null: use len bytes of heap memory)
          lstParam.len  = mcTransfer.Size(); // cMemTools::u32U8toU32(lpu8PayloadRx + 21);  // Size of working buffer [byte]
          
          if (lcMsg.GetPayload().Len() > (3 + 1 + 1 + 4 + 4 + 4))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3 + 1 + 1 + 4 + 4 + 4)), lcMsg.GetPayload().Len() - (3 + 1 + 1 + 4 + 4 + 4));
          }

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;

          mpcFsJob->vStartMkfs(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x02, 0x00): // Cmd Request Mount
        {
          if (lcMsg.GetPayload().Len() < (3 + 1))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x02, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstMount lstParam;
  
          lstParam.fs = mpstFs;   // Pointer to the filesystem object to be registered (NULL:unmount)
          mpcFsJob->mau8PathMem1[0] = 0; // = ""
          lstParam.path = (char*)mpcFsJob->mau8PathMem1;
          lstParam.opt = lpu8PayloadRx[3];  // Mount option: 0=Do not mount (delayed mount), 1=Mount immediately

          if (lcMsg.GetPayload().Len() > (3 + 1))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3 + 1)), lcMsg.GetPayload().Len() - (3 + 1));
          }

          mpcFsJob->vStartMount(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x03, 0x00): // Cmd Request Getfree
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x03, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstGetfree lstParam;

          mpcFsJob->mau8PathMem1[0] = 0; // = ""
          lstParam.path = (char*)mpcFsJob->mau8PathMem1; // Logical drive number
          //lstParam.nclst =        // Pointer to a variable to return number of free clusters
          lstParam.fatfs = &mpstFs; // Pointer to a pointer to return corresponding filesystem object

          if (lcMsg.GetPayload().Len() > (3))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3)), lcMsg.GetPayload().Len() - (3));
          }

          mpcFsJob->vStartGetfree(&lstParam);
          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x04, 0x00): // Cmd Request Mkdir
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x04, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstMkdir lstParam;

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;

          if (lcMsg.GetPayload().Len() > (3))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3)), lcMsg.GetPayload().Len() - (3));
          }

          mpcFsJob->vStartMkdir(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x05, 0x00): // Cmd Request Opendir
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x05, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstOpendir lstParam;

          lstParam.dp   = &mpcFsJob->mDir;
          lstParam.path = (char*)mpcFsJob->mau8PathMem1;

          if (lcMsg.GetPayload().Len() > (3))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3)), lcMsg.GetPayload().Len() - (3));
          }

          mpcFsJob->vStartOpendir(&lstParam);
          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x06, 0x00): // Cmd Request Readdir
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x06, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstReaddir lstParam;

          lstParam.dp   = &mpcFsJob->mDir;
          lstParam.fno  = &mpcFsJob->mFileInfo;

          mpcFsJob->vStartReaddir(&lstParam);
          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x07, 0x00): // Cmd Request Closedir
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x07, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstClosedir lstParam;

          lstParam.dp   = &mpcFsJob->mDir;

          mpcFsJob->vStartClosedir(&lstParam);
          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x08, 0x00): // Cmd Request Stat
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x08, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstStat lstParam;

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;    // Pointer to the file path
          lstParam.fno = &mpcFsJob->mFileInfo; // Pointer to file information to return

          if (lcMsg.GetPayload().Len() > (3))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3)), lcMsg.GetPayload().Len() - (3));
          }

          mpcFsJob->vStartStat(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x09, 0x00): // Cmd Request Open
        {
          if (lcMsg.GetPayload().Len() < (3 + 1))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x09, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstOpen lstParam;

          lstParam.fp  = &mpcFsJob->mFile;               // Pointer to the blank file object
          lstParam.path = (char*)mpcFsJob->mau8PathMem1; // Pointer to the file name
          lstParam.mode = lpu8PayloadRx[3];              // Access mode and open mode flags
                                                         // - FA_OPEN_EXISTING  0x00
                                                         // - FA_READ           0x01
                                                         // - FA_WRITE          0x02
                                                         // - FA_CREATE_NEW     0x04
                                                         // - FA_CREATE_ALWAYS  0x08
                                                         // - FA_OPEN_ALWAYS    0x10
                                                         // - FA_OPEN_APPEND    0x30

          if (lcMsg.GetPayload().Len() > (3 + 1))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3 + 1)), lcMsg.GetPayload().Len() - (3 + 1));
          }

          mpcFsJob->vStartOpen(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0A, 0x00): // Cmd Request Truncate
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0A, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstTruncate lstParam;

          lstParam.fp  = &mpcFsJob->mFile;   // Pointer to the file object 

          mpcFsJob->vStartTruncate(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0B, 0x00): // Cmd Request Lseek
        {
          if (lcMsg.GetPayload().Len() != (3+4))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0B, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstLseek lstParam;

          lstParam.fp = &mpcFsJob->mFile;   // Pointer to the file object 
          lstParam.ofs = cMemTools::u32U8toU32(lpu8PayloadRx + 3);  // File pointer from top of file

          mpcFsJob->vStartLseek(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0C, 0x00): // Cmd Request Write
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0C, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstWrite lstParam;

          lstParam.fp = &mpcFsJob->mFile;   // Open file to be written
          lstParam.buff = (void*)mpcFsJob->mau8TrfMem;  // Data to be written
          lstParam.btw = mcTransfer.Len();
          //lstParam.bw;    // Number of bytes written

          mpcFsJob->vStartWrite(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0D, 0x00): // Cmd Request Read
        {
          if (lcMsg.GetPayload().Len() != (3 + 4))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0D, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstRead lstParam;

          lstParam.fp = &mpcFsJob->mFile;                           // Open file to be read
          lstParam.buff = (void*)mpcFsJob->mau8TrfMem;              // Data buffer to store the read data
          lstParam.btr  = cMemTools::u32U8toU32(lpu8PayloadRx + 3); // Number of bytes to read
          //lstParam.br;    // Number of bytes read

          mpcFsJob->vStartRead(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0E, 0x00): // Cmd Request Close
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0E, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstClose lstParam;

          lstParam.fp = &mpcFsJob->mFile;   // Open file to be closed

          mpcFsJob->vStartClose(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x0F, 0x00): // Cmd Request Sync
        {
          if (lcMsg.GetPayload().Len() != (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x0F, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstSync lstParam;

          lstParam.fp = &mpcFsJob->mFile;   // Open file to be synced

          mpcFsJob->vStartSync(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x10, 0x00): // Cmd Request Unlink
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x10, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstUnlink lstParam;

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;    // Pointer to the file or directory path

          if (lcMsg.GetPayload().Len() > (3))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (3)), lcMsg.GetPayload().Len() - (3));
          }

          mpcFsJob->vStartUnlink(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x11, 0x00): // Cmd Request Chmod
        {
          if (lcMsg.GetPayload().Len() < (5))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x11, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstChmod lstParam;

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;   // Pointer to the file path
          lstParam.attr = lpu8PayloadRx[3];    // Attribute bits to set/clear
          lstParam.mask = lpu8PayloadRx[4];    // Attribute mask to change

          if (lcMsg.GetPayload().Len() > (5))
          {
            cMemTools::vMemCpy((u8*)mpcFsJob->mau8PathMem1, (lpu8PayloadRx + (5)), lcMsg.GetPayload().Len() - (5));
          }

          mpcFsJob->vStartChmod(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x12, 0x00): // Cmd Request Utime
        {
          if (lcMsg.GetPayload().Len() < (3))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x12, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstUtime lstParam;

          lstParam.path = (char*)mpcFsJob->mau8PathMem1;  // Pointer to the file/directory name
          lstParam.fno = &mpcFsJob->mFileInfo;    // Timestamp to be set

          mpcFsJob->vStartUtime(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x13, 0x00): // Cmd Request Rename
        {
          if (lcMsg.GetPayload().Len() != 3)
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x13, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstRename lstParam;

          lstParam.path_old = (char*)mpcFsJob->mau8PathMem1;  // Pointer to the object name to be renamed
          lstParam.path_new = (char*)mpcFsJob->mau8PathMem2;  // Pointer to the new name

          mpcFsJob->vStartRename(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;

      case lcMsg.cId24(0x02, 0x14, 0x00): // Cmd Request Expand
        {
          if (lcMsg.GetPayload().Len() != (3 + 5))
          {
            const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), 0x14, 0x00, FR_INVALID_PARAMETER, 0x00 };
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
            return True;
          }

          cFsJob::tstExpand lstParam;

          lstParam.fp  = &mpcFsJob->mFile;      // Pointer to the file object
          lstParam.fsz = cMemTools::u32U8toU32(lpu8PayloadRx + 3);      // File size to be expanded to
          lstParam.opt = lpu8PayloadRx[7];      // Operation mode 0:Find and prepare or 1:Find and allocate

          mpcFsJob->vStartExpand(&lstParam);

          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCmd);
        }
        break;
    }

    if (lcMsg.u8GetId() == lcMsg.cId8Set(2))
    {
      if (lpu8PayloadRx[1] <= 0x14)
      {
        // Antwort vorbereiten
        u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x02), lpu8PayloadRx[1], 0x00 };
        mcTxMsgTx.Set(lau8Resp, sizeof(lau8Resp));
      }
      return True; // Consumed
    }
  }

  // Open
  if (lcMsg.isSetId(0x00, 0x00, 0x00))
  {
    if (cMemTools::u32U8toU32(lpu8PayloadRx + 3) == 0x01020304)
    {
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, FR_OK };
      mbOpen = True;
      u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
    }
    else
    {
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, FR_INVALID_PARAMETER };
      u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)lau8Resp, sizeof(lau8Resp));
    }
  }

  return True; // Consumed
}


void cBotNetMsgPortFatFs::vProcess(u16 lu16TimeDiff_ms)
{
  UNUSED(lu16TimeDiff_ms);

  switch ((cBotNetMsgPortFatFs::cJobs)mcJob)
  {
    case cJobs::nJobRead:
      {
        u8 lu8MsgSysSpaceLeft;
        // Versuchen direkt vier weiter Data-Nachrichten abzuschicken
        for (u8 lu8MsgCnt = 0; lu8MsgCnt < 4; lu8MsgCnt++)
        {
          if (mcTransfer.SpaceLeft() == 0) break;
          mu8MsgIdx++;

          u16 lu16DataToSend = mcTransfer.SpaceLeft();
          if (lu16DataToSend > 48)
          {
            lu16DataToSend = 48;
          }
          else
          {
            mu8MsgIdx = 0xFF;
          }

          if (mu8MsgIdx >= 12) mu8MsgIdx = 0xFF;

          //                                           Resp. Idx       FC    MI
          const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x04), 0x00, mu8MsgIdx };
          mcTxMsgTx.Set(lau8Resp, sizeof(lau8Resp));

          mcTransfer.Take((mcTxMsgTx.Data() + mcTxMsgTx.Len()), lu16DataToSend);
          mcTxMsgTx.Len(mcTxMsgTx.Len() + lu16DataToSend);

          lu8MsgSysSpaceLeft = u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)mcTxMsgTx.Data(), mcTxMsgTx.Len());
          
          if ((!lu8MsgSysSpaceLeft) || (mu8MsgIdx >= 12))
          {
            // Kein Platz mehr im Nachrichtenpuffer
            break;
          };
        }

        if (!mcTransfer.SpaceLeft() || (mu8MsgIdx >= 12))
        {
          vFinished();
        }
      }
      break;

    case cJobs::nJobCmd:
      if (mpcFsJob->isReady())
      {
        // Antwort wurde schon beim Request vorbereitet
        mcTxMsgTx.Add(mpcFsJob->mResFatFs);
        mcTxMsgTx.Add(mpcFsJob->mResDataChanged);
        mcTxMsgTx.Add(mpcFsJob->mcResData);

        u8PutInt(mcBn->mcAdr, mcTxDAdr, 7, (u8*)mcTxMsgTx.Data(), mcTxMsgTx.Len());
        vFinished();
      }
      break;


    default:
      vFinished();
  }
}

void cBotNetMsgPortFatFs::vTick10ms()
{
  vProcess(10);
}

