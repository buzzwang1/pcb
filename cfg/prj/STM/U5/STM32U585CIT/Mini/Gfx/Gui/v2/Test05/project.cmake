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
    Gfx/Texture/32/v00.00.01
    Gfx/Bitmap/Convert/v00.00.01
    Gfx/Bitmap/Tools/v00.00.01
    Gfx/Screen/Screen/v00.00.02
    Gfx/Screen/Tools/v00.00.01
    Gfx/Paint/Basic/v00.00.01
    Gfx/MemPaint/v00.00.01
    Gfx/Paint/Triangle/Basic/v00.00.01
    Gfx/Paint/Triangle/ZBasic/v00.00.01
    #Gfx/Paint/Triangle/BrigthGrad/v00.00.01
    #Gfx/Paint/Triangle/ColGrad/v00.00.01
    #Gfx/Paint/Triangle/TextureBrigthGrad/v00.00.01
    Gfx/Paint/Triangle/BrigthFlat/v00.00.01
    Gfx/Paint/Triangle/ZTextureBrigthGrad/v00.00.01
    Gfx/Sprite/Sprite/v00.00.01
    Gfx/Sprite/SEngine/v00.00.01
    Gfx/Font/RFont/v00.00.01
    Gfx/Graph/XY/v00.00.02
    Gfx/Graph/Y/v00.00.02
    Gfx/Ui/Element/v00.00.04
    Gfx/Ui/Cursor/v00.00.02

    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Data/ComDat/ComDat/v00.00.06
    Data/Msg/v00.00.01
    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/Tree/Idx/16Bit/12Byte/v00.00.01
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03
    
    Data/Math/MathTools/v00.00.01
    Data/Math/Matrix/4x4f/v00.00.01
    Data/Math/Vector/3f/v00.00.01

    Data/Sort/q/v00.00.01

    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/HAL/v01.01.00/Core
    Driver/STM32/u5/HAL/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/u5/HAL/v01.01.00/HAL:lib_o3_32Mhz
    Driver/STM32/U5/HAL/GPPin/v01.00.02

    Driver/STM32/Device/Displays/ILI9341/U5xx/SPI/v01.00.03:Mini


    System/CyclicCaller/v00.00.01:stm32u5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32u5xx

    Test/STM32/DiffTimer/v00.00.01:stm32u5xx_TIM2


    APP/LED/v00.00.01:STM32U5_HAL
    main/STM32U585CIT/Mini/Gfx/Gui/v2/Test05
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
