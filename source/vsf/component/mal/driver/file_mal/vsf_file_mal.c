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

#include "../../vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_USE_FS == ENABLED && VSF_USE_FILE_MAL == ENABLED

#define VSF_MAL_INHERIT
#define VSF_FILE_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_FILE_MAL_CFG_DEBUG == ENABLED
#   define __vk_file_mal_trace(...)                                             \
            vsf_trace(VSF_TRACE_DEBUG, "file_mal: " __VA_ARGS__)
#else
#   define __vk_file_mal_trace(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_file_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static void __vk_file_mal_init(uintptr_t target, vsf_evt_t evt);
static void __vk_file_mal_fini(uintptr_t target, vsf_evt_t evt);
static void __vk_file_mal_read(uintptr_t target, vsf_evt_t evt);
static void __vk_file_mal_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const i_mal_drv_t VK_FILE_MAL_DRV = {
    .blksz          = __vk_file_mal_blksz,
    .init           = __vk_file_mal_init,
    .fini           = __vk_file_mal_fini,
    .read           = __vk_file_mal_read,
    .write          = __vk_file_mal_write,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_file_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    return( (vk_file_mal_t *)mal)->block_size;
}

static void __vk_file_mal_init(uintptr_t target, vsf_evt_t evt)
{
    vk_file_mal_t *pthis = (vk_file_mal_t *)target;
    VSF_MAL_ASSERT((pthis != NULL) && (pthis->file != NULL) && (pthis->block_size > 0));
    vsf_eda_return();
}

static void __vk_file_mal_fini(uintptr_t target, vsf_evt_t evt)
{
    vsf_eda_return();
}

static void __vk_file_mal_read(uintptr_t target, vsf_evt_t evt)
{
    vk_file_mal_t *pthis = (vk_file_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_file_read(pthis->file, pthis->args.addr, pthis->args.size, pthis->args.buff, &pthis->rw_size);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = vk_file_get_errcode(pthis->file);
        vsf_eda_return();
        break;
    }
}

static void __vk_file_mal_write(uintptr_t target, vsf_evt_t evt)
{
    vk_file_mal_t *pthis = (vk_file_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_file_write(pthis->file, pthis->args.addr, pthis->args.size, pthis->args.buff, &pthis->rw_size);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = vk_file_get_errcode(pthis->file);
        vsf_eda_return();
        break;
    }
}

#endif
