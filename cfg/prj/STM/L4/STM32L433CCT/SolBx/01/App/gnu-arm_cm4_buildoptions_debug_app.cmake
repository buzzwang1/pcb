list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DVECT_TAB_OFFSET=0x8000 # 32k Offset
)


list(APPEND PCB_ListTargetCompileOptionsC
# ===============================================================================================================
# Project defined settings
# ===============================================================================================================
  -DNDEBUG
)

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------

  #-O (Same as -O1)
  #-O0 (do no optimization, the default if no optimization level is specified)
  #-O1 (optimize minimally)
  #-O2 (optimize more)
  #-O3 (optimize even more)
  #-Ofast (optimize very aggressively to the point of breaking standard compliance)
  #-Og (Optimize debugging experience. -Og enables optimizations that do not interfere with debugging. It should be the optimization level of choice for the standard edit-compile-debug cycle, offering a reasonable level of optimization while maintaining fast compilation and a good debugging experience.)
  #-Os (Optimize for size. -Os enables all -O2 optimizations that do not typically increase code size. It also performs further optimizations designed to reduce code size. -Os disables the following optimization flags: -falign-functions -falign-jumps -falign-loops -falign-labels -freorder-blocks -freorder-blocks-and-partition -fprefetch-loop-arrays -ftree-vect-loop-version)


	-Og
)

