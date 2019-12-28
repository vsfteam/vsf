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

#if VSF_USE_MAL == ENABLED && VSF_USE_MIM_MAL == ENABLED

#define VSF_MAL_INHERIT
#define VSF_MIM_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_mim_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_mim_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
static void __vk_mim_mal_init(uintptr_t target, vsf_evt_t evt);
static void __vk_mim_mal_fini(uintptr_t target, vsf_evt_t evt);
static void __vk_mim_mal_read(uintptr_t target, vsf_evt_t evt);
static void __vk_mim_mal_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const i_mal_drv_t VK_MIM_MAL_DRV = {
    .blksz          = __vk_mim_mal_blksz,
    .buffer         = __vk_mim_mal_buffer,
    .init           = __vk_mim_mal_init,
    .fini           = __vk_mim_mal_fini,
    .read           = __vk_mim_mal_read,
    .write          = __vk_mim_mal_write,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_mim_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_mim_mal_t *pthis = (vk_mim_mal_t *)mal;
    return vk_mal_blksz(pthis->host_mal, addr, size, op);
}

static bool __vk_mim_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_mim_mal_t *pthis = (vk_mim_mal_t *)mal;
    return vk_mal_prepare_buffer(pthis->host_mal, addr + pthis->offset, size, op, mem);
}

static void __vk_mim_mal_init(uintptr_t target, vsf_evt_t evt)
{
    vsf_eda_return();
}

static void __vk_mim_mal_fini(uintptr_t target, vsf_evt_t evt)
{
    vsf_eda_return();
}

static void __vk_mim_mal_read(uintptr_t target, vsf_evt_t evt)
{
    vk_mim_mal_t *pthis = (vk_mim_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_read(pthis->host_mal, pthis->args.addr + pthis->offset, pthis->args.size, pthis->args.buff);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = pthis->host_mal->result.errcode;
        vsf_eda_return();
        break;
    }
}

static void __vk_mim_mal_write(uintptr_t target, vsf_evt_t evt)
{
    vk_mim_mal_t *pthis = (vk_mim_mal_t *)target;

    switch (evt) {
    case VSF_EVT_INIT:
        vk_mal_write(pthis->host_mal, pthis->args.addr + pthis->offset, pthis->args.size, pthis->args.buff);
        break;
    case VSF_EVT_RETURN:
        pthis->result.errcode = pthis->host_mal->result.errcode;
        vsf_eda_return();
        break;
    }
}

#endif
