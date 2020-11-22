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

#define __VSF_FIFO_CLASS_IMPLEMENT

#include "service/vsf_service_cfg.h"

#if VSF_USE_FIFO == ENABLED
#include "vsf_fifo.h"
#include "hal/arch/vsf_arch.h"

#if defined(VSF_FIFO_CFG_ATOM_ACCESS_DEPENDENCY)
#   include VSF_FIFO_CFG_ATOM_ACCESS_DEPENDENCY
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_FIFO_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_FIFO_CFG_PROTECT_LEVEL  none
 *!
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_FIFO_CFG_PROTECT_LEVEL  scheduler
 *!
 *!         NOTE: This macro should be defined in vsf_usr_cfg.h
 */
#   define VSF_FIFO_CFG_PROTECT_LEVEL           interrupt
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_fifo_protect                  vsf_protect(VSF_FIFO_CFG_PROTECT_LEVEL)
#define __vsf_fifo_unprotect                vsf_unprotect(VSF_FIFO_CFG_PROTECT_LEVEL)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_fifo_init(vsf_fifo_t *fifo, vsf_fifo_fast_index_t fifo_depth)
{
    VSF_SERVICE_ASSERT(fifo != NULL);
    fifo->head = fifo->tail = fifo->number = 0;
    fifo->depth = fifo_depth;
}

bool vsf_fifo_push(vsf_fifo_t *fifo, uintptr_t item, vsf_fifo_fast_item_size_t item_size)
{
    bool ret = false;

    VSF_SERVICE_ASSERT((fifo != NULL) && (item_size > 0));
    VSF_SERVICE_ASSERT(fifo->depth > 0);

    vsf_protect_t orig = __vsf_fifo_protect();
    if (fifo->number < fifo->depth) {
        __vsf_fifo_unprotect(orig);

        ret = true;
        if (item != (uintptr_t)NULL) {
            uint8_t *buffer = (uint8_t *)fifo->nodes + item_size * fifo->head;
            memcpy(buffer, (const void *)item, item_size);
        }
        if (++fifo->head >= fifo->depth) {
            fifo->head = 0;
        }

        orig = __vsf_fifo_protect();
        fifo->number++;
    }
    __vsf_fifo_unprotect(orig);

    return ret;
}

bool vsf_fifo_pop(vsf_fifo_t *fifo, uintptr_t item, vsf_fifo_fast_item_size_t item_size)
{
    bool ret = false;

    VSF_SERVICE_ASSERT((fifo != NULL) && (item_size > 0));
    VSF_SERVICE_ASSERT(fifo->depth > 0);

    vsf_protect_t orig = __vsf_fifo_protect();
    if (fifo->number > 0) {
        __vsf_fifo_unprotect(orig);

        ret = true;
        if (item != (uintptr_t)NULL) {
            uint8_t *buffer = (uint8_t *)fifo->nodes + item_size * fifo->tail;
            memcpy((void *)item, buffer, item_size);
        }
        if (++fifo->tail >= fifo->depth) {
            fifo->tail = 0;
        }

        orig = __vsf_fifo_protect();
        fifo->number--;
    }
    __vsf_fifo_unprotect(orig);
    return ret;
}

SECTION(".text.vsf.utilities.vsf_fifo_get_head")
uintptr_t vsf_fifo_get_head(vsf_fifo_t *fifo, vsf_fifo_fast_item_size_t item_size)
{
    VSF_SERVICE_ASSERT((fifo != NULL) && (item_size > 0));
    VSF_SERVICE_ASSERT(fifo->depth > 0);

    vsf_protect_t orig = __vsf_fifo_protect();
    if (fifo->number < fifo->depth) {
        __vsf_fifo_unprotect(orig);
        return (uintptr_t)((uint8_t *)fifo->nodes + item_size * fifo->head);
    }
    __vsf_fifo_unprotect(orig);
    return (uintptr_t)NULL;
}

SECTION(".text.vsf.utilities.vsf_fifo_get_tail")
uintptr_t vsf_fifo_get_tail(vsf_fifo_t *fifo, vsf_fifo_fast_item_size_t item_size)
{
    VSF_SERVICE_ASSERT((fifo != NULL) && (item_size > 0));
    VSF_SERVICE_ASSERT(fifo->depth > 0);

    vsf_protect_t orig = __vsf_fifo_protect();
    if (fifo->number > 0) {
        __vsf_fifo_unprotect(orig);
        return (uintptr_t)((uint8_t *)fifo->nodes + item_size * fifo->tail);
    }
    __vsf_fifo_unprotect(orig);
    return (uintptr_t)NULL;
}

SECTION(".text.vsf.utilities.vsf_fifo_get_number")
vsf_fifo_index_t vsf_fifo_get_number(vsf_fifo_t *fifo)
{
    vsf_fifo_index_t number;

    vsf_protect_t orig = __vsf_fifo_protect();
        number = fifo->number;
    __vsf_fifo_unprotect(orig);
    return number;
}

#endif
