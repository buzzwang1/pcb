# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "ucos")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./port/os_cpu_c.c
	./port/os_dbg.c
	./source/os_core.c
	./source/os_dbg_r.c
	./source/os_flag.c
	./source/os_mbox.c
	./source/os_mem.c
	./source/os_mutex.c
	./source/os_q.c
	./source/os_sem.c
	./source/os_task.c
	./source/os_time.c
	./source/os_tmr.c
	./source/ucos_ii.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./include/includes.h
	./include/os_cfg.h
	./include/os_cfg_r.h
	./include/os_cpu.h
	./include/ucos_ii.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	./port/os_cpu_a.s
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./include
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

