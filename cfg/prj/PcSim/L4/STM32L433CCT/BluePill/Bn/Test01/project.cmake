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

    General/TypeDef/v00.00.01:Win32

    Data/ComDat/ComDat/v00.00.05

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03

    Data/String/Cli/v00.00.01
    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Data/BotNet/v00.00.03/Base/v00.00.01
    Data/BotNet/v00.00.03/BnLinks/Base/v00.00.01
    Data/BotNet/v00.00.03/BnLinks/I2c/v00.00.01
    Data/BotNet/v00.00.03/BnLinks/BotCom/v00.00.02
    #Data/BotNet/v00.00.03/BnLinks/nRf905/v00.00.01
    Data/BotNet/v00.00.03/BnMsgSys/Base/v00.00.01
    Data/BotNet/v00.00.03/BnStreamSys/Base/v00.00.01
    Data/BotNet/v00.00.03/BnStreamSys/Ports/Base/v00.00.01
    Data/BotNet/v00.00.03/BnStreamSys/Ports/Cmd/v00.00.01
    Data/BotNet/v00.00.03/BnStreamSys/Ports/Com/v00.00.01
    Data/BotNet/v00.00.03/BnStreamSys/Ports/Iap/v00.00.01:STM32F303CCT_PcSim
    
    Driver/Win32/Sim/GPPin/v01.00.00
    Driver/Win32/Sim/cI2C/v01.00.01
    Driver/Win32/Sim/cUart/v00.00.01

    #APP/LED/v00.00.01:STM32L4_HAL
    System/CyclicCaller/v00.00.01:Win32
    System/Win32/ClockInfo/v00.00.01:Win32

    main/PcSim/STM32L433CCT/Bn/Test01
)

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
