# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "fatfs")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./doc/img/app1.c
	./doc/img/app2.c
	./doc/img/app3.c
	./doc/img/app4.c
	# List of all source files found in FatFs_gen_files
	./_var/FatFs/exfuns/exfuns.c
	./_var/FatFs/exfuns/fattester.c
	./_var/FatFs/src/diskio.c
	./_var/FatFs/src/ff.c
	./_var/FatFs/src/option/cc936.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./dummy.h
	# List of all source files found in FatFs_gen_headers
	./_var/FatFs/dummy.h
	./_var/FatFs/exfuns/exfuns.h
	./_var/FatFs/exfuns/fattester.h
	./_var/FatFs/src/diskio.h
	./_var/FatFs/src/ff.h
	./_var/FatFs/src/ffconf.h
	./_var/FatFs/src/integer.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in FatFs_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	# List of all source files found in FatFs_gen_include_directories
	./_var/FatFs
	./_var/FatFs/exfuns
	./_var/FatFs/src
)


# List of all source files excluded by the user
set (gen_excluded_sourcefiles 
	./doc/img/app1.c
	./doc/img/app2.c
	./doc/img/app3.c
	./doc/img/app4.c
)


# List of all header files excluded by the user
set (gen_excluded_headerfiles 
)


# List of all include folders excluded by the user
set (gen_excluded_includes 
	./doc/img
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

