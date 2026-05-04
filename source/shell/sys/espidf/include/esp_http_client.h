/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * Clean-room re-implementation of ESP-IDF public API "esp_http_client.h".
 *
 * This file is authored from the ESP-IDF public API reference only. No
 * code is copied or derived from the ESP-IDF source tree. Names, enum
 * values and function signatures are kept compatible so that unmodified
 * ESP-IDF applications can compile against this header.
 *
 * Baseline: ESP-IDF v5.x public API surface (42 public functions).
 *
 * Scope:
 *   Full Tier 1 + Tier 2 (config-macro gated) + Tier 3. A small number of
 *   Tier 3 APIs whose semantics cannot be mapped onto the current
 *   vsf_http_client transport contract are compiled as stubs returning
 *   ESP_ERR_NOT_SUPPORTED; this is documented per-function in the port.
 */

#ifndef __VSF_ESPIDF_ESP_HTTP_CLIENT_H__
#define __VSF_ESPIDF_ESP_HTTP_CLIENT_H__

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define DEFAULT_HTTP_BUF_SIZE                   (512)

// ESP-IDF esp_http_client error code range (0x7000 + n)
#define ESP_ERR_HTTP_BASE                       (0x7000)
#define ESP_ERR_HTTP_MAX_REDIRECT               (ESP_ERR_HTTP_BASE + 1)
#define ESP_ERR_HTTP_CONNECT                    (ESP_ERR_HTTP_BASE + 2)
#define ESP_ERR_HTTP_WRITE_DATA                 (ESP_ERR_HTTP_BASE + 3)
#define ESP_ERR_HTTP_FETCH_HEADER               (ESP_ERR_HTTP_BASE + 4)
#define ESP_ERR_HTTP_INVALID_TRANSPORT          (ESP_ERR_HTTP_BASE + 5)
#define ESP_ERR_HTTP_CONNECTING                 (ESP_ERR_HTTP_BASE + 6)
#define ESP_ERR_HTTP_EAGAIN                     (ESP_ERR_HTTP_BASE + 7)
#define ESP_ERR_HTTP_CONNECTION_CLOSED          (ESP_ERR_HTTP_BASE + 8)
#define ESP_ERR_HTTP_NOT_MODIFIED               (ESP_ERR_HTTP_BASE + 9)
#define ESP_ERR_HTTP_RANGE_NOT_SATISFIABLE      (ESP_ERR_HTTP_BASE + 10)
#define ESP_ERR_HTTP_READ_TIMEOUT               (ESP_ERR_HTTP_BASE + 11)
#define ESP_ERR_HTTP_INCOMPLETE_DATA            (ESP_ERR_HTTP_BASE + 12)

/*============================ TYPES =========================================*/

typedef struct esp_http_client        *esp_http_client_handle_t;
typedef struct esp_http_client_event  *esp_http_client_event_handle_t;

// Event ids delivered to http_event_handle_cb.
// HEADER_SENT kept as alias for legacy code (same value as HEADERS_SENT).
typedef enum {
    HTTP_EVENT_ERROR = 0,
    HTTP_EVENT_ON_CONNECTED,
    HTTP_EVENT_HEADERS_SENT,
    HTTP_EVENT_HEADER_SENT = HTTP_EVENT_HEADERS_SENT,
    HTTP_EVENT_ON_HEADER,
    HTTP_EVENT_ON_HEADERS_COMPLETE,
    HTTP_EVENT_ON_STATUS_CODE,
    HTTP_EVENT_ON_DATA,
    HTTP_EVENT_ON_FINISH,
    HTTP_EVENT_DISCONNECTED,
    HTTP_EVENT_REDIRECT,
} esp_http_client_event_id_t;

typedef struct esp_http_client_event {
    esp_http_client_event_id_t  event_id;
    esp_http_client_handle_t    client;
    void                       *data;
    int                         data_len;
    void                       *user_data;
    char                       *header_key;
    char                       *header_value;
} esp_http_client_event_t;

typedef struct esp_http_client_redirect_event_data {
    esp_http_client_handle_t    client;
    int                         status_code;
} esp_http_client_redirect_event_data_t;

// Transport selection (auto-selected from scheme when url is given).
typedef enum {
    HTTP_TRANSPORT_UNKNOWN = 0x0,
    HTTP_TRANSPORT_OVER_TCP,
    HTTP_TRANSPORT_OVER_SSL,
} esp_http_client_transport_t;

// TLS preferred protocol version. Forwarded to mbedtls_session when the
// underlying layer supports it; ignored otherwise.
typedef enum {
    ESP_HTTP_CLIENT_TLS_VER_ANY     = 0,
    ESP_HTTP_CLIENT_TLS_VER_TLS_1_2 = 0x1,
    ESP_HTTP_CLIENT_TLS_VER_TLS_1_3 = 0x2,
    ESP_HTTP_CLIENT_TLS_VER_MAX,
} esp_http_client_proto_ver_t;

