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

    Data/ComDat/ComDat/v00.02.01
    Data/ComDat/ComSeq/v00.00.01
    Data/ComDat/ComComp/v00.00.01
    Data/ComDat/ComRegMap/v00.00.01
    Data/ComDat/ComSeqHdl/v00.00.01

    Data/Msg/v00.00.01

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.01.00|-Ofast
    Data/Mem/Buffer/Ring/RingBufT/v00.01.00

    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32U5xx:Miniv28b2:default
    Data/Mem/BuRam/Def/v00.00.01:STM32U5xx:MB
    Data/Mem/BuRam/cBuRam/v00.00.01:STM32U5xx
    Data/Mem/BitField/cBitField/v00.00.01

    Data/String/Cli/v00.01.00
    Data/String/CStrT/v00.01.00:default
    Data/String/StringTools/v00.00.02

    Data/DateTime/Clock/v00.00.03:STM32U5xx
    Data/Math/Pid/Float/v00.00.01
    
    Data/Sys/Wuf/Def/v00.00.01:STM32U5xx
    Data/Sys/Wuf/cWufHandler/v00.00.01:STM32U5xx
    
    Data/Sys/Err/Def/v00.00.01:STM32U5xx
    Data/Sys/Err/cErr/v00.00.01:STM32U5xx

    Data/Sys/cDmaHelper/v00.00.01
    Data/Sys/DepTree/cDepTreeCfg/v00.00.01:MiniAddon
    Data/Sys/DepTree/cDepTree/v00.00.01
    Data/Sys/DepTree/cComponent/v00.00.01

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
    Gfx/Font/RFont/v00.00.02
    Gfx/Graph/XY/v00.00.02
    Gfx/Graph/Y/v00.00.02
    Gfx/Ui/Element/v00.00.04
    Gfx/Ui/Cursor/v00.00.02

    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Data/Math/MathTools/v00.00.01
    Data/Math/Matrix/4x4f/v00.00.01
    Data/Math/Vector/3f/v00.00.01

    Data/Sort/q/v00.00.01
    
        
    #Data/BotNet/v00.02.00/Base/Main/v00.00.02:default
    #  Data/BotNet/v00.02.00/Base/Misc/Adr/v00.00.01:default
    #  Data/BotNet/v00.02.00/Base/Misc/Cfg/v00.00.01:default
    #Data/BotNet/v00.02.00/Base/Misc/ErrCnt/v00.00.01:default
    #  Data/BotNet/v00.02.00/Base/Misc/Spop/v00.00.02:STM32U5xx
    #  Data/BotNet/v00.02.00/Base/Misc/Msg/v00.00.01:default
    #Data/BotNet/v00.02.00/BnLinks/Base/v00.00.02:default|-Ofast
    #  Data/BotNet/v00.02.00/BnLinks/I2c/v00.00.05:default|-Ofast
    #  Data/BotNet/v00.02.00/BnLinks/UsartMpHd/v00.00.04:default|-Ofast
    #  Data/BotNet/v00.02.00/BnLinks/BotCom/v00.00.03:default
    #  Data/BotNet/v00.02.00/BnLinks/nRf905/v00.00.03:default|-Ofast
    #  Data/BotNet/v00.02.00/BnLinks/nRf905Net/v00.00.03:default|-Ofast
    #Data/BotNet/v00.02.00/BnMsgSys/Base/v00.00.02:default
    #  Data/BotNet/v00.02.00/BnMsgSys/RRpt/v00.00.01:default
    #  Data/BotNet/v00.02.00/BnMsgSys/Btr/v00.00.02:default
    #  Data/BotNet/v00.02.00/BnMsgSys/Spop/v00.00.02:default
    #Data/BotNet/v00.02.00/BnStreamSys/Base/v00.00.01:default
    #  Data/BotNet/v00.02.00/BnStreamSys/Ports/Base/v00.00.01:default
    #  Data/BotNet/v00.02.00/BnStreamSys/Ports/Cmd/v00.00.01:default
    #  Data/BotNet/v00.02.00/BnStreamSys/Ports/Com/v00.00.01:default
    #  Data/BotNet/v00.02.00/BnStreamSys/Ports/Echo/v00.00.01:default


    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/Hal/v01.01.00/Core
    Driver/STM32/U5/Hal/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/u5/HAL/v01.01.00/HAL:lib_o3_32Mhz
    Driver/STM32/U5/Hal/GPPin/v01.00.02
    #Driver/STM32/U5/Hal/cUart/Int/v01.00.01
    Driver/STM32/U5/Hal/cUart/MpHd/v01.01.00_Mini:Tim16|-Ofast
    Driver/STM32/U5/Hal/cI2C/v01.01.01|-Ofast
    Driver/STM32/U5/Hal/cSpi/v01.01.01|-Ofast
    #Driver/STM32/Device/Com/nRF905/Int/v01.00.00:MBv28b2_STM32U5xx
    
    #Driver/STM32/Device/xSpi/Flash/General/v01.00.00
    #Driver/STM32/Device/xSpi/Flash/ZD25WQ32/U5_OSPI/v01.00.00:U5Board100P
    #Driver/STM32/Device/EEP/v01.00.00:Dummy

    Driver/STM32/Device/INA3221/v02.00.00:default

    Driver/STM32/Device/TPS55288/v02.00.00:default
    Driver/STM32/Device/Tmp102/v02.00.00:default

    Driver/STM32/Device/BQ25798/v02.00.00:default
    
    Driver/STM32/Device/Displays/ILI9341/U5xx/SPI/v01.00.04:default:Miniv28
    Driver/STM32/Device/Displays/XPT2046/U5xx/SPI/v01.00.04:default:Miniv28

    #Driver/STM32/Device/xSpi/Flash/General/v01.00.00
    #Driver/STM32/Device/xSpi/Flash/ZD25WQ32/U5_OSPI/v01.00.00:Mini32Mhz

    System/STM32/ClockInfo/v00.00.01:stm32u5xx

    ExtLibs/Os/FreeRTOS/v10.05.01:Miniv28

    APP/LED/v00.00.01:STM32U5_HAL
    APP/Job/JobHdl/v00.00.01:us

    main/STM32U585CIT/Mini/MBv28b2/Main04Gfx
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
