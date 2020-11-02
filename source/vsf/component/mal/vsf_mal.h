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

#if     defined(__VSF_MAL_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_MAL_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is needed to use mal
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_mal_t)
#if VSF_USE_SIMPLE_STREAM == ENABLED
dcl_simple_class(vk_mal_stream_t)
#endif

dcl_simple_class(vk_mal_drv_t)

typedef enum vsf_mal_op_t {
    VSF_MAL_OP_ERASE,
    VSF_MAL_OP_READ,
    VSF_MAL_OP_WRITE,
} vsf_mal_op_t;

typedef enum vsf_mal_feature_t {
    VSF_MAL_READABLE            = 1 << 0,
    VSF_MAL_WRITABLE            = 1 << 1,
    VSF_MAL_ERASABLE            = 1 << 2,
    VSF_MAL_NON_UNIFIED_BLOCK   = 1 << 3,
} vsf_mal_feature_t;

def_simple_class(vk_mal_drv_t) {
    protected_member(
        // blksz can be called after successfully initialized
        uint_fast32_t (*blksz)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
        bool (*buffer)(vk_mal_t *mal, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
        vsf_peda_evthandler_t init;
        vsf_peda_evthandler_t fini;
        vsf_peda_evthandler_t erase;
        vsf_peda_evthandler_t read;
        vsf_peda_evthandler_t write;
    )
};

def_simple_class(vk_mal_t) {
    public_member(
        const vk_mal_drv_t *drv;
        void *param;
        uint64_t size;
        uint8_t feature;
    )
};

#if VSF_USE_SIMPLE_STREAM == ENABLED
def_simple_class(vk_mal_stream_t) {
    public_member(
        vk_mal_t *mal;
    )
    protected_member(
        uint64_t addr;
        uint32_t size;
        uint32_t rw_size;
        vsf_stream_t *stream;
        uint32_t cur_size;
        uint8_t *cur_buff;
        vsf_eda_t *cur_eda;
    )
};
#endif

#if defined(__VSF_MAL_CLASS_IMPLEMENT) || defined(__VSF_MAL_CLASS_INHERIT__)
__vsf_component_peda_ifs(vk_mal_init)
__vsf_component_peda_ifs(vk_mal_fini)
__vsf_component_peda_ifs(vk_mal_erase,
    uint64_t addr;
    uint32_t size;
)
__vsf_component_peda_ifs(vk_mal_read,
    uint64_t addr;
    uint32_t size;
    uint8_t *buff;
    uint32_t rsize;
)
__vsf_component_peda_ifs(vk_mal_write,
    uint64_t addr;
    uint32_t size;
    uint8_t *buff;
    uint32_t wsize;
)
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_mal_init(vk_mal_t *pthis);
extern vsf_err_t vk_mal_fini(vk_mal_t *pthis);
extern uint_fast32_t vk_mal_blksz(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op);
// vk_mal_prepare_buffer is used to get buffer for read/write if VSF_MAL_LOCAL_BUFFER is supported
extern bool vk_mal_prepare_buffer(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_mal_op_t op, vsf_mem_t *mem);
// to erase all, size should be 0
extern vsf_err_t vk_mal_erase(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size);
extern vsf_err_t vk_mal_read(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);
extern vsf_err_t vk_mal_write(vk_mal_t *pthis, uint_fast64_t addr, uint_fast32_t size, uint8_t *buff);

#if VSF_USE_SIMPLE_STREAM == ENABLED
extern vsf_err_t vk_mal_read_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
extern vsf_err_t vk_mal_write_stream(vk_mal_stream_t *pthis, uint_fast64_t addr, uint_fast32_t size, vsf_stream_t *stream);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./driver/mim_mal/vsf_mim_mal.h"
#include "./driver/mem_mal/vsf_mem_mal.h"
#include "./driver/fakefat32_mal/vsf_fakefat32_mal.h"
#include "./driver/scsi_mal/vsf_scsi_mal.h"
#include "./driver/file_mal/vsf_file_mal.h"

#undef __VSF_MAL_CLASS_IMPLEMENT
#undef __VSF_MAL_CLASS_INHERIT__

#endif      // VSF_USE_MAL
#endif      // __VSF_MAL_H__
