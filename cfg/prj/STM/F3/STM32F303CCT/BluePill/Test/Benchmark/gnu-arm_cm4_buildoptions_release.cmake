list(APPEND PCB_ListTargetCompileOptionsC
# ===============================================================================================================
# Project defined settings
# ===============================================================================================================
  #-DNDEBUG
)

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------

	-O3
)

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------
  -ffast-math                          # https://gcc.gnu.org/wiki/FloatingPointMath
)

