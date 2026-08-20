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

    Data/ComDat/ComDat/v00.02.01

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.01.00
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:STM32f103:default
    Data/Math/cFixPti2210/v00.00.02
      
    Data/BotNet/v00.02.00/Base/Main/v00.00.02:UpLinkOnly
      Data/BotNet/v00.02.00/Base/Misc/Adr/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/Cfg/v00.00.01:Small
      Data/BotNet/v00.02.00/Base/Misc/Msg/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/MsgPool/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/ErrCnt/v00.00.01:Dummy
      Data/BotNet/v00.02.00/Base/Misc/Spop/v00.00.02:Dummy
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

    Data/String/Cli/v00.01.00:noHist
    Data/String/CStrT/v00.01.00:NoFpu
    Data/String/StringTools/v00.00.02

    Driver/ARM/Cmsis/V05.00.08/Core:CM3

    Driver/STM32/F1/Hal/v01.08.05/Core
    Driver/STM32/F1/Hal/v01.08.05/Device/STM32F103xx:default
    Driver/STM32/F1/Hal/v01.08.05/HAL:default

    Driver/STM32/F1/HAL/GPPin/v01.00.02
    Driver/STM32/Device/EEP/v01.01.00:Dummy

    APP/Job/JobHdl/v00.00.01:ms
    APP/LED/v00.00.01:STM32F1_HAL
    System/CyclicCaller/v00.00.02:stm32f10x_Hal_TIM3
    System/STM32/ClockInfo/v00.00.01:stm32f10x_hal

    ExtLibs/Com/tinyusb/20260607/src
    ExtLibs/Com/tinyusb/20260607/hw:stm32f1:stm32f103_bluepill

    main/STM32F103C8T/Usb/Botnet
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/stm32f103c8_flash.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm3_buildoptions.cmake")
