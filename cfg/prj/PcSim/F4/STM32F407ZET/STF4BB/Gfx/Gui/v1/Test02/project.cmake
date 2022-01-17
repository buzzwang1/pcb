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

    General/Typedef/v00.00.01:Win32

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
    Gfx/Ui/Element/v00.00.03
    Gfx/Ui/ElementTree/v00.00.03
    Gfx/Ui/Cursor/v00.00.02

    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Data/ComDat/ComDat/v00.00.06
    Data/Msg/v00.00.01
    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/Tree/Idx/16Bit/10Byte/v00.00.01
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

    Data/DateTime/Clock/v00.00.01

    #Driver/Stm32/F407ZET/Bsp:fsmc
    #Driver/Stm32/F407ZET/Core
    #Driver/ARM/Cmsis/V05.00.08/Core:CM4
    #Driver/STM32/Device/F407ZET_BB/System:NoOs
    Driver/Win32/Display/v00.00.01


    #main/STM32F407ZET/STF4BB/Gfx/config:default
    main/PcSim/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test02
)

#set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f407zet_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
