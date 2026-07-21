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

#include "./vsf_wifi.h"

#if VSF_USE_WIFI == ENABLED && VSF_WIFI_USE_WPA == ENABLED

#include "./vsf_wifi_priv.h"
#include "./vsf_wifi_wpa.h"
#include "./vsf_wifi_crypto.h"

#include <string.h>
#include <stdlib.h>

/*============================ MACROS ========================================*/

/* EAPOL-Key frame field offsets, relative to the 802.1X header (the pointer
 * handed to vsf_wifi_eapol_rx).  Layout: 802.1X(4) + key descriptor(95) +
 * key data(var).  Ref IEEE 802.11i / 802.1X-2004. */
#define EAPOL_TYPE_OFF          1       /* 802.1X packet type (3 = Key)      */
#define EAPOL_BODYLEN_OFF       2       /* 802.1X length (BE)                */
#define EK_DESCTYPE_OFF         4       /* 2 = RSN, 254 = WPA                */
#define EK_KEYINFO_OFF          5       /* key information (BE)             */
#define EK_KEYLEN_OFF           7       /* pairwise key length (BE)         */
#define EK_REPLAY_OFF           9       /* replay counter (8)              */
#define EK_NONCE_OFF            17      /* key nonce (32)                  */
#define EK_MIC_OFF              81      /* key MIC (16)                    */
#define EK_KEYDATALEN_OFF       97      /* key data length (BE)            */
#define EK_KEYDATA_OFF          99      /* key data                        */
#define EK_HDR_LEN              99      /* fixed length before key data    */

/* 802.1X packet type. */
#define EAPOL_TYPE_KEY          0x03

/* Key Information bit fields. */
#define KI_VERSION_MASK         0x0007  /* 2 = HMAC-SHA1-128 / AES (CCMP)  */
#define KI_KEYTYPE_PAIRWISE     0x0008
#define KI_INSTALL              0x0040
#define KI_ACK                  0x0080
#define KI_MIC                  0x0100
#define KI_SECURE               0x0200
#define KI_ENCRYPTED            0x1000

#define KI_VERSION_AES          2

/* EAPOL-Key messages we transmit (key descriptor version 2). */
#define KI_M2   (KI_VERSION_AES | KI_KEYTYPE_PAIRWISE | KI_MIC)
#define KI_M4   (KI_VERSION_AES | KI_KEYTYPE_PAIRWISE | KI_MIC | KI_SECURE)
/* Group Key Handshake reply (G2): group key type (pairwise bit clear),
 * MIC + secure set, no key data. */
#define KI_G2   (KI_VERSION_AES | KI_MIC | KI_SECURE)

/* Re-arm the handshake timer after each step we answer.
 * 5 GHz links sometimes need more than 1 s for the AP to respond after M2,
 * especially when the 802.11 MAC layer retransmits M2 several times.
 * Use a generous timeout so late-but-valid M3/G1 frames are not dropped. */
#define __WPA_STEP_TIMEOUT_MS   2500

/* Working buffer cap for a TX EAPOL frame / an RX MIC recompute / key-data
 * unwrap.  An EAPOL handshake frame is well under this. */
#define __WPA_BUF_MAX           256

/*============================ LOCAL FUNCTIONS ===============================*/

static uint16_t __wpa_rd16be(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static void __wpa_wr16be(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)(v & 0xFF);
}

/* Fill a 32-byte SNonce.  rand() is not cryptographically strong, but the
 * handshake only needs a value that does not repeat across attempts; mixing
 * in the MAC, the ANonce and a monotonic counter keeps successive SNonces
 * distinct. */
static void __wpa_gen_snonce(vsf_wifi_t *wifi, uint8_t out[32])
{
    static uint32_t ctr = 0;
    int i;
    ctr++;
    for (i = 0; i < 32; i++) {
        out[i] = (uint8_t)((unsigned)rand()
                ^ (ctr >> (i & 7))
                ^ wifi->mac[i % 6]
                ^ wifi->wpa_anonce[i]);
    }
}

