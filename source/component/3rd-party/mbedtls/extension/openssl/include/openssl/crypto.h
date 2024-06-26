#ifndef OPENSSL_CRYPTO_H
#define OPENSSL_CRYPTO_H

#include <stdlib.h>
#include <string.h>
#include <openssl/macros.h>

// mbedtls has only mbedtls_calloc and mbedtls_free, has no realloc support

// void *CRYPTO_malloc(size_t num, const char *file, int line);
#define CRYPTO_malloc(__num, __file, __line)                malloc(__num)

// void *CRYPTO_zalloc(size_t num, const char *file, int line);
#define CRYPTO_zalloc(__num, __file, __line)                calloc(1, (__num))

// void *CRYPTO_realloc(void *addr, size_t num, const char *file, int line);
#define CRYPTO_realloc(__addr, __num, __file, __line)       realloc((__addr), (__num))

// void CRYPTO_free(void *ptr, const char *file, int line);
#define CRYPTO_free(__ptr, __file, __line)                  free(__ptr)

static inline void OPENSSL_clear_free(void *ptr, size_t num) {
    if (ptr != NULL) {
        memset(ptr, 0, num);
        CRYPTO_free(ptr, OPENSSL_FILE, OPENSSL_LINE);
    }
}

// void OPENSSL_cleanse(void *ptr, size_t len);
#define OPENSSL_cleanse(__ptr, __len)                       memset((__ptr), 0, (__len))

#define OPENSSL_malloc(__num)                               CRYPTO_malloc((__num), OPENSSL_FILE, OPENSSL_LINE)
#define OPENSSL_zalloc(__num)                               CRYPTO_zalloc((__num), OPENSSL_FILE, OPENSSL_LINE)
#define OPENSSL_free(__addr)                                CRYPTO_free((__addr), OPENSSL_FILE, OPENSSL_LINE)

#endif      // OPENSSL_CRYPTO_H
