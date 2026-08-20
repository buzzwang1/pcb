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

    Data/ComDat/ComDat/v00.01.00

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.01.00
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32F303CCT:default

    Data/String/Cli/v00.01.00:noHist
    Data/String/CStrT/v00.01.00:NoFpu
    Data/String/StringTools/v00.00.02

    Data/BotNet/v00.01.00/Base/Main/v00.00.02:PcIfc
      Data/BotNet/v00.01.00/Base/Misc/Adr/v00.00.01:default
      Data/BotNet/v00.01.00/Base/Misc/Cfg/v00.00.01:default
      Data/BotNet/v00.01.00/Base/Misc/Msg/v00.00.01:default
      Data/BotNet/v00.01.00/Base/Misc/MsgPool/v00.00.01:default
      Data/BotNet/v00.01.00/Base/Misc/ErrCnt/v00.00.01:default
      Data/BotNet/v00.01.00/Base/Misc/Spop/v00.00.02:Dummy
    Data/BotNet/v00.01.00/BnLinks/Base/v00.00.02:default
      #Data/BotNet/v00.01.00/BnLinks/UsartMpHd/v00.00.04:default
      #Data/BotNet/v00.01.00/BnLinks/I2c/v00.00.05:default
      Data/BotNet/v00.01.00/BnLinks/BotCom/v00.00.03:default
      Data/BotNet/v00.01.00/BnLinks/nRf905/v00.00.03:default
      Data/BotNet/v00.01.00/BnLinks/nRf905Net/v00.00.03:default
    Data/BotNet/v00.01.00/BnMsgSys/Base/v00.00.02:default
    #  Data/BotNet/v00.01.00/BnMsgSys/RRpt/v00.00.01:Dummy
    #  Data/BotNet/v00.01.00/BnMsgSys/Btr/v00.00.02:default
    #  Data/BotNet/v00.01.00/BnMsgSys/Spop/v00.00.02:default
    #Data/BotNet/v00.01.00/BnStreamSys/Base/v00.00.01:Dummy
    #  Data/BotNet/v00.01.00/BnStreamSys/Ports/Base/v00.00.01:Dummy

    Gfx/General/GfxDefs/v00.00.01:32Bit
    Gfx/Bitmap/Bitmap/v00.00.01
    Gfx/Bitmap/Tools/v00.00.01
    Gfx/Screen/Screen/v00.00.02
    Gfx/Screen/Tools/v00.00.01
    Gfx/Paint/Basic/v00.00.01
    Gfx/Sprite/Sprite/v00.00.01
    Gfx/Sprite/SEngine/v00.00.01
    Gfx/Font/RFont/v00.00.02

    Resource/Base/v00.00.01
    Resource/Gen/Gfx/Fonts/1BPP_1G/v00.00.01

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/STM32/F3/STD/v01.02.03/Core/stm32f30x:72Mhz
    Driver/STM32/F3/STD/v01.02.03/STD:default 

    Driver/STM32/F3/STD/GPPin/v01.00.02
    #Driver/STM32/F3/STD/cI2C/v01.00.05_Tim:Tim15
    Driver/STM32/F3/STD/cUart/Int/v01.00.01
    Driver/STM32/Device/Com/nRF905/Int/v01.01.00:TB01_STM32F303CCT
    Driver/STM32/Device/Displays/HX8347/v00.00.01:F303CCT_SPI2_DMA


    APP/LED/v00.00.01:VirtLed
    System/CyclicCaller/v00.00.01:stm32f30x_TIM3
    System/STM32/ClockInfo/v00.00.01:stm32f30x

    main/${Main_Location}
)

message(STATUS "${PCB_ProjectCfg} linker file")
message(STATUS "Include linker file: ${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/linker_${PCB_ProjectCfg}.cmake")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
