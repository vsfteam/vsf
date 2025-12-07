#ifndef __VSF_LINUX_CRYPTO_HASH_H__
#define __VSF_LINUX_CRYPTO_HASH_H__

#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/string.h>

struct crypto_ahash;

struct hash_alg_common {
    unsigned int digestsize;
    struct crypto_alg base;
};

struct shash_desc {
    struct crypto_shash *tfm;
};

struct shash_alg {
    unsigned int descsize;

    union {
        struct hash_alg_common;
        struct hash_alg_common halg;
    };
};

struct crypto_shash {
    struct crypto_tfm base;
};

#define SHASH_DESC_ON_STACK(shash, ctx)                                         \
    char __##shash##_desc[sizeof(struct shash_desc) + sizeof(psa_hash_operation_t)]\
                VSF_CAL_ALIGN(__alignof__(struct shash_desc));\
    struct shash_desc *shash = (struct shash_desc *)__##shash##_desc

static inline struct crypto_tfm *crypto_shash_tfm(struct crypto_shash *tfm)
{
    return &tfm->base;
}

static inline void *crypto_shash_ctx(struct crypto_shash *tfm)
{
    return crypto_tfm_ctx(&tfm->base);
}

static inline struct shash_alg *__crypto_shash_alg(struct crypto_alg *alg)
{
    return vsf_container_of(alg, struct shash_alg, base);
}

static inline struct shash_alg *crypto_shash_alg(struct crypto_shash *tfm)
{
    return __crypto_shash_alg(crypto_shash_tfm(tfm)->__crt_alg);
}

static inline unsigned int crypto_shash_descsize(struct crypto_shash *tfm)
{
    return crypto_shash_alg(tfm)->descsize;
}

static inline struct crypto_shash *crypto_alloc_shash(const char *alg_name, u32 type, u32 mask)
{
    extern const struct crypto_type crypto_shash_type;
    return (struct crypto_shash *)crypto_alloc_tfm(alg_name, &crypto_shash_type, type, mask);
}

static inline void crypto_free_shash(struct crypto_shash *tfm)
{
    crypto_destroy_tfm(tfm, crypto_shash_tfm(tfm));
}

int crypto_shash_init(struct shash_desc *desc);
int crypto_shash_setkey(struct crypto_shash *tfm, const u8 *key, unsigned int keylen);
int crypto_shash_export(struct shash_desc *desc, void *out);
int crypto_shash_import(struct shash_desc *desc, const void *in);
int crypto_shash_finup(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out);
int crypto_shash_update(struct shash_desc *desc, const u8 *data, unsigned int len);
int crypto_shash_final(struct shash_desc *desc, u8 *out);

static inline int crypto_shash_digest(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out)
{
    if (crypto_shash_init(desc) || crypto_shash_finup(desc, data, len, out)) {
        return -EIO;
    }
    return 0;
}

static inline int crypto_shash_tfm_digest(struct crypto_shash *tfm, const u8 *data, unsigned int len, u8 *out)
{
    SHASH_DESC_ON_STACK(desc, tfm);
    desc->tfm = tfm;
    return crypto_shash_digest(desc, data, len, out);
}

#endif