/* Build and transmit an EAPOL-Key frame as a ToDS 802.11 data frame:
 *   802.11 data hdr(24) + LLC/SNAP+0x888E(8) + EAPOL-Key(99 + key_data).
 * The MIC is computed over the EAPOL bytes with the MIC field zeroed, using
 * KCK = PTK[0:16]. */
static vsf_err_t __wpa_send_eapol(vsf_wifi_t *wifi, uint8_t ver,
        uint16_t key_info, uint16_t key_len,
        const uint8_t *nonce, const uint8_t *key_data, uint16_t key_data_len)
{
    static const uint8_t snap[8] = {
        0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x88, 0x8E
    };
    uint8_t  buf[__WPA_BUF_MAX];
    uint16_t eapol_len = (uint16_t)(EK_HDR_LEN + key_data_len);
    uint16_t total     = (uint16_t)(26 + 8 + eapol_len);

    if (total > sizeof(buf)) return VSF_ERR_FAIL;
    memset(buf, 0, total);

    /* 802.11 QoS data header (ToDS): addr1=BSSID(RA), addr2=our MAC(TA),
     * addr3=BSSID(DA).  Use QoS Data (subtype 8, FC=0x8801) instead of plain
     * Data because the AP negotiated WMM and may reject non-QoS frames. */
    buf[0] = 0x88;  buf[1] = 0x01;
    memcpy(&buf[4],  wifi->mlme_bssid, 6);
    memcpy(&buf[10], wifi->mac,        6);
    memcpy(&buf[16], wifi->mlme_bssid, 6);
    /* bytes 22-23: seq control (hardware fills) */
    /* bytes 24-25: QoS Control = 0x0000 (TID 0, BE).
     * The AP sends EAPOL-Key M1 with TID 0; mirror that for M2/M4 so the
     * frame is not classified into an AC that the AP may drop before the
     * 4-way handshake completes. */
    buf[24] = 0x00; buf[25] = 0x00;

    /* LLC/SNAP + EAPOL ethertype. */
    memcpy(&buf[26], snap, 8);

    /* EAPOL-Key. */
    uint8_t *ek = &buf[34];
    ek[0] = ver;
    ek[EAPOL_TYPE_OFF] = EAPOL_TYPE_KEY;
    __wpa_wr16be(&ek[EAPOL_BODYLEN_OFF], (uint16_t)(eapol_len - 4));
    ek[EK_DESCTYPE_OFF] = 0x02;             /* RSN key descriptor          */
    __wpa_wr16be(&ek[EK_KEYINFO_OFF], key_info);
    __wpa_wr16be(&ek[EK_KEYLEN_OFF],  key_len);
    memcpy(&ek[EK_REPLAY_OFF], wifi->wpa_replay, 8);
    if (nonce != NULL) {
        memcpy(&ek[EK_NONCE_OFF], nonce, 32);
    }
    /* IV / RSC / Key ID / MIC already zeroed by memset. */
    __wpa_wr16be(&ek[EK_KEYDATALEN_OFF], key_data_len);
    if ((key_data != NULL) && (key_data_len > 0)) {
        memcpy(&ek[EK_KEYDATA_OFF], key_data, key_data_len);
    }

    /* MIC over the EAPOL bytes (MIC field currently zero), KCK = PTK[0:16]. */
    if (key_info & KI_MIC) {
        uint8_t mic[16];
        if (vsf_wifi_eapol_mic(wifi->wpa_ptk, ek, eapol_len, mic)
                != VSF_ERR_NONE) {
            return VSF_ERR_FAIL;
        }
        memcpy(&ek[EK_MIC_OFF], mic, 16);
    }

    return vsf_wifi_mlme_tx(wifi, buf, total);
}

