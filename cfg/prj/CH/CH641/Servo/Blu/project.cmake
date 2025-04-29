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

    #Data/ComDat/ComDat/v00.00.06

    #Data/Mem/MemTools/v00.00.03:default
    #Data/Mem/Buffer/BArray/BArrayT/v00.00.02 #|-Ofast
    #Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01 #|-Ofast
    #Data/Mem/Buffer/Ring/RingBufT/v00.00.03 #|-Ofast
    #Data/Mem/Buffer/Ring/ComBuf/v00.00.03_unsafe #|-Ofast
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.01:CH641:Servo

    #Data/BotNet/v00.00.06/Base/Main/v00.00.01:Blu:Servo1
    #Data/BotNet/v00.00.06/Base/Misc/Adr/v00.00.01:default
    #Data/BotNet/v00.00.06/Base/Misc/Cfg/v00.00.01:Servo1_Blu
    #Data/BotNet/v00.00.06/Base/Misc/Msg/v00.00.01:RiscV
    #Data/BotNet/v00.00.06/Base/Misc/Spop/v00.00.01:CH32v305
    #Data/BotNet/v00.00.06/BnLinks/Base/v00.00.02:default
    #Data/BotNet/v00.00.06/BnLinks/UsartMpHd/v00.00.03:default #|-Ofast
    #Data/BotNet/v00.00.06/BnMsgSys/Base/v00.00.01:default
    #Data/BotNet/v00.00.06/BnMsgSys/Spop/v00.00.01:Servo5:Blu:cH32V

    
    #Data/String/CStrT/v00.00.01_NoFpu
    #Data/String/StringTools/v00.00.02

    Driver/Ch32/CH641/STD/v01.00.01/Core:default
    Driver/Ch32/CH641/STD/v01.00.01/STD:default
    Driver/Ch32/CH641/STD/GPPin/v01.00.00
    #Driver/Ch32/CH641/STD/cUart/MpHd/v01.00.02_Servo5:Tim10 #|-Ofast


    #APP/LED/v00.00.01:CH32V3_STD
    System/STM32/ClockInfo/v00.00.01:CH641_8Mhz

    main/CH641/Servo/Blu
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/CH641_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions.cmake")
