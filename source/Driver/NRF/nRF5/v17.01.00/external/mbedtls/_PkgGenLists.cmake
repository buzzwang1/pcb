# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "mbedtls")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./library/aes.c
	./library/aesni.c
	./library/arc4.c
	./library/aria.c
	./library/asn1parse.c
	./library/asn1write.c
	./library/base64.c
	./library/bignum.c
	./library/blowfish.c
	./library/camellia.c
	./library/ccm.c
	./library/certs.c
	./library/chacha20.c
	./library/chachapoly.c
	./library/cipher.c
	./library/cipher_wrap.c
	./library/cmac.c
	./library/ctr_drbg.c
	./library/debug.c
	./library/des.c
	./library/dhm.c
	./library/ecdh.c
	./library/ecdsa.c
	./library/ecjpake.c
	./library/ecp.c
	./library/ecp_curves.c
	./library/entropy.c
	./library/entropy_poll.c
	./library/error.c
	./library/gcm.c
	./library/havege.c
	./library/hkdf.c
	./library/hmac_drbg.c
	./library/md.c
	./library/md2.c
	./library/md4.c
	./library/md5.c
	./library/md_wrap.c
	./library/memory_buffer_alloc.c
	./library/net_sockets.c
	./library/nist_kw.c
	./library/oid.c
	./library/padlock.c
	./library/pem.c
	./library/pk.c
	./library/pk_wrap.c
	./library/pkcs11.c
	./library/pkcs12.c
	./library/pkcs5.c
	./library/pkparse.c
	./library/pkwrite.c
	./library/platform.c
	./library/platform_util.c
	./library/poly1305.c
	./library/ripemd160.c
	./library/rsa.c
	./library/rsa_internal.c
	./library/sha1.c
	./library/sha256.c
	./library/sha512.c
	./library/ssl_cache.c
	./library/ssl_ciphersuites.c
	./library/ssl_cli.c
	./library/ssl_cookie.c
	./library/ssl_srv.c
	./library/ssl_ticket.c
	./library/ssl_tls.c
	./library/threading.c
	./library/timing.c
	./library/version.c
	./library/version_features.c
	./library/x509.c
	./library/x509_create.c
	./library/x509_crl.c
	./library/x509_crt.c
	./library/x509_csr.c
	./library/x509write_crt.c
	./library/x509write_csr.c
	./library/xtea.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./include/mbedtls/aes.h
	./include/mbedtls/aesni.h
	./include/mbedtls/arc4.h
	./include/mbedtls/aria.h
	./include/mbedtls/asn1.h
	./include/mbedtls/asn1write.h
	./include/mbedtls/base64.h
	./include/mbedtls/bignum.h
	./include/mbedtls/blowfish.h
	./include/mbedtls/bn_mul.h
	./include/mbedtls/camellia.h
	./include/mbedtls/ccm.h
	./include/mbedtls/certs.h
	./include/mbedtls/chacha20.h
	./include/mbedtls/chachapoly.h
	./include/mbedtls/check_config.h
	./include/mbedtls/cipher.h
	./include/mbedtls/cipher_internal.h
	./include/mbedtls/cmac.h
	./include/mbedtls/compat-1.3.h
	./include/mbedtls/config.h
	./include/mbedtls/ctr_drbg.h
	./include/mbedtls/debug.h
	./include/mbedtls/des.h
	./include/mbedtls/dhm.h
	./include/mbedtls/ecdh.h
	./include/mbedtls/ecdsa.h
	./include/mbedtls/ecjpake.h
	./include/mbedtls/ecp.h
	./include/mbedtls/ecp_internal.h
	./include/mbedtls/entropy.h
	./include/mbedtls/entropy_poll.h
	./include/mbedtls/error.h
	./include/mbedtls/gcm.h
	./include/mbedtls/havege.h
	./include/mbedtls/hkdf.h
	./include/mbedtls/hmac_drbg.h
	./include/mbedtls/md.h
	./include/mbedtls/md2.h
	./include/mbedtls/md4.h
	./include/mbedtls/md5.h
	./include/mbedtls/md_internal.h
	./include/mbedtls/memory_buffer_alloc.h
	./include/mbedtls/net.h
	./include/mbedtls/net_sockets.h
	./include/mbedtls/nist_kw.h
	./include/mbedtls/oid.h
	./include/mbedtls/padlock.h
	./include/mbedtls/pem.h
	./include/mbedtls/pk.h
	./include/mbedtls/pk_internal.h
	./include/mbedtls/pkcs11.h
	./include/mbedtls/pkcs12.h
	./include/mbedtls/pkcs5.h
	./include/mbedtls/platform.h
	./include/mbedtls/platform_time.h
	./include/mbedtls/platform_util.h
	./include/mbedtls/poly1305.h
	./include/mbedtls/ripemd160.h
	./include/mbedtls/rsa.h
	./include/mbedtls/rsa_internal.h
	./include/mbedtls/sha1.h
	./include/mbedtls/sha256.h
	./include/mbedtls/sha512.h
	./include/mbedtls/ssl.h
	./include/mbedtls/ssl_cache.h
	./include/mbedtls/ssl_ciphersuites.h
	./include/mbedtls/ssl_cookie.h
	./include/mbedtls/ssl_internal.h
	./include/mbedtls/ssl_ticket.h
	./include/mbedtls/threading.h
	./include/mbedtls/timing.h
	./include/mbedtls/version.h
	./include/mbedtls/x509.h
	./include/mbedtls/x509_crl.h
	./include/mbedtls/x509_crt.h
	./include/mbedtls/x509_csr.h
	./include/mbedtls/xtea.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./include/mbedtls
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