/* Extract the GTK from the (already decrypted) key-data KDE list of M3.
 * GTK KDE = 0xDD len 00-0F-AC 01 [keyid/tx][rsvd][GTK...].
 *
 * The key data is a sequence of IEs (type-length-value).  We must parse
 * them properly: skip each IE by (2 + length) bytes.  The previous code
 * did byte-scan (p++) for non-DD elements, which broke on any 0x00 byte
 * inside an RSN IE body (the OUI 00:0F:AC contains 0x00), causing the
 * parser to stop before reaching the GTK KDE.  This was the root cause
 * of GTK being all-zeros after handshake. */
static void __wpa_parse_gtk(vsf_wifi_t *wifi, const uint8_t *data, uint16_t len)
{
    const uint8_t *p   = data;
    const uint8_t *end = data + len;

    while (p + 2 <= end) {
        uint8_t t = p[0];
        uint8_t l = p[1];
        if (t == 0x00) break;                   /* padding byte            */
        if (p + 2 + l > end) break;             /* truncated IE            */
        if ((t == 0xDD) && (l >= 6) && (p[2] == 0x00) && (p[3] == 0x0F)
                && (p[4] == 0xAC) && (p[5] == 0x01)) {
            uint8_t glen = (uint8_t)(l - 6);
            if (glen > sizeof(wifi->wpa_gtk)) glen = sizeof(wifi->wpa_gtk);
            wifi->wpa_gtk_keyidx = (uint8_t)(p[6] & 0x03);
            memcpy(wifi->wpa_gtk, &p[8], glen);
            wifi->wpa_gtk_len = glen;
            vsf_wifi_trace_debug("wifi: GTK extracted keyidx=%u len=%u"
                    VSF_TRACE_CFG_LINEEND,
                    (unsigned)wifi->wpa_gtk_keyidx, (unsigned)glen);
            return;
        }
        p += 2 + l;                             /* skip entire IE          */
    }
    vsf_trace_warning("wifi: GTK KDE not found in key data (len=%u)"
            VSF_TRACE_CFG_LINEEND, (unsigned)len);
}

/* Handle EAPOL-Key M1 (ANonce, ACK, no MIC): derive the PTK and answer M2.
 * If this is a retransmission (same ANonce as already processed), just re-send
 * M2 with the existing SNonce/PTK to avoid a race where the AP already
 * validated our earlier M2 and is sending M3 using that PTK. */
