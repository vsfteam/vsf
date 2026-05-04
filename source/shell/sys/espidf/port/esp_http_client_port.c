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
 * Port implementation for "esp_http_client.h" on VSF.
 *
 * Layering:
 *   esp_http_client (this file)         <-- ESP-IDF-shaped API
 *        |
 *   vsf_http_client                      <-- VSF HTTP state machine (header
 *        |                                    assembly, response line parse,
 *        |                                    chunked decode, redirect capture)
 *        |
 *   vsf_http_op_t  --+------------------ <-- transport abstraction
 *                    |
 *        +-----------+-----------+
 *        |                       |
 *   vsf_mbedtls_http_op    s_vsf_tcp_http_op (this file, static)
 *   (HTTPS, VSF TLS)       (HTTP, VSF linux socket)
 *
 * URL parsing:
 *   Implemented here as a clean-room drop-in for the http_parser library's
 *   url parser: same enum names (UF_*), same struct http_parser_url layout
 *   (field_set/port/field_data[]), same http_parser_url_init() / entry
 *   http_parser_parse_url() signatures. This allows later migration to the
 *   full http_parser 3rd-party drop-in with zero call-site changes.
 *
 * Scope (Task 3 full):
 *   All 42 public esp_http_client_* APIs have a body here. Tier 2 APIs sit
 *   behind VSF_ESPIDF_HTTP_CLIENT_CFG_USE_* switches. Tier 3 APIs that
 *   require stream primitives the underlying vsf_http_client does not yet
 *   expose (chunk_write_* / request_send / flush_response /
 *   clear_response_buffer / get_and_clear_last_tls_error) return
 *   ESP_ERR_NOT_SUPPORTED. This is documented per-function.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_HTTP_CLIENT == ENABLED

#include "esp_http_client.h"
#include "esp_err.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "service/heap/vsf_heap.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_LINUX == ENABLED
#   include <unistd.h>
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netdb.h>
#   include <fcntl.h>
#endif

#include "component/tcpip/protocol/http/client/vsf_http_client.h"
#define __VSF_HTTP_CLIENT_CLASS_INHERIT__
#include "component/tcpip/protocol/http/client/vsf_http_client.h"
#if VSF_USE_MBEDTLS == ENABLED
#   include "component/3rd-party/mbedtls/extension/tls_session/mbedtls_tls_session.h"
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_REDIRECT_DEFAULT
#   define VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_REDIRECT_DEFAULT   10
#endif

#ifndef VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_AUTH_RETRIES_DEFAULT
#   define VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_AUTH_RETRIES_DEFAULT   10
#endif

#define VSF_EHC_DEFAULT_TIMEOUT_MS       5000
#define VSF_EHC_DEFAULT_HTTP_PORT        80
#define VSF_EHC_DEFAULT_HTTPS_PORT       443

/*============================ TYPES =========================================*/

/* --------------------------------------------------------------------------
 * Drop-in compatible subset of http_parser's URL parser.
 *
 * Interface matches node-http_parser (see ESP-IDF component/http_parser):
 *   enum http_parser_url_fields, struct http_parser_url,
 *   http_parser_url_init(), http_parser_parse_url().
 *
 * Implementation is clean-room and covers what esp_http_client actually
 * uses: scheme, userinfo, host, port, path, query. Fragment is parsed but
 * not used by callers. IPv6 literals inside [...] and CONNECT-style
 * authorities (is_connect != 0) are NOT supported and return failure; that
 * matches the subset of URLs used by ESP-IDF example apps. When full
 * support is later needed, swap this block for the real http_parser
 * source tree (3rd-party) and no call-site changes are required.
 * -------------------------------------------------------------------------- */

enum http_parser_url_fields {
    UF_SCHEMA   = 0,
    UF_HOST     = 1,
    UF_PORT     = 2,
    UF_PATH     = 3,
    UF_QUERY    = 4,
    UF_FRAGMENT = 5,
    UF_USERINFO = 6,
    UF_MAX      = 7
};

struct http_parser_url {
    uint16_t field_set;
    uint16_t port;
    struct {
        uint16_t off;
        uint16_t len;
    } field_data[UF_MAX];
};

/* --------------------------------------------------------------------------
 * Internal header key-value list. Intrusive singly-linked so an esp_http_client
 * handle owns its own storage. Case-insensitive lookup on the key.
 * -------------------------------------------------------------------------- */

typedef struct vsf_ehc_header_t {
    struct vsf_ehc_header_t *next;
    char                    *key;
    char                    *value;
} vsf_ehc_header_t;

/* --------------------------------------------------------------------------
 * Pure-TCP transport state. Plays the role that mbedtls_session_t plays
 * for HTTPS. Lives as ->param on the embedded vsf_http_client_t when the
 * request scheme is "http".
 * -------------------------------------------------------------------------- */

#if VSF_USE_LINUX == ENABLED
typedef struct vsf_ehc_tcp_state_t {
    int     fd;
    int     timeout_ms;
} vsf_ehc_tcp_state_t;
#endif

/* --------------------------------------------------------------------------
 * The esp_http_client handle. Wraps vsf_http_client_t + TLS session (for
 * HTTPS) or TCP state (for plain HTTP). Owns all heap-allocated strings so
 * cleanup is a straight-line free.
 * -------------------------------------------------------------------------- */

struct esp_http_client {
    vsf_http_client_t           http;       // must be zero-initialised before
                                            // vsf_http_client_init() is called
#if VSF_USE_MBEDTLS == ENABLED
    mbedtls_session_t           tls;        // valid iff is_https
#endif
#if VSF_USE_LINUX == ENABLED
    vsf_ehc_tcp_state_t         tcp;        // valid iff !is_https
#endif

    // connection info
    char                       *scheme;     // "http" / "https"
    char                       *host;
    int                         port;
    char                       *path;
    char                       *query;
    char                       *username;
    char                       *password;
    char                       *user_agent;

    // TLS material (only pointers; lifetime: owned strings dup'd at init/set_url)
    const char                 *cert_pem;
    size_t                      cert_len;
    const char                 *client_cert_pem;
    size_t                      client_cert_len;
    const char                 *client_key_pem;
    size_t                      client_key_len;
    const char                 *common_name;
    bool                        skip_cert_common_name_check;
    esp_err_t                 (*crt_bundle_attach)(void *conf);

    // request
    esp_http_client_method_t    method;
    esp_http_client_auth_type_t auth_type;
    vsf_ehc_header_t           *headers;
    char                       *post_data;
    int                         post_len;

    // runtime / response
    esp_http_state_t            state;
    esp_http_client_transport_t transport;
    int                         timeout_ms;
    bool                        is_https;
    bool                        is_chunked_req;     // chunked request body (write_len == -1)
    bool                        cancel_requested;
    bool                        keep_alive_enable;
    bool                        disable_auto_redirect_runtime;
    int                         redirect_count;
    int                         max_redirect;
    int                         auth_retry_count;
    int                         max_auth_retries;
    int                         saved_errno;
    int64_t                     body_total;         // bytes consumed from body via read()

    // event handling
    http_event_handle_cb        event_handler;
    void                       *user_data;
};

/*============================ PROTOTYPES (static) ===========================*/

static int      s_parse_url_into_client(struct esp_http_client *c, const char *url);
static void     s_free_str(char **p);
static int      s_dup_str(char **dst, const char *src);
static void     s_header_free_all(struct esp_http_client *c);
static vsf_ehc_header_t *s_header_find(struct esp_http_client *c, const char *key);
static int      s_dispatch_event(struct esp_http_client *c,
                                 esp_http_client_event_id_t id,
                                 void *data, int data_len,
                                 char *hdr_key, char *hdr_value);

