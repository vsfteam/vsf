/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_USE_MBEDTLS == ENABLED

#include <unistd.h>

#include <linux/crypto.h>
#include <crypto/internal.h>
#include <crypto/akcipher.h>
#include <crypto/kpp.h>
#include <crypto/hash.h>
#include <crypto/sha2.h>
#include <crypto/ecdh.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <psa/crypto.h>
#include <mbedtls/ecdh.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct psa_key_ctx_t {
    psa_key_id_t key_id;
} psa_key_ctx_t;

typedef struct psa_ecdh_ctx_t {
    mbedtls_ecp_group_id id;
    mbedtls_ecp_group grp;

    mbedtls_mpi d;
    mbedtls_ecp_point Q;
} psa_ecdh_ctx_t;

typedef struct vsf_linux_crypto_t {
    vsf_dlist_t alg_list;
    vsf_dlist_t tmpl_list;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
} vsf_linux_crypto_t;

/*============================ GLOBAL VARIABLES ==============================*/

const struct crypto_type crypto_shash_type = {
    .type = CRYPTO_ALG_TYPE_HASH,
    .tfmsize = offsetof(struct crypto_shash, base),
};

const struct crypto_type crypto_akcipher_type = {
    .type = CRYPTO_ALG_TYPE_AKCIPHER,
    .tfmsize = offsetof(struct crypto_akcipher, base),
};

const struct crypto_type crypto_kpp_type = {
    .type = CRYPTO_ALG_TYPE_KPP,
    .tfmsize = offsetof(struct crypto_kpp, base),
};

/*============================ PROTOTYPES ====================================*/

static int __rsa_pkcs1pad_create(struct crypto_template *tmpl, struct crypto_alg_tree *arg);
static void __psa_free_key_ctx(struct crypto_tfm *tfm);

static void __ecdh_nist_p192_init_tfm(struct crypto_tfm *tfm);
static void __ecdh_nist_p256_init_tfm(struct crypto_tfm *tfm);
static void __ecdh_nist_p384_init_tfm(struct crypto_tfm *tfm);
static int __ecdh_set_secret(struct crypto_kpp *tfm, const void *buf, unsigned int len);
static int __ecdh_generate_public_key(struct kpp_request *req);
static int __ecdh_compute_shared_secret(struct kpp_request *req);

/*============================ LOCAL VARIABLES ===============================*/

static struct shash_alg __sha256_algs[] = {
    {
        .base.cra_name              = "sha224",
        .base.cra_blocksize         = SHA224_BLOCK_SIZE,
        .base.alg                   = PSA_ALG_SHA_224,
        .digestsize                 = SHA224_DIGEST_SIZE,
        .descsize                   = sizeof(psa_hash_operation_t),
    },
    {
        .base.cra_name              = "sha256",
        .base.cra_blocksize         = SHA256_BLOCK_SIZE,
        .base.alg                   = PSA_ALG_SHA_256,
        .digestsize                 = SHA256_DIGEST_SIZE,
        .descsize                   = sizeof(psa_hash_operation_t),
    },
    {
        .base.cra_name              = "hmac(sha224)",
        .base.cra_blocksize         = SHA224_BLOCK_SIZE,
        .base.cra_ctxsize           = sizeof(psa_key_ctx_t),
        .base.cra_exit              = __psa_free_key_ctx,
        .base.alg                   = PSA_ALG_HMAC(PSA_ALG_SHA_224),
        .digestsize                 = SHA224_DIGEST_SIZE,
        .descsize                   = sizeof(psa_mac_operation_t),
    },
    {
        .base.cra_name              = "hmac(sha256)",
        .base.cra_blocksize         = SHA256_BLOCK_SIZE,
        .base.cra_ctxsize           = sizeof(psa_key_ctx_t),
        .base.cra_exit              = __psa_free_key_ctx,
        .base.alg                   = PSA_ALG_HMAC(PSA_ALG_SHA_256),
        .digestsize                 = SHA256_DIGEST_SIZE,
        .descsize                   = sizeof(psa_mac_operation_t),
    },
};