static void __wpa_handle_m1(vsf_wifi_t *wifi, const uint8_t *ek,
        uint16_t key_len)
{
    vsf_wifi_trace_debug("wifi: 4-way M1 received, key_len=%u"
            VSF_TRACE_CFG_LINEEND, (unsigned)key_len);
    bool is_retransmit = (memcmp(&ek[EK_NONCE_OFF], wifi->wpa_anonce, 32) == 0)
                      && (wifi->wpa_snonce[0] | wifi->wpa_snonce[1]);

    memcpy(wifi->wpa_replay, &ek[EK_REPLAY_OFF], 8);

    if (!is_retransmit) {
        memcpy(wifi->wpa_anonce, &ek[EK_NONCE_OFF], 32);
        __wpa_gen_snonce(wifi, wifi->wpa_snonce);

        /* PTK = PRF(PMK, AA=BSSID, SPA=our MAC, ANonce, SNonce). */
        if (vsf_wifi_prf_ptk(wifi->wpa_auth.psk, wifi->mlme_bssid, wifi->mac,
                wifi->wpa_anonce, wifi->wpa_snonce, wifi->wpa_ptk)
                != VSF_ERR_NONE) {
            vsf_wifi_trace_info("wifi: PTK derivation failed"
                    VSF_TRACE_CFG_LINEEND);
            vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
            return;
        }
        vsf_wifi_trace_debug("wifi: PTK derived OK"
                VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_wifi_trace_debug("wifi: 4-way M1 retransmit, re-sending M2"
                VSF_TRACE_CFG_LINEEND);
    }

    /* M2: SNonce + our RSN IE in key data, MIC under the fresh KCK.
     * Send M2 once normally; if this is an M1 retransmit (AP did not receive
     * our first M2) the is_retransmit path above will be taken and we re-send
     * the same M2.  For the first M2, wait for either the AP's M3 or another
     * M1 retransmit.  Sending M2 back-to-back without waiting can keep us
     * transmitting while the AP is sending M3, causing us to miss M3. */
    vsf_err_t m2_err = __wpa_send_eapol(wifi, ek[0], KI_M2, key_len,
            wifi->wpa_snonce, wifi->wpa_rsn_ie, wifi->wpa_rsn_ie_len);
    if (m2_err != VSF_ERR_NONE) {
        vsf_trace_warning("wifi: EAPOL M2 tx failed err=%d"
                VSF_TRACE_CFG_LINEEND, (int)m2_err);
    } else {
        vsf_wifi_trace_debug("wifi: 4-way M2 sent (single)%s"
                VSF_TRACE_CFG_LINEEND,
                is_retransmit ? " [on M1 retx]" : "");
    }
    vsf_wifi_mlme_arm_timer(wifi, __WPA_STEP_TIMEOUT_MS);
}

/* Handle EAPOL-Key M3 (ANonce, ACK, MIC, install, encrypted GTK): verify
 * MIC, unwrap the GTK, answer M4 and bring the link up. */
static void __wpa_handle_m3(vsf_wifi_t *wifi, const uint8_t *ek,
        uint16_t key_info, uint16_t eapol_len)
{
    uint8_t  tmp[__WPA_BUF_MAX];
    uint8_t  calc_mic[16];
    uint8_t  rx_mic[16];

    if (eapol_len > sizeof(tmp)) {
        vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
        return;
    }

    /* MIC check: recompute over the frame with the MIC field zeroed. */
    memcpy(rx_mic, &ek[EK_MIC_OFF], 16);
    memcpy(tmp, ek, eapol_len);
    memset(&tmp[EK_MIC_OFF], 0, 16);
    if (vsf_wifi_eapol_mic(wifi->wpa_ptk, tmp, eapol_len, calc_mic)
            != VSF_ERR_NONE) {
        vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
        return;
    }
    if (memcmp(calc_mic, rx_mic, 16) != 0) {
        vsf_trace_warning("wifi: 4-way M3 MIC mismatch" VSF_TRACE_CFG_LINEEND);
        vsf_trace_warning("  KCK: %02X%02X%02X%02X %02X%02X%02X%02X "
                "%02X%02X%02X%02X %02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
                wifi->wpa_ptk[0], wifi->wpa_ptk[1], wifi->wpa_ptk[2], wifi->wpa_ptk[3],
                wifi->wpa_ptk[4], wifi->wpa_ptk[5], wifi->wpa_ptk[6], wifi->wpa_ptk[7],
                wifi->wpa_ptk[8], wifi->wpa_ptk[9], wifi->wpa_ptk[10], wifi->wpa_ptk[11],
                wifi->wpa_ptk[12], wifi->wpa_ptk[13], wifi->wpa_ptk[14], wifi->wpa_ptk[15]);
        vsf_trace_warning("  rx_mic : %02X%02X%02X%02X %02X%02X%02X%02X "
                "%02X%02X%02X%02X %02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
                rx_mic[0], rx_mic[1], rx_mic[2], rx_mic[3],
                rx_mic[4], rx_mic[5], rx_mic[6], rx_mic[7],
                rx_mic[8], rx_mic[9], rx_mic[10], rx_mic[11],
                rx_mic[12], rx_mic[13], rx_mic[14], rx_mic[15]);
        vsf_trace_warning("  calc_mic: %02X%02X%02X%02X %02X%02X%02X%02X "
                "%02X%02X%02X%02X %02X%02X%02X%02X" VSF_TRACE_CFG_LINEEND,
                calc_mic[0], calc_mic[1], calc_mic[2], calc_mic[3],
                calc_mic[4], calc_mic[5], calc_mic[6], calc_mic[7],
                calc_mic[8], calc_mic[9], calc_mic[10], calc_mic[11],
                calc_mic[12], calc_mic[13], calc_mic[14], calc_mic[15]);
        vsf_trace_warning("  AA=%02X:%02X:%02X:%02X:%02X:%02X "
                "SPA=%02X:%02X:%02X:%02X:%02X:%02X" VSF_TRACE_CFG_LINEEND,
                wifi->mlme_bssid[0], wifi->mlme_bssid[1], wifi->mlme_bssid[2],
                wifi->mlme_bssid[3], wifi->mlme_bssid[4], wifi->mlme_bssid[5],
                wifi->mac[0], wifi->mac[1], wifi->mac[2],
                wifi->mac[3], wifi->mac[4], wifi->mac[5]);
        vsf_trace_warning("  eapol_len=%u" VSF_TRACE_CFG_LINEEND, eapol_len);
        vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_MIC_FAILURE);
        return;
    }

    /* ANonce must match the value bound into the PTK at M1. */
    if (memcmp(&ek[EK_NONCE_OFF], wifi->wpa_anonce, 32) != 0) {
        vsf_trace_warning("wifi: 4-way M3 ANonce mismatch" VSF_TRACE_CFG_LINEEND);
        vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
        return;
    }

    /* Recover the GTK from the key data (AES key-unwrapped with KEK). */
    uint16_t kdl = __wpa_rd16be(&ek[EK_KEYDATALEN_OFF]);
    if (kdl > 0) {
        uint8_t  kd[__WPA_BUF_MAX];
        uint16_t out_len;
        if (key_info & KI_ENCRYPTED) {
            if ((kdl < 24) || (kdl % 8) || ((uint16_t)(kdl - 8) > sizeof(kd))) {
                vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
                return;
            }
            if (vsf_wifi_aes_unwrap(&wifi->wpa_ptk[16], VSF_WIFI_KEK_LEN,
                    &ek[EK_KEYDATA_OFF], kdl, kd) != VSF_ERR_NONE) {
                vsf_trace_warning("wifi: 4-way GTK unwrap failed"
                        VSF_TRACE_CFG_LINEEND);
                vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
                return;
            }
            out_len = (uint16_t)(kdl - 8);
        } else {
            if (kdl > sizeof(kd)) {
                vsf_wifi_mlme_handshake_fail(wifi, WIFI_REASON_UNSPECIFIED);
                return;
            }
            memcpy(kd, &ek[EK_KEYDATA_OFF], kdl);
            out_len = kdl;
        }
        __wpa_parse_gtk(wifi, kd, out_len);
    }

    /* Echo M3's replay counter in M4. */
    memcpy(wifi->wpa_replay, &ek[EK_REPLAY_OFF], 8);

    /* M4: no key data, MIC + secure set.
     * Send M4 multiple times to improve reliability: without hardware ACK
     * the AP may not receive a single transmission.  In tests, M2 required
     * ~4 attempts before the AP acknowledged; M4 is sent only once by the
     * standard flow, giving ~75% chance of failure.  Redundant M4s are
     * harmless (AP ignores duplicates once PTK is installed). */
    for (int __m4_i = 0; __m4_i < 3; __m4_i++) {
        if (__wpa_send_eapol(wifi, ek[0], KI_M4, 0, NULL, NULL, 0)
                != VSF_ERR_NONE) {
            vsf_trace_warning("wifi: EAPOL M4 tx[%d] failed" VSF_TRACE_CFG_LINEEND,
                    __m4_i);
        }
    }

    /* PTK/GTK derived; the cipher backend installation lands in Task 5.  Mark
     * the keys valid and bring the link up. */
    wifi->wpa_ptk_valid = true;
    vsf_wifi_trace_debug("wifi: 4-way M3 rx, M4 sent (keys ready)"
            VSF_TRACE_CFG_LINEEND);
    vsf_wifi_mlme_handshake_done(wifi);
}

