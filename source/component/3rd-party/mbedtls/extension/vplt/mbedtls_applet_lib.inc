#include <mbedtls/version.h>
#include <mbedtls/error.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/certs.h>
#include <mbedtls/md5.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/aes.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/debug.h>

#include "./mbedtls_vplt.h"

#ifndef __VSF_APPLET__
#	warning this file should be used in applets.
#else

static vsf_mbedtls_vplt_t * __mbedtls_get_vplt(void)
{
    static vsf_mbedtls_vplt_t *__mbedtls_vplt = NULL;
    if (NULL == __mbedtls_vplt) {
        extern void * vsf_vplt_link(void *vplt, char *symname);
        __mbedtls_vplt = vsf_vplt_link(NULL, "mbedtls");
    }
    return __mbedtls_vplt;
}

#define VSF_APPLET_MBEDTLS_VPLT                                                 \
            ((vsf_mbedtls_vplt_t *)__mbedtls_get_vplt())
#define VSF_APPLET_MBEDTLS_ENTRY(__NAME)                                        \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_APPLET_MBEDTLS_VPLT, __NAME)
#define VSF_APPLET_MBEDTLS_IMP(...)                                             \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_APPLET_MBEDTLS_VPLT, __VA_ARGS__)

// <mbedtls/version.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_number, unsigned int, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_number)();
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_string, void, char *string) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_string)(string);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_get_string_full, void, char *string) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_get_string_full)(string);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_version_check_feature, int, const char *feature) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_version_check_feature)(feature);
}

// <mbedtls/error.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_strerror, void, int errnum, char *buffer, size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_strerror)(errnum, buffer, buflen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_high_level_strerr, const char *, int error_code) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_high_level_strerr)(error_code);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_low_level_strerr, const char *, int error_code) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_low_level_strerr)(error_code);
}

// <mbedtls/debug.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_set_threshold, void, int threshold) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_set_threshold)(threshold);
}
//VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_msg, void, const mbedtls_ssl_context *ssl, int level,
//                             const char *file, int line,
//                             const char *format, ...) {
//    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
//    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_msg)(ssl, level, file, line, format, ...);
//}
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_ret, void, const mbedtls_ssl_context *ssl, int level,
                             const char *file, int line,
                             const char *text, int ret) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_ret)(ssl, level, file, line, text, ret);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_buf, void, const mbedtls_ssl_context *ssl, int level,
                             const char *file, int line, const char *text,
                             const unsigned char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_buf)(ssl, level, file, line, text, buf, len);
}
#if defined(MBEDTLS_BIGNUM_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_mpi, void, const mbedtls_ssl_context *ssl, int level,
                             const char *file, int line,
                             const char *text, const mbedtls_mpi *X) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_mpi)(ssl, level, file, line, text, X);
}
#endif
#if defined(MBEDTLS_ECP_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_ecp, void, const mbedtls_ssl_context *ssl, int level,
                             const char *file, int line,
                             const char *text, const mbedtls_ecp_point *X) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_ecp)(ssl, level, file, line, text, X);
}
#endif
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_print_crt, void, const mbedtls_ssl_context *ssl, int level,
                             const char *file, int line,
                             const char *text, const mbedtls_x509_crt *crt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_print_crt)(ssl, level, file, line, text, crt);
}
#endif
#if defined(MBEDTLS_ECDH_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_debug_printf_ecdh, void, const mbedtls_ssl_context *ssl, int level,
                               const char *file, int line,
                               const mbedtls_ecdh_context *ecdh,
                               mbedtls_debug_ecdh_attr attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_debug_printf_ecdh)(ssl, level, file, line, ecdh, attr);
}
#endif

