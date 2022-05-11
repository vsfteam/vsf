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

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#define __VSF_LINUX_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/arpa/inet.h"
#   include "../../include/pthread.h"
#   include "../../include/sys/select.h"
#else
#   include <unistd.h>
#   include <arpa/inet.h>
#   include <pthread.h>
#   include <sys/select.h>
#endif

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   include "../../include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif

#define __VSF_LINUX_HTTPD_CLASS_IMPLEMENT
#include "./vsf_linux_httpd.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM is needed
#endif

#define MODULE_NAME                             "httpd"

#if VSF_LINUX_HTTPD_CFG_REQUEST_BUFSIZE < 1024
#   warning VSF_LINUX_HTTPD_CFG_REQUEST_BUFSIZE MUST be large enough to hold all \
        http request header and http response header.
#endif

#if VSF_LINUX_HTTPD_CFG_TRACE_REQUEST == ENABLED
#   define vsf_linux_httpd_trace_request(...)   vsf_trace_debug(__VA_ARGS__)
#else
#   define vsf_linux_httpd_trace_request(...)
#endif

#if VSF_LINUX_HTTPD_CFG_TRACE_EVNET == ENABLED
#   define vsf_linux_httpd_trace_event(...)     vsf_trace_debug(__VA_ARGS__)
#else
#   define vsf_linux_httpd_trace_event(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_linux_httpd_strmapper_t {
    union {
        vsf_linux_httpd_response_t response;
        vsf_linux_httpd_mime_t mime;
        vsf_linux_httpd_encoding_t encoding;
    };
    const char *str;
} __vsf_linux_httpd_strmapper_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const char * __vsf_linux_httpd_method[VSF_LINUX_HTTPD_METHOD_NUM + 1] = {
    [VSF_LINUX_HTTPD_GET]                   = "GET ",
    [VSF_LINUX_HTTPD_POST]                  = "POST ",
    // last NULL terminator
    [VSF_LINUX_HTTPD_METHOD_NUM]            = NULL,
};

static const __vsf_linux_httpd_strmapper_t __vsf_linux_httpd_response_mapper[] = {
#define __vsf_linux_httpd_def_response(__response, __str)                       \
    {   .response = (__response),   .str = (__str) }

    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_OK,              "OK"),

    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_BAD_REQUEST,     "Bad Request"),
    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_NOT_FOUND,       "Not Found"),

    __vsf_linux_httpd_def_response(VSF_LINUX_HTTPD_NOT_IMPLEMENT,   "Not Implemented"),
};

static const __vsf_linux_httpd_strmapper_t __vsf_linux_httpd_mime_mapper[] = {
#define __vsf_linux_httpd_def_mime(__mime, __ext)                               \
    {   .mime = (__mime),           .str = (__ext) }

    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_TEXT_HTML,      "html"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_TEXT_HTML,      "htm"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_TEXT_XML,       "xml"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_TEXT_CSS,       "css"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_TEXT_PLAIN,     "txt"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_GIF,      "gif"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_PNG,      "png"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_JPEG,     "jpeg"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_JPEG,     "jpg"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_SVG,      "svg"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_IMAGE_ICON,     "ico"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_VIDEO_AVI,      "avi"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_AUDIO_MPEG,     "mp3"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_AUDIO_WAV,      "wav"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_AUDIO_OGG,      "ogg"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_APP_PDF,        "pdf"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_APP_JS,         "js"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_APP_TAR,        "tar"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_APP_ZIP,        "zip"),
    __vsf_linux_httpd_def_mime(VSF_LINUX_HTTPD_MIME_APP_JSON,       "json"),
};

static const __vsf_linux_httpd_strmapper_t __vsf_linux_httpd_encoding_mapper[] = {
#define __vsf_linux_httpd_def_encoding(__encoding, __ext)                       \
    {   .encoding = (__encoding),   .str = (__ext) }

    __vsf_linux_httpd_def_encoding(VSF_LINUX_HTTPD_ENCODING_GZIP,   "gzip"),
};

static const char * __vsf_linux_httpd_encoding[VSF_LINUX_HTTPD_ENCODING_NUM] = {
    [VSF_LINUX_HTTPD_ENCODING_GZIP - 1]     = "gzip",
};