/* Group Key Handshake message 1 (GTK rekey, IEEE 802.11i Sec 8.5.4).
 *
 * Once the link is up the AP periodically refreshes the group key.  It sends
 * a single EAPOL-Key (group type, MIC + secure set, ACK set, encrypted GTK
 * KDE in the key data) and expects a Group message 2 (G2) confirmation.  We
 * reuse the 4-way KCK/KEK already installed in wpa_ptk: verify the MIC with
 * KCK, AES-unwrap the new GTK with KEK, install it, then echo G2.  The link
 * stays in WIFI_MLME_RUN throughout. */
static void __wpa_handle_group_m1(vsf_wifi_t *wifi, const uint8_t *ek,
        uint16_t key_info, uint16_t eapol_len)
{
    uint8_t  tmp[__WPA_BUF_MAX];
    uint8_t  calc_mic[16];
    uint8_t  rx_mic[16];

    /* Only meaningful after the pairwise keys are installed. */
    if (!wifi->wpa_ptk_valid) return;
    if (eapol_len > sizeof(tmp)) return;

    /* MIC check over the frame with the MIC field zeroed (KCK = PTK[0:16]). */
    memcpy(rx_mic, &ek[EK_MIC_OFF], 16);
    memcpy(tmp, ek, eapol_len);
    memset(&tmp[EK_MIC_OFF], 0, 16);
    if (vsf_wifi_eapol_mic(wifi->wpa_ptk, tmp, eapol_len, calc_mic)
            != VSF_ERR_NONE) {
        return;
    }
    if (memcmp(calc_mic, rx_mic, 16) != 0) {
        vsf_trace_warning("wifi: group rekey MIC mismatch" VSF_TRACE_CFG_LINEEND);
        return;
    }

    /* Recover the new GTK from the key data (AES key-unwrapped with KEK). */
    uint16_t kdl = __wpa_rd16be(&ek[EK_KEYDATALEN_OFF]);
    if (kdl > 0) {
        uint8_t  kd[__WPA_BUF_MAX];
        uint16_t out_len;
        if (key_info & KI_ENCRYPTED) {
            if ((kdl < 24) || (kdl % 8) || ((uint16_t)(kdl - 8) > sizeof(kd))) {
                return;
            }
            if (vsf_wifi_aes_unwrap(&wifi->wpa_ptk[16], VSF_WIFI_KEK_LEN,
                    &ek[EK_KEYDATA_OFF], kdl, kd) != VSF_ERR_NONE) {
                vsf_trace_warning("wifi: group rekey GTK unwrap failed"
                        VSF_TRACE_CFG_LINEEND);
                return;
            }
            out_len = (uint16_t)(kdl - 8);
        } else {
            if (kdl > sizeof(kd)) return;
            memcpy(kd, &ek[EK_KEYDATA_OFF], kdl);
            out_len = kdl;
        }
        /* Updates wifi->wpa_gtk / wpa_gtk_len / wpa_gtk_keyidx in place; the
         * software CCMP RX path picks up the new GTK immediately. */
        __wpa_parse_gtk(wifi, kd, out_len);
    }

    /* If a hardware crypto backend owns the keys, re-program the new GTK.
     * The install is asynchronous; G2 is sent immediately so the AP does not
     * retransmit group M1.  A transient mismatch means a few broadcast frames
     * may be dropped until the key script finishes. */
    if (wifi->wpa_hw_crypto && (wifi->wpa_gtk_len > 0)
            && (wifi->drv != NULL) && (wifi->drv->crypto_ops != NULL)
            && (wifi->drv->crypto_ops->install_key != NULL)) {
        vsf_err_t err = wifi->drv->crypto_ops->install_key(wifi,
                wifi->wpa_gtk_keyidx, false, wifi->wpa_gtk,
                wifi->wpa_gtk_len, NULL, NULL);
        if (err != VSF_ERR_NONE) {
            vsf_wifi_trace_info(
                    "wifi: hardware GTK rekey install failed (err=%d)"
                    VSF_TRACE_CFG_LINEEND, (int)err);
        }
    }

    /* Echo the AP's replay counter in G2. */
    memcpy(wifi->wpa_replay, &ek[EK_REPLAY_OFF], 8);

    /* G2: group key type, MIC + secure, no key data.  The Key ID from the
     * group M1 is carried back in the key-info field per the spec. */
    uint16_t g2_info = (uint16_t)(KI_G2 | (key_info & (3 << 4)));
    if (__wpa_send_eapol(wifi, ek[0], g2_info, 0, NULL, NULL, 0)
            != VSF_ERR_NONE) {
        vsf_trace_warning("wifi: group G2 tx failed" VSF_TRACE_CFG_LINEEND);
    }
    vsf_trace_info("wifi: GTK rekey done, G2 sent (keyidx=%u)"
            VSF_TRACE_CFG_LINEEND, (unsigned)wifi->wpa_gtk_keyidx);
}