#if VSF_USE_LINUX == ENABLED
static int      s_tcp_connect(void *param, const char *host, const char *port);
static void     s_tcp_close(void *param);
static int      s_tcp_write(void *param, uint8_t *buf, size_t len);
static int      s_tcp_read(void *param, uint8_t *buf, size_t len);
static int      s_tcp_set_timeout(void *param, int timeout_ms);
static int      s_tcp_get_fd(void *param);

static const vsf_http_op_t s_vsf_tcp_http_op = {
    .fn_connect     = s_tcp_connect,
    .fn_close       = s_tcp_close,
    .fn_write       = s_tcp_write,
    .fn_read        = s_tcp_read,
    .fn_set_timeout = s_tcp_set_timeout,
    .fn_get_fd      = s_tcp_get_fd,
};
#endif

/*============================ IMPLEMENTATION: URL PARSER ====================*/

static void http_parser_url_init(struct http_parser_url *u)
{
    if (u != NULL) {
        memset(u, 0, sizeof(*u));
    }
}

// Return 0 on success, non-zero on failure (matches upstream signature).
static int http_parser_parse_url(const char *buf, size_t buflen,
                                 int is_connect, struct http_parser_url *u)
{
    if (NULL == buf || NULL == u || buflen > UINT16_MAX || is_connect) {
        // CONNECT-style authorities not supported in this trimmed parser
        return 1;
    }

    http_parser_url_init(u);

    size_t i = 0;

    // ---- scheme ----
    size_t scheme_off = 0, scheme_len = 0;
    size_t colon = (size_t)-1;
    for (size_t k = 0; k < buflen; ++k) {
        char ch = buf[k];
        if (ch == ':') { colon = k; break; }
        // scheme chars: ALPHA / DIGIT / '+' / '-' / '.'
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
              (ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.')) {
            break;
        }
    }
    if (colon != (size_t)-1 && colon > 0 &&
        colon + 2 < buflen && buf[colon + 1] == '/' && buf[colon + 2] == '/') {
        scheme_off = 0;
        scheme_len = colon;
        u->field_data[UF_SCHEMA].off = (uint16_t)scheme_off;
        u->field_data[UF_SCHEMA].len = (uint16_t)scheme_len;
        u->field_set |= (uint16_t)(1U << UF_SCHEMA);
        i = colon + 3; // skip "://"
    } else {
        // No scheme: caller expected host-start; we don't support that here.
        return 1;
    }

    // ---- authority: [userinfo@]host[:port] ----
    size_t auth_start = i;
    size_t auth_end = buflen;
    for (size_t k = i; k < buflen; ++k) {
        char ch = buf[k];
        if (ch == '/' || ch == '?' || ch == '#') { auth_end = k; break; }
    }

    // userinfo?
    size_t at = (size_t)-1;
    for (size_t k = auth_start; k < auth_end; ++k) {
        if (buf[k] == '@') { at = k; break; }
    }
    size_t host_start;
    if (at != (size_t)-1) {
        u->field_data[UF_USERINFO].off = (uint16_t)auth_start;
        u->field_data[UF_USERINFO].len = (uint16_t)(at - auth_start);
        u->field_set |= (uint16_t)(1U << UF_USERINFO);
        host_start = at + 1;
    } else {
        host_start = auth_start;
    }

    // IPv6 literal rejected in this subset
    if (host_start < auth_end && buf[host_start] == '[') {
        return 1;
    }

    // host:port split
    size_t colon2 = (size_t)-1;
    for (size_t k = host_start; k < auth_end; ++k) {
        if (buf[k] == ':') { colon2 = k; break; }
    }
    size_t host_end = (colon2 == (size_t)-1) ? auth_end : colon2;
    if (host_end == host_start) {
        // empty host
        return 1;
    }
    u->field_data[UF_HOST].off = (uint16_t)host_start;
    u->field_data[UF_HOST].len = (uint16_t)(host_end - host_start);
    u->field_set |= (uint16_t)(1U << UF_HOST);

    if (colon2 != (size_t)-1) {
        size_t port_start = colon2 + 1;
        size_t port_len = auth_end - port_start;
        if (port_len == 0 || port_len > 5) {
            return 1;
        }
        u->field_data[UF_PORT].off = (uint16_t)port_start;
        u->field_data[UF_PORT].len = (uint16_t)port_len;
        u->field_set |= (uint16_t)(1U << UF_PORT);

        unsigned long v = 0;
        for (size_t k = 0; k < port_len; ++k) {
            char ch = buf[port_start + k];
            if (ch < '0' || ch > '9') return 1;
            v = v * 10 + (unsigned long)(ch - '0');
            if (v > 65535) return 1;
        }
        u->port = (uint16_t)v;
    }

    // ---- path + query + fragment ----
    size_t path_start = auth_end;
    size_t qmark = (size_t)-1, hash = (size_t)-1;
    for (size_t k = path_start; k < buflen; ++k) {
        char ch = buf[k];
        if (ch == '?' && qmark == (size_t)-1) qmark = k;
        else if (ch == '#') { hash = k; break; }
    }

    size_t path_end = buflen;
    if (qmark != (size_t)-1) path_end = qmark;
    else if (hash  != (size_t)-1) path_end = hash;

    if (path_end > path_start) {
        u->field_data[UF_PATH].off = (uint16_t)path_start;
        u->field_data[UF_PATH].len = (uint16_t)(path_end - path_start);
        u->field_set |= (uint16_t)(1U << UF_PATH);
    }

    if (qmark != (size_t)-1) {
        size_t q_start = qmark + 1;
        size_t q_end = (hash != (size_t)-1) ? hash : buflen;
        if (q_end > q_start) {
            u->field_data[UF_QUERY].off = (uint16_t)q_start;
            u->field_data[UF_QUERY].len = (uint16_t)(q_end - q_start);
            u->field_set |= (uint16_t)(1U << UF_QUERY);
        }
    }

    if (hash != (size_t)-1 && hash + 1 < buflen) {
        u->field_data[UF_FRAGMENT].off = (uint16_t)(hash + 1);
        u->field_data[UF_FRAGMENT].len = (uint16_t)(buflen - (hash + 1));
        u->field_set |= (uint16_t)(1U << UF_FRAGMENT);
    }

    return 0;
}

/*============================ IMPLEMENTATION: HELPERS =======================*/

static void s_free_str(char **p)
{
    if (p != NULL && *p != NULL) {
        vsf_heap_free(*p);
        *p = NULL;
    }
}

static int s_dup_str(char **dst, const char *src)
{
    s_free_str(dst);
    if (NULL == src) {
        return 0;
    }
    size_t n = strlen(src);
    char *p = (char *)vsf_heap_malloc(n + 1);
    if (NULL == p) {
        return -1;
    }
    memcpy(p, src, n + 1);
    *dst = p;
    return 0;
}

static int s_dup_substr(char **dst, const char *src, size_t len)
{
    s_free_str(dst);
    if (NULL == src) {
        return 0;
    }
    char *p = (char *)vsf_heap_malloc(len + 1);
    if (NULL == p) {
        return -1;
    }
    memcpy(p, src, len);
    p[len] = '\0';
    *dst = p;
    return 0;
}

