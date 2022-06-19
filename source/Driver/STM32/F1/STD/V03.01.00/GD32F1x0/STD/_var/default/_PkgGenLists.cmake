# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "std")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
	./_var/default/Src/gd32f1x0_adc.c
	./_var/default/Src/gd32f1x0_can.c
	./_var/default/Src/gd32f1x0_cec.c
	./_var/default/Src/gd32f1x0_cmp.c
	./_var/default/Src/gd32f1x0_crc.c
	./_var/default/Src/gd32f1x0_dac.c
	./_var/default/Src/gd32f1x0_dbg.c
	./_var/default/Src/gd32f1x0_dma.c
	./_var/default/Src/gd32f1x0_exti.c
	./_var/default/Src/gd32f1x0_fmc.c
	./_var/default/Src/gd32f1x0_fwdgt.c
	./_var/default/Src/gd32f1x0_gpio.c
	./_var/default/Src/gd32f1x0_i2c.c
	./_var/default/Src/gd32f1x0_ivref.c
	./_var/default/Src/gd32f1x0_misc.c
	./_var/default/Src/gd32f1x0_opa.c
	./_var/default/Src/gd32f1x0_pmu.c
	./_var/default/Src/gd32f1x0_rcu.c
	./_var/default/Src/gd32f1x0_rtc.c
	./_var/default/Src/gd32f1x0_slcd.c
	./_var/default/Src/gd32f1x0_spi.c
	./_var/default/Src/gd32f1x0_syscfg.c
	./_var/default/Src/gd32f1x0_timer.c
	./_var/default/Src/gd32f1x0_tsi.c
	./_var/default/Src/gd32f1x0_usart.c
	./_var/default/Src/gd32f1x0_wwdgt.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Inc/gd32f1x0_adc.h
	./Inc/gd32f1x0_can.h
	./Inc/gd32f1x0_cec.h
	./Inc/gd32f1x0_cmp.h
	./Inc/gd32f1x0_crc.h
	./Inc/gd32f1x0_dac.h
	./Inc/gd32f1x0_dbg.h
	./Inc/gd32f1x0_dma.h
	./Inc/gd32f1x0_exti.h
	./Inc/gd32f1x0_fmc.h
	./Inc/gd32f1x0_fwdgt.h
	./Inc/gd32f1x0_gpio.h
	./Inc/gd32f1x0_i2c.h
	./Inc/gd32f1x0_ivref.h
	./Inc/gd32f1x0_misc.h
	./Inc/gd32f1x0_opa.h
	./Inc/gd32f1x0_pmu.h
	./Inc/gd32f1x0_rcu.h
	./Inc/gd32f1x0_rtc.h
	./Inc/gd32f1x0_slcd.h
	./Inc/gd32f1x0_spi.h
	./Inc/gd32f1x0_syscfg.h
	./Inc/gd32f1x0_timer.h
	./Inc/gd32f1x0_tsi.h
	./Inc/gd32f1x0_usart.h
	./Inc/gd32f1x0_wwdgt.h
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

