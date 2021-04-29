# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "main")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./src/fs.c
	./src/fsdata.c
	./src/httpd.c
	./src/httpd_cgi_ssi.c
	./src/main.c
	./src/netconf.c
	./src/serial_debug.c
	./src/stm32f4x7_eth_bsp.c
	./src/stm32f4xx_it.c
	./src/system_stm32f4xx.c
	./syscalls.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./inc/fs.h
	./inc/fsdata.h
	./inc/httpd.h
	./inc/httpd_structs.h
	./inc/lwipopts.h
	./inc/main.h
	./inc/netconf.h
	./inc/serial_debug.h
	./inc/stm32f4x7_eth_bsp.h
	./inc/stm32f4x7_eth_conf.h
	./inc/stm32f4xx_it.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./inc
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

