# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "main")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./LedCtrl.cpp
	./USB_udisk_Device/Internal_Flash.cpp
	./USB_udisk_Device/SPI_FLASH.cpp
	./USB_udisk_Device/SW_UDISK.cpp
	./USB_udisk_Device/ch32v30x_usbhs_device.cpp
	./USB_udisk_Device/usb_desc.cpp
	./main.cpp
	./syscalls.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./LedCtrl.h
	./USB_udisk_Device/Internal_Flash.h
	./USB_udisk_Device/SPI_FLASH.h
	./USB_udisk_Device/SW_UDISK.h
	./USB_udisk_Device/ch32v30x_usbhs_device.h
	./USB_udisk_Device/usb_desc.h
	./ch32v30x_conf.h
	./main.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	.
	./USB_udisk_Device
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

