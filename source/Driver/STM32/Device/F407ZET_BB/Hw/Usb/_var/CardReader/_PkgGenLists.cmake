# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "usb")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in CardReader_gen_files
	./_var/CardReader/DeviceLib/Class/msc/src/usbd_msc_bot.c
	./_var/CardReader/DeviceLib/Class/msc/src/usbd_msc_core.c
	./_var/CardReader/DeviceLib/Class/msc/src/usbd_msc_data.c
	./_var/CardReader/DeviceLib/Class/msc/src/usbd_msc_scsi.c
	./_var/CardReader/DeviceLib/Core/src/usbd_core.c
	./_var/CardReader/DeviceLib/Core/src/usbd_ioreq.c
	./_var/CardReader/DeviceLib/Core/src/usbd_req.c
	./_var/CardReader/OTG_Driver/src/usb_core.c
	./_var/CardReader/OTG_Driver/src/usb_dcd.c
	./_var/CardReader/OTG_Driver/src/usb_dcd_int.c
	./_var/CardReader/USB_APP/usb_bsp.c
	./_var/CardReader/USB_APP/usbd_desc.c
	./_var/CardReader/USB_APP/usbd_storage_msd.c
	./_var/CardReader/USB_APP/usbd_usr.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	# List of all source files found in CardReader_gen_headers
	./_var/CardReader/DeviceLib/Class/msc/inc/usbd_msc_bot.h
	./_var/CardReader/DeviceLib/Class/msc/inc/usbd_msc_core.h
	./_var/CardReader/DeviceLib/Class/msc/inc/usbd_msc_data.h
	./_var/CardReader/DeviceLib/Class/msc/inc/usbd_msc_mem.h
	./_var/CardReader/DeviceLib/Class/msc/inc/usbd_msc_scsi.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_conf_template.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_core.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_def.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_ioreq.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_req.h
	./_var/CardReader/DeviceLib/Core/inc/usbd_usr.h
	./_var/CardReader/OTG_Driver/inc/usb_bsp.h
	./_var/CardReader/OTG_Driver/inc/usb_conf_template.h
	./_var/CardReader/OTG_Driver/inc/usb_core.h
	./_var/CardReader/OTG_Driver/inc/usb_dcd.h
	./_var/CardReader/OTG_Driver/inc/usb_dcd_int.h
	./_var/CardReader/OTG_Driver/inc/usb_defines.h
	./_var/CardReader/OTG_Driver/inc/usb_hcd.h
	./_var/CardReader/OTG_Driver/inc/usb_hcd_int.h
	./_var/CardReader/OTG_Driver/inc/usb_otg.h
	./_var/CardReader/OTG_Driver/inc/usb_regs.h
	./_var/CardReader/USB_APP/usb_conf.h
	./_var/CardReader/USB_APP/usbd_conf.h
	./_var/CardReader/USB_APP/usbd_desc.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in CardReader_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	# List of all source files found in CardReader_gen_include_directories
	./_var/CardReader/DeviceLib/Class/msc/inc
	./_var/CardReader/DeviceLib/Core/inc
	./_var/CardReader/OTG_Driver/inc
	./_var/CardReader/USB_APP
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

