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

    Data/Mem/MemPart/v00.00.01
    Data/Sys/Wuf/Def/v00.00.01:stm32U5xx
    Data/Mem/BuRam/Def/v00.00.01:STM32U5xx:MB
    Data/Mem/RomConst/v00.00.02:STM32U5xx:Miniv28b2:default

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/Hal/v01.01.00/Core
    Driver/STM32/U5/Hal/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/U5/Hal/v01.01.00/HAL:lib_o3_32Mhz

    ExtLibs/Memory/uzlib/v01.00.00_STM32U5_Hack

    main/${Main_Location}
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
