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

#if VSF_USE_SCSI == ENABLED && VSF_USE_MAL == ENABLED && VSF_USE_MAL_SCSI == ENABLED

#define VSF_SCSI_INHERIT
#define VSF_VIRTUAL_SCSI_INHERIT
#define VSF_MAL_SCSI_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool __vk_mal_scsi_buffer(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
static void __vk_mal_scsi_init(uintptr_t target, vsf_evt_t evt);
static void __vk_mal_scsi_read(uintptr_t target, vsf_evt_t evt);
static void __vk_mal_scsi_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const i_virtual_scsi_drv_t VK_MAL_VIRTUAL_SCSI_DRV = {
    .drv_type               = VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL,
    .param_subcall          = {
        .buffer             = __vk_mal_scsi_buffer,
        .init               = __vk_mal_scsi_init,
        .read               = __vk_mal_scsi_read,
        .write              = __vk_mal_scsi_write,
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static bool __vk_mal_scsi_buffer(vk_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem)
{
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)scsi;
    vk_virtual_scsi_param_t *param = mal_scsi->param;
    return vk_mal_prepare_buffer(mal_scsi->mal,
                addr * param->block_size, size * param->block_size,
                is_read ? VSF_MAL_OP_READ : VSF_MAL_OP_WRITE, mem);
}

static void __vk_mal_scsi_return(vk_mal_scsi_t *mal_scsi)
{
    mal_scsi->result.errcode = vk_mal_get_result(mal_scsi->mal, &mal_scsi->result.reply_len);
    vsf_eda_return();
}

static void __vk_mal_scsi_init(uintptr_t target, vsf_evt_t evt)
{
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        mal_scsi->mal_stream.mal = mal_scsi->mal;
#endif
        vk_mal_init(mal_scsi->mal);
        break;
    case VSF_EVT_RETURN:
        __vk_mal_scsi_return(mal_scsi);
        break;
    }
}

static void __vk_mal_scsi_read(uintptr_t target, vsf_evt_t evt)
{
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)target;
    vk_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        if (mal_scsi->is_stream) {
            vk_mal_read_stream(&mal_scsi->mal_stream,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.stream);
        } else {
#endif
            vk_mal_read(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.mem.pchBuffer);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        __vk_mal_scsi_return(mal_scsi);
        break;
    }
}

static void __vk_mal_scsi_write(uintptr_t target, vsf_evt_t evt)
{
    vk_mal_scsi_t *mal_scsi = (vk_mal_scsi_t *)target;
    vk_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        if (mal_scsi->is_stream) {
            vk_mal_write_stream(&mal_scsi->mal_stream,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.stream);
        } else {
#endif
            vk_mal_write(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.mem.pchBuffer);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        __vk_mal_scsi_return(mal_scsi);
        break;
    }
}

#endif
