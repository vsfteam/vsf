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

#ifndef __VSF_POOL_H__
#define __VSF_POOL_H__

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Defining type of pool member
    typedef struct xxxx_t {
        ......
    } xxxx_t;

    // 2. Declare the pool with user type
    dcl_vsf_pool(xxxx_pool)

    // 3. Defining the pool
    def_vsf_pool(xxxx_pool, xxxx_t)

    // 4. Implement the pool
    imp_vsf_pool(xxxx_pool, xxxx_t)

    // 5. Defining pool variable
    static NO_INIT vsf_pool(xxxx_pool) __xxxx_pool;

    void user_example_task(void)
    {
        ......

        // 6. Initialization user pool with size, attached object and code region .
        //    the attached object and code region can be omitted.

        //    VSF_POOL_INIT(xxxx_pool, &__xxxx_pool, 8);

        //    if you haven't decided the number of item inside the pool, you can
        //    reply on the so-called "feed-on-heap" feature.

        //    VSF_POOL_PREPARE(xxxx_pool, &__xxxx_pool);

        //    you just need to use either one of these two APIs mentioned above.
        //    A tyipcal code example taking advantage of those two APIs is:

    #ifndef XXXX_POOL_ITEM_NUM
        VSF_POOL_PREPARE(xxxx_pool, &__xxxx_pool);
    #else
        VSF_POOL_INIT(xxxx_pool, &__xxxx_pool, XXXX_POOL_ITEM_NUM);
    #endif

        ......

        // 7. Alloc memory from user pool
        xxxx_t *target_ptr = VSF_POOL_ALLOC(xxxx_pool, &__xxxx_pool);
        if (NULL != target_ptr) {
            // 7.1. Do something when alloc successfully

            // 8. Free memory to user pool
            VSF_POOL_FREE(xxxx_pool, &__xxxx_pool, target_ptr);
        } else {
            // 7.2. Do something when alloc failed
        }

        ......
    }
 */

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_POOL == ENABLED
/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__VSF_POOL_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_POOL_CLASS_IMPLEMENT
#elif   defined(__VSF_POOL_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_POOL_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

/*! \note Enable statistic mode by default */
#ifndef VSF_POOL_CFG_STATISTIC_MODE
#   define  VSF_POOL_CFG_STATISTIC_MODE     ENABLED
#endif

#ifndef VSF_POOL_CFG_FEED_ON_HEAP
#   define VSF_POOL_CFG_FEED_ON_HEAP        ENABLED
#endif

#ifndef VSF_POOL_CFG_SUPPORT_USER_OBJECT
#   define VSF_POOL_CFG_SUPPORT_USER_OBJECT ENABLED
#endif

#define __vsf_pool(__name)          __name##_pool_t
#define __vsf_pool_item(__name)     __name##_pool_item_t

