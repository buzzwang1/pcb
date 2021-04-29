# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "stm32F4discovery_board")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./fonts.c
	./lcd_log.c
	./stm32f4_discovery.c
	./stm32f4_discovery_audio_codec.c
	./stm32f4_discovery_debug.c
	./stm32f4_discovery_lcd.c
	./stm32f4_discovery_lis302dl.c
	./stm32f4_discovery_sdio_sd.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./fonts.h
	./lcd_log.h
	./lcd_log_conf.h
	./stm32f4_discovery.h
	./stm32f4_discovery_audio_codec.h
	./stm32f4_discovery_debug.h
	./stm32f4_discovery_lcd.h
	./stm32f4_discovery_lis302dl.h
	./stm32f4_discovery_sdio_sd.h
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

