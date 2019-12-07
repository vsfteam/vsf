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

#if VSF_USE_MAL == ENABLED && VSF_USE_MEM_MAL == ENABLED

#define VSF_MAL_INHERIT
#define VSF_MEM_MAL_IMPLEMENT

// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vk_mem_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
static bool __vk_mem_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
static void __vk_mem_mal_init(uintptr_t target, vsf_evt_t evt);
static void __vk_mem_mal_fini(uintptr_t target, vsf_evt_t evt);
static void __vk_mem_mal_read(uintptr_t target, vsf_evt_t evt);
static void __vk_mem_mal_write(uintptr_t target, vsf_evt_t evt);

/*============================ GLOBAL VARIABLES ==============================*/

const i_mal_drv_t VK_MEM_MAL_DRV = {
    .blksz          = __vk_mem_mal_blksz,
    .buffer         = __vk_mem_mal_buffer,
    .init           = __vk_mem_mal_init,
    .fini           = __vk_mem_mal_fini,
    .read           = __vk_mem_mal_read,
    .write          = __vk_mem_mal_write,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vk_mem_mal_blksz(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)mal;
    return pthis->blksz;
}

static bool __vk_mem_mal_buffer(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)mal;
    mem->pchBuffer = &pthis->mem.pchBuffer[addr];
    mem->nSize = size;
    return true;
}

static void __vk_mem_mal_init(uintptr_t target, vsf_evt_t evt)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)target;
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->result.errcode = VSF_ERR_NONE;
    pthis->result.size = 0;
    vsf_eda_return();
}

static void __vk_mem_mal_fini(uintptr_t target, vsf_evt_t evt)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)target;
    VSF_MAL_ASSERT(pthis != NULL);
    pthis->result.errcode = VSF_ERR_NONE;
    pthis->result.size = 0;
    vsf_eda_return();
}

static void __vk_mem_mal_read(uintptr_t target, vsf_evt_t evt)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)target;
    uint_fast64_t addr;
    uint_fast32_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    addr = pthis->args.addr;
    size = pthis->args.size;
    VSF_MAL_ASSERT((size > 0) && ((addr + size) <= pthis->mem.nSize));

    if (pthis->args.buff != &pthis->mem.pchBuffer[addr]) {
        memcpy(pthis->args.buff, &pthis->mem.pchBuffer[addr], size);
    }
    pthis->result.errcode = VSF_ERR_NONE;
    pthis->result.size = size;
    vsf_eda_return();
}

static void __vk_mem_mal_write(uintptr_t target, vsf_evt_t evt)
{
    vk_mem_mal_t *pthis = (vk_mem_mal_t *)target;
    uint_fast64_t addr;
    uint_fast32_t size;

    VSF_MAL_ASSERT(pthis != NULL);
    addr = pthis->args.addr;
    size = pthis->args.size;
    VSF_MAL_ASSERT((size > 0) && ((addr + size) <= pthis->mem.nSize));

    if (pthis->args.buff != &pthis->mem.pchBuffer[addr]) {
        memcpy(&pthis->mem.pchBuffer[addr], pthis->args.buff, size);
    }
    pthis->result.errcode = VSF_ERR_NONE;
    pthis->result.size = size;
    vsf_eda_return();
}

#endif