static struct akcipher_alg __akcipher_algs[] = {
    {
        // rsa is just a place holder for parsing alg string
        .base.cra_name              = "rsa",
        .base.alg                   = PSA_ALG_VENDOR_FLAG,
    },
};

static struct kpp_alg __kpp_algs[] = {
    {
        .base.cra_name              = "ecdh-nist-p192",
        .base.cra_ctxsize           = sizeof(psa_ecdh_ctx_t),
        .base.cra_init              = __ecdh_nist_p192_init_tfm,
        .base.alg                   = PSA_ALG_ECDH,
        .set_secret                 = __ecdh_set_secret,
        .generate_public_key        = __ecdh_generate_public_key,
        .compute_shared_secret      = __ecdh_compute_shared_secret,
    },
    {
        .base.cra_name              = "ecdh-nist-p256",
        .base.cra_ctxsize           = sizeof(psa_ecdh_ctx_t),
        .base.cra_init              = __ecdh_nist_p256_init_tfm,
        .base.alg                   = PSA_ALG_ECDH,
        .set_secret                 = __ecdh_set_secret,
        .generate_public_key        = __ecdh_generate_public_key,
        .compute_shared_secret      = __ecdh_compute_shared_secret,
    },
    {
        .base.cra_name              = "ecdh-nist-p384",
        .base.cra_ctxsize           = sizeof(psa_ecdh_ctx_t),
        .base.cra_init              = __ecdh_nist_p384_init_tfm,
        .base.alg                   = PSA_ALG_ECDH,
        .set_secret                 = __ecdh_set_secret,
        .generate_public_key        = __ecdh_generate_public_key,
        .compute_shared_secret      = __ecdh_compute_shared_secret,
    },
};

static struct crypto_template __rsa_pkcs1pad_tmpl = {
    .name                           = "pkcs1pad",
    .crate                          = __rsa_pkcs1pad_create,
};

static VSF_CAL_NO_INIT vsf_linux_crypto_t __vsf_linux_crypto;

/*============================ IMPLEMENTATION ================================*/

// generic

static void __psa_free_key_ctx(struct crypto_tfm *tfm)
{
    psa_key_ctx_t *key = (psa_key_ctx_t *)&tfm[1];
    psa_destroy_key(key->key_id);
}

// crypto

int crypto_register_alg(struct crypto_alg *alg)
{
    if (vsf_dlist_is_in(struct crypto_alg, node, &__vsf_linux_crypto.alg_list, alg)) {
        return -EEXIST;
    }

    vsf_dlist_add_to_head(struct crypto_alg, node, &__vsf_linux_crypto.alg_list, alg);
    return 0;
}

void crypto_unregister_alg(struct crypto_alg *alg)
{
    if (!vsf_dlist_is_in(struct crypto_alg, node, &__vsf_linux_crypto.alg_list, alg)) {
        return;
    }

    vsf_dlist_remove(struct crypto_alg, node, &__vsf_linux_crypto.alg_list, alg);
}

static void __crypto_free_alg_tree(struct crypto_alg_tree *root)
{
    if (root->next != NULL) {
        __crypto_free_alg_tree(root->next);
        root->next = NULL;
    }
    kfree(root);
}

