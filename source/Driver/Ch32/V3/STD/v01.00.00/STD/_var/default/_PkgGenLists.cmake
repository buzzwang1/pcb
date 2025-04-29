# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "std")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
	./_var/default/Src/ch32v30x_adc.c
	./_var/default/Src/ch32v30x_bkp.c
	./_var/default/Src/ch32v30x_can.c
	./_var/default/Src/ch32v30x_crc.c
	./_var/default/Src/ch32v30x_dac.c
	./_var/default/Src/ch32v30x_dbgmcu.c
	./_var/default/Src/ch32v30x_dma.c
	./_var/default/Src/ch32v30x_dvp.c
	./_var/default/Src/ch32v30x_eth.c
	./_var/default/Src/ch32v30x_exti.c
	./_var/default/Src/ch32v30x_flash.c
	./_var/default/Src/ch32v30x_fsmc.c
	./_var/default/Src/ch32v30x_gpio.c
	./_var/default/Src/ch32v30x_i2c.c
	./_var/default/Src/ch32v30x_iwdg.c
	./_var/default/Src/ch32v30x_misc.c
	./_var/default/Src/ch32v30x_opa.c
	./_var/default/Src/ch32v30x_pwr.c
	./_var/default/Src/ch32v30x_rcc.c
	./_var/default/Src/ch32v30x_rng.c
	./_var/default/Src/ch32v30x_rtc.c
	./_var/default/Src/ch32v30x_sdio.c
	./_var/default/Src/ch32v30x_spi.c
	./_var/default/Src/ch32v30x_tim.c
	./_var/default/Src/ch32v30x_usart.c
	./_var/default/Src/ch32v30x_wwdg.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Inc/ch32v30x.h
	./Inc/ch32v30x_adc.h
	./Inc/ch32v30x_bkp.h
	./Inc/ch32v30x_can.h
	./Inc/ch32v30x_crc.h
	./Inc/ch32v30x_dac.h
	./Inc/ch32v30x_dbgmcu.h
	./Inc/ch32v30x_dma.h
	./Inc/ch32v30x_dvp.h
	./Inc/ch32v30x_eth.h
	./Inc/ch32v30x_exti.h
	./Inc/ch32v30x_flash.h
	./Inc/ch32v30x_fsmc.h
	./Inc/ch32v30x_gpio.h
	./Inc/ch32v30x_i2c.h
	./Inc/ch32v30x_iwdg.h
	./Inc/ch32v30x_misc.h
	./Inc/ch32v30x_opa.h
	./Inc/ch32v30x_pwr.h
	./Inc/ch32v30x_rcc.h
	./Inc/ch32v30x_rng.h
	./Inc/ch32v30x_rtc.h
	./Inc/ch32v30x_sdio.h
	./Inc/ch32v30x_spi.h
	./Inc/ch32v30x_tim.h
	./Inc/ch32v30x_usart.h
	./Inc/ch32v30x_usb.h
	./Inc/ch32v30x_wwdg.h
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

