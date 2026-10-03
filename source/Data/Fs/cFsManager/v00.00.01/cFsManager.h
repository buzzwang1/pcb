#pragma once

#include "Typedef.h"
#include "cJobHdl.h"
#include "cStrT.h"
#include "ff.h"


class cFsJob : public cJobHandler
{
  public:
  enum struct cJobs : u8
  {
    nMkfs = (u8)cJobHandler::cJobs::nLast,
    nMount,
    nGetfree,

    nMkdir,
    nOpendir,
    nReaddir,
    nClosedir,

    nStat,
    nOpen,
    nTruncate,
    nLseek,
    nWrite,
    nRead,
    nClose,
    nSync,
    nUnlink,
    nChmod,
    nUtime,
    nRename,
    nExpand,
    nLast
  };

  enum struct tstMems : u8
  {
    nMemFs,
    nMemFile,
    nMemDir,
    nMemFileInfo,
    nMemCmdMem,
    nMemPathMem1,
    nMemPathMem2,
    nMemTrfMem,
  };

  struct tstMkfs
  {
    char*      path;   // Logical drive number
    MKFS_PARM  opt;    // Format options
    void*      work;   // Pointer to working buffer (null: use len bytes of heap memory)
    u32        len;    // Size of working buffer [byte]
  };

  struct tstMount
  {
    FATFS* fs;   // Pointer to the filesystem object to be registered (NULL:unmount)
    char*  path; // Logical drive number to be mounted/unmounted
    u8     opt;  // Mount option: 0=Do not mount (delayed mount), 1=Mount immediately
  };

  struct tstGetfree
  {
    char*   path;   // Logical drive number
    u32     nclst;  // Pointer to a variable to return number of free clusters
    FATFS** fatfs;  // Pointer to a pointer to return corresponding filesystem object
  };

  struct tstMkdir
  {
    char* path;     // Pointer to the directory path
  };

  struct tstOpendir
  {
    DIR*  dp;    // Pointer to directory object to create
    char* path;  // Pointer to the directory path
  };

  struct tstReaddir
  {
    DIR*      dp;    // Pointer to the open directory object
    FILINFO* fno;    // Pointer to file information to return
  };

  struct tstClosedir
  {
    DIR* dp;         // Pointer to the directory object to be closed
  };

  struct tstStat
  {
    char*    path;   // Pointer to the file path
    FILINFO* fno;    // Pointer to file information to return
  };

  struct tstOpen
  {
    FIL*  fp;      // Pointer to the blank file object
    char* path;    // Pointer to the file name
    u8    mode;    // Access mode and open mode flags
  };

  struct tstTruncate
  {
    FIL* fp;      // Pointer to the file object
  };

  struct tstLseek
  {
    FIL*    fp;   // Pointer to the file object
    FSIZE_t ofs;  // File pointer from top of file
  };

  struct tstWrite
  {
    FIL*  fp;    // Open file to be written
    void* buff;  // Data to be written
    UINT  btw;   // Number of bytes to write
    UINT  bw;    // Number of bytes written
  };

  struct tstRead
  {
    FIL* fp;     // Open file to be read
    void* buff;  // Data buffer to store the read data
    UINT btr;    // Number of bytes to read
    UINT br;     // Number of bytes read
  };

  struct tstClose
  {
    FIL* fp;    // Open file to be closed
  };

  struct tstSync
  {
    FIL* fp;    // Open file to be synced
  };

  struct tstUnlink
  {
    char* path;    //Pointer to the file or directory path
  };

  struct tstChmod
  {
    char* path;   // Pointer to the file path
    BYTE attr;    // Attribute bits to set/clear
    BYTE mask;    // Attribute mask to change
  };

  struct tstUtime
  {
    char* path;    // Pointer to the file/directory name
    FILINFO* fno;  // Timestamp to be set
  };

  struct tstRename
  {
    char* path_old;   // Pointer to the object name to be renamed
    char* path_new;   // Pointer to the new name
  };

