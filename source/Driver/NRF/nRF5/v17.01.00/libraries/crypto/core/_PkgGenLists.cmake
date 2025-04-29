# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "crypto")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./nrf_crypto_aead.c
	./nrf_crypto_aes.c
	./nrf_crypto_aes_shared.c
	./nrf_crypto_ecc.c
	./nrf_crypto_ecdh.c
	./nrf_crypto_ecdsa.c
	./nrf_crypto_eddsa.c
	./nrf_crypto_error.c
	./nrf_crypto_hash.c
	./nrf_crypto_hkdf.c
	./nrf_crypto_hmac.c
	./nrf_crypto_init.c
	./nrf_crypto_rng.c
	./nrf_crypto_shared.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./nrf_crypto.h
	./nrf_crypto_aead.h
	./nrf_crypto_aead_backend.h
	./nrf_crypto_aead_shared.h
	./nrf_crypto_aes.h
	./nrf_crypto_aes_backend.h
	./nrf_crypto_aes_shared.h
	./nrf_crypto_ecc.h
	./nrf_crypto_ecc_backend.h
	./nrf_crypto_ecc_shared.h
	./nrf_crypto_ecdh.h
	./nrf_crypto_ecdh_backend.h
	./nrf_crypto_ecdh_shared.h
	./nrf_crypto_ecdsa.h
	./nrf_crypto_ecdsa_backend.h
	./nrf_crypto_ecdsa_shared.h
	./nrf_crypto_eddsa.h
	./nrf_crypto_eddsa_backend.h
	./nrf_crypto_eddsa_shared.h
	./nrf_crypto_error.h
	./nrf_crypto_hash.h
	./nrf_crypto_hash_backend.h
	./nrf_crypto_hash_shared.h
	./nrf_crypto_hkdf.h
	./nrf_crypto_hmac.h
	./nrf_crypto_hmac_backend.h
	./nrf_crypto_hmac_shared.h
	./nrf_crypto_init.h
	./nrf_crypto_mem.h
	./nrf_crypto_rng.h
	./nrf_crypto_rng_backend.h
	./nrf_crypto_rng_shared.h
	./nrf_crypto_shared.h
	./nrf_crypto_types.h
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

