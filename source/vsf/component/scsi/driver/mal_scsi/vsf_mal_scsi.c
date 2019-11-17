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

#if VSF_USE_SCSI == ENABLED && VSF_USE_MAL_SCSI == ENABLED

#define VSF_SCSI_INHERIT
#define VSF_VIRTUAL_SCSI_INHERIT
#define VSF_MAL_SCSI_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static bool vsf_mal_scsi_buffer(vsf_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem);
static void vsf_mal_scsi_init(uintptr_t target, vsf_evt_t evt);
static void vsf_mal_scsi_read(uintptr_t target, vsf_evt_t evt);
static void vsf_mal_scsi_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_virtual_scsi_drv_t vsf_mal_virtual_scsi_drv = {
    .drv_type               = VSF_VIRTUAL_SCSI_DRV_PARAM_SUBCALL,
    .param_subcall          = {
        .buffer             = vsf_mal_scsi_buffer,
        .init               = vsf_mal_scsi_init,
        .read               = vsf_mal_scsi_read,
        .write              = vsf_mal_scsi_write,
    },
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static bool vsf_mal_scsi_buffer(vsf_scsi_t *scsi, bool is_read, uint_fast64_t addr, uint_fast32_t size, vsf_mem_t *mem)
{
    vsf_mal_scsi_t *mal_scsi = (vsf_mal_scsi_t *)scsi;
    vsf_virtual_scsi_param_t *param = mal_scsi->param;
    return vsf_mal_prepare_buffer(mal_scsi->mal,
                addr * param->block_size, size * param->block_size,
                is_read ? VSF_MAL_OP_READ : VSF_MAL_OP_WRITE, mem);
}

static void vsf_mal_scsi_return(vsf_mal_scsi_t *mal_scsi)
{
    mal_scsi->result.errcode = vsf_mal_get_result(mal_scsi->mal, &mal_scsi->result.reply_len);
    vsf_eda_return();
}

static void vsf_mal_scsi_init(uintptr_t target, vsf_evt_t evt)
{
    vsf_mal_scsi_t *mal_scsi = (vsf_mal_scsi_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_mal_init(mal_scsi->mal);
        break;
    case VSF_EVT_RETURN:
        vsf_mal_scsi_return(mal_scsi);
        break;
    }
}

static void vsf_mal_scsi_read(uintptr_t target, vsf_evt_t evt)
{
    vsf_mal_scsi_t *mal_scsi = (vsf_mal_scsi_t *)target;
    vsf_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        if (mal_scsi->is_stream) {
            vsf_mal_read_stream(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.stream);
        } else {
#endif
            vsf_mal_read(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.mem.pchBuffer);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        vsf_mal_scsi_return(mal_scsi);
        break;
    }
}

static void vsf_mal_scsi_write(uintptr_t target, vsf_evt_t evt)
{
    vsf_mal_scsi_t *mal_scsi = (vsf_mal_scsi_t *)target;
    vsf_virtual_scsi_param_t *param = mal_scsi->param;

    switch (evt) {
    case VSF_EVT_INIT:
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        if (mal_scsi->is_stream) {
            vsf_mal_write_stream(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.stream);
        } else {
#endif
            vsf_mal_write(mal_scsi->mal,
                mal_scsi->addr * param->block_size, mal_scsi->size * param->block_size,
                mal_scsi->args.mem.pchBuffer);
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        }
#endif
        break;
    case VSF_EVT_RETURN:
        vsf_mal_scsi_return(mal_scsi);
        break;
    }
}

#endif
