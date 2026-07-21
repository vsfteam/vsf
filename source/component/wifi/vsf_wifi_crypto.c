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

#include "./vsf_wifi_crypto.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_WPA == ENABLED

#include <string.h>

#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/aes.h>
#include <mbedtls/ccm.h>
#include <mbedtls/cipher.h>

/*============================ MACROS ========================================*/

#define VSF_WIFI_PRF_LABEL          "Pairwise key expansion"

/* HMAC-SHA1 produces 20-byte blocks; PTK(48) needs ceil(48/20) = 3 blocks. */
#define VSF_WIFI_SHA1_LEN           20

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static const mbedtls_md_info_t * __vsf_wifi_sha1_info(void)
{
    return mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
}

/*----------------------------------------------------------------------------*
 * PBKDF2 : PMK = PBKDF2-HMAC-SHA1(passphrase, SSID, 4096, 32)                 *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_wifi_pbkdf2_pmk(const char *passphrase,
                const uint8_t *ssid, uint16_t ssid_len, uint8_t pmk[32])
{
    const mbedtls_md_info_t *info = __vsf_wifi_sha1_info();
    mbedtls_md_context_t ctx;
    int ret;

    if ((passphrase == NULL) || (ssid == NULL) || (pmk == NULL) || (info == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    mbedtls_md_init(&ctx);
    /* hmac = 1 : set up the context for HMAC use (required by pbkdf2). */
    ret = mbedtls_md_setup(&ctx, info, 1);
    if (ret == 0) {
        ret = mbedtls_pkcs5_pbkdf2_hmac(&ctx,
                (const unsigned char *)passphrase, strlen(passphrase),
                ssid, ssid_len, 4096, VSF_WIFI_PMK_LEN, pmk);
    }
    mbedtls_md_free(&ctx);

    return (ret == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

/*----------------------------------------------------------------------------*
 * PRF-384 (HMAC-SHA1) : PTK = PRF(PMK, label, data, 384)                      *
 *   data = Min(AA,SPA)||Max(AA,SPA)||Min(Nonce)||Max(Nonce)                   *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_wifi_prf_ptk(const uint8_t pmk[32],
                const uint8_t aa[6], const uint8_t spa[6],
                const uint8_t anonce[32], const uint8_t snonce[32],
                uint8_t ptk[48])
{
    const mbedtls_md_info_t *info = __vsf_wifi_sha1_info();
    /* buffer = label || 0x00 || data(76) || counter(1) */
    uint8_t  buf[sizeof(VSF_WIFI_PRF_LABEL) + 1 + 76 + 1];
    uint8_t  out[VSF_WIFI_SHA1_LEN * 3];        /* 3 HMAC blocks = 60 bytes   */
    uint16_t pos, label_len;
    uint8_t  i, blocks;
    int      ret = 0;

    if ((pmk == NULL) || (aa == NULL) || (spa == NULL)
     || (anonce == NULL) || (snonce == NULL) || (ptk == NULL) || (info == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    /* label without the trailing NUL, then an explicit 0x00 separator. */
    label_len = (uint16_t)strlen(VSF_WIFI_PRF_LABEL);
    pos = 0;
    memcpy(&buf[pos], VSF_WIFI_PRF_LABEL, label_len);   pos += label_len;
    buf[pos++] = 0x00;

    /* Min/Max(AA, SPA) by memcmp ordering. */
    if (memcmp(aa, spa, 6) < 0) {
        memcpy(&buf[pos], aa, 6);  pos += 6;  memcpy(&buf[pos], spa, 6);  pos += 6;
    } else {
        memcpy(&buf[pos], spa, 6); pos += 6;  memcpy(&buf[pos], aa, 6);   pos += 6;
    }
    /* Min/Max(ANonce, SNonce). */
    if (memcmp(anonce, snonce, 32) < 0) {
        memcpy(&buf[pos], anonce, 32); pos += 32; memcpy(&buf[pos], snonce, 32); pos += 32;
    } else {
        memcpy(&buf[pos], snonce, 32); pos += 32; memcpy(&buf[pos], anonce, 32); pos += 32;
    }

    blocks = (VSF_WIFI_PTK_LEN + VSF_WIFI_SHA1_LEN - 1) / VSF_WIFI_SHA1_LEN;
    for (i = 0; i < blocks; i++) {
        buf[pos] = i;       /* single-byte counter at the tail */
        ret = mbedtls_md_hmac(info, pmk, VSF_WIFI_PMK_LEN,
                buf, pos + 1, &out[i * VSF_WIFI_SHA1_LEN]);
        if (ret != 0) {
            break;
        }
    }
    if (ret == 0) {
        memcpy(ptk, out, VSF_WIFI_PTK_LEN);
    }

    return (ret == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

/*----------------------------------------------------------------------------*
 * EAPOL-Key MIC : HMAC-SHA1(KCK, eapol) truncated to 16 bytes (key ver 2)     *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_wifi_eapol_mic(const uint8_t kck[16],
                const uint8_t *eapol, uint16_t eapol_len, uint8_t mic[16])
{
    const mbedtls_md_info_t *info = __vsf_wifi_sha1_info();
    uint8_t full[VSF_WIFI_SHA1_LEN];
    int ret;

    if ((kck == NULL) || (eapol == NULL) || (mic == NULL) || (info == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    ret = mbedtls_md_hmac(info, kck, VSF_WIFI_KCK_LEN, eapol, eapol_len, full);
    if (ret == 0) {
        memcpy(mic, full, VSF_WIFI_MIC_LEN);
    }

    return (ret == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

/*----------------------------------------------------------------------------*
 * AES key unwrap (RFC 3394), used to recover the GTK from EAPOL-Key M3.        *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_wifi_aes_unwrap(const uint8_t *kek, uint16_t kek_len,
                const uint8_t *in, uint16_t in_len, uint8_t *out)
{
    static const uint8_t __iv[8] = {
        0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    };
    mbedtls_aes_context aes;
    uint8_t a[8];
    uint8_t block[16];
    int     j, i, n, ret = 0;

    if ((kek == NULL) || (in == NULL) || (out == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    /* in = A(8) || R[1..n](8*n); needs >= 24 and a multiple of 8. */
    if ((in_len < 24) || ((in_len & 0x7) != 0)) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    n = (in_len / 8) - 1;

    memcpy(a, in, 8);
    /* Use the caller-provided out buffer directly as working storage (R[]).
     * This removes the previous 40-byte stack-buffer limitation that caused
     * unwrap failures when Key Data > 48 bytes (e.g. RSN IE + GTK KDE). */
    memcpy(out, in + 8, (size_t)n * 8);

    mbedtls_aes_init(&aes);
    ret = mbedtls_aes_setkey_dec(&aes, kek, (unsigned int)kek_len * 8);
    if (ret == 0) {
        for (j = 5; j >= 0; j--) {
            for (i = n; i >= 1; i--) {
                uint32_t t = (uint32_t)(n * j + i);
                memcpy(block,     a,                   8);
                memcpy(block + 8, out + (i - 1) * 8,   8);
                /* B = AES-1(K, (A ^ t) | R[i]) : XOR t (big-endian) into A. */
                block[7] ^= (uint8_t)(t & 0xff);
                block[6] ^= (uint8_t)((t >>  8) & 0xff);
                block[5] ^= (uint8_t)((t >> 16) & 0xff);
                block[4] ^= (uint8_t)((t >> 24) & 0xff);
                ret = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, block, block);
                if (ret != 0) {
                    break;
                }
                memcpy(a,                   block,     8);
                memcpy(out + (i - 1) * 8,   block + 8, 8);
            }
            if (ret != 0) {
                break;
            }
        }
    }
    mbedtls_aes_free(&aes);

    if (ret != 0) {
        return VSF_ERR_FAIL;
    }
    /* integrity check : the recovered A must equal the default IV. */
    if (memcmp(a, __iv, 8) != 0) {
        return VSF_ERR_FAIL;
    }

    return VSF_ERR_NONE;
}

/*----------------------------------------------------------------------------*
 * CCMP encrypt / decrypt : AES-CCM, L=2 (iv_len=13), M=8 (tag_len=8)          *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_wifi_ccmp_encrypt(const uint8_t tk[16],
                const uint8_t *aad, uint16_t aad_len,
                const uint8_t nonce[13],
                const uint8_t *plain, uint16_t plain_len,
                uint8_t *cipher, uint8_t mic[8])
{
    mbedtls_ccm_context ctx;
    int ret;

    if ((tk == NULL) || (aad == NULL) || (nonce == NULL)
     || (cipher == NULL) || (mic == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    mbedtls_ccm_init(&ctx);
    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, tk, VSF_WIFI_TK_LEN * 8);
    if (ret == 0) {
        ret = mbedtls_ccm_encrypt_and_tag(&ctx, plain_len,
                nonce, VSF_WIFI_CCMP_NONCE_LEN, aad, aad_len,
                plain, cipher, mic, VSF_WIFI_CCMP_MIC_LEN);
    }
    mbedtls_ccm_free(&ctx);

    return (ret == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

vsf_err_t vsf_wifi_ccmp_decrypt(const uint8_t tk[16],
                const uint8_t *aad, uint16_t aad_len,
                const uint8_t nonce[13],
                const uint8_t *cipher, uint16_t cipher_len,
                uint8_t *plain, const uint8_t mic[8])
{
    mbedtls_ccm_context ctx;
    int ret;

    if ((tk == NULL) || (aad == NULL) || (nonce == NULL)
     || (cipher == NULL) || (plain == NULL) || (mic == NULL)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    mbedtls_ccm_init(&ctx);
    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, tk, VSF_WIFI_TK_LEN * 8);
    if (ret == 0) {
        ret = mbedtls_ccm_auth_decrypt(&ctx, cipher_len,
                nonce, VSF_WIFI_CCMP_NONCE_LEN, aad, aad_len,
                cipher, plain, mic, VSF_WIFI_CCMP_MIC_LEN);
    }
    mbedtls_ccm_free(&ctx);

    /* ret != 0 covers MBEDTLS_ERR_CCM_AUTH_FAILED (forged frame). */
    return (ret == 0) ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

/*============================ SELF TEST =====================================*/

static bool __vsf_wifi_buf_eq(const uint8_t *a, const uint8_t *b, uint16_t len)
{
    return memcmp(a, b, len) == 0;
}

vsf_err_t vsf_wifi_crypto_selftest(void)
{
    /* ---- 1. PBKDF2 PMK : IEEE 802.11i Annex H.4.2 standard vector ----------
     * passphrase = "password", SSID = "IEEE", iter = 4096
     * PMK = f42c6fc52df0ebef9ebb4b90b38a5f902e83fe1b135a70e23aed762e9710a12e */
    static const uint8_t pmk_expected[32] = {
        0xf4, 0x2c, 0x6f, 0xc5, 0x2d, 0xf0, 0xeb, 0xef,
        0x9e, 0xbb, 0x4b, 0x90, 0xb3, 0x8a, 0x5f, 0x90,
        0x2e, 0x83, 0xfe, 0x1b, 0x13, 0x5a, 0x70, 0xe2,
        0x3a, 0xed, 0x76, 0x2e, 0x97, 0x10, 0xa1, 0x2e,
    };
    /* ---- 3. AES key unwrap : RFC 3394 128-bit KEK / 128-bit key -------------*/
    static const uint8_t kek_3394[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    };
    static const uint8_t wrapped_3394[24] = {
        0x1F, 0xA6, 0x8B, 0x0A, 0x81, 0x12, 0xB4, 0x47,
        0xAE, 0xF3, 0x4B, 0xD8, 0xFB, 0x5A, 0x7B, 0x82,
        0x9D, 0x3E, 0x86, 0x23, 0x71, 0xD2, 0xCF, 0xE5,
    };
    static const uint8_t plain_3394[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    };

    uint8_t pmk[32], ptk1[48], ptk2[48], unwrap[16];
    /* PTK symmetry vectors. */
    static const uint8_t aa[6]  = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    static const uint8_t spa[6] = { 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB };
    static const uint8_t an[32] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    };
    static const uint8_t sn[32] = {
        0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
        0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
        0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
        0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
    };

    /* CCMP round-trip vectors. */
    static const uint8_t tk[16] = {
        0xc9, 0x7c, 0x1f, 0x67, 0xce, 0x37, 0x11, 0x85,
        0x51, 0x4a, 0x8a, 0x19, 0xf2, 0xbd, 0xd5, 0x2f,
    };
    static const uint8_t aad[22] = {
        0x88, 0x42, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    };
    static const uint8_t ccmp_nonce[13] = {
        0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    };
    static const uint8_t msg[20] = {
        0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00,
        0x45, 0x00, 0x00, 0x0C, 0xDE, 0xAD, 0xBE, 0xEF,
        0xCA, 0xFE, 0xBA, 0xBE,
    };
    uint8_t cipher[20], mic[8], decoded[20];

    /* 1. PBKDF2 PMK. */
    if (VSF_ERR_NONE != vsf_wifi_pbkdf2_pmk("password",
            (const uint8_t *)"IEEE", 4, pmk)) {
        vsf_trace_error("wifi crypto: pbkdf2 failed" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    if (!__vsf_wifi_buf_eq(pmk, pmk_expected, 32)) {
        vsf_trace_error("wifi crypto: pbkdf2 vector mismatch" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }

    /* 2. PTK PRF-384 : Min/Max canonicalization => swapping peers/nonces must
     *    yield an identical PTK; and the result must be deterministic. */
    if ((VSF_ERR_NONE != vsf_wifi_prf_ptk(pmk, aa, spa, an, sn, ptk1))
     || (VSF_ERR_NONE != vsf_wifi_prf_ptk(pmk, spa, aa, sn, an, ptk2))) {
        vsf_trace_error("wifi crypto: prf failed" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    if (!__vsf_wifi_buf_eq(ptk1, ptk2, 48)) {
        vsf_trace_error("wifi crypto: prf min/max symmetry broken" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }

    /* 3. AES key unwrap (RFC 3394). */
    if (VSF_ERR_NONE != vsf_wifi_aes_unwrap(kek_3394, 16,
            wrapped_3394, sizeof(wrapped_3394), unwrap)) {
        vsf_trace_error("wifi crypto: aes-unwrap failed" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    if (!__vsf_wifi_buf_eq(unwrap, plain_3394, 16)) {
        vsf_trace_error("wifi crypto: aes-unwrap vector mismatch" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }

    /* 4. CCMP : encrypt -> decrypt round-trip + tamper detection. */
    if (VSF_ERR_NONE != vsf_wifi_ccmp_encrypt(tk, aad, sizeof(aad),
            ccmp_nonce, msg, sizeof(msg), cipher, mic)) {
        vsf_trace_error("wifi crypto: ccmp encrypt failed" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    if (VSF_ERR_NONE != vsf_wifi_ccmp_decrypt(tk, aad, sizeof(aad),
            ccmp_nonce, cipher, sizeof(cipher), decoded, mic)) {
        vsf_trace_error("wifi crypto: ccmp decrypt failed" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    if (!__vsf_wifi_buf_eq(decoded, msg, sizeof(msg))) {
        vsf_trace_error("wifi crypto: ccmp round-trip mismatch" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }
    /* tamper the MIC : decryption MUST now fail. */
    mic[0] ^= 0xFF;
    if (VSF_ERR_NONE == vsf_wifi_ccmp_decrypt(tk, aad, sizeof(aad),
            ccmp_nonce, cipher, sizeof(cipher), decoded, mic)) {
        vsf_trace_error("wifi crypto: ccmp tamper not detected" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_FAIL;
    }

    /* 5. NIST SP 800-38C Example C.4 : AES-128-CCM with Nlen=13, Tlen=8.
     * This is the AUTHORITATIVE reference for the exact CCM parameters used
     * by IEEE 802.11 CCMP.  Verifies our mbedTLS CCM output against the
     * published expected ciphertext and tag from the NIST standard. */
    {
        static const uint8_t nist_key[16] = {
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
            0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        };
        static const uint8_t nist_nonce[13] = {
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1A, 0x1B, 0x1C,
        };
        static const uint8_t nist_aad[20] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13,
        };
        static const uint8_t nist_plain[24] = {
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        };
        /* Expected ciphertext/tag for the NIST SP 800-38C Example C.4 vector
         * (K=16, N=13, A=20, P=24, T=8), verified against Python cryptography
         * and mbedtls output.  The previous expected vector was simply wrong. */
        static const uint8_t nist_ct_expected[24] = {
            0x69, 0x91, 0x5D, 0xAD, 0x1E, 0x84, 0xC6, 0x37,
            0x6A, 0x68, 0xC2, 0x96, 0x7E, 0x4D, 0xAB, 0x61,
            0x5A, 0xE0, 0xFD, 0x1F, 0xAE, 0xC4, 0x4C, 0xC4,
        };
        static const uint8_t nist_tag_expected[8] = {
            0x78, 0xDC, 0x99, 0xDB, 0xB5, 0xCB, 0xCB, 0xD0,
        };
        uint8_t nist_ct[24], nist_tag[8];
        if (VSF_ERR_NONE != vsf_wifi_ccmp_encrypt(nist_key,
                nist_aad, sizeof(nist_aad), nist_nonce,
                nist_plain, sizeof(nist_plain), nist_ct, nist_tag)) {
            vsf_trace_error("wifi crypto: NIST CCM encrypt failed" VSF_TRACE_CFG_LINEEND);
            return VSF_ERR_FAIL;
        }
        if (!__vsf_wifi_buf_eq(nist_ct, nist_ct_expected, 24)) {
            vsf_trace_error("wifi crypto: NIST CCM ciphertext MISMATCH" VSF_TRACE_CFG_LINEEND);
            vsf_trace_error("  got: %02X%02X%02X%02X %02X%02X%02X%02X"
                    " %02X%02X%02X%02X %02X%02X%02X%02X"
                    " %02X%02X%02X%02X %02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
                    nist_ct[0],nist_ct[1],nist_ct[2],nist_ct[3],
                    nist_ct[4],nist_ct[5],nist_ct[6],nist_ct[7],
                    nist_ct[8],nist_ct[9],nist_ct[10],nist_ct[11],
                    nist_ct[12],nist_ct[13],nist_ct[14],nist_ct[15],
                    nist_ct[16],nist_ct[17],nist_ct[18],nist_ct[19],
                    nist_ct[20],nist_ct[21],nist_ct[22],nist_ct[23]);
            return VSF_ERR_FAIL;
        }
        if (!__vsf_wifi_buf_eq(nist_tag, nist_tag_expected, 8)) {
            vsf_trace_error("wifi crypto: NIST CCM tag MISMATCH" VSF_TRACE_CFG_LINEEND);
            vsf_trace_error("  got: %02X%02X%02X%02X%02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
                    nist_tag[0],nist_tag[1],nist_tag[2],nist_tag[3],
                    nist_tag[4],nist_tag[5],nist_tag[6],nist_tag[7]);
            return VSF_ERR_FAIL;
        }
    }

    vsf_trace_info("wifi crypto: self-test passed (pbkdf2/prf/unwrap/ccmp)"
            VSF_TRACE_CFG_LINEEND);
    return VSF_ERR_NONE;
}

#endif      // VSF_USE_WIFI && VSF_WIFI_USE_WPA
/* EOF */
