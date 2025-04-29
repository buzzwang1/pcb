using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using uint64 = System.UInt64;
using uint32 = System.UInt32;
using uint16 = System.UInt16;
using uint8 = System.Byte;
using u64 = System.UInt64;
using u32 = System.UInt32;
using u16 = System.UInt16;
using u8 = System.Byte;

using int64 = System.Int64;
using int32 = System.Int32;
using int16 = System.Int16;
using int8 = System.SByte;
using i64 = System.Int64;
using i32 = System.Int32;
using i16 = System.Int16;
using i8 = System.SByte;

class cConIoUsart :public cConIo
{
public:
  cConIoUsart(USART_TypeDef* lUart, uint32 lui32BufOutSize, uint32 lui32BufInSize)
    : cConIo(lUart, lui32BufOutSize, lui32BufInSize) {
    vHwInit();
  }

  //PA2: USART2_TX
  //PA3: USART2_RX

  void vHwInit()
  {
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if (mUart == USART1)
    {
    }
    else if (mUart == USART2)
    {
      //PA2: USART2_TX
      //PA3: USART2_RX
      cGPPIN mcPinTx(RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_2, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, (uint8)0);
      cGPPIN mcPinRx(RCC_AHBPeriph_GPIOA, GPIOA, GPIO_Pin_3, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, (uint8)0);
      GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
      GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
      /* Enable peripheral clock for USART2 */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

      /* USART2 IRQ Channel Configuration:
      *  IRQ Channel 37
      *  High pre-emption priority
      *  High sub-priority
      * */
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    }
    else if (mUart == USART3)
    {
      //PB10: USART2_TX
      //PB11: USART2_RX
      cGPPIN mcPinTx(RCC_AHBPeriph_GPIOB, GPIOB, GPIO_Pin_10, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, (uint8)0);
      cGPPIN mcPinRx(RCC_AHBPeriph_GPIOB, GPIOB, GPIO_Pin_11, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, (uint8)0);
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_7);
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_7);
      /* Enable peripheral clock for USART3 */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

      /* USART3 IRQ Channel Configuration: */
      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;

    }

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* USART configuration */
    USART_Init(mUart, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    /* NVIC Configuration */
    NVIC_Init(&NVIC_InitStructure);

    /* Enable Receive Data register not empty interrupt */
    USART_ITConfig(mUart, USART_IT_RXNE, ENABLE);

    /* Enable USART */
    USART_Cmd(mUart, ENABLE);
  }
};

class cBotNetStreamPort_ComPort : public cBotNetStreamPort
{
public:

  cConIoUsart mcConIo;

  cBotNetStreamPort_ComPort(USART_TypeDef* lUart)
    : mcConIo(lUart, cBotNet_MsgSize, cBotNet_MsgSize)
  {
    mszName = (char*)"BnComPort";

    if (lUart == USART1)
    {
      mszName += (char*)" U1";
    }
    else if (lUart == USART2)
    {
      mszName += (char*)" U2";
    }
    else if (lUart == USART3)
    {
      mszName += (char*)" U3";
    }
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort) + 
           sizeof(cBotNetStreamPort_ComPort);
  }

  void vProcess(void)
  {
    // Rx Data from HW port
    if (mcConIo.mcRingBufIn->cnt())
    {
      mcConIo.mcRingBufIn->move(&mcTxRingBuf, mcConIo.mcRingBufIn->cnt());
    }

    // Tx data to HW port
    if (mcRxRingBuf.cnt())
    {
      mcRxRingBuf.move(mcConIo.mcRingBufOut, mcRxRingBuf.cnt());
      if (mcConIo.mcRingBufOut->cnt())
      {
        mcConIo.vStartTx();
      }
    }
  }

  bool bProcessCmd(cStr &lcStrCmd, cStr &lcStrParam)
  {
    (void)lcStrCmd; /* avoid warning */
    (void)lcStrParam; /* avoid warning */
    return False;
  }

  virtual void vPrintCmds()
  {};
};

#include "cBnStreamSysPortCmd.h"
#include "cBotnet.h"

