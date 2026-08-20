
list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DVECT_TAB_OFFSET=0x0000 # 16k Offset
)

list(APPEND PCB_ListTargetCompileOptionsC
# ===============================================================================================================
# Project defined settings
# ===============================================================================================================
  #-DNDEBUG
)

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------

	-Os
)

