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

    Driver/ARM/Cmsis/V05.06.00/Core:CM0plus
    Driver/ATMEL/SAMD10/v00.00.01/core
    Driver/ATMEL/SAMD10/v00.00.01/drivers

    Driver/Atmel/SAMD10/GPPin/v01.00.02

    APP/LED/v00.00.01:SAMD10
    #System/CyclicCaller/v00.00.01:GD32f130_TIM2
    #System/STM32/ClockInfo/v00.00.01:GD32f130_8Mhz

    main/ATMEL/SAMD10/UbTech/Servo3/LedBlink
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/samd10d14.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm0p_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm0p_buildoptions.cmake")
