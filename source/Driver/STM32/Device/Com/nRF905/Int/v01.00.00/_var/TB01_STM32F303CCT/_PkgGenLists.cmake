# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "nrf905")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./cnRF905.cpp
	# List of all source files found in TB01_STM32F303CCT_gen_files
	./_var/TB01_STM32F303CCT/nRF905_spi.cpp
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./cnrf905.h
	# List of all source files found in TB01_STM32F303CCT_gen_headers
	./_var/TB01_STM32F303CCT/nrf905_spi.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in TB01_STM32F303CCT_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	# List of all source files found in TB01_STM32F303CCT_gen_include_directories
	./_var/TB01_STM32F303CCT
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
