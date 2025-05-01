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

    Data/ComDat/ComDat/v00.00.07

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03_unsafe
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.01:GD32f130G6:Servo1
    Data/Math/cFixPti1814/v00.00.01


    Data/BotNet/v00.00.06/Base/Main/v00.00.01:UpLinkOnly
    Data/BotNet/v00.00.06/Base/Misc/Adr/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/Cfg/v00.00.01:Servo1_App
    Data/BotNet/v00.00.06/Base/Misc/Msg/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/ErrCnt/v00.00.01:Dummy
    Data/BotNet/v00.00.06/Base/Misc/Spop/v00.00.01:GD32f130G6
    Data/BotNet/v00.00.06/BnLinks/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnLinks/UsartMpHd/v00.00.03:default
    Data/BotNet/v00.00.06/BnMsgSys/Base/v00.00.01:default
    Data/BotNet/v00.00.06/BnMsgSys/Spop/v00.00.01:Servo1:App
    Data/BotNet/v00.00.06/BnStreamSys/Base/v00.00.01:Servo1_App
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Base/v00.00.01:Servo1_App
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Cmd/v00.00.01:Servo1_App

    Data/String/Cli/v00.00.01_noHist    
    Data/String/CStrT/v00.00.01_NoFpu
    Data/String/StringTools/v00.00.02

    Driver/ARM/Cmsis/V05.00.08/Core:CM3
    Driver/STM32/F1/STD/v03.01.00/GD32F1x0/Core:Servo1
    Driver/STM32/F1/STD/v03.01.00/GD32F1x0/STD:default

    Driver/STM32/F1/STD/GPPin/v01.00.02_GD
    Driver/STM32/F1/STD/cUart/MpHd/v01.00.02_GD_Servo1:Tim16


    APP/LED/v00.00.01:GD32F1_STD
    System/STM32/ClockInfo/v00.00.01:GD32f130_8Mhz

    Test/STM32/TimeStampArray/v00.00.01:GD32F1x0_8MHz_TIM15

    main/GD32F130G6/UbTech/Servo1/AppDebug
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/GD32F130G6_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")
