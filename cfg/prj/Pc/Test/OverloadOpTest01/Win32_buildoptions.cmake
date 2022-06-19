
message(STATUS "${PCB_ProjectCfg} configuration")
include("${CMAKE_CURRENT_LIST_DIR}/Win32_buildoptions_${PCB_ProjectCfg}.cmake")

list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DPCB_PROJECT_${PCB_Project}
)

list(APPEND PCB_ListTargetLinkOptions
# Linker specific settings:
# -----------------------------------------------
  #/WHOLEARCHIVE:lib\\xxx_lib.lib
  /subsystem:console
)

message(STATUS "Config for SUBSYSTEM:CONSOLE")

