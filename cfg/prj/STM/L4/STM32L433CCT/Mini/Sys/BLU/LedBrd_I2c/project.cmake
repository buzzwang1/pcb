# Projects settings,
#   - list list of used packages
#   - linker file
#   - whatever
# To select the packages, that should be used
#
# Is included from the [root]/CMakeList.txt
#
#
# Syntax module list: 
#  Package only  (without variants/configurations) [Folder relative to /pkg]
#  Package extra (with variants/configurations)    [Folder relative to /pkg]:[module variant/configuration]
#
#  examples:
#  uart                     # simplest
#  uart:B9600               # with configuration
#  uart:B19200              # 
#  driver/uart:var1         # with subfolder 


string(REGEX REPLACE "_" "/" Main_Location "${PCB_Project}")


if (PCB_ProjectCfg STREQUAL "Debug")
  set (lLibExtention "lib_og_24Mhz")
endif()

if (PCB_ProjectCfg STREQUAL "Release")
  set (lLibExtention "lib_os_24Mhz")
endif()

if (PCB_ProjectCfg STREQUAL "RamRelease")
  set (lLibExtention "lib_os_24Mhz")
endif()


set(PCB_ProjectPackageList
    General/TypeDef/v00.00.01:ArmCx

    Data/ComDat/ComDat/v00.00.07

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32L433CCT:Miniv10b2:default
    Data/Mem/BuRam/Def/v00.00.01:STM32L4:MB
    Data/Mem/BuRam/cBuRam/v00.00.01:STM32L4

    Data/String/CStrT/v00.00.01_NoFpu
    Data/String/StringTools/v00.00.02

    Data/BotNet/v00.00.06/Base/Main/v00.00.02:Blu:Mini
    Data/BotNet/v00.00.06/Base/Misc/Adr/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/Cfg/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/ErrCnt/v00.00.01:Dummy
    Data/BotNet/v00.00.06/Base/Misc/Spop/v00.00.02:STM32L433CCT
    Data/BotNet/v00.00.06/Base/Misc/Msg/v00.00.01:default
    Data/BotNet/v00.00.06/BnLinks/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnLinks/I2c/v00.00.05:default
    #Data/BotNet/v00.00.06/BnLinks/nRf905/v00.00.02:default
    #Data/BotNet/v00.00.06/BnLinks/nRf905Net/v00.00.03:default
    Data/BotNet/v00.00.06/BnMsgSys/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/Btr/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/Spop/v00.00.02:default

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/L4/HAL/v01.15.01/Core
    Driver/STM32/L4/HAL/v01.15.01/Device/stm32l433xx:default
    Driver/STM32/L4/HAL/v01.15.01/HAL:${lLibExtention}
    Driver/STM32/L4/HAL/GPPin/v01.00.02
    #Driver/STM32/L4/HAL/cUart/Int/v01.00.01
    #Driver/STM32/L4/HAL/cUart/MpHd/v01.00.02_Mini:Tim15
    Driver/STM32/L4/Hal/cI2C/v01.00.09

    #Driver/STM32/Device/Com/nRF905/Int/v01.00.00:MBv7b2_STM32L433CCT
    Driver/STM32/Device/EEP/v01.00.00:Dummy

    APP/LED/v00.00.01:STM32L4_HAL
    System/CyclicCaller/v00.00.01:stm32l4xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32l4xx

    APP/Job/JobHdl/v00.00.01
 
    main/STM32L433CCT/Mini/Sys/BLU/LedBrd_I2c
)


if (PCB_ProjectCfg STREQUAL "RamRelease")
  set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32l433cc_ramblu.ld")
else() 
  set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32l433cc_flash.ld")
endif()

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
                                                                                                                                                                                                                                                                                                                                                                                                                                                      