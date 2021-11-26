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

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED

#include "service/vsf_service.h"
#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#include "mbedtls/config.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void * __vsf_mbedtls_calloc(size_t n, size_t size)
{
    uint_fast32_t allsize = n * size;
    void *buffer = vsf_heap_malloc(allsize);
    if (buffer != NULL) {
        memset(buffer, 0, allsize);
    }
    return buffer;
}

void __vsf_mbedtls_free(void *ptr)
{
    vsf_heap_free(ptr);
}

void __vsf_mbedtls_exit(int status)
{
    vsf_thread_exit();
}

#if RNG_COUNT > 0
WEAK(mbedtls_hardware_poll)
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    static bool __is_inited = false;
    size_t cur_len = 0;

    VSF_ASSERT(!(len % (RNG_BITLEN >> 3)));

    if (!__is_inited) {
        __is_inited = true;
        vsf_hw_rng_init(&vsf_rng0);
    }

    while (cur_len < len) {
        if (VSF_ERR_NONE != vsf_hw_rng_generate(&vsf_rng0, (uint32_t *)output)) {
            break;
        }

        cur_len += RNG_BITLEN >> 3;
        output += RNG_BITLEN >> 3;
    }
    *olen = cur_len;
    return 0;
}
#endif

#endif      // VSF_USE_MBEDTLS