// valid vsf_linux_httpd_mime_t starts from 1, so minus 1 as index
static const char * __vsf_linux_httpd_mime[VSF_LINUX_HTTPD_MIME_NUM] = {
    [VSF_LINUX_HTTPD_MIME_TEXT_HTML - 1]    = "text/html",
    [VSF_LINUX_HTTPD_MIME_TEXT_XML - 1]     = "text/xml",
    [VSF_LINUX_HTTPD_MIME_TEXT_CSS - 1]     = "text/css",
    [VSF_LINUX_HTTPD_MIME_TEXT_PLAIN - 1]   = "text/plain",
    [VSF_LINUX_HTTPD_MIME_IMAGE_GIF - 1]    = "image/gif",
    [VSF_LINUX_HTTPD_MIME_IMAGE_PNG - 1]    = "image/png",
    [VSF_LINUX_HTTPD_MIME_IMAGE_JPEG - 1]   = "image/jpeg",
    [VSF_LINUX_HTTPD_MIME_IMAGE_SVG - 1]    = "image/svg+xml",
    [VSF_LINUX_HTTPD_MIME_IMAGE_ICON - 1]   = "image/x-icon",
    [VSF_LINUX_HTTPD_MIME_VIDEO_AVI - 1]    = "video/x-msvideo",
    [VSF_LINUX_HTTPD_MIME_AUDIO_MPEG - 1]   = "audio/mpeg",
    [VSF_LINUX_HTTPD_MIME_AUDIO_WAV - 1]    = "audio/x-wav",
    [VSF_LINUX_HTTPD_MIME_AUDIO_OGG - 1]    = "audio/x-oggvoribs",
    [VSF_LINUX_HTTPD_MIME_APP_PDF - 1]      = "application/pdf",
    [VSF_LINUX_HTTPD_MIME_APP_JS - 1]       = "application/x-javascript",
    [VSF_LINUX_HTTPD_MIME_APP_TAR - 1]      = "application/x-tar",
    [VSF_LINUX_HTTPD_MIME_APP_ZIP - 1]      = "application/zip",
    [VSF_LINUX_HTTPD_MIME_APP_JSON - 1]     = "application/json",
};

// valid vsf_linux_httpd_charset_t starts from 1, so minus 1 as index
static const char * __vsf_linux_httpd_charset[VSF_LINUX_HTTPD_CHARSET_NUM] = {
    [VSF_LINUX_HTTPD_CHARSET_UTF8 - 1]      = "utf8",
};

/*============================ IMPLEMENTATION ================================*/

// urihandler
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED

/*============================ MACROS ========================================*/

#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
#   include "./urihandler/file/__vsf_linux_urihandler_file.inc"
#endif

/*============================ MACROS ========================================*/

#define __VSF_LINUX_HTTPD_USE_DEFAULT_URIHANDLER

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_httpd_urihandler_t __vsf_linux_httpd_urihandler[] = {
#if VSF_LINUX_HTTPD_CFG_FILESYSTEM == ENABLED
    {
        .match          = VSF_LINUX_HTTPD_URI_MATCH_ANY,
        .type           = VSF_LINUX_HTTPD_URI_OP,
        .op             = &__vsf_linux_httpd_urihandler_file_op,
    },
#endif
};

#endif

// response
#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static const char * __vsf_linux_httpd_get_response_str(vsf_linux_httpd_response_t response)
{
    for (int i = 0; i < dimof(__vsf_linux_httpd_response_mapper); i++) {
        if (__vsf_linux_httpd_response_mapper[i].response == response) {
            return __vsf_linux_httpd_response_mapper[i].str;
        }
    }
    // if assert here, add corresponding response to __vsf_linux_httpd_response_mapper
    VSF_LINUX_ASSERT(false);
    // statement is unreachable in IAR
    return "UNKNOWN";
}

static const char * __vsf_linux_httpd_get_mime_str(vsf_linux_httpd_mime_t type)
{
    // skip VSF_LINUX_HTTPD_MIME_INVALID
    if (    (type > VSF_LINUX_HTTPD_MIME_INVALID)
        &&  (type <= dimof(__vsf_linux_httpd_mime))) {
        return __vsf_linux_httpd_mime[type - 1];
    }
    // if assert here, add corresponding mime to __vsf_linux_httpd_mime
    VSF_LINUX_ASSERT(false);
    // statement is unreachable in IAR
    return "UNKNOWN";
}

static const char * __vsf_linux_httpd_get_charset_str(vsf_linux_httpd_charset_t charset)
{
    // skip VSF_LINUX_HTTPD_CHARSET_INVALID
    if (    (charset > VSF_LINUX_HTTPD_CHARSET_INVALID)
        &&  (charset <= dimof(__vsf_linux_httpd_charset))) {
        return __vsf_linux_httpd_charset[charset - 1];
    }
    // if assert here, add corresponding charset to __vsf_linux_httpd_charset
    VSF_LINUX_ASSERT(false);
    // statement is unreachable in IAR
    return "UNKNOWN";
}

static const char * __vsf_linux_httpd_get_encoding_str(vsf_linux_httpd_encoding_t encoding)
{
    // skip VSF_LINUX_HTTPD_INVALID_INVALID
    if (    (encoding > VSF_LINUX_HTTPD_ENCODING_INVALID)
        &&  (encoding <= dimof(__vsf_linux_httpd_encoding))) {
        return __vsf_linux_httpd_encoding[encoding - 1];
    }
    // if assert here, add corresponding charset to __vsf_linux_httpd_encoding
    VSF_LINUX_ASSERT(false);
    // statement is unreachable in IAR
    return "UNKNOWN";
}

