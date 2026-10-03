/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"      /* Basic definitions of FatFs */
#include "diskio.h"    /* Declarations FatFs MAI */

#include "cSysDPool.h"

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE pdrv    /* Physical drive nmuber to identify the drive */
)
{
  UNUSED(pdrv);

  if (cSysDPool::mFs.mpcZD25WQ32->isIdle()) return RES_OK;
  if (cSysDPool::mFs.mpcZD25WQ32->isError()) return RES_ERROR;

  return RES_NOTRDY;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE pdrv        /* Physical drive nmuber to identify the drive */
)
{
  UNUSED(pdrv);
  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
  BYTE pdrv,    /* Physical drive nmuber to identify the drive */
  BYTE *buff,    /* Data buffer to store read data */
  LBA_t sector,  /* Start sector in LBA */
  UINT count    /* Number of sectors to read */
)
{
  UNUSED(pdrv);

  cSysDPool::mFs.mpcZD25WQ32->i8StartRead(cSysDPool::mFs.mpcZD25WQ32->u32GetBaseAdr() + sector * 4096, buff, count * 4096);
  cSysDPool::mFs.mpcZD25WQ32->vDoProcess(0);

  while (!cSysDPool::mFs.mpcZD25WQ32->isIdle())
  {
    if (cSysDPool::mFs.mpcZD25WQ32->isError()) break;
    vTaskDelay(pdMS_TO_TICKS(1));
    cSysDPool::mFs.mpcZD25WQ32->vDoProcess(1000);
  }

  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
  BYTE pdrv,      /* Physical drive nmuber to identify the drive */
  const BYTE *buff,  /* Data to be written */
  LBA_t sector,    /* Start sector in LBA */
  UINT count      /* Number of sectors to write */
)
{
  UNUSED(pdrv);

  cSysDPool::mFs.mpcZD25WQ32->i8StartEraseSectors(cSysDPool::mFs.mpcZD25WQ32->u32GetBaseAdr() + sector * 4096, count * 4096);
  cSysDPool::mFs.mpcZD25WQ32->vDoProcess(0);

  while (!cSysDPool::mFs.mpcZD25WQ32->isIdle())
  {
    if (cSysDPool::mFs.mpcZD25WQ32->isError()) break;
    vTaskDelay(pdMS_TO_TICKS(1));
    cSysDPool::mFs.mpcZD25WQ32->vDoProcess(1000);
  }

  cSysDPool::mFs.mpcZD25WQ32->i8StartWrite((u8*)buff, cSysDPool::mFs.mpcZD25WQ32->u32GetBaseAdr() + sector * 4096, count * 4096);
  cSysDPool::mFs.mpcZD25WQ32->vDoProcess(0);

  while (!cSysDPool::mFs.mpcZD25WQ32->isIdle())
  {
    if (cSysDPool::mFs.mpcZD25WQ32->isError()) break;
    vTaskDelay(pdMS_TO_TICKS(1));
    cSysDPool::mFs.mpcZD25WQ32->vDoProcess(1000);
  }

  return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
  BYTE pdrv,    /* Physical drive nmuber (0..) */
  BYTE cmd,    /* Control code */
  void *buff    /* Buffer to send/receive control data */
)
{
  UNUSED(pdrv);
  DRESULT res = RES_ERROR;

  switch (cmd)
  {
    case CTRL_SYNC:
      // Keine Cache-Verzögerung -> direkt OK
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT:
      // 4 MB Gesamtgröße / 4096 Bytes pro Sektor = 1024 Sektoren
      *(LBA_t*)buff = 1024;
      res = RES_OK;
      break;

    case GET_SECTOR_SIZE:
      *(WORD*)buff = 4096; // Sektorgröße definieren
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE:
      *(DWORD*)buff = 1; // 1 Sektor entspricht einem Löschblock (4KB)
      res = RES_OK;
      break;

    default:
      res = RES_PARERR;
      break;
  }

  return res;
}


DWORD get_fattime()
{
  //SYSTEMTIME st;
  //GetLocalTime(&st);

  // Jahr muss relativ zu 1980 berechnet werden

  return ((/* year */  50 & 127)      << 25)
       | ((/* Month */  6 & 15)  << 21)
       | ((/* Day */    4 & 31)    << 16)
       | ((/* Hour */   3 & 31)   << 11)
       | ((/* Minute */ 2 & 63) << 5)
       | ((/* Second */ 1 / 2) & 31);
}