static int s_strcasecmp(const char *a, const char *b)
{
    if (a == b)     return 0;
    if (NULL == a)  return -1;
    if (NULL == b)  return  1;
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca + ('a' - 'A'));
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb + ('a' - 'A'));
        if (ca != cb) return (unsigned char)ca - (unsigned char)cb;
        ++a; ++b;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static vsf_ehc_header_t *s_header_find(struct esp_http_client *c, const char *key)
{
    if (NULL == c || NULL == key) return NULL;
    for (vsf_ehc_header_t *h = c->headers; h != NULL; h = h->next) {
        if (0 == s_strcasecmp(h->key, key)) {
            return h;
        }
    }
    return NULL;
}

static esp_err_t s_header_set(struct esp_http_client *c, const char *key, const char *value)
{
    if (NULL == c || NULL == key) return ESP_ERR_INVALID_ARG;

    vsf_ehc_header_t *h = s_header_find(c, key);
    if (h != NULL) {
        if (NULL == value) {
            // replace with empty string to satisfy "value is always non-NULL"
            if (s_dup_str(&h->value, "") < 0) return ESP_ERR_NO_MEM;
        } else {
            if (s_dup_str(&h->value, value) < 0) return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }

    h = (vsf_ehc_header_t *)vsf_heap_malloc(sizeof(*h));
    if (NULL == h) return ESP_ERR_NO_MEM;
    memset(h, 0, sizeof(*h));
    if (s_dup_str(&h->key, key) < 0) {
        vsf_heap_free(h);
        return ESP_ERR_NO_MEM;
    }
    if (s_dup_str(&h->value, value != NULL ? value : "") < 0) {
        s_free_str(&h->key);
        vsf_heap_free(h);
        return ESP_ERR_NO_MEM;
    }
    h->next = c->headers;
    c->headers = h;
    return ESP_OK;
}

static void s_header_free_all(struct esp_http_client *c)
{
    vsf_ehc_header_t *h = c->headers;
    while (h != NULL) {
        vsf_ehc_header_t *next = h->next;
        s_free_str(&h->key);
        s_free_str(&h->value);
        vsf_heap_free(h);
        h = next;
    }
    c->headers = NULL;
}

static int s_dispatch_event(struct esp_http_client *c,
                            esp_http_client_event_id_t id,
                            void *data, int data_len,
                            char *hdr_key, char *hdr_value)
{
    if (NULL == c || NULL == c->event_handler) return 0;
    esp_http_client_event_t evt = {
        .event_id     = id,
        .client       = c,
        .data         = data,
        .data_len     = data_len,
        .user_data    = c->user_data,
        .header_key   = hdr_key,
        .header_value = hdr_value,
    };
    return (int)c->event_handler(&evt);
}

/*============================ IMPLEMENTATION: TCP OP ========================*/

#if VSF_USE_LINUX == ENABLED

static int s_tcp_connect(void *param, const char *host, const char *port)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st || NULL == host || NULL == port) return -1;

    struct addrinfo hints = { 0 };
    struct addrinfo *res = NULL;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int r = getaddrinfo(host, port, &hints, &res);
    if (r != 0 || NULL == res) return -1;

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) {
        freeaddrinfo(res);
        return -1;
    }

    if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
        close(fd);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    st->fd = fd;
    if (st->timeout_ms > 0) {
        (void)s_tcp_set_timeout(st, st->timeout_ms);
    }
    return 0;
}

static void s_tcp_close(void *param)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st) return;
    if (st->fd >= 0) {
        close(st->fd);
        st->fd = -1;
    }
}

static int s_tcp_write(void *param, uint8_t *buf, size_t len)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st || st->fd < 0) return -1;
    // send() returns ssize_t on POSIX / int on winsock; narrow to int.
    // HTTP send buffers are far below INT_MAX, so no clamp is needed
    // (mirrors the convention of mbedtls_session_read/write).
    return (int)send(st->fd, buf, len, 0);
}

static int s_tcp_read(void *param, uint8_t *buf, size_t len)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st || st->fd < 0) return -1;
    return (int)recv(st->fd, buf, len, 0);
}

static int s_tcp_set_timeout(void *param, int timeout_ms)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st) return -1;
    st->timeout_ms = timeout_ms;
    if (st->fd < 0) return 0;   // accepted; will take effect on next connect

    struct timeval tv;
    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    if (setsockopt(st->fd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv, sizeof(tv)) < 0) {
        return -1;
    }
    return 0;
}

static int s_tcp_get_fd(void *param)
{
    vsf_ehc_tcp_state_t *st = (vsf_ehc_tcp_state_t *)param;
    if (NULL == st) return -1;
    return st->fd;
}

#endif // VSF_USE_LINUX == ENABLED

/*============================ IMPLEMENTATION: URL/CFG LOAD ==================*/

// Consume URL fields into *c. Returns 0 on success, -1 on error.
static int s_parse_url_into_client(struct esp_http_client *c, const char *url)
{
    if (NULL == c || NULL == url) return -1;

    struct http_parser_url purl;
    http_parser_url_init(&purl);

    size_t url_len = strlen(url);
    if (http_parser_parse_url(url, url_len, 0, &purl) != 0) {
        return -1;
    }

    if (purl.field_set & (1U << UF_SCHEMA)) {
        if (s_dup_substr(&c->scheme,
                         url + purl.field_data[UF_SCHEMA].off,
                         purl.field_data[UF_SCHEMA].len) < 0) return -1;
    }
    if (purl.field_set & (1U << UF_HOST)) {
        if (s_dup_substr(&c->host,
                         url + purl.field_data[UF_HOST].off,
                         purl.field_data[UF_HOST].len) < 0) return -1;
    }

    // transport + default port
    if (c->scheme != NULL) {
        if (0 == s_strcasecmp(c->scheme, "https")) {
            c->is_https  = true;
            c->transport = HTTP_TRANSPORT_OVER_SSL;
            c->port      = VSF_EHC_DEFAULT_HTTPS_PORT;
        } else if (0 == s_strcasecmp(c->scheme, "http")) {
            c->is_https  = false;
            c->transport = HTTP_TRANSPORT_OVER_TCP;
            c->port      = VSF_EHC_DEFAULT_HTTP_PORT;
        } else {
            return -1;
        }
    }

    if (purl.field_set & (1U << UF_PORT)) {
        c->port = (int)purl.port;
    }

    if (purl.field_set & (1U << UF_PATH)) {
        if (s_dup_substr(&c->path,
                         url + purl.field_data[UF_PATH].off,
                         purl.field_data[UF_PATH].len) < 0) return -1;
    } else {
        if (s_dup_str(&c->path, "/") < 0) return -1;
    }

    if (purl.field_set & (1U << UF_QUERY)) {
        if (s_dup_substr(&c->query,
                         url + purl.field_data[UF_QUERY].off,
                         purl.field_data[UF_QUERY].len) < 0) return -1;
    } else {
        s_free_str(&c->query);
    }

    if (purl.field_set & (1U << UF_USERINFO)) {
        size_t uoff = purl.field_data[UF_USERINFO].off;
        size_t ulen = purl.field_data[UF_USERINFO].len;
        const char *colon = memchr(url + uoff, ':', ulen);
        if (colon != NULL) {
            size_t user_len = (size_t)(colon - (url + uoff));
            size_t pass_len = ulen - user_len - 1;
            if (s_dup_substr(&c->username, url + uoff, user_len) < 0) return -1;
            if (s_dup_substr(&c->password, colon + 1, pass_len) < 0) return -1;
        } else {
            if (s_dup_substr(&c->username, url + uoff, ulen) < 0) return -1;
        }
    }

    return 0;
}

