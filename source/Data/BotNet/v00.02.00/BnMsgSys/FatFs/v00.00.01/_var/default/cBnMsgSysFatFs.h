#ifndef __BOTNET_MESSAGE_SYS_FATFS_H__
#define __BOTNET_MESSAGE_SYS_FATFS_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cJobHdl.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"
#include "cFsManager.h"

class cBotNetMsgPortFatFs : public cJobHandler, public cBotNet_MsgSysProcess
{
  public:
  enum class cJobs : u8
  {
    nJobNop = (u8)cJobHandler::cJobs::nLast,
    nJobRead,
    nJobWrite,
    nJobCmd,
  };

  u8               mu8MsgTx_Buf[cBotNet_MsgSize];
  cBarryPtrT<u16>  mcTxMsgTx;
  cBotNetAdress    mcTxDAdr;

  cFsJob*          mpcFsJob;
  FATFS*           mpstFs;
  cBarryPtrT<u16>  mcTransfer;
  bool             mbOpen;
  u8               mu8MsgIdx;

  cBotNetMsgPortFatFs(cBotNet* lcBotNet, cFsJob* lpcFsJob, FATFS* lpstFs)
    : cJobHandler(), cBotNet_MsgSysProcess(lcBotNet), 
      mcTxMsgTx(mu8MsgTx_Buf, cBotNet_MsgSize)
  {
    mbOpen   = False;
    mpcFsJob = lpcFsJob;
    mpstFs   = lpstFs;

    mpcFsJob->vSetFatFs(lpstFs);
  }

  bool isOpen() {return mbOpen;}

  bool bMsg(cBotNetMsg_MsgProt& lcMsg) override;

  void vProcess(u16 lu16TimeDiff_ms);
  void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_FATFS_H__
