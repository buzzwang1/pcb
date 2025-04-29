# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "hal")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./Inc/nrf_ecb.c
	./Inc/nrf_nvmc.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Inc/nrf_aar.h
	./Inc/nrf_acl.h
	./Inc/nrf_adc.h
	./Inc/nrf_bprot.h
	./Inc/nrf_ccm.h
	./Inc/nrf_clock.h
	./Inc/nrf_comp.h
	./Inc/nrf_dppi.h
	./Inc/nrf_ecb.h
	./Inc/nrf_egu.h
	./Inc/nrf_ficr.h
	./Inc/nrf_gpio.h
	./Inc/nrf_gpiote.h
	./Inc/nrf_i2s.h
	./Inc/nrf_kmu.h
	./Inc/nrf_lpcomp.h
	./Inc/nrf_mpu.h
	./Inc/nrf_mwu.h
	./Inc/nrf_nfct.h
	./Inc/nrf_nvmc.h
	./Inc/nrf_pdm.h
	./Inc/nrf_power.h
	./Inc/nrf_ppi.h
	./Inc/nrf_pwm.h
	./Inc/nrf_qdec.h
	./Inc/nrf_qspi.h
	./Inc/nrf_radio.h
	./Inc/nrf_regulators.h
	./Inc/nrf_rng.h
	./Inc/nrf_rtc.h
	./Inc/nrf_saadc.h
	./Inc/nrf_spi.h
	./Inc/nrf_spim.h
	./Inc/nrf_spis.h
	./Inc/nrf_spu.h
	./Inc/nrf_systick.h
	./Inc/nrf_temp.h
	./Inc/nrf_timer.h
	./Inc/nrf_twi.h
	./Inc/nrf_twim.h
	./Inc/nrf_twis.h
	./Inc/nrf_uart.h
	./Inc/nrf_uarte.h
	./Inc/nrf_usbd.h
	./Inc/nrf_vmc.h
	./Inc/nrf_wdt.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./Inc
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

