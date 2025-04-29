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

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03

    Data/String/Cli/v00.00.01
    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/L4/v01.15.01/Core
    Driver/STM32/L4/v01.15.01/Device/stm32l433xx:default
    Driver/STM32/L4/v01.15.01/HAL:lib_o3
    Driver/STM32/L4/GPPin/v01.00.02
    Driver/STM32/L4/cUart/Int/v01.00.01

    APP/LED/v00.00.01:STM32L4_HAL
    System/STM32/ClockInfo/v00.00.01:stm32l4xx

    main/STM32L433CCT/Tests/PowTest
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32l433cc_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
