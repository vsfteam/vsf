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

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED && VSF_AIC8800_USE_MBEDTLS_ACCELERATOR == ENABLED

#include "mbedtls/bignum.h"

#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"
#include "mbedtls/gcm.h"
#include "mbedtls/ecp.h"

#include "ce_api.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef MBEDTLS_AES_SETKEY_ENC_ALT
int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    ctx->rk = ctx->buf;

    switch( keybits )
    {
        case 128: ctx->nr = 10; break;
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }
    memcpy(ctx->buf, key, keybits/8);

    return( 0 );
}
#endif

#ifdef MBEDTLS_AES_SETKEY_DEC_ALT
int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    ctx->rk = ctx->buf;

    switch( keybits )
    {
        case 128: ctx->nr = 10; break;
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }
    memcpy(ctx->buf, key, keybits/8);

    return( 0 );
}
#endif

#if     defined(MBEDTLS_AES_ENCRYPT_ALT) || defined(MBEDTLS_AES_DECRYPT_ALT)    \
    ||  defined(MBEDTLS_AES_ENCRYPT_ECB_ALT)
static int __mbedtls_aes_crypto(  mbedtls_aes_context *ctx,
                                  int mode,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    dma_cypt_t dma_cypt_obj = {0};
    aes_state_t aes_obj = {0};

    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = AES_MODE;
    //aes_encr
    if( mode == MBEDTLS_AES_ENCRYPT )
        dma_cypt_obj.aes_enc_decr_sel = AES_ENCR;
    else
        dma_cypt_obj.aes_enc_decr_sel = AES_DECR;
    //aes_mode
    dma_cypt_obj.aes_mode = AES_ECB_MODE;

    //key length
    switch( ctx->nr )
    {
        case 10: dma_cypt_obj.aes_keylen = AES_128_MODE; break;
        case 12: dma_cypt_obj.aes_keylen = AES_192_MODE; break;
        case 14: dma_cypt_obj.aes_keylen = AES_256_MODE; break;
        default : return( -1 );
    }
    dma_cypt_obj.aes_endian_sel = AES_LITTLE_ENDIAN;

    for(int i = 0; i < 8; i++)
        dma_cypt_obj.aes_key[i] = ctx->rk[i];

    dma_cypt_init(&dma_cypt_obj);
    aes_obj.aes_data = (uint32_t *)input;
    aes_obj.length_byte = 16;

    aes_calc(&dma_cypt_obj, &aes_obj, (uint32_t *)output);

    return( 0 );
}
#endif

#ifdef MBEDTLS_AES_ENCRYPT_ALT
int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return __mbedtls_aes_crypto(ctx, MBEDTLS_AES_ENCRYPT, input, output);
}
#endif

#ifdef MBEDTLS_AES_DECRYPT_ALT
int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    return __mbedtls_aes_crypto(ctx, MBEDTLS_AES_DECRYPT, input, output);
}
#endif

#ifdef MBEDTLS_AES_ENCRYPT_ECB_ALT
int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] )
{
    return __mbedtls_aes_crypto(ctx, mode, input, output);
}
#endif