// <mbedtls/md5.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_init, void, mbedtls_md5_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_free, void, mbedtls_md5_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_clone, void, mbedtls_md5_context *dst,
                       const mbedtls_md5_context *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_clone)(dst, src);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_starts_ret, int, mbedtls_md5_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_starts_ret)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_update_ret, int, mbedtls_md5_context *ctx,
                           const unsigned char *input,
                           size_t ilen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_update_ret)(ctx, input, ilen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_finish_ret, int, mbedtls_md5_context *ctx,
                           unsigned char output[16]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_finish_ret)(ctx, output);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_internal_md5_process, int, mbedtls_md5_context *ctx,
                                 const unsigned char data[64]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_internal_md5_process)(ctx, data);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_ret, int, const unsigned char *input,
                    size_t ilen,
                    unsigned char output[16]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_ret)(input, ilen, output);
}
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_md5_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_md5_self_test)(verbose);
}
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/sha1.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_init, void, mbedtls_sha1_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_free, void, mbedtls_sha1_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_clone, void, mbedtls_sha1_context *dst,
                        const mbedtls_sha1_context *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_clone)(dst, src);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_starts_ret, int, mbedtls_sha1_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_starts_ret)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_update_ret, int, mbedtls_sha1_context *ctx,
                            const unsigned char *input,
                            size_t ilen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_update_ret)(ctx, input, ilen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_finish_ret, int, mbedtls_sha1_context *ctx,
                            unsigned char output[20]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_finish_ret)(ctx, output);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_internal_sha1_process, int, mbedtls_sha1_context *ctx,
                                  const unsigned char data[64]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_internal_sha1_process)(ctx, data);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_ret, int, const unsigned char *input,
                     size_t ilen,
                     unsigned char output[20]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_ret)(input, ilen, output);
}
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha1_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha1_self_test)(verbose);
}
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/sha256.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_init, void, mbedtls_sha256_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_free, void, mbedtls_sha256_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_clone, void, mbedtls_sha256_context *dst,
                          const mbedtls_sha256_context *src) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_clone)(dst, src);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_starts_ret, int, mbedtls_sha256_context *ctx, int is224) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_starts_ret)(ctx, is224);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_update_ret, int, mbedtls_sha256_context *ctx,
                              const unsigned char *input,
                              size_t ilen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_update_ret)(ctx, input, ilen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_finish_ret, int, mbedtls_sha256_context *ctx,
                              unsigned char output[32]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_finish_ret)(ctx, output);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_internal_sha256_process, int, mbedtls_sha256_context *ctx,
                                    const unsigned char data[64]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_internal_sha256_process)(ctx, data);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_ret, int, const unsigned char *input,
                       size_t ilen,
                       unsigned char output[32],
                       int is224) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_ret)(input, ilen, output, is224);
}
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_sha256_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_sha256_self_test)(verbose);
}
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/aes.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_init, void, mbedtls_aes_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_free, void, mbedtls_aes_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_free)(ctx);
}
#if defined(MBEDTLS_CIPHER_MODE_XTS)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_xts_init, void, mbedtls_aes_xts_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_xts_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_xts_free, void, mbedtls_aes_xts_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_xts_free)(ctx);
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_setkey_enc, int, mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_setkey_enc)(ctx, key, keybits);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_setkey_dec, int, mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_setkey_dec)(ctx, key, keybits);
}
#if defined(MBEDTLS_CIPHER_MODE_XTS)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_xts_setkey_enc, int, mbedtls_aes_xts_context *ctx,
                               const unsigned char *key,
                               unsigned int keybits) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_xts_setkey_enc)(ctx, key, keybits);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_xts_setkey_dec, int, mbedtls_aes_xts_context *ctx,
                               const unsigned char *key,
                               unsigned int keybits) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_xts_setkey_dec)(ctx, key, keybits);
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_ecb, int, mbedtls_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_ecb)(ctx, mode, input, output);
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_cbc, int, mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_cbc)(ctx, mode, length, iv, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_CIPHER_MODE_XTS)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_xts, int, mbedtls_aes_xts_context *ctx,
                          int mode,
                          size_t length,
                          const unsigned char data_unit[16],
                          const unsigned char *input,
                          unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_xts)(ctx, mode, length, data_unit, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */
#if defined(MBEDTLS_CIPHER_MODE_CFB)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_cfb128, int, mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_cfb128)(ctx, mode, length, iv_off, iv, input, output);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_cfb8, int, mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_cfb8)(ctx, mode, length, iv, input, output);
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */
#if defined(MBEDTLS_CIPHER_MODE_OFB)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_ofb, int, mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_ofb)(ctx, length, iv_off, iv, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */
#if defined(MBEDTLS_CIPHER_MODE_CTR)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_crypt_ctr, int, mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_crypt_ctr)(ctx, length, nc_off, nonce_counter, stream_block, input, output);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */
VSF_APPLET_MBEDTLS_IMP(mbedtls_internal_aes_encrypt, int, mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_internal_aes_encrypt)(ctx, input, output);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_internal_aes_decrypt, int, mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_internal_aes_decrypt)(ctx, input, output);
}
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_aes_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_aes_self_test)(verbose);
}
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/pk.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_info_from_type, const mbedtls_pk_info_t *, mbedtls_pk_type_t pk_type)  {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_info_from_type)(pk_type);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_init, void, mbedtls_pk_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_free, void, mbedtls_pk_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_free)(ctx);
}
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_restart_init, void, mbedtls_pk_restart_ctx *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_restart_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_restart_free, void, mbedtls_pk_restart_ctx *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_restart_free)(ctx);
}
#endif /* MBEDTLS_ECDSA_C && MBEDTLS_ECP_RESTARTABLE */
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_setup, int, mbedtls_pk_context *ctx, const mbedtls_pk_info_t *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_setup)(ctx, info);
}
#if defined(MBEDTLS_USE_PSA_CRYPTO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_setup_opaque, int, mbedtls_pk_context *ctx,
                            const psa_key_id_t key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_setup_opaque)(ctx, key);
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_setup_rsa_alt, int, mbedtls_pk_context *ctx, void *key,
                             mbedtls_pk_rsa_alt_decrypt_func decrypt_func,
                             mbedtls_pk_rsa_alt_sign_func sign_func,
                             mbedtls_pk_rsa_alt_key_len_func key_len_func) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_setup_rsa_alt)(ctx, key, decrypt_func, sign_func, key_len_func);
}
#endif /* MBEDTLS_PK_RSA_ALT_SUPPORT */
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_get_bitlen, size_t, const mbedtls_pk_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_get_bitlen)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_can_do, int, const mbedtls_pk_context *ctx, mbedtls_pk_type_t type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_can_do)(ctx, type);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_verify, int, mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg,
                      const unsigned char *hash, size_t hash_len,
                      const unsigned char *sig, size_t sig_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_verify)(ctx, md_alg, hash, hash_len, sig, sig_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_verify_restartable, int, mbedtls_pk_context *ctx,
                                  mbedtls_md_type_t md_alg,
                                  const unsigned char *hash, size_t hash_len,
                                  const unsigned char *sig, size_t sig_len,
                                  mbedtls_pk_restart_ctx *rs_ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_verify_restartable)(ctx, md_alg, hash, hash_len, sig, sig_len, rs_ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_verify_ext, int, mbedtls_pk_type_t type, const void *options,
                          mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg,
                          const unsigned char *hash, size_t hash_len,
                          const unsigned char *sig, size_t sig_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_verify_ext)(type, options, ctx, md_alg, hash, hash_len, sig, sig_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_sign, int, mbedtls_pk_context *ctx, mbedtls_md_type_t md_alg,
                    const unsigned char *hash, size_t hash_len,
                    unsigned char *sig, size_t *sig_len,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_sign)(ctx, md_alg, hash, hash_len, sig, sig_len, f_rng, p_rng);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_sign_restartable, int, mbedtls_pk_context *ctx,
                                mbedtls_md_type_t md_alg,
                                const unsigned char *hash, size_t hash_len,
                                unsigned char *sig, size_t *sig_len,
                                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                mbedtls_pk_restart_ctx *rs_ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_sign_restartable)(ctx, md_alg, hash, hash_len, sig, sig_len, f_rng, p_rng, rs_ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_decrypt, int, mbedtls_pk_context *ctx,
                       const unsigned char *input, size_t ilen,
                       unsigned char *output, size_t *olen, size_t osize,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_decrypt)(ctx, input, ilen, output, olen, osize, f_rng, p_rng);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_encrypt, int, mbedtls_pk_context *ctx,
                       const unsigned char *input, size_t ilen,
                       unsigned char *output, size_t *olen, size_t osize,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_encrypt)(ctx, input, ilen, output, olen, osize, f_rng, p_rng);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_check_pair, int, const mbedtls_pk_context *pub, const mbedtls_pk_context *prv) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_check_pair)(pub, prv);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_debug, int, const mbedtls_pk_context *ctx, mbedtls_pk_debug_item *items) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_debug)(ctx, items);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_get_name, const char *, const mbedtls_pk_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_get_name)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_get_type, mbedtls_pk_type_t, const mbedtls_pk_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_get_type)(ctx);
}
#if defined(MBEDTLS_PK_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_parse_key, int, mbedtls_pk_context *ctx,
                         const unsigned char *key, size_t keylen,
                         const unsigned char *pwd, size_t pwdlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_parse_key)(ctx, key, keylen, pwd, pwdlen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_parse_public_key, int, mbedtls_pk_context *ctx,
                                const unsigned char *key, size_t keylen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_parse_public_key)(ctx, key, keylen);
}
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_parse_keyfile, int, mbedtls_pk_context *ctx,
                             const char *path, const char *password) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_parse_keyfile)(ctx, path, password);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_parse_public_keyfile, int, mbedtls_pk_context *ctx, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_parse_public_keyfile)(ctx, path);
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_PK_PARSE_C */
#if defined(MBEDTLS_PK_WRITE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_write_key_der, int, mbedtls_pk_context *ctx, unsigned char *buf, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_write_key_der)(ctx, buf, size);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_write_pubkey_der, int, mbedtls_pk_context *ctx, unsigned char *buf, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_write_pubkey_der)(ctx, buf, size);
}
#if defined(MBEDTLS_PEM_WRITE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_write_pubkey_pem, int, mbedtls_pk_context *ctx, unsigned char *buf, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_write_pubkey_pem)(ctx, buf, size);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_write_key_pem, int, mbedtls_pk_context *ctx, unsigned char *buf, size_t size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_write_key_pem)(ctx, buf, size);
}
#endif /* MBEDTLS_PEM_WRITE_C */
#endif /* MBEDTLS_PK_WRITE_C */
#if defined(MBEDTLS_PK_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_parse_subpubkey, int, unsigned char **p, const unsigned char *end,
                               mbedtls_pk_context *pk) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_parse_subpubkey)(p, end, pk);
}
#endif /* MBEDTLS_PK_PARSE_C */
#if defined(MBEDTLS_PK_WRITE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_write_pubkey, int, unsigned char **p, unsigned char *start,
                            const mbedtls_pk_context *key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_write_pubkey)(p, start, key);
}
#endif /* MBEDTLS_PK_WRITE_C */
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_load_file, int, const char *path, unsigned char **buf, size_t *n) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_load_file)(path, buf, n);
}
#endif
#if defined(MBEDTLS_USE_PSA_CRYPTO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_pk_wrap_as_opaque, int, mbedtls_pk_context *pk,
                              psa_key_id_t *key,
                              psa_algorithm_t hash_alg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_pk_wrap_as_opaque)(pk, key, hash_alg);
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */

