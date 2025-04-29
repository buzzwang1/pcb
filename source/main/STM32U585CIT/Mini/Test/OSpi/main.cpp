#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;

// STM32U5XX
// ARM®-based Cortex®-M33 32b MCU
// Rom 1024KB/2048KB
// Ram 768KB
// 160Mhz


//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
//
//  PB09  -> Status Led
//
//  PA00   -> Wakeup (100k Pull Down)
//  PA08   -> reserve
//  PB01   -> reserve
//  PB12   -> reserve
//
//  USART1 -> BotCom ComPort (19200)
//  ---
//  ---
//
//  Power Control
//    Battery Control:
//      Balancer:
//      PB00  -> U_Bat4
//      PA06  -> U_Bat3
//      PB02  -> U_Bat2
//      PA15  -> U_Bat1
//
//    Regulator In:
//      PA04  -> Dac
//      PA07  -> En
//
//    Regulator Out:
//      PA05  -> Dac
//      PB08  -> En
//      PA06  -> U_Sys: ADC_IN11
//
//   Switch:
//      PC13: Battery MainSwitch
//      PA10: LowPower(3V3, ADC, I2C)
//
//
//  I2C1  -> BotNet 1.0.0.0
//    PB06  -> I2C1 SCL
//    PB07  -> I2C1 SDA
//
//  U1    -> BotNet 1.E.0.0
//    PA09  -> U1 Tx/Rx
//
//  U2    -> Debug
//    PA02  -> U2 Tx
//    PA03  -> U2 Rx
//
//  I2C2  -> INA3221 2x, Display (Optional) , Apds9960 (Optional)
//    PB10  -> I2C2 SCL
//    PB11  -> I2C2 SDA
//
//
//        nRF905:            Display:
//  PB03: SPI1 Clock         SPI1 Clock
//  PB04: SPI1 MISO          SPI1 MISO
//  PB05: SPI1 MOSI          SPI1 MOSI
//  PA12: SPI1 Chip Select   SPI1 Display Chip Select
//  PB14: TX or RX mode      SPI1 Touch Chip Select
//  PA11: Standby            Dipslay Reset
//  PB15: Power up           Display DC
//  PB13: Data Ready         Touch IRQ
//
//
//  QSPI_P1:
//    PE10 => CLK
//    PE11 => NCS
//    PE12 => IO0
//    PE13 => IO1
//    PE14 => IO2
//    PE15 => IO3
//

LED<GPIOB_BASE, 9> mcLedRed;
//LED<GPIOA_BASE, 8> mcPA08;
//LED<GPIOB_BASE, 1> mcPB01;
//LED<GPIOB_BASE, 12> mcPB12;

//cGpPin mLowPower(GPIOA_BASE, 10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
//cGpPin mMod2_En(GPIOB_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
//cGpPin mLed_En(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cClockInfo mcClkInfo;


//SRAM1: 192k: 0x2000.0000 - 0x2002.FFFF
//SRAM2:  64k: 0x2003.0000 - 0x2003.FFFF
//SRAM3: 512k: 0x2004.0000 - 0x200B.FFFF

cDiffTimer                               mcDiffTimer;


OSPI_HandleTypeDef hospi1;
/* W25XXXÏµÁÐ */
#define W25X10_FLASH_ID            0x1130EF  // 1M bit
#define W25X20_FLASH_ID            0x1230EF  // 2M bit
#define W25X40_FLASH_ID            0x1330EF  // 4M bit
#define W25X80_FLASH_ID            0x1440EF  // 8M bit
#define W25Q16_FLASH_ID1           0x1530EF  // 16M bit
#define W25Q16_FLASH_ID2           0x1540EF  // 16M bit
#define W25Q32_FLASH_ID1           0x1640EF  // 32M bit
#define W25Q32_FLASH_ID2           0x1660BA  // 32M bit
#define W25Q64_FLASH_ID1           0x1740EF  // 64M bit
#define W25Q64_FLASH_ID2           0x1760EF  // 64M bit
#define W25Q128_FLASH_ID1          0x1840EF  // 128M bit
#define W25Q128_FLASH_ID2          0x1860EF  // 128M bit
#define W25Q256_FLASH_ID1          0x1940EF  // 256M bit
#define W25Q256_FLASH_ID2          0x1960EF  // 256M bit
#define SPI_FLASH_SectorSize       4096