cBotNetStreamPort_BotNetCmdPort::cBotNetStreamPort_BotNetCmdPort(cBotNet* lcBotNet)
  : mcStrIn(mBufDataIn, 0, BN_CMD_PORT_KEY_MAXLINE), mcStrPrompt(mBufPrompt, 10)
{
  mszName = (const char8*)"BnCmdPort v01.00.02";

  mcBotNet = lcBotNet;
  
  mcStrPrompt.Itoa(0, mcBotNet->mcAdr.mAdr.stAdr.AdrD4, 16);
  mcStrPrompt += '.';
  mcStrPrompt.Itoa(2, mcBotNet->mcAdr.mAdr.stAdr.AdrD3, 16);
  mcStrPrompt += '.';
  mcStrPrompt.Itoa(4, mcBotNet->mcAdr.mAdr.stAdr.AdrD2, 16);
  mcStrPrompt += '.';
  mcStrPrompt.Itoa(6, mcBotNet->mcAdr.mAdr.stAdr.AdrD1, 16);
  mcStrPrompt += (const char8*)": ";

  mui8CursorPos = 0;
}

uint32 cBotNetStreamPort_BotNetCmdPort::SizeOf()
{
  return sizeof(cBotNetStreamPort_BotNetCmdPort);
}

void cBotNetStreamPort_BotNetCmdPort::vPrompt()
{
  if (mcStrPrompt.Len())
  {
    mcMyPrinter.vPrint(mcStrPrompt);
  }
}

bool cBotNetStreamPort_BotNetCmdPort::isKbHit()
{
  return (mcRxRingBuf.cnt() > 0);
}

char8 cBotNetStreamPort_BotNetCmdPort::chGetChr()
{
  return (char8)mcRxRingBuf.get();
}



void cBotNetStreamPort_BotNetCmdPort::vProcessCmd()
{
  uint16 lui16t;
  cStr lcStrParam;

  mcStrIn.Trim();

  // Parameter herausschneiden
  // Erstes Space suchen
  lui16t = mcStrIn.IndexOf(0, ' ');

  if (lui16t == (uint16)-1)
  {
    lui16t = 0;
  }

  lcStrParam.muiSize = mcStrIn.muiSize - lui16t;
  lcStrParam.muiLen = mcStrIn.muiLen - lui16t;
  lcStrParam.mpu8Data = mcStrIn.mpu8Data + lui16t;

  lcStrParam.Trim();


  // Einfache Suche nach dem Kommando
  uint8 liPortIdx;
  bool  lbCmdFound;

  lbCmdFound = False;

  for (liPortIdx = 0; liPortIdx < cBotNet_StreamSysPortsCnt; liPortIdx++)
  {
    if (mcBotNet->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      if (mcBotNet->mcStreamSys.mcPorts[liPortIdx]->bProcessCmd(mcStrIn, lcStrParam))
      {
        lbCmdFound = True;
        break;
      }
    }
  }

  if (lbCmdFound)
  {
    mcMyPrinter.vPrintLn((const char8*)"");
  }
  else
  {
    mcMyPrinter.vPrintLn((const char8*)"???");
  }
}

