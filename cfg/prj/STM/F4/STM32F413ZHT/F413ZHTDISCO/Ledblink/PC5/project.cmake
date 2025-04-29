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

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F4/v01.07.11/Core
    Driver/STM32/F4/v01.07.11/Device/stm32f413xx:default
    Driver/STM32/F4/v01.07.11/HAL:default
    Driver/STM32/F4/GPPin/v01.00.02

    System/CyclicCaller/v00.00.01:stm32f4xx_Hal_TIM3
    System/STM32/ClockInfo/v00.00.01:stm32f4xx_hal

    APP/LED/v00.00.01:STM32F4_HAL
    main/${Main_Location}
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f413zht_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