static vsf_linux_httpd_mime_t __vsf_linux_httpd_get_mime_by_ext(char *ext)
{
    for (int i = 0; i < dimof(__vsf_linux_httpd_mime_mapper); i++) {
        if (!strcasecmp(__vsf_linux_httpd_mime_mapper[i].str, ext)) {
            return __vsf_linux_httpd_mime_mapper[i].mime;
        }
    }
    return VSF_LINUX_HTTPD_MIME_INVALID;
}

// request

static void __vsf_linux_httpd_parse_header_item(char *items,
                uintalu_t *bitmap, uint_fast16_t bitsize,
                const char *item_arr[], uint_fast16_t item_arr_size)
{
    char ch, *tmp_ptr;
    for (int i = 0; i < item_arr_size; i++) {
        tmp_ptr = strstr(items, item_arr[i]);
        if (tmp_ptr != NULL) {
            ch = tmp_ptr[strlen(item_arr[i])];
            if ((ch != ',') && (ch != ' ') && (ch != ';') && (ch != '\0')) {
                break;
            }
            if (tmp_ptr != items) {
                ch = tmp_ptr[-1];
                if ((ch != ',') && (ch != ' ') && (ch != ';')) {
                    break;
                }
            }

            // valid mime start from 1
            vsf_bitmap_set(bitmap, i + 1);
        }
    }
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization
#   pragma diag_suppress=pe546
#endif

static vsf_err_t __vsf_linux_httpd_parse_request(vsf_linux_httpd_request_t *request)
{
    // add NULL terminator to request->buffer
    char *cur_ptr = (char *)request->buffer, *end_ptr, *tmp_ptr, *tmp_end_ptr;
    uint_fast32_t size = vsf_stream_get_data_size(request->stream_in);

    // one more byte has already been reserved
    VSF_LINUX_ASSERT(size <= (sizeof(request->buffer) - 1));
    // append last NULL terminator, so no need to check overflow later
    cur_ptr[size] = '\0';
    end_ptr = strstr((const char *)cur_ptr, "\r\n\r\n");
    if (NULL == end_ptr) {
        return VSF_ERR_NOT_READY;
    }
    end_ptr += 4;
    size = end_ptr - cur_ptr;

    vsf_linux_httpd_trace_request(                                              \
        "****************************** HTTP REQUEST ******************************"\
        VSF_TRACE_CFG_LINEEND);

    // http request header ready, parse into request
    // 1. http request method/uri/query/protocol
    // 1.1 http request method
    for (int i = 0; i < dimof(__vsf_linux_httpd_method); i++) {
        if (NULL == __vsf_linux_httpd_method[i]) {
        __not_implement:
            request->response = VSF_LINUX_HTTPD_NOT_IMPLEMENT;
            return VSF_ERR_FAIL;
        }
        if (!strncasecmp((const char *)cur_ptr, __vsf_linux_httpd_method[i],
                            strlen(__vsf_linux_httpd_method[i]))) {
            cur_ptr += strlen(__vsf_linux_httpd_method[i]);
            while (' ' == *cur_ptr) {
                cur_ptr++;
            }
            request->method = i;
            break;
        }
    }
    // 1.2 http request uri
    request->uri = cur_ptr;
    cur_ptr = strchr((const char *)cur_ptr, ' ');
    if (NULL == cur_ptr) {
    __bad_request:
        request->response = VSF_LINUX_HTTPD_BAD_REQUEST;
        return VSF_ERR_FAIL;
    }
    *cur_ptr++ = '\0';
    while (' ' == *cur_ptr) {
        cur_ptr++;
    }
    // 1.3 http uri query
    tmp_ptr = strchr((const char *)request->uri, '?');
    if (tmp_ptr != NULL) {
        *tmp_ptr++ = '\0';
        while (' ' == *tmp_ptr) {
            tmp_ptr++;
        }
    }
    request->query = tmp_ptr;
    // 1.4 http request protocol
    if (strncasecmp((const char *)cur_ptr, "HTTP/1.", sizeof("HTTP/1.") - 1)) {
        // PE546 in IAR: transfer of control bypasses initialization of: i
        goto __not_implement;
    }
    cur_ptr = strstr((const char *)cur_ptr, "\r\n");
    if (NULL == cur_ptr) {
        goto __bad_request;
    }
    cur_ptr += 2;

    vsf_linux_httpd_trace_request("uri: %s" VSF_TRACE_CFG_LINEEND, request->uri);
    if (request->query != NULL) {
        vsf_linux_httpd_trace_request("    query: %s" VSF_TRACE_CFG_LINEEND, request->query);
    }

    // 2. htte request header
    while (cur_ptr < end_ptr) {
        tmp_end_ptr = strstr((const char *)cur_ptr, "\r\n");
        if (tmp_end_ptr == cur_ptr) {
            // end of request
            cur_ptr += 2;
            break;
        } else if (NULL == tmp_end_ptr) {
            goto __bad_request;
        }
        *tmp_end_ptr = '\0';
        tmp_end_ptr += 2;

        vsf_linux_httpd_trace_request("%s" VSF_TRACE_CFG_LINEEND, cur_ptr);

        tmp_ptr = cur_ptr;
        cur_ptr = strchr((const char *)cur_ptr, ':');
        if (NULL == cur_ptr) {
            goto __bad_request;
        }
        *cur_ptr++ = '\0';
        while (' ' == *cur_ptr) {
            cur_ptr++;
        }

        if (!strcasecmp((const char *)tmp_ptr, "Content-Length")) {
            request->content_length = atoi(cur_ptr);
        } else if (!strcasecmp((const char *)tmp_ptr, "Range")) {
            // PE546 in IAR: transfer of control bypasses initialization of: i
            goto __not_implement;
        } else if (!strcasecmp((const char *)tmp_ptr, "Connection")) {
            if (!strcasecmp((const char *)cur_ptr, "close")) {
                // no need to set to false, becasue it's default value
            } else if (!strcasecmp((const char *)cur_ptr, "Keep-Alive")) {
                request->keep_alive = true;
            } else {
                goto __bad_request;
            }
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept")) {
            __vsf_linux_httpd_parse_header_item(cur_ptr,
                        request->mime_map, VSF_LINUX_HTTPD_MIME_NUM,
                        __vsf_linux_httpd_mime, dimof(__vsf_linux_httpd_mime));
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Encoding")) {
            __vsf_linux_httpd_parse_header_item(cur_ptr,
                        request->encoding_map, VSF_LINUX_HTTPD_ENCODING_NUM,
                        __vsf_linux_httpd_encoding, dimof(__vsf_linux_httpd_encoding));
        } else if (!strcasecmp((const char *)tmp_ptr, "Host")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "User-Agent")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Cookie")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Charset")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Accept-Language")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Refer")) {
        } else if (!strcasecmp((const char *)tmp_ptr, "Authorization")) {
        }

        cur_ptr = tmp_end_ptr;
    }

    // read http request header out of stream_in
    vsf_stream_read(request->stream_in, NULL, size);
    return VSF_ERR_NONE;
}

