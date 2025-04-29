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

    Data/Mem/MemTools/v00.00.03:default

    Driver/ARM/Cmsis/V05.00.08/Core:CM4 
    Driver/STM32/L4/HAL/v01.15.01/Core
    Driver/STM32/L4/HAL/v01.15.01/Device/stm32l433xx:default|-Ofast
    Driver/STM32/L4/HAL/v01.15.01/HAL:lib_o3
    Driver/STM32/L4/HAL/GPPin/v01.00.02

    main/${Main_Location}
)

message(STATUS "Include linker file: ${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