  struct tstExpand
  {
    FIL* fp;       // Pointer to the file object
    FSIZE_t fsz;  // File size to be expanded to
    BYTE opt;     // Operation mode 0:Find and prepare or 1:Find and allocate
  };


  FATFS*  mpFs;
  FIL     mFile;
  DIR     mDir;
  FILINFO mFileInfo;

  u8      mau8CmdMem[32];
  u8      mau8PathMem1[256];
  u8      mau8PathMem2[256];
  u8      mau8TrfMem[4096];

  BYTE    mResFatFs;
  u8      mResDataChanged;
  u8      mau8ResData[32];

  cBarryPtrT<u16> mcPath1;
  cBarryPtrT<u16> mcPath2;
  cBarryPtrT<u16> mcTrf;
  cBarryPtrT<u16> mcResData;

  cFsJob()
    : mcPath1((u8*)mau8PathMem1,  sizeof(mau8PathMem1)),
      mcPath2((u8*)mau8PathMem2,  sizeof(mau8PathMem2)),
      mcTrf((u8*)mau8TrfMem,      sizeof(mau8TrfMem)),
      mcResData((u8*)mau8ResData, sizeof(mau8ResData))
  {
  }

  void vSetFatFs(FATFS* lpFs)
  {
    mpFs = lpFs;
  }


  cBarryPtrT<u16>& cGetPath()    { return mcPath1; }
  cBarryPtrT<u16>& cGetPath1()   { return mcPath1; }
  cBarryPtrT<u16>& cGetPath2()   { return mcPath2; }
  cBarryPtrT<u16>& cGetTrfMem()  { return mcTrf; }
  cBarryPtrT<u16>& cGetResData() { return mcResData; }

  void cGetDataWrite(cBarryPtrT<u16>& lcPtr, u8 lu8Idx)
  {
    switch (lu8Idx)
    {
      case 0: lcPtr.From((u8*)mpFs,       0, sizeof(FATFS) - FF_MAX_SS); break;
      case 1: lcPtr.From((u8*)&mFile,     0, sizeof(FIL)   - FF_MAX_SS); break;
      case 2: lcPtr.From((u8*)&mDir,      0, sizeof(DIR)); break;
      case 3: lcPtr.From((u8*)&mFileInfo, 0, sizeof(FILINFO)); break;
      case 4: lcPtr.From((u8*)mau8CmdMem, 0, sizeof(mau8CmdMem)); break;
      case 5: lcPtr.From((u8*)&mau8PathMem1, 0, sizeof(mau8PathMem1)); break;
      case 6: lcPtr.From((u8*)&mau8PathMem2, 0, sizeof(mau8PathMem2)); break;
      case 7: lcPtr.From((u8*)&mau8TrfMem,   0, sizeof(mau8TrfMem)); break;
    }
  }

  void cGetDataRead(cBarryPtrT<u16>& lcPtr, u8 lu8Idx)
  {
    switch (lu8Idx)
    {
      case 0: lcPtr.From((u8*)mpFs,       sizeof(FATFS) - FF_MAX_SS, sizeof(FATFS) - FF_MAX_SS); break;
      case 1: lcPtr.From((u8*)&mFile,     sizeof(FIL) - FF_MAX_SS,   sizeof(FIL)   - FF_MAX_SS); break;
      case 2: lcPtr.From((u8*)&mDir,      sizeof(DIR),               sizeof(DIR)); break;
      case 3: lcPtr.From((u8*)&mFileInfo, sizeof(FILINFO),           sizeof(FILINFO)); break;
      case 4: lcPtr.From((u8*)mau8CmdMem, sizeof(mau8CmdMem),        sizeof(mau8CmdMem)); break;
      case 5: lcPtr.From(mcPath1); break;
      case 6: lcPtr.From(mcPath2); break;
      case 7: lcPtr.From(mcTrf); break;
    }
  }



