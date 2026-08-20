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

    Data/Mem/MemTools/v00.00.03:default
    Data/Mem/Buffer/BArray/BArrayT/v00.01.00
    Data/Mem/Buffer/Ring/RingBufT/v00.00.03
    Data/Mem/MemPart/v00.00.01
    Data/Mem/RomConst/v00.00.02:Rom:nrf52840
    Data/Math/cFixPti2210/v00.00.02
      
    Data/BotNet/v00.02.00/Base/Main/v00.00.02:UpLinkOnly
      Data/BotNet/v00.02.00/Base/Misc/Adr/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/Cfg/v00.00.01:Small
      Data/BotNet/v00.02.00/Base/Misc/Msg/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/MsgPool/v00.00.01:default
      Data/BotNet/v00.02.00/Base/Misc/ErrCnt/v00.00.01:Dummy
      Data/BotNet/v00.02.00/Base/Misc/Spop/v00.00.02:Dummy
    Data/BotNet/v00.02.00/BnLinks/Base/v00.00.02:default
      #Data/BotNet/v00.02.00/BnLinks/Usb/v00.00.03:TinyUsb
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

    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/NRF/nRF5/v17.01.00/nrf52840/Core:default
    Driver/NRF/nRF5/v17.01.00/System:nrf52:nrf52840
    Driver/NRF/nRF5/v17.01.00/Boards:SuperMini
    Driver/NRF/nRF5/v17.01.00/Driver:default
    Driver/NRF/nRF5/v17.01.00/Integration
    Driver/NRF/nRF5/v17.01.00/soc
    Driver/NRF/nRF5/v17.01.00/HAL
    Driver/NRF/nRF5/v17.01.00/Utils

    Driver/NRF/nRF5/v17.01.00/Ble/nrf_ble_gatt
    Driver/NRF/nRF5/v17.01.00/Ble/nrf_ble_qwr
    Driver/NRF/nRF5/v17.01.00/Ble/ble_advertising
    Driver/NRF/nRF5/v17.01.00/Ble/common
    Driver/NRF/nRF5/v17.01.00/Ble/peer_manager
    Driver/NRF/nRF5/v17.01.00/Ble/ble_link_ctx_manager
    #Driver/NRF/nRF5/v17.01.00/Ble/ble_services/ble_bas
    #Driver/NRF/nRF5/v17.01.00/Ble/ble_services/ble_dis
    Driver/NRF/nRF5/v17.01.00/Ble/ble_services/ble_nus

    Driver/NRF/nRF5/v17.01.00/libraries/atomic
    Driver/NRF/nRF5/v17.01.00/libraries/atomic_fifo
    Driver/NRF/nRF5/v17.01.00/libraries/atomic_flags
    Driver/NRF/nRF5/v17.01.00/libraries/balloc
    #Driver/NRF/nRF5/v17.01.00/libraries/bootloader/ble_dfu
    Driver/NRF/nRF5/v17.01.00/libraries/bsp/core
    Driver/NRF/nRF5/v17.01.00/libraries/bsp/ble
    Driver/NRF/nRF5/v17.01.00/libraries/button
    #Driver/NRF/nRF5/v17.01.00/libraries/cli
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/core
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/nrf_hw
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/nrf_sw
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/cc310
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/cc310_bl
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/mbedtls
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/cifra
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/micro_ecc
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/optiga
    #Driver/NRF/nRF5/v17.01.00/libraries/crypto/backend/oberon
    #Driver/NRF/nRF5/v17.01.00/libraries/crc16
    #Driver/NRF/nRF5/v17.01.00/libraries/crc32
    #Driver/NRF/nRF5/v17.01.00/libraries/csense_drv
    #Driver/NRF/nRF5/v17.01.00/libraries/csense
    Driver/NRF/nRF5/v17.01.00/libraries/delay
    #Driver/NRF/nRF5/v17.01.00/libraries/ecc
    Driver/NRF/nRF5/v17.01.00/libraries/experimental_section_vars
    Driver/NRF/nRF5/v17.01.00/libraries/fifo
    Driver/NRF/nRF5/v17.01.00/libraries/gpiote
    Driver/NRF/nRF5/v17.01.00/libraries/fds
    #Driver/NRF/nRF5/v17.01.00/libraries/fstorage
    #Driver/NRF/nRF5/v17.01.00/libraries/gfx
    Driver/NRF/nRF5/v17.01.00/libraries/hardfault:nrf52
    #Driver/NRF/nRF5/v17.01.00/libraries/hci
    Driver/NRF/nRF5/v17.01.00/libraries/log
    #Driver/NRF/nRF5/v17.01.00/libraries/led_softblink
    #Driver/NRF/nRF5/v17.01.00/libraries/low_power_pwm
    Driver/NRF/nRF5/v17.01.00/libraries/mem_manager
    Driver/NRF/nRF5/v17.01.00/libraries/memobj
    #Driver/NRF/nRF5/v17.01.00/libraries/mpu
    Driver/NRF/nRF5/v17.01.00/libraries/mutex
    #Driver/NRF/nRF5/v17.01.00/libraries/pwm
    Driver/NRF/nRF5/v17.01.00/libraries/pwr_mgmt
    Driver/NRF/nRF5/v17.01.00/libraries/queue
    Driver/NRF/nRF5/v17.01.00/libraries/ringbuf
    Driver/NRF/nRF5/v17.01.00/libraries/scheduler:default
    #Driver/NRF/nRF5/v17.01.00/libraries/sdcard
    Driver/NRF/nRF5/v17.01.00/libraries/sensorsim
    #Driver/NRF/nRF5/v17.01.00/libraries/slip
    Driver/NRF/nRF5/v17.01.00/libraries/sortlist
    #Driver/NRF/nRF5/v17.01.00/libraries/spi_mngr
    Driver/NRF/nRF5/v17.01.00/libraries/stack_info
    #Driver/NRF/nRF5/v17.01.00/libraries/stack_guard
    Driver/NRF/nRF5/v17.01.00/libraries/strerror
    #Driver/NRF/nRF5/v17.01.00/libraries/svc
    Driver/NRF/nRF5/v17.01.00/libraries/timer:timer2
    #Driver/NRF/nRF5/v17.01.00/libraries/twi_mngr
    Driver/NRF/nRF5/v17.01.00/libraries/uart:fifo
    Driver/NRF/nRF5/v17.01.00/libraries/util
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/cdc/acm
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/hid/generic
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/msc
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/hid
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/hid/mouse
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/hid/kbd
    #Driver/NRF/nRF5/v17.01.00/libraries/usbd/class/audio


    Driver/NRF/nRF5/v17.01.00/softdevice/sd:sd:s140
    Driver/NRF/nRF5/v17.01.00/softdevice/sdh/core
    Driver/NRF/nRF5/v17.01.00/softdevice/sdh/ble
    Driver/NRF/nRF5/v17.01.00/softdevice/sdh/soc

    Driver/NRF/nRF5/v17.01.00/external/fprintf
    #Driver/NRF/nRF5/v17.01.00/external/mbedtls
    #Driver/NRF/nRF5/v17.01.00/external/nrf_cc310:cortex-m4:hard-float:default
    #Driver/NRF/nRF5/v17.01.00/external/nrf_oberon:cortex-m4:hard-float:default
    #Driver/NRF/nRF5/v17.01.00/external/nrf_tls/mbedtls:nrf_crypto
    Driver/NRF/nRF5/v17.01.00/external/utf_converter
    Driver/NRF/nRF5/v17.01.00/external/segger_rtt

    #Driver/NRF/nRF5/v17.01.00/nfc/ndef
    #Driver/NRF/nRF5/v17.01.00/nfc/platform
    #Driver/NRF/nRF5/v17.01.00/nfc/t2t_parser
    #Driver/NRF/nRF5/v17.01.00/nfc/t2t_lib
    #Driver/NRF/nRF5/v17.01.00/nfc/t4t_parser
    #Driver/NRF/nRF5/v17.01.00/nfc/t4t_lib

    APP/Job/JobHdl/v00.00.01:ms
    #APP/LED/v00.00.01:STM32L4_HAL
    #System/CyclicCaller/v00.00.01:stm32l4xx_TIM6
    #System/STM32/ClockInfo/v00.00.01:stm32l4xx

    main/NRF52840/ble/Botnet
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/nrf52840.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")