#define __declare_vsf_pool(__name)                                              \
            typedef union vsf_pool_item(__name) vsf_pool_item(__name);          \
            dcl_class(vsf_pool(__name))

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
#   define ____define_vsf_pool_tag(__name)                                      \
            SECTION(".text." #__name "_pool_get_tag")                           \
            extern uintptr_t __name##_pool_get_tag(vsf_pool(__name) *);         \
            SECTION(".text." #__name "_pool_set_tag")                           \
            extern uintptr_t __name##_pool_set_tag(vsf_pool(__name) *, uintptr_t);
#   define ____implement_vsf_pool_tag(__name)                                   \
            WEAK(__name##_pool_get_tag)                                         \
            SECTION(".text." #__name "_pool_get_tag")                           \
            uintptr_t __name##_pool_get_tag(vsf_pool(__name) *this_ptr)         \
            {                                                                   \
                return vsf_pool_get_tag((vsf_pool_t *)this_ptr);                \
            }                                                                   \
            WEAK(__name##_pool_set_tag)                                         \
            SECTION(".text." #__name "_pool_set_tag")                           \
            uintptr_t __name##_pool_set_tag( vsf_pool(__name) *this_ptr,        \
                                    uintptr_t target_ptr)                       \
            {                                                                   \
                return vsf_pool_set_tag((vsf_pool_t *)this_ptr, target_ptr);    \
            }
#   define __define_vsf_pool_tag(__name)    ____define_vsf_pool_tag(__name)
#   define __implement_vsf_pool_tag(__name) ____implement_vsf_pool_tag(__name)
#else
#   define __define_vsf_pool_tag(__name)
#   define __implement_vsf_pool_tag(__name)
#endif

#define __define_vsf_pool(__name, __type)                                       \
    union vsf_pool_item(__name) {                                               \
        implement(vsf_slist_t)                                                  \
        __type __item;                                                          \
    };                                                                          \
    def_class(vsf_pool(__name),                                                 \
        public_member(                                                          \
            implement(vsf_pool_t)                                               \
        )                                                                       \
    )                                                                           \
    end_def_class(vsf_pool(__name))                                             \
    extern void __name##_pool_init(vsf_pool(__name) *, vsf_pool_cfg_t *);       \
    SECTION(".text." #__name "_pool_init_ex")                                   \
    extern void __name##_pool_init_ex(                                          \
        vsf_pool(__name) *, uint_fast16_t, vsf_pool_cfg_t *);                   \
    extern bool __name##_pool_add_buffer(                                       \
        vsf_pool(__name) *, uintptr_t, uint_fast32_t );                         \
    extern __type *__name##_pool_alloc(vsf_pool(__name) *);                     \
    extern void __name##_pool_free(vsf_pool(__name) *, __type *);               \
    SECTION(".text." #__name "_pool_add_buffer_ex")                             \
    extern bool __name##_pool_add_buffer_ex(                                    \
        vsf_pool(__name) *this_ptr,                                             \
        void *buffer_ptr,                                                       \
        uint_fast32_t u32_size,                                                 \
        vsf_pool_item_init_evt_handler_t *handler_fn);                          \
    SECTION(".text." #__name "_get_pool_item_count")                            \
    extern uint_fast32_t __name##_get_pool_item_count(vsf_pool(__name) *);      \
    SECTION(".text." #__name "_pool_get_region")                                \
    extern code_region_t *__name##_pool_get_region(vsf_pool(__name) *);         \
    __define_vsf_pool_tag(__name)


#define __implement_vsf_pool(__name, __type)                                    \
WEAK(__name##_pool_init)                                                        \
void __name##_pool_init(vsf_pool(__name) *this_ptr, vsf_pool_cfg_t *cfg_ptr)    \
{                                                                               \
    vsf_pool_init(  &(this_ptr->use_as__vsf_pool_t),                            \
                    sizeof(__type),                                             \
                    __alignof__(__type),                                        \
                    cfg_ptr);                                                   \
}                                                                               \
void __name##_pool_init_ex( vsf_pool(__name) *this_ptr,                         \
                            uint_fast16_t align,                                \
                            vsf_pool_cfg_t *cfg_ptr)                            \
{                                                                               \
    vsf_pool_init(  &(this_ptr->use_as__vsf_pool_t),                            \
                    sizeof(__type),                                             \
                    max(align,__alignof__(__type)),                             \
                    cfg_ptr);                                                   \
}                                                                               \
WEAK(__name##_pool_add_buffer)                                                  \
bool __name##_pool_add_buffer(                                                  \
    vsf_pool(__name) *this_ptr, uintptr_t buffer_ptr, uint_fast32_t u32_size)   \
{                                                                               \
    return vsf_pool_add_buffer((vsf_pool_t *)this_ptr, buffer_ptr,              \
                            u32_size, sizeof(vsf_pool_item(__name)));           \
}                                                                               \
WEAK(__name##_pool_add_buffer_ex)                                               \
SECTION(".text." #__name "_pool_add_buffer_ex")                                 \
bool __name##_pool_add_buffer_ex(                                               \
        vsf_pool(__name) *this_ptr,                                             \
        void *buffer_ptr,                                                       \
        uint_fast32_t u32_size,                                                 \
        vsf_pool_item_init_evt_handler_t *handler_fn)                           \
{                                                                               \
    return vsf_pool_add_buffer_ex(                                              \
                    (vsf_pool_t *)this_ptr, (uintptr_t)buffer_ptr,              \
                    u32_size,                                                   \
                    sizeof(vsf_pool_item(__name)),                              \
                    handler_fn);                                                \
}                                                                               \
WEAK(__name##_pool_alloc)                                                       \
__type *__name##_pool_alloc(vsf_pool(__name) *this_ptr)                         \
{                                                                               \
    return (__type *)vsf_pool_alloc((vsf_pool_t *)this_ptr);                    \
}                                                                               \
WEAK(__name##_pool_free)                                                        \
void __name##_pool_free(vsf_pool(__name) *this_ptr, __type *item_ptr)           \
{                                                                               \
    vsf_pool_free((vsf_pool_t *)this_ptr, (uintptr_t)item_ptr);                 \
}                                                                               \
WEAK(__name##_get_pool_item_count)                                              \
SECTION(".text." #__name "_get_pool_item_count")                                \
uint_fast32_t __name##_get_pool_item_count(vsf_pool(__name) *this_ptr)          \
{                                                                               \
    return vsf_pool_get_count((vsf_pool_t *)this_ptr);                          \
}                                                                               \
WEAK(__name##_pool_get_region)                                                  \
SECTION(".text." #__name "_pool_get_region")                                    \
code_region_t *__name##_pool_get_region(vsf_pool(__name) *this_ptr)             \
{                                                                               \
    return vsf_pool_get_region((vsf_pool_t *)this_ptr);                         \
}                                                                               \
__implement_vsf_pool_tag(__name)




//! \name pool normal access
//! @{
#define vsf_pool(__name)            __vsf_pool(__name)
#define vsf_pool_item(__name)       __vsf_pool_item(__name)

#define declare_vsf_pool(__name)        /* the name of the pool */              \
            __declare_vsf_pool(__name)

#define dcl_vsf_pool(__name)            /* the name of the pool */              \
            declare_vsf_pool(__name)

#define define_vsf_pool(__name,         /* the name of the pool */              \
                     __type)            /* the type of the unit */              \
            __define_vsf_pool(__name, __type)

#define def_vsf_pool(__name,            /* the name of the pool */              \
                     __type)            /* the type of the unit */              \
            define_vsf_pool(__name, __type)

#define end_def_pool(__name)
#define end_define_pool(__name)

#define implement_vsf_pool(__name,      /* the name of the pool */              \
                     __type)            /* the type of the unit */              \
            __implement_vsf_pool(__name, __type)

#define imp_vsf_pool(__name,            /* the name of the pool */              \
                     __type)            /* the type of the unit */              \
            implement_vsf_pool(__name, __type)

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define VSF_POOL_INIT(__NAME,       /* the name of the pool */                  \
                      __VSF_POOL,   /* the address of the pool */               \
                      __SIZE)       /* the total size of the pool */            \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = #__NAME,                                   \
                };                                                              \
                __NAME##_pool_init((__VSF_POOL), &cfg);                         \
                static NO_INIT vsf_pool_item(__NAME) __buffer[__SIZE];          \
                vsf_pool_add_buffer((vsf_pool_t *)(__VSF_POOL),                 \
                                    __buffer,                                   \
                                    sizeof(__buffer),                           \
                                    sizeof(vsf_pool_item(__NAME)));             \
            } while(0)
#else
#define VSF_POOL_INIT(__NAME,       /* the name of the pool */                  \
                      __VSF_POOL,   /* the address of the pool */               \
                      __SIZE,       /* the total size of the pool */            \
                      ...)          /* the address of an attached object */     \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = #__NAME,                                   \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init((__VSF_POOL), &cfg);                         \
                static NO_INIT vsf_pool_item(__NAME) __buffer[__SIZE];          \
                vsf_pool_add_buffer((vsf_pool_t *)(__VSF_POOL),                 \
                                    __buffer,                                   \
                                    sizeof(__buffer),                           \
                                    sizeof(vsf_pool_item(__NAME)));             \
            } while(0)
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define VSF_POOL_PREPARE(__NAME,    /* the name of the pool */                  \
                      __VSF_POOL)   /* the address of the pool */               \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool(__name) *this_ptr = (__VSF_POOL);                       \
                vsf_pool_cfg_t cfg = {                                          \
                    (const uint8_t *)#__NAME,

#define END_VSF_POOL_PREPARE(__NAME)                                            \
                };                                                              \
                __NAME##_pool_init((this_ptr), &cfg);                           \
            } while(0);

#else
#define VSF_POOL_PREPARE(                                                       \
                        __NAME,     /* the name of the pool */                  \
                        __VSF_POOL, /* the address of the pool */               \
                        ...)        /* the address of an attached object */     \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = (const uint8_t *)#__NAME,                  \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init((__VSF_POOL), &cfg);                         \
            } while(0)
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define VSF_POOL_INIT_EX(                                                       \
                    __NAME,         /* the name of the pool */                  \
                    __VSF_POOL,     /* the address of the pool */               \
                    __SIZE,         /* the total size of the pool */            \
                    __ALIGN)        /* the item alignment */                    \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = #__NAME,                                   \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &cfg);           \
                static NO_INIT vsf_pool_item(__NAME)                            \
                    __buffer[__SIZE] ALIGN((__ALIGN));                          \
                vsf_pool_add_buffer((vsf_pool_t *)(__VSF_POOL),                 \
                                    __buffer,                                   \
                                    sizeof(__buffer),                           \
                                    sizeof(vsf_pool_item(__NAME)));             \
            } while(0)
#else
#define VSF_POOL_INIT_EX(                                                       \
                    __NAME,         /* the name of the pool */                  \
                    __VSF_POOL,     /* the address of the pool */               \
                    __SIZE,         /* the total size of the pool */            \
                    __ALIGN,        /* the item alignment */                    \
                    ...)            /* the address of an attached object */     \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = #__NAME,                                   \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &cfg);           \
                static NO_INIT vsf_pool_item(__NAME)                            \
                    __buffer[__SIZE] ALIGN((__ALIGN));                          \
                vsf_pool_add_buffer((vsf_pool_t *)(__VSF_POOL),                 \
                                    __buffer,                                   \
                                    sizeof(__buffer),                           \
                                    sizeof(vsf_pool_item(__NAME)));             \
            } while(0)
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define VSF_POOL_PREPARE_EX(                                                    \
                    __NAME,         /* the name of the pool */                  \
                    __VSF_POOL,     /* the address of the pool */               \
                    __ALIGN)        /* the item alignment */                    \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = (const uint8_t *)#__NAME,                  \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &cfg);           \
            } while(0)
