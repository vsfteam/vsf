#ifndef OPENSSL_BN_H
#define OPENSSL_BN_H

#include <openssl/crypto.h>
#include <openssl/types.h>

#include <mbedtls/bignum.h>
#include <mbedtls/rsa.h>
#include <mbedtls/md.h>
#include <stdlib.h>

#include <hal/arch/vsf_arch.h>

#define BN_ULONG                    mbedtls_mpi_uint
#if     defined(MBEDTLS_HAVE_INT64)
#define BN_BYTES                    8
#define BN_MASK2                    (0xFFFFFFFFFFFFFFFFULL)
#elif   defined(MBEDTLS_HAVE_INT32)
#define BN_BYTES                    4
#define BN_MASK2                    (0xFFFFFFFFUL)
#else
#   error one of MBEDTLS_HAVE_INT64/MBEDTLS_HAVE_INT32 should be defined in mbedtls
#endif

#define BN_BITS2                    (BN_BYTES * 8)
#define BN_BITS                     (BN_BITS2 * 2)

#define BN_FLG_MALLOCED             0x01
#define BN_FLG_STATIC_DATA          0x02

#define BN_FLG_CONSTTIME            0x04
#define BN_FLG_SECURE               0x08

#ifdef BN_DEBUG
#   define BN_FLG_FIXED_TOP         0x10000
#else
#   define BN_FLG_FIXED_TOP         0
#endif

struct bignum_st {
    union {
        mbedtls_mpi mbedtls_bn;
        struct {
            int __mbedtls_bn_s;
            size_t dmax;
            BN_ULONG *d;
        };
    };
    int top;
    int neg;
    int flags;
};

struct bignum_ctx {
    int not_necessary;
};

#ifdef OPENSSL_CFG_INLINE
OPENSSL_CFG_INLINE int BN_get_flags(const BIGNUM *a, int n) {
    return a->flags & n;
}

OPENSSL_CFG_INLINE BIGNUM *BN_new(void) {
    BIGNUM *bn = (BIGNUM *)OPENSSL_zalloc(sizeof(*bn));
    if (bn != NULL) {
        mbedtls_mpi_init(&bn->mbedtls_bn);
        bn->flags = BN_FLG_MALLOCED;
    }
    return bn;
}

OPENSSL_CFG_INLINE void BN_free(BIGNUM *a) {
    if (!BN_get_flags(a, BN_FLG_STATIC_DATA)) {
        mbedtls_mpi_free(&a->mbedtls_bn);
    }
    if (BN_get_flags(a, BN_FLG_MALLOCED)) {
        OPENSSL_free(a);
    }
}

OPENSSL_CFG_INLINE void BN_clear(BIGNUM *a) {
    if (a != NULL) {
        if (a->d != NULL) {
            OPENSSL_cleanse(a->d, sizeof(*a->d) * a->dmax);
        }
        a->neg = a->top = 0;
        a->flags &= ~BN_FLG_FIXED_TOP;
    }
}

OPENSSL_CFG_INLINE BIGNUM *bn_expand2(BIGNUM *b, int words) {
    if (words > b->dmax) {
        BN_ULONG *a = (BN_ULONG *)OPENSSL_zalloc(words * sizeof(*a));
        if (NULL == a) {
            return NULL;
        }

        VSF_ASSERT(b->top <= words);
        if (b->top > 0) {
            memcpy(a, b->d, sizeof(*a) * b->top);
        }

        if (b->d != NULL) {
            OPENSSL_clear_free(b->d, b->dmax * sizeof(b->d[0]));
        }
        b->d = a;
        b->dmax = words;
    }
    return b;
}

OPENSSL_CFG_INLINE BIGNUM *bn_expand(BIGNUM *a, int bits) {
    if (bits > (INT_MAX - BN_BITS2 + 1)) {
        return NULL;
    }
    if (((bits + BN_BITS2 - 1) / BN_BITS2) < a->dmax) {
        return a;
    }
    return bn_expand2(a, (bits + BN_BITS2 - 1) / BN_BITS2);
}

OPENSSL_CFG_INLINE int BN_set_word(BIGNUM *a, BN_ULONG w) {
    if (bn_expand(a, sizeof(BN_ULONG) * 8) != NULL) {
        return 0;
    }
    a->neg = 0;
    a->d[0] = w;
    a->top = !!w;
    a->flags &= ~BN_FLG_FIXED_TOP;
    return 1;
}

OPENSSL_CFG_INLINE BN_ULONG BN_get_word(const BIGNUM *a) {
    return a->top > 1 ? BN_MASK2 : a->top == 1 ? a->d[0] : 0;
}

