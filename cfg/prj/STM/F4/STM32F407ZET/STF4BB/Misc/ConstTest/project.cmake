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
    Driver/STM32/F4/STD/v01.08.00/Core/stm32f42x:default
    Driver/STM32/F4/STD/v01.08.00/STD:default:f40x:default
    Driver/STM32/F4/STD/GPPin/v01.00.00

    System/CyclicCaller/v00.00.01:stm32f4xx_Tim3
    System/STM32/ClockInfo/v00.00.01:stm32f4xx

    APP/LED/v00.00.01:STM32F4_STD
    main/STM32F407ZET/STF4BB/Misc/ConstTest
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f407zet_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
