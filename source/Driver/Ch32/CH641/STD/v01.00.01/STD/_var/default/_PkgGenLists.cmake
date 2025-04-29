# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "std")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
	./_var/default/Src/ch641_adc.c
	./_var/default/Src/ch641_dbgmcu.c
	./_var/default/Src/ch641_dma.c
	./_var/default/Src/ch641_exti.c
	./_var/default/Src/ch641_flash.c
	./_var/default/Src/ch641_gpio.c
	./_var/default/Src/ch641_i2c.c
	./_var/default/Src/ch641_misc.c
	./_var/default/Src/ch641_pwr.c
	./_var/default/Src/ch641_rcc.c
	./_var/default/Src/ch641_tim.c
	./_var/default/Src/ch641_usart.c
	./_var/default/Src/ch641_wwdg.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Inc/ch641.h
	./Inc/ch641_adc.h
	./Inc/ch641_dbgmcu.h
	./Inc/ch641_dma.h
	./Inc/ch641_exti.h
	./Inc/ch641_flash.h
	./Inc/ch641_gpio.h
	./Inc/ch641_i2c.h
	./Inc/ch641_misc.h
	./Inc/ch641_pwr.h
	./Inc/ch641_rcc.h
	./Inc/ch641_tim.h
	./Inc/ch641_usart.h
	./Inc/ch641_usbpd.h
	./Inc/ch641_wwdg.h
	# List of all source files found in default_gen_headers
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in default_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./Inc
	# List of all source files found in default_gen_include_directories
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

