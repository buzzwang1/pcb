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
    General/TypeDef/Work_Typedef

    Graphics/General/GfxDefs/Work_GfxDefs
    Graphics/Screen/Work_Screen
    Data/Mem/Work_MemTools
    Data/Mem/Work_MemWrap:stdlib
    Driver/STM32/Device/Displays/ILI9341/F407_MyAvr/Dio/v01_00_00

    Driver/STM32/F407VGT/Core/v1.0.0:default
    Driver/STM32/F407VGT/STD/v1.0.0
    Driver/STM32/F4/STD/GPPin/v01.00.00

    APP/LED/STM32F429ZIT6
    System/CyclicCaller/Work_CyclicCaller

    main/${Main_Location}
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f407vgt_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")

