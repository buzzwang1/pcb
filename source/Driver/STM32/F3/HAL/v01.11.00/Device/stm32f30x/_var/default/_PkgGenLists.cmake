# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "Device")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in default_gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./stm32f301x8.h
	./stm32f302x8.h
	./stm32f302xc.h
	./stm32f302xe.h
	./stm32f303x8.h
	./stm32f303xc.h
	./stm32f303xe.h
	./stm32f318xx.h
	./stm32f328xx.h
	./stm32f334x8.h
	./stm32f358xx.h
	./stm32f373xc.h
	./stm32f378xx.h
	./stm32f398xx.h
	# List of all source files found in default_gen_headers
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in default_gen_asmfiles
	./_var/default/startup_stm32f30x.S
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
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

