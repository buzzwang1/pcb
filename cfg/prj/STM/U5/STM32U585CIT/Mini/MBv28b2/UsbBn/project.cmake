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
    
    Data/Mem/MemTools/v00.00.03:default_arm
    Data/Mem/Buffer/BArray/BArrayT/v00.01.00|-Ofast
    Data/Mem/Buffer/Ring/RingBufT/v00.01.00

    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32U5xx:Miniv28b2:default

    Data/String/Cli/v00.01.00:default   
    Data/String/CStrT/v00.01.00:default
    Data/String/StringTools/v00.00.02 

    Data/BotNet/v00.02.00/Base/Main/v00.00.02:UpLinkOnly
      Data/BotNet/v00.02.00/Base/Misc/Adr/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/Cfg/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/ErrCnt/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/Spop/v00.00.02:STM32U5xx
      Data/BotNet/v00.02.00/Base/Misc/Msg/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/MsgPool/v00.00.01:default
    Data/BotNet/v00.02.00/BnLinks/Base/v00.00.02:default
      Data/BotNet/v00.02.00/BnLinks/Usb/v00.00.03:TinyUsb
    Data/BotNet/v00.02.00/BnMsgSys/Base/v00.00.02:default
      Data/BotNet/v00.02.00/BnMsgSys/RRpt/v00.00.01:default
      Data/BotNet/v00.02.00/BnMsgSys/Btr/v00.00.02:default
      Data/BotNet/v00.02.00/BnMsgSys/MemView/v00.00.02:Small
      Data/BotNet/v00.02.00/BnMsgSys/Spop/v00.00.02:OnlyFlashRam
    Data/BotNet/v00.02.00/BnStreamSys/Base/v00.00.01:Servo1_App
      Data/BotNet/v00.02.00/BnStreamSys/Ports/Base/v00.00.01:Servo1_App
      Data/BotNet/v00.02.00/BnStreamSys/Ports/Cmd/v00.00.01:Servo1_App


    ExtLibs/Com/tinyusb/20260607/src
    ExtLibs/Com/tinyusb/20260607/hw:stm32u5:MBv28b2

    Driver/ARM/Cmsis/V05.06.00/Core:CM33
    Driver/STM32/U5/Hal/v01.01.00/Core
    Driver/STM32/U5/Hal/v01.01.00/Device/STM32U575xx:default
    Driver/STM32/U5/Hal/v01.01.00/HAL:lib_o3_32Mhz
    Driver/STM32/U5/Hal/GPPin/v01.00.02
    Driver/STM32/U5/Hal/cTim/v01.01.01
    Driver/STM32/Device/EEP/v01.01.00:Dummy

    System/CyclicCaller/v00.00.02:stm32U5xx_TIM6
    System/STM32/ClockInfo/v00.00.01:stm32u5xx


    
    APP/LED/v00.00.01:STM32U5_HAL
    APP/Job/JobHdl/v00.00.01:ms

    main/STM32U585CIT/Mini/MBv28b2/UsbBn
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32U575ZI_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
