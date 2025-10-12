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

    main/Pc/Test/CppTests
)

message(STATUS "Include buildoptions file: ${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions.cmake")