// Zetta - ZD25WQ32CEIGR
// Page Size 256B(CR.QP = 0)
// 64 Blocks  je 64kB = > 4MB
// 16 Sectors je  4KB = > 64kb
// 16 Pages   je 256B = > 4kB
//
// 10ms 4K - byte sector erase time
// 2ms page programming time
// = > 4MB: 32768ms
// = > 1MB:  8192ms
// = > 100kB:   800ms


// Gemessen STM32U575 160Mhz OSPI 80Mhz
//
//
//  Chip   Erase:         11140us
//  Sector Erase:         11140us
//  Sector Write Single:  2160us
//  Sector Write Quad:    2160us
//  Sector Read Single:    618us
//  Sector Read Dual:      618us 
//  Sector Read Quad:      618us
//



volatile u32 mu32FlashID;
volatile u32 mu32FlashSize;
volatile u32 mu32FlashSectorCount;
volatile u32 mu32FlashSectorSize;
volatile u8  mu8FlashStatus[3];


int32_t OSPI_ReadSingleData(u8 lu8Instruction, u8* lpu8Dest, u8 lu8Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef s_command;

  cMemTools::vMemSet((u8*)&s_command, 0, sizeof(s_command));

  s_command.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.Instruction = lu8Instruction;
  s_command.AddressMode = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = HAL_OSPI_DATA_1_LINE;
  s_command.DummyCycles = 0;
  s_command.NbData = lu8Size; // Width of useful ID
  s_command.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(hospi1, lpu8Dest, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }
  return 0;
}

// Bit S0: Busy
// Bit S1: WEL Write Enable
int32_t OSPI_AutoPollingStatus(OSPI_HandleTypeDef* hospi1, u8 lu8Match, u8 lu8Mask)
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
  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  sConfig.Match         = lu8Match;
  sConfig.Mask          = lu8Mask;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = 0x10;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi1, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

static bool OSPI_isWriteEnable(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);
  return  (mu8FlashStatus[0] & 2) == 2;
}

static bool OSPI_isBusy(OSPI_HandleTypeDef* hospi1)
{
  OSPI_ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1, hospi1);
  // Check for Write in Progress (WIP)
  if (mu8FlashStatus[0] & 1) return True;
  return False;
}


static u8 OSPI_WriteEnable(OSPI_HandleTypeDef* hospi1)
{
  if (!OSPI_isWriteEnable(hospi1))
  {
    OSPI_RegularCmdTypeDef sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    /* Enable write operations ------------------------------------------ */
    sCommand.Instruction = 06; // Write Enable
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

    if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return -1;
    }

    if (OSPI_AutoPollingStatus(hospi1, 2, 2)) // Write Enable
    {
      return -1;
    }
  }

  return 0;
}



u8 OSPI_Erase_Chip(OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));


  if (OSPI_WriteEnable(hospi1))
  {
    return -1;
  }

  sCommand.Instruction = 0xC7;
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (OSPI_AutoPollingStatus(hospi1, 0, 1) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


// see page 84
int32_t OSPI_ResetMemory(OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  sCommand.Instruction     = 0x66; // Reset
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;

  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
    return 1;
  }

  sCommand.Instruction     = 0x99; // Reset
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
    return 1;
  }

  // Wait at least tRST: 30us
  for (uint32_t temp = 0; temp < 500000; temp++)
  {
    __NOP();
  }

  if (OSPI_AutoPollingStatus(hospi1, 0, 0x01))     // Write in progress
  {
    //Error_Handler();
  }

  // Read Status Register-1
  if (OSPI_ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1, hospi1))
  {
    //Error_Handler();
  }

  // Read Status Register-2
  if (OSPI_ReadSingleData(0x35, (u8*)&mu8FlashStatus[1], 1, hospi1))
  {
    //Error_Handler();
  }

  // Read Status Register-3
  if (OSPI_ReadSingleData(0x15, (u8*)&mu8FlashStatus[2], 1, hospi1))
  {
    //Error_Handler();
  }

  return 0;
}


