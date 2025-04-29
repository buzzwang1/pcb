# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "f429iboard")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./Common/fonts.c
	./Common/lcd_log.c
	./STM32F429I-Discovery/stm32f429i_discovery.c
	./STM32F429I-Discovery/stm32f429i_discovery_i2c_ee.c
	./STM32F429I-Discovery/stm32f429i_discovery_ioe.c
	./STM32F429I-Discovery/stm32f429i_discovery_l3gd20.c
	./STM32F429I-Discovery/stm32f429i_discovery_lcd.c
	./STM32F429I-Discovery/stm32f429i_discovery_sdram.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Common/fonts.h
	./Common/lcd_log.h
	./Common/lcd_log_conf_template.h
	./Common/stm32_eval_legacy.h
	./STM32F429I-Discovery/stm32f429i_discovery.h
	./STM32F429I-Discovery/stm32f429i_discovery_i2c_ee.h
	./STM32F429I-Discovery/stm32f429i_discovery_ioe.h
	./STM32F429I-Discovery/stm32f429i_discovery_l3gd20.h
	./STM32F429I-Discovery/stm32f429i_discovery_lcd.h
	./STM32F429I-Discovery/stm32f429i_discovery_sdram.h
	./lcd_log_conf.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./Common
	./STM32F429I-Discovery
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

