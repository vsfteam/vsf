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

#include <mbedtls/sha1.h>
#include <mbedtls/base64.h>

/*============================ MACROS ========================================*/

#if VSF_USE_MBEDTLS != ENABLED
#   error sha1 in mbedtls is used as handshake algo, please enable VSF_USE_MBEDTLS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    WEBSOCKET_STATE_CONNECTING,
    WEBSOCKET_STATE_CONNECTTED,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size);
static vsf_err_t __vsf_linux_httpd_urihandler_websocket_fini(vsf_linux_httpd_request_t *req);
static vsf_err_t __vsf_linux_httpd_urihandler_websocket_serve(vsf_linux_httpd_request_t *req);
static void __vsf_linux_httpd_urihandler_socket_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);

/*============================ LOCAL VARIABLES ===============================*/

static char __websocket_magic[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_httpd_urihandler_op_t vsf_linux_httpd_urihandler_websocket_op = {
    .init_fn                = __vsf_linux_httpd_urihandler_websocket_init,
    .fini_fn                = __vsf_linux_httpd_urihandler_websocket_fini,
    .serve_fn               = __vsf_linux_httpd_urihandler_websocket_serve,
    .stream_evthandler_fn   = __vsf_linux_httpd_urihandler_socket_stream_evthandler,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_linux_httpd_urihandler_socket_stream_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vsf_linux_httpd_request_t *req = param;
    const vsf_linux_httpd_urihandler_t *urihandler = req->urihandler;
    vsf_linux_httpd_urihandler_websocket_t *urihandler_websocket = &req->urihandler_ctx.websocket;

    switch (evt) {
    case VSF_STREAM_ON_IN:
        switch (urihandler_websocket->state) {
        case WEBSOCKET_STATE_CONNECTTED:
            if (vsf_stream_get_data_size(stream) > 2) {
                uint16_t header;
                vsf_stream_read(stream, (uint8_t *)&header, 2);
            }
            break;
        }
        break;
    case VSF_STREAM_ON_OUT:
        switch (urihandler_websocket->state) {
        case WEBSOCKET_STATE_CONNECTING:
            if (!vsf_stream_get_data_size(stream)) {
                urihandler_websocket->state = WEBSOCKET_STATE_CONNECTTED;
                if (urihandler->websocket.on_open != NULL) {
                    urihandler->websocket.on_open(req);
                }
            }
            break;
        }
        break;
    }
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size)
{
    VSF_LINUX_ASSERT((req != NULL) && (req->uri != NULL));
    if (!req->websocket || (NULL == req->websocket_key)) {
        req->response = VSF_LINUX_HTTPD_BAD_REQUEST;
        return VSF_ERR_FAIL;
    }

    vsf_linux_httpd_urihandler_websocket_t *urihandler_websocket = &req->urihandler_ctx.websocket;
    vsf_fifo_stream_t *stream;
    int bufsize = sizeof(req->buffer) / 2;

    urihandler_websocket->state = WEBSOCKET_STATE_CONNECTING;

    stream = &urihandler_websocket->stream_out;
    stream->op = &vsf_fifo_stream_op;
    stream->buffer = req->buffer;
    stream->size = bufsize;
    VSF_STREAM_INIT(stream);
    stream->tx.param = req;
    VSF_STREAM_CONNECT_TX(stream);
    req->is_stream_out_started = true;
    req->stream_out = &stream->use_as__vsf_stream_t;

    stream = &urihandler_websocket->stream_in;
    stream->op = &vsf_fifo_stream_op;
    stream->buffer = req->buffer + bufsize;
    stream->size = bufsize;
    VSF_STREAM_INIT(stream);
    stream->rx.param = req;
    VSF_STREAM_CONNECT_RX(stream);
    req->stream_in = &stream->use_as__vsf_stream_t;

    vsf_stream_write_str(req->stream_out, "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Accept: ");

    char secbuf[32 + sizeof(__websocket_magic)];
    strcpy(secbuf, req->websocket_key);
    strcat(secbuf, __websocket_magic);
    free(req->websocket_key);
    req->websocket_key = NULL;

    mbedtls_sha1_context ctx;
    uint8_t sha1_output[20];
    mbedtls_sha1_init(&ctx);
    mbedtls_sha1_starts(&ctx);
    mbedtls_sha1_update(&ctx, (const unsigned char *)secbuf, strlen(secbuf));
    mbedtls_sha1_finish(&ctx, sha1_output);
    mbedtls_sha1_free(&ctx);

    size_t olen;
    mbedtls_base64_encode((unsigned char *)secbuf, sizeof(secbuf), &olen,
            (const unsigned char *)sha1_output, sizeof(sha1_output));
    vsf_stream_write_str(req->stream_out, secbuf);
    vsf_stream_write_str(req->stream_out, "\r\n\r\n");

    req->response = VSF_LINUX_HTTPD_OK;
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_fini(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_serve(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}