/*============================ IMPLEMENTATION ================================*/

void vsf_wifi_eapol_rx(vsf_wifi_t *wifi, const uint8_t *eapol, uint16_t len)
{
    vsf_wifi_trace_debug(
        "wifi: eapol_rx len=%u state=%u type=%u desc=%u key_info=0x%04X"
        VSF_TRACE_CFG_LINEEND,
        (unsigned)len, (unsigned)wifi->mlme_state,
        (unsigned)eapol[EAPOL_TYPE_OFF], (unsigned)eapol[EK_DESCTYPE_OFF],
        (unsigned)__wpa_rd16be(&eapol[EK_KEYINFO_OFF]));
    if ((wifi == NULL) || (eapol == NULL)) return;
    /* Accept EAPOL both during the 4-way handshake and when already connected.
     * In RUN state this handles AP M3 retries (AP didn't receive our M4):
     * we simply resend M4 without disrupting the existing link. */
    if ((wifi->mlme_state != WIFI_MLME_4WAY)
            && (wifi->mlme_state != WIFI_MLME_RUN)) return;
    if (len < EK_HDR_LEN) return;

    /* Must be an EAPOL-Key frame with an RSN key descriptor. */
    if (eapol[EAPOL_TYPE_OFF] != EAPOL_TYPE_KEY) return;
    if (eapol[EK_DESCTYPE_OFF] != 0x02) return;         /* RSN only          */

    uint16_t key_info = __wpa_rd16be(&eapol[EK_KEYINFO_OFF]);
    uint16_t key_len  = __wpa_rd16be(&eapol[EK_KEYLEN_OFF]);

    /* Key descriptor version 2 (AES / HMAC-SHA1-128) is the only one we do. */
    if ((key_info & KI_VERSION_MASK) != KI_VERSION_AES) return;
    /* Only EAPOL-Key messages from the AP (ACK set) drive us. */
    if (!(key_info & KI_ACK)) return;

    /* Bound the EAPOL length by the 802.1X body length field. */
    uint16_t eapol_len = (uint16_t)(4 + __wpa_rd16be(&eapol[EAPOL_BODYLEN_OFF]));
    if (eapol_len > len) eapol_len = len;
    if (eapol_len < EK_HDR_LEN) return;

    if (key_info & KI_KEYTYPE_PAIRWISE) {
        /* Pairwise EAPOL-Key: the 4-way handshake. */
        if (key_info & KI_MIC) {
            __wpa_handle_m3(wifi, eapol, key_info, eapol_len);
        } else {
            __wpa_handle_m1(wifi, eapol, key_len);
        }
    } else {
        /* Group EAPOL-Key: GTK rekey.  Only valid on an established link
         * (RUN state with keys installed) and carries a MIC. */
        if ((wifi->mlme_state == WIFI_MLME_RUN) && (key_info & KI_MIC)) {
            __wpa_handle_group_m1(wifi, eapol, key_info, eapol_len);
        }
    }
}

#endif // VSF_USE_WIFI && VSF_WIFI_USE_WPA
