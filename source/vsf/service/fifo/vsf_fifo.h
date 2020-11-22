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

#ifndef __VSF_FIFO_H__
#define __VSF_FIFO_H__

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Defining type of fifo member
    typedef struct xxxx_t {
        ......
    } xxxx_t;

    // 2. Declare the fifo with user type
    dcl_vsf_fifo(xxxx_fifo)

    // 3. Defining the fifo
    def_vsf_fifo(xxxx_fifo, xxxx_t, 8)

    // 4. Defining fifo variable
    static NO_INIT vsf_fifo(xxxx_fifo) __xxxx_fifo;

    void user_example_task(void)
    {
        ......

        // 5. Initialization user fifo.

              vsf_fifo_init(&__xxxx_fifo);

        ......

        // 6. Push to user fifo

              xxxx_t xxxx_to_push = {
                  ........
              };
              vsf_fifo_push(&__xxxx_fifo, &xxxx_to_push);

        // 7. Pop from user fifo

              xxxx_t xxxx_to_pop;
              vsf_fifo_pop(&__xxxx_fifo, &xxxx_to_pop);

        // 8. Get pointer of head/tail element

              xxxx_t *xxxx_head_ptr = vsf_fifo_get_head(&__xxxx_fifo);
              write members pointed by xxxx_head_ptr;
              vsf_fifo_push(&__xxxx_fifo, NULL);

              xxxx_t *xxxx_tail_ptr = vsf_fifo_get_tail(&__xxxx_fifo);
              read members pointed by xxxx_head_ptr
              vsf_fifo_pop(&__xxxx_fifo, NULL);

        ......
    }
 */

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_FIFO == ENABLED
/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_FIFO_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_FIFO_CLASS_IMPLEMENT
#elif   defined(__VSF_FIFO_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_FIFO_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#define __vsf_fifo(__name)              __name##_fifo_t

#define __declare_vsf_fifo(__name)                                              \
            dcl_simple_class(vsf_fifo(__name))

#define __define_vsf_fifo(__name, __item_type, __depth)                         \
            def_simple_class(vsf_fifo(__name)) {                                \
                private_member(                                                 \
                    implement(vsf_fifo_base_t)                                  \
                    __item_type mem[__depth];                                   \
                )                                                               \
            };

//! \name fifo normal access
//! @{
#define vsf_fifo(__name)                __vsf_fifo(__name)

#define declare_vsf_fifo(__name)        /* the name of the fifo */              \
            __declare_vsf_fifo(__name)

#define dcl_vsf_fifo(__name)            /* the name of the fifo */              \
            declare_vsf_fifo(__name)

#define define_vsf_fifo(__name,         /* the name of the fifo */              \
                        __item_type,    /* the type of the item */              \
                        __depth)        /* the depth of the fifo */             \
            __define_vsf_fifo(__name, __item_type, (__depth))

#define def_vsf_fifo(   __name,         /* the name of the fifo */              \
                        __item_type,    /* the type of the item */              \
                        __depth)        /* the depth of the fifo */             \
            define_vsf_fifo(__name, __item_type, (__depth))

#define end_def_fifo(__name)
#define end_define_fifo(__name)

// upper-case for simplified user interface
#define VSF_FIFO_INIT(__fifo)           /* the address of the fifo */           \
            vsf_fifo_init((vsf_fifo_t *)(__fifo), dimof(__fifo->__mem));

#define VSF_FIFO_GET_HEAD(__fifo)       /* the address of the fifo */           \
            vsf_fifo_get_head((vsf_fifo_t *)(__fifo), sizeof(__fifo->__mem[0]))

#define VSF_FIFO_GET_TAIL(__fifo)       /* the address of the fifo */           \
            vsf_fifo_get_tail((vsf_fifo_t *)(__fifo), sizeof(__fifo->__mem[0]))

#define VSF_FIFO_PUSH(  __fifo,         /* the address of the fifo */           \
                        __item)         /* the address of the item to push */   \
            vsf_fifo_push((vsf_fifo_t *)(__fifo), (__item), sizeof(__fifo->__mem[0]))

#define VSF_FIFO_POP(   __fifo,         /* the address of the fifo */           \
                        __item)         /* the address of the item to push */   \
            vsf_fifo_pop((vsf_fifo_t *)(__fifo), (__item), sizeof(__fifo->__mem[0]))

//! @}

#ifndef vsf_fifo_index_t
#   define vsf_fifo_index_t             uint8_t
#endif
#ifndef vsf_fifo_fast_index_t
#   define vsf_fifo_fast_index_t        uint_fast8_t
#endif
#ifndef vsf_fifo_item_size_t
#   define vsf_fifo_item_size_t         uint16_t
#endif
#ifndef vsf_fifo_fast_item_size_t
#   define vsf_fifo_fast_item_size_t    uint_fast16_t
#endif

/*============================ TYPES =========================================*/

dcl_simple_class(vsf_fifo_t)
dcl_simple_class(vsf_fifo_base_t)

def_simple_class(vsf_fifo_base_t) {
    private_member(
        vsf_fifo_index_t head;
        vsf_fifo_index_t tail;
        vsf_fifo_index_t number;
        vsf_fifo_index_t depth;
    )
};

//! \name fifo
//! @{
def_simple_class(vsf_fifo_t) {
    private_member(
        implement(vsf_fifo_base_t)
        uint32_t nodes[0];
    )
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_fifo_init(vsf_fifo_t *fifo, vsf_fifo_fast_index_t fifo_depth);
extern bool vsf_fifo_push(vsf_fifo_t *fifo, uintptr_t item, vsf_fifo_fast_item_size_t item_size);
extern bool vsf_fifo_pop(vsf_fifo_t *fifo, uintptr_t item, vsf_fifo_fast_item_size_t item_size);

SECTION(".text.vsf.utilities.vsf_fifo_get_head")
extern uintptr_t vsf_fifo_get_head(vsf_fifo_t *fifo, vsf_fifo_fast_item_size_t item_size);

SECTION(".text.vsf.utilities.vsf_fifo_get_tail")
extern uintptr_t vsf_fifo_get_tail(vsf_fifo_t *fifo, vsf_fifo_fast_item_size_t item_size);

SECTION(".text.vsf.utilities.vsf_fifo_get_number")
extern vsf_fifo_index_t vsf_fifo_get_number(vsf_fifo_t *fifo);

#ifdef __cplusplus
}
#endif

#endif
#endif
