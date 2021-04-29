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
    Driver/ARM/Cmsis/V05.00.08/Dsp:CM4
    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F4/STD/v01.08.00/Core/stm32f42x:default
    Driver/STM32/F4/STD/v01.08.00/STD:default:f42x:default

    main/STM32F429ZIT6/F429IDISCO/Ex/01/10_MiniOszi
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f429zi_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
