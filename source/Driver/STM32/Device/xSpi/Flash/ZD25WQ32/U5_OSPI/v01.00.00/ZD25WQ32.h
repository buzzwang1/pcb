#ifndef __OSPI_ZD25WQ32_H__
#define __OSPI_ZD25WQ32_H__

#include "FlashGeneral.h"
#include "ZD25WQ32_HW.h"


// Zetta - ZD25WQ32CEIGR
// Page Size 256B(CR.QP = 0)
// 64 Blocks  je 64kB = > 4MB
// 16 Sectors je  4KB = > 64kb
// 16 Pages   je 256B = > 4kB
//
// 10ms 4K - byte sector erase time
// 2ms page programming time
// = > 4MB:   32768ms
// = > 1MB:    8192ms
// = > 100kB:   800ms

// Gemessen STM32U575 160Mhz OSPI 80Mhz
//
//  Chip   Erase:         11140us
//  Sector Erase:         11140us
//  Sector Write Single:  2160us
//  Sector Write Quad:    2160us
//  Sector Read Single:    618us
//  Sector Read Dual:      618us
//  Sector Read Quad:      618us
//

class cZD25WQ32 : public cSpiFlashGeneral, public cZD25WQ32_HW
{
  public:

  cZD25WQ32()
    : cSpiFlashGeneral(4096, 64 * 16, 0x90000000)
  {

  }


