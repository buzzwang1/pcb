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
    Driver/ARM/Cmsis/V05.06.00/Core:CM7
    Driver/STM32/H7/HAL/v01.11.00/Core
    Driver/STM32/H7/HAL/v01.11.00/Device/STM32H7B0xx:default
    Driver/STM32/H7/HAL/v01.11.00/HAL:default

    main/STM32H7/HalLib/v01.11.00
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/STM32H7B0xx_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm7_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm7_buildoptions.cmake")
