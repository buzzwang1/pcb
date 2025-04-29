#ifndef __BOTNET_STREAM_SYS_PORT_IAP_H__
#define __BOTNET_STREAM_SYS_PORT_IAP_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"

#include "cBnStreamSysPortBase.h"
#include "cCli.h"
#include "iap_platform.h"

class cBotNet;

class cBnCliCmd_IAP_SysInfo: public cCliCmd
{
  public:
    cBnCliCmd_IAP_SysInfo():cCliCmd((const char*)"SysInfo", (const char*)"System Info") 
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};


class cBotNetStreamPort_BotNetMemPort_Base: public cBotNetStreamPort
{
  public:

  typedef enum
  {
    enIdle = 0,
    enWaitForCmdSessionStart,
    enWaitForCmdData,
    enWaitForWriteData,
    enProcessing,
    enExit,
  }tenMainStates;

  typedef enum
  {
    enNop = 0,
    enInterfaceInfo1,
    enInterfaceInfo2,

    enInterfaceMemoryInfo1,
    enInterfaceMemoryInfo2,
    enInterfaceMemoryInfo3,
    enInterfaceMemoryInfo4,
    enInterfaceMemoryInfo5,

    enWrite1,
    enWrite2,
    enWrite3,

    enRead1,
    enRead2,
    enRead3,

    enChecksum1,
    enChecksum2,

    enCall1,
    enCall2,

    enJump1,
    enJump2,

    enExit1,

    enEnter1,
    enEnter2,

  }tenCmdStates;


  tenMainStates menMainState;
  tenCmdStates  menCmdState;

  uint8 maui8Ram[InAppProg_Platform_Ram_Size];

  uint8  mui8PartitionsIdx;
  uint32 mui32Size_Requested;
  uint32 mui32Size;
  uint8* mpui8Adr_Requested;
  uint8* mpui8Adr;

  uint32 mui32MasterID;
  uint32 mui32SlaveID;

  uint32 mui32Checksum;

  uint8  mui8InBuf[32];
  uint8  mui8OutBuf[32];
  bool   mbTxRequest;

  uint32 mui32IapSessionTimeout;

  //Zwischenpuffer. Je nach Controller müssen 2, 4, 8 Byte Blöcke geschrieben werden.
  uint8  mui8FlashWriteBuf[32];
  cbArrayExtT<u16> mcFlashWriteBuf;

  //uint8  mui8Error;

  u8                          mu8PartitionCnt;
  const tstMemPart_Partition* mstPartitions;

  cBotNetStreamPort_BotNetMemPort_Base(u8 lu8PartitionCnt, const tstMemPart_Partition* lstPartitions)
     : mcFlashWriteBuf(mui8FlashWriteBuf, 32)
  {
    mszName    = "BnMemPort v01.00.02";

    mu8PartitionCnt = lu8PartitionCnt;
    mstPartitions = lstPartitions;

    menMainState    = enWaitForCmdSessionStart;
    menCmdState     = enNop;
    //mui8Error       = 0;
    mui32MasterID   = 0;
    mui32SlaveID    = 0;
    mui32IapSessionTimeout = 0;
    mbTxRequest     = False;
  }