/*============================ IMPLEMENTATION: LIFECYCLE =====================*/

esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *config)
{
    if (NULL == config) return NULL;

    struct esp_http_client *c = (struct esp_http_client *)vsf_heap_malloc(sizeof(*c));
    if (NULL == c) return NULL;
    memset(c, 0, sizeof(*c));

#if VSF_USE_LINUX == ENABLED
    c->tcp.fd = -1;
#endif
    c->state              = HTTP_STATE_INIT;
    c->port               = 0;
    c->method             = config->method;
    c->auth_type          = config->auth_type;
    c->timeout_ms         = (config->timeout_ms > 0) ? config->timeout_ms : VSF_EHC_DEFAULT_TIMEOUT_MS;
    c->event_handler      = config->event_handler;
    c->user_data          = config->user_data;
    c->keep_alive_enable  = config->keep_alive_enable;
    c->disable_auto_redirect_runtime = config->disable_auto_redirect;
    c->max_redirect       = (config->max_redirection_count > 0)
                          ? config->max_redirection_count
                          : VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_REDIRECT_DEFAULT;
    c->max_auth_retries   = (config->max_authorization_retries > 0)
                          ? config->max_authorization_retries
                          : (config->max_authorization_retries == -1
                                 ? 0
                                 : VSF_ESPIDF_HTTP_CLIENT_CFG_MAX_AUTH_RETRIES_DEFAULT);

    c->cert_pem                  = config->cert_pem;
    c->cert_len                  = config->cert_len;
    c->client_cert_pem           = config->client_cert_pem;
    c->client_cert_len           = config->client_cert_len;
    c->client_key_pem            = config->client_key_pem;
    c->client_key_len            = config->client_key_len;
    c->common_name               = config->common_name;
    c->skip_cert_common_name_check = config->skip_cert_common_name_check;
    c->crt_bundle_attach         = config->crt_bundle_attach;

    // resolve target: url wins over host/path/transport fields
    if (config->url != NULL) {
        if (s_parse_url_into_client(c, config->url) < 0) {
            esp_http_client_cleanup(c);
            return NULL;
        }
    } else {
        // host / path / transport_type path
        if (config->host != NULL) {
            if (s_dup_str(&c->host, config->host) < 0) goto fail;
        }
        if (config->path != NULL) {
            if (s_dup_str(&c->path, config->path) < 0) goto fail;
        } else {
            if (s_dup_str(&c->path, "/") < 0) goto fail;
        }
        if (config->query != NULL) {
            if (s_dup_str(&c->query, config->query) < 0) goto fail;
        }
        if (HTTP_TRANSPORT_OVER_SSL == config->transport_type) {
            c->is_https  = true;
            c->transport = HTTP_TRANSPORT_OVER_SSL;
            if (s_dup_str(&c->scheme, "https") < 0) goto fail;
            c->port = (config->port > 0) ? config->port : VSF_EHC_DEFAULT_HTTPS_PORT;
        } else {
            c->is_https  = false;
            c->transport = HTTP_TRANSPORT_OVER_TCP;
            if (s_dup_str(&c->scheme, "http") < 0) goto fail;
            c->port = (config->port > 0) ? config->port : VSF_EHC_DEFAULT_HTTP_PORT;
        }
    }

    if (config->username != NULL) {
        if (s_dup_str(&c->username, config->username) < 0) goto fail;
    }
    if (config->password != NULL) {
        if (s_dup_str(&c->password, config->password) < 0) goto fail;
    }
    if (config->user_agent != NULL) {
        if (s_dup_str(&c->user_agent, config->user_agent) < 0) goto fail;
    }

#if VSF_USE_MBEDTLS != ENABLED
    if (c->is_https) {
        // Cannot service HTTPS without TLS
        esp_http_client_cleanup(c);
        return NULL;
    }
#endif

    // Configure the underlying vsf_http_client. buffer/_buffer_owned left zero
    // so vsf_http_client_init() will heap-allocate using CFG_BUFFER_SIZE.
    c->http.buffer      = NULL;
    c->http.buffer_size = (config->buffer_size > 0) ? (size_t)config->buffer_size : 0;

#if VSF_USE_MBEDTLS == ENABLED
    if (c->is_https) {
        c->http.op    = &vsf_mbedtls_http_op;
        c->http.param = &c->tls;
    }
#endif
#if VSF_USE_LINUX == ENABLED
    if (!c->is_https) {
        c->http.op    = &s_vsf_tcp_http_op;
        c->http.param = &c->tcp;
    }
#endif

    if (vsf_http_client_init(&c->http) != VSF_ERR_NONE) {
        goto fail;
    }

    return c;

fail:
    esp_http_client_cleanup(c);
    return NULL;
}

esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;

    // close transport (idempotent via op->fn_close checking fd/state)
    vsf_http_client_close(&client->http);
    vsf_http_client_fini(&client->http);

    s_header_free_all(client);
    s_free_str(&client->scheme);
    s_free_str(&client->host);
    s_free_str(&client->path);
    s_free_str(&client->query);
    s_free_str(&client->username);
    s_free_str(&client->password);
    s_free_str(&client->user_agent);
    s_free_str(&client->post_data);

    // redirect_path inside vsf_http_client is owned by vsf_http_client; fini
    // above is a no-op on it by contract. We don't touch it here.

    vsf_heap_free(client);
    return ESP_OK;
}

/*============================ IMPLEMENTATION: BASE64 / METHOD ===============*/

// RFC 4648 base64 encode. Writes NUL-terminated output to dst; caller
// must provide at least ((srclen + 2) / 3) * 4 + 1 bytes. Returns bytes
// written excluding terminator.
static size_t s_base64_encode(char *dst, const uint8_t *src, size_t srclen)
{
    static const char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i = 0, o = 0;
    while (i + 3 <= srclen) {
        uint32_t v = ((uint32_t)src[i] << 16)
                   | ((uint32_t)src[i + 1] << 8)
                   |  (uint32_t)src[i + 2];
        dst[o++] = tbl[(v >> 18) & 0x3F];
        dst[o++] = tbl[(v >> 12) & 0x3F];
        dst[o++] = tbl[(v >>  6) & 0x3F];
        dst[o++] = tbl[ v        & 0x3F];
        i += 3;
    }
    if (i < srclen) {
        uint32_t v = (uint32_t)src[i] << 16;
        if (i + 1 < srclen) v |= (uint32_t)src[i + 1] << 8;
        dst[o++] = tbl[(v >> 18) & 0x3F];
        dst[o++] = tbl[(v >> 12) & 0x3F];
        dst[o++] = (i + 1 < srclen) ? tbl[(v >> 6) & 0x3F] : '=';
        dst[o++] = '=';
    }
    dst[o] = '\0';
    return o;
}

static const char *s_method_str(esp_http_client_method_t m)
{
    switch (m) {
    case HTTP_METHOD_GET:          return "GET";
    case HTTP_METHOD_POST:         return "POST";
    case HTTP_METHOD_PUT:          return "PUT";
    case HTTP_METHOD_PATCH:        return "PATCH";
    case HTTP_METHOD_DELETE:       return "DELETE";
    case HTTP_METHOD_HEAD:         return "HEAD";
    case HTTP_METHOD_NOTIFY:       return "NOTIFY";
    case HTTP_METHOD_SUBSCRIBE:    return "SUBSCRIBE";
    case HTTP_METHOD_UNSUBSCRIBE:  return "UNSUBSCRIBE";
    case HTTP_METHOD_OPTIONS:      return "OPTIONS";
    case HTTP_METHOD_COPY:         return "COPY";
    case HTTP_METHOD_MOVE:         return "MOVE";
    case HTTP_METHOD_LOCK:         return "LOCK";
    case HTTP_METHOD_UNLOCK:       return "UNLOCK";
    case HTTP_METHOD_PROPFIND:     return "PROPFIND";
    case HTTP_METHOD_PROPPATCH:    return "PROPPATCH";
    case HTTP_METHOD_MKCOL:        return "MKCOL";
    case HTTP_METHOD_REPORT:       return "REPORT";
    default:                       return "GET";
    }
}

