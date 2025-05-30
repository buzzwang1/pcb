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

    Data/ComDat/ComDat/v00.00.06
    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03

    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02
    
    Driver/STM32/Device/APDS9960/v01.00.00
    Driver/STM32/Device/HTU21D/v01.00.00
    #Driver/STM32/Device/INA219/v01.00.01
    Driver/STM32/Device/Displays/SSD1306/v01.00.01:A_128x64x1BPP

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/HAL/v01.01.00/Core
    Driver/STM32/u5/HAL/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/u5/HAL/v01.01.00/HAL:lib_o3_32Mhz
    
    Driver/STM32/U5/HAL/GPPin/v01.00.02
    Driver/STM32/U5/HAL/cI2C/v01.00.07


    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Gfx/General/GfxDefs/v00.00.01:32Bit
    Gfx/Bitmap/Bitmap/v00.00.01
    Gfx/Bitmap/Tools/v00.00.01
    Gfx/Screen/Screen/v00.00.02
    Gfx/Screen/Tools/v00.00.01
    Gfx/Paint/Basic/v00.00.01
    Gfx/Sprite/Sprite/v00.00.01
    Gfx/Sprite/SEngine/v00.00.01
    Gfx/Font/RFont/v00.00.01


    APP/LED/v00.00.01:STM32U5_HAL
    System/CyclicCaller/v00.00.01:stm32u5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32u5xx

    main/${Main_Location}
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