static vsf_linux_httpd_urihandler_t * __vsf_linux_httpe_parse_uri_in_list(
                    vsf_linux_httpd_urihandler_t *urihandler,
                    uint_fast16_t num_of_handler,
                    char *uri)
{
    char *ext = strrchr(uri, '.');
    if (ext != NULL) {
        ext++;
    }

    for (uint_fast16_t i = 0; i < num_of_handler; i++, urihandler++) {
        if (    ((urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_EXT) && (ext != NULL) && !strcmp(ext, urihandler->ext))
            ||  ((urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_URI) && !strcmp(uri, urihandler->uri))
            ||  (urihandler->match & VSF_LINUX_HTTPD_URI_MATCH_ANY)) {
            return urihandler;
        }
    }
    return NULL;
}

static vsf_linux_httpd_urihandler_t * __vsf_linux_httpd_parse_uri(vsf_linux_httpd_session_t *session, char *uri)
{
    vsf_linux_httpd_t *httpd = session->httpd;
    vsf_linux_httpd_urihandler_t * urihandler = __vsf_linux_httpe_parse_uri_in_list(httpd->urihandler, httpd->num_of_urihandler, uri);

#ifdef __VSF_LINUX_HTTPD_USE_DEFAULT_URIHANDLER
    if (NULL == urihandler) {
        urihandler = __vsf_linux_httpe_parse_uri_in_list((vsf_linux_httpd_urihandler_t *)__vsf_linux_httpd_urihandler, dimof(__vsf_linux_httpd_urihandler), uri);
    }
#endif

    return urihandler;
}

// session
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
imp_vsf_pool(vsf_linux_httpd_session_pool, vsf_linux_httpd_session_t)
#endif

static void __vsf_linux_httpd_session_delete(vsf_linux_httpd_session_t *session)
{
    vsf_linux_httpd_t *httpd = session->httpd;

    if (session->fd_stream_in >= 0) {
        close(session->fd_stream_in);
    }
    if (session->fd_stream_out >= 0) {
        close(session->fd_stream_out);
    }
    if (session->fd_socket >= 0) {
        close(session->fd_socket);
    }

    vsf_dlist_remove(vsf_linux_httpd_session_t, session_node, &httpd->session_list, session);
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    VSF_POOL_FREE(vsf_linux_httpd_session_pool, &httpd->session_pool, session);
#else
    free(session);
#endif
}