#else
#define VSF_POOL_PREPARE_EX(                                                    \
                        __NAME,     /* the name of the pool */                  \
                        __VSF_POOL, /* the address of the pool */               \
                        __ALIGN,    /* the item alignment */                    \
                        ...)        /* the address of an attached object */     \
                                    /* the address of the code region obj_ptr */\
            do {                                                                \
                vsf_pool_cfg_t cfg = {                                          \
                    .pool_name_str = (const uint8_t *)#__NAME,                  \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &cfg);           \
            } while(0)
#endif

#define VSF_POOL_ADD_BUFFER(__NAME,     /* the name of the pool */              \
                            __VSF_POOL, /* the address of the pool */           \
                            __BUFFER,   /* the address of the buffer */         \
                            __SIZE)     /* the size of the buffer */            \
            __NAME##_pool_add_buffer(   (__VSF_POOL),                           \
                                        (uintptr_t)(__BUFFER),                  \
                                        (__SIZE))

#define VSF_POOL_ADD_BUFFER_EX(__NAME,  /* the name of the pool */              \
                            __VSF_POOL, /* the address of the pool */           \
                            __BUFFER,   /* the address of the buffer */         \
                            __SIZE,     /* the size of the buffer */            \
                            __HANDLER)  /* the block initialisation routine */  \
            __NAME##_pool_add_buffer_ex((__VSF_POOL),                           \
                                        (uintptr_t)(__BUFFER),                  \
                                        (__SIZE),                               \
                                        (__HANDLER))

