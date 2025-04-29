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

    Data/ComDat/ComDat/v00.00.06
    Data/Msg/v00.00.01
    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/Tree/Idx/16Bit/10Byte/v00.00.01
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03

    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Data/Safety/v00.00.01:default

    #Driver/Stm32/F407ZET/Bsp:fsmc
    #Driver/Stm32/F407ZET/Core
    #Driver/ARM/Cmsis/V05.00.08/Core:CM4

    main/Pc/Test/OverloadOpTest01
)

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")

