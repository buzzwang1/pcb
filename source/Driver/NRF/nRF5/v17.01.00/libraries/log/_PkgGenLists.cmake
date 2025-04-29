# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "log")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./src/nrf_log_backend_flash.c
	./src/nrf_log_backend_rtt.c
	./src/nrf_log_backend_serial.c
	./src/nrf_log_backend_uart.c
	./src/nrf_log_default_backends.c
	./src/nrf_log_frontend.c
	./src/nrf_log_str_formatter.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./nrf_log.h
	./nrf_log_backend_flash.h
	./nrf_log_backend_interface.h
	./nrf_log_backend_rtt.h
	./nrf_log_backend_uart.h
	./nrf_log_ctrl.h
	./nrf_log_default_backends.h
	./nrf_log_instance.h
	./nrf_log_str_formatter.h
	./nrf_log_types.h
	./src/nrf_log_backend_serial.h
	./src/nrf_log_ctrl_internal.h
	./src/nrf_log_internal.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	./src
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

