
message(STATUS "${PCB_ProjectCfg} configuration")
include("${CMAKE_CURRENT_LIST_DIR}/gnu-arm_cm4_buildoptions_${PCB_ProjectCfg}.cmake")

list(APPEND PCB_ListTargetDefinition
# Compile Definitions
# -----------------------------------------------
  -DPCB_PROJECT_${PCB_Project}
  -DEMBEDDED
  
#  -D__NO_SYSTEM_INIT


  -DAPP_TIMER_V2
  -DAPP_TIMER_V2_RTC1_ENABLED
  -DBLE_STACK_SUPPORT_REQD
  -DBOARD_CUSTOM
  -DCONFIG_GPIO_AS_PINRESET
  -DFLOAT_ABI_HARD
  #-DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
  -DNRF52840_XXAA
  #-DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
  -DNRF_SD_BLE_API_VERSION=7
  -DS140
  -DSOFTDEVICE_PRESENT

  -D__STACK_SIZE=0x2000 #8kb
  -D__HEAP_SIZE=0x8000 #32kb

  -DARM_MATH_CM4
  -DARM_MATH_MATRIX_CHECK
  -DARM_MATH_ROUNDING
)

list(APPEND PCB_ListTargetCompileOptionsCommon
# Target specific settings:
# -----------------------------------------------
  -mcpu=cortex-m4 
  -mthumb 
  -mfloat-abi=hard
  -mfpu=fpv4-sp-d16

  -Wall
  -g3
  -fdata-sections
  #-ffast-math
  -ffunction-sections
  -Wdouble-promotion
)

list(APPEND PCB_ListTargetCompileOptionsCommon
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
  -std=gnu++17
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

  #--specs=nosys.specs
  --specs=nano.specs

  # -lstdc++ 
  # -lsupc++ 
  # -lnosys

  -lm

  # -nostartfiles # https://stackoverflow.com/questions/43050089/when-is-the-gcc-flag-nostartfiles-used
                  # https://stackoverflow.com/questions/13734745/why-do-i-have-an-undefined-reference-to-init-in-libc-init-array
                  # https://stackoverflow.com/questions/15265295/understanding-the-libc-init-array

  -eReset_Handler

  -Wl,--gc-sections
  -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${PCB_Project}.map
  # Don't optimize the startup code
  -Wl,--whole-archive -L${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -l${PCB_PkgPrefix}core_lib -l${PCB_PkgPrefix}integration_lib -l${PCB_PkgPrefix}sdh_ble_lib -l${PCB_PkgPrefix}sdh_soc_lib -Wl,--no-whole-archive
  -T${LINKER_SCRIPT}
)