void cBotNetStreamPort_BotNetCmdPort::vProcess()
{
  uint16 lui16t;
  char8  lc8Zeichen[2];
  static uint16 lui16Zeichen = 0;

  lui16t = mcMyPrinter.mcTextOut.cnt();
  if (lui16t)
  {
    if (lui16t > mcTxRingBuf.space_left()) lui16t = mcTxRingBuf.space_left();

    mcMyPrinter.mcTextOut.move(&mcTxRingBuf, lui16t);
  }

  lui16t = mcMyPrinter.mcTextOut.cnt();
  if (lui16t < mcTxRingBuf.mtySize)
  {
    lui16t = mcRxRingBuf.cnt();

    while (lui16t)
    {
      lui16t--;
      lc8Zeichen[0] = chGetChr();
      lc8Zeichen[1] = 0;

      if ((lc8Zeichen[0] != 0) && (lui16Zeichen == 256))
      {
        if (lc8Zeichen[0] != '[')
        {
          lui16Zeichen += (uint8)lc8Zeichen[0];
        }
      }
      else
      {
        lui16Zeichen = (uint8)lc8Zeichen[0];
      }


      //if (lui16Zeichen != BN_CMD_PORT_KEY_ESC) // ESC ?
      {
        switch (lui16Zeichen)
        {
        case 256:
        case BN_CMD_PORT_KEY_ARROW:
          lui16Zeichen = 256;
          break;

        case BN_CMD_PORT_KEY_ARROW_LEFT: // Pfeil links
          if (mui8CursorPos > 0)
          {
            mcMyPrinter.vPrint((const char8*)"\b");
            mui8CursorPos--;
          }
          break;

        case BN_CMD_PORT_KEY_ARROW_RIGHT: // Pfeil rechts
          if (mui8CursorPos < mcStrIn.Len())
          {
            mcMyPrinter.vPutChr(mcStrIn[mui8CursorPos]);
            mui8CursorPos++;
          }
          break;

#if (BN_CMD_PORT_HISTORY == 1)
        case BN_CMD_PORT_KEY_ARROW_UP: // Pfeil hoch
        case BN_CMD_PORT_KEY_ARROW_DOWN: // Pfeil runter
        {
          /*uint8 lui16t;


          // Cursor ganz nach rechts
          lui16t = mcStrIn.Len();
          while (mui8CursorPos < lui16t)
          {
            vPrint(" ");
            mui8CursorPos++;
          }

          // Löscht die Zeile
          while (DStr8_ui8Len(&mstStr1) > 0)
          {
            DStr8_Mid(0, (uint8)(DStr8_ui8Len(&mstStr1) - 1), &mstStr2, &mstStr1);
            DStr8_Copy(&mstStr1, &mstStr2);
            vPrint("\b \b");
          }

          if (lui16Zeichen == CLI_KEY_ARROW_UP)   CLI_i8HistoryGet(&mstStr1, 1);
          if (lui16Zeichen == CLI_KEY_ARROW_DOWN) CLI_i8HistoryGet(&mstStr1, 0);
          vPrint(DStr8_pi8Data(&mstStr1));

          */
        }

        mui8CursorPos = mcStrIn.Len();
        break;
#endif  //#if (CLI_HISTORY == 1)

        case BN_CMD_PORT_KEY_RETURN: // return
        {
          mcMyPrinter.vPrintLn((char8*)"");

#if (BN_CMD_PORT_HISTORY == 1)
          //CLI_i8HistoryAdd(mstStr1);
#endif



          vProcessCmd();

          vPrompt();
          mui8CursorPos = 0;
          mcStrIn = "";
        }
        break;

        case BN_CMD_PORT_KEY_BACKSPACE: // backspace
          if (mcStrIn.Len() && (mui8CursorPos > 0))
          {
            uint16 lui16t;

            mui8CursorPos--;

            // Geht zu anfang der Zeile
            lui16t = mui8CursorPos + 1;
            while (lui16t)
            {
              mcMyPrinter.vPrint((const char8*)"\b");
              lui16t--;
            }
            mcStrIn.Cut(mui8CursorPos, 1);

            mcMyPrinter.vPrint(mcStrIn);
            mcMyPrinter.vPrint((const char8*)" ");

            // Alte Postion
            lui16t = mcStrIn.Len() + 1;
            while (lui16t > mui8CursorPos)
            {
              mcMyPrinter.vPrint((const char8*)"\b");
              lui16t--;
            }
          }
          break;

        default:
          if (mui8CursorPos < BN_CMD_PORT_KEY_MAXLINE)
          {
            uint8 lui8t;

            mui8CursorPos++;
            mcStrIn.Insert((uint8)(mui8CursorPos - 1), lc8Zeichen[0]);

            for (lui8t = (mui8CursorPos - 1); lui8t < mcStrIn.Len(); lui8t++)
            {
              mcMyPrinter.vPutChr(mcStrIn[lui8t]);
            }

            for (lui8t = mui8CursorPos; lui8t < mcStrIn.Len(); lui8t++)
            {
              mcMyPrinter.vPrint((const char8*)"\b");
            }
          }
          break;
        }
      }
    }
  }

  lui16t = mcMyPrinter.mcTextOut.cnt();
  if (lui16t)
  {
    if (lui16t > mcTxRingBuf.space_left()) lui16t = mcTxRingBuf.space_left();

    mcMyPrinter.mcTextOut.move(&mcTxRingBuf, lui16t);
  }
};


