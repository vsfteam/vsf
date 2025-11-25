#ifndef __VSF_LINUX_CRYPTO_INTERNAL_H__
#define __VSF_LINUX_CRYPTO_INTERNAL_H__

#include <linux/types.h>

struct crypto_type {
    unsigned int type;
    unsigned int tfmsize;
};

void *crypto_alloc_tfm(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask);

#endif
