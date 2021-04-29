# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "main")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./main.cpp
	./stdio/printf.c
	./syscalls.c
	./ub_lib/bilder/GUI1.c
	./ub_lib/bilder/GUI2.c
	./ub_lib/font/ub_font_arial_7x10.c
	./ub_lib/stm32_ub_button.c
	./ub_lib/stm32_ub_font.c
	./ub_lib/stm32_ub_graphic2d.c
	./ub_lib/stm32_ub_i2c3.c
	./ub_lib/stm32_ub_lcd_ili9341.c
	./ub_lib/stm32_ub_led.c
	./ub_lib/stm32_ub_sdram.c
	./ub_lib/stm32_ub_spi5.c
	./ub_lib/stm32_ub_systick.c
	./ub_lib/stm32_ub_touch_stmpe811.c
	./ub_lib/stm32_ub_uart.c
	./uboszi_lib/adc.c
	./uboszi_lib/fft.c
	./uboszi_lib/menu.c
	./uboszi_lib/oszi.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./main.h
	./ub_lib/stm32_ub_button.h
	./ub_lib/stm32_ub_font.h
	./ub_lib/stm32_ub_graphic2d.h
	./ub_lib/stm32_ub_i2c3.h
	./ub_lib/stm32_ub_lcd_ili9341.h
	./ub_lib/stm32_ub_led.h
	./ub_lib/stm32_ub_sdram.h
	./ub_lib/stm32_ub_spi5.h
	./ub_lib/stm32_ub_systick.h
	./ub_lib/stm32_ub_touch_stmpe811.h
	./ub_lib/stm32_ub_uart.h
	./uboszi_lib/adc.h
	./uboszi_lib/fft.h
	./uboszi_lib/menu.h
	./uboszi_lib/oszi.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	./ub_lib
	./uboszi_lib
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