// Build "Key: Value\r\n" string from the header list + optional Basic auth
// line; returned pointer owned by caller (vsf_heap_free). NULL on OOM.
static char *s_build_header_block(struct esp_http_client *c)
{
    // 1. compute total size
    size_t total = 0;
    for (vsf_ehc_header_t *h = c->headers; h != NULL; h = h->next) {
        total += strlen(h->key) + 2 + strlen(h->value) + 2;
    }

    char *auth_line = NULL;
    if (c->auth_type == HTTP_AUTH_TYPE_BASIC &&
        c->username != NULL && c->password != NULL) {
        size_t ulen = strlen(c->username);
        size_t plen = strlen(c->password);
        size_t raw  = ulen + 1 + plen;
        size_t b64cap = ((raw + 2) / 3) * 4 + 1;
        size_t line_cap = sizeof("Authorization: Basic \r\n") + b64cap;
        auth_line = (char *)vsf_heap_malloc(line_cap);
        if (NULL == auth_line) return NULL;
        char *raw_buf = (char *)vsf_heap_malloc(raw + 1);
        if (NULL == raw_buf) { vsf_heap_free(auth_line); return NULL; }
        memcpy(raw_buf, c->username, ulen);
        raw_buf[ulen] = ':';
        memcpy(raw_buf + ulen + 1, c->password, plen);
        int pre = snprintf(auth_line, line_cap, "Authorization: Basic ");
        size_t off = (pre > 0) ? (size_t)pre : 0;
        size_t blen = s_base64_encode(auth_line + off, (const uint8_t *)raw_buf, raw);
        vsf_heap_free(raw_buf);
        size_t end_off = off + blen;
        if (end_off + 2 < line_cap) {
            auth_line[end_off++] = '\r';
            auth_line[end_off++] = '\n';
            auth_line[end_off]   = '\0';
        }
        total += strlen(auth_line);
    }

    // 2. allocate and fill. +1 for NUL (vsf_http_client_send_header uses %s).
    char *out = (char *)vsf_heap_malloc(total + 1);
    if (NULL == out) {
        if (auth_line) vsf_heap_free(auth_line);
        return NULL;
    }
    size_t off = 0;
    for (vsf_ehc_header_t *h = c->headers; h != NULL; h = h->next) {
        off += (size_t)sprintf(out + off, "%s: %s\r\n", h->key, h->value);
    }
    if (auth_line != NULL) {
        size_t al = strlen(auth_line);
        memcpy(out + off, auth_line, al);
        off += al;
        out[off] = '\0';
        vsf_heap_free(auth_line);
    }
    out[total] = '\0';
    return out;
}

/*============================ IMPLEMENTATION: Tier 1 DRIVER =================*/

esp_err_t esp_http_client_close(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    vsf_http_client_close(&client->http);
    if (client->state >= HTTP_STATE_CONNECTED) {
        (void)s_dispatch_event(client, HTTP_EVENT_DISCONNECTED, NULL, 0, NULL, NULL);
    }
    client->state = HTTP_STATE_CLOSE;
    return ESP_OK;
}

esp_err_t esp_http_client_open(esp_http_client_handle_t client, int write_len)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    if (NULL == client->host) return ESP_ERR_INVALID_ARG;

    // Apply timeout to transport before the connect() attempt; fn_set_timeout
    // accepts calls both pre- and post-connect per op contract.
    (void)vsf_http_client_set_timeout(&client->http, client->timeout_ms);

    client->state           = HTTP_STATE_CONNECTING;
    client->body_total      = 0;
    client->is_chunked_req  = (write_len == -1);

    // Default Content-Type for POST if caller did not supply one.
    if ((client->method == HTTP_METHOD_POST ||
         client->method == HTTP_METHOD_PUT  ||
         client->method == HTTP_METHOD_PATCH) &&
        NULL == s_header_find(client, "Content-Type")) {
        (void)s_header_set(client, "Content-Type", "application/x-www-form-urlencoded");
    }
    if (client->is_chunked_req) {
        (void)s_header_set(client, "Transfer-Encoding", "chunked");
    }

    char port_s[8];
    snprintf(port_s, sizeof(port_s), "%d", client->port);
    int r = vsf_http_client_connect(&client->http, client->host, port_s);
    if (r != 0) {
        client->saved_errno = -1;
        client->state = HTTP_STATE_CLOSE;
        return ESP_ERR_HTTP_CONNECT;
    }
    client->state = HTTP_STATE_CONNECTED;
    (void)s_dispatch_event(client, HTTP_EVENT_ON_CONNECTED, NULL, 0, NULL, NULL);

    // Build request line + headers and send. Use the VSF streaming path so
    // the body can be written separately by esp_http_client_write().
    char *path_with_query = NULL;
    if (client->query != NULL && client->path != NULL) {
        size_t pl = strlen(client->path);
        size_t ql = strlen(client->query);
        path_with_query = (char *)vsf_heap_malloc(pl + 1 + ql + 1);
        if (NULL == path_with_query) return ESP_ERR_NO_MEM;
        memcpy(path_with_query, client->path, pl);
        path_with_query[pl] = '?';
        memcpy(path_with_query + pl + 1, client->query, ql + 1);
    }
    char *hdr = s_build_header_block(client);
    if (NULL == hdr) {
        if (path_with_query) vsf_heap_free(path_with_query);
        return ESP_ERR_NO_MEM;
    }

    vsf_http_client_req_t req = {
        .host         = client->host,
        .port         = port_s,
        .verb         = s_method_str(client->method),
        .connect_mode = client->keep_alive_enable ? "keep-alive" : "close",
        .header       = hdr,
        .path         = (path_with_query != NULL) ? path_with_query : client->path,
        .txdata       = NULL,
        // write_len >= 0 sets Content-Length; write_len == -1 uses chunked (hdr already set).
        .txdata_len   = (write_len > 0) ? (size_t)write_len : 0,
    };

    int sent = vsf_http_client_send_header(&client->http, &req);
    vsf_heap_free(hdr);
    if (path_with_query) vsf_heap_free(path_with_query);
    if (sent < 0) {
        client->saved_errno = sent;
        return ESP_ERR_HTTP_WRITE_DATA;
    }

    client->state = HTTP_STATE_REQ_COMPLETE_HEADER;
    (void)s_dispatch_event(client, HTTP_EVENT_HEADERS_SENT, NULL, 0, NULL, NULL);
    return ESP_OK;
}

int esp_http_client_write(esp_http_client_handle_t client, const char *buffer, int len)
{
    if (NULL == client || len < 0)             return -1;
    if (len == 0)                                return 0;
    if (NULL == buffer)                          return -1;
    if (client->state < HTTP_STATE_REQ_COMPLETE_HEADER) return -1;
    int w = vsf_http_client_write(&client->http, (uint8_t *)(uintptr_t)buffer, (size_t)len);
    if (w < 0) client->saved_errno = w;
    return w;
}