// <mbedtls/x509_crt.h>

#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse_der, int, mbedtls_x509_crt *chain,
                               const unsigned char *buf,
                               size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse_der)(chain, buf, buflen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse_der_with_ext_cb, int, mbedtls_x509_crt *chain,
                                           const unsigned char *buf,
                                           size_t buflen,
                                           int make_copy,
                                           mbedtls_x509_crt_ext_cb_t cb,
                                           void *p_ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse_der_with_ext_cb)(chain, buf, buflen, make_copy, cb, p_ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse_der_nocopy, int, mbedtls_x509_crt *chain,
                                      const unsigned char *buf,
                                      size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse_der_nocopy)(chain, buf, buflen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse, int, mbedtls_x509_crt *chain, const unsigned char *buf, size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse)(chain, buf, buflen);
}
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse_file, int, mbedtls_x509_crt *chain, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse_file)(chain, path);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_parse_path, int, mbedtls_x509_crt *chain, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_parse_path)(chain, path);
}
#endif /* MBEDTLS_FS_IO */
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_parse_subject_alt_name, int, const mbedtls_x509_buf *san_buf,
                                        mbedtls_x509_subject_alternative_name *san) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_parse_subject_alt_name)(san_buf, san);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_info, int, char *buf, size_t size, const char *prefix,
                          const mbedtls_x509_crt *crt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_info)(buf, size, prefix, crt);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_verify_info, int, char *buf, size_t size, const char *prefix,
                                 uint32_t flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_verify_info)(buf, size, prefix, flags);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_verify, int, mbedtls_x509_crt *crt,
                            mbedtls_x509_crt *trust_ca,
                            mbedtls_x509_crl *ca_crl,
                            const char *cn, uint32_t *flags,
                            int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                            void *p_vrfy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_verify)(crt, trust_ca, ca_crl, cn, flags, f_vrfy, p_vrfy);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_verify_with_profile, int, mbedtls_x509_crt *crt,
                                         mbedtls_x509_crt *trust_ca,
                                         mbedtls_x509_crl *ca_crl,
                                         const mbedtls_x509_crt_profile *profile,
                                         const char *cn, uint32_t *flags,
                                         int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                                         void *p_vrfy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_verify_with_profile)(crt, trust_ca, ca_crl, profile, cn, flags, f_vrfy, p_vrfy);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_verify_restartable, int, mbedtls_x509_crt *crt,
                                        mbedtls_x509_crt *trust_ca,
                                        mbedtls_x509_crl *ca_crl,
                                        const mbedtls_x509_crt_profile *profile,
                                        const char *cn, uint32_t *flags,
                                        int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                                        void *p_vrfy,
                                        mbedtls_x509_crt_restart_ctx *rs_ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_verify_restartable)(crt, trust_ca, ca_crl, profile, cn, flags, f_vrfy, p_vrfy, rs_ctx);
}
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_verify_with_ca_cb, int, mbedtls_x509_crt *crt,
                                       mbedtls_x509_crt_ca_cb_t f_ca_cb,
                                       void *p_ca_cb,
                                       const mbedtls_x509_crt_profile *profile,
                                       const char *cn, uint32_t *flags,
                                       int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                                       void *p_vrfy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_verify_with_ca_cb)(crt, f_ca_cb, p_ca_cb, profile, cn, flags, f_vrfy, p_vrfy);
}
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */
#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_check_key_usage, int, const mbedtls_x509_crt *crt,
                                     unsigned int usage) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_check_key_usage)(crt, usage);
}
#endif /* MBEDTLS_X509_CHECK_KEY_USAGE) */
#if defined(MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_check_extended_key_usage, int, const mbedtls_x509_crt *crt,
                                              const char *usage_oid,
                                              size_t usage_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_check_extended_key_usage)(crt, usage_oid, usage_len);
}
#endif /* MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE */
#if defined(MBEDTLS_X509_CRL_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_is_revoked, int, const mbedtls_x509_crt *crt, const mbedtls_x509_crl *crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_is_revoked)(crt, crl);
}
#endif /* MBEDTLS_X509_CRL_PARSE_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_init, void, mbedtls_x509_crt *crt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_init)(crt);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_free, void, mbedtls_x509_crt *crt) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_free)(crt);
}
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_restart_init, void, mbedtls_x509_crt_restart_ctx *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_restart_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crt_restart_free, void, mbedtls_x509_crt_restart_ctx *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crt_restart_free)(ctx);
}
#endif /* MBEDTLS_ECDSA_C && MBEDTLS_ECP_RESTARTABLE */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_WRITE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_init, void, mbedtls_x509write_cert *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_version, void, mbedtls_x509write_cert *ctx, int version) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_version)(ctx, version);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_serial, int, mbedtls_x509write_cert *ctx, const mbedtls_mpi *serial) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_serial)(ctx, serial);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_validity, int, mbedtls_x509write_cert *ctx, const char *not_before,
                                       const char *not_after) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_validity)(ctx, not_before, not_after);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_issuer_name, int, mbedtls_x509write_cert *ctx,
                                          const char *issuer_name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_issuer_name)(ctx, issuer_name);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_subject_name, int, mbedtls_x509write_cert *ctx,
                                           const char *subject_name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_subject_name)(ctx, subject_name);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_subject_key, void, mbedtls_x509write_cert *ctx, mbedtls_pk_context *key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_subject_key)(ctx, key);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_issuer_key, void, mbedtls_x509write_cert *ctx, mbedtls_pk_context *key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_issuer_key)(ctx, key);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_md_alg, void, mbedtls_x509write_cert *ctx, mbedtls_md_type_t md_alg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_md_alg)(ctx, md_alg);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_extension, int, mbedtls_x509write_cert *ctx,
                                        const char *oid, size_t oid_len,
                                        int critical,
                                        const unsigned char *val, size_t val_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_extension)(ctx, oid, oid_len, critical, val, val_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_basic_constraints, int, mbedtls_x509write_cert *ctx,
                                                int is_ca, int max_pathlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_basic_constraints)(ctx, is_ca, max_pathlen);
}
#if defined(MBEDTLS_SHA1_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_subject_key_identifier, int, mbedtls_x509write_cert *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_subject_key_identifier)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_authority_key_identifier, int, mbedtls_x509write_cert *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_authority_key_identifier)(ctx);
}
#endif /* MBEDTLS_SHA1_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_key_usage, int, mbedtls_x509write_cert *ctx,
                                        unsigned int key_usage) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_key_usage)(ctx, key_usage);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_set_ns_cert_type, int, mbedtls_x509write_cert *ctx,
                                           unsigned char ns_cert_type) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_set_ns_cert_type)(ctx, ns_cert_type);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_free, void, mbedtls_x509write_cert *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_der, int, mbedtls_x509write_cert *ctx, unsigned char *buf, size_t size,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_der)(ctx, buf, size, f_rng, p_rng);
}
#if defined(MBEDTLS_PEM_WRITE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509write_crt_pem, int, mbedtls_x509write_cert *ctx, unsigned char *buf, size_t size,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509write_crt_pem)(ctx, buf, size, f_rng, p_rng);
}
#endif /* MBEDTLS_PEM_WRITE_C */
#endif /* MBEDTLS_X509_CRT_WRITE_C */

