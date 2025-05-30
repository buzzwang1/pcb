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
  set (lLibExtention "lib_og_32Mhz")
endif()

if (PCB_ProjectCfg STREQUAL "Release")
  set (lLibExtention "lib_os_32Mhz")
endif()

if (PCB_ProjectCfg STREQUAL "RamRelease")
  set (lLibExtention "lib_os_32Mhz")
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
    Data/Mem/RomConst/v00.00.02:STM32U5xx:Miniv28b2:default
    Data/Mem/BuRam/Def/v00.00.01:STM32U5xx:MB
    Data/Mem/BuRam/cBuRam/v00.00.01:STM32U5xx

    Data/String/CStrT/v00.00.01_NoFpu
    Data/String/StringTools/v00.00.02

    Data/BotNet/v00.00.06/Base/Main/v00.00.02:Blu:Mini
    Data/BotNet/v00.00.06/Base/Misc/Adr/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/Cfg/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/ErrCnt/v00.00.01:Dummy
    Data/BotNet/v00.00.06/Base/Misc/Spop/v00.00.02:STM32U5xx
    Data/BotNet/v00.00.06/Base/Misc/Msg/v00.00.01:default
    Data/BotNet/v00.00.06/BnLinks/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnLinks/nRf905/v00.00.02:default
    Data/BotNet/v00.00.06/BnLinks/nRf905Net/v00.00.03:default
    Data/BotNet/v00.00.06/BnMsgSys/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/Btr/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/Spop/v00.00.02:default

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/Hal/v01.01.00/Core
    Driver/STM32/U5/Hal/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/U5/Hal/v01.01.00/HAL:lib_o3_32Mhz
    Driver/STM32/U5/Hal/GPPin/v01.00.02
    #Driver/STM32/U5/Hal/cUart/Int/v01.00.01
    Driver/STM32/U5/Hal/cI2C/v01.00.09

    Driver/STM32/Device/Com/nRF905/Int/v01.00.00:MBv28b2_STM32U5xx
    Driver/STM32/Device/EEP/v01.00.00:Dummy

    APP/LED/v00.00.01:STM32U5_HAL
    System/CyclicCaller/v00.00.01:stm32U5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32u5xx

    APP/Job/JobHdl/v00.00.01
 
    main/STM32U585CIT/Mini/Sys/BLU/MB_nrf905
)


if (PCB_ProjectCfg STREQUAL "RamRelease")
  set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_ramblu.ld")
else() 
  set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")
endif()

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
