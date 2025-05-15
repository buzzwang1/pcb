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

    Data/ComDat/ComDat/v00.00.07

    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.00.02|-Ofast
    Data/Mem/Buffer/BArray/BArrayExtT/v00.00.01|-Ofast
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/Buffer/Ring/ComBuf/v00.00.03|-Ofast
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32U5xx:Miniv28b2:default
    Data/Mem/BuRam/Def/v00.00.01:STM32U5xx:MB
    Data/Mem/BuRam/cBuRam/v00.00.01:STM32U5xx


    Data/String/Cli/v00.00.01    
    Data/String/CStrT/v00.00.01
    Data/String/StringTools/v00.00.02

    Data/DateTime/Clock/v00.00.03:STM32U5xx
    Data/Math/Pid/Float/v00.00.01
    Data/Sys/Wuf/Def/v00.00.01:STM32U5xx
    Data/Sys/Wuf/cWufHandler/v00.00.01:STM32U5xx
    Data/Sys/Err/Def/v00.00.01:STM32U5xx
    Data/Sys/Err/cErr/v00.00.01:STM32U5xx


    Data/BotNet/v00.00.06/Base/Main/v00.00.02:default
    Data/BotNet/v00.00.06/Base/Misc/Adr/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/Cfg/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/ErrCnt/v00.00.01:default
    Data/BotNet/v00.00.06/Base/Misc/Spop/v00.00.02:STM32U5xx
    Data/BotNet/v00.00.06/Base/Misc/Msg/v00.00.01:default
    Data/BotNet/v00.00.06/BnLinks/Base/v00.00.02:default|-Ofast
    Data/BotNet/v00.00.06/BnLinks/I2c/v00.00.05:default|-Ofast
    Data/BotNet/v00.00.06/BnLinks/UsartMpHd/v00.00.03:default|-Ofast
    Data/BotNet/v00.00.06/BnLinks/BotCom/v00.00.03:default
    Data/BotNet/v00.00.06/BnLinks/nRf905/v00.00.02:default|-Ofast
    Data/BotNet/v00.00.06/BnLinks/nRf905Net/v00.00.03:default|-Ofast
    Data/BotNet/v00.00.06/BnLinks/vlink/v00.00.01:default
    Data/BotNet/v00.00.06/BnMsgSys/Base/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/Btr/v00.00.02:default
    Data/BotNet/v00.00.06/BnMsgSys/RRpt/v00.00.01:default
    Data/BotNet/v00.00.06/BnMsgSys/Spop/v00.00.02:default
    Data/BotNet/v00.00.06/BnStreamSys/Base/v00.00.01:default
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Base/v00.00.01:default
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Cmd/v00.00.01:default
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Com/v00.00.01:default
    Data/BotNet/v00.00.06/BnStreamSys/Ports/Echo/v00.00.01:default


    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/Hal/v01.01.00/Core
    Driver/STM32/U5/Hal/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/U5/Hal/v01.01.00/HAL:lib_o3_32Mhz
    Driver/STM32/U5/Hal/GPPin/v01.00.02
    Driver/STM32/U5/Hal/cUart/Int/v01.00.01
    Driver/STM32/U5/Hal/cUart/MpHd/v01.00.02_Mini:Tim16|-Ofast
    Driver/STM32/U5/Hal/cI2C/v01.00.09|-Ofast
    Driver/STM32/Device/Com/nRF905/Int/v01.00.00:MBv28b2_STM32U5xx
    #Driver/STM32/Device/xSpi/Flash/General/v01.00.00
    #Driver/STM32/Device/xSpi/Flash/ZD25WQ32/U5_OSPI/v01.00.00:U5Board100P
    Driver/STM32/Device/EEP/v01.00.00:Dummy

    Driver/STM32/Device/INA3221/v01.00.01
    Driver/STM32/Device/MP8859/v01.00.01
    Driver/STM32/Device/TPS55288/v01.00.01


    APP/LED/v00.00.01:STM32U5_HAL
    System/CyclicCaller/v00.00.02_mini:stm32u5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32u5xx
    APP/Job/JobHdl/v00.00.01


    APP/BSP/MB/Pkg/SysPkgClock/v00.00.01:master
    APP/BSP/MB/Pkg/SysPkgCom/v00.00.01:MBv28b2
    APP/BSP/MB/Pkg/SysPkgMsg/v00.00.01:MBv28b2
    APP/BSP/MB/Pkg/SysPkgPMon/v00.00.01:MBv28b2
    APP/BSP/MB/Pkg/SysPkgSMan/v00.00.01:MBv28b2
    APP/BSP/MB/Pkg/SysPkgBoard/v00.00.01:MBv28b2
    APP/BSP/MB/System/v00.00.01:MBv28b2

    main/STM32U585CIT/Mini/MBv28b2/Main01
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
