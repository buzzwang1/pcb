# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "soc")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./nrfx_atomic.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./nrfx_atomic.h
	./nrfx_atomic_internal.h
	./nrfx_coredep.h
	./nrfx_irqs.h
	./nrfx_irqs_nrf51.h
	./nrfx_irqs_nrf52810.h
	./nrfx_irqs_nrf52811.h
	./nrfx_irqs_nrf52820.h
	./nrfx_irqs_nrf52832.h
	./nrfx_irqs_nrf52833.h
	./nrfx_irqs_nrf52840.h
	./nrfx_irqs_nrf9160.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
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

