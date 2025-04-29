# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "softdevice")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in sd_gen_files
	# List of all source files found in sd_s140_gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	# List of all source files found in sd_gen_headers
	./_var/sd/mbr/headers/nrf_mbr.h
	./_var/sd/mbr/headers/nrf_svc.h
	# List of all source files found in sd_s140_gen_headers
	./_var/sd/_var/s140/headers/ble.h
	./_var/sd/_var/s140/headers/ble_err.h
	./_var/sd/_var/s140/headers/ble_gap.h
	./_var/sd/_var/s140/headers/ble_gatt.h
	./_var/sd/_var/s140/headers/ble_gattc.h
	./_var/sd/_var/s140/headers/ble_gatts.h
	./_var/sd/_var/s140/headers/ble_hci.h
	./_var/sd/_var/s140/headers/ble_l2cap.h
	./_var/sd/_var/s140/headers/ble_ranges.h
	./_var/sd/_var/s140/headers/ble_types.h
	./_var/sd/_var/s140/headers/nrf52/nrf_mbr.h
	./_var/sd/_var/s140/headers/nrf_error.h
	./_var/sd/_var/s140/headers/nrf_error_sdm.h
	./_var/sd/_var/s140/headers/nrf_error_soc.h
	./_var/sd/_var/s140/headers/nrf_nvic.h
	./_var/sd/_var/s140/headers/nrf_sd_def.h
	./_var/sd/_var/s140/headers/nrf_sdm.h
	./_var/sd/_var/s140/headers/nrf_soc.h
	./_var/sd/_var/s140/headers/nrf_svc.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in sd_gen_asmfiles
	# List of all source files found in sd_s140_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	# List of all source files found in sd_gen_include_directories
	./_var/sd/mbr/headers
	# List of all source files found in sd_s140_gen_include_directories
	./_var/sd/_var/s140/headers
	./_var/sd/_var/s140/headers/nrf52
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