static void __vsf_linux_httpd_send_response(vsf_linux_httpd_session_t *session)
{
    vsf_stream_t *stream = session->request.stream_out;

#define vsf_stream_write_str(__stream, __str)                                   \
            do {                                                                \
                vsf_stream_write((__stream), (uint8_t *)(__str), strlen(__str));\
            } while (false)
#define vsf_stream_write_int(__stream, __int)                                   \
            do {                                                                \
                char __int_buffer[16];                                          \
                itoa((__int), __int_buffer, 10);                                \
                vsf_stream_write((__stream), (uint8_t *)(__int_buffer),         \
                    strlen(__int_buffer));                                      \
            } while (false)

    vsf_stream_write_str(stream, "HTTP/1.1 ");
    vsf_stream_write_int(stream, session->request.response);
    vsf_stream_write_str(stream, " ");
    vsf_stream_write_str(stream,
                    __vsf_linux_httpd_get_response_str(session->request.response));
    vsf_stream_write_str(stream, "\r\n");

    if (session->request.mime != VSF_LINUX_HTTPD_MIME_INVALID) {
        vsf_stream_write_str(stream, "Content-Type: ");
        vsf_stream_write_str(stream,
                    __vsf_linux_httpd_get_mime_str(session->request.mime));
        if (session->request.charset != VSF_LINUX_HTTPD_CHARSET_INVALID) {
            vsf_stream_write_str(stream, "; charset=");
            vsf_stream_write_str(stream,
                    __vsf_linux_httpd_get_charset_str(session->request.charset));
        }
        vsf_stream_write_str(stream, "\r\n");
    }
    if (session->request.encoding != VSF_LINUX_HTTPD_ENCODING_INVALID) {
        vsf_stream_write_str(stream, "Content-Encoding: ");
        vsf_stream_write_str(stream,
                    __vsf_linux_httpd_get_encoding_str(session->request.encoding));
        vsf_stream_write_str(stream, "\r\n");
    }
    if (VSF_LINUX_HTTPD_OK == session->request.response) {
        vsf_stream_write_str(stream, "Content-Length: ");
        vsf_stream_write_int(stream, session->request.content_length);
        vsf_stream_write_str(stream, "\r\n");
    }
    vsf_stream_write_str(stream, "\r\n");
}

