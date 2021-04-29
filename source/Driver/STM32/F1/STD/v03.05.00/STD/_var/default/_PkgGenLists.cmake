# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "std")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
	./_var/default/Src/stm32f10x_adc.c
	./_var/default/Src/stm32f10x_bkp.c
	./_var/default/Src/stm32f10x_dac.c
	./_var/default/Src/stm32f10x_dma.c
	./_var/default/Src/stm32f10x_flash.c
	./_var/default/Src/stm32f10x_gpio.c
	./_var/default/Src/stm32f10x_i2c.c
	./_var/default/Src/stm32f10x_misc.c
	./_var/default/Src/stm32f10x_pwr.c
	./_var/default/Src/stm32f10x_rcc.c
	./_var/default/Src/stm32f10x_rtc.c
	./_var/default/Src/stm32f10x_spi.c
	./_var/default/Src/stm32f10x_tim.c
	./_var/default/Src/stm32f10x_usart.c
	./_var/default/Src/stm32f10x_wwdg.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Inc/stm32f10x_adc.h
	./Inc/stm32f10x_bkp.h
	./Inc/stm32f10x_dac.h
	./Inc/stm32f10x_dma.h
	./Inc/stm32f10x_flash.h
	./Inc/stm32f10x_gpio.h
	./Inc/stm32f10x_i2c.h
	./Inc/stm32f10x_misc.h
	./Inc/stm32f10x_pwr.h
	./Inc/stm32f10x_rcc.h
	./Inc/stm32f10x_rtc.h
	./Inc/stm32f10x_spi.h
	./Inc/stm32f10x_tim.h
	./Inc/stm32f10x_usart.h
	./Inc/stm32f10x_wwdg.h
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

