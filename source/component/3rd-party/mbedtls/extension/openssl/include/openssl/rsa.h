#ifndef OPENSSL_RSA_H
#define OPENSSL_RSA_H

#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/types.h>

#include <mbedtls/rsa.h>
#include <mbedtls/md.h>
#include <stdlib.h>

struct rsa_st {
    mbedtls_rsa_context mbedtls_rsa;

    BIGNUM *n;
    BIGNUM *e;

    BIGNUM *d;
    BIGNUM *p;
    BIGNUM *q;

    BIGNUM *dmp1;
    BIGNUM *dmq1;
    BIGNUM *igmp;
};

#ifdef OPENSSL_CFG_INLINE
OPENSSL_CFG_INLINE RSA *RSA_new(void) {
    RSA *rsa = (RSA *)OPENSSL_zalloc(sizeof(*rsa));
    if (rsa != NULL) {
        mbedtls_rsa_init(&rsa->mbedtls_rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_NONE);
    }
    return rsa;
}

OPENSSL_CFG_INLINE void RSA_free(RSA *r) {
    mbedtls_rsa_free(&r->mbedtls_rsa);
    OPENSSL_free(r);
}

OPENSSL_CFG_INLINE int RSA_size(const RSA *r) {
    return BN_num_bytes(r->n);
}

OPENSSL_CFG_INLINE int RSA_verify(int type, const unsigned char *m, unsigned int m_len,
                    const unsigned char *sigbuf, unsigned int siglen, RSA *rsa) {
    mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
    return !mbedtls_rsa_pkcs1_verify(&rsa->mbedtls_rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, md_alg, m_len, m, sigbuf);
}

#else       // OPENSSL_CFG_INLINE

RSA *RSA_new(void);
void RSA_free(RSA *r);
int RSA_size(const RSA *r);
int RSA_verify(int type, const unsigned char *m, unsigned int m_len,
                const unsigned char *sigbuf, unsigned int siglen, RSA *rsa);

#endif      // OPENSSL_CFG_INLINE

#endif      // OPENSSL_RSA_H
