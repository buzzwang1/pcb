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
    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/L4/HAL/v01.17.00/Core
    Driver/STM32/L4/HAL/v01.17.00/Device/stm32l4r9xx:default
    Driver/STM32/L4/HAL/v01.17.00/HAL:default

    main/STM32L4/HalLib/v01.17.00
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/STM32L4R9AIIx_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
