# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "integration")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./nrfx/legacy/nrf_drv_clock.c
	./nrfx/legacy/nrf_drv_power.c
	./nrfx/legacy/nrf_drv_ppi.c
	./nrfx/legacy/nrf_drv_rng.c
	./nrfx/legacy/nrf_drv_spi.c
	./nrfx/legacy/nrf_drv_spis.c
	./nrfx/legacy/nrf_drv_swi.c
	./nrfx/legacy/nrf_drv_twi.c
	./nrfx/legacy/nrf_drv_uart.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./nrfx/legacy/apply_old_config.h
	./nrfx/legacy/nrf_drv_clock.h
	./nrfx/legacy/nrf_drv_common.h
	./nrfx/legacy/nrf_drv_comp.h
	./nrfx/legacy/nrf_drv_gpiote.h
	./nrfx/legacy/nrf_drv_i2s.h
	./nrfx/legacy/nrf_drv_lpcomp.h
	./nrfx/legacy/nrf_drv_pdm.h
	./nrfx/legacy/nrf_drv_power.h
	./nrfx/legacy/nrf_drv_ppi.h
	./nrfx/legacy/nrf_drv_pwm.h
	./nrfx/legacy/nrf_drv_qdec.h
	./nrfx/legacy/nrf_drv_qspi.h
	./nrfx/legacy/nrf_drv_rng.h
	./nrfx/legacy/nrf_drv_rtc.h
	./nrfx/legacy/nrf_drv_saadc.h
	./nrfx/legacy/nrf_drv_spi.h
	./nrfx/legacy/nrf_drv_spis.h
	./nrfx/legacy/nrf_drv_swi.h
	./nrfx/legacy/nrf_drv_systick.h
	./nrfx/legacy/nrf_drv_timer.h
	./nrfx/legacy/nrf_drv_twi.h
	./nrfx/legacy/nrf_drv_twis.h
	./nrfx/legacy/nrf_drv_uart.h
	./nrfx/legacy/nrf_drv_usbd.h
	./nrfx/legacy/nrf_drv_usbd_errata.h
	./nrfx/legacy/nrf_drv_wdt.h
	./nrfx/nrfx_config.h
	./nrfx/nrfx_glue.h
	./nrfx/nrfx_log.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./nrfx/legacy
	./nrfx
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

