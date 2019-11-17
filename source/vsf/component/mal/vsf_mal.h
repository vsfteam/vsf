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

#ifndef __VSF_MAL_H__
#define __VSF_MAL_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_mal_cfg.h"

#if VSF_USE_MAL == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(VSF_MAL_IMPLEMENT)
#   undef VSF_MAL_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_MAL_INHERIT)
#   undef VSF_MAL_INHERIT
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use mal
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_mal_t)

enum vsf_mal_op_t {
    VSF_MAL_OP_ERASE,
    VSF_MAL_OP_READ,
    VSF_MAL_OP_WRITE,
};
typedef enum vsf_mal_op_t vsf_mal_op_t;

enum vsf_mal_feature_t {
    VSF_MAL_READABLE            = 1 << 0,
    VSF_MAL_WRITABLE            = 1 << 1,
    VSF_MAL_ERASABLE            = 1 << 2,
    VSF_MAL_NON_UNIFIED_BLOCK   = 1 << 3,
};
typedef enum vsf_mal_feature_t vsf_mal_feature_t;

struct vsf_mal_drv_t {
    // blksz can be called after successfully initialized
    uint_fast32_t (*blksz)(vsf_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
    bool (*buffer)(vsf_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
    void (*init)(uintptr_t target, vsf_evt_t evt);
    void (*fini)(uintptr_t target, vsf_evt_t evt);
    void (*erase)(uintptr_t target, vsf_evt_t evt);
    void (*read)(uintptr_t target, vsf_evt_t evt);
    void (*write)(uintptr_t target, vsf_evt_t evt);
};
typedef struct vsf_mal_drv_t vsf_mal_drv_t;

def_simple_class(vsf_mal_t) {
    public_member(
        const vsf_mal_drv_t *drv;
        void *param;
        uint64_t size;
        uint8_t feature;
    )

    protected_member(
        struct {
            uint64_t addr;
            uint32_t size;
            uint8_t *buff;
        } args;
        struct {
            uint32_t size;
            vsf_err_t errcode;
        } result;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        struct {
            uint64_t addr;
            uint32_t size;
            uint32_t rw_size;
            vsf_stream_t *stream;
            uint8_t *cur_buff;
            vsf_eda_t *cur_eda;
        } stream;
#endif
    )
};

/*============================ INCLUDES ======================================*/

#include "./driver/mem_mal/vsf_mem_mal.h"
#include "./driver/fakefat32_mal/vsf_fakefat32_mal.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_mal_init(vsf_mal_t *pthis);
extern vsf_err_t vsf_mal_fini(vsf_mal_t *pthis);
extern uint_fast32_t vsf_mal_blksz(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
// vsf_mal_prepare_buffer is used to get buffer for read/write if VSF_MAL_LOCAL_BUFFER is supported
extern bool vsf_mal_prepare_buffer(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
// to erase all, size should be 0
extern vsf_err_t vsf_mal_erase(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size);
extern vsf_err_t vsf_mal_read(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);
extern vsf_err_t vsf_mal_write(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);

#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
extern vsf_err_t vsf_mal_read_stream(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
extern vsf_err_t vsf_mal_write_stream(vsf_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
#endif

extern vsf_err_t vsf_mal_get_result(vsf_mal_t *pthis, uint32_t *size);

#endif      // VSF_USE_MAL
#endif      // __VSF_MAL_H__
