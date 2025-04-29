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
    General/TypeDef/v00.00.01:ArmCx

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F3/STD/v02.02.01/GD32F3x0/Core:default
    Driver/STM32/F3/STD/v02.02.01/GD32F3x0/STD:default
    Driver/STM32/F3/STD/GPPin/v01.00.02_GD

    APP/LED/v00.00.01:GD32F3_STD
    System/CyclicCaller/v00.00.01:GD32f3x0_TIM2
    System/STM32/ClockInfo/v00.00.01:GD32f3x0_8Mhz

    main/GD32F330G8/Mot/LedBlink
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/GD32F330G8_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
