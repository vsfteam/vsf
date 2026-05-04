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
    mbedtls_x509_crt_free(&session->clicert);
    mbedtls_pk_free(&session->pkey);
    mbedtls_ssl_free(&session->ssl);
    mbedtls_ssl_config_free(&session->conf);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_entropy_free(&session->entropy);
}

int mbedtls_session_write(mbedtls_session_t *session, uint8_t *buf, size_t len)
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

int mbedtls_session_read(mbedtls_session_t *session, uint8_t *buf, size_t len)
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
    const char *sni = session->common_name != NULL ? session->common_name : host;
    int ret;

    // Initialise every mbedtls context up-front so that the unified `fail`
    // cleanup path can safely call *_free() on any of them regardless of how
    // far connect() progressed.
    mbedtls_ctr_drbg_init(&session->ctr_drbg);
    mbedtls_entropy_init(&session->entropy);
    mbedtls_x509_crt_init(&session->cacert);
    mbedtls_x509_crt_init(&session->clicert);
    mbedtls_pk_init(&session->pkey);
    mbedtls_net_init(&session->server_fd);
    mbedtls_ssl_config_init(&session->conf);
    mbedtls_ssl_init(&session->ssl);

    mbedtls_trace("\n  . Seeding the random number generator...");
    ret = mbedtls_ctr_drbg_seed(&session->ctr_drbg, mbedtls_entropy_func, &session->entropy, NULL, 0);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto fail;
    }
    mbedtls_trace(" ok\n");

    mbedtls_trace("  . Loading the CA root certificate ...");
    ret = mbedtls_x509_crt_parse(&session->cacert, cert, cert_len);
    if (ret < 0) {
        mbedtls_trace(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", (unsigned int)-ret);
        goto fail;
    }
    mbedtls_trace(" ok (%d skipped)\n", ret);

    // Client certificate / private key for mutual-TLS. Loaded only when the
    // caller supplied both cert and key (fallback: unauthenticated client,
    // original behaviour).
    if (session->client_cert != NULL && session->client_key != NULL) {
        mbedtls_trace("  . Loading the client certificate ...");
        ret = mbedtls_x509_crt_parse(&session->clicert, session->client_cert, session->client_cert_len);
        if (ret < 0) {
            mbedtls_trace(" failed\n  ! mbedtls_x509_crt_parse(client) returned -0x%x\n\n", (unsigned int)-ret);
            goto fail;
        }
        ret = mbedtls_pk_parse_key(&session->pkey,
                session->client_key, session->client_key_len,
                session->key_password, session->key_password_len);
        if (ret != 0) {
            mbedtls_trace(" failed\n  ! mbedtls_pk_parse_key returned -0x%x\n\n", (unsigned int)-ret);
            goto fail;
        }
        mbedtls_trace(" ok\n");
    }

    mbedtls_trace("  . Connecting to tcp/%s/%s...", host, port);
    if ((ret = mbedtls_net_connect(&session->server_fd, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto fail;
    }
    mbedtls_trace(" ok\n");

    mbedtls_trace("  . Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&session->conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto fail;
    }
    mbedtls_trace(" ok\n");

    // TLS version lock (tls_version == 0 => any, PRESET_DEFAULT, original behaviour).
    if (session->tls_version == 1) {
        mbedtls_ssl_conf_min_version(&session->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
        mbedtls_ssl_conf_max_version(&session->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
    }
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    else if (session->tls_version == 2) {
        mbedtls_ssl_conf_min_version(&session->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
        mbedtls_ssl_conf_max_version(&session->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_4);
    }
#endif

    // skip_cn_check == false => VERIFY_OPTIONAL (original behaviour).
    mbedtls_ssl_conf_authmode(&session->conf,
            session->skip_cn_check ? MBEDTLS_SSL_VERIFY_NONE : MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&session->conf, &session->cacert, NULL);
    mbedtls_ssl_conf_rng(&session->conf, mbedtls_ctr_drbg_random, &session->ctr_drbg);

    // Bind client certificate to the ssl_config if loaded above.
    if (session->client_cert != NULL && session->client_key != NULL) {
        ret = mbedtls_ssl_conf_own_cert(&session->conf, &session->clicert, &session->pkey);
        if (ret != 0) {
            mbedtls_trace(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
            goto fail;
        }
    }

    // ALPN (alpn_protos == NULL => not configured, original behaviour).
    if (session->alpn_protos != NULL) {
        ret = mbedtls_ssl_conf_alpn_protocols(&session->conf, session->alpn_protos);
        if (ret != 0) {
            mbedtls_trace(" failed\n  ! mbedtls_ssl_conf_alpn_protocols returned %d\n\n", ret);
            goto fail;
        }
    }

    // Read timeout (timeout_ms == 0 => blocking, original behaviour).
    if (session->timeout_ms > 0) {
        mbedtls_ssl_conf_read_timeout(&session->conf, (uint32_t)session->timeout_ms);
    }

    ret = mbedtls_ssl_setup(&session->ssl, &session->conf);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto fail;
    }
    ret = mbedtls_ssl_set_hostname(&session->ssl, sni);
    if (ret != 0) {
        mbedtls_trace(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto fail;
    }
    // bio recv: always use mbedtls_net_recv_timeout so that
    // mbedtls_ssl_conf_read_timeout() can be updated at runtime via
    // mbedtls_session_set_timeout(). When timeout_ms is 0 the underlying
    // select() is called with NULL, which is equivalent to a blocking recv.
    mbedtls_ssl_set_bio(&session->ssl, &session->server_fd,
            mbedtls_net_send, NULL, mbedtls_net_recv_timeout);

    mbedtls_trace("  . Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&session->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_trace(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int)-ret);
            goto fail;
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

fail:
    mbedtls_session_cleanup(session);
    return -1;
}

int mbedtls_session_get_fd(mbedtls_session_t *session)
{
    return session != NULL ? session->server_fd.fd : -1;
}

int mbedtls_session_set_timeout(mbedtls_session_t *session, int timeout_ms)
{
    if (NULL == session) {
        return -1;
    }
    session->timeout_ms = timeout_ms;
    // conf_read_timeout reads 0 as "no timeout". Writing the config is safe
    // both before and after ssl_setup(); mbedtls consults conf->read_timeout
    // on every ssl_read() call via the net_recv_timeout bio wired up in
    // mbedtls_session_connect().
    mbedtls_ssl_conf_read_timeout(&session->conf,
            (uint32_t)(timeout_ms > 0 ? timeout_ms : 0));
    return 0;
}

#endif      // VSF_USE_MBEDTLS