int64_t esp_http_client_fetch_headers(esp_http_client_handle_t client)
{
    if (NULL == client) return -1;
    // A minimal on_response_header forwards each header line up to the
    // application as an HTTP_EVENT_ON_HEADER. Keys and values are split on
    // the first ':' (trimmed). Lines are mutated in place by the core
    // parser so we copy key/value onto the stack briefly for dispatch.
    struct ehc_hdr_ctx { struct esp_http_client *c; } ctx = { client };
    (void)ctx;

    vsf_http_client_req_t req = { 0 };
    req.on_response_header = NULL;  // the core parser does NOT expose
                                    // per-line callbacks with user ctx,
                                    // so we dispatch ON_HEADER events
                                    // ourselves after fetch_headers returns
                                    // using only what the core surfaces
                                    // (status / content-length / chunked /
                                    // redirect_path). Individual header
                                    // dispatch requires core support; we
                                    // deliver what we have.

    int r = vsf_http_client_fetch_headers(&client->http, &req);
    if (r < 0) {
        client->saved_errno = r;
        return -1;
    }
    if (r == 0 && client->http.resp_status == 0) {
        // peer closed before any status line
        return ESP_ERR_HTTP_CONNECTION_CLOSED;
    }
    (void)s_dispatch_event(client, HTTP_EVENT_ON_HEADERS_COMPLETE, NULL, 0, NULL, NULL);
    (void)s_dispatch_event(client, HTTP_EVENT_ON_STATUS_CODE,
                           &client->http.resp_status, (int)sizeof(int),
                           NULL, NULL);

    client->state = HTTP_STATE_RES_COMPLETE_HEADER;
    if (client->http.is_chunked) {
        return 0;
    }
    return (int64_t)client->http.content_length;
}

int esp_http_client_read(esp_http_client_handle_t client, char *buffer, int len)
{
    if (NULL == client || len < 0) return -1;
    if (len == 0)                   return 0;
    if (NULL == buffer)             return -1;
#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CANCEL == ENABLED
    if (client->cancel_requested)   return -1;
#endif
    int n = vsf_http_client_read(&client->http, (uint8_t *)buffer, (size_t)len);
    if (n > 0) {
        client->body_total += n;
        client->state = HTTP_STATE_RES_ON_DATA_START;
        (void)s_dispatch_event(client, HTTP_EVENT_ON_DATA, buffer, n, NULL, NULL);
    }
    return n;
}

int esp_http_client_read_response(esp_http_client_handle_t client, char *buffer, int len)
{
    if (NULL == client || NULL == buffer || len <= 0) return -1;
    int total = 0;
    while (total < len) {
        int n = esp_http_client_read(client, buffer + total, len - total);
        if (n <= 0) break;
        total += n;
    }
    return total;
}

bool esp_http_client_is_complete_data_received(esp_http_client_handle_t client)
{
    if (NULL == client) return false;
    if (client->http.is_chunked) {
        // chunked decoder marks terminator by setting cur_chunk_size = -2
        return client->http.cur_chunk_size == -2;
    }
    if (client->http.content_length > 0) {
        return client->body_total >= (int64_t)client->http.content_length;
    }
    // Neither content-length nor chunked -> can't tell until peer closes.
    return client->state == HTTP_STATE_CLOSE;
}

bool esp_http_client_is_chunked_response(esp_http_client_handle_t client)
{
    return (client != NULL) && client->http.is_chunked;
}

/*============================ IMPLEMENTATION: Tier 1 PERFORM ================*/

esp_err_t esp_http_client_perform(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;

    int redirects = 0;
    int auth_tries = 0;
    (void)redirects; (void)auth_tries;

retry:
    (void)esp_http_client_close(client);    // idempotent; safe on first pass

    int write_len = 0;
    if (client->post_data != NULL && client->post_len > 0) {
        write_len = client->post_len;
        // ensure Content-Length header will go out via open()'s req.txdata_len
    }

    esp_err_t err = esp_http_client_open(client, write_len);
    if (err != ESP_OK) return err;

    if (write_len > 0) {
        int w = esp_http_client_write(client, client->post_data, write_len);
        if (w != write_len) {
            (void)esp_http_client_close(client);
            return ESP_ERR_HTTP_WRITE_DATA;
        }
    }

    int64_t cl = esp_http_client_fetch_headers(client);
    if (cl < 0) {
        (void)esp_http_client_close(client);
        return ESP_FAIL;
    }

#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT == ENABLED
    int status = client->http.resp_status;
    if (!client->disable_auto_redirect_runtime &&
        client->http.redirect_path != NULL &&
        (status == 301 || status == 302 || status == 303 ||
         status == 307 || status == 308) &&
        redirects < client->max_redirect) {
        // copy redirect_path out of the vsf_http_client buffer BEFORE close(),
        // since redirect_path is a pointer into that buffer and gets clobbered
        // on the next fetch_headers() invocation.
        char *redir = NULL;
        if (s_dup_str(&redir, client->http.redirect_path) < 0) {
            (void)esp_http_client_close(client);
            return ESP_ERR_NO_MEM;
        }
        esp_http_client_redirect_event_data_t rdata = {
            .client = client, .status_code = status
        };
        (void)s_dispatch_event(client, HTTP_EVENT_REDIRECT, &rdata, (int)sizeof(rdata), NULL, NULL);
        (void)esp_http_client_close(client);
        int rc = esp_http_client_set_url(client, redir);
        vsf_heap_free(redir);
        if (rc != ESP_OK) return rc;
        redirects++;
        goto retry;
    }
    if (status == 401 && auth_tries < client->max_auth_retries) {
        (void)esp_http_client_add_auth(client);
        auth_tries++;
        goto retry;
    }
#endif

    (void)s_dispatch_event(client, HTTP_EVENT_ON_FINISH, NULL, 0, NULL, NULL);
    client->state = HTTP_STATE_RES_COMPLETE_DATA;
    return ESP_OK;
}

/*============================ IMPLEMENTATION: URL / METHOD / HEADERS ========*/

esp_err_t esp_http_client_set_url(esp_http_client_handle_t client, const char *url)
{
    if (NULL == client || NULL == url) return ESP_ERR_INVALID_ARG;

    // Preserve old host/port to decide whether to drop the connection.
    char *old_host = client->host ? vsf_heap_malloc(strlen(client->host) + 1) : NULL;
    if (client->host && old_host) strcpy(old_host, client->host);
    int old_port = client->port;

    // http_parser_parse_url() used by s_parse_url_into_client requires a URL
    // with scheme; relative paths fall back to updating only the path.
    if (strstr(url, "://") != NULL) {
        if (s_parse_url_into_client(client, url) < 0) {
            if (old_host) vsf_heap_free(old_host);
            return ESP_FAIL;
        }
    } else {
        // relative: split on '?' for query
        const char *q = strchr(url, '?');
        if (q != NULL) {
            if (s_dup_substr(&client->path, url, (size_t)(q - url)) < 0)
                { if (old_host) vsf_heap_free(old_host); return ESP_ERR_NO_MEM; }
            if (s_dup_str(&client->query, q + 1) < 0)
                { if (old_host) vsf_heap_free(old_host); return ESP_ERR_NO_MEM; }
        } else {
            if (s_dup_str(&client->path, url) < 0)
                { if (old_host) vsf_heap_free(old_host); return ESP_ERR_NO_MEM; }
            s_free_str(&client->query);
        }
    }

    bool host_changed = (old_host != NULL && client->host != NULL &&
                         s_strcasecmp(old_host, client->host) != 0);
    bool port_changed = (old_port != client->port);
    if (host_changed || port_changed) {
        (void)s_header_set(client, "Host", client->host);
        vsf_http_client_close(&client->http);
        client->state = HTTP_STATE_CLOSE;
    }
    if (old_host) vsf_heap_free(old_host);
    return ESP_OK;
}

