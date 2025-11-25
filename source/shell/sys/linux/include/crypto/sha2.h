#ifndef __VSF_LINUX_CRYPTO_SHA2_H__
#define __VSF_LINUX_CRYPTO_SHA2_H__

#define SHA224_DIGEST_SIZE                          28
#define SHA224_BLOCK_SIZE                           64

#define SHA256_DIGEST_SIZE                          32
#define SHA256_BLOCK_SIZE                           64

#define SHA384_DIGEST_SIZE                          48
#define SHA384_BLOCK_SIZE                           128

#define SHA512_DIGEST_SIZE                          64
#define SHA512_BLOCK_SIZE                           128

#include <psa/crypto.h>

struct sha256_state {
    psa_hash_operation_t;
};

#endif