typedef esp_err_t (*http_event_handle_cb)(esp_http_client_event_t *evt);

typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_NOTIFY,
    HTTP_METHOD_SUBSCRIBE,
    HTTP_METHOD_UNSUBSCRIBE,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_COPY,
    HTTP_METHOD_MOVE,
    HTTP_METHOD_LOCK,
    HTTP_METHOD_UNLOCK,
    HTTP_METHOD_PROPFIND,
    HTTP_METHOD_PROPPATCH,
    HTTP_METHOD_MKCOL,
    HTTP_METHOD_REPORT,
    HTTP_METHOD_MAX,
} esp_http_client_method_t;

typedef enum {
    HTTP_AUTH_TYPE_NONE = 0,
    HTTP_AUTH_TYPE_BASIC,
    HTTP_AUTH_TYPE_DIGEST,
} esp_http_client_auth_type_t;

// Socket address family. AF_UNSPEC=0, AF_INET=2, AF_INET6=10 on most
// POSIX systems including VSF linux layer; values replicated here so
// users do not need <sys/socket.h>.
typedef enum {
    HTTP_ADDR_TYPE_UNSPEC = 0,
    HTTP_ADDR_TYPE_INET   = 2,
    HTTP_ADDR_TYPE_INET6  = 10,
} esp_http_client_addr_type_t;

typedef enum {
    HTTP_STATE_UNINIT = 0,
    HTTP_STATE_INIT,
    HTTP_STATE_CONNECTING,
    HTTP_STATE_CONNECTED,
    HTTP_STATE_REQ_COMPLETE_HEADER,
    HTTP_STATE_REQ_COMPLETE_DATA,
    HTTP_STATE_RES_COMPLETE_HEADER,
    HTTP_STATE_RES_ON_DATA_START,
    HTTP_STATE_RES_COMPLETE_DATA,
    HTTP_STATE_CLOSE
} esp_http_state_t;

// Subset of HTTP status codes that ESP-IDF applications typically switch on.
typedef enum {
    HttpStatus_Ok                  = 200,
    HttpStatus_PartialContent      = 206,
    HttpStatus_MultipleChoices     = 300,
    HttpStatus_MovedPermanently    = 301,
    HttpStatus_Found               = 302,
    HttpStatus_SeeOther            = 303,
    HttpStatus_NotModified         = 304,
    HttpStatus_TemporaryRedirect   = 307,
    HttpStatus_PermanentRedirect   = 308,
    HttpStatus_BadRequest          = 400,
    HttpStatus_Unauthorized        = 401,
    HttpStatus_Forbidden           = 403,
    HttpStatus_NotFound            = 404,
    HttpStatus_RangeNotSatisfiable = 416,
    HttpStatus_InternalError       = 500
} HttpStatus_Code;

// esp_http_client_config_t: superset of fields used by ESP-IDF examples.
// Fields guarded by CONFIG_* on upstream are always present here; ones
// the shim does not honour are accepted and silently ignored so that
// unmodified example code still compiles and runs.
typedef struct {
    const char                      *url;
    const char                      *host;
    int                              port;
    const char                      *username;
    const char                      *password;
    esp_http_client_auth_type_t      auth_type;
    const char                      *path;
    const char                      *query;
    union {
        const char                  *cert_pem;
        const char                  *cert_der;
    };
    size_t                           cert_len;
    union {
        const char                  *client_cert_pem;
        const char                  *client_cert_der;
    };
    size_t                           client_cert_len;
    const char                      *client_key_pem;
    size_t                           client_key_len;
    const char                      *client_key_password;
    size_t                           client_key_password_len;
    esp_http_client_proto_ver_t      tls_version;
    const char                      *user_agent;
    esp_http_client_method_t         method;
    int                              timeout_ms;
    bool                             disable_auto_redirect;
    int                              max_redirection_count;
    int                              max_authorization_retries;
    http_event_handle_cb             event_handler;
    esp_http_client_transport_t      transport_type;
    int                              buffer_size;
    int                              buffer_size_tx;
    void                            *user_data;
    bool                             is_async;
    bool                             use_global_ca_store;
    bool                             skip_cert_common_name_check;
    const char                      *common_name;
    esp_err_t                      (*crt_bundle_attach)(void *conf);
    bool                             keep_alive_enable;
    int                              keep_alive_idle;
    int                              keep_alive_interval;
    int                              keep_alive_count;
    // if_name intentionally omitted (requires struct ifreq from lwip);
    // callers relying on it must bind at their own transport layer.
    const char                     **alpn_protos;
    esp_http_client_addr_type_t      addr_type;
} esp_http_client_config_t;

/*============================ PROTOTYPES ====================================*/