static void __vsf_linux_httpd_stream_evthandler(vsf_stream_t *no_used, void *param, vsf_stream_evt_t evt)
{
    vsf_linux_httpd_session_t *session = param;
    vsf_err_t err;

    switch (evt) {
    case VSF_STREAM_ON_RX:
        // stream_write is called in httpd thread, so can use all linux APIs here
        err = __vsf_linux_httpd_parse_request(&session->request);
        if (err < 0) {
            vsf_mem_stream_t *stream;
        __error:
            stream = &session->request.urihandler_ctx.header.stream;
            stream->op = &vsf_mem_stream_op;
            stream->buffer = session->request.buffer;
            stream->size = sizeof(session->request.buffer);
            stream->align = 0;
            VSF_STREAM_INIT(stream);

            vsf_linux_fd_t *sfd = vsf_linux_rx_stream(&stream->use_as__vsf_stream_t);
            if (NULL == sfd) {
                session->fatal_error = true;
                break;
            }

            session->request.stream_out = &stream->use_as__vsf_stream_t;
            session->fd_stream_out = sfd->fd;
            __vsf_linux_httpd_send_response(session);
            session->request.is_stream_out_started = true;
        } else if (VSF_ERR_NONE == err) {
            // request parsed, close stream_in(note that there maybe data in stream_in)
            char *ext;
            uint8_t *ptr;
            uint_fast32_t size = vsf_stream_get_rbuf(session->request.stream_in, &ptr);
            char *uri = session->request.uri;

            session->request.stream_in = NULL;
            session->wait_stream_in = false;

            ext = strrchr(uri, '.');
            if (ext != NULL) {
                ext++;
                session->request.mime = __vsf_linux_httpd_get_mime_by_ext(ext);
            }

            // find a suitable urihandler, and pass ptr/size to urihandler init
            vsf_linux_httpd_urihandler_t *urihandler;
            while (true) {
                urihandler = __vsf_linux_httpd_parse_uri(session, uri);
                if (NULL == urihandler) {
                    session->request.response = VSF_LINUX_HTTPD_NOT_FOUND;
                    vsf_trace_error(MODULE_NAME ": handler for uri not found: %s" VSF_TRACE_CFG_LINEEND, uri);
                    goto __error;
                }
                if (VSF_LINUX_HTTPD_URI_REMAP == urihandler->type) {
                    uri = urihandler->target_uri;
                    ext = strrchr(uri, '.');
                    if (ext != NULL) {
                        ext++;

                        // try to parse mime again if not parsed before
                        if (VSF_LINUX_HTTPD_MIME_INVALID == session->request.mime) {
                            session->request.mime = __vsf_linux_httpd_get_mime_by_ext(ext);
                        }

                        // if remap to a compressed format, check and set content-encoding
                        for (int i = 0; i < dimof(__vsf_linux_httpd_encoding_mapper); i++) {
                            if (!strcasecmp(__vsf_linux_httpd_encoding_mapper[i].str, ext)) {
                                session->request.encoding = __vsf_linux_httpd_encoding_mapper[i].encoding;
                                break;
                            }
                        }
                    }
                    continue;
                }
                break;
            }
            session->request.uri = uri;

            if (VSF_ERR_NONE != urihandler->op->init_fn(&session->request, ptr, size)) {
                vsf_trace_error(MODULE_NAME ": fail to initialize request for %s, %s" VSF_TRACE_CFG_LINEEND,
                                    uri, __vsf_linux_httpd_get_response_str(session->request.response));
                goto __error;
            }
            if (    (session->request.mime != VSF_LINUX_HTTPD_MIME_INVALID)
                &&  !vsf_bitmap_get(session->request.mime_map, session->request.mime)) {
                session->request.response = VSF_LINUX_HTTPD_NOT_ACCEPTABLE;
            }
            session->request.urihandler = urihandler;

            vsf_linux_fd_t *sfd;
            vsf_stream_t *stream;

            stream = session->request.stream_in;
            if (stream != NULL) {
                sfd = vsf_linux_tx_stream(stream);
                if (NULL == sfd) {
                    vsf_trace_error(MODULE_NAME ": fail to create fd for stream_in." VSF_TRACE_CFG_LINEEND);
                    session->fatal_error = true;
                    break;
                }
                session->fd_stream_in = sfd->fd;
            }

            stream = session->request.stream_out;
            VSF_LINUX_ASSERT(stream != NULL);
            sfd = vsf_linux_rx_stream(stream);
            if (NULL == sfd) {
                vsf_trace_error(MODULE_NAME ": fail to create fd for stream_out." VSF_TRACE_CFG_LINEEND);
                session->fatal_error = true;
                break;
            }
            session->fd_stream_out = sfd->fd;

            // write response header to stream_out
            __vsf_linux_httpd_send_response(session);

            if (VSF_ERR_NONE != urihandler->op->serve_fn(&session->request)) {
                vsf_trace_error(MODULE_NAME ": fail to serve urihandler." VSF_TRACE_CFG_LINEEND);
                vsf_stream_disconnect_tx(session->request.stream_out);
                break;
            }
        }
        break;
    }
}

static void __vsf_linux_httpd_session_reset_reuqest(vsf_linux_httpd_session_t *session)
{
    if (session->fd_stream_in >= 0) {
        close(session->fd_stream_in);
        session->fd_stream_in = -1;
    }
    if (session->fd_stream_out >= 0) {
        close(session->fd_stream_out);
        session->fd_stream_out = -1;
    }

    memset(&session->request, 0, sizeof(session->request));

    // setup context for handling http header
    vsf_mem_stream_t *stream = &session->request.urihandler_ctx.header.stream;
    stream->op = &vsf_mem_stream_op;
    stream->buffer = session->request.buffer;
    // reserve one byte for NULL terminator
    stream->size = sizeof(session->request.buffer) - 1;
    stream->rx.evthandler = __vsf_linux_httpd_stream_evthandler;
    stream->rx.param = session;
    VSF_STREAM_INIT(stream);
    VSF_STREAM_CONNECT_RX(stream);

    session->wait_stream_out = session->wait_stream_in = false;
    session->request.stream_in = &stream->use_as__vsf_stream_t;
}

static vsf_linux_httpd_session_t * __vsf_linux_httpd_session_new(vsf_linux_httpd_t *httpd)
{
    vsf_linux_httpd_session_t *session;
#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    session = VSF_POOL_ALLOC(vsf_linux_httpd_session_pool, &httpd->session_pool);
#else
    session = malloc(sizeof(vsf_linux_httpd_session_t));
#endif

    if (session != NULL) {
        session->fd_stream_out = session->fd_stream_in = -1;
        __vsf_linux_httpd_session_reset_reuqest(session);
        session->fd_socket = -1;
        session->httpd = httpd;
        session->fatal_error = false;
        vsf_dlist_init_node(vsf_linux_httpd_session_t, session_node, session);
        vsf_dlist_add_to_head(vsf_linux_httpd_session_t, session_node, &httpd->session_list, session);
    }
    return session;
}

