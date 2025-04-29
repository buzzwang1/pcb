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


    Driver/ARM/Cmsis/V05.00.08/Core:CM4
    Driver/NRF/nRF5/v17.01.00/nrf52840/Core:default
    Driver/NRF/nRF5/v17.01.00/System:nrf52:nrf52840
    Driver/NRF/nRF5/v17.01.00/Boards:SuperMini
    Driver/NRF/nRF5/v17.01.00/Integration
    Driver/NRF/nRF5/v17.01.00/soc
    Driver/NRF/nRF5/v17.01.00/HAL
    Driver/NRF/nRF5/v17.01.00/Driver:default
    Driver/NRF/nRF5/v17.01.00/Utils

    Driver/NRF/nRF5/v17.01.00/libraries/atomic
    Driver/NRF/nRF5/v17.01.00/libraries/atomic_fifo
    Driver/NRF/nRF5/v17.01.00/libraries/atomic_flags
    Driver/NRF/nRF5/v17.01.00/libraries/balloc
    Driver/NRF/nRF5/v17.01.00/libraries/delay
    Driver/NRF/nRF5/v17.01.00/libraries/fifo
    Driver/NRF/nRF5/v17.01.00/libraries/log
    Driver/NRF/nRF5/v17.01.00/libraries/experimental_section_vars
    Driver/NRF/nRF5/v17.01.00/libraries/ringbuf
    Driver/NRF/nRF5/v17.01.00/libraries/memobj
    Driver/NRF/nRF5/v17.01.00/libraries/mutex
    Driver/NRF/nRF5/v17.01.00/libraries/strerror
    Driver/NRF/nRF5/v17.01.00/libraries/pwr_mgmt
    Driver/NRF/nRF5/v17.01.00/libraries/uart:fifo

    Driver/NRF/nRF5/v17.01.00/external/fprintf
    Driver/NRF/nRF5/v17.01.00/external/utf_converter
    Driver/NRF/nRF5/v17.01.00/external/segger_rtt

    Driver/NRF/nRF5/v17.01.00/softdevice/sd:nosd

    main/NRF52840/Ls/Test0
)

set(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/nrf52840.ld")

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions.cmake")