bool cBotNetStreamPort_BotNetCmdPort::bProcessCmd(cStr& lcStrCmd, cStr& lcStrParam)
{
  static const char8 lszVersion[] = "ver";
  static const char8 lszVersionOut[] = "v1.2.3";

  static const char8 lszPorts[] = "ports";
  static const char8 lszPort[] = ") Port: ";
  static const char8 lszCnx[] = ") Cnx: ";

  static const char8 lszSlaves[] = "slaves";
  static const char8 lszSlave[] = ") Slave: ";
  static const char8 lszSlaveOn[] = "On";
  static const char8 lszSlaveOff[] = "Off";

  static const char8 lszHelp[] = "help";

  char8  lszStrBuf[16];
  cStr  lszStr(lszStrBuf, 0, 16);

  (void)lcStrParam; ; /* avoid warning */

  // ------- ver
  if (lcStrCmd.Instr(0, (char8*)lszVersion) != (uint16)-1)
  {
    mcMyPrinter.vPrintLn((char8*)lszVersionOut);
    return True;
  }
  // ------- ports
  else if (lcStrCmd.Instr(0, (char8*)lszPorts) != (uint16)-1)
  {
    // Einfache Suche nach dem Kommando
    uint8 liPortIdx;
    uint8 liCnxIdx;

    for (liPortIdx = 0; liPortIdx < cBotNet_StreamSysPortsCnt; liPortIdx++)
    {
      if (mcBotNet->mcStreamSys.mcPorts[liPortIdx] != null)
      {
        lszStr.Itoa(liPortIdx, 10);
        lszStr += (char8*)lszPort;
        mcMyPrinter.vPrint(lszStr);
        mcMyPrinter.vPrintLn(mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mszName);
        for (liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
        {
          if (mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx] != 0)
          {
            mcMyPrinter.vPrint((const char8*)"  ");
            lszStr.Itoa(liCnxIdx, 10);
            lszStr += (const char8*)lszCnx;
            mcMyPrinter.vPrint(lszStr);

            if (mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestAdr.mAdr.u16Adr != 0)
            {
              lszStr.Itoa(0, mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestAdr.mAdr.stAdr.AdrD4, 16);
              lszStr += '.';
              lszStr.Itoa(2,mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestAdr.mAdr.stAdr.AdrD3, 16);
              lszStr += '.';
              lszStr.Itoa(4, mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestAdr.mAdr.stAdr.AdrD2, 16);
              lszStr += '.';
              lszStr.Itoa(6,mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestAdr.mAdr.stAdr.AdrD1, 16);
              lszStr += (const char8*)": ";
              lszStr.Itoa(8,mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mcBnDestPortIdx, 10);
              mcMyPrinter.vPrint(lszStr);
            }
            else
            {
              mcMyPrinter.vPrintLn(mcBotNet->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mszName);
            }
          }
        }
      }
    }
    return True;
  }
  // ------- slaves
  else if (lcStrCmd.Instr(0, (char8*)lszSlaves) != (uint16)-1)
  {
    // Einfache Suche nach dem Kommando
    uint8 liSlaveIdx;

    for (liSlaveIdx = 0; liSlaveIdx < cBotNet_MasterSlaveCnt; liSlaveIdx++)
    {
      if (mcBotNet->mcDownLinks[liSlaveIdx] != null)
      {
        lszStr.Itoa(liSlaveIdx + 1, 10);
        lszStr += (char8*)lszSlave;
        mcMyPrinter.vPrint(lszStr);

        lszStr.Itoa(0, mcBotNet->mcDownLinks[liSlaveIdx]->mcBnAdr.mAdr.stAdr.AdrD4, 16);
        lszStr += '.';
        lszStr.Itoa(2, mcBotNet->mcDownLinks[liSlaveIdx]->mcBnAdr.mAdr.stAdr.AdrD3, 16);
        lszStr += '.';
        lszStr.Itoa(4, mcBotNet->mcDownLinks[liSlaveIdx]->mcBnAdr.mAdr.stAdr.AdrD2, 16);
        lszStr += '.';
        lszStr.Itoa(6, mcBotNet->mcDownLinks[liSlaveIdx]->mcBnAdr.mAdr.stAdr.AdrD1, 16);
        lszStr += (const char8*)": ";
        mcMyPrinter.vPrint(lszStr);

        if (mcBotNet->mcDownLinks[liSlaveIdx]->mcComNode->mStatus.IsOnline)
        {
          mcMyPrinter.vPrintLn((char8*)lszSlaveOn);
        }
        else
        {
          mcMyPrinter.vPrintLn((char8*)lszSlaveOff);
        }
      }
    }
    return True;
  }
  // ------- help
  else if (lcStrCmd.Instr(0, (char8*)lszHelp) != (uint16)-1)
  {
    // Einfache Suche nach dem Kommando
    uint8 liPortIdx;

    mcMyPrinter.vPrintLn((char8*)lszVersionOut);
    mcMyPrinter.vPrintLn((char8*)lszHelp);
    mcMyPrinter.vPrintLn((char8*)lszVersion);
    mcMyPrinter.vPrintLn((char8*)lszPorts);
    mcMyPrinter.vPrintLn((char8*)lszSlaves);

    for (liPortIdx = 1; liPortIdx < 16; liPortIdx++)
    {
      if (mcBotNet->mcStreamSys.mcPorts[liPortIdx] != null)
      {
        mcBotNet->mcStreamSys.mcPorts[liPortIdx]->vPrintCmds();
      }
    }
    return True;
  }

  return False;
};


void cBotNetStreamPort_BotNetCmdPort::vPrintCmds()
{
};



#endif // __BOTNET_STREAM_SYS_PORT_COM_H__
