message(STATUS "${PCB_ProjectCfg} configuration")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-riscv_buildoptions_${PCB_ProjectCfg}.cmake")

list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DPCB_PROJECT_${PCB_Project}
  -DEMBEDDED
  
  -DCH641


# -D__NO_SYSTEM_INIT

# -D__FPU_PRESENT
)

#CH32V305: V4F IMAFC

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------
  # https://www.sifive.com/blog/all-aboard-part-1-compiler-args

  -march=rv32ecxw 
  -mabi=ilp32e
  -msmall-data-limit=0
  -msave-restore

  -Wall
  -g3
  -fdata-sections
  -ffast-math
  -ffunction-sections
  -Wdouble-promotion
)

list(APPEND PCB_ListTargetCompileOptionsAsm
# ASM specific settings:
# -----------------------------------------------
)


list(APPEND PCB_ListTargetCompileOptionsCpp
# C++ specific settings:
# -----------------------------------------------
  -fmessage-length=0 
  -fsigned-char 
  -fdata-sections 
  -Wextra  
  -std=gnu++11 
  -fabi-version=6 
  -fno-exceptions 
  -fno-rtti 
  -fno-use-cxa-atexit 
  -fno-threadsafe-statics 
  -nostdinc++  
)

list(APPEND PCB_ListTargetCompileOptionsC
# C specific settings:
# -----------------------------------------------

  -std=c11
  -fmessage-length=0 
  -fsigned-char 
  -fdata-sections 
  -Wextra  
  -std=gnu99  
)

list(APPEND PCB_ListTargetLinkOptions
# Linker specific settings:
# -----------------------------------------------
  ${PCB_ListTargetCompileOptionsCommon}

  --specs=nano.specs
  --specs=nosys.specs

  # -lstdc++ 
  # -lsupc++ 
  # -lnosys

  -lm

  -nostartfiles # https://stackoverflow.com/questions/43050089/when-is-the-gcc-flag-nostartfiles-used
                # https://stackoverflow.com/questions/13734745/why-do-i-have-an-undefined-reference-to-init-in-libc-init-array
                # https://stackoverflow.com/questions/15265295/understanding-the-libc-init-array

  #-eReset_Handler
  -ehandle_reset

  -Wl,--gc-sections
  -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${PCB_Project}.map
  # Don't optimize the startup code
  -Wl,--whole-archive -L${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -l${PCB_PkgPrefix}core_lib  -Wl,--no-whole-archive    
  -T${LINKER_SCRIPT}
)