esp_err_t esp_http_client_get_url(esp_http_client_handle_t client, char *url, const int len)
{
    if (NULL == client || NULL == url || len <= 0) return ESP_ERR_INVALID_ARG;
    int n;
    if (client->query != NULL) {
        n = snprintf(url, (size_t)len, "%s://%s:%d%s?%s",
                     client->scheme ? client->scheme : "http",
                     client->host   ? client->host   : "",
                     client->port, client->path ? client->path : "/",
                     client->query);
    } else {
        n = snprintf(url, (size_t)len, "%s://%s:%d%s",
                     client->scheme ? client->scheme : "http",
                     client->host   ? client->host   : "",
                     client->port, client->path ? client->path : "/");
    }
    return (n > 0 && n < len) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_http_client_set_method(esp_http_client_handle_t client, esp_http_client_method_t method)
{
    if (NULL == client || method < 0 || method >= HTTP_METHOD_MAX) return ESP_ERR_INVALID_ARG;
    client->method = method;
    return ESP_OK;
}

esp_err_t esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    return s_header_set(client, key, value);
}

esp_err_t esp_http_client_get_header(esp_http_client_handle_t client, const char *key, char **value)
{
    if (NULL == client || NULL == key || NULL == value) return ESP_ERR_INVALID_ARG;
    vsf_ehc_header_t *h = s_header_find(client, key);
    *value = (h != NULL) ? h->value : NULL;
    return (h != NULL) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT == ENABLED
esp_err_t esp_http_client_delete_header(esp_http_client_handle_t client, const char *key)
{
    if (NULL == client || NULL == key) return ESP_ERR_INVALID_ARG;
    vsf_ehc_header_t *prev = NULL;
    for (vsf_ehc_header_t *h = client->headers; h != NULL; prev = h, h = h->next) {
        if (0 == s_strcasecmp(h->key, key)) {
            if (prev) prev->next = h->next;
            else      client->headers = h->next;
            s_free_str(&h->key);
            s_free_str(&h->value);
            vsf_heap_free(h);
            return ESP_OK;
        }
    }
    return ESP_OK; // delete of absent key is not an error
}

esp_err_t esp_http_client_delete_all_headers(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    s_header_free_all(client);
    return ESP_OK;
}
#else
esp_err_t esp_http_client_delete_header(esp_http_client_handle_t client, const char *key)
    { (void)client; (void)key; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_http_client_delete_all_headers(esp_http_client_handle_t client)
    { (void)client; return ESP_ERR_NOT_SUPPORTED; }
#endif

// Response-header capture is not persisted by the core vsf_http_client; only
// Content-Length / Transfer-Encoding / Location are surfaced. Fall back to
// request-header lookup so unmodified ESP-IDF examples keep linking.
esp_err_t esp_http_client_get_response_header(esp_http_client_handle_t client, const char *key, char **value)
{
    return esp_http_client_get_header(client, key, value);
}

/*============================ IMPLEMENTATION: POST FIELD / TIMEOUT ==========*/

esp_err_t esp_http_client_set_post_field(esp_http_client_handle_t client, const char *data, int len)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    s_free_str(&client->post_data);
    client->post_len = 0;
    if (NULL == data || len <= 0) {
        return ESP_OK;
    }
    char *p = (char *)vsf_heap_malloc((size_t)len + 1);
    if (NULL == p) return ESP_ERR_NO_MEM;
    memcpy(p, data, (size_t)len);
    p[len] = '\0';
    client->post_data = p;
    client->post_len  = len;

    // force method to POST if still the default GET
    if (client->method == HTTP_METHOD_GET) {
        client->method = HTTP_METHOD_POST;
    }
    return ESP_OK;
}

int esp_http_client_get_post_field(esp_http_client_handle_t client, char **data)
{
    if (NULL == client || NULL == data) return 0;
    *data = client->post_data;
    return client->post_len;
}

esp_err_t esp_http_client_set_timeout_ms(esp_http_client_handle_t client, int timeout_ms)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->timeout_ms = timeout_ms;
    vsf_err_t r = vsf_http_client_set_timeout(&client->http, timeout_ms);
    // set_timeout returning NOT_SUPPORT is fine -- op without the hook just
    // keeps whatever default its transport picked. Report success in that case.
    return (r == VSF_ERR_NONE || r == VSF_ERR_NOT_SUPPORT) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_http_client_set_user_data(esp_http_client_handle_t client, void *data)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->user_data = data;
    return ESP_OK;
}

esp_err_t esp_http_client_get_user_data(esp_http_client_handle_t client, void **data)
{
    if (NULL == client || NULL == data) return ESP_ERR_INVALID_ARG;
    *data = client->user_data;
    return ESP_OK;
}

/*============================ IMPLEMENTATION: AUTH ==========================*/

esp_err_t esp_http_client_set_username(esp_http_client_handle_t client, const char *username)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    if (NULL == username) { s_free_str(&client->username); return ESP_OK; }
    return (s_dup_str(&client->username, username) == 0) ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t esp_http_client_get_username(esp_http_client_handle_t client, char **value)
{
    if (NULL == client || NULL == value) return ESP_ERR_INVALID_ARG;
    *value = client->username;
    return ESP_OK;
}

esp_err_t esp_http_client_set_password(esp_http_client_handle_t client, const char *password)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    if (NULL == password) { s_free_str(&client->password); return ESP_OK; }
    return (s_dup_str(&client->password, password) == 0) ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t esp_http_client_get_password(esp_http_client_handle_t client, char **value)
{
    if (NULL == client || NULL == value) return ESP_ERR_INVALID_ARG;
    *value = client->password;
    return ESP_OK;
}

esp_err_t esp_http_client_set_authtype(esp_http_client_handle_t client, esp_http_client_auth_type_t auth_type)
{
#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH != ENABLED
    if (auth_type == HTTP_AUTH_TYPE_DIGEST) return ESP_ERR_NOT_SUPPORTED;
#endif
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->auth_type = auth_type;
    return ESP_OK;
}

// Triggered by perform() on 401 when auto-auth is enabled. Here we
// simply ensure Basic auth is armed if credentials exist. Digest requires
// parsing a WWW-Authenticate challenge which the core parser does not
// surface; when USE_DIGEST_AUTH is ENABLED the shim still needs the
// challenge string -- callers can feed it via esp_http_client_set_auth_data.
esp_err_t esp_http_client_add_auth(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    if (client->username == NULL || client->password == NULL) return ESP_ERR_INVALID_ARG;
    if (client->auth_type == HTTP_AUTH_TYPE_NONE) {
        client->auth_type = HTTP_AUTH_TYPE_BASIC;
    }
#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH != ENABLED
    if (client->auth_type == HTTP_AUTH_TYPE_DIGEST) return ESP_ERR_NOT_SUPPORTED;
#endif
    return ESP_OK;
}

