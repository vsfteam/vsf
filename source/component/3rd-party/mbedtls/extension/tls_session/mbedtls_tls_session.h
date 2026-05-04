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

#ifndef __MBEDTLS_TLS_SESSION__
#define __MBEDTLS_TLS_SESSION__

/*============================ INCLUDES ======================================*/

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>

#if     defined(__VSF_MBEDTLS_TLS_SESSION_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_MBEDTLS_TLS_SESSION_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(mbedtls_session_t) {
    private_member(
        mbedtls_ssl_context ssl;
        mbedtls_ssl_config conf;
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_net_context server_fd;
        mbedtls_x509_crt cacert;
        mbedtls_x509_crt clicert;   // client certificate (used only when client_cert is set)
        mbedtls_pk_context pkey;    // client private key (used only when client_key is set)
    )
    public_member(
        // Server CA certificate. NULL = fall back to built-in mbedtls_test_cas_pem
        // (preserves original behaviour).
        const unsigned char *cert;
        size_t cert_len;

        // ---- Extensions (all zero-initialised => fall back to original behaviour) ----
        // Client certificate / private key for mutual-TLS. The pair must be set
        // together; either both NULL (no client auth, default) or both non-NULL.
        const unsigned char *client_cert;
        size_t client_cert_len;
        const unsigned char *client_key;
        size_t client_key_len;
        const unsigned char *key_password;  // optional, NULL if key is unencrypted
        size_t key_password_len;

        // TLS protocol version lock.
        //   0 = any (PRESET_DEFAULT, unchanged behaviour)
        //   1 = force TLS 1.2
        //   2 = force TLS 1.3 (only effective if MBEDTLS_SSL_PROTO_TLS1_3 is
        //                      enabled in config; ignored otherwise)
        int tls_version;

        // SNI / server-name override.
        //   NULL = use the `host` argument of mbedtls_session_connect()
        //          (original behaviour)
        const char *common_name;

        // When true, authmode becomes VERIFY_NONE; otherwise VERIFY_OPTIONAL
        // (original behaviour).
        bool skip_cn_check;

        // NULL-terminated array of ALPN protocol names, e.g. { "h2", "http/1.1", NULL }.
        // NULL = ALPN not configured (original behaviour).
        const char **alpn_protos;

        // Read timeout in milliseconds. 0 = blocking (original behaviour).
        // Non-zero: bio recv switches to mbedtls_net_recv_timeout().
        int timeout_ms;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void mbedtls_session_cleanup(mbedtls_session_t *session);
extern int mbedtls_session_write(mbedtls_session_t *session, uint8_t *buf, size_t len);
extern int mbedtls_session_read(mbedtls_session_t *session, uint8_t *buf, size_t len);
extern void mbedtls_session_close(mbedtls_session_t *session);
extern int mbedtls_session_connect(mbedtls_session_t *session, const char *host, const char *port);

// Update the read timeout of an existing session. Legal at any time:
//   - Before connect: stores into session->timeout_ms, applied at connect().
//   - After connect: additionally patches mbedtls_ssl_conf_read_timeout so
//     that the very next mbedtls_ssl_read() honours the new value (the bio
//     recv is mbedtls_net_recv_timeout which reads the timeout from conf on
//     every call).
// timeout_ms <= 0 means "no timeout" (blocking).
extern int mbedtls_session_set_timeout(mbedtls_session_t *session, int timeout_ms);

// Returns the underlying TCP socket fd. Valid only after a successful
// mbedtls_session_connect(); returns -1 when the session has not been
// connected (mbedtls_net_init() initialises fd to -1).
extern int mbedtls_session_get_fd(mbedtls_session_t *session);

#endif