static const char *__crypto_generate_alg_tree(const char *alg_name, struct crypto_alg_tree **node)
{
    const char *tmp = NULL;
    struct crypto_alg *alg = NULL;
    __vsf_dlist_foreach_unsafe(struct crypto_alg, node, &__vsf_linux_crypto.alg_list) {
        if (strstr(alg_name, _->cra_name) == alg_name) {
            tmp = &alg_name[strlen(_->cra_name)];
            if ((*tmp == '\0') || (*tmp == ',') || (*tmp == ')')) {
                alg_name = tmp;
                alg = _;
                break;
            }
        }
    }
    if (alg != NULL) {
        *node = kzalloc(sizeof(**node), GFP_KERNEL);
        if (NULL == *node) {
            return ERR_PTR(-ENOMEM);
        }

        (*node)->alg = alg;
        goto parsed;
    }

    *node = NULL;
    struct crypto_template *tmpl = NULL;
    struct crypto_alg_tree *child = NULL;
    __vsf_dlist_foreach_unsafe(struct crypto_template, node, &__vsf_linux_crypto.tmpl_list) {
        if (strstr(alg_name, _->name) == alg_name) {
            tmp = &alg_name[strlen(_->name)];
            if (*tmp == '(') {
                tmpl = _;
                break;
            }
        }
    }
    if (tmpl != NULL) {
        tmp = __crypto_generate_alg_tree(++tmp, &child);
        if (IS_ERR(tmp)) {
            alg_name = tmp;
            goto child_err;
        } else if (*tmp != ')') {
            alg_name = ERR_PTR(-EINVAL);
            goto child_err;
        }
        tmp++;

        int ret = tmpl->crate(tmpl, child);
        if (ret) {
            alg_name = ERR_PTR(ret);
            goto child_err;
        }
        __crypto_free_alg_tree(child);
        alg_name = __crypto_generate_alg_tree(alg_name, node);
        if (IS_ERR(alg_name)) {
            goto err;
        }
        return alg_name;
    }

    return ERR_PTR(-EINVAL);

parsed:
    if (*alg_name == ',') {
        alg_name = __crypto_generate_alg_tree(++alg_name, &(*node)->next);
        if (IS_ERR(alg_name)) {
            goto err;
        }
    }
    return alg_name;
child_err:
    if (child != NULL) {
        __crypto_free_alg_tree(child);
    }
    return alg_name;
err:
    if (*node != NULL) {
        __crypto_free_alg_tree(*node);
        *node = NULL;
    }
    return alg_name;
}

struct crypto_alg *crypto_find_alg(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask)
{
    __vsf_dlist_foreach_unsafe(struct crypto_alg, node, &__vsf_linux_crypto.alg_list) {
        if (!strcmp(alg_name, _->cra_name)) {
            return _;
        }
    }

    struct crypto_alg *alg = NULL;
    struct crypto_alg_tree *root = NULL;
    const char *remain = __crypto_generate_alg_tree(alg_name, &root);
    if ((*remain != '\0') || (NULL == root) || (root->next != NULL)) {
        alg = ERR_PTR(-EINVAL);
    } else {
        alg = root->alg;
    }

    if (root != NULL) {
        __crypto_free_alg_tree(root);
    }
    return alg;
}

void *crypto_alloc_tfm(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask)
{
    struct crypto_alg *alg = crypto_find_alg(alg_name, frontend, type, mask);
    if (IS_ERR(alg)) {
        return alg;
    }

    unsigned int tfmsize = frontend->tfmsize;
    char *mem = kzalloc(tfmsize + sizeof(struct crypto_tfm) + alg->cra_ctxsize, GFP_KERNEL);
    if (NULL == mem) {
         return ERR_PTR(-ENOMEM);
    }

    struct crypto_tfm *tfm = (struct crypto_tfm *)(mem + tfmsize);
    tfm ->__crt_alg = alg;
    if (alg->cra_init != NULL) {
        alg->cra_init(tfm);
    }
    return mem;
}

void crypto_destroy_tfm(void *mem, struct crypto_tfm *tfm)
{
    struct crypto_alg *alg = tfm->__crt_alg;
    if (alg->cra_exit) {
        alg->cra_exit(tfm);
    }
    kfree(mem);
}

void crypto_req_done(void *data, int err)
{
    struct crypto_wait *wait = (struct crypto_wait *)data;
    if (err == -EINPROGRESS) {
        return;
    }
    wait->err = err;
    complete(&wait->completion);
}

// template

