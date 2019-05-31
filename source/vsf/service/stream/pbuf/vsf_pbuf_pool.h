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

#ifndef __VSF_PBUF_POOL_H__
#define __VSF_PBUF_POOL_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"
#if VSF_USE_PBUF == ENABLED
#include "./vsf_pbuf.h"
#include "service/pool/vsf_pool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
#define __declare_pbuf_pool(__NAME)                                             \
        typedef struct __NAME##_pool_item_t __NAME##_pool_item_t;               \
        typedef struct __NAME##_pool_t      __NAME##_pool_t;

#define __def_pbuf_pool(__NAME, __SIZE, ...)                                    \
        struct __NAME##_pool_item_t {                                           \
            implement(vsf_pbuf_t);                                              \
            uint8_t chBuffer[__SIZE];                                           \
        };                                                                      \
        struct __NAME##_pool_t {                                                \
            implement(vsf_pbuf_pool_t)                                          \
            __VA_ARGS__                                                         \
        };

#define __vsf_pbuf_pool_req_pbuf_evt(__NAME, __POOL)                            \
            (req_pbuf_evt_t)                                                    \
            { .fnHandler = __NAME##_req_pbuf_evt_handler,                       \
              .pTarget = (__POOL),                                              \
            }

#if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
#define __implement_pbuf_pool(__NAME)                                           \
      vsf_pbuf_t *__NAME##_req_pbuf_evt_handler(void *pTarget,                  \
                                                int_fast32_t nNoLessThan,       \
                                                int_fast32_t nBestSize,         \
                                                uint_fast16_t hwReserve)        \
{                                                                               \
    vsf_pool(__NAME) *ptThis = (vsf_pool(__NAME) *)pTarget;                     \
    ASSERT(NULL != ptThis);                                                     \
    if (vsf_pbuf_get_pool_item_count(                                           \
            &(ptThis->use_as__vsf_pbuf_pool_t)) <= hwReserve) {                 \
        return NULL;                                                            \
    }                                                                           \
    return vsf_pbuf_pool_alloc(&(ptThis->use_as__vsf_pbuf_pool_t));             \
}
#else
#define __implement_pbuf_pool(__NAME)                                           \
      vsf_pbuf_t *__NAME##_req_pbuf_evt_handler(void *pTarget,                  \
                                                int_fast32_t nNoLessThan,       \
                                                int_fast32_t nBestSize,         \
                                                uint_fast16_t hwReserve)        \
{                                                                               \
    vsf_pool(__NAME) *ptThis = (vsf_pool(__NAME) *)pTarget;                     \
    ASSERT(NULL != ptThis);                                                     \
    return vsf_pbuf_pool_alloc(&(ptThis->use_as__vsf_pbuf_pool_t));             \
}
#endif

