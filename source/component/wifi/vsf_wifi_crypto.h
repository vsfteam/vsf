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

#ifndef __VSF_WIFI_CRYPTO_H__
#define __VSF_WIFI_CRYPTO_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_wifi_cfg.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_WPA == ENABLED

#include "kernel/vsf_kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/* WPA2-PSK / CCMP key material sizes (octets). */
#define VSF_WIFI_PMK_LEN        32      /* pairwise master key                 */
#define VSF_WIFI_PTK_LEN        48      /* CCMP PTK = KCK(16)+KEK(16)+TK(16)   */
#define VSF_WIFI_KCK_LEN        16      /* EAPOL-Key MIC key                   */
#define VSF_WIFI_KEK_LEN        16      /* EAPOL-Key encryption (GTK) key      */
#define VSF_WIFI_TK_LEN         16      /* CCMP temporal key                   */
#define VSF_WIFI_NONCE_LEN      32      /* ANonce / SNonce                     */
#define VSF_WIFI_MIC_LEN        16      /* EAPOL-Key MIC (HMAC-SHA1 truncated) */
#define VSF_WIFI_CCMP_NONCE_LEN 13      /* CCM nonce: flag(1)+A2(6)+PN(6)      */
#define VSF_WIFI_CCMP_MIC_LEN   8       /* CCMP MIC                            */

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \brief Derive the PMK from a passphrase using PBKDF2-HMAC-SHA1 (4096 rounds).
 *        salt = SSID, dkLen = 32.  (IEEE 802.11i WPA-PSK)
 */
extern vsf_err_t vsf_wifi_pbkdf2_pmk(const char *passphrase,
                const uint8_t *ssid, uint16_t ssid_len, uint8_t pmk[32]);

/**
 * \brief Derive the 384-bit PTK using PRF-384 (HMAC-SHA1).
 *        label = "Pairwise key expansion",
 *        data  = Min(AA,SPA)||Max(AA,SPA)||Min(ANonce,SNonce)||Max(ANonce,SNonce).
 *        ptk layout: KCK[0:16] | KEK[16:32] | TK[32:48].
 */
extern vsf_err_t vsf_wifi_prf_ptk(const uint8_t pmk[32],
                const uint8_t aa[6], const uint8_t spa[6],
                const uint8_t anonce[32], const uint8_t snonce[32],
                uint8_t ptk[48]);

/**
 * \brief Compute the EAPOL-Key MIC (key descriptor version 2 = HMAC-SHA1
 *        truncated to 16 bytes).  The caller must zero the MIC field of the
 *        EAPOL frame before calling.
 */
extern vsf_err_t vsf_wifi_eapol_mic(const uint8_t kck[16],
                const uint8_t *eapol, uint16_t eapol_len, uint8_t mic[16]);

/**
 * \brief AES key unwrap (RFC 3394), used to recover the GTK from EAPOL-Key M3.
 *        in_len must be a multiple of 8 and >= 24; out_len = in_len - 8.
 */
extern vsf_err_t vsf_wifi_aes_unwrap(const uint8_t *kek, uint16_t kek_len,
                const uint8_t *in, uint16_t in_len, uint8_t *out);

/**
 * \brief CCMP encrypt: AES-CCM with L=2, M=8.  Produces cipher (== plain_len)
 *        and an 8-byte MIC.  nonce = flag||A2||PN (13 bytes); aad = 802.11 AAD.
 */
extern vsf_err_t vsf_wifi_ccmp_encrypt(const uint8_t tk[16],
                const uint8_t *aad, uint16_t aad_len,
                const uint8_t nonce[13],
                const uint8_t *plain, uint16_t plain_len,
                uint8_t *cipher, uint8_t mic[8]);

/**
 * \brief CCMP decrypt + authenticate.  Returns VSF_ERR_FAIL if the MIC check
 *        fails (forged / corrupted frame).
 */
extern vsf_err_t vsf_wifi_ccmp_decrypt(const uint8_t tk[16],
                const uint8_t *aad, uint16_t aad_len,
                const uint8_t nonce[13],
                const uint8_t *cipher, uint16_t cipher_len,
                uint8_t *plain, const uint8_t mic[8]);

/**
 * \brief Run the built-in crypto self-test against IEEE 802.11i / RFC test
 *        vectors.  Returns VSF_ERR_NONE if all vectors pass.
 */
extern vsf_err_t vsf_wifi_crypto_selftest(void);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_WIFI && VSF_WIFI_USE_WPA
#endif      // __VSF_WIFI_CRYPTO_H__
/* EOF */