// ----- lifecycle ---------------------------------------------------------
extern esp_http_client_handle_t     esp_http_client_init(const esp_http_client_config_t *config);
extern esp_err_t                    esp_http_client_cleanup(esp_http_client_handle_t client);

// ----- request/response driving -----------------------------------------
extern esp_err_t                    esp_http_client_perform(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_prepare(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_open(esp_http_client_handle_t client, int write_len);
extern int                          esp_http_client_write(esp_http_client_handle_t client, const char *buffer, int len);
extern esp_err_t                    esp_http_client_request_send(esp_http_client_handle_t client, int write_len);
extern int                          esp_http_client_chunk_write_begin(esp_http_client_handle_t client, const int len);
extern int                          esp_http_client_chunk_write_end(esp_http_client_handle_t client, bool last_chunk);
extern int64_t                      esp_http_client_fetch_headers(esp_http_client_handle_t client);
extern int                          esp_http_client_read(esp_http_client_handle_t client, char *buffer, int len);
extern int                          esp_http_client_read_response(esp_http_client_handle_t client, char *buffer, int len);
extern esp_err_t                    esp_http_client_flush_response(esp_http_client_handle_t client, int *len);
extern esp_err_t                    esp_http_client_close(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_clear_response_buffer(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_cancel_request(esp_http_client_handle_t client);

// ----- URL / method / headers / post body --------------------------------
extern esp_err_t                    esp_http_client_set_url(esp_http_client_handle_t client, const char *url);
extern esp_err_t                    esp_http_client_get_url(esp_http_client_handle_t client, char *url, const int len);
extern esp_err_t                    esp_http_client_set_method(esp_http_client_handle_t client, esp_http_client_method_t method);
extern esp_err_t                    esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value);
extern esp_err_t                    esp_http_client_get_header(esp_http_client_handle_t client, const char *key, char **value);
extern esp_err_t                    esp_http_client_delete_header(esp_http_client_handle_t client, const char *key);
extern esp_err_t                    esp_http_client_delete_all_headers(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_get_response_header(esp_http_client_handle_t client, const char *key, char **value);
extern esp_err_t                    esp_http_client_set_post_field(esp_http_client_handle_t client, const char *data, int len);
extern int                          esp_http_client_get_post_field(esp_http_client_handle_t client, char **data);

// ----- auth --------------------------------------------------------------
extern esp_err_t                    esp_http_client_set_username(esp_http_client_handle_t client, const char *username);
extern esp_err_t                    esp_http_client_get_username(esp_http_client_handle_t client, char **value);
extern esp_err_t                    esp_http_client_set_password(esp_http_client_handle_t client, const char *password);
extern esp_err_t                    esp_http_client_get_password(esp_http_client_handle_t client, char **value);
extern esp_err_t                    esp_http_client_set_authtype(esp_http_client_handle_t client, esp_http_client_auth_type_t auth_type);
extern esp_err_t                    esp_http_client_add_auth(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_set_auth_data(esp_http_client_handle_t client, const char *auth_data, int len);

// ----- redirection -------------------------------------------------------
extern esp_err_t                    esp_http_client_set_redirection(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_reset_redirect_counter(esp_http_client_handle_t client);

// ----- misc getters ------------------------------------------------------
extern int                          esp_http_client_get_status_code(esp_http_client_handle_t client);
extern int64_t                      esp_http_client_get_content_length(esp_http_client_handle_t client);
extern int64_t                      esp_http_client_get_content_range(esp_http_client_handle_t client);
extern bool                         esp_http_client_is_chunked_response(esp_http_client_handle_t client);
extern bool                         esp_http_client_is_complete_data_received(esp_http_client_handle_t client);
extern bool                         esp_http_client_is_persistent_connection(esp_http_client_handle_t client);
extern esp_http_client_transport_t  esp_http_client_get_transport_type(esp_http_client_handle_t client);
extern esp_http_state_t             esp_http_client_get_state(esp_http_client_handle_t client);
extern int                          esp_http_client_get_socket(esp_http_client_handle_t client);
extern int                          esp_http_client_get_errno(esp_http_client_handle_t client);
extern esp_err_t                    esp_http_client_get_chunk_length(esp_http_client_handle_t client, int *len);
extern esp_err_t                    esp_http_client_get_and_clear_last_tls_error(esp_http_client_handle_t client, int *esp_tls_error_code, int *esp_tls_flags);

// ----- runtime setters ---------------------------------------------------
extern esp_err_t                    esp_http_client_set_timeout_ms(esp_http_client_handle_t client, int timeout_ms);
extern esp_err_t                    esp_http_client_set_user_data(esp_http_client_handle_t client, void *data);
extern esp_err_t                    esp_http_client_get_user_data(esp_http_client_handle_t client, void **data);

#ifdef __cplusplus
}
#endif

#endif // __VSF_ESPIDF_ESP_HTTP_CLIENT_H__
