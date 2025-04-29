# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "nrf_oberon")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	# List of all source files found in cortex-m4_gen_files
	# List of all source files found in cortex-m4_hard-float_gen_files
	# List of all source files found in cortex-m4_hard-float_default_gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./include/mbedtls/chacha20_alt.h
	./include/mbedtls/ecjpake_alt.h
	./include/mbedtls/poly1305_alt.h
	./include/mbedtls/sha1_alt.h
	./include/mbedtls/sha256_alt.h
	./include/ocrypto_aes_cbc.h
	./include/ocrypto_aes_ccm.h
	./include/ocrypto_aes_cmac.h
	./include/ocrypto_aes_ctr.h
	./include/ocrypto_aes_eax.h
	./include/ocrypto_aes_gcm.h
	./include/ocrypto_aes_key.h
	./include/ocrypto_chacha20.h
	./include/ocrypto_chacha20_poly1305.h
	./include/ocrypto_chacha20_poly1305_inc.h
	./include/ocrypto_constant_time.h
	./include/ocrypto_curve25519.h
	./include/ocrypto_curve_p224.h
	./include/ocrypto_curve_p256.h
	./include/ocrypto_ecdh_p224.h
	./include/ocrypto_ecdh_p256.h
	./include/ocrypto_ecdsa_p224.h
	./include/ocrypto_ecdsa_p256.h
	./include/ocrypto_ecjpake_p256.h
	./include/ocrypto_ed25519.h
	./include/ocrypto_hkdf_sha1.h
	./include/ocrypto_hkdf_sha256.h
	./include/ocrypto_hkdf_sha512.h
	./include/ocrypto_hmac_sha1.h
	./include/ocrypto_hmac_sha256.h
	./include/ocrypto_hmac_sha512.h
	./include/ocrypto_poly1305.h
	./include/ocrypto_rsa.h
	./include/ocrypto_rsa_key.h
	./include/ocrypto_sc_p224.h
	./include/ocrypto_sc_p256.h
	./include/ocrypto_sha1.h
	./include/ocrypto_sha224.h
	./include/ocrypto_sha256.h
	./include/ocrypto_sha384.h
	./include/ocrypto_sha512.h
	./include/ocrypto_srp.h
	./include/ocrypto_srtp.h
	# List of all source files found in cortex-m4_gen_headers
	# List of all source files found in cortex-m4_hard-float_gen_headers
	# List of all source files found in cortex-m4_hard-float_default_gen_headers
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in cortex-m4_gen_asmfiles
	# List of all source files found in cortex-m4_hard-float_gen_asmfiles
	# List of all source files found in cortex-m4_hard-float_default_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./include/mbedtls
	./include
	# List of all source files found in cortex-m4_gen_include_directories
	# List of all source files found in cortex-m4_hard-float_gen_include_directories
	# List of all source files found in cortex-m4_hard-float_default_gen_include_directories
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

