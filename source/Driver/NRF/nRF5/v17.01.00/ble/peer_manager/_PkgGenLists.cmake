# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "ble_peer_manager")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./auth_status_tracker.c
	./gatt_cache_manager.c
	./gatts_cache_manager.c
	./id_manager.c
	./nrf_ble_lesc.c
	./peer_data_storage.c
	./peer_database.c
	./peer_id.c
	./peer_manager.c
	./peer_manager_handler.c
	./pm_buffer.c
	./security_dispatcher.c
	./security_manager.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./auth_status_tracker.h
	./gatt_cache_manager.h
	./gatts_cache_manager.h
	./id_manager.h
	./nrf_ble_lesc.h
	./peer_data_storage.h
	./peer_database.h
	./peer_id.h
	./peer_manager.h
	./peer_manager_handler.h
	./peer_manager_internal.h
	./peer_manager_types.h
	./pm_buffer.h
	./security_dispatcher.h
	./security_manager.h
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