// <mbedtls/x509_crl.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_parse_der, int, mbedtls_x509_crl *chain,
                               const unsigned char *buf, size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_parse_der)(chain, buf, buflen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_parse, int, mbedtls_x509_crl *chain, const unsigned char *buf, size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_parse)(chain, buf, buflen);
}
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_parse_file, int, mbedtls_x509_crl *chain, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_parse_file)(chain, path);
}
#endif /* MBEDTLS_FS_IO */
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_info, int, char *buf, size_t size, const char *prefix,
                          const mbedtls_x509_crl *crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_info)(buf, size, prefix, crl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_init, void, mbedtls_x509_crl *crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_init)(crl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_x509_crl_free, void, mbedtls_x509_crl *crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_x509_crl_free)(crl);
}

// <mbedtls/ctr_drgb.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_init, void, mbedtls_ctr_drbg_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_seed, int, mbedtls_ctr_drbg_context *ctx,
                          int (*f_entropy)(void *, unsigned char *, size_t),
                          void *p_entropy,
                          const unsigned char *custom,
                          size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_seed)(ctx, f_entropy, p_entropy, custom, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_free, void, mbedtls_ctr_drbg_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_set_prediction_resistance, void, mbedtls_ctr_drbg_context *ctx,
                                                int resistance) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_set_prediction_resistance)(ctx, resistance);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_set_entropy_len, void, mbedtls_ctr_drbg_context *ctx,
                                      size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_set_entropy_len)(ctx, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_set_nonce_len, int, mbedtls_ctr_drbg_context *ctx,
                                   size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_set_nonce_len)(ctx, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_set_reseed_interval, void, mbedtls_ctr_drbg_context *ctx,
                                          int interval) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_set_reseed_interval)(ctx, interval);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_reseed, int, mbedtls_ctr_drbg_context *ctx,
                            const unsigned char *additional, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_reseed)(ctx, additional, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_update_ret, int, mbedtls_ctr_drbg_context *ctx,
                                const unsigned char *additional,
                                size_t add_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_update_ret)(ctx, additional, add_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_random_with_add, int, void *p_rng,
                                     unsigned char *output, size_t output_len,
                                     const unsigned char *additional, size_t add_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_random_with_add)(p_rng, output, output_len, additional, add_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_random, int, void *p_rng,
                            unsigned char *output, size_t output_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_random)(p_rng, output, output_len);
}
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_write_seed_file, int, mbedtls_ctr_drbg_context *ctx, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_write_seed_file)(ctx, path);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_update_seed_file, int, mbedtls_ctr_drbg_context *ctx, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_update_seed_file)(ctx, path);
}
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ctr_drbg_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ctr_drbg_self_test)(verbose);
}
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/ssl.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_ciphersuite_name, const char *, const int ciphersuite_id) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_ciphersuite_name)(ciphersuite_id);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_ciphersuite_id, int, const char *ciphersuite_name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_ciphersuite_id)(ciphersuite_name);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_init, void, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_init)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_setup, int, mbedtls_ssl_context *ssl,
                      const mbedtls_ssl_config *conf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_setup)(ssl, conf);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_session_reset, int, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_session_reset)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_endpoint, void, mbedtls_ssl_config *conf, int endpoint) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_endpoint)(conf, endpoint);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_transport, void, mbedtls_ssl_config *conf, int transport) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_transport)(conf, transport);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_authmode, void, mbedtls_ssl_config *conf, int authmode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_authmode)(conf, authmode);
}
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_verify, void, mbedtls_ssl_config *conf,
                             int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                             void *p_vrfy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_verify)(conf, f_vrfy, p_vrfy);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_rng, void, mbedtls_ssl_config *conf,
                          int (*f_rng)(void *, unsigned char *, size_t),
                          void *p_rng) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_rng)(conf, f_rng, p_rng);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dbg, void, mbedtls_ssl_config *conf,
                          void (*f_dbg)(void *, int, const char *, int, const char *),
                          void  *p_dbg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dbg)(conf, f_dbg, p_dbg);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_bio, void, mbedtls_ssl_context *ssl,
                         void *p_bio,
                         mbedtls_ssl_send_t *f_send,
                         mbedtls_ssl_recv_t *f_recv,
                         mbedtls_ssl_recv_timeout_t *f_recv_timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_bio)(ssl, p_bio, f_send, f_recv, f_recv_timeout);
}
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_cid, int, mbedtls_ssl_context *ssl,
                        int enable,
                        unsigned char const *own_cid,
                        size_t own_cid_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_cid)(ssl, enable, own_cid, own_cid_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_peer_cid, int, mbedtls_ssl_context *ssl,
                             int *enabled,
                             unsigned char peer_cid[MBEDTLS_SSL_CID_OUT_LEN_MAX],
                             size_t *peer_cid_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_peer_cid)(ssl, enabled, peer_cid, peer_cid_len);
}
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_mtu, void, mbedtls_ssl_context *ssl, uint16_t mtu) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_mtu)(ssl, mtu);
}
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_verify, void, mbedtls_ssl_context *ssl,
                            int (*f_vrfy)(void *, mbedtls_x509_crt *, int, uint32_t *),
                            void *p_vrfy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_verify)(ssl, f_vrfy, p_vrfy);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_read_timeout, void, mbedtls_ssl_config *conf, uint32_t timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_read_timeout)(conf, timeout);
}
#if defined(MBEDTLS_SSL_RECORD_CHECKING)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_check_record, int, mbedtls_ssl_context const *ssl,
                             unsigned char *buf,
                             size_t buflen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_check_record)(ssl, buf, buflen);
}
#endif /* MBEDTLS_SSL_RECORD_CHECKING */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_timer_cb, void, mbedtls_ssl_context *ssl,
                              void *p_timer,
                              mbedtls_ssl_set_timer_t *f_set_timer,
                              mbedtls_ssl_get_timer_t *f_get_timer) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_timer_cb)(ssl, p_timer, f_set_timer, f_get_timer);
}
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_SRV_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_session_tickets_cb, void, mbedtls_ssl_config *conf,
                                         mbedtls_ssl_ticket_write_t *f_ticket_write,
                                         mbedtls_ssl_ticket_parse_t *f_ticket_parse,
                                         void *p_ticket) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_session_tickets_cb)(conf, f_ticket_write, f_ticket_parse, p_ticket);
}
#endif /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_SRV_C */
#if defined(MBEDTLS_SSL_EXPORT_KEYS)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_export_keys_cb, void, mbedtls_ssl_config *conf,
                                     mbedtls_ssl_export_keys_t *f_export_keys,
                                     void *p_export_keys) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_export_keys_cb)(conf, f_export_keys, p_export_keys);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_export_keys_ext_cb, void, mbedtls_ssl_config *conf,
                                         mbedtls_ssl_export_keys_ext_t *f_export_keys_ext,
                                         void *p_export_keys) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_export_keys_ext_cb)(conf, f_export_keys_ext, p_export_keys);
}
#endif /* MBEDTLS_SSL_EXPORT_KEYS */
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_async_private_cb, void, mbedtls_ssl_config *conf,
                                       mbedtls_ssl_async_sign_t *f_async_sign,
                                       mbedtls_ssl_async_decrypt_t *f_async_decrypt,
                                       mbedtls_ssl_async_resume_t *f_async_resume,
                                       mbedtls_ssl_async_cancel_t *f_async_cancel,
                                       void *config_data) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_async_private_cb)(conf, f_async_sign, f_async_decrypt, f_async_resume, f_async_cancel, config_data);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_get_async_config_data, void *, const mbedtls_ssl_config *conf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_get_async_config_data)(conf);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_async_operation_data, void *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_async_operation_data)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_async_operation_data, void, mbedtls_ssl_context *ssl
                                          void *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_async_operation_data)(ssl, ctx);
}
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY) && defined(MBEDTLS_SSL_SRV_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dtls_cookies, void, mbedtls_ssl_config *conf,
                                   mbedtls_ssl_cookie_write_t *f_cookie_write,
                                   mbedtls_ssl_cookie_check_t *f_cookie_check,
                                   void *p_cookie) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dtls_cookies)(conf, f_cookie_write, f_cookie_check, p_cookie);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_client_transport_id, int, mbedtls_ssl_context *ssl,
                                        const unsigned char *info,
                                        size_t ilen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_client_transport_id)(ssl, info, ilen);
}
#endif /* MBEDTLS_SSL_DTLS_HELLO_VERIFY && MBEDTLS_SSL_SRV_C */
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dtls_anti_replay, void, mbedtls_ssl_config *conf, char mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dtls_anti_replay)(conf, mode);
}
#endif /* MBEDTLS_SSL_DTLS_ANTI_REPLAY */
#if defined(MBEDTLS_SSL_DTLS_BADMAC_LIMIT)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dtls_badmac_limit, void, mbedtls_ssl_config *conf, unsigned limit) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dtls_badmac_limit)(conf, limit);
}
#endif /* MBEDTLS_SSL_DTLS_BADMAC_LIMIT */
#if defined(MBEDTLS_SSL_PROTO_DTLS)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_datagram_packing, void, mbedtls_ssl_context *ssl,
                                      unsigned allow_packing) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_datagram_packing)(ssl, allow_packing);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_handshake_timeout, void, mbedtls_ssl_config *conf, uint32_t min, uint32_t max) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_handshake_timeout)(conf, min, max);
}
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#if defined(MBEDTLS_SSL_SRV_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_session_cache, void, mbedtls_ssl_config *conf,
                                    void *p_cache,
                                    int (*f_get_cache)(void *, mbedtls_ssl_session *),
                                    int (*f_set_cache)(void *, const mbedtls_ssl_session *)) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_session_cache)(conf, p_cache, f_get_cache, f_set_cache);
}
#endif /* MBEDTLS_SSL_SRV_C */
#if defined(MBEDTLS_SSL_CLI_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_session, int, mbedtls_ssl_context *ssl, const mbedtls_ssl_session *session) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_session)(ssl, session);
}
#endif /* MBEDTLS_SSL_CLI_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_session_load, int, mbedtls_ssl_session *session,
                             const unsigned char *buf,
                             size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_session_load)(session, buf, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_session_save, int, const mbedtls_ssl_session *session,
                             unsigned char *buf,
                             size_t buf_len,
                             size_t *olen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_session_save)(session, buf, buf_len, olen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_session_pointer, const mbedtls_ssl_session *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_session_pointer)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_ciphersuites, void, mbedtls_ssl_config *conf,
                                   const int *ciphersuites) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_ciphersuites)(conf, ciphersuites);
}
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_cid, int, mbedtls_ssl_config *conf, size_t len,
                         int ignore_other_cids) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_cid)(conf, len, ignore_other_cids);
}
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_ciphersuites_for_version, void, mbedtls_ssl_config *conf,
                                               const int *ciphersuites,
                                               int major, int minor) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_ciphersuites_for_version)(conf, ciphersuites, major, minor);
}
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_cert_profile, void, mbedtls_ssl_config *conf,
                                   const mbedtls_x509_crt_profile *profile) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_cert_profile)(conf, profile);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_ca_chain, void, mbedtls_ssl_config *conf,
                               mbedtls_x509_crt *ca_chain,
                               mbedtls_x509_crl *ca_crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_ca_chain)(conf, ca_chain, ca_crl);
}
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_ca_cb, void, mbedtls_ssl_config *conf,
                            mbedtls_x509_crt_ca_cb_t f_ca_cb,
                            void *p_ca_cb) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_ca_cb)(conf, f_ca_cb, p_ca_cb);
}
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_own_cert, int, mbedtls_ssl_config *conf,
                              mbedtls_x509_crt *own_cert,
                              mbedtls_pk_context *pk_key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_own_cert)(conf, own_cert, pk_key);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_psk, int, mbedtls_ssl_config *conf,
                         const unsigned char *psk, size_t psk_len,
                         const unsigned char *psk_identity, size_t psk_identity_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_psk)(conf, psk, psk_len, psk_identity, psk_identity_len);
}
#if defined(MBEDTLS_USE_PSA_CRYPTO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_psk_opaque, int, mbedtls_ssl_config *conf,
                                psa_key_id_t psk,
                                const unsigned char *psk_identity,
                                size_t psk_identity_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_psk_opaque)(conf, psk, psk_identity, psk_identity_len);
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_psk, int, mbedtls_ssl_context *ssl,
                           const unsigned char *psk, size_t psk_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_psk)(ssl, psk, psk_len);
}
#if defined(MBEDTLS_USE_PSA_CRYPTO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_psk_opaque, int, mbedtls_ssl_context *ssl,
                                  psa_key_id_t psk) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_psk_opaque)(ssl, psk);
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_psk_cb, void, mbedtls_ssl_config *conf,
                             int (*f_psk)(void *, mbedtls_ssl_context *, const unsigned char *,
                                          size_t),
                             void *p_psk) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_psk_cb)(conf, f_psk, p_psk);
}
#endif /* MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED */
#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_SSL_SRV_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dh_param_bin, int, mbedtls_ssl_config *conf,
                                  const unsigned char *dhm_P, size_t P_len,
                                  const unsigned char *dhm_G,  size_t G_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dh_param_bin)(conf, dhm_P, P_len, dhm_G, G_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dh_param_ctx, int, mbedtls_ssl_config *conf, mbedtls_dhm_context *dhm_ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dh_param_ctx)(conf, dhm_ctx);
}
#endif /* MBEDTLS_DHM_C && defined(MBEDTLS_SSL_SRV_C) */
#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_SSL_CLI_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dhm_min_bitlen, void, mbedtls_ssl_config *conf,
                                     unsigned int bitlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dhm_min_bitlen)(conf, bitlen);
}
#endif /* MBEDTLS_DHM_C && MBEDTLS_SSL_CLI_C */
#if defined(MBEDTLS_ECP_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_curves, void, mbedtls_ssl_config *conf,
                             const mbedtls_ecp_group_id *curves) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_curves)(conf, curves);
}
#endif /* MBEDTLS_ECP_C */
#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_sig_hashes, void, mbedtls_ssl_config *conf,
                                 const int *hashes) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_sig_hashes)(conf, hashes);
}
#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hostname, int, mbedtls_ssl_context *ssl, const char *hostname) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hostname)(ssl, hostname);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_own_cert, int, mbedtls_ssl_context *ssl,
                                mbedtls_x509_crt *own_cert,
                                mbedtls_pk_context *pk_key) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_own_cert)(ssl, own_cert, pk_key);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_ca_chain, void, mbedtls_ssl_context *ssl,
                                 mbedtls_x509_crt *ca_chain,
                                 mbedtls_x509_crl *ca_crl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_ca_chain)(ssl, ca_chain, ca_crl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_authmode, void, mbedtls_ssl_context *ssl,
                                 int authmode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_authmode)(ssl, authmode);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_sni, void, mbedtls_ssl_config *conf,
                          int (*f_sni)(void *, mbedtls_ssl_context *, const unsigned char *,
                                       size_t),
                          void *p_sni) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_sni)(conf, f_sni, p_sni);
}
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_set_hs_ecjpake_password, int, mbedtls_ssl_context *ssl,
                                        const unsigned char *pw,
                                        size_t pw_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_set_hs_ecjpake_password)(ssl, pw, pw_len);
}
#endif /*MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */
#if defined(MBEDTLS_SSL_ALPN)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_alpn_protocols, int, mbedtls_ssl_config *conf, const char **protos) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_alpn_protocols)(conf, protos);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_alpn_protocol, const char *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_alpn_protocol)(ssl);
}
#endif /* MBEDTLS_SSL_ALPN */
#if defined(MBEDTLS_SSL_DTLS_SRTP)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_srtp_mki_value_supported, void, mbedtls_ssl_config *conf,
                                               int support_mki_value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_srtp_mki_value_supported)(conf, support_mki_value);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_dtls_srtp_protection_profiles, int, mbedtls_ssl_config *conf,
    const mbedtls_ssl_srtp_profile *profiles) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_dtls_srtp_protection_profiles)(conf, profiles);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_dtls_srtp_set_mki_value, int, mbedtls_ssl_context *ssl,
                                        unsigned char *mki_value,
                                        uint16_t mki_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_dtls_srtp_set_mki_value)(ssl, mki_value, mki_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_dtls_srtp_negotiation_result, void, const mbedtls_ssl_context *ssl,
                                                  mbedtls_dtls_srtp_info *dtls_srtp_info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_dtls_srtp_negotiation_result)(ssl, dtls_srtp_info);
}
#endif /* MBEDTLS_SSL_DTLS_SRTP */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_max_version, void, mbedtls_ssl_config *conf, int major, int minor) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_max_version)(conf, major, minor);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_min_version, void, mbedtls_ssl_config *conf, int major, int minor) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_min_version)(conf, major, minor);
}
#if defined(MBEDTLS_SSL_FALLBACK_SCSV) && defined(MBEDTLS_SSL_CLI_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_fallback, void, mbedtls_ssl_config *conf, char fallback) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_fallback)(conf, fallback);
}
#endif /* MBEDTLS_SSL_FALLBACK_SCSV && MBEDTLS_SSL_CLI_C */
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_encrypt_then_mac, void, mbedtls_ssl_config *conf, char etm) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_encrypt_then_mac)(conf, etm);
}
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */
#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_extended_master_secret, void, mbedtls_ssl_config *conf, char ems) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_extended_master_secret)(conf, ems);
}
#endif /* MBEDTLS_SSL_EXTENDED_MASTER_SECRET */
#if defined(MBEDTLS_ARC4_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_arc4_support, void, mbedtls_ssl_config *conf, char arc4) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_arc4_support)(conf, arc4);
}
#endif /* MBEDTLS_ARC4_C */
#if defined(MBEDTLS_SSL_SRV_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_cert_req_ca_list, void, mbedtls_ssl_config *conf,
                                       char cert_req_ca_list) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_cert_req_ca_list)(conf, cert_req_ca_list);
}
#endif /* MBEDTLS_SSL_SRV_C */
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_max_frag_len, int, mbedtls_ssl_config *conf, unsigned char mfl_code) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_max_frag_len)(conf, mfl_code);
}
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */
#if defined(MBEDTLS_SSL_TRUNCATED_HMAC)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_truncated_hmac, void, mbedtls_ssl_config *conf, int truncate) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_truncated_hmac)(conf, truncate);
}
#endif /* MBEDTLS_SSL_TRUNCATED_HMAC */
#if defined(MBEDTLS_SSL_CBC_RECORD_SPLITTING)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_cbc_record_splitting, void, mbedtls_ssl_config *conf, char split) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_cbc_record_splitting)(conf, split);
}
#endif /* MBEDTLS_SSL_CBC_RECORD_SPLITTING */
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_session_tickets, void, mbedtls_ssl_config *conf, int use_tickets) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_session_tickets)(conf, use_tickets);
}
#endif /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_CLI_C */
#if defined(MBEDTLS_SSL_RENEGOTIATION)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_renegotiation, void, mbedtls_ssl_config *conf, int renegotiation) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_renegotiation)(conf, renegotiation);
}
#endif /* MBEDTLS_SSL_RENEGOTIATION */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_legacy_renegotiation, void, mbedtls_ssl_config *conf, int allow_legacy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_legacy_renegotiation)(conf, allow_legacy);
}
#if defined(MBEDTLS_SSL_RENEGOTIATION)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_renegotiation_enforced, void, mbedtls_ssl_config *conf, int max_records) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_renegotiation_enforced)(conf, max_records);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_conf_renegotiation_period, void, mbedtls_ssl_config *conf,
                                           const unsigned char period[8]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_conf_renegotiation_period)(conf, period);
}
#endif /* MBEDTLS_SSL_RENEGOTIATION */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_check_pending, int, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_check_pending)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_bytes_avail, size_t, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_bytes_avail)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_verify_result, uint32_t, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_verify_result)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_ciphersuite, const char *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_ciphersuite)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_version, const char *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_version)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_record_expansion, int, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_record_expansion)(ssl);
}
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_output_max_frag_len, size_t, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_output_max_frag_len)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_input_max_frag_len, size_t, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_input_max_frag_len)(ssl);
}
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_max_out_record_payload, int, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_max_out_record_payload)(ssl);
}
#if defined(MBEDTLS_X509_CRT_PARSE_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_peer_cert, const mbedtls_x509_crt *, const mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_peer_cert)(ssl);
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_SSL_CLI_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_session, int, const mbedtls_ssl_context *ssl, mbedtls_ssl_session *session) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_session)(ssl, session);
}
#endif /* MBEDTLS_SSL_CLI_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_handshake, int, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_handshake)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_handshake_step, int, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_handshake_step)(ssl);
}
#if defined(MBEDTLS_SSL_RENEGOTIATION)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_renegotiate, int, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_renegotiate)(ssl);
}
#endif /* MBEDTLS_SSL_RENEGOTIATION */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_read, int, mbedtls_ssl_context *ssl, unsigned char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_read)(ssl, buf, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_write, int, mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_write)(ssl, buf, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_send_alert_message, int, mbedtls_ssl_context *ssl,
                                   unsigned char level,
                                   unsigned char message) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_send_alert_message)(ssl, level, message);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_close_notify, int, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_close_notify)(ssl);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_free, void, mbedtls_ssl_context *ssl) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_free)(ssl);
}
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_context_save, int, mbedtls_ssl_context *ssl,
                             unsigned char *buf,
                             size_t buf_len,
                             size_t *olen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_context_save)(ssl, buf, buf_len, olen);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_context_load, int, mbedtls_ssl_context *ssl,
                             const unsigned char *buf,
                             size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_context_load)(ssl, buf, len);
}
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_config_init, void, mbedtls_ssl_config *conf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_config_init)(conf);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_config_defaults, int, mbedtls_ssl_config *conf,
                                int endpoint, int transport, int preset) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_config_defaults)(conf, endpoint, transport, preset);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_config_free, void, mbedtls_ssl_config *conf) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_config_free)(conf);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_session_init, void, mbedtls_ssl_session *session) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_session_init)(session);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_session_free, void, mbedtls_ssl_session *session) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_session_free)(session);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_tls_prf, int, const mbedtls_tls_prf_types prf,
                         const unsigned char *secret, size_t slen,
                         const char *label,
                         const unsigned char *random, size_t rlen,
                         unsigned char *dstbuf, size_t dlen) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_tls_prf)(prf, secret, slen, label, random, rlen, dstbuf, dlen);
}

