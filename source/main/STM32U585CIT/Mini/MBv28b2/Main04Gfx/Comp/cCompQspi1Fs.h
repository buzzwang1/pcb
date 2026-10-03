#pragma once

#include "cSysDPool.h"


#ifdef __cplusplus
extern "C" {
#endif

  extern void  vTaskFs(void* argument);

#ifdef __cplusplus
}
#endif

class cCompQSpi1Fs : public cComponent
{
  public:

  FATFS* mpFs;

  cCompQSpi1Fs()
    : cComponent(cDepTreeCfg::cComp::nQSpi1Fs, { cDepTreeCfg::cComp::nQSpi1 })
  {
    mpFs = &cSysDPool::mFs.mstFs;
  }

  bool bInit() override
  {
    //cComponentList::mcList1ms.Add(this->mu8Idx);

    vTaskFsStart();

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    //cComponentList::mcList1ms.vRemove(this->mu8Idx);

    vTaskFsTerminate();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    //mcZD25WQ32.vDoProcess(1000);  

    return cComponent::bRun();
  }


  void vTaskFsStart()
  {
    const osThreadAttr_t TaskFs_attributes = {
      .name       = "cFs",                                   ///< name of the thread
      .attr_bits  = 0,                                       ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.Fs.Tcb,         ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.Fs.Tcb),  ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.Fs.Stack,        ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.Fs.StackSize(),  ///< size of stack
      .priority   = (osPriority_t)osPriorityLow,             ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };

    cSysDPool::mSys.mcTasks.Fs.Handle = osThreadNew(vTaskFs, (void*)null, &TaskFs_attributes);
  }

  void vTaskFsTerminate()
  {
    osThreadTerminate(cSysDPool::mSys.mcTasks.Fs.Handle);
  }


  static void vFsTask()
  {
    // Dateisystem mounten
    FATFS* lpFs = &cSysDPool::mFs.mstFs;

    //MKFS_PARM opt = { FM_FAT | FM_SFD, 1, 0, 0, 4096 };
    //f_mkfs("", &opt, &cSysDPool::mFs.macFsJob[0].mau8TrfMem, 4096);

    f_mount(lpFs, "", 1);

    // Test
    {
      FRESULT res;
      DIR     dir;
      FILINFO fno; // Statisch, um Stack-Überlauf zu vermeiden (oder normal deklarieren)

      // 1. Root-Verzeichnis öffnen ("/" steht für das Root-Verzeichnis, "0:/" falls Laufwerksnummern aktiv sind)
      res = f_opendir(&dir, "/");
      if (res != FR_OK)
      {
        //printf("Fehler beim Öffnen des Root-Verzeichners (Fehler-Code: %d)\n", res);
        __asm("nop");
      }

      //printf("\n--- Inhalt des Root-Verzeichnisses ---\n");

      // Schleife über alle Einträge im Verzeichnis
      for (;;)
      {
        // Liest den nächsten Eintrag
        res = f_readdir(&dir, &fno);

        // Wenn ein Fehler auftritt oder keine Einträge mehr da sind (fno.fname[0] == 0), Schleife beenden
        if (res != FR_OK || fno.fname[0] == 0) {
          break;
        }

        // Prüfen, ob es sich um ein Verzeichnis oder eine Datei handelt
        if (fno.fattrib & AM_DIR) {
          // Es ist ein Ordner
          //printf("  [DIR]  %s\n", fno.fname);
          __asm("nop");
        }
        else
        {
          // Es ist eine Datei (wird auch die Größe in Bytes ausgegeben)
          //printf("  [FILE] %s  (%lu Bytes)\n", fno.fname, (unsigned long)fno.fsize);
          __asm("nop");
        }
      }

      // Verzeichnis wieder schließen, um Ressourcen freizugeben
      f_closedir(&dir);
    }

    while (1)
    {
      for (u8 lu8FsJobIdx = 0; lu8FsJobIdx < 4; lu8FsJobIdx++)
      {
        cFsJob* lpcFsJob = &cSysDPool::mFs.macFsJob[lu8FsJobIdx];

        while (lpcFsJob->isBusy())
        {
          lpcFsJob->vProcess(0);
        }
      }

      vTaskDelay(pdMS_TO_TICKS(10));
    }
  }

};

void vTaskFs(void* argument)
{
  UNUSED(argument);
  while (1)
  {
    cCompQSpi1Fs::vFsTask();
  }
}