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


set(Main_Location "STM32F407ZET/STF4BB/Ex/03_Key")

set(PCB_ProjectPackageList

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F4/STD/v01.08.00/Core/stm32f40x:default
    Driver/STM32/F4/STD/v01.08.00/STD:default:f40x:default


    Driver/STM32/Device/F407ZET_BB/System:NoOs
    Driver/STM32/Device/F407ZET_BB/hw/adc:Div2
    Driver/STM32/Device/F407ZET_BB/hw/beep
    Driver/STM32/Device/F407ZET_BB/hw/key
    Driver/STM32/Device/F407ZET_BB/hw/led
    main/STM32F407ZET/STF4BB/Ex/config:default
    main/${Main_Location}
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f407zet_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