#ifdef MBEDTLS_AES_ENCRYPT_CBC_ALT
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    int ret = 0;
    uint8_t temp[16] = {0};
    dma_cypt_t dma_cypt_obj = {0};
    aes_state_t aes_obj = {0};

    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = AES_MODE;
    //aes_encr
    if( mode == MBEDTLS_AES_ENCRYPT ) {
        dma_cypt_obj.aes_enc_decr_sel = AES_ENCR;
    } else {
        dma_cypt_obj.aes_enc_decr_sel = AES_DECR;
        memcpy(temp, &(input[length - 16]), 16);
    }
    //aes_mode
    dma_cypt_obj.aes_mode = AES_CBC_MODE;

    //key length
    switch( ctx->nr )
    {
        case 10: dma_cypt_obj.aes_keylen = AES_128_MODE; break;
        case 12: dma_cypt_obj.aes_keylen = AES_192_MODE; break;
        case 14: dma_cypt_obj.aes_keylen = AES_256_MODE; break;
        default : return -1;
    }
    dma_cypt_obj.aes_endian_sel = AES_LITTLE_ENDIAN;
    for(int i = 0; i < 8; i++)
        dma_cypt_obj.aes_key[i] = ctx->rk[i];
    memcpy(dma_cypt_obj.aes_iv, iv, 16);

    dma_cypt_init(&dma_cypt_obj);
    aes_obj.aes_data = (uint32_t *)input;
    aes_obj.length_byte = length;
    aes_calc(&dma_cypt_obj, &aes_obj, (uint32_t *)output);

    if( mode == MBEDTLS_AES_ENCRYPT ) {
        memcpy(iv, &(output[length - 16]), 16);
    } else {
        memcpy(iv, temp, 16);
    }

    return ret;
}
#endif

#ifdef MBEDTLS_AES_ENCRYPT_GCM_ALT
int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    int i;
    uint32_t auth_pad_len = 0;
    dma_cypt_t dma_cypt_obj = {0};
    aes_gcm_state_t aes_gcm_obj = {0};
    mbedtls_aes_context *aes_ctx = (mbedtls_aes_context *)(ctx->cipher_ctx.cipher_ctx);

    if(iv_len != 12) {
        dbg("HW unsupport iv_len %d \r\n", iv_len);
        return -1;
    }

    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = AES_MODE;
    //aes_encr
    if( mode == MBEDTLS_AES_ENCRYPT )
        dma_cypt_obj.aes_enc_decr_sel = AES_ENCR;
    else
        dma_cypt_obj.aes_enc_decr_sel = AES_DECR;
    //aes_mode
    dma_cypt_obj.aes_mode = AES_GCM_MODE;

    //key length
    switch( aes_ctx->nr )
    {
        case 10: dma_cypt_obj.aes_keylen = AES_128_MODE; break;
        case 12: dma_cypt_obj.aes_keylen = AES_192_MODE; break;
        case 14: dma_cypt_obj.aes_keylen = AES_256_MODE; break;
        default : return( -1 );
    }

    dma_cypt_obj.aes_endian_sel = AES_LITTLE_ENDIAN;

    for(i = 0; i < 8; i++) {
        dma_cypt_obj.aes_key[i] = aes_ctx->rk[i];
    }

    memcpy(dma_cypt_obj.aes_iv, iv, 12);
    dma_cypt_obj.aes_iv[3] = 0x01000000;

    aes_gcm_obj.auth_dat_len_byte = add_len;
    auth_pad_len = aes_gcm_obj.auth_dat_len_byte;
    if(auth_pad_len & 0x3) {
        auth_pad_len = (((auth_pad_len >> 2) + 1) << 2);
    } else {
        auth_pad_len = aes_gcm_obj.auth_dat_len_byte;
    }
    aes_gcm_obj.auth_dat_pad_len_byte = auth_pad_len;
    aes_gcm_obj.auth_data = (uint32_t *)add;//padding
    aes_gcm_obj.cipher_dat_len_byte = length;
    aes_gcm_obj.aes_data = (uint32_t *)input;
    dma_cypt_obj.aes_a_len = aes_gcm_obj.auth_dat_len_byte * 8;//bit
    dma_cypt_obj.aes_plain_len = aes_gcm_obj.cipher_dat_len_byte * 8;//bit

    dma_cypt_init(&dma_cypt_obj);
    aes_gcm_calc(&dma_cypt_obj, &aes_gcm_obj, (uint32_t *)output, (uint32_t *)tag);

    return( 0 );
}
#endif