// <mbedtls/net_sockets.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_net_init, void, mbedtls_net_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_connect, int, mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_connect)(ctx, host, port, proto);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_bind, int, mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_bind)(ctx, bind_ip, port, proto);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_accept, int, mbedtls_net_context *bind_ctx,
                       mbedtls_net_context *client_ctx,
                       void *client_ip, size_t buf_size, size_t *ip_len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_accept)(bind_ctx, client_ctx, client_ip, buf_size, ip_len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_poll, int, mbedtls_net_context *ctx, uint32_t rw, uint32_t timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_poll)(ctx, rw, timeout);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_set_block, int, mbedtls_net_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_set_block)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_set_nonblock, int, mbedtls_net_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_set_nonblock)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_usleep, void, unsigned long usec) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_usleep)(usec);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_recv, int, void *ctx, unsigned char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_recv)(ctx, buf, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_send, int, void *ctx, const unsigned char *buf, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_send)(ctx, buf, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_recv_timeout, int, void *ctx, unsigned char *buf, size_t len,
                             uint32_t timeout) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_recv_timeout)(ctx, buf, len, timeout);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_close, void, mbedtls_net_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_close)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_net_free, void, mbedtls_net_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_net_free)(ctx);
}

// <mbedtls/entropy.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_init, void, mbedtls_entropy_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_init)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_free, void, mbedtls_entropy_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_free)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_add_source, int, mbedtls_entropy_context *ctx,
                               mbedtls_entropy_f_source_ptr f_source, void *p_source,
                               size_t threshold, int strong) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_add_source)(ctx, f_source, p_source, threshold, strong);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_gather, int, mbedtls_entropy_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_gather)(ctx);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_func, int, void *data, unsigned char *output, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_func)(data, output, len);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_update_manual, int, mbedtls_entropy_context *ctx,
                                  const unsigned char *data, size_t len) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_update_manual)(ctx, data, len);
}
#if defined(MBEDTLS_ENTROPY_NV_SEED)
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_update_nv_seed, int, mbedtls_entropy_context *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_update_nv_seed)(ctx);
}
#endif /* MBEDTLS_ENTROPY_NV_SEED */
#if defined(MBEDTLS_FS_IO)
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_write_seed_file, int, mbedtls_entropy_context *ctx, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_write_seed_file)(ctx, path);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_update_seed_file, int, mbedtls_entropy_context *ctx, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_update_seed_file)(ctx, path);
}
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_SELF_TEST)
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_self_test)(verbose);
}
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
VSF_APPLET_MBEDTLS_IMP(mbedtls_entropy_source_self_test, int, int verbose) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_entropy_source_self_test)(verbose);
}
#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */
#endif /* MBEDTLS_SELF_TEST */

