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
    WEBSOCKET_STATE_PARSE_HEADER = WEBSOCKET_STATE_CONNECTTED,
    WEBSOCKET_STATE_PARSE_REALLEN,
    WEBSOCKET_STATE_PARSE_MASKING,
    WEBSOCKET_STATE_PARSE_DATA,
    WEBSOCKET_STATE_CLOSING,
    WEBSOCKET_STATE_CLOSED,
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
    uint_fast32_t size;
    uint8_t header[8];

    switch (evt) {
    case VSF_STREAM_ON_IN:
        size = vsf_stream_get_data_size(stream);
        switch (urihandler_websocket->state) {
        case WEBSOCKET_STATE_PARSE_HEADER:
        parse_header:
            if (size >= 2) {
                size -= vsf_stream_read(stream, header, 2);
                urihandler_websocket->is_fin = !!(header[0] & 0x80);
                urihandler_websocket->opcode = header[0] & 0x0F;
                switch (urihandler_websocket->opcode) {
                case 0:
                    urihandler_websocket->is_start = false;
                parse_len:
                    urihandler_websocket->is_masking = !!(header[1] & 0x80);
                    header[1] &= ~0x80;
                    switch (header[1]) {
                    case 126:
                        urihandler_websocket->len_size = 2;
                        urihandler_websocket->state = WEBSOCKET_STATE_PARSE_REALLEN;
                        goto parse_reallen;
                    case 127:
                        urihandler_websocket->len_size = 8;
                        urihandler_websocket->state = WEBSOCKET_STATE_PARSE_REALLEN;
                        goto parse_reallen;
                    default:
                        urihandler_websocket->len_size = 0;
                        urihandler_websocket->payload_len = header[1];
                        goto parsed_reallen;
                    }
                    break;
                case 1:
                    urihandler_websocket->is_string = true;
                    urihandler_websocket->is_start = true;
                    goto parse_len;
                case 2:
                    urihandler_websocket->is_string = false;
                    urihandler_websocket->is_start = true;
                    goto parse_len;
                case 8:
                case 9:
                case 10:
                    goto parse_len;
                }
            }
            break;
        case WEBSOCKET_STATE_PARSE_REALLEN:
        parse_reallen:
            if (size >= urihandler_websocket->len_size) {
                size -= vsf_stream_read(stream, header, urihandler_websocket->len_size);
                switch (urihandler_websocket->len_size) {
                case 2: urihandler_websocket->payload_len = get_unaligned_be16(header); break;
                case 8: urihandler_websocket->payload_len = get_unaligned_be64(header); break;
                }
            parsed_reallen:
                if (urihandler_websocket->is_masking) {
                    urihandler_websocket->state = WEBSOCKET_STATE_PARSE_MASKING;
                    goto parse_masking;
                } else if (urihandler_websocket->payload_len > 0) {
                    urihandler_websocket->state = WEBSOCKET_STATE_PARSE_DATA;
                    goto parse_data;
                } else {
                    urihandler_websocket->state = WEBSOCKET_STATE_PARSE_HEADER;
                    goto parse_header;
                }
            }
            break;
        case WEBSOCKET_STATE_PARSE_MASKING:
        parse_masking:
            if (size < 4) {
                break;
            }
            size -= vsf_stream_read(stream, urihandler_websocket->masking_key, 4);
            urihandler_websocket->masking_pos = 0;
            urihandler_websocket->state = WEBSOCKET_STATE_PARSE_DATA;
            if (0 == urihandler_websocket->payload_len) {
                urihandler_websocket->state = WEBSOCKET_STATE_PARSE_HEADER;
                goto parse_header;
            }
            // fallthrough
        case WEBSOCKET_STATE_PARSE_DATA:
        parse_data:
            while (size > 0) {
                uint8_t *buf;
                uint_fast32_t cur_size = vsf_stream_get_rbuf(stream, &buf);
                cur_size = vsf_min(cur_size, size);

                uint_fast32_t realsize = vsf_min(cur_size, urihandler_websocket->payload_len);
                urihandler_websocket->payload_len -= realsize;
                size -= realsize;

                if (urihandler_websocket->is_masking) {
                    uint8_t masking_pos = urihandler_websocket->masking_pos;
                    for (uint_fast32_t i = 0; i < realsize; i++) {
                        buf[i] ^= urihandler_websocket->masking_key[masking_pos];
                        masking_pos++;
                        masking_pos &= 3;
                    }
                    urihandler_websocket->masking_pos = masking_pos;
                }

                switch (urihandler_websocket->opcode) {
                case 0:
                case 1:
                case 2:
                    if (urihandler->websocket.on_message != NULL) {
                        urihandler->websocket.on_message(req,
                            (const vsf_linux_httpd_urihandler_websocket_t *)urihandler_websocket,
                            buf, realsize);
                    }
                    break;
                case 8:
                    header[0] = 0x88;
                    header[1] = 0x02;
                    vsf_stream_write(req->stream_out, header, 2);
                    vsf_stream_write(req->stream_out, buf, realsize);
                    urihandler_websocket->state = WEBSOCKET_STATE_CLOSING;
                    break;
                }
                vsf_stream_read(stream, NULL, realsize);

                if (0 == urihandler_websocket->payload_len) {
                    urihandler_websocket->state = WEBSOCKET_STATE_PARSE_HEADER;
                    if (size > 0) {
                        goto parse_header;
                    }
                }
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
        case WEBSOCKET_STATE_CLOSING:
            if (!vsf_stream_get_data_size(stream)) {
                vsf_stream_disconnect_rx(req->stream_in);
                vsf_stream_disconnect_tx(stream);
                urihandler_websocket->state = WEBSOCKET_STATE_CLOSED;
                if (urihandler->websocket.on_close != NULL) {
                    urihandler->websocket.on_close(req);
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
    memset(stream, 0, sizeof(*stream));
    stream->op = &vsf_fifo_stream_op;
    stream->buffer = req->buffer;
    stream->size = bufsize;
    VSF_STREAM_INIT(stream);
    stream->tx.param = req;
    VSF_STREAM_CONNECT_TX(stream);
    req->is_stream_out_started = true;
    req->stream_out = &stream->use_as__vsf_stream_t;

    stream = &urihandler_websocket->stream_in;
    memset(stream, 0, sizeof(*stream));
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

int vsf_linux_httpd_websocket_write(vsf_linux_httpd_request_t *req, uint8_t *buf, int len, bool is_string)
{
    vsf_stream_t *stream = req->stream_out;
    if (NULL == stream) {
        return -1;
    }

    uint8_t len_size = len < 126 ? 1 : len < 65535 ? 2 : 8;
    int size_dst = vsf_stream_get_free_size(stream);
    if (size_dst < 2) {
        return 0;
    }

    uint8_t header[8];
    header[0] = is_string ? 0x81 : 0x82;
__again:
    switch (len_size) {
    case 1:
        len = vsf_min(len, size_dst - 2);
        header[1] = len;
        break;
    case 2:
        if (size_dst < 4) {
            len = 125;
            goto __again;
        }
        len = vsf_min(len, size_dst - 4);
        header[1] = 126;
        put_unaligned_be16(len, &header[2]);
        len_size++;
        break;
    case 8:
        if (size_dst < 10) {
            len = 65535;
            goto __again;
        }
        len = vsf_min(len, size_dst - 10);
        header[1] = 127;
        put_unaligned_be64(len, &header[2]);
        len_size++;
        break;
    }
    vsf_stream_write(stream, header, 1 + len_size);
    vsf_stream_write(stream, buf, len);
    return len;
}