#ifdef MBEDTLS_SHA256_COMPUTE_ALT
void mbedtls_sha256( const unsigned char *input, size_t ilen,
             unsigned char output[32], int is224 )
{
    dma_cypt_t dma_cypt_obj = {0};
    hash_state_t sha256_obj;

    // DMA init
    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = HASH_MODE;
    if(is224) {
        dma_cypt_obj.hash_mode = HASH_SHA224_MODE;
    } else {
        dma_cypt_obj.hash_mode = HASH_SHA256_MODE;
    }
    dma_cypt_obj.hash_pad_sel = HASH_HW_PAD;
    dma_cypt_init(&dma_cypt_obj);
    sha256_obj.buf = (uint8_t *)input;
    sha256_obj.length_byte = ilen;
    hash_proc_512bit(&dma_cypt_obj, &sha256_obj, (unsigned int *)output);
}
#endif

#ifdef MBEDTLS_RSA_EXPMOD_ALT
int mbedtls_mpi_exp_mod( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RR )
{
    dma_cypt_t dma_cypt_obj = {0};

    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = RSA_MODE;
    dma_cypt_obj.rsa_keylen = 1;
    if(N->n >= 64) {
        dma_cypt_obj.rsa_mode = RSA_2048_MODE;
    } else if(N->n == 32) {
        dma_cypt_obj.rsa_mode = RSA_1024_MODE;
    } else {
        dbg("HW RSA not supported N->n %d\r\n", N->n);
        return -1;
    }
    dma_cypt_obj.mod_mult_parameter = N->p[0];
    dma_cypt_init(&dma_cypt_obj);
    rsa_coef_init(E->p, N->p);
    rsa_calc(&dma_cypt_obj, A->p, X->p);

    return 0;
}
#endif

#ifdef MBEDTLS_ECC_MULCPMB_ALT
static int ecp_mul_comb( mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
                         const mbedtls_mpi *m, const mbedtls_ecp_point *P,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         mbedtls_ecp_restart_ctx *rs_ctx )
{
    dma_cypt_t dma_cypt_obj = {0};
    uint32_t ecc_result[64] = {0};

    dma_cypt_obj.ce_chn = DMA_CHANNEL_CYPT_IDX0;
    dma_cypt_obj.dma_cypt_mode = ECC_MODE;
    switch (grp->nbits)
    {
        case 160: dma_cypt_obj.ecc_mode = ECC_160_MODE; break;
        case 224: dma_cypt_obj.ecc_mode = ECC_224_MODE; break;
        case 256: dma_cypt_obj.ecc_mode = ECC_256_MODE; break;
        case 384: dma_cypt_obj.ecc_mode = ECC_384_MODE; break;
        case 512: dma_cypt_obj.ecc_mode = ECC_512_MODE; break;
        default : dbg("ECC HW not supported\r\n"); return -1;
    }
    if(NULL == grp->A.p) {
        dma_cypt_obj.ecc_a_eq_p3 = ECC_A_EQ_MINUS_3;
    } else {
        dma_cypt_obj.ecc_a_eq_p3 = ECC_A_NOT_EQ_MINUS_3;
    }
    dma_cypt_obj.mod_mult_parameter = grp->P.p[0];

    dma_cypt_init(&dma_cypt_obj);
    curve_coef_init(&dma_cypt_obj, grp->A.p, grp->P.p, NULL);
    ecc_generate_key(&dma_cypt_obj, m->p, P->X.p, P->Y.p, ecc_result);
    while(aic_get_ecc_generate_key_ready() == 0);

    uint8_t *p = (uint8_t *)ecc_result;
    mbedtls_mpi_grow( &(R->X), grp->nbits/16);
    R->X.s = 1;
    memcpy(R->X.p, p, grp->nbits/8);
    p += grp->nbits/8;
    mbedtls_mpi_grow( &(R->Y), grp->nbits/16);
    R->Y.s = 1;
    memcpy(R->Y.p, p, grp->nbits/8);
    p += grp->nbits/8;
    mbedtls_mpi_grow( &(R->Z), grp->nbits/16);
    R->Z.s = 1;
    R->Z.p[0] = 1;

    return 0;
}
#endif

#endif      // VSF_USE_MBEDTLS