// httpd
static int __vsf_linux_httpd_set_fds(vsf_linux_httpd_t *httpd, fd_set *rset, fd_set *wset)
{
    int fd_max = -1;
    __vsf_dlist_foreach_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
        if (_->request.stream_out != NULL) {
            if (!_->wait_stream_out) {
                if (_->fd_socket > fd_max) {
                    fd_max = _->fd_socket;
                }
                FD_SET(_->fd_socket, wset);
            } else if(_->fd_stream_out >= 0) {
                if (_->fd_stream_out > fd_max) {
                    fd_max = _->fd_stream_out;
                }
                FD_SET(_->fd_stream_out, rset);
            }
        }
        if (_->request.stream_in != NULL) {
            if (!_->wait_stream_in) {
                if (_->fd_socket > fd_max) {
                    fd_max = _->fd_socket;
                }
                FD_SET(_->fd_socket, rset);
            } else if(_->fd_stream_in >= 0) {
                if (_->fd_stream_in > fd_max) {
                    fd_max = _->fd_stream_in;
                }
                FD_SET(_->fd_stream_in, wset);
            }
        }
    }
    return fd_max;
}

static void * __vsf_linux_httpd_thread(void *param)
{
    vsf_linux_httpd_t *httpd = param;
    int fd_listen, fd_num, fd_socket;
    fd_set rfds, wfds;
    struct sockaddr_in host_addr;
    vsf_stream_t *stream;

#if defined(VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE) && (VSF_LINUX_HTTPD_CFG_SESSION_POLL_SIZE > 0)
    VSF_POOL_PREPARE(vsf_linux_httpd_session_pool, &httpd->session_pool);
    VSF_POOL_ADD_BUFFER(vsf_linux_httpd_session_pool, &httpd->session_pool, &httpd->sessions, sizeof(httpd->sessions));
#endif

    fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_listen < 0) {
        vsf_trace_error(MODULE_NAME ": fail to cerate listen socket." VSF_TRACE_CFG_LINEEND);
        goto __exit;
    }

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_port = htons(httpd->port);
    if (bind(fd_listen, (struct sockaddr *)&host_addr, sizeof(host_addr))) {
        vsf_trace_error(MODULE_NAME ": fail to bind socket to port %d." VSF_TRACE_CFG_LINEEND, httpd->port);
        goto __close_fd_and_exit;
    }

    if (listen(fd_listen, httpd->backlog)) {
        vsf_trace_error(MODULE_NAME ": fail to listen." VSF_TRACE_CFG_LINEEND);
        goto __close_fd_and_exit;
    }

    while (true) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(fd_listen, &rfds);
        fd_num = __vsf_linux_httpd_set_fds(httpd, &rfds, &wfds);
        fd_num = vsf_max(fd_num, fd_listen) + 1;
        fd_num = select(fd_num, &rfds, &wfds, NULL, NULL);
        if (fd_num < 0) {
            vsf_trace_error(MODULE_NAME ": fail to select." VSF_TRACE_CFG_LINEEND);
            goto __close_session_fd_and_exit;
        } else if (0 == fd_num) {
            continue;
        }

        if (FD_ISSET(fd_listen, &rfds)) {
            FD_CLR(fd_listen, &rfds);
            fd_num--;

            vsf_linux_httpd_trace_event(MODULE_NAME ": connect event." VSF_TRACE_CFG_LINEEND);

            struct sockaddr_in client_addr;
            socklen_t client_addr_len;
            fd_socket = accept(fd_listen, (struct sockaddr *)&client_addr, &client_addr_len);
            if (fd_socket >= 0) {
                struct vsf_linux_httpd_session_t *session = __vsf_linux_httpd_session_new(httpd);
                if (NULL == session) {
                    close(fd_socket);
                } else {
                    session->client_addr = client_addr;
                    session->fd_socket = fd_socket;
                }
            }
        }

        __vsf_dlist_foreach_next_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
            VSF_LINUX_ASSERT(fd_num >= 0);
            if (0 == fd_num) {
                break;
            }

            VSF_LINUX_ASSERT(_->fd_socket >= 0);

            uint8_t *ptr;
            uint_fast32_t size;
            bool is_socket_accessable, is_stream_accessable;

            // can write to stream_in or can read from socket, mutual exclusive events
            is_socket_accessable = FD_ISSET(_->fd_socket, &rfds);
            is_stream_accessable = (_->fd_stream_in >= 0) && FD_ISSET(_->fd_stream_in, &wfds);
            if (is_socket_accessable || is_stream_accessable) {
                VSF_LINUX_ASSERT(   (is_socket_accessable && !is_stream_accessable)
                                ||  (!is_socket_accessable && is_stream_accessable));
                if (is_socket_accessable) {
                    vsf_linux_httpd_trace_event(MODULE_NAME ": socket rx event." VSF_TRACE_CFG_LINEEND);
                } else {
                    vsf_linux_httpd_trace_event(MODULE_NAME ": stream tx event." VSF_TRACE_CFG_LINEEND);
                }
                fd_num--;

                stream = _->request.stream_in;
                VSF_LINUX_ASSERT(stream != NULL);

                size = vsf_stream_get_wbuf(stream, &ptr);
                if (size > 0) {
                    ssize_t realsize = read(_->fd_socket, ptr, size);
                    if (0 == realsize) {
                        // socket closed by remote
                        vsf_linux_httpd_trace_event(MODULE_NAME ": socket disconnect event." VSF_TRACE_CFG_LINEEND);
                        __vsf_linux_httpd_session_delete(_);
                        continue;
                    } else if (realsize < 0) {
                        vsf_trace_error(MODULE_NAME ": fail to read socket." VSF_TRACE_CFG_LINEEND);
                        __vsf_linux_httpd_session_delete(_);
                        continue;
                    }
#if VSF_LINUX_HTTPD_CFG_TRACE_EVNET == ENABLED
                    vsf_trace_buffer(VSF_TRACE_DEBUG, ptr, realsize);
#endif
                    if (_->fd_stream_in < 0) {
                        // no fd_stream_in while waiting for http request
                        vsf_stream_write(stream, NULL, realsize);
                        if (_->fatal_error) {
                            __vsf_linux_httpd_session_delete(_);
                            continue;
                        }
                    } else {
                        write(_->fd_stream_in, ptr, realsize);
                    }
                    _->wait_stream_in = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_accessable);
                    _->wait_stream_in = true;
                }
            }

            // can read from stream_out or can write to socket, mutual exclusive events
            is_socket_accessable = FD_ISSET(_->fd_socket, &wfds);
            is_stream_accessable = (_->fd_stream_out >= 0) && FD_ISSET(_->fd_stream_out, &rfds);
            if (is_socket_accessable || is_stream_accessable) {
                VSF_LINUX_ASSERT(   (is_socket_accessable && !is_stream_accessable)
                                ||  (!is_socket_accessable && is_stream_accessable));
                if (is_socket_accessable) {
                    vsf_linux_httpd_trace_event(MODULE_NAME ": socket tx event." VSF_TRACE_CFG_LINEEND);
                } else {
                    vsf_linux_httpd_trace_event(MODULE_NAME ": stream rx event." VSF_TRACE_CFG_LINEEND);
                }
                fd_num--;

                stream = _->request.stream_out;
                VSF_LINUX_ASSERT(stream != NULL);

                size = vsf_stream_get_rbuf(stream, &ptr);
                if (size > 0) {
                    ssize_t realsize = write(_->fd_socket, ptr, size);
                    if (realsize < 0) {
                        vsf_trace_error(MODULE_NAME ": fail to write socket." VSF_TRACE_CFG_LINEEND);
                        __vsf_linux_httpd_session_delete(_);
                        continue;
                    }
#if VSF_LINUX_HTTPD_CFG_TRACE_EVNET == ENABLED
                    vsf_trace_buffer(VSF_TRACE_DEBUG, ptr, realsize);
#endif
                    VSF_LINUX_ASSERT(_->fd_stream_out >= 0);
                    read(_->fd_stream_out, ptr, realsize);
                    _->wait_stream_out = false;
                } else {
                    VSF_LINUX_ASSERT(!is_stream_accessable);

                    if (!_->request.is_stream_out_started || vsf_stream_is_tx_connected(stream)) {
                        _->wait_stream_out = true;
                    } else {
                        // tx side of stream_out is disconnected, session end
                        if (_->request.urihandler != NULL) {
                            _->request.urihandler->op->fini_fn(&_->request);
                        }

                        if (_->request.keep_alive) {
                            __vsf_linux_httpd_session_reset_reuqest(_);
                        } else {
                            vsf_linux_httpd_trace_event(MODULE_NAME ": socket disconnect event." VSF_TRACE_CFG_LINEEND);
                            __vsf_linux_httpd_session_delete(_);
                        }
                    }
                }
            }
        }
    }

__close_session_fd_and_exit:
    __vsf_dlist_foreach_next_unsafe(vsf_linux_httpd_session_t, session_node, &httpd->session_list) {
        if (_->request.urihandler != NULL) {
            _->request.urihandler->op->fini_fn(&_->request);
        }
        __vsf_linux_httpd_session_delete(_);
    }
__close_fd_and_exit:
    if (fd_listen >= 0) {
        close(fd_listen);
    }
__exit:
    return NULL;
}

vsf_err_t vsf_linux_httpd_start(vsf_linux_httpd_t *httpd)
{
    return 0 == pthread_create(NULL, NULL, __vsf_linux_httpd_thread, httpd) ?
                VSF_ERR_NONE : VSF_ERR_FAIL;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET && VSF_LINUX_SOCKET_USE_INET