int32_t OSPI_ReadId(OSPI_HandleTypeDef* hospi1)
{
  if (OSPI_ReadSingleData(0x9F, (u8*)&mu32FlashID, 3, hospi1))
  {
    //Error_Handler();
    __asm("NOP");
  }

  mu32FlashSize = 0;
  mu32FlashSectorCount = 0x00;
  mu32FlashSectorSize = 0x00;

  switch (mu32FlashID)
  {
    /* W25XXXÏµÁÐÐ¾Æ¬ */
  case W25X10_FLASH_ID:    /* 0xEF3011-----1M bit */
    mu32FlashSize = 1;
    break;

  case W25X20_FLASH_ID:    /* 0xEF3012-----2M bit */
    mu32FlashSize = 2;
    break;

  case W25X40_FLASH_ID:    /* 0xEF3013-----4M bit */
    mu32FlashSize = 4;
    break;

  case W25X80_FLASH_ID:    /* 0xEF4014-----8M bit */
    mu32FlashSize = 8;
    break;

  case W25Q16_FLASH_ID1:   /* 0xEF3015-----16M bit */
  case W25Q16_FLASH_ID2:   /* 0xEF4015-----16M bit */
    mu32FlashSize = 16;
    break;

  case W25Q32_FLASH_ID1:   /* 0xEF4016-----32M bit */
  case W25Q32_FLASH_ID2:   /* 0xEF6016-----32M bit */
    mu32FlashSize = 32;
    break;

  case W25Q64_FLASH_ID1:   /* 0xEF4017-----64M bit */
  case W25Q64_FLASH_ID2:   /* 0xEF6017-----64M bit */
    mu32FlashSize = 64;
    break;

  case W25Q128_FLASH_ID1:  /* 0xEF4018-----128M bit */
  case W25Q128_FLASH_ID2:  /* 0xEF6018-----128M bit */
    mu32FlashSize = 128;
    break;

  case W25Q256_FLASH_ID1:  /* 0xEF4019-----256M bit */
  case W25Q256_FLASH_ID2:  /* 0xEF6019-----256M bit */
    mu32FlashSize = 256;
    break;
  default:
    if ((mu32FlashID != 0xFFFFFFFF) || (mu32FlashID != 0x00000000))
    {
      mu32FlashSize = 16;
    }
    else
    {
      mu32FlashSize = 0x00;
    }
    break;
  }
  mu32FlashSize = ((uint32_t)mu32FlashSize * 1024) * ((uint32_t)1024 / 8);

  if (mu32FlashSize)
  {
    mu32FlashSectorCount = mu32FlashSize / SPI_FLASH_SectorSize;
    mu32FlashSectorSize = SPI_FLASH_SectorSize;
  }

  return 0;
}


int32_t OSPI_SetupQuadSpi(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);

  // The Quad Enable(QE) bit is a non - volatile read / write bit in the status register (S9)that allows Quad SPI
  // and QPI operation.When the QE bit is set to a 0 state(factory default for part numbers with ordering options
  // “IM”& “JM”), the / WP pin and / HOLD are enabled.When the QE bit is set to a 1(factory default for Quad
  //  Enabled part numbers with ordering option “IQ”& “JQ”), , the Quad IO2and IO3 pins are enabled, and / WP
  //  and / HOLD functions are disabled.
  if (mu8FlashStatus[1] & 2)
  {
    // Nothing todo, QE is already set
  }
  {
    OSPI_RegularCmdTypeDef  sCommand;
    cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

    // Set QE
    mu8FlashStatus[1] |= 2;

    if (OSPI_WriteEnable(hospi1) != HAL_OK)
    {
      //Error_Handler();
      __asm("NOP");
      return 1;
    }

    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.Instruction        = 0x01;   // Write Status Register
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData             = 2;
    sCommand.Address            = 0;
    sCommand.DummyCycles        = 0;

    /* Configure the command */
    if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      __asm("NOP");
      return -1;
    }

    if (HAL_OSPI_Transmit(hospi1, (u8*)mu8FlashStatus, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      __asm("NOP");
      return -1;
    }

    if (OSPI_AutoPollingStatus(hospi1, 0, 0x01))     // Write in progress
    {
      //Error_Handler();
      __asm("NOP");
    }
    __asm("NOP");

    mu8FlashStatus[0] = mu8FlashStatus[1] = mu8FlashStatus[2] = 0xFF;
    // Read Status Register-1
    if (OSPI_ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1, hospi1))
    {
      //Error_Handler();
      __asm("NOP");
    }

    // Read Status Register-2
    if (OSPI_ReadSingleData(0x35, (u8*)&mu8FlashStatus[1], 1, hospi1))
    {
      //Error_Handler();
      __asm("NOP");
    }

    // Read Status Register-3
    if (OSPI_ReadSingleData(0x15, (u8*)&mu8FlashStatus[2], 1, hospi1))
    {
      //Error_Handler();
      __asm("NOP");
    }
  }
  return 0;
}