// <mbedtls/ssl_ciphersuites.h>

VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_list_ciphersuites, const int *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_list_ciphersuites)();
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_ciphersuite_from_string, const mbedtls_ssl_ciphersuite_t *, const char *ciphersuite_name) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_ciphersuite_from_string)(ciphersuite_name);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_ciphersuite_from_id, const mbedtls_ssl_ciphersuite_t *, int ciphersuite_id) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_ciphersuite_from_id)(ciphersuite_id);
}
#if defined(MBEDTLS_PK_C)
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_ciphersuite_sig_pk_alg, mbedtls_pk_type_t, const mbedtls_ssl_ciphersuite_t *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_ciphersuite_sig_pk_alg)(info);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_get_ciphersuite_sig_alg, mbedtls_pk_type_t, const mbedtls_ssl_ciphersuite_t *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_get_ciphersuite_sig_alg)(info);
}
#endif /* MBEDTLS_PK_C */
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_ciphersuite_uses_ec, int, const mbedtls_ssl_ciphersuite_t *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_ciphersuite_uses_ec)(info);
}
VSF_APPLET_MBEDTLS_IMP(mbedtls_ssl_ciphersuite_uses_psk, int, const mbedtls_ssl_ciphersuite_t *info) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_MBEDTLS_ENTRY(mbedtls_ssl_ciphersuite_uses_psk)(info);
}

#endif
