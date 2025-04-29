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
    General/TypeDef/v00.00.01:RiscV

    Data/ComDat/ComDat/v00.00.06

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02 #|-Ofast
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01 #|-Ofast
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03 #|-Ofast
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03_unsafe #|-Ofast
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.01:CH32V305:Servo5
    Data/Math/cFixPti1814/v00.00.01

    Data/String/Cli/v00.00.01
    
    Data/String/CStrT/v00.00.01_NoFpu
    Data/String/StringTools/v00.00.02

    Driver/Ch32/V3/STD/v01.00.00/Core:default
    Driver/Ch32/V3/STD/v01.00.00/STD:default
    Driver/Ch32/V3/STD/GPPin/v01.00.00
    Driver/Ch32/V3/STD/cI2C/v01.00.07
    Driver/STM32/Device/EEP/v01.00.00:ZD24C08A
    
    Driver/STM32/Device/Tmp102/v01.00.00
    #Driver/STM32/Device/APDS9960/v01.00.00
    Driver/STM32/Device/HTU21D/v01.00.00
    Driver/STM32/Device/Displays/SSD1306/v01.00.01:A_128x64x1BPP    


    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01
    Resource/Gen/Gfx/Screens/1BPP_1G/v00.00.01

    Gfx/General/GfxDefs/v00.00.01:32Bit
    Gfx/Bitmap/Bitmap/v00.00.01
    Gfx/Bitmap/Tools/v00.00.01
    Gfx/Screen/Screen/v00.00.02
    Gfx/Screen/Tools/v00.00.01
    Gfx/Paint/Basic/v00.00.01
    Gfx/Sprite/Sprite/v00.00.01
    Gfx/Sprite/SEngine/v00.00.01
    Gfx/Font/RFont/v00.00.01
    
    APP/LED/v00.00.01:CH32V3_STD
    System/CyclicCaller/v00.00.01:CH32v3xx_SysTick_24Mhz
    System/STM32/ClockInfo/v00.00.01:CH32V3xx

    main/CH32V305/LeuchtEi/I2cTest
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/CH32V305_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