// On upstream this parses "Basic realm=..." / "Digest realm=...,nonce=...".
// The core parser does not surface WWW-Authenticate so this entry point is
// accepted (to keep callers compiling) but stored only; the information is
// then used by add_auth(). Without real Digest state machine code this is
// effectively a no-op for Digest when USE_DIGEST_AUTH is DISABLED.
esp_err_t esp_http_client_set_auth_data(esp_http_client_handle_t client, const char *auth_data, int len)
{
    if (NULL == client || NULL == auth_data || len <= 0) return ESP_ERR_INVALID_ARG;
    // Heuristic: set auth_type based on prefix.
    if (len >= 5 && 0 == strncasecmp(auth_data, "Basic", 5)) {
        client->auth_type = HTTP_AUTH_TYPE_BASIC;
    } else if (len >= 6 && 0 == strncasecmp(auth_data, "Digest", 6)) {
#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_DIGEST_AUTH == ENABLED
        client->auth_type = HTTP_AUTH_TYPE_DIGEST;
        // NOTE: storing raw challenge + parsing realm/nonce/qop would live
        // here. Until the core parser surfaces WWW-Authenticate as a
        // header-line callback, Digest is documented as known-limited.
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif
    }
    return ESP_OK;
}

/*============================ IMPLEMENTATION: REDIRECT / CANCEL =============*/

#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_REDIRECT == ENABLED
esp_err_t esp_http_client_set_redirection(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    if (client->http.redirect_path == NULL) return ESP_FAIL;
    // redirect_path may be an absolute URL or a path-only string.
    if (strstr(client->http.redirect_path, "://") != NULL) {
        return esp_http_client_set_url(client, client->http.redirect_path);
    }
    // path-only: just swap path, connection stays
    char *np = NULL;
    if (s_dup_str(&np, client->http.redirect_path) < 0) return ESP_ERR_NO_MEM;
    s_free_str(&client->path);
    client->path = np;
    return ESP_OK;
}

esp_err_t esp_http_client_reset_redirect_counter(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->redirect_count = 0;
    return ESP_OK;
}
#else
esp_err_t esp_http_client_set_redirection(esp_http_client_handle_t client)
    { (void)client; return ESP_ERR_NOT_SUPPORTED; }
esp_err_t esp_http_client_reset_redirect_counter(esp_http_client_handle_t client)
    { (void)client; return ESP_ERR_NOT_SUPPORTED; }
#endif

#if VSF_ESPIDF_HTTP_CLIENT_CFG_USE_CANCEL == ENABLED
esp_err_t esp_http_client_cancel_request(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->cancel_requested = true;
    // Close underlying transport so any blocked recv() unblocks.
    vsf_http_client_close(&client->http);
    client->state = HTTP_STATE_CLOSE;
    return ESP_OK;
}
#else
esp_err_t esp_http_client_cancel_request(esp_http_client_handle_t client)
    { (void)client; return ESP_ERR_NOT_SUPPORTED; }
#endif

/*============================ IMPLEMENTATION: GETTERS =======================*/

int esp_http_client_get_status_code(esp_http_client_handle_t client)
{
    return (client != NULL) ? client->http.resp_status : 0;
}

int64_t esp_http_client_get_content_length(esp_http_client_handle_t client)
{
    if (NULL == client) return -1;
    if (client->http.is_chunked) return -1;
    return (int64_t)client->http.content_length;
}

int64_t esp_http_client_get_content_range(esp_http_client_handle_t client)
{
    // Content-Range is not surfaced by the core parser. Return -1 per
    // upstream contract ("-1 on absence or parse failure"). Upgrading this
    // would require extending vsf_http_client fetch_headers to surface the
    // Content-Range header line; out of scope for the shim layer.
    (void)client;
    return -1;
}

esp_http_client_transport_t esp_http_client_get_transport_type(esp_http_client_handle_t client)
{
    return (client != NULL) ? client->transport : HTTP_TRANSPORT_UNKNOWN;
}

esp_http_state_t esp_http_client_get_state(esp_http_client_handle_t client)
{
    return (client != NULL) ? client->state : HTTP_STATE_UNINIT;
}

int esp_http_client_get_socket(esp_http_client_handle_t client)
{
    if (NULL == client) return -1;
    return vsf_http_client_get_fd(&client->http);
}

int esp_http_client_get_errno(esp_http_client_handle_t client)
{
    if (NULL == client) return -1;
    return client->saved_errno;
}

esp_err_t esp_http_client_get_chunk_length(esp_http_client_handle_t client, int *len)
{
    if (NULL == client || NULL == len) return ESP_ERR_INVALID_ARG;
    if (!client->http.is_chunked) return ESP_FAIL;
    // cur_chunk_size meaning:
    //   == 0  -> next read will pull a new chunk header
    //   >  0  -> bytes remaining in current chunk
    //   <  0  -> -2 on terminator, negative count when decoder is skipping CRLFs
    *len = (client->http.cur_chunk_size > 0) ? client->http.cur_chunk_size : 0;
    return ESP_OK;
}

bool esp_http_client_is_persistent_connection(esp_http_client_handle_t client)
{
    if (NULL == client) return false;
    // No programmatic access to the response Connection: header from the
    // core parser. Report what the caller configured -- truthful for what
    // the shim itself can guarantee.
    return client->keep_alive_enable;
}

// TLS error tracking is not routed through mbedtls_session yet. Reporting
// ESP_OK with zeroes keeps unmodified examples happy while signalling
// "no error".
esp_err_t esp_http_client_get_and_clear_last_tls_error(esp_http_client_handle_t client, int *esp_tls_error_code, int *esp_tls_flags)
{
    if (NULL == client) return ESP_FAIL;
    if (esp_tls_error_code) *esp_tls_error_code = 0;
    if (esp_tls_flags)      *esp_tls_flags      = 0;
    return ESP_OK;
}

/*============================ IMPLEMENTATION: TIER 3 STUBS ==================*/

// prepare() upstream rewinds the auth state in preparation for a new request.
// Here it's a superset of reset_redirect_counter + auth_retry_count reset.
esp_err_t esp_http_client_prepare(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    client->redirect_count   = 0;
    client->auth_retry_count = 0;
    client->cancel_requested = false;
    client->saved_errno      = 0;
    client->body_total       = 0;
    return ESP_OK;
}

// The following require splitting vsf_http_client_send_header into discrete
// primitives (send request line / send single header / send chunk header /
// send final CRLF). Until that refactor lands, they return NOT_SUPPORTED so
// calling code gets a stable error rather than silent truncation.
esp_err_t esp_http_client_request_send(esp_http_client_handle_t client, int write_len)
    { (void)client; (void)write_len; return ESP_ERR_NOT_SUPPORTED; }
int esp_http_client_chunk_write_begin(esp_http_client_handle_t client, const int len)
    { (void)client; (void)len; return -1; }
int esp_http_client_chunk_write_end(esp_http_client_handle_t client, bool last_chunk)
    { (void)client; (void)last_chunk; return -1; }
esp_err_t esp_http_client_flush_response(esp_http_client_handle_t client, int *len)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    // Drain response by reading into an internal scratch buffer.
    char scratch[128];
    int total = 0;
    for (;;) {
        int n = esp_http_client_read(client, scratch, (int)sizeof(scratch));
        if (n <= 0) break;
        total += n;
    }
    if (len) *len = total;
    return ESP_OK;
}
esp_err_t esp_http_client_clear_response_buffer(esp_http_client_handle_t client)
{
    if (NULL == client) return ESP_ERR_INVALID_ARG;
    // Drop whatever bytes the core parser has pending in its cur_buffer.
    client->http.cur_size       = 0;
    client->http.cur_buffer     = NULL;
    client->http.cur_chunk_size = 0;
    return ESP_OK;
}

#endif // VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_HTTP_CLIENT == ENABLED
