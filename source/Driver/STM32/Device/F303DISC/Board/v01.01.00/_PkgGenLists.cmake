# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "f303board")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./STM32_USB-FS-Device_Driver/src/usb_core.c
	./STM32_USB-FS-Device_Driver/src/usb_init.c
	./STM32_USB-FS-Device_Driver/src/usb_int.c
	./STM32_USB-FS-Device_Driver/src/usb_mem.c
	./STM32_USB-FS-Device_Driver/src/usb_regs.c
	./STM32_USB-FS-Device_Driver/src/usb_sil.c
	./stm32f3_discovery.c
	./stm32f3_discovery_l3gd20.c
	./stm32f3_discovery_lsm303dlhc.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./STM32_USB-FS-Device_Driver/inc/usb_core.h
	./STM32_USB-FS-Device_Driver/inc/usb_def.h
	./STM32_USB-FS-Device_Driver/inc/usb_init.h
	./STM32_USB-FS-Device_Driver/inc/usb_int.h
	./STM32_USB-FS-Device_Driver/inc/usb_lib.h
	./STM32_USB-FS-Device_Driver/inc/usb_mem.h
	./STM32_USB-FS-Device_Driver/inc/usb_regs.h
	./STM32_USB-FS-Device_Driver/inc/usb_sil.h
	./STM32_USB-FS-Device_Driver/inc/usb_type.h
	./stm32f3_discovery.h
	./stm32f3_discovery_l3gd20.h
	./stm32f3_discovery_lsm303dlhc.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./STM32_USB-FS-Device_Driver/inc
	.
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