#define VSF_POOL_FREE(__NAME,           /* the name of the pool */              \
                      __VSF_POOL,       /* the address of the pool */           \
                      __ITEM)           /* the address of the memory block */   \
                                        /* to be released */                    \
            __NAME##_pool_free((__VSF_POOL), (__ITEM))

#define VSF_POOL_ALLOC(__NAME,          /* the name of the pool */              \
                       __VSF_POOL)      /* the address of the pool */           \
            __NAME##_pool_alloc((__VSF_POOL))

#define VSF_POOL_ITEM_COUNT(__NAME,     /* the name of the pool */              \
                            __VSF_POOL) /* the address of the pool */           \
            __NAME##_get_pool_item_count((__VSF_POOL))

#define VSF_POOL_GET_REGION(__NAME,      /* the name of the pool */             \
                           __VSF_POOL)   /* the address of the pool */          \
            __NAME##_pool_get_region((__VSF_POOL))

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
#define VSF_POOL_GET_TAG(__NAME,     /* the name of the pool */                 \
                         __VSF_POOL)    /* the address of the pool */           \
            __NAME##_pool_get_tag((__VSF_POOL))

#define VSF_POOL_SET_TAG(__NAME,     /* the name of the pool */                 \
                         __VSF_POOL,    /* the address of the pool */           \
                         __TARGET)      /* the address of the target */         \
            __NAME##_pool_set_tag((__VSF_POOL), (__TARGET))
