# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "driver")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
	./_var/default/src/nrfx_adc.c
	./_var/default/src/nrfx_clock.c
	./_var/default/src/nrfx_comp.c
	./_var/default/src/nrfx_dppi.c
	./_var/default/src/nrfx_gpiote.c
	./_var/default/src/nrfx_i2s.c
	./_var/default/src/nrfx_lpcomp.c
	./_var/default/src/nrfx_nfct.c
	./_var/default/src/nrfx_nvmc.c
	./_var/default/src/nrfx_pdm.c
	./_var/default/src/nrfx_power.c
	./_var/default/src/nrfx_ppi.c
	./_var/default/src/nrfx_pwm.c
	./_var/default/src/nrfx_qdec.c
	./_var/default/src/nrfx_qspi.c
	./_var/default/src/nrfx_rng.c
	./_var/default/src/nrfx_rtc.c
	./_var/default/src/nrfx_saadc.c
	./_var/default/src/nrfx_spi.c
	./_var/default/src/nrfx_spim.c
	./_var/default/src/nrfx_spis.c
	./_var/default/src/nrfx_swi.c
	./_var/default/src/nrfx_systick.c
	./_var/default/src/nrfx_temp.c
	./_var/default/src/nrfx_timer.c
	./_var/default/src/nrfx_twi.c
	./_var/default/src/nrfx_twi_twim.c
	./_var/default/src/nrfx_twim.c
	./_var/default/src/nrfx_twis.c
	./_var/default/src/nrfx_uart.c
	./_var/default/src/nrfx_uarte.c
	./_var/default/src/nrfx_usbd.c
	./_var/default/src/nrfx_wdt.c
	./_var/default/src/prs/nrfx_prs.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./include/nrf_bitmask.h
	./include/nrfx_adc.h
	./include/nrfx_clock.h
	./include/nrfx_comp.h
	./include/nrfx_dppi.h
	./include/nrfx_gpiote.h
	./include/nrfx_i2s.h
	./include/nrfx_lpcomp.h
	./include/nrfx_nfct.h
	./include/nrfx_nvmc.h
	./include/nrfx_pdm.h
	./include/nrfx_power.h
	./include/nrfx_power_clock.h
	./include/nrfx_ppi.h
	./include/nrfx_pwm.h
	./include/nrfx_qdec.h
	./include/nrfx_qspi.h
	./include/nrfx_rng.h
	./include/nrfx_rtc.h
	./include/nrfx_saadc.h
	./include/nrfx_saadc_v2.h
	./include/nrfx_spi.h
	./include/nrfx_spim.h
	./include/nrfx_spis.h
	./include/nrfx_swi.h
	./include/nrfx_systick.h
	./include/nrfx_temp.h
	./include/nrfx_timer.h
	./include/nrfx_twi.h
	./include/nrfx_twi_twim.h
	./include/nrfx_twim.h
	./include/nrfx_twis.h
	./include/nrfx_uart.h
	./include/nrfx_uarte.h
	./include/nrfx_usbd.h
	./include/nrfx_wdt.h
	./nrfx.h
	./nrfx_common.h
	./nrfx_errors.h
	# List of all source files found in default_gen_headers
	./_var/default/src/nrfx_usbd_errata.h
	./_var/default/src/prs/nrfx_prs.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in default_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./include
	.
	# List of all source files found in default_gen_include_directories
	./_var/default/src
	./_var/default/src/prs
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