int crypto_register_template(struct crypto_template *tmpl)
{
    if (vsf_dlist_is_in(struct crypto_alg, node, &__vsf_linux_crypto.tmpl_list, tmpl)) {
        return -EEXIST;
    }

    vsf_dlist_add_to_head(struct crypto_alg, node, &__vsf_linux_crypto.tmpl_list, tmpl);
    return 0;
}

// HASH

int crypto_register_shash(struct shash_alg *alg)
{
    return crypto_register_alg(&alg->base);
}

void crypto_unregister_shash(struct shash_alg *alg)
{
    crypto_unregister_alg(&alg->base);
}

int crypto_register_shashes(struct shash_alg *algs, int count)
{
    int i, ret;
    for (i = 0; i < count; i++) {
        ret = crypto_register_shash(&algs[i]);
        if (ret) { goto err; }
    }
    return 0;

err:
    for (--i; i >= 0; --i) {
        crypto_unregister_shash(&algs[i]);
    }
    return ret;
}

int crypto_shash_setkey(struct crypto_shash *tfm, const u8 *key, unsigned int keylen)
{
    psa_key_ctx_t *keyctx = (psa_key_ctx_t *)&tfm[1];
    psa_algorithm_t psa_alg = crypto_shash_alg(tfm)->base.alg;
    psa_key_attributes_t attr = { 0 };

    if (PSA_ALG_IS_HMAC(psa_alg)) {
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
        psa_set_key_algorithm(&attr, psa_alg);
        psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    } else {
        return -EOPNOTSUPP;
    }
    if (PSA_SUCCESS != psa_import_key(&attr, key, keylen, &keyctx->key_id)) {
        return -EIO;
    }
    return 0;
}

int crypto_shash_init(struct shash_desc *desc)
{
    psa_key_ctx_t *keyctx = (psa_key_ctx_t *)&desc->tfm[1];
    psa_algorithm_t psa_alg = crypto_shash_alg(desc->tfm)->base.alg;
    if (PSA_ALG_IS_HMAC(psa_alg)) {
        psa_mac_operation_t *op = (psa_mac_operation_t *)&desc[1];
        op->id = 0;
        if (PSA_SUCCESS == psa_mac_sign_setup(op, keyctx->key_id, psa_alg)) {
            return 0;
        }
    } else if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        op->id = 0;
        if (PSA_SUCCESS == psa_hash_setup(op, psa_alg)) {
            return 0;
        }
    }
    return -EOPNOTSUPP;
}