#endif
//! @}

/*============================ TYPES =========================================*/

dcl_class(vsf_pool_t)

typedef
void
vsf_pool_item_init_evt_handler_t(   uintptr_t target_ptr,
                                    uintptr_t item_ptr,
                                    uint_fast32_t item_size);


#if     VSF_POOL_CFG_STATISTIC_MODE == ENABLED                                  \
    ||  VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED
typedef struct vsf_pool_info_t{
#   if  VSF_POOL_CFG_STATISTIC_MODE == ENABLED
    implement(vsf_slist_node_t)
    const uint8_t *pool_name;
#   endif
    uint32_t item_size;
    uint16_t u15_align          : 15;
    uint16_t is_no_feed_on_heap : 1;
}vsf_pool_info_t;
#endif

//! \name pool
//! @{
def_class(vsf_pool_t,

    private_member(
        vsf_slist_t free_list;      /*!< free list */
        uint16_t free_cnt;          /*!< the number of free blocks */
        uint16_t used_cnt;
    )

#if     VSF_POOL_CFG_STATISTIC_MODE == ENABLED                                  \
    ||  VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED
    private_member(
        implement_ex(vsf_pool_info_t, statistic)
    )
#endif

#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
    private_member(
        vsf_pool_item_init_evt_handler_t *item_init_fn;
    )
#endif

#if !defined(VSF_POOL_CFG_ATOM_ACCESS)
    private_member(
        /*! protection region defined by user */
        code_region_t *region_ptr;
    )
#endif

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    protected_member(
        /*! you can use it to carry mutex or other target */
        uintptr_t target_ptr;
    )
#endif

)
end_def_class(vsf_pool_t)
//! @}

typedef struct vsf_pool_cfg_t {
    const uint8_t *pool_name_str;
    uintptr_t target_ptr;
    code_region_t *region_ptr;
    vsf_pool_item_init_evt_handler_t *item_init_fn;
}vsf_pool_cfg_t;