#define __init_pbuf_pool(__NAME, __POOL, __ID, __COUNT, ...)                    \
        do {                                                                    \
            NO_INIT static vsf_pool_block(__NAME)                               \
                    s_t##__NAME##DataBuffer[__COUNT];                           \
            const vsf_pbuf_adapter_t *pAdatper =                                \
                    vsf_pbuf_adapter_get(__ID);                                 \
            vsf_pool_cfg_t tCFG = {                                             \
                .pTarget = (void *)pAdatper,                                    \
                .pchPoolName = #__NAME,                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
                                                                                \
            vsf_pbuf_pool_init( &((__POOL)->use_as__vsf_pbuf_pool_t),           \
                                sizeof(vsf_pool_block(__NAME)),                 \
                                __alignof__(vsf_pool_block(__NAME)),            \
                                &tCFG);                                         \
            vsf_pbuf_pool_add_buffer(   &((__POOL)->use_as__vsf_pbuf_pool_t),   \
                                        s_t##__NAME##DataBuffer,                \
                                        sizeof(s_t##__NAME##DataBuffer),        \
                                        sizeof(vsf_pool_block(__NAME)));        \
        } while(0)

#define init_pbuf_pool(__NAME, __POOL, __ID, __COUNT, ...)                      \
        __init_pbuf_pool(__NAME, (__POOL), (__ID), (__COUNT), __VA_ARGS__)

#define __prepare_pbuf_pool(__NAME, __POOL, __ID, ...)                          \
        do {                                                                    \
            const vsf_pbuf_adapter_t *pAdatper =                                \
                    vsf_pbuf_adapter_get(__ID);                                 \
            vsf_pool_cfg_t tCFG = {                                             \
                .pTarget = (void *)pAdatper,                                    \
                .pchPoolName = #__NAME,                                         \
                .fnItemInit = &vsf_pbuf_pool_item_init_event_handler,           \
                __VA_ARGS__                                                     \
            };                                                                  \
                                                                                \
            vsf_pbuf_pool_init( &((__POOL)->use_as__vsf_pbuf_pool_t),           \
                                sizeof(vsf_pool_block(__NAME)),                 \
                                __alignof__(vsf_pool_block(__NAME)),            \
                                &tCFG);                                         \
        } while(0)

#define prepare_pbuf_pool(__NAME, __POOL, __ID, ...)                            \
        __prepare_pbuf_pool(__NAME, (__POOL), (__ID), __VA_ARGS__)


#define def_pbuf_pool(__NAME, __SIZE, ...)                                      \
        __def_pbuf_pool(__NAME, (__SIZE), __VA_ARGS__)

#define declare_pbuf_pool(__NAME)       __declare_pbuf_pool(__NAME)

#define vsf_pbuf_pool_req_pbuf_evt(__NAME, __POOL)                                   \
            __vsf_pbuf_pool_req_pbuf_evt(__NAME, (__POOL))

#define implement_pbuf_pool(...)        __implement_pbuf_pool(__VA_ARGS__)        

#define vsf_pbuf_pool_adapter(__ID, __POOL)                                     \
        {                                                                       \
            .ptTarget = (__POOL),                                               \
            .ID = (__ID),                                                       \
            .piMethods = &VSF_PBUF_ADAPTER_METHODS_STREAM_SRC,                  \
        }
/*============================ TYPES =========================================*/

typedef union vsf_pbuf_pool_item_t vsf_pbuf_pool_item_t;            
typedef struct vsf_pbuf_pool_t vsf_pbuf_pool_t;

union vsf_pbuf_pool_item_t {                                                 
    implement(vsf_slist_t)                                                   
    vsf_pbuf_t                  tMem;                                            
};                                                                           
struct vsf_pbuf_pool_t {                                                     
    implement(vsf_pool_t)                                                    
};                                                                           


/*============================ GLOBAL VARIABLES ==============================*/
extern const i_pbuf_methods_t VSF_PBUF_ADAPTER_METHODS_STREAM_SRC;

/*============================ PROTOTYPES ====================================*/
extern 
void vsf_pbuf_pool_init(vsf_pbuf_pool_t *pthis,  
                        uint32_t wItemSize, 
                        uint_fast16_t hwAlign, 
                        vsf_pool_cfg_t *pcfg);         

extern 
bool vsf_pbuf_pool_add_buffer(  vsf_pbuf_pool_t *pthis, 
                                void *ptBuffer, 
                                uint_fast32_t wSize, 
                                uint_fast16_t hwItemSize) ;   
                           
extern vsf_pbuf_t *vsf_pbuf_pool_alloc(vsf_pbuf_pool_t *);            
           
extern void vsf_pbuf_pool_free(vsf_pbuf_pool_t *, vsf_pbuf_t *); 
                         
SECTION(".text." "vsf_pbuf" "_get_pool_item_count")                                 
extern uint_fast32_t vsf_pbuf_get_pool_item_count(vsf_pbuf_pool_t *); 
       
SECTION(".text." "vsf_pbuf" "_pool_get_region")                                     
extern code_region_t *vsf_pbuf_pool_get_region(vsf_pbuf_pool_t *);  
         
SECTION(".text." "vsf_pbuf" "_pool_get_target")                                     
extern void *vsf_pbuf_pool_get_target(vsf_pbuf_pool_t *);        
            
#endif
#endif
/* EOF */