int32_t OSPI_SetupDummyCycles(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);
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


int32_t OSPI_SetupQuadDDRMode(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);
  // NA for W25Q64JV
  // DTR not need to be activated. It is controlled via Cmd.
  // DTR Fast Read Quad: EDh:  24Bit Adr: 8Bit Dummy: nx 8 Bit Data
  return 0;
}



int32_t OSPI_EnableMemoryMappedMode(OSPI_HandleTypeDef* hospi1)
{
  OSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
  OSPI_RegularCmdTypeDef sCommand;

  cMemTools::vMemSet((u8*)&s_mem_mapped_cfg, 0, sizeof(s_mem_mapped_cfg));
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  /* Initialize memory-mapped mode for read operations */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_READ_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.DummyCycles        = 6;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  sCommand.Instruction        = 0xEB; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;

  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }


  sCommand.OperationType      = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.DummyCycles        = 0;
  sCommand.Instruction        = 0x32; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)

  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }


  /* Configure the memory mapped mode with TimeoutCounter Disabled*/
  s_mem_mapped_cfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
  s_mem_mapped_cfg.TimeOutPeriod = 10;
  if (HAL_OSPI_MemoryMapped(hospi1, &s_mem_mapped_cfg) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }


  return 0;
}

int32_t OSPI_Write_Single(u8* buffer, uint32_t address, uint32_t buffer_size, OSPI_HandleTypeDef* hospi1)
{

  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));
  //uint32_t end_addr, current_size, current_addr;

  if (OSPI_WriteEnable(hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }

  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = 0x2;   // Singe Input Page Program
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.NbData             = buffer_size;
  sCommand.Address            = address;
  sCommand.DummyCycles        = 0;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (HAL_OSPI_Transmit(hospi1, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (OSPI_AutoPollingStatus(hospi1, 0, 0x01))     // Write in progress
  {
    //Error_Handler();
    __asm("NOP");
  }
  __asm("NOP");
  return 0;
}

int32_t OSPI_Write_Quad(u8* buffer, uint32_t address, uint32_t buffer_size, OSPI_HandleTypeDef* hospi1)
{

  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  if (OSPI_WriteEnable(hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }

  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = 0x32;   // Quad Input Page Program
                                        // 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.NbData             = buffer_size;
  sCommand.Address            = address;
  sCommand.DummyCycles        = 0;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (HAL_OSPI_Transmit(hospi1, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (OSPI_AutoPollingStatus(hospi1, 0, 0x01))     // Write in progress
  {
    //Error_Handler();
    __asm("NOP");
  }
  __asm("NOP");
  return 0;
}

int32_t OSPI_WriteMemory(u8* buffer, uint32_t address, uint32_t buffer_size, OSPI_HandleTypeDef* hospi1)
{
  return OSPI_Write_Single(buffer, address, buffer_size, hospi1);
  //return OSPI_Write_Quad(buffer, address, buffer_size, hospi1);
}


u8 OSPI_Read_Single(u8* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction        = 0x0B; // Fast Read I/O => 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + 8Bit Dummy (1 Lines) + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytes     = 0;
  sCommand.AlternateBytesSize = 1;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 8U;
  sCommand.NbData             = Size;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (HAL_OSPI_Receive(hospi1, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


u8 OSPI_Read_Dual(u8* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction        = 0x3B; // Dual Fast Read => 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + 8 Cycles Dummy + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytes     = 0;
  sCommand.AlternateBytesSize = 1;
  sCommand.DataMode           = HAL_OSPI_DATA_2_LINES;
  sCommand.DummyCycles        = 8U;
  sCommand.NbData             = Size;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (HAL_OSPI_Receive(hospi1, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

u8 OSPI_Read_Quad(u8* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction        = 0x6B; // Quad Fast Read => 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + 8 Cycles Dummy + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytes     = 0;
  sCommand.AlternateBytesSize = 1;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.DummyCycles        = 8U;
  sCommand.NbData             = Size;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (HAL_OSPI_Receive(hospi1, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

u8 OSPI_Read_Quad_IO(u8* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction        = 0xEB; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 2 + 4 Cycles Dummy + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytes     = 0;
  sCommand.AlternateBytesSize = 1;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.DummyCycles        = 6U;
  sCommand.NbData             = Size;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (HAL_OSPI_Receive(hospi1, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}



u8 OSPI_Erase_Sector_Start(uint32_t u32PageAddr, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;
  cMemTools::vMemSet((u8*)&sCommand, 0, sizeof(sCommand));

  if (OSPI_WriteEnable(hospi1) != HAL_OK)
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

  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }
  return 0;
}

u8 OSPI_Erase_Sector(uint32_t u32PageAddr, OSPI_HandleTypeDef* hospi1)
{
  if (OSPI_Erase_Sector_Start(u32PageAddr, hospi1) != HAL_OK)
  {
    return -1;
  }

  if (OSPI_AutoPollingStatus(hospi1, 0, 1) != HAL_OK)
  {
    return -1;
  }

  return 0;
}



u8 OSPI_Read(u8* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  //return OSPI_Read_Single(pData, ReadAddr, Size, hospi1);
  //return OSPI_Read_Dual(pData, ReadAddr, Size, hospi1);
  //return OSPI_Read_Quad(pData, ReadAddr, Size, hospi1);
  return OSPI_Read_Quad_IO(pData, ReadAddr, Size, hospi1);
}


/* USER CODE END OCTOSPI1_Init 2 */


void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  if (ospiHandle->Instance == OCTOSPI1)
  {
    /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

    /* USER CODE END OCTOSPI1_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      //Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PE10    ------> OCTOSPIM_P1_CLK
    PE11    ------> OCTOSPIM_P1_NCS
    PE12    ------> OCTOSPIM_P1_IO0
    PE13    ------> OCTOSPIM_P1_IO1
    PE14    ------> OCTOSPIM_P1_IO2
    PE15    ------> OCTOSPIM_P1_IO3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

    /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}


/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{
  // https://www.youtube.com/watch?v=JAlMxgjlSkE

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg;
  cMemTools::vMemSet((u8*)&sOspiManagerCfg, 0, sizeof(sOspiManagerCfg));

  // STM32H7 Core clock: 280Mhz
  //         OSPI clock: 140Mhz
  //
  // HAL_OSPI WinBond W25Q64JV-DTR 8MB = 2^23 = 8388608
  // Mircon Mode, weil Daten komment als u8 (D0 D1 D2 D3) und nicht als u16 (D1 D0 D3 D2) raus
  //
  //

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 23;         // 8MB = 2^23 = 8388608

  //hospi1.Init.ClockPrescaler        = 2;    // 160Mhz / 2 = 80,00Mhz
  //hospi1.Init.ClockPrescaler        = 3;    // 160Mhz / 3 = 53,33Mhz
  //hospi1.Init.ClockPrescaler        = 4;    // 160Mhz / 4 = 40,00Mhz
  hospi1.Init.ClockPrescaler = 2;    // 160Mhz / 2 = 80Mhz
  //hospi1.Init.ChipSelectHighTime    = 4;   // t_SHSL2 = 50ns (DS S.89), 160Mhz / 4(Prescale) = 40Mhz    => 25.00ns; 25.00ns * 2 = 50ns
  //hospi1.Init.ChipSelectHighTime    = 3;   // t_SHSL2 = 50ns (DS S.89), 160Mhz / 3(Prescale) = 53,33Mhz => 18.75ns; 18.75ns * 3 = 56ns
  hospi1.Init.ChipSelectHighTime = 4;        // t_SHSL2 = 50ns (DS S.89), 160Mhz / 2(Prescale) = 80Mhz    => 12.50ns; 12.50ns * 4 = 50ns

  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0; // Mode 0 = Low Level; Mode 3 = High Level; W25Q64JV supports both (DS S.10)
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE; // HAL_OSPI_DHQC_DISABLE;   HAL_OSPI_DHQC_ENABLE is recommended when using DTR-Mode
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 0;


  HAL_OSPI_MspInit(&hospi1);


  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    asm("NOP");
  }



  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  if (OSPI_ResetMemory(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }


  if (OSPI_ReadId(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }

  if (OSPI_SetupQuadSpi(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }


  if (OSPI_SetupDummyCycles(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }

  if (OSPI_SetupQuadDDRMode(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }

  //if (OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
  //{
  //  //Error_Handler();
  //  __asm("NOP");
  //}
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if (ospiHandle->Instance == OCTOSPI1)
  {
    /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

    /* USER CODE END OCTOSPI1_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_OSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PE2     ------> OCTOSPIM_P1_IO2
    PB2     ------> OCTOSPIM_P1_CLK
    PD11     ------> OCTOSPIM_P1_IO0
    PD12     ------> OCTOSPIM_P1_IO1
    PD13     ------> OCTOSPIM_P1_IO3
    PB6     ------> OCTOSPIM_P1_NCS
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);

    /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

    /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}


void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}


void SysTick_Handler(void)
{
  HAL_IncTick();
}



/*
static void Error_Handler(void)
{
  while (1)
  {
  }
}*/


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(u8 *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



u32 mu32SystemTime = 0;

void MAIN_vTick10msHp(void)
{
}


void MAIN_vTick10msLp(void)
{
}


void MAIN_vTick1000msLp(void)
{
  static uint8 lui8t = 0;
  mcLedRed.Toggle();
  lui8t++;
}


void MAIN_vInitSystem(void)
{
    mcClkInfo.Update();

    SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
    HAL_Init();
    MX_OCTOSPI1_Init();

    CycCall_Start(NULL /*1ms_HP*/,
                  MAIN_vTick10msHp /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
}

volatile u32 mu32Cmd = 0;
volatile u32 mu32Adr = 0;
volatile u8  mu8MyPage1[4096] = "Hello world.";
volatile u8  mu8MyPage2[4096];
volatile u32 mu32DiffTime = 0;
volatile u32 mu32DiffTime2[40] = {0};


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("NOP");
    __asm("NOP");
    mcDiffTimer.start();

    switch (mu32Cmd)
    {
      case 0:
        break;
      case 1:
        if (OSPI_ResetMemory(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 2:
        if (OSPI_ReadId(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 3:
        if (OSPI_SetupQuadSpi(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 4:
        if (OSPI_SetupDummyCycles(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 5:
        if (OSPI_SetupQuadDDRMode(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 6:
        if (OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 7:
        /* Clear busy bit */
        /* WARNING: Do not make any other memory-mapped access (even using debugger) */
        HAL_OSPI_Abort(&hospi1);

        mu32DiffTime = mcDiffTimer.get();
        break;


      case 10:
        if (OSPI_Erase_Chip(&hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 11:
        if (OSPI_Erase_Sector(mu32Adr, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 12:
        if (OSPI_Erase_Sector_Start(mu32Adr, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        while (OSPI_isBusy(&hospi1));
        mu32DiffTime = mcDiffTimer.get();
        break;


      case 20:
        if (OSPI_Write_Single((u8*)mu8MyPage1, mu32Adr, 4096, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 21:
        break;
      case 22:
        if (OSPI_Write_Quad((u8*)mu8MyPage1, mu32Adr, 4096, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 30:
        if (OSPI_Read_Single((u8*)mu8MyPage2, mu32Adr, 4096, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 31:
        if (OSPI_Read_Dual((u8*)mu8MyPage2, mu32Adr, 4096, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
      case 32:
        if (OSPI_Read_Quad_IO((u8*)mu8MyPage2, mu32Adr, 4096, &hospi1) != HAL_OK)
        {
          //Error_Handler();
          __asm("NOP");
        }
        mu32DiffTime = mcDiffTimer.get();
        break;
    }
    mu32DiffTime2[mu32Cmd] = mu32DiffTime;
    mu32Cmd = 0;
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

/**
 * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 160000000
  *            HCLK(Hz)                       = 160000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_MBOOST                     = 1
  *            PLL_M                          = 1
  *            PLL_N                          = 80
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 4
  */
static void SystemClock_Config(void)
{
  /* Enable PWR clock */
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

  /* Set the regulator supply output voltage */
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }

  /* Switch to SMPS regulator instead of LDO */
  //LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  //while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  //{
  //}

  /* Enable MSI oscillator */
  LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4); // => 4Mhz
  LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
  LL_RCC_MSIS_Enable();
  while (LL_RCC_MSIS_IsReady() != 1)
  {
  }

  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  /* Configure PLL clock source */
  LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

  /* Enable the EPOD to reach max frequency */
  LL_PWR_EnableEPODBooster();
  while (LL_PWR_IsActiveFlag_BOOST() == 0)
  {
  }

  /* Main PLL configuration and activation */
  // 4Mhz (MSI)  * 80 / 2 => 160Mhz
  LL_RCC_PLL1_EnableDomain_SYS();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
  LL_RCC_PLL1_SetDivider(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);

  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1)
  {
  }

  /* Set AHB, APB1, APB2 and APB3 prescalers */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  /* Set PLL1 as System Clock Source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  /* Set systick to 1ms with frequency set to 160MHz */
  LL_Init1msTick(160000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(160000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}