//! \name vsf pool interface
//! @{
dcl_interface(i_pool_t)
def_interface(i_pool_t)
    void (*Init)            (vsf_pool_t *obj_ptr,
                             uint32_t item_size,
                             uint_fast16_t align,
                             vsf_pool_cfg_t *cfg_ptr);
    struct {
        bool (*AddEx)       (vsf_pool_t *obj_ptr,
                             uintptr_t buffer_ptr,
                             uint32_t buffer_size,
                             uint32_t item_size,
                             vsf_pool_item_init_evt_handler_t *item_init_fn);
        bool (*Add)         (vsf_pool_t *obj_ptr,
                             uintptr_t buffer_ptr,
                             uint32_t buffer_size,
                             uint32_t item_size);
    } Buffer;

    uintptr_t (*Allocate)   (vsf_pool_t *obj_ptr);
    void (*Free)            (vsf_pool_t *obj_ptr, uintptr_t item_ptr);
    uint_fast16_t  (*Count) (vsf_pool_t *obj_ptr);
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    struct {
        uintptr_t (*Get)    (vsf_pool_t *obj_ptr);
        uintptr_t (*Set)    (vsf_pool_t *obj_ptr, uintptr_t target_ptr);
    } Tag;
#endif
end_def_interface(i_pool_t)
//! @}
/*============================ GLOBAL VARIABLES ==============================*/
extern const i_pool_t VSF_POOL;

/*============================ PROTOTYPES ====================================*/

/*! \brief initialise target pool
 *! \param obj_ptr address of the target pool
 *! \param item_size memory item size
 *! \param align Item Alignment
 *! \param cfg_ptr configurations
 *! \return none
 */
extern void vsf_pool_init(  vsf_pool_t *obj_ptr,
                            uint32_t item_size,
                            uint_fast16_t align,
                            vsf_pool_cfg_t *cfg_ptr);

/*! \brief add memory to pool
 *! \param obj_ptr             address of the target pool
 *! \param buffer_ptr          address of the target memory
 *! \param buffer_size      the size of the target memory
 *! \param item_size        memory block size of the pool
 *! \param item_init_fn       block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added
 */
extern bool
vsf_pool_add_buffer_ex( vsf_pool_t *obj_ptr,
                        uintptr_t buffer_ptr,
                        uint32_t buffer_size,
                        uint32_t item_size,
                        vsf_pool_item_init_evt_handler_t *item_init_fn);

/*! \brief add memory to pool
 *! \param obj_ptr        address of the target pool
 *! \param buffer_ptr      address of the target memory
 *! \param buffer_size  the size of the target memory
 *! \param item_size    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added
 */
extern bool vsf_pool_add_buffer(vsf_pool_t *obj_ptr,
                                uintptr_t buffer_ptr,
                                uint32_t buffer_size,
                                uint32_t item_size);

/*! \brief try to fetch a memory block from the target pool
 *! \param obj_ptr    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
extern uintptr_t vsf_pool_alloc(vsf_pool_t *obj_ptr);

/*! \brief return a memory block to the target pool
 *! \param obj_ptr     address of the target pool
 *! \param item_ptr    target memory block
 *! \return none
 */
extern void vsf_pool_free(vsf_pool_t *obj_ptr, uintptr_t item_ptr);

SECTION(".text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param obj_ptr    address of the target pool
 *! \return the number of memory blocks
 */
extern uint_fast16_t vsf_pool_get_count(vsf_pool_t *obj_ptr);

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
SECTION(".text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param obj_ptr    address of the target pool
 *! \return the address of the object
 */
extern uintptr_t vsf_pool_get_tag(vsf_pool_t *obj_ptr);

SECTION(".text.vsf.utilities.vsf_pool_set_tag")
/*! \brief set the address of the object which is attached to the pool
 *! \param obj_ptr    address of the target pool
 *! \return the address of the object
 */
extern uintptr_t vsf_pool_set_tag(vsf_pool_t *obj_ptr, uintptr_t target_ptr);
#endif

SECTION(".text.vsf.utilities.vsf_pool_get_region")
/*! \brief get the address of the code region used by this pool
 *! \param obj_ptr    address of the target pool
 *! \return the address of the code region
 */
extern code_region_t *vsf_pool_get_region(vsf_pool_t *obj_ptr);

#ifdef __cplusplus
}
#endif

#endif
#endif
