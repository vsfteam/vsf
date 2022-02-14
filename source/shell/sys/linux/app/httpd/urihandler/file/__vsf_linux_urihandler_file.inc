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
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_linux_httpd_urihandler_file_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size);
static vsf_err_t __vsf_linux_httpd_urihandler_file_fini(vsf_linux_httpd_request_t *req);
static vsf_err_t __vsf_linux_httpd_urihandler_file_serve(vsf_linux_httpd_request_t *req);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_httpd_urihandler_op_t __vsf_linux_httpd_urihandler_file_op = {
	.init_fn        = __vsf_linux_httpd_urihandler_file_init,
    .fini_fn        = __vsf_linux_httpd_urihandler_file_fini,
    .serve_fn       = __vsf_linux_httpd_urihandler_file_serve,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_linux_httpd_urihandler_file_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_linux_httpd_urihandler_file_t *urihandler_file = container_of(eda, vsf_linux_httpd_urihandler_file_t, eda);
    vsf_linux_httpd_request_t *request = container_of(urihandler_file, vsf_linux_httpd_request_t, urihandler_ctx);
    vk_file_stream_t *file_stream = &urihandler_file->file_stream;
    vsf_stream_t *stream = &urihandler_file->stream.use_as__vsf_stream_t;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_file_read_stream(file_stream, stream, request->content_length);
        request->is_stream_out_started = true;
        break;
    case VSF_EVT_RETURN:
        break;
    }
}

static vsf_err_t __vsf_linux_httpd_urihandler_file_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size)
{
    VSF_LINUX_ASSERT((req != NULL) && (req->uri != NULL));

    vsf_linux_httpd_session_t *session = container_of(req, vsf_linux_httpd_session_t, request);
    vsf_linux_httpd_urihandler_file_t *urihandler_file = &req->urihandler_ctx.file;
    char *uri = req->uri, *root_path = session->httpd->root_path;

    if (NULL == root_path) {
    __not_found:
        req->response = VSF_LINUX_HTTPD_NOT_FOUND;
        return VSF_ERR_FAIL;
    }

    // get root file
    vk_file_t *root_dir = NULL, *uri_file;
    if (root_path != NULL) {
        vk_file_open(NULL, root_path, &root_dir);
        if (vsf_eda_get_return_value() != VSF_ERR_NONE) {
            goto __not_found;
        }
    }

    if (uri[0] == '/') {
        uri++;
    }
    vk_file_open(root_dir, uri, &uri_file);
    if (vsf_eda_get_return_value() != VSF_ERR_NONE) {
        goto __not_found;
    }
    vk_file_close(root_dir);

    // if uri is a directory, get index.html/index.htm
    if (uri_file->attr & VSF_FILE_ATTR_DIRECTORY) {
        vk_file_t *html_file;
        vk_file_open(uri_file, "index.html", &html_file);
        if (vsf_eda_get_return_value() != VSF_ERR_NONE) {
            vk_file_open(uri_file, "index.htm", &html_file);
            if (vsf_eda_get_return_value() != VSF_ERR_NONE) {
                vk_file_close(uri_file);
                goto __not_found;
            }
        }
        vk_file_close(uri_file);
        uri_file = html_file;
        req->mime = VSF_LINUX_HTTPD_MIME_TEXT_HTML;
    }
    urihandler_file->file_stream.file = uri_file;
    req->content_length = uri_file->size;

    vsf_fifo_stream_t *stream = &urihandler_file->stream;
    stream->op = &vsf_fifo_stream_op;
    stream->buffer = req->buffer;
    stream->size = sizeof(req->buffer);
    VSF_STREAM_INIT(stream);
    req->stream_out = &stream->use_as__vsf_stream_t;

    req->response = VSF_LINUX_HTTPD_OK;

    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_file_fini(vsf_linux_httpd_request_t *req)
{
    vsf_linux_httpd_urihandler_file_t *urihandler_file = &req->urihandler_ctx.file;
    VSF_LINUX_ASSERT(urihandler_file->file_stream.file != NULL);
    vk_file_close(urihandler_file->file_stream.file);
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_file_serve(vsf_linux_httpd_request_t *req)
{
    vsf_linux_httpd_urihandler_file_t *urihandler_file = &req->urihandler_ctx.file;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    urihandler_file->eda.on_terminate = NULL;
#endif
    urihandler_file->eda.fn.evthandler = __vsf_linux_httpd_urihandler_file_evthandler;
    return vsf_eda_init(&urihandler_file->eda);
}
