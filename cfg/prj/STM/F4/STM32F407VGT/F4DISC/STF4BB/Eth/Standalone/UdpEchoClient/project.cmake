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

    Driver/STM32/Device/F4DISC/Board/V01_01_00
    Driver/STM32/Device/F4DISC/LwIP/v01_03_02:StandaloneIpv4

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F4/STD/v01.08.00/Core/stm32f40x:144MHz
    Driver/STM32/F4/STD/v01.08.00/STD:default:f40x:ethernet

    main/STM32F407VGT/F4DISC/BB/Eth/Standalone/UdpEchoClient
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f407vgt_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")

