# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "nrf_cc310")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./common/integration_test_plat_utils.c
	# List of all source files found in cortex-m4_gen_files
	# List of all source files found in cortex-m4_hard-float_gen_files
	# List of all source files found in cortex-m4_hard-float_default_gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./common/integration_test_plat_defs.h
	./common/integration_test_ssi_data.h
	./common/integration_test_ssi_defs.h
	./include/ccsw_crys_rsa_shared_types.h
	./include/crys_aesccm.h
	./include/crys_aesccm_error.h
	./include/crys_chacha.h
	./include/crys_chacha_error.h
	./include/crys_chacha_poly.h
	./include/crys_chacha_poly_error.h
	./include/crys_common.h
	./include/crys_common_error.h
	./include/crys_dh.h
	./include/crys_dh_error.h
	./include/crys_dh_kg.h
	./include/crys_ec_edw_api.h
	./include/crys_ec_mont_api.h
	./include/crys_ec_mont_edw_error.h
	./include/crys_ecpki_build.h
	./include/crys_ecpki_dh.h
	./include/crys_ecpki_domain.h
	./include/crys_ecpki_ecdsa.h
	./include/crys_ecpki_error.h
	./include/crys_ecpki_kg.h
	./include/crys_ecpki_types.h
	./include/crys_error.h
	./include/crys_hash.h
	./include/crys_hash_defs.h
	./include/crys_hash_error.h
	./include/crys_hkdf.h
	./include/crys_hkdf_error.h
	./include/crys_hmac.h
	./include/crys_hmac_defs.h
	./include/crys_hmac_error.h
	./include/crys_kdf.h
	./include/crys_kdf_error.h
	./include/crys_pka_defs_hw.h
	./include/crys_poly.h
	./include/crys_poly_error.h
	./include/crys_rnd.h
	./include/crys_rnd_error.h
	./include/crys_rsa_build.h
	./include/crys_rsa_error.h
	./include/crys_rsa_kg.h
	./include/crys_rsa_prim.h
	./include/crys_rsa_schemes.h
	./include/crys_rsa_types.h
	./include/crys_srp.h
	./include/crys_srp_error.h
	./include/dx_reg_base_host.h
	./include/sns_silib.h
	./include/ssi_aes.h
	./include/ssi_aes_defs.h
	./include/ssi_aes_error.h
	./include/ssi_bitops.h
	./include/ssi_pal_abort.h
	./include/ssi_pal_barrier.h
	./include/ssi_pal_compiler.h
	./include/ssi_pal_dma.h
	./include/ssi_pal_dma_defs.h
	./include/ssi_pal_dma_plat.h
	./include/ssi_pal_error.h
	./include/ssi_pal_file.h
	./include/ssi_pal_file_plat.h
	./include/ssi_pal_fips.h
	./include/ssi_pal_init.h
	./include/ssi_pal_list.h
	./include/ssi_pal_log.h
	./include/ssi_pal_mem.h
	./include/ssi_pal_memmap.h
	./include/ssi_pal_mutex.h
	./include/ssi_pal_mutex_plat.h
	./include/ssi_pal_perf.h
	./include/ssi_pal_perf_plat.h
	./include/ssi_pal_sem.h
	./include/ssi_pal_sem_plat.h
	./include/ssi_pal_trng.h
	./include/ssi_pal_types.h
	./include/ssi_pal_types_plat.h
	./include/ssi_pka_hw_plat_defs.h
	./include/ssi_regs.h
	./include/ssi_sram_map.h
	./include/ssi_util_defs.h
	./include/ssi_util_error.h
	./include/ssi_util_key_derivation.h
	./include/ssi_util_key_derivation_defs.h
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
	./common
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

