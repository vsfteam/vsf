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

#include "../../vsf_scsi_cfg.h"

#if VSF_USE_SCSI == ENABLED && VSF_USE_MAL == ENABLED && VSF_SCSI_USE_MAL_SCSI == ENABLED

#define __VSF_SCSI_CLASS_INHERIT__
#define __VSF_VIRTUAL_SCSI_CLASS_INHERIT__
#define __VSF_MAL_SCSI_CLASS_IMPLEMENT

#include "../../vsf_scsi.h"
#include "./vsf_mal_scsi.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool __vk_mal_scsi_buffer(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
dcl_vsf_peda_methods(static, __vk_mal_scsi_init)
dcl_vsf_peda_methods(static, __vk_mal_scsi_read)
dcl_vsf_peda_methods(static, __vk_mal_scsi_write)

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

const vk_virtual_scsi_drv_t vk_mal_virtual_scsi_drv = {
    .drv_type               = VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL,
    .param_subcall          = {
        .buffer             = __vk_mal_scsi_buffer,
        .init               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mal_scsi_init),
        .read               = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mal_scsi_read),
        .write              = (vsf_peda_evthandler_t)vsf_peda_func(__vk_mal_scsi_write),
    },
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static bool __vk_mal_scsi_buffer(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem)
{
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)scsi;
    vk_virtual_scsi_param_t *param = mal_scsi->param;
    return vk_mal_prepare_buffer(mal_scsi->mal,
                addr * param->block_size, size * param->block_size,
                is_read ? VSF_MAL_OP_READ : VSF_MAL_OP_WRITE, mem);
}

__vsf_component_peda_ifs_entry(__vk_mal_scsi_init, vk_virtual_scsi_init)
{
    vsf_peda_begin();
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)&vsf_this;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SIMPLE_STREAM == ENABLED
        mal_scsi->mal_stream.mal = mal_scsi->mal;
#endif
        vk_mal_init(mal_scsi->mal);
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_mal_scsi_read, vk_virtual_scsi_read)
{
    vsf_peda_begin();
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)&vsf_this;
    vk_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SIMPLE_STREAM == ENABLED
        if (mal_scsi->is_stream) {
            vk_mal_read_stream(&mal_scsi->mal_stream,
                vsf_local.addr * param->block_size, vsf_local.size * param->block_size,
                (vsf_stream_t *)vsf_local.mem_stream);
        } else {
#endif
            vk_mal_read(mal_scsi->mal,
                vsf_local.addr * param->block_size, vsf_local.size * param->block_size,
                ((vsf_mem_t *)vsf_local.mem_stream)->buffer);
#if VSF_USE_SIMPLE_STREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_mal_scsi_write, vk_virtual_scsi_write)
{
    vsf_peda_begin();
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)&vsf_this;
    vk_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SIMPLE_STREAM == ENABLED
        if (mal_scsi->is_stream) {
            vk_mal_write_stream(&mal_scsi->mal_stream,
                vsf_local.addr * param->block_size, vsf_local.size * param->block_size,
                (vsf_stream_t *)vsf_local.mem_stream);
        } else {
#endif
            vk_mal_write(mal_scsi->mal,
                vsf_local.addr * param->block_size, vsf_local.size * param->block_size,
                ((vsf_mem_t *)vsf_local.mem_stream)->buffer);
#if VSF_USE_SIMPLE_STREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif
