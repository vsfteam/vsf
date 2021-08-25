#include <unistd.h>
#include <stdio.h>

#define __VSF_LINUX_HTTPD_CLASS_INHERIT__
#include "shell/sys/linux/app/httpd/vsf_linux_httpd.h"

#if VSF_USE_LINUX == ENABLED && APP_USE_LINUX_HTTPD_DEMO == ENABLED
#ifndef APP_LINUX_HTTPD_DEMO_CFG_ROOT
static const char *__user_httpd_index = STR(
  <html>
    <head>
      <title>
        Hello World
      </title>
    </head>
    <body>
      Hello World
    </body>
  </html>
);

typedef struct __user_httpd_urihandler_t {
    vsf_mem_stream_t stream;
} __user_httpd_urihandler_t;

static vsf_err_t __user_httpd_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size)
{
    __user_httpd_urihandler_t *urihandler = (__user_httpd_urihandler_t *)&req->urihandler_ctx.priv;

    vsf_mem_stream_t *stream = &urihandler->stream;
    stream->op = &vsf_mem_stream_op;
    stream->buffer = req->buffer;
    stream->size = sizeof(req->buffer);
    stream->align = 0;
    VSF_STREAM_INIT(stream);
    req->stream_out = &stream->use_as__vsf_stream_t;

    req->content_length = strlen(__user_httpd_index);
    req->content_type = VSF_LINUX_HTTPD_CONTENT_TEXT_XML,
    req->response = VSF_LINUX_HTTPD_OK;
    return VSF_ERR_NONE;
}

static vsf_err_t __user_httpd_fini(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __user_httpd_serve(vsf_linux_httpd_request_t *req)
{
    __user_httpd_urihandler_t *urihandler = (__user_httpd_urihandler_t *)&req->urihandler_ctx.priv;
    VSF_STREAM_WRITE(&urihandler->stream, (uint8_t *)__user_httpd_index, strlen(__user_httpd_index));
    VSF_STREAM_DISCONNECT_TX(&urihandler->stream);
    return VSF_ERR_NONE;
}

static const vsf_linux_httpd_urihandler_op_t __user_httpd_urihandler_op = {
    .init_fn            = __user_httpd_init,
    .fini_fn            = __user_httpd_fini,
    .serve_fn           = __user_httpd_serve,
};

static vsf_linux_httpd_urihandler_t __user_httpd_urihandler[] = {
    {
        .match          = VSF_LINUX_HTTPD_URI_MATCH_URI,
        .uri            = "/",
        .type           = VSF_LINUX_HTTPD_URI_OP,
        .op             = &__user_httpd_urihandler_op,
    },
};
#endif

static vsf_linux_httpd_t __user_httpd = {
#ifdef APP_LINUX_HTTPD_DEMO_CFG_ROOT
    .root_path          = APP_LINUX_HTTPD_DEMO_CFG_ROOT,
#else
    .num_of_urihandler  = dimof(__user_httpd_urihandler),
    .urihandler         = __user_httpd_urihandler,
#endif
    .port               = 80,
    .backlog            = 1,
};

int httpd_main(int argc, char *argv[])
{
    vsf_linux_httpd_start(&__user_httpd);
    return 0;
}
#endif
