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

    Gfx/General/GfxDefs/v00.00.01:32Bit
    Gfx/Bitmap/Bitmap/v00.00.01
    Gfx/Bitmap/Tools/v00.00.01
    Gfx/Screen/Screen/v00.00.02
    Gfx/Screen/Tools/v00.00.01
    Gfx/Paint/Basic/v00.00.01
    Gfx/Sprite/Sprite/v00.00.01
    Gfx/Sprite/SEngine/v00.00.01
    Gfx/Font/RFont/v00.00.01
    Gfx/Graph/XY/v00.00.02

    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Data/ComDat/ComDat/v00.00.05

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03

    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Driver/ARM/Cmsis/V05.00.08/Core:CM3
    Driver/STM32/F1/STD/v03.05.00/Core/stm32f10x:default
    Driver/STM32/F1/STD/v03.05.00/STD:default

    Driver/STM32/F1/STD/GPPin/v01.00.02
    Driver/STM32/F1/STD/cI2C/v01.00.02

    Driver/STM32/Device/Displays/SSD1306/v01.00.01:128x64x1BPP
    Driver/STM32/Device/LSM303DLHC/Acc/v01.00.00
    Driver/STM32/Device/LSM303DLHC/Mag/v01.00.00

    APP/LED/v00.00.01:STM32F1_STD
    System/CyclicCaller/v00.00.01:stm32f10x_TIM2
    System/STM32/ClockInfo/v00.00.01:stm32f10x

    main/STM32F103C8T/SolBx/02/MotBx/01
)

message(STATUS "Include linker file: ${CMAKE_CURRENT_LIST_DIR}/stm32f103c8_flash.ld")
set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")

