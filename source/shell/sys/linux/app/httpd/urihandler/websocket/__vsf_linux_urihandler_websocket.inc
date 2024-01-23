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
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size);
static vsf_err_t __vsf_linux_httpd_urihandler_websocket_fini(vsf_linux_httpd_request_t *req);
static vsf_err_t __vsf_linux_httpd_urihandler_websocket_serve(vsf_linux_httpd_request_t *req);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_linux_httpd_urihandler_op_t vsf_linux_httpd_urihandler_websocket_op = {
    .init_fn        = __vsf_linux_httpd_urihandler_websocket_init,
    .fini_fn        = __vsf_linux_httpd_urihandler_websocket_fini,
    .serve_fn       = __vsf_linux_httpd_urihandler_websocket_serve,
};

/*============================ IMPLEMENTATION ================================*/

static void __vsf_linux_httpd_urihandler_websocket_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_linux_httpd_urihandler_websocket_t *urihandler_websocket = container_of(eda, vsf_linux_httpd_urihandler_websocket_t, eda);
    vsf_linux_httpd_request_t *request = container_of(urihandler_websocket, vsf_linux_httpd_request_t, urihandler_ctx);
    vsf_stream_t *stream = &urihandler_websocket->stream.use_as__vsf_stream_t;

    switch (evt) {
    case VSF_EVT_INIT:
        request->is_stream_out_started = true;
        break;
    case VSF_EVT_RETURN:
        break;
    }
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_init(vsf_linux_httpd_request_t *req, uint8_t *data, uint_fast32_t size)
{
    VSF_LINUX_ASSERT((req != NULL) && (req->uri != NULL));

    vsf_linux_httpd_session_t *session = container_of(req, vsf_linux_httpd_session_t, request);
    vsf_linux_httpd_urihandler_websocket_t *urihandler_websocket = &req->urihandler_ctx.websocket;
    char *uri = req->uri;

    req->response = VSF_LINUX_HTTPD_OK;

    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_fini(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_websocket_serve(vsf_linux_httpd_request_t *req)
{
    vsf_linux_httpd_urihandler_websocket_t *urihandler_websocket = &req->urihandler_ctx.websocket;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    urihandler_websocket->eda.on_terminate = NULL;
#endif
    urihandler_websocket->eda.fn.evthandler = __vsf_linux_httpd_urihandler_websocket_evthandler;
    return vsf_eda_init(&urihandler_websocket->eda);
}
