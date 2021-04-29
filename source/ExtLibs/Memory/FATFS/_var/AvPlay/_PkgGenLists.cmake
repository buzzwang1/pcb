# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "fatfs")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./doc/img/app1.c
	./doc/img/app2.c
	./doc/img/app3.c
	./doc/img/app4.c
	# List of all source files found in AvPlay_gen_files
	./_var/AvPlay/exfuns/exfuns.c
	./_var/AvPlay/exfuns/fattester.c
	./_var/AvPlay/exfuns/mycc936.c
	./_var/AvPlay/src/diskio.c
	./_var/AvPlay/src/ff.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./dummy.h
	# List of all source files found in AvPlay_gen_headers
	./_var/AvPlay/exfuns/exfuns.h
	./_var/AvPlay/exfuns/fattester.h
	./_var/AvPlay/src/diskio.h
	./_var/AvPlay/src/ff.h
	./_var/AvPlay/src/ffconf.h
	./_var/AvPlay/src/integer.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in AvPlay_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	# List of all source files found in AvPlay_gen_include_directories
	./_var/AvPlay/exfuns
	./_var/AvPlay/src
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

