#ifndef __VSF_LINUX_CRYPTO_ECDH_H__
#define __VSF_LINUX_CRYPTO_ECDH_H__

struct ecdh {
    char *key;
    unsigned short key_size;
};

unsigned int crypto_ecdh_key_len(const struct ecdh *params);
int crypto_ecdh_encode_key(char *buf, unsigned int len, const struct ecdh *p);
int crypto_ecdh_decode_key(const char *buf, unsigned int len, struct ecdh *p);

#endif
