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

set(PCB_ProjectPackageList
    General/TypeDef/v00.00.01:RiscV

    Driver/Ch32/V3/STD/v01.00.00/Core:default
    Driver/Ch32/V3/STD/v01.00.00/STD:default
    Driver/Ch32/V3/STD/GPPin/v01.00.00

    APP/LED/v00.00.01:CH32V3_STD
    System/CyclicCaller/v00.00.01:CH32v3xx_SysTick
    System/STM32/ClockInfo/v00.00.01:CH32V3xx

    main/CH32V305/Mot/LedBlink
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/CH32V305_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