  void vStartMkfs(tstMkfs* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstMkfs));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nMkfs);
  }

  void vStartMount(tstMount* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstMount));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nMount);
  }

  void vStartGetfree(tstGetfree* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstGetfree));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nGetfree);
  }

  void vStartMkdir(tstMkdir* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstMkdir));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nMkdir);
  }

  void vStartOpendir(tstOpendir* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstOpendir));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nOpendir);
  }

  void vStartReaddir(tstReaddir* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstReaddir));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nReaddir);
  }

  void vStartClosedir(tstClosedir* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstClosedir));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nClosedir);
  }

  void vStartStat(tstStat* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstStat));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nStat);
  }

  void vStartOpen(tstOpen* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstOpen));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nOpen);
  }

  void vStartTruncate(tstTruncate* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstTruncate));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nTruncate);
  }

  void vStartLseek(tstLseek* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstLseek));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nLseek);
  }

  void vStartWrite(tstWrite* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstWrite));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nWrite);
  }

  void vStartRead(tstRead* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstRead));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nRead);
  }

  void vStartClose(tstClose* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstClose));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nClose);
  }

  void vStartSync(tstSync* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstSync));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nGetfree);
  }

  void vStartUnlink(tstUnlink* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstUnlink));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nUnlink);
  }

  void vStartChmod(tstChmod* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstChmod));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nChmod);
  }

  void vStartUtime(tstUtime* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstUtime));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nUtime);
  }

  void vStartRename(tstRename* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstRename));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nRename);
  }

  void vStartExpand(tstExpand* lstCmd)
  {
    cMemTools::vMemCpy(mau8CmdMem, (u8*)lstCmd, sizeof(tstExpand));
    cJobHandler::vStart((cJobHandler::cJobs)cJobs::nExpand);
  }

  void vProcess(u16 lu16TimeDiff_ms) override
  {
    UNUSED(lu16TimeDiff_ms);
    //cJobHandler::vProcess(lu16TimeDiff_ms);

    //if (!cJobHandler::isBusy())
    //{
    //}
    //else
    {
      switch ((cJobs)mcJob)
      {
        case cJobs::nMkfs:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstMkfs* lpstCmd = (tstMkfs*)mau8CmdMem;
                  mResFatFs = f_mkfs(lpstCmd->path, &lpstCmd->opt, lpstCmd->work, lpstCmd->len);

                  mResDataChanged = 0;
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nMount:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstMount* lpstCmd = (tstMount*)mau8CmdMem;
                  mResFatFs = f_mount(lpstCmd->fs, lpstCmd->path, lpstCmd->opt);

                  mResDataChanged = (1 << (u8)tstMems::nMemFs);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nGetfree:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstGetfree* lpstCmd = (tstGetfree*)mau8CmdMem;
                  DWORD nclst;
                  mResFatFs = f_getfree(lpstCmd->path, &nclst, lpstCmd->fatfs);
                  lpstCmd->nclst = (u32)nclst;

                  mResDataChanged = 0;
                  nclst *= FF_MAX_SS;
                  mcResData.Setu32Com(nclst);

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nMkdir:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstMkdir* lpstCmd = (tstMkdir*)mau8CmdMem;
                  mResFatFs = f_mkdir(lpstCmd->path);
                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nOpendir:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstOpendir* lpstCmd = (tstOpendir*)mau8CmdMem;
                  mResFatFs = f_opendir(lpstCmd->dp, lpstCmd->path);

                  mResDataChanged = (1 << (u8)tstMems::nMemDir);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nReaddir:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstReaddir* lpstCmd = (tstReaddir*)mau8CmdMem;

                  cMemTools::vMemSet((u8*)lpstCmd->fno, 0, sizeof(FILINFO));
                  mResFatFs = f_readdir(lpstCmd->dp, lpstCmd->fno);

                  mResDataChanged = (1 << (u8)tstMems::nMemFileInfo);

                  if ((mResFatFs != FR_OK) || (lpstCmd->fno->fname[0] == 0))
                  {
                    mcResData.Set(0);
                  }
                  else
                  {
                    mcResData.Set(1);
                  }

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nClosedir:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstClosedir* lpstCmd = (tstClosedir*)mau8CmdMem;
                  mResFatFs = f_closedir(lpstCmd->dp);

                  mResDataChanged = (1 << (u8)tstMems::nMemDir);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nStat:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstStat* lpstCmd = (tstStat*)mau8CmdMem;

                  cMemTools::vMemSet((u8*)lpstCmd->fno, 0, sizeof(FILINFO));
                  mResFatFs = f_stat(lpstCmd->path, lpstCmd->fno);

                  mResDataChanged = (1 << (u8)tstMems::nMemFileInfo);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nOpen:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstOpen* lpstCmd = (tstOpen*)mau8CmdMem;
                  mResFatFs = f_open(lpstCmd->fp, lpstCmd->path, lpstCmd->mode);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nTruncate:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstTruncate* lpstCmd = (tstTruncate*)mau8CmdMem;
                  mResFatFs = f_truncate(lpstCmd->fp);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        case cJobs::nLseek:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstLseek* lpstCmd = (tstLseek*)mau8CmdMem;
                  mResFatFs = f_lseek(lpstCmd->fp, lpstCmd->ofs);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nWrite:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstWrite* lpstCmd = (tstWrite*)mau8CmdMem;
                  UINT luBtw;
                  UINT luBw;

                  if (lpstCmd->btw < mcTrf.Size())
                  {
                    luBtw = lpstCmd->btw;
                  }
                  else
                  {
                    luBtw = mcTrf.Size();
                  }

                  mResFatFs = f_write(lpstCmd->fp, lpstCmd->buff, luBtw, &luBw);

                  lpstCmd->bw = luBw;

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Setu32Com(luBw);

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nRead:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstRead* lpstCmd = (tstRead*)mau8CmdMem;
                  UINT luBtr;
                  UINT luBr;

                  if (mcTrf.Size() < lpstCmd->btr)
                  {
                    luBtr = mcTrf.Size();
                  }
                  else
                  {
                    luBtr = lpstCmd->btr;
                  }

                  mResFatFs = f_read(lpstCmd->fp, lpstCmd->buff, luBtr, &luBr);
                  lpstCmd->br = luBr;

                  mResDataChanged = (1 << (u8)tstMems::nMemFile) | (1 << (u8)tstMems::nMemTrfMem);
                  mcTrf.Len((u16)luBr);
                  mcResData.Setu32Com(luBr);

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nClose:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstClose* lpstCmd = (tstClose*)mau8CmdMem;
                  mResFatFs = f_close(lpstCmd->fp);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nSync:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstSync* lpstCmd = (tstSync*)mau8CmdMem;
                  mResFatFs = f_sync(lpstCmd->fp);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nUnlink:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstUnlink* lpstCmd = (tstUnlink*)mau8CmdMem;
                  mResFatFs = f_unlink(lpstCmd->path);

                  mResDataChanged = 0;
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nChmod:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstChmod* lpstCmd = (tstChmod*)mau8CmdMem;
                  mResFatFs = f_chmod(lpstCmd->path, lpstCmd->attr, lpstCmd->mask);

                  mResDataChanged = 0;
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nUtime:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstUtime* lpstCmd = (tstUtime*)mau8CmdMem;
                  mResFatFs = f_utime(lpstCmd->path, lpstCmd->fno);

                  mResDataChanged = (1 << (u8)tstMems::nMemFileInfo);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nRename:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstRename* lpstCmd = (tstRename*)mau8CmdMem;
                  mResFatFs = f_rename(lpstCmd->path_old, lpstCmd->path_new);

                  mResDataChanged = 0;
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;


        case cJobs::nExpand:
          {
            switch(cJobHandler::mcState)
            {
              case cJobHandler::cJobStates::stJobProcessStart:
                {
                  tstExpand* lpstCmd = (tstExpand*)mau8CmdMem;
                  mResFatFs = f_expand(lpstCmd->fp, lpstCmd->fsz, lpstCmd->opt);

                  mResDataChanged = (1 << (u8)tstMems::nMemFile);
                  mcResData.Clear();

                  cJobHandler::vFinished();
                }
                break;
              default: break;
            }
          }
          break;

        default: cJobHandler::vFinished();
      }
    }
  }
};
