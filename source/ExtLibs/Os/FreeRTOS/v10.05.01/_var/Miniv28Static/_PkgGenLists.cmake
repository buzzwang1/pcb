# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "FreeRTOS")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./CMSIS_RTOS_V2/cmsis_os2.c
	./croutine.c
	./event_groups.c
	./list.c
	./portable/GCC/ARM_CM33_NTZ/non_secure/port.c
	./portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
	./queue.c
	./stream_buffer.c
	./tasks.c
	./timers.c
	# List of all source files found in Miniv28Static_gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./CMSIS_RTOS_V2/cmsis_os.h
	./CMSIS_RTOS_V2/cmsis_os2.h
	./CMSIS_RTOS_V2/freertos_mpool.h
	./CMSIS_RTOS_V2/freertos_os2.h
	./CMSIS_RTOS_V2/os_tick.h
	./include/FreeRTOS.h
	./include/FreeRTOSConfig_template.h
	./include/StackMacros.h
	./include/atomic.h
	./include/croutine.h
	./include/deprecated_definitions.h
	./include/event_groups.h
	./include/list.h
	./include/message_buffer.h
	./include/mpu_prototypes.h
	./include/mpu_wrappers.h
	./include/portable.h
	./include/projdefs.h
	./include/queue.h
	./include/semphr.h
	./include/stack_macros.h
	./include/stream_buffer.h
	./include/task.h
	./include/timers.h
	./portable/GCC/ARM_CM33_NTZ/non_secure/portasm.h
	./portable/GCC/ARM_CM33_NTZ/non_secure/portmacro.h
	./portable/GCC/ARM_CM33_NTZ/non_secure/portmacrocommon.h
	# List of all source files found in Miniv28Static_gen_headers
	./_var/Miniv28Static/FreeRTOSConfig.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in Miniv28Static_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./CMSIS_RTOS_V2
	./include
	./portable/GCC/ARM_CM33_NTZ/non_secure
	# List of all source files found in Miniv28Static_gen_include_directories
	./_var/Miniv28Static
)


# List of all source files excluded by the user
set (gen_excluded_sourcefiles 
)


# List of all header files excluded by the user
set (gen_excluded_headerfiles 
)


# List of all include folders excluded by the user
set (gen_excluded_includes 
)


# Final list of source files. allfiles minus excluded files
if (gen_files AND gen_excluded_sourcefiles)
    list(REMOVE_ITEM gen_files ${gen_excluded_sourcefiles})
endif()

# Final list of source files. allfiles minus excluded files
if (gen_asmfiles AND gen_excluded_sourcefiles)
    list(REMOVE_ITEM gen_asmfiles ${gen_excluded_sourcefiles})
endif()

# Final list of header files. allfiles minus excluded headers
if (gen_headers AND gen_excluded_headerfiles)
    list(REMOVE_ITEM gen_headers ${gen_excluded_headerfiles})
endif()

# Final list of included folders. allfolders minus excluded folders
if (gen_include_directories AND gen_excluded_includes)
    list(REMOVE_ITEM gen_include_directories ${gen_excluded_includes})
endif()

