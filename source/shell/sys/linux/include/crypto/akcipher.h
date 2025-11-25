#ifndef __VSF_LINUX_CRYPTO_AKCIPHER_H__
#define __VSF_LINUX_CRYPTO_AKCIPHER_H__

#include <linux/atomic.h>
#include <linux/crypto.h>

struct akcipher_request {
    struct crypto_async_request base;
    struct scatterlist *src;
    struct scatterlist *dst;
    unsigned int src_len;
    unsigned int dst_len;
};

struct crypto_akcipher {
    unsigned int reqsize;
    struct crypto_tfm base;
};

struct akcipher_alg {
    int (*encrypt)(struct akcipher_request *req);
    int (*decrypt)(struct akcipher_request *req);
    int (*set_pub_key)(struct crypto_akcipher *tfm, const void *key, unsigned int keylen);
    int (*set_priv_key)(struct crypto_akcipher *tfm, const void *key, unsigned int keylen);
    unsigned int (*max_size)(struct crypto_akcipher *tfm);
    int (*init)(struct crypto_akcipher *tfm);
    void (*exit)(struct crypto_akcipher *tfm);

    struct crypto_alg base;
};

static inline struct crypto_tfm *crypto_akcipher_tfm(struct crypto_akcipher *tfm)
{
    return &tfm->base;
}

static inline struct akcipher_alg *crypto_akcipher_alg(struct crypto_akcipher *tfm)
{
    struct crypto_alg *alg = crypto_akcipher_tfm(tfm)->__crt_alg;
    return container_of(alg, struct akcipher_alg, base);
}

static inline unsigned int crypto_akcipher_reqsize(struct crypto_akcipher *tfm)
{
    return tfm->reqsize;
}

static inline void akcipher_request_set_tfm(struct akcipher_request *req, struct crypto_akcipher *tfm)
{
    req->base.tfm = crypto_akcipher_tfm(tfm);
}

static inline struct crypto_akcipher *crypto_akcipher_reqtfm(struct akcipher_request *req)
{
    return container_of(req->base.tfm, struct crypto_akcipher, base);
}

static inline int crypto_akcipher_set_pub_key(struct crypto_akcipher *tfm, const void *key, unsigned int keylen)
{
    struct akcipher_alg *alg = crypto_akcipher_alg(tfm);
    return alg->set_pub_key(tfm, key, keylen);
}

static inline struct akcipher_request *akcipher_request_alloc(struct crypto_akcipher *tfm, gfp_t gfp)
{
    struct akcipher_request *req = kmalloc(sizeof(*req) + crypto_akcipher_reqsize(tfm), gfp);
    if (req) { akcipher_request_set_tfm(req, tfm); }
    return req;
}

static inline void akcipher_request_free(struct akcipher_request *req)
{
    kfree(req);
}

static inline void akcipher_request_set_callback(struct akcipher_request *req, u32 flgs, crypto_completion_t cmpl, void *data)
{
    req->base.complete = cmpl;
    req->base.data = data;
    req->base.flags = flgs;
}

static inline void akcipher_request_set_crypt(struct akcipher_request *req, struct scatterlist *src, struct scatterlist *dst, unsigned int src_len, unsigned int dst_len)
{
    req->src = src;
    req->dst = dst;
    req->src_len = src_len;
    req->dst_len = dst_len;
}

static inline int crypto_akcipher_encrypt(struct akcipher_request *req)
{
    struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
    return crypto_akcipher_alg(tfm)->encrypt(req);
}

struct crypto_akcipher *crypto_alloc_akcipher(const char *alg_name, u32 type, u32 mask);
static inline void crypto_free_akcipher(struct crypto_akcipher *tfm)
{
    crypto_destroy_tfm(tfm, crypto_akcipher_tfm(tfm));
}

#endif
