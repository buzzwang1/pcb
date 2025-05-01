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

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/L5/HAL/v01.00.04/Core
    Driver/STM32/L5/HAL/v01.00.04/Device/STM32l552xx:default
    Driver/STM32/L5/HAL/v01.00.04/HAL:default

    Driver/STM32/L5/HAL/GPPin/v01.00.02

    APP/LED/v00.00.01:STM32L5_HAL
    System/CyclicCaller/v00.00.01:stm32l5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32l5xx

    main/STM32L522ZET/LedBlink/PB7
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32l522ZET_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
