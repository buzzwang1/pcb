

list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DVECT_TAB_OFFSET=0x0000 # 0k Offset
  #-DTESTIRQLOCK
  #-DTESTU1IRQ
  #-DTESTU2IRQ
  #-DTESTI2C1IRQ
  #-DTESTI2C2IRQ
  #-DTESTRFIRQ
  #-DTESTSYSTIMIRQ
  #-DTESTSYS1MSHPIRQ
  #-DTESTSYS10MSLPIRQ
  #-DTESTSYS100MSLPIRQ
  #-DTESTSYS1000MSLPIRQ
  #-DTESTSYSRUNTIME
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