OPENSSL_CFG_INLINE int BN_is_zero(const BIGNUM *a) {
    return a->top == 0;
}

OPENSSL_CFG_INLINE int BN_num_bits_word(BN_ULONG l) {
    return vsf_msb32(l);
}

OPENSSL_CFG_INLINE int BN_num_bits(const BIGNUM *a) {
    if (BN_is_zero(a)) {
        return 0;
    }
    int i = a->top - 1;
    return (i * BN_BITS2) + BN_num_bits_word(a->d[i]);
}

OPENSSL_CFG_INLINE int BN_set_bit(BIGNUM *a, int n) {
    return !mbedtls_mpi_set_bit(&a->mbedtls_bn, n, 1);
}

OPENSSL_CFG_INLINE int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b) {
    return !mbedtls_mpi_sub_mpi(&r->mbedtls_bn, &a->mbedtls_bn, &b->mbedtls_bn);
}

OPENSSL_CFG_INLINE BIGNUM *BN_mod_inverse(BIGNUM *r, const BIGNUM *a, const BIGNUM *n, BN_CTX *ctx) {
    BIGNUM *tmp = NULL;
    if (NULL == r) {
        r = tmp = BN_new();
    }
    if (NULL == r) {
        return NULL;
    }
    if (mbedtls_mpi_inv_mod(&r->mbedtls_bn, &a->mbedtls_bn, &n->mbedtls_bn)) {
        if (tmp != NULL) {
            BN_free(tmp);
        }
        return NULL;
    }
    return r;
}

OPENSSL_CFG_INLINE int BN_div(BIGNUM *dv, BIGNUM *rem, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx) {
    return !mbedtls_mpi_div_mpi(&dv->mbedtls_bn, &rem->mbedtls_bn, &m->mbedtls_bn, &d->mbedtls_bn);
}
OPENSSL_CFG_INLINE int BN_mod(BIGNUM *rem, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx) {
    return !mbedtls_mpi_mod_mpi(&rem->mbedtls_bn, &m->mbedtls_bn, &d->mbedtls_bn);
}
OPENSSL_CFG_INLINE int BN_mod_sqr(BIGNUM *r, const BIGNUM *a, const BIGNUM *m, BN_CTX *ctx) {
    return !mbedtls_mpi_mul_mpi(&r->mbedtls_bn, &a->mbedtls_bn, &a->mbedtls_bn)
        &&  !mbedtls_mpi_mod_mpi(&r->mbedtls_bn, &r->mbedtls_bn, &m->mbedtls_bn);
}

OPENSSL_CFG_INLINE BIGNUM *BN_bin2bn(const unsigned char *s, int len, BIGNUM *r) {
    BIGNUM *tmp = NULL;
    if (NULL == r) {
        r = tmp = BN_new();
    }
    if (NULL == r) {
        return NULL;
    }
    if (mbedtls_mpi_read_string(&r->mbedtls_bn, 16, (const char *)s)) {
        if (tmp != NULL) {
            BN_free(tmp);
        }
        return NULL;
    }
    return r;
}





OPENSSL_CFG_INLINE BN_CTX *BN_CTX_new(void) {
    return (BN_CTX *)OPENSSL_zalloc(sizeof(BN_CTX));
}

OPENSSL_CFG_INLINE void BN_CTX_free(BN_CTX *c) {
    OPENSSL_free(c);
}

#else       // OPENSSL_INLINE

BIGNUM *BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret);
int BN_get_flags(const BIGNUM *b, int n);
BIGNUM *BN_new(void);
void BN_free(BIGNUM *a);
void BN_clear(BIGNUM *a);
BIGNUM *bn_expand2(BIGNUM *b, int words);
BIGNUM *bn_expand(BIGNUM *a, int bits);
int BN_set_word(BIGNUM *a, BN_ULONG w);
BN_ULONG BN_get_word(const BIGNUM *a);
int BN_is_zero(const BIGNUM *a);
int BN_num_bits(const BIGNUM *a);
int BN_set_bit(BIGNUM *a, int n);
int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
BIGNUM *BN_mod_inverse(BIGNUM *in, const BIGNUM *a, const BIGNUM *n, BN_CTX *ctx);
int BN_div(BIGNUM *dv, BIGNUM *rem, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx);
int BN_mod(BIGNUM *rem, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx);
int BN_mod_sqr(BIGNUM *r, const BIGNUM *a, const BIGNUM *m, BN_CTX *ctx);



BN_CTX *BN_CTX_new(void);
void BN_CTX_free(BN_CTX *c);

#endif      // OPENSSL_INLINE

#define BN_num_bytes(__a)           ((BN_num_bits(__a) + 7) / 8)

#endif      // OPENSSL_BN_H