  i32 i32ReadSingleData(u8 lu8Instruction, u8* lpu8Dest, u8 lu8Size)
  {
    OSPI_RegularCmdTypeDef s_command;
    cMemTools::vMemSet((u8*)&s_command, 0, sizeof(s_command));

    s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = lu8Instruction;
    s_command.DataMode        = HAL_OSPI_DATA_1_LINE;
    s_command.NbData          = lu8Size; // Width of useful ID

    /* Configure the command */
    if (HAL_OSPI_Command(&mstOSpi, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    /* Reception of the data */
    if (HAL_OSPI_Receive(&mstOSpi, lpu8Dest, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }
    return 0;
  }

  // Bit S0: Busy
  // Bit S1: WEL Write Enable
  i32 i32AutoPollingStatus(u8 lu8Match, u8 lu8Mask)
  {
    OSPI_AutoPollingTypeDef sConfig;
    OSPI_RegularCmdTypeDef s_command;

    cMemTools::vMemSet((u8*)&sConfig, 0, sizeof(sConfig));
    cMemTools::vMemSet((u8*)&s_command, 0, sizeof(s_command));

    s_command.Instruction     = 0x5;
    s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.DataMode        = HAL_OSPI_DATA_1_LINE;
    s_command.NbData          = 1;

    /* Configure the command */
    if (HAL_OSPI_Command(&mstOSpi, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    sConfig.Match         = lu8Match;
    sConfig.Mask          = lu8Mask;
    sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
    sConfig.Interval      = 0x10;
    sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(&mstOSpi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    return 0;
  }

  bool isBusy()
  {
    i32ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1);
    // Check for Write in Progress (WIP)
    if (mu8FlashStatus[0] & 1) return True;
    return False;
  }

  bool isWriteEnable()
  {
    return (mu8FlashStatus[0] & 2) == 2;
  }


  i32 i32WriteEnable()
  {
    if (!isWriteEnable())
    {
      OSPI_RegularCmdTypeDef sCommand;
      cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

      /* Enable write operations ------------------------------------------ */
      sCommand.Instruction = 06; // Write Enable
      sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

      if (HAL_OSPI_Command(&mstOSpi, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        return -1;
      }

      if (i32AutoPollingStatus(2, 2)) // Write Enable
      {
        vSetError();
        return -1;
      }
    }
    return 0;
  }


  i32 i32EraseChip()
  {
    OSPI_RegularCmdTypeDef sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    if (i32WriteEnable())
    {
      vSetError();
      return -1;
    }

    sCommand.Instruction = 0xC7;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

    if (HAL_OSPI_Command(&mstOSpi, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    if (i32AutoPollingStatus(0, 1) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    return 0;
  }


  // see page 84
  i32 i32ResetMemory()
  {
    OSPI_RegularCmdTypeDef  sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    sCommand.Instruction     = 0x66; // Reset
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    sCommand.Instruction     = 0x99; // Reset
    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }
    return 0;
  }

  i32 i32ReadStatus()
  {
    mu8FlashStatus[0] = mu8FlashStatus[1] = mu8FlashStatus[2] = 0xFF;

    if (i32AutoPollingStatus(0, 0x01))     // Write in progress
    {
      vSetError();
      return -1;
    }

    // Read Status Register-1
    if (i32ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1))
    {
      vSetError();
      return -1;
    }

    // Read Status Register-2
    if (i32ReadSingleData(0x35, (u8*)&mu8FlashStatus[1], 1))
    {
      vSetError();
      return -1;
    }

    // Read Status Register-3
    if (i32ReadSingleData(0x15, (u8*)&mu8FlashStatus[2], 1))
    {
      vSetError();
      return -1;
    }
    return 0;
  }


  i32 i32ReadId()
  {
    if (i32ReadSingleData(0x9F, (u8*)&mu32FlashID, 3))
    {
       vSetError();
       return -1;
    }

    if (!isKnownId(mu32FlashID))
    {
      vSetError();
      return -1;
    }
    return 0;
  }


  i32 i32SetupQuadSpi()
  {
    UNUSED(&mstOSpi);

    // The Quad Enable(QE) bit is a non - volatile read / write bit in the status register (S9)that allows Quad SPI
    // and QPI operation.When the QE bit is set to a 0 state(factory default for part numbers with ordering options
    // “IM”& “JM”), the / WP pin and / HOLD are enabled.When the QE bit is set to a 1(factory default for Quad
    //  Enabled part numbers with ordering option “IQ”& “JQ”), , the Quad IO2and IO3 pins are enabled, and / WP
    //  and / HOLD functions are disabled.
    if (mu8FlashStatus[1] & 2)
    {
      // Nothing todo, QE is already set
    }
    else
    {
      OSPI_RegularCmdTypeDef  sCommand;
      cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

      // Set QE
      mu8FlashStatus[1] |= 2;

      if (i32WriteEnable() != HAL_OK)
      {
        vSetError();
        return -1;
      }

      sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
      sCommand.Instruction        = 0x01;   // Write Status Register
      sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
      sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
      sCommand.NbData             = 2;


      /* Configure the command */
      if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        vSetError();
        return -1;
      }

      if (HAL_OSPI_Transmit(&mstOSpi, (u8*)mu8FlashStatus, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        vSetError();
        return -1;
      }

      if (i32AutoPollingStatus(0, 0x01))     // Write in progress
      {
        vSetError();
        return -1;
      }

      if (i32ReadStatus())
      {
        vSetError();
        return -1;
      }

    }
    return 0;
  }

  i32 i32SetupDummyCycles()
  {
    // NA for W25Q64JV

    // Fast Read(0Bh)
    //   The Fast Read instruction is similar to the Read Data instruction except that it can operate at the highest
    //   possible frequency of FR(see AC Electrical Characteristics).This is accomplished by adding eight “dummy”
    //   clocks after the 24 - bit address as shown in Figure 16. The dummy clocks allow the devices internal circuits
    //   additional time for setting up the initial address.During the dummy clocks the data value on the DO pin is
    //   a “don’t care”.

    // See C0h

    return 0;
  }


  i32 i32SetupQuadDDRMode()
  {
    // NA for W25Q64JV
    // DTR not need to be activated. It is controlled via Cmd.
    // DTR Fast Read Quad: EDh:  24Bit Adr: 8Bit Dummy: nx 8 Bit Data
    return 0;
  }


  i32 i32EnableMemoryMappedMode()
  {
    OSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
    OSPI_RegularCmdTypeDef sCommand;

    cMemTools::vMemSet((u8*)&s_mem_mapped_cfg, 0, sizeof(s_mem_mapped_cfg));
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    /* Initialize memory-mapped mode for read operations */
    sCommand.OperationType      = HAL_OSPI_OPTYPE_READ_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
    sCommand.DummyCycles        = 6;
    sCommand.Instruction        = 0xEB; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)

    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    sCommand.OperationType      = HAL_OSPI_OPTYPE_WRITE_CFG;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.DummyCycles        = 0;
    sCommand.Instruction        = 0x32; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)

    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }


    /* Configure the memory mapped mode with TimeoutCounter Disabled*/
    s_mem_mapped_cfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
    s_mem_mapped_cfg.TimeOutPeriod = 10;
    if (HAL_OSPI_MemoryMapped(&mstOSpi, &s_mem_mapped_cfg) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    return 0;
  }

  i32 i32Write_Start(u8* buffer, u32 address, u32 buffer_size)
  {

    OSPI_RegularCmdTypeDef  sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    if (i32WriteEnable() != HAL_OK)
    {
      vSetError();
      return -1;
    }

    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction        = 0x32;   // Quad Input Page Program
                                          // 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + Data (4 Lines)
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData             = buffer_size;
    sCommand.Address            = address;


    /* Configure the command */
    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    if (HAL_OSPI_Transmit(&mstOSpi, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }
    return 0;
  }

  i32 i32Read(u8* pData, u32 ReadAddr, u32 Size)
  {
    OSPI_RegularCmdTypeDef  sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    /* Initialize the read command */
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction        = 0xEB; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.Address            = ReadAddr;
    sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
    sCommand.DummyCycles        = 6U;
    sCommand.NbData             = Size;

    /* Configure the command */
    if (HAL_OSPI_Command(&mstOSpi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    if (HAL_OSPI_Receive(&mstOSpi, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }

    return 0;
  }


  i32 i32EraseSectorStart(u32 u32PageAddr)
  {
    OSPI_RegularCmdTypeDef  sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    if (i32WriteEnable() != HAL_OK)
    {
      //Error_Handler();
      return 1;
    }

    /* Initialize the read command */
    sCommand.Instruction        = 0x20; // Sector Erase (SE) (20H)
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.Address            = u32PageAddr;

    if (HAL_OSPI_Command(&mstOSpi, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      vSetError();
      return -1;
    }
    return 0;
  }

  void vDoProcess(u32 u32DiffTime_us)
  {
    bool lbLoop;
    mu32Delay_us += u32DiffTime_us;
    do
    {
      lbLoop = False;
      switch (menState)
      {
        case StInitMissing:
          break;
        case StIdle:
          break;

        case StError:
          break;

        case StInit1:
          vInit();
          menState = StInit2;
          lbLoop = True;
          break;
        case StInit2:
          i32ResetMemory();
          mu32Delay_us = 0;
          menState = StInit3;
          break;
        case StInit3:
          if (mu32Delay_us >= 30)
          {
            i32ReadStatus();
            menState = StInit4;
          }
          break;
        case StInit4:
          i32SetupQuadSpi();
          menState = StInit5;
          break;
        case StInit5:
          i32SetupDummyCycles();
          menState = StInit6;
          break;
        case StInit6:
          i32SetupQuadDDRMode();
          lbLoop = True;
          menState = StInit7;
          break;
        case StInit7:
          mbInit = True;
          menState = StIdle;
          break;

        case StWrite1:
          i32Write_Start(mu32WorkAdrRam, mu32WorkAdrFlash, mu32WorkSize);
          menState = StWrite2;
          break;
        case StWrite2:
          if (!isBusy())
          {
            menState = StIdle;
          }
          break;
        case StWrite3:
          break;
        case StWrite4:
          break;

        case StRead1:
          i32Read(mu32WorkAdrRam, mu32WorkAdrFlash, mu32WorkSize);
          menState = StIdle;
          break;
        case StRead2:
          break;
        case StRead3:
          break;
        case StRead4:
          break;

        case StEraseSector1:
          i32EraseSectorStart(mu32WorkAdrFlash);
          menState = StEraseSector2;
          break;
        case StEraseSector2:
          if (!isBusy())
          {
            mu32WorkSize -= mu32FlashSectorSize;
            if (mu32WorkSize > 0)
            {
              menState = StEraseSector1;
            }
            else
            {
              menState = StEraseSector3;
            }
            lbLoop = True;
          }
          break;
        case StEraseSector3:
          menState = StIdle;
          break;
        case StEraseSector4:
          break;

        case StEraseChip1:
          i32EraseChip();
          menState = StEraseChip2;
          break;
        case StEraseChip2:
          if (!isBusy())
          {
            menState = StEraseChip3;
            lbLoop = True;
          }
          break;
        case StEraseChip3:
          menState = StIdle;
          break;
        case StEraseChip4:
          break;
      }

      if (mu8Error != 0)
      {
        menState = StError;
      }

    } while (lbLoop);
  }

  i8 i8StartInit()
  {
    if ((menState == StIdle) ||
        (menState == StError))
    {
      menState = StInit1;
    }
    return FRET_OK;
  }

  i8 i8StartEraseSectors(u32 lu32StartAdr, u32 lu32Size)
  {
    if (isIdle())
    {
      if (lu32StartAdr > mu32FlashBaseAdr)
      {
        lu32StartAdr    -= mu32FlashBaseAdr;
        mu32WorkAdrFlash = lu32StartAdr & 0xFFFFF000;
        mu32WorkAdrRam   = 0;
        mu32WorkSize     = lu32Size;
        menState         = StEraseSector1;
        return FRET_OK;
      }
    }
    return FRET_ERR_GENERAL;
  }

  i8 i8StartRead(u32 lu32SrcAdr, u8* lu8DstAdr, u32 lu32Size)
  {
    if (isIdle())
    {
      if (lu32SrcAdr > mu32FlashBaseAdr)
      {
        lu32SrcAdr      -= mu32FlashBaseAdr;
        mu32WorkAdrFlash = lu32SrcAdr;
        mu32WorkAdrRam   = lu8DstAdr;
        mu32WorkSize     = lu32Size;
        menState         = StRead1;
        return FRET_OK;
      }
      return FRET_OK;
    }
    return FRET_ERR_GENERAL;
  }

  i8 i8StartWrite(u8* lu8SrcAdr, u32 lu32DstAdr, u32 lu32Size)
  {
    if (isIdle())
    {
      if (lu32DstAdr > mu32FlashBaseAdr)
      {
        lu32DstAdr -= mu32FlashBaseAdr;
        mu32WorkAdrFlash = lu32DstAdr;
        mu32WorkAdrRam   = lu8SrcAdr;
        mu32WorkSize     = lu32Size;
        menState = StWrite1;
        return FRET_OK;
      }
    }
    return FRET_ERR_GENERAL;
  }

  bool isIdle()
  {
    if ((menState == StIdle) &&
        (menState != StError))
    {
      return True;
    }
    return False;
  }
};



#endif // __OSPI_ZD25WQ32_H__
