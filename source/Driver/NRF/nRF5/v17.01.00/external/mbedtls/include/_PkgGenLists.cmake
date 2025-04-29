# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./mbedtls/aes.h
	./mbedtls/aesni.h
	./mbedtls/arc4.h
	./mbedtls/aria.h
	./mbedtls/asn1.h
	./mbedtls/asn1write.h
	./mbedtls/base64.h
	./mbedtls/bignum.h
	./mbedtls/blowfish.h
	./mbedtls/bn_mul.h
	./mbedtls/camellia.h
	./mbedtls/ccm.h
	./mbedtls/certs.h
	./mbedtls/chacha20.h
	./mbedtls/chachapoly.h
	./mbedtls/check_config.h
	./mbedtls/cipher.h
	./mbedtls/cipher_internal.h
	./mbedtls/cmac.h
	./mbedtls/compat-1.3.h
	./mbedtls/config.h
	./mbedtls/ctr_drbg.h
	./mbedtls/debug.h
	./mbedtls/des.h
	./mbedtls/dhm.h
	./mbedtls/ecdh.h
	./mbedtls/ecdsa.h
	./mbedtls/ecjpake.h
	./mbedtls/ecp.h
	./mbedtls/ecp_internal.h
	./mbedtls/entropy.h
	./mbedtls/entropy_poll.h
	./mbedtls/error.h
	./mbedtls/gcm.h
	./mbedtls/havege.h
	./mbedtls/hkdf.h
	./mbedtls/hmac_drbg.h
	./mbedtls/md.h
	./mbedtls/md2.h
	./mbedtls/md4.h
	./mbedtls/md5.h
	./mbedtls/md_internal.h
	./mbedtls/memory_buffer_alloc.h
	./mbedtls/net.h
	./mbedtls/net_sockets.h
	./mbedtls/nist_kw.h
	./mbedtls/oid.h
	./mbedtls/padlock.h
	./mbedtls/pem.h
	./mbedtls/pk.h
	./mbedtls/pk_internal.h
	./mbedtls/pkcs11.h
	./mbedtls/pkcs12.h
	./mbedtls/pkcs5.h
	./mbedtls/platform.h
	./mbedtls/platform_time.h
	./mbedtls/platform_util.h
	./mbedtls/poly1305.h
	./mbedtls/ripemd160.h
	./mbedtls/rsa.h
	./mbedtls/rsa_internal.h
	./mbedtls/sha1.h
	./mbedtls/sha256.h
	./mbedtls/sha512.h
	./mbedtls/ssl.h
	./mbedtls/ssl_cache.h
	./mbedtls/ssl_ciphersuites.h
	./mbedtls/ssl_cookie.h
	./mbedtls/ssl_internal.h
	./mbedtls/ssl_ticket.h
	./mbedtls/threading.h
	./mbedtls/timing.h
	./mbedtls/version.h
	./mbedtls/x509.h
	./mbedtls/x509_crl.h
	./mbedtls/x509_crt.h
	./mbedtls/x509_csr.h
	./mbedtls/xtea.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./mbedtls
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

