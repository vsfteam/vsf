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
#include "hal/vsf_hal_cfg.h"
#include "./device.h"
// for vsf_heap
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/

#if VSF_ARCH_MMU == ENABLED
#   ifndef VSF_DRIVER_MMU
#       define VSF_DRIVER_MMU                   ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if defined(VSF_HAL_CFG_COHERENT_START) && defined(VSF_HAL_CFG_COHERENT_SIZE) && (VSF_USE_HEAP == ENABLED)
struct __vsf_coherent_heap_t {
    // one more as terminator
    vsf_dlist_t freelist[VSF_HEAP_CFG_FREELIST_NUM + 1];
};
typedef struct __vsf_coherent_heap_t __vsf_coherent_heap_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_DRIVER_MMU == ENABLED && VSF_ARCH_MMU == ENABLED
static uint32_t __f1c100s_mmu_ttb[4096] ALIGN(0x4000);
#endif

#if defined(VSF_HAL_CFG_COHERENT_START) && defined(VSF_HAL_CFG_COHERENT_SIZE) && (VSF_USE_HEAP == ENABLED)
static NO_INIT __vsf_coherent_heap_t __vsf_coherent_heap;
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_HEAP == ENABLED
#   if defined(VSF_HAL_CFG_COHERENT_START) && defined(VSF_HAL_CFG_COHERENT_SIZE)
void * vsf_coherent_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment)
{
    return __vsf_heap_malloc_alligned_ex(size, alignment, &__vsf_mmu_heap.freelist[0]);
}

void * vsf_coherent_heap_malloc(uint_fast32_t size)
{
    return __vsf_heap_malloc_ex(size, &__vsf_mmu_heap.freelist[0]);
}

void vsf_coherent_heap_free(void *buffer)
{
    return __vsf_heap_free_ex(buffer, &__vsf_mmu_heap.freelist[0]);
}

void * vsf_dma_alloc_coherent(uint_fast32_t size)
{
    return vsf_coherent_heap_malloc(size);
}

void vsf_dma_free_coherent(void *buffer)
{
    return vsf_coherent_heap_free(buffer);
}
#   endif

void * vsf_dma_alloc_noncoherent(uint_fast32_t size)
{
    return vsf_heap_malloc(size);
}

void vsf_dma_free_noncoherent(void *buffer)
{
    vsf_heap_free(buffer);
}
#endif

/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_driver_init(void)
{
#if 0
    // clock and dram are initialized in spl
    f1cx00s_clock_init(F1CX00S_PLL_CPU_CLK_HZ);

    f1cx00s_dram_param_t param = {
        .base           = 0x80000000,
        .size           = 32,
        .clk            = F1CX00S_PLL_DDR_CLK_HZ / 1000000,
        .access_mode    = 1,
        .cs_num         = 1,
        .ddr8_remap     = 0,
        .sdr_ddr        = DRAM_TYPE_DDR,
        .bwidth         = 16,
        .col_width      = 10,
        .row_width      = 13,
        .bank_size      = 4,
        .cas            = 0x3,
    };

    if (f1cx00s_dram_init(&param)) {
        return false;
    }
#endif

#if VSF_DRIVER_MMU == ENABLED && VSF_ARCH_MMU == ENABLED
    // map 0x00000000 to 0x00000000 with 2G size
    vsf_arch_mmu_map(__f1c100s_mmu_ttb, 0x00000000, 0x00000000, 0x80000000, MMU_MAP_TYPE_NCNB);
    // map 0x80000000 to 0x80000000 with 2G size
    vsf_arch_mmu_map(__f1c100s_mmu_ttb, 0x80000000, 0x80000000, 0x80000000, MMU_MAP_TYPE_NCNB);
    // map 0x80000000 to 0x80000000 with 32M size
    vsf_arch_mmu_map(__f1c100s_mmu_ttb, 0x80000000, 0x80000000, 0x02000000, MMU_MAP_TYPE_CB);
#   if defined(VSF_HAL_CFG_COHERENT_START) && defined(VSF_HAL_CFG_COHERENT_SIZE)
    vsf_arch_mmu_map(__f1c100s_mmu_ttb, VSF_HAL_CFG_COHERENT_START, VSF_HAL_CFG_COHERENT_START, VSF_HAL_CFG_COHERENT_SIZE, MAP_TYPE_NCNB);
#       if VSF_USE_HEAP == ENABLED
    memset(&__vsf_mmu_heap, 0, sizeof(__vsf_mmu_heap));
    __vsf_heap_add_ex((uint8_t *)VSF_HAL_CFG_COHERENT_START, VSF_HAL_CFG_COHERENT_SIZE, &__vsf_mmu_heap.freelist[0]);
#       endif
#   endif

    vsf_arch_mmu_enable(__f1c100s_mmu_ttb);
#endif
    return true;
}


/* EOF */