int crypto_shash_export(struct shash_desc *desc, void *out)
{
    psa_algorithm_t psa_alg = crypto_shash_alg(desc->tfm)->base.alg;
    if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        psa_hash_operation_t *op_out = (psa_hash_operation_t *)out;
        op_out->id = 0;
        if (PSA_SUCCESS != psa_hash_clone(op, op_out)) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

int crypto_shash_import(struct shash_desc *desc, const void *in)
{
    psa_algorithm_t psa_alg = crypto_shash_alg(desc->tfm)->base.alg;
    if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        psa_hash_operation_t *op_in = (psa_hash_operation_t *)in;
        op->id = 0;
        if (PSA_SUCCESS != psa_hash_clone(op_in, op)) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

int crypto_shash_finup(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out)
{
    struct shash_alg *alg = crypto_shash_alg(desc->tfm);
    psa_algorithm_t psa_alg = alg->base.alg;
    size_t digestsize_in = alg->digestsize, digestsize_out;

    if (PSA_ALG_IS_HMAC(psa_alg)) {
        psa_mac_operation_t *op = (psa_mac_operation_t *)&desc[1];
        if (    (PSA_SUCCESS != psa_mac_update(op, data, len))
            ||  (PSA_SUCCESS != psa_mac_sign_finish(op, out, digestsize_in, &digestsize_out))) {
            return -EIO;
        }
        return 0;
    } else if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        if (    (PSA_SUCCESS != psa_hash_update(op, data, len))
            ||  (PSA_SUCCESS != psa_hash_finish(op, out, digestsize_in, &digestsize_out))) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

int crypto_shash_update(struct shash_desc *desc, const u8 *data, unsigned int len)
{
    psa_algorithm_t psa_alg = crypto_shash_alg(desc->tfm)->base.alg;
    if (PSA_ALG_IS_HMAC(psa_alg)) {
        psa_mac_operation_t *op = (psa_mac_operation_t *)&desc[1];
        if (PSA_SUCCESS != psa_mac_update(op, data, len)) {
            return -EIO;
        }
    } else if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        if (PSA_SUCCESS != psa_hash_update(op, data, len)) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

int crypto_shash_final(struct shash_desc *desc, u8 *out)
{
    struct shash_alg *alg = crypto_shash_alg(desc->tfm);
    psa_algorithm_t psa_alg = alg->base.alg;
    size_t digestsize_in = alg->digestsize, digestsize_out;

    if (PSA_ALG_IS_HMAC(psa_alg)) {
        psa_mac_operation_t *op = (psa_mac_operation_t *)&desc[1];
        if (PSA_SUCCESS != psa_mac_sign_finish(op, out, digestsize_in, &digestsize_out)) {
            return -EIO;
        }
    } else if (PSA_ALG_IS_HASH(psa_alg)) {
        psa_hash_operation_t *op = (psa_hash_operation_t *)&desc[1];
        if (PSA_SUCCESS != psa_hash_finish(op, out, digestsize_in, &digestsize_out)) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

// akcipher

int crypto_register_akcipher(struct akcipher_alg *alg)
{
    return crypto_register_alg(&alg->base);
}

void crypto_unregister_akcipher(struct akcipher_alg *alg)
{
    crypto_unregister_alg(&alg->base);
}

int crypto_register_akciphers(struct akcipher_alg *algs, int count)
{
    int i, ret;
    for (i = 0; i < count; i++) {
        ret = crypto_register_akcipher(&algs[i]);
        if (ret) { goto err; }
    }
    return 0;

err:
    for (--i; i >= 0; --i) {
        crypto_unregister_akcipher(&algs[i]);
    }
    return ret;
}

int crypto_akcipher_encrypt(struct akcipher_request *req)
{
    struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
    psa_algorithm_t psa_alg = crypto_akcipher_alg(tfm)->base.alg;
    psa_key_ctx_t *keyctx = (psa_key_ctx_t *)&tfm[1];
    size_t real_size = 0;

    if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(psa_alg)) {
        psa_alg = PSA_ALG_RSA_PKCS1V15_CRYPT;
        if (PSA_SUCCESS != psa_asymmetric_encrypt(keyctx->key_id, psa_alg,
                            req->src->buf, req->src_len,
                            NULL, 0,        // no salt, not OAEP
                            req->dst->buf, req->dst_len, &real_size)) {
            return -EIO;
        }
        return 0;
    }
    return -EOPNOTSUPP;
}

static int __rsa_pkcs1pad_set_pub_key(struct crypto_akcipher *tfm, const void *key, unsigned int keylen)
{
    psa_key_ctx_t *keyctx = (psa_key_ctx_t *)&tfm[1];
    psa_key_attributes_t attr = { 0 };

    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attr, PSA_ALG_RSA_PKCS1V15_CRYPT);
    psa_set_key_type(&attr, PSA_KEY_TYPE_RSA_PUBLIC_KEY);
    if (PSA_SUCCESS != psa_import_key(&attr, key, keylen, &keyctx->key_id)) {
        return -EIO;
    }
    return 0;
}

static int __rsa_pkcs1pad_set_priv_key(struct crypto_akcipher *tfm, const void *key, unsigned int keylen)
{
    return -ENOTSUPP;
}

static int __rsa_pkcs1pad_create(struct crypto_template *tmpl, struct crypto_alg_tree *arg)
{
    if (    (NULL == arg) || (NULL == arg->alg) || strcmp(arg->alg->cra_name, "rsa")
        ||  (NULL == arg->next) || (NULL == arg->next->alg) || !PSA_ALG_IS_HASH(arg->next->alg->alg)) {
        return -EINVAL;
    }

    size_t namelen = strlen("pkcs1pad(rsa,") + strlen(arg->next->alg->cra_name) + 2 /* ")\0" */;
    struct akcipher_alg *alg = kzalloc(sizeof(*alg) + namelen, GFP_KERNEL);
    if (NULL == alg) {
        return -ENOMEM;
    }

    char *name = (char *)&alg[1];
    strcpy(name, "pkcs1pad(rsa,");
    strcat(name, arg->next->alg->cra_name);
    strcat(name, ")");

    alg->base.alg = PSA_ALG_RSA_PKCS1V15_SIGN(arg->next->alg->alg);
    alg->base.cra_ctxsize = sizeof(psa_key_ctx_t);
    alg->base.cra_exit = __psa_free_key_ctx;
    alg->base.cra_name = (const char *)name;
    alg->set_priv_key = __rsa_pkcs1pad_set_priv_key;
    alg->set_pub_key = __rsa_pkcs1pad_set_pub_key;
    return crypto_register_akcipher(alg);
}

// kpp

int crypto_register_kpp(struct kpp_alg *alg)
{
    return crypto_register_alg(&alg->base);
}

void crypto_unregister_kpp(struct kpp_alg *alg)
{
    crypto_unregister_alg(&alg->base);
}

int crypto_register_kpps(struct kpp_alg *algs, int count)
{
    int i, ret;
    for (i = 0; i < count; i++) {
        ret = crypto_register_kpp(&algs[i]);
        if (ret) { goto err; }
    }
    return 0;

err:
    for (--i; i >= 0; --i) {
        crypto_unregister_kpp(&algs[i]);
    }
    return ret;
}

static int __ecdh_set_secret(struct crypto_kpp *tfm, const void *buf, unsigned int len)
{
    struct ecdh params;

    if (crypto_ecdh_decode_key((const char *)buf, len, &params) < 0) {
        return -EINVAL;
    }
    if (params.key_size != 0) {
        return -ENOTSUPP;
    }
    return 0;
}

static int __ecdh_generate_public_key(struct kpp_request *req)
{
    struct crypto_kpp *tfm = crypto_kpp_reqtfm(req);
    psa_ecdh_ctx_t *ctx = (psa_ecdh_ctx_t *)&tfm[1];
    if (mbedtls_ecdh_gen_public(&ctx->grp, &ctx->d, &ctx->Q,
                mbedtls_ctr_drbg_random, &__vsf_linux_crypto.ctr_drbg)) {
        return -EIO;
    }
    return 0;
}

static int __ecdh_compute_shared_secret(struct kpp_request *req)
{
    struct crypto_kpp *tfm = crypto_kpp_reqtfm(req);
    psa_ecdh_ctx_t *ctx = (psa_ecdh_ctx_t *)&tfm[1];
    mbedtls_ecp_point Qp;
    mbedtls_mpi z;

    int result = 0;

    mbedtls_ecp_point_init(&Qp);
    mbedtls_mpi_init(&z);
    if (mbedtls_ecp_point_read_binary(&ctx->grp, &Qp, req->src->buf, req->src_len)) {
        return -ENOMEM;
    }

    if (    (mbedtls_ecdh_compute_shared(&ctx->grp, &z, &Qp, &ctx->d,
                    mbedtls_ctr_drbg_random, &__vsf_linux_crypto.ctr_drbg))
        ||  (mbedtls_mpi_write_binary(&z, req->dst->buf, req->dst_len))) {
        result = -EIO;
    }
    mbedtls_mpi_free(&z);
    mbedtls_ecp_point_free(&Qp);
    return result;
}

static void __ecdh_nist_p192_init_tfm(struct crypto_tfm *tfm)
{
    psa_ecdh_ctx_t *ecdh_ctx = (psa_ecdh_ctx_t *)&tfm[1];
    mbedtls_ecp_group_load(&ecdh_ctx->grp, MBEDTLS_ECP_DP_SECP192R1);
}

static void __ecdh_nist_p256_init_tfm(struct crypto_tfm *tfm)
{
    psa_ecdh_ctx_t *ecdh_ctx = (psa_ecdh_ctx_t *)&tfm[1];
    mbedtls_ecp_group_load(&ecdh_ctx->grp, MBEDTLS_ECP_DP_SECP256R1);
}

static void __ecdh_nist_p384_init_tfm(struct crypto_tfm *tfm)
{
    psa_ecdh_ctx_t *ecdh_ctx = (psa_ecdh_ctx_t *)&tfm[1];
    mbedtls_ecp_group_load(&ecdh_ctx->grp, MBEDTLS_ECP_DP_SECP384R1);
}

static inline u8 *ecdh_pack_data(void *dst, const void *src, size_t sz)
{
    memcpy(dst, src, sz);
    return dst + sz;
}

static inline const u8 *ecdh_unpack_data(void *dst, const void *src, size_t sz)
{
    memcpy(dst, src, sz);
    return src + sz;
}

unsigned int crypto_ecdh_key_len(const struct ecdh *params)
{
    return sizeof(struct kpp_secret) + sizeof(params->key_size) + params->key_size;
}

int crypto_ecdh_encode_key(char *buf, unsigned int len, const struct ecdh *params)
{
    if (!buf || (len != crypto_ecdh_key_len(params))) {
        return -EINVAL;
    }

    u8 *ptr = (u8 *)buf;
    struct kpp_secret secret = {
        .type = CRYPTO_KPP_SECRET_TYPE_ECDH,
        .len = len
    };

    ptr = ecdh_pack_data(ptr, &secret, sizeof(secret));
    ptr = ecdh_pack_data(ptr, &params->key_size, sizeof(params->key_size));
    ecdh_pack_data(ptr, params->key, params->key_size);

    return 0;
}

int crypto_ecdh_decode_key(const char *buf, unsigned int len, struct ecdh *params)
{
    const u8 *ptr = (u8 *)buf;
    struct kpp_secret secret;

    if (!buf || len < (sizeof(struct kpp_secret) + sizeof(params->key_size))) {
        return -EINVAL;
    }

    ptr = ecdh_unpack_data(&secret, ptr, sizeof(secret));
    if ((secret.type != CRYPTO_KPP_SECRET_TYPE_ECDH) || (len < secret.len)) {
        return -EINVAL;
    }

    ptr = ecdh_unpack_data(&params->key_size, ptr, sizeof(params->key_size));
    if (secret.len != crypto_ecdh_key_len(params)) {
        return -EINVAL;
    }

    params->key = (void *)ptr;
    return 0;
}

// vsf_linux

int vsf_linux_crypto_init(void)
{
    if (PSA_SUCCESS != psa_crypto_init()) {
        return -ENOMEM;
    }

    // register algs
    vsf_dlist_init(&__vsf_linux_crypto.alg_list);
    crypto_register_shashes(__sha256_algs, ARRAY_SIZE(__sha256_algs));
    crypto_register_kpps(__kpp_algs, ARRAY_SIZE(__kpp_algs));
    crypto_register_akciphers(__akcipher_algs, ARRAY_SIZE(__akcipher_algs));

    // register templates
    vsf_dlist_init(&__vsf_linux_crypto.tmpl_list);
    crypto_register_template(&__rsa_pkcs1pad_tmpl);

    mbedtls_entropy_init(&__vsf_linux_crypto.entropy);
    mbedtls_ctr_drbg_init(&__vsf_linux_crypto.ctr_drbg);

    static const unsigned char __custom[] = "vsf_linux_crypto";
    mbedtls_ctr_drbg_seed(&__vsf_linux_crypto.ctr_drbg,
            mbedtls_entropy_func, &__vsf_linux_crypto.entropy,
            __custom, strlen((const char *)__custom));
    return 0;
}

#endif
