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

#if VSF_USE_MBEDTLS == ENABLED

#define __VSF_MBEDTLS_TLS_SESSION_CLASS_IMPLEMENT
#include "./mbedtls_tls_session.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

//#define mbedtls_trace(...)            printf(__VA_ARGS__)
#ifndef mbedtls_trace
#   define mbedtls_trace(...)
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void mbedtls_session_cleanup(mbedtls_session_t *session)
{
    mbedtls_net_free(&session->server_fd);
    mbedtls_x509_crt_free(&session->cacert);
    mbedtls_ssl_free(&session->ssl);
    mbedtls_ssl_config_free(&session->conf);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_entropy_free(&session->entropy);
}

int mbedtls_session_write(mbedtls_session_t *session, uint8_t *buf, uint16_t len)
{
    int ret, result = 0;
    mbedtls_trace("  > Write to server:");
    while (len > 0) {
        ret = mbedtls_ssl_write(&session->ssl, buf, len);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }
        if (ret < 0) {
            mbedtls_trace(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            return -1;
        }
        buf += ret;
        len -= ret;
        result += ret;
    }
    mbedtls_trace(" %d bytes written\n\n", result);
    return result;
}

int mbedtls_session_read(mbedtls_session_t *session, uint8_t *buf, uint16_t len)
{
    int ret, result = 0;
    mbedtls_trace("  < Read from server:");
    while (len > 0) {
        ret = mbedtls_ssl_read(&session->ssl, buf, len);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY || ret == 0) {
            break;
        }
        if (ret < 0) {
            mbedtls_trace("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            return ret;
        }
        buf += ret;
        len -= ret;
        result += ret;
    }
    mbedtls_trace(" %d bytes read\n\n", result);
    return result;
}

void mbedtls_session_close(mbedtls_session_t *session)
{
    mbedtls_ssl_close_notify(&session->ssl);
    mbedtls_session_cleanup(session);
}

int mbedtls_session_connect(mbedtls_session_t *session, const char *host, const char *port)
{
    const unsigned char *cert = session->cert != NULL ? session->cert : (const unsigned char *)mbedtls_test_cas_pem;
    size_t cert_len = session->cert != NULL ? session->cert_len : mbedtls_test_cas_pem_len;
    int ret;

    mbedtls_ctr_drbg_init(&session->ctr_drbg);
    mbedtls_entropy_init(&session->entropy);
    mbedtls_trace("\n  . Seeding the random number generator...");
    ret = mbedtls_ctr_drbg_seed(&session->ctr_drbg, mbedtls_entropy_func, &session->entropy, NULL, 0);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto free_entropy_and_fail;
    }
    mbedtls_trace(" ok\n");

    mbedtls_trace("  . Loading the CA root certificate ...");
    mbedtls_x509_crt_init(&session->cacert);
    ret = mbedtls_x509_crt_parse(&session->cacert, cert, cert_len);
    if (ret < 0) {
        mbedtls_trace(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
        goto free_cert_and_fail;
    }
    mbedtls_trace(" ok (%d skipped)\n", ret);

    mbedtls_trace("  . Connecting to tcp/%s/%s...", host, port);
    mbedtls_net_init(&session->server_fd);
    if ((ret = mbedtls_net_connect(&session->server_fd, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto free_server_fd_and_fail;
    }
    mbedtls_trace(" ok\n");

    mbedtls_trace("  . Setting up the SSL/TLS structure...");
    mbedtls_ssl_config_init(&session->conf);
    if ((ret = mbedtls_ssl_config_defaults(&session->conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto free_conf_and_fail;
    }
    mbedtls_trace(" ok\n");

    mbedtls_ssl_conf_authmode(&session->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&session->conf, &session->cacert, NULL);
    mbedtls_ssl_conf_rng(&session->conf, mbedtls_ctr_drbg_random, &session->ctr_drbg);

    mbedtls_ssl_init(&session->ssl);
    ret = mbedtls_ssl_setup(&session->ssl, &session->conf);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto free_ssl_and_fail;
    }
    ret = mbedtls_ssl_set_hostname(&session->ssl, host);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto free_ssl_and_fail;
    }
    mbedtls_ssl_set_bio(&session->ssl, &session->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    mbedtls_trace("  . Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&session->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_trace(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int)-ret);
            goto free_ssl_and_fail;
        }
    }
    mbedtls_trace(" ok\n");

    mbedtls_trace("  . Verifying peer X.509 certificate...");
    {
        uint32_t flags = mbedtls_ssl_get_verify_result(&session->ssl);
        if (flags != 0) {
            mbedtls_trace(" failed\n");

            char msg[256];
            mbedtls_x509_crt_verify_info(msg, sizeof(msg), "  ! ", flags);
            mbedtls_trace("%s\n", msg);
        } else {
            mbedtls_trace(" ok\n");
        }
    }

    return 0;

free_ssl_and_fail:
    mbedtls_ssl_free(&session->ssl);
free_conf_and_fail:
    mbedtls_ssl_config_free(&session->conf);
free_server_fd_and_fail:
    mbedtls_net_free(&session->server_fd);
free_cert_and_fail:
    mbedtls_x509_crt_free(&session->cacert);
free_entropy_and_fail:
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_entropy_free(&session->entropy);
    return -1;
}

#endif      // VSF_USE_MBEDTLS
