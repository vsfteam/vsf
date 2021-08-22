/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

static vsf_err_t __vsf_linux_httpd_urihandler_file_init(vsf_linux_httpd_request_t *req, vsf_stream_t *in, vsf_stream_t *out);
static vsf_err_t __vsf_linux_httpd_urihandler_file_fini(vsf_linux_httpd_request_t *req);
static vsf_err_t __vsf_linux_httpd_urihandler_file_serve(vsf_linux_httpd_request_t *req);

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_linux_httpd_urihandler_op_t __vsf_linux_httpd_urihandler_file_op = {
	.init_fn        = __vsf_linux_httpd_urihandler_file_init,
    .fini_fn        = __vsf_linux_httpd_urihandler_file_fini,
    .serve_fn       = __vsf_linux_httpd_urihandler_file_serve,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_linux_httpd_urihandler_file_init(vsf_linux_httpd_request_t *req, vsf_stream_t *in, vsf_stream_t *out)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_file_fini(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_linux_httpd_urihandler_file_serve(vsf_linux_httpd_request_t *req)
{
    return VSF_ERR_NONE;
}
