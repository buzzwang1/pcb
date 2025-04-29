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

set(PCB_ProjectPackageList
    General/TypeDef/v00.00.01:ArmCx

    Data/ComDat/ComDat/v00.00.06

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.01:STM32L433CCT_MB

    Data/String/Cli/v00.00.01
    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Data/DateTime/Clock/v00.00.02:STM32L4

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/L4/v01.15.01/Core
    Driver/STM32/L4/v01.15.01/Device/stm32l433xx:default
    Driver/STM32/L4/v01.15.01/HAL:lib_o3
    Driver/STM32/L4/GPPin/v01.00.02
    Driver/STM32/L4/cUart/Int/v01.00.01
    Driver/STM32/L4/cI2C/v01.00.06

    Driver/STM32/Device/Com/nRF905/Int/v01.00.00:MB01_STM32L433CCT
    Driver/STM32/Device/INA219/v01.00.01

    System/CyclicCaller/v00.00.01:stm32l4xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32l4xx

    APP/LED/v00.00.01:STM32L4_HAL
    main/${Main_Location}
)

message(STATUS "Include linker file: ${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
