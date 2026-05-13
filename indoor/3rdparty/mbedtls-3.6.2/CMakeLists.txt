# add_compile_options(-w)

add_definitions(
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)

FILE(GLOB MBEDTLS_SRC
    library/md5.c
    library/error.c
    library/sha1.c
    library/platform_util.c
    library/md.c
    library/psa_util.c
    library/asn1parse.c
    library/bignum.c
    library/bignum_core.c
    library/constant_time.c
    library/asn1write.c
    library/psa_crypto.c
    library/psa_crypto_slot_management.c
    library/psa_crypto_storage.c
    library/psa_its_file.c
    library/psa_crypto_driver_wrappers_no_static.c
    library/psa_crypto_ffdh.c
    library/psa_crypto_ecp.c
    library/ecp.c
    library/ecp_curves.c
    library/ecdh.c
    library/ecdsa.c
    library/hmac_drbg.c
    library/psa_crypto_rsa.c
    library/rsa.c
    library/oid.c
    library/rsa_alt_helpers.c
    library/entropy.c
    library/entropy_poll.c
    library/des.c
    library/psa_crypto_client.c
    library/ctr_drbg.c
    library/aes.c
    library/aesni.c
    library/psa_crypto_pake.c
    library/ecjpake.c
    library/psa_crypto_mac.c
    library/cmac.c
    library/cipher.c
    library/nist_kw.c
    library/chachapoly.c
    library/poly1305.c
    library/chacha20.c
    library/ccm.c
    library/gcm.c
    library/cipher_wrap.c
    library/aria.c
    library/camellia.c
    library/psa_crypto_cipher.c
    library/psa_crypto_aead.c
    library/psa_crypto_hash.c
    library/sha3.c
    library/sha512.c
    library/sha256.c
    library/ripemd160.c
)

add_library(mbedtls STATIC ${MBEDTLS_SRC})