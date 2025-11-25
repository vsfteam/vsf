#ifndef __VSF_LINUX_CRYPTO_H__
#define __VSF_LINUX_CRYPTO_H__

#include <linux/types.h>
#include <linux/slab.h>

#include <psa/crypto.h>

#define CRYPTO_ALG_TYPE_MASK                        0x0000000f
#define CRYPTO_ALG_TYPE_CIPHER                      0x00000001
#define CRYPTO_ALG_TYPE_AEAD                        0x00000003
#define CRYPTO_ALG_TYPE_LSKCIPHER                   0x00000004
#define CRYPTO_ALG_TYPE_SKCIPHER                    0x00000005
#define CRYPTO_ALG_TYPE_AKCIPHER                    0x00000006
#define CRYPTO_ALG_TYPE_SIG                         0x00000007
#define CRYPTO_ALG_TYPE_KPP                         0x00000008
#define CRYPTO_ALG_TYPE_ACOMPRESS                   0x0000000a
#define CRYPTO_ALG_TYPE_SCOMPRESS                   0x0000000b
#define CRYPTO_ALG_TYPE_RNG                         0x0000000c
#define CRYPTO_ALG_TYPE_HASH                        0x0000000e
#define CRYPTO_ALG_TYPE_SHASH                       0x0000000e
#define CRYPTO_ALG_TYPE_AHASH                       0x0000000f

#define CRYPTO_TFM_REQ_MAY_BACKLOG                  0x00000400

typedef void (*crypto_completion_t)(void *req, int err);

struct crypto_tfm;
struct crypto_async_request {
    crypto_completion_t complete;
    void *data;
    struct crypto_tfm *tfm;
    u32 flags;
};

struct crypto_alg {
    vsf_dlist_node_t node;

    unsigned int cra_blocksize;
    unsigned int cra_ctxsize;
    const char *cra_name;

    psa_algorithm_t alg;
};

struct crypto_template;
struct crypto_alg_tree {
    struct crypto_alg_tree *next;
    struct crypto_alg *alg;
};

struct crypto_template {
    vsf_dlist_node_t node;
    int (*crate)(struct crypto_template *tmpl, struct crypto_alg_tree *arg);
    const char *name;
};

struct crypto_tfm {
    struct crypto_alg *__crt_alg;
    void *__crt_ctx[];
};

struct crypto_wait {
    int err;
};
#define DECLARE_CRYPTO_WAIT(_wait)                  struct crypto_wait _wait = { 0 }

struct crypto_type;
void *crypto_alloc_tfm(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask);
static inline void crypto_destroy_tfm(void *mem, struct crypto_tfm *tfm)
{
    kfree(mem);
}

static inline void *crypto_tfm_ctx(struct crypto_tfm *tfm)
{
    return tfm->__crt_ctx;
}

void crypto_req_done(void *req, int err);

static inline int crypto_wait_req(int err, struct crypto_wait *wait)
{
    switch (err) {
    case -EINPROGRESS:
    case -EBUSY:
//        wait_for_completion(&wait->completion);
//        reinit_completion(&wait->completion);
        err = wait->err;
        break;
    }
    return err;
}

static inline void crypto_free_tfm(struct crypto_tfm *tfm)
{
    return crypto_destroy_tfm(tfm, tfm);
}

int vsf_linux_crypto_init(void);

#endif