  virtual void vReInit()
  {
    cBotNetStreamPort::vReInit();
    menMainState = enWaitForCmdSessionStart;
    menCmdState = enNop;
    mui32IapSessionTimeout = 0;
    mbTxRequest = False;
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort_BotNetMemPort_Base);
  }


  uint32 ui32ChkSum(uint8* lpui8Src, uint32 lui32Len)
  {
    uint32 lui32Checksum;

    lui32Checksum = 0;
    while (lui32Len > 0)
    {
      lui32Checksum += *((uint8*)lpui8Src);
      (uint8*)lpui8Src++;
      lui32Len--;
    }
    return (lui32Checksum+1);
  };

  u32 vMemCopy(const uint8* lpvSrc, uint8* lpvDst, uint32 lui32Len)
  {
    u32 lu32Checksum;

    lu32Checksum = 0;
    while (lui32Len > 0)
    {
    *((uint8*)lpvDst) = *((uint8*)lpvSrc);
    lu32Checksum     += *((uint8*)lpvSrc);
    (uint8*)lpvDst++;
    (uint8*)lpvSrc++;
      lui32Len--;
    }
    return lu32Checksum;
  };

  void vClearBuf(uint8* lpui8Dst, uint32 lui32Len)
  {
    uint8 luit;
    for (luit=0; luit < lui32Len; luit++)  lpui8Dst[luit] = 0;
  };

  void vClearBuf(uint8* lpui8Dst)
  {
    vClearBuf(lpui8Dst, 32);
  };


  void vTick10ms(void)
  {
    if (mui32IapSessionTimeout < (5 * 60 * 1000)) // 5 min Timeout
    {
      mui32IapSessionTimeout += 10;
    }
    else
    {
      mui32IapSessionTimeout = 0;
      menMainState    = enWaitForCmdSessionStart;
      menCmdState     = enNop;
      mcRxRingBuf.reset();
      mcTxRingBuf.reset();
    }
  }

  inline void vGetFirstInput()
  {
    mui8InBuf[0] = mcRxRingBuf.get();
  }

  inline void vGetRestInput()
  {
    mcRxRingBuf.get(&mui8InBuf[1], 31);
  }

  inline void vGetFullInput()
  {
    mcRxRingBuf.get(mui8InBuf, 32);
  }

  void vSetOutput()
  {
    if (mbTxRequest)
    {
      mcTxRingBuf.put(mui8OutBuf, 32);
      mbTxRequest = False;
    }
  }

  void vDoCmd1(void)
  {
    if (
         ((menMainState == enWaitForCmdSessionStart) && (mui8InBuf[0] == 0x81)) // Enter ?
         ||
         (menMainState == enWaitForCmdData)
       )
    {
      mui32IapSessionTimeout = 0;
      switch (mui8InBuf[0])
      {
        case 0x00: // Interface Info
          menCmdState  = enInterfaceInfo1;
          menMainState = enProcessing;
          break;
        case 0x01: // Interface Memory Info
          menCmdState  = enInterfaceMemoryInfo1;
          menMainState = enProcessing;
          break;

        case 0x10: // Write
          menCmdState  = enWrite1;
          menMainState = enProcessing;
          break;

        case 0x20: // Read
          menCmdState  = enRead1;
          menMainState = enProcessing;
          break;

        case 0x30: // Checksum
          menCmdState  = enChecksum1;
          menMainState = enProcessing;
          break;

        case 0x40: // Call
          menCmdState  = enCall1;
          menMainState = enProcessing;
          break;
        case 0x41: // Jump to Application
          menCmdState  = enJump1;
          menMainState = enProcessing;
          break;

        case 0x80: // Exit
          menCmdState  = enExit1;
          menMainState = enProcessing;
          break;

        case 0x81: // Enter
          menCmdState  = enEnter1;
          menMainState = enProcessing;
          break;

        default:
          menCmdState  = enExit1;
          menMainState = enProcessing;
      }
    }
  }

  void vDoCmd2(void)
  {
    if (menCmdState != enNop)
    {
      vClearBuf(mui8OutBuf);
    }

    switch (menCmdState)
    {
      case enInterfaceInfo1:
        {
          mui8OutBuf[0] = 0;
          mui8OutBuf[1] = 0;
          vMemCopy((uint8*)InAppProg_Interfacename, (uint8*)&mui8OutBuf[ 2], sizeof(InAppProg_Interfacename));
          vMemCopy((uint8*)InAppProg_Platform_Name, (uint8*)&mui8OutBuf[16], 16);
          mbTxRequest = True;

          menCmdState = enInterfaceInfo2;
        }
        break;
      case enInterfaceInfo2:
        {
          mui8OutBuf[0] = 0;
          mui8OutBuf[1] = 1;
          vMemCopy((uint8*)InAppProg_Platform_Info, (uint8*)&mui8OutBuf[ 2], 30);
          mbTxRequest = True;

          menCmdState  = enNop;
          menMainState = enWaitForCmdData;
        }
        break;

      case enInterfaceMemoryInfo1:
        {
          uint32 lui32Temp;

          mui8OutBuf[0] = 1;
          mui8OutBuf[1] = 0;

          mui8PartitionsIdx = 0;

          lui32Temp = mu8PartitionCnt;
                                                 /*CPCP*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[12], 4);
          lui32Temp = (uint32)maui8Ram;          /*WAWA*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[20], 4);
          lui32Temp = sizeof(maui8Ram);          /*WSWS*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[28], 4);
          mbTxRequest = True;
          menCmdState = enInterfaceMemoryInfo2;
        }
        break;

      case enInterfaceMemoryInfo2:
        {
          uint16 lui16Temp;
          uint32 lui32Temp;
          const tstMemPart_Partition* lpstPartition;
          lpstPartition = &(mstPartitions[mui8PartitionsIdx]);

          mui8OutBuf[0] = 1;
          mui8OutBuf[1] = 1;
          mui8OutBuf[3] = 0;

          lui32Temp = mui8PartitionsIdx;             /*Idx*/   vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[ 4], 4);
          lui16Temp = lpstPartition->ui16Sort;       /*Sort*/  vMemCopy((uint8*)&lui16Temp, (uint8*)&mui8OutBuf[ 8], 2);
          lui16Temp = lpstPartition->ui16Type;       /*Type*/  vMemCopy((uint8*)&lui16Temp, (uint8*)&mui8OutBuf[10], 2);
          lui32Temp = lpstPartition->ui32Block_Size; /*SCSC*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[12], 4);
          lui32Temp = lpstPartition->ui32Start;      /*AAAA*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[20], 4);
          lui32Temp = lpstPartition->ui32Size;       /*SSSS*/  vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[28], 4);
          mbTxRequest = True;
          menCmdState = enInterfaceMemoryInfo3;
        }
        break;

      case enInterfaceMemoryInfo3:
        {
          uint32 lui32Temp;
          const tstMemPart_Partition*lpstPartition;
          lpstPartition = &(mstPartitions[mui8PartitionsIdx]);

          mui8OutBuf[0] = 1;
          mui8OutBuf[1] = 1;
          mui8OutBuf[3] = 1;
          lui32Temp = mui8PartitionsIdx;  /*Idx*/   vMemCopy((uint8*)&lui32Temp,            (uint8*)&mui8OutBuf[ 4], 4);
                                                    vMemCopy((uint8*)lpstPartition->szName, (uint8*)&mui8OutBuf[ 8], 24);
          mbTxRequest = True;

          mui8PartitionsIdx++;
          if (mui8PartitionsIdx < mu8PartitionCnt)
          {
            menCmdState = enInterfaceMemoryInfo2;
          }
          else
          {
            menCmdState  = enNop;
            menMainState = enWaitForCmdData;
          }
        }
        break;


      case enWrite1: // Write
        {
          uint32 lui32Temp;
          uint32 lui32Size;
          uint8* lpui8Adr;

          if (mui8InBuf[1] == 128)
          {

            vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&lpui8Adr, 4);
            vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&lui32Size, 4);

            vClearBuf(mui8FlashWriteBuf, 32);
            mcFlashWriteBuf.Clear();

            mui32Size_Requested = lui32Size;
            mpui8Adr_Requested  = lpui8Adr;
            mui32Checksum       = 1;

            mui32Size = mui32Size_Requested;
            mpui8Adr  = mpui8Adr_Requested;

            if (InAppProg_Platform_u16GetMemorySort(mpui8Adr_Requested) == InAppProg_Sort_Flash)
            {
              uint32 lui32t;
              uint32 lui32Pages;

              /* Porgram FLASH Bank1 ********************************************************/
              /* Unlock the Flash Bank1 Program Erase controller */
              InAppProg_Platform_vFlash_Unlock();

              lui32Pages = InAppProg_Platform_ui32Flash_Size2Pages(mpui8Adr_Requested, mui32Size_Requested);

              for (lui32t = 0; lui32t < lui32Pages; lui32t++)
              {
                InAppProg_Platform_vFlash_EreasePage(mpui8Adr);
                mpui8Adr += mstPartitions[0].ui32Block_Size;
              }
            }
            mui8OutBuf[0] = 0x10;
            mui8OutBuf[1] = 0; // ACK
            mui8OutBuf[2] = 0;
            vMemCopy((uint8*)&mui8InBuf[ 8],  (uint8*)&mui8OutBuf[ 8], 4);
            vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&mui8OutBuf[16], 4);
            lui32Temp = (lui32Size >> 7) + 1000; /*Timeout*/
            vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[24], 4);

            mui32Size = mui32Size_Requested;
            mpui8Adr  = mpui8Adr_Requested;

            mbTxRequest = True;
            menMainState = enWaitForWriteData;
            menCmdState  = enWrite2;
          }
          else
          {
            mui8OutBuf[0] = 0x10;
            mui8OutBuf[1] = 1; // NACK
            mui8OutBuf[2] = 0;
            vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&lui32Size, 4);
            lui32Temp = (lui32Size >> 7) + 1000; /*Timeout*/
            vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[24], 4);

            menCmdState  = enNop;
            menMainState = enWaitForCmdData;
          }
        }
        break;

      case enWrite2: // Write
        {
          uint32 lui32Size2;

          lui32Size2 = mui32Size;
          if (lui32Size2 > 30) lui32Size2 = 30;

          if (InAppProg_Platform_u16GetMemorySort(mpui8Adr_Requested) == InAppProg_Sort_Flash)
          {
             u8 lui8i;

             for (lui8i = 0; lui8i < lui32Size2; lui8i++)
             {
               mcFlashWriteBuf.Add(mui8InBuf[2 + lui8i]);
               if (mcFlashWriteBuf.Len() == 32)
               {
                  mui32Checksum += InAppProg_Platform_vFlash_Program(mpui8Adr, mui8FlashWriteBuf, 32);
                  vClearBuf(mui8FlashWriteBuf, 32);
                  mcFlashWriteBuf.Clear();

                  mpui8Adr += 32;
               }
             }
          }
          else
          {
            mui32Checksum += vMemCopy((uint8*)&mui8InBuf[2], (uint8*)mpui8Adr, lui32Size2);
            mpui8Adr  += lui32Size2;
          }

          mui32Size -= lui32Size2;

          if (mui8InBuf[1] == 0xFF) //Letztes Paket ?
          {
            //if (mui32Size != 0) // zu wenig Daten empfangen ?
            //{
            //}

            mui32Size = 0;
          }

          if (mui32Size == 0)
          {
            if (InAppProg_Platform_u16GetMemorySort(mpui8Adr_Requested) == InAppProg_Sort_Flash)
            {
               // Zwischenbuffer Flushen
               if (mcFlashWriteBuf.Len() > 0)
               {
                  mui32Checksum += InAppProg_Platform_vFlash_Program(mpui8Adr, mui8FlashWriteBuf, 32);
               }

               InAppProg_Platform_vFlash_Lock();
            }

            menCmdState  = enWrite3;
            menMainState = enProcessing;
          }
        }
        break;

      case enWrite3:
        {
          mui8OutBuf[0] = 0x10;
          mui8OutBuf[1] = 0;
          mui8OutBuf[2] = 1;
          vMemCopy((uint8*)&mpui8Adr_Requested,  (uint8*)&mui8OutBuf[ 8], 4);
          vMemCopy((uint8*)&mui32Size_Requested, (uint8*)&mui8OutBuf[16], 4);
          vMemCopy((uint8*)&mui32Checksum, (uint8*)&mui8OutBuf[20], 4);

          mbTxRequest  = True;
          menCmdState  = enNop;
          menMainState = enWaitForCmdData;
        }
        break;

      case enRead1: // Read
        {
          uint32 lui32Temp;
          uint32 lui32Size;
          uint8* lpui8Adr;

          // Process the data from the Master
          vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&lpui8Adr, 4);
          vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&lui32Size, 4);

          mui32Size_Requested = lui32Size;
          mpui8Adr_Requested  = lpui8Adr;

          mui8PartitionsIdx = 128;

          mui32Size     = lui32Size;
          mpui8Adr      = lpui8Adr;
          mui32Checksum = 1;

          // Send Ack
          mui8OutBuf[0] = 0x20;
          mui8OutBuf[1] = 0;
          mui8OutBuf[2] = 0;

          vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&mui8OutBuf[ 8], 4);
          vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&mui8OutBuf[16], 4);

          lui32Temp = (lui32Size >> 7) + 1000; /*Timeout*/
          vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[24], 4);

          mbTxRequest  = True;
          menCmdState  = enRead2;
        }
        break;

      case enRead2: // Read
        {
          uint32 lui32Size2;

          lui32Size2   = mui32Size;

          mui8OutBuf[0] = 0x20;
          mui8OutBuf[1] = 0xFF; //Letztes Paket

          if (lui32Size2 > 30)
          {
            lui32Size2 = 30;
            mui8OutBuf[1] = mui8PartitionsIdx; //Paketzähler
          }

          // Read the data and send it to the Master
          mui32Checksum += vMemCopy((uint8*)mpui8Adr, (uint8*)&mui8OutBuf[2], lui32Size2);
          mpui8Adr  += lui32Size2;
          mui32Size -= lui32Size2;

          if (mui8PartitionsIdx == 254)
          {
            mui8PartitionsIdx = 128;
          }
          else
          {
            mui8PartitionsIdx++;
          }

          mbTxRequest = True;

          if (mui32Size == 0)
          {
            menCmdState  = enRead3;
          }
        }
        break;

      case enRead3: // Read
        {
          //Send Ack
          mui8OutBuf[0] = 0x20;
          mui8OutBuf[1] = 0;
          mui8OutBuf[2] = 1;
          vMemCopy((uint8*)&mpui8Adr_Requested,  (uint8*)&mui8OutBuf[ 8], 4);
          vMemCopy((uint8*)&mui32Size_Requested, (uint8*)&mui8OutBuf[16], 4);
          vMemCopy((uint8*)&mui32Checksum, (uint8*)&mui8OutBuf[20], 4);

          mbTxRequest  = True;
          menCmdState  = enNop;
          menMainState = enWaitForCmdData;
        }
        break;

      case enChecksum1:
        {
          uint32 lui32Temp;

          vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&mpui8Adr, 4);
          vMemCopy((uint8*)&mui8InBuf[16], (uint8*)&mui32Size, 4);
          mpui8Adr_Requested  = mpui8Adr;
          mui32Size_Requested = mui32Size;
          mui32Checksum = 1;

          mui8OutBuf[0] = 0x30;
          mui8OutBuf[1] = 0;
          mui8OutBuf[2] = 0;
          lui32Temp = (mui32Size >> 7) + 1000; /*Timeout*/
          vMemCopy((uint8*)&lui32Temp, (uint8*)&mui8OutBuf[24], 4);

          mbTxRequest  = True;
          menCmdState  = enChecksum2;
        }
        break;

      case enChecksum2:
        {
          if (mui32Size > 1024)
          {
            mui32Checksum += (ui32ChkSum(mpui8Adr, 1024) - 1);
            mpui8Adr += 1024;
            mui32Size -= 1024;
          }
          else
          {
            mui8OutBuf[0] = 0x30;
            mui8OutBuf[1] = 0;
            mui8OutBuf[2] = 1;
            vMemCopy((uint8*)&mpui8Adr_Requested,  (uint8*)&mui8OutBuf[ 8], 4);
            vMemCopy((uint8*)&mui32Size_Requested, (uint8*)&mui8OutBuf[16], 4);

            mui32Checksum += (ui32ChkSum(mpui8Adr, mui32Size) - 1);
            vMemCopy((uint8*)&mui32Checksum, (uint8*)&mui8OutBuf[20], 4);

            mbTxRequest  = True;
            menCmdState  = enNop;
            menMainState = enWaitForCmdData;
          }
        }
        break;

    case enCall1: // Call
      {
        uint8* lpui8Adr;

        vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&lpui8Adr,  4);

        mui8OutBuf[0] = 0x40;
        mui8OutBuf[1] = 0;
        vMemCopy((uint8*)&lpui8Adr,  (uint8*)&mui8OutBuf[ 8], 4);
        mbTxRequest  = True;
        menCmdState  = enCall2;
      }
      break;

    case enCall2: // Call
      {
        uint8* lpui8Adr;
        uint32 lui32Param[4] = {0};

        vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&lpui8Adr,  4);
        vMemCopy((uint8*)&mui8InBuf[12], (uint8*)lui32Param, 4*4);

        InAppProg_Platform_vCall(mpui8Adr, lui32Param);

        menCmdState  = enNop;
        menMainState = enWaitForCmdData;
      }
      break;

    case enJump1: // Jump
      {
        uint8* lpui8Adr;
        vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&lpui8Adr,  4);

        mpui8Adr = lpui8Adr;

        mui8OutBuf[0] = 0x41;
        mui8OutBuf[1] = 0;
        vMemCopy((uint8*)&lpui8Adr,  (uint8*)&mui8OutBuf[ 8], 4);
        mbTxRequest  = True;
        menCmdState  = enJump2;
      }
      break;

    case enJump2: // Jump
      {
        InAppProg_Platform_vJumpToApp(mpui8Adr);

        menCmdState  = enNop;
        menMainState = enWaitForCmdData;
      }
      break;

    case enExit1: // Exit
      menCmdState  = enNop;
      menMainState = enWaitForCmdSessionStart;
      break;

    case enEnter1: // Start
      {
        uint32 lui32Temp;
        vMemCopy((uint8*)&mui8InBuf[ 4], (uint8*)&lui32Temp,  4);

        mui8OutBuf[0] = 0x81;
        mui8OutBuf[1] = 1; //NACK

        if (lui32Temp == 0x01020304)
        {
          mui8OutBuf[1] = 0; // ACK
          vMemCopy((uint8*)&mui8InBuf[ 8], (uint8*)&mui32MasterID,  4);
          vMemCopy((uint8*)&mui8InBuf[12], (uint8*)&mui32SlaveID,  4);
          menCmdState  = enEnter2;
        }
        else
        {
          menCmdState  = enExit1;
        }

        vMemCopy((uint8*)&mui8InBuf[ 4],  (uint8*)&mui8OutBuf[ 4], 12);
        lui32Temp = cBotNet_UpLinkComBufSize;
        vMemCopy((uint8*)&lui32Temp,  (uint8*)&mui8OutBuf[16], 4);

        mbTxRequest  = True;

      }
      break;

    case enEnter2: // Start
      {
        menCmdState  = enNop;
        menMainState = enWaitForCmdData;
      }
      break;

      default:
        menCmdState  = enNop;
        menMainState = enWaitForCmdData;
        break;
    }
  }

  void vProcess(void)
  {
    switch (menMainState)
    {
      case enIdle:
        break;
      case enWaitForCmdSessionStart:
      case enWaitForCmdData:
        if ((mcRxRingBuf.cnt() >= 32) && (mcTxRingBuf.isEmpty()))
        {
          vGetFirstInput();
          vDoCmd1();
          if (menMainState == enProcessing)
          {
            vGetRestInput();
            vDoCmd2();
          }
        }
        break;
      case enWaitForWriteData:
        if ((mcRxRingBuf.cnt() >= 32) && (mcTxRingBuf.isEmpty()))
        {
          vGetFullInput();
          vDoCmd2();
        }
        break;
      case enProcessing:
        if (mcTxRingBuf.isEmpty())
        {
          vDoCmd2();
        }
        break;
      case enExit:
      default:
        break;
    }
    vSetOutput();
  }
};


class cBotNetStreamPort_BotNetMemPort : public cBotNetStreamPort_BotNetMemPort_Base
{
public: 
  cBotNetStreamPort_BotNetMemPort(u8 lu8PartitionCnt, const tstMemPart_Partition* lstPartitions)
    : cBotNetStreamPort_BotNetMemPort_Base(lu8PartitionCnt, lstPartitions)
  {}

  cBnCliCmd_IAP_SysInfo  mcCliCmd_SysInfo;

  void vAddedToBn(cBotNet* lcBotNet) override;
};

#endif // __BOTNET_STREAM_SYS_PORT_IAP_H__
