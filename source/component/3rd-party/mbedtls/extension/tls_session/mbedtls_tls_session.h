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
    )
    public_member(
        const unsigned char *cert;
        size_t cert_len;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void mbedtls_session_cleanup(mbedtls_session_t *session);
extern int mbedtls_session_write(mbedtls_session_t *session, uint8_t *buf, uint16_t len);
extern int mbedtls_session_read(mbedtls_session_t *session, uint8_t *buf, uint16_t len);
extern void mbedtls_session_close(mbedtls_session_t *session);
extern int mbedtls_session_connect(mbedtls_session_t *session, const char *host, const char *port);

#endif
