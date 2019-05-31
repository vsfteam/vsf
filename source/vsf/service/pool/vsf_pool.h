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
    typedef struct {
        ......
    } xxxx_t;

    // 2. Declare the pool with user type
    declare_vsf_pool(xxxx_pool, xxxx_t)

    // 3. Defining the pool  
    def_vsf_pool(xxxx_pool, xxxx_t)
    
    // 4. Implement the pool
    implement_vsf_pool(xxxx_pool, xxxx_t )

    // 4. Defining pool variable
    static NO_INIT vsf_pool(xxxx_pool) __xxxx_pool;

    void user_example_task(void)
    {
        ......

        // 5. Initialization user pool with size, attached object and code region .
        //    the attached object and code region can be omitted.

        //    VSF_POOL_INIT(xxxx_pool, &__xxxx_pool, 8);

        //    if you haven't decided the number of item inside the pool, you can
        //    replies on the so-called "feed-on-heap" feature.

        //    VSF_POOL_PREPARE(xxxx_pool, &__xxxx_pool);

        //    you just need to use either one of these two APIs mentioned above.
        //    A tyipcal code example taking advantage of those two APIs is:

    #ifndef XXXX_POOL_ITEM_NUM
        VSF_POOL_PREPARE(xxxx_pool, &__xxxx_pool);
    #else
        VSF_POOL_INIT(xxxx_pool, &__xxxx_pool, XXXX_POOL_ITEM_NUM);
    #endif

        ......

        // 6. Alloc memory from user pool
        xxxx_t *ptarget = VSF_POOL_ALLOC(xxxx_pool, &__xxxx_pool);
        if (NULL != ptarget) {
            // 6.1. Do something when alloc successfully
            
            // 7. Free memory to user pool
            VSF_POOL_FREE(xxxx_pool, &__xxxx_pool, ptarget);
        } else {
            // 6.2. Do something when alloc failed
        }

        ......
    }
 */

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_POOL_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_POOL_CLASS_IMPLEMENT
#elif   defined(__VSF_POOL_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_POOL_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"


/*============================ MACROS ========================================*/

#define __vsf_pool(__name)          __name##_pool_t
#define __vsf_pool_block(__name)    __name##_pool_item_t

#define __declare_vsf_pool(__name)                                              \
            typedef union __name##_pool_item_t __name##_pool_item_t;            \
            typedef struct __name##_pool_t __name##_pool_t;

#define __def_vsf_pool(__name, __type)                                          \
    union __name##_pool_item_t {                                                \
        implement(vsf_slist_t)                                                  \
        __type                  tMem;                                           \
    };                                                                          \
    struct __name##_pool_t {                                                    \
        implement(vsf_pool_t)                                                   \
    };                                                                          \
    extern void __name##_pool_init(__name##_pool_t *, vsf_pool_cfg_t *);        \
SECTION(".text." #__name "_pool_init_ex")                                       \
    extern void __name##_pool_init_ex(                                          \
        __name##_pool_t *, uint_fast16_t, vsf_pool_cfg_t *);                    \
    extern bool __name##_pool_add_buffer(                                       \
        __name##_pool_t *, void *, uint_fast32_t );                             \
    extern __type *__name##_pool_alloc(__name##_pool_t *);                      \
    extern void __name##_pool_free(__name##_pool_t *, __type *);                \
SECTION(".text." #__name "_pool_add_buffer_ex")                                 \
    extern bool __name##_pool_add_buffer_ex(                                    \
        __name##_pool_t *pthis,                                                 \
        void *ptBuffer,                                                         \
        uint_fast32_t wSize,                                                    \
        vsf_pool_item_init_evt_handler_t *fnHandler);                           \
SECTION(".text." #__name "_get_pool_item_count")                                \
    extern uint_fast32_t __name##_get_pool_item_count(__name##_pool_t *);       \
SECTION(".text." #__name "_pool_get_region")                                    \
    extern code_region_t *__name##_pool_get_region(__name##_pool_t *);          \
SECTION(".text." #__name "_pool_get_target")                                    \
    extern void *__name##_pool_get_target(__name##_pool_t *);                   \
SECTION(".text." #__name "_pool_set_target")                                    \
    extern void *__name##_pool_set_target(__name##_pool_t *, void *);


#define __implement_vsf_pool(__name, __type)                                    \
WEAK                                                                            \
void __name##_pool_init(__name##_pool_t *ptThis, vsf_pool_cfg_t *ptCFG)         \
{                                                                               \
    vsf_pool_init(  &(ptThis->use_as__vsf_pool_t),                              \
                    sizeof(__type),                                             \
                    __alignof__(__type),                                        \
                    ptCFG);                                                     \
}                                                                               \
void __name##_pool_init_ex( __name##_pool_t *ptThis,                            \
                            uint_fast16_t hwAlign,                              \
                            vsf_pool_cfg_t *ptCFG)                              \
{                                                                               \
    vsf_pool_init(  &(ptThis->use_as__vsf_pool_t),                              \
                    sizeof(__type),                                             \
                    max(hwAlign,__alignof__(__type)),                           \
                    ptCFG);                                                     \
}                                                                               \
WEAK                                                                            \
bool __name##_pool_add_buffer(                                                  \
    __name##_pool_t *ptThis, void *ptBuffer, uint_fast32_t wSize)               \
{                                                                               \
    return vsf_pool_add_buffer((vsf_pool_t *)ptThis, ptBuffer,                  \
                            wSize, sizeof(__name##_pool_item_t));               \
}                                                                               \
WEAK                                                                            \
SECTION(".text." #__name "_pool_add_buffer_ex")                                 \
bool __name##_pool_add_buffer_ex(                                               \
        __name##_pool_t *ptThis,                                                \
        void *ptBuffer,                                                         \
        uint_fast32_t wSize,                                                    \
        vsf_pool_item_init_evt_handler_t *fnHandler)                            \
{                                                                               \
    return vsf_pool_add_buffer_ex(                                              \
                                    (vsf_pool_t *)ptThis, ptBuffer,             \
                                    wSize,                                      \
                                    sizeof(__name##_pool_item_t),               \
                                    fnHandler );                                \
}                                                                               \
WEAK                                                                            \
__type *__name##_pool_alloc(__name##_pool_t *ptThis)                            \
{                                                                               \
    return (__type *)vsf_pool_alloc((vsf_pool_t *)ptThis);                      \
}                                                                               \
WEAK                                                                            \
void __name##_pool_free(__name##_pool_t *ptThis, __type *ptItem)                \
{                                                                               \
    vsf_pool_free((vsf_pool_t *)ptThis, (void *)ptItem);                        \
}                                                                               \
WEAK                                                                            \
SECTION(".text." #__name "_get_pool_item_count")                                \
uint_fast32_t __name##_get_pool_item_count(__name##_pool_t *ptThis)             \
{                                                                               \
    return vsf_pool_get_count((vsf_pool_t *)ptThis);                            \
}                                                                               \
WEAK                                                                            \
SECTION(".text." #__name "_pool_get_region")                                    \
code_region_t *__name##_pool_get_region(__name##_pool_t *ptThis)                \
{                                                                               \
    return vsf_pool_get_region((vsf_pool_t *)ptThis);                           \
}                                                                               \
WEAK                                                                            \
SECTION(".text." #__name "_pool_get_target")                                    \
void *__name##_pool_get_target(__name##_pool_t *ptThis)                         \
{                                                                               \
    return vsf_pool_get_tag((vsf_pool_t *)ptThis);                              \
}                                                                               \
WEAK                                                                            \
SECTION(".text." #__name "_pool_set_target")                                    \
void *__name##_pool_set_target(__name##_pool_t *ptThis, void *pTarget)          \
{                                                                               \
    return vsf_pool_set_tag((vsf_pool_t *)ptThis, pTarget);                     \
}




//! \name pool normal access
//! @{
#define vsf_pool(__name)            __vsf_pool(__name)
#define vsf_pool_block(__name)      __vsf_pool_block(__name)

#define declare_vsf_pool(__name)        /* the name of the pool */              \
            __declare_vsf_pool(__name)

#define def_vsf_pool(__name,            /* the name of the pool */              \
                     __type)            /* the type of the pool */              \
            __def_vsf_pool(__name, __type)

#define implement_vsf_pool(__name,      /* the name of the pool */              \
                     __type)            /* the type of the pool */              \
            __implement_vsf_pool(__name, __type)


#define VSF_POOL_INIT(__NAME,           /* the name of the pool */              \
                      __VSF_POOL,       /* the address of the pool */           \
                      __SIZE,           /* the total size of the pool */        \
                      ...)              /* the address of an attached object */ \
                                        /* the address of the code region obj */\
            do {                                                                \
                vsf_pool_cfg_t tCFG = {                                         \
                    .pchPoolName = #__NAME,                                     \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init((__VSF_POOL), &tCFG);                        \
                static NO_INIT __NAME##_pool_item_t s_tBuffer[__SIZE];          \
                vsf_pool_add_buffer(  (vsf_pool_t *)(__VSF_POOL),               \
                                    s_tBuffer,                                  \
                                    sizeof(s_tBuffer),                          \
                                    sizeof(__NAME##_pool_item_t));              \
            } while(0)                                                                         

#define VSF_POOL_PREPARE(__NAME,           /* the name of the pool */           \
                      __VSF_POOL,       /* the address of the pool */           \
                      ...)              /* the address of an attached object */ \
                                        /* the address of the code region obj */\
            do {                                                                \
                vsf_pool_cfg_t tCFG = {                                         \
                    .pchPoolName = (const uint8_t *)#__NAME,                    \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init((__VSF_POOL), &tCFG);                        \
            } while(0)

#define VSF_POOL_INIT_EX(__NAME,        /* the name of the pool */              \
                      __VSF_POOL,       /* the address of the pool */           \
                      __SIZE,           /* the total size of the pool */        \
                      __ALIGN,          /* the item alignment */                \
                      ...)              /* the address of an attached object */ \
                                        /* the address of the code region obj */\
            do {                                                                \
                vsf_pool_cfg_t tCFG = {                                         \
                    .pchPoolName = #__NAME,                                     \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &tCFG);          \
                static NO_INIT __NAME##_pool_item_t                             \
                    s_tBuffer[__SIZE] ALIGN((__ALIGN));                         \
                vsf_pool_add_buffer(  (vsf_pool_t *)(__VSF_POOL),               \
                                    s_tBuffer,                                  \
                                    sizeof(s_tBuffer),                          \
                                    sizeof(__NAME##_pool_item_t));              \
            } while(0) 

#define VSF_POOL_PREPARE_EX(__NAME,           /* the name of the pool */        \
                      __VSF_POOL,       /* the address of the pool */           \
                      __ALIGN,          /* the item alignment */                \
                      ...)              /* the address of an attached object */ \
                                        /* the address of the code region obj */\
            do {                                                                \
                vsf_pool_cfg_t tCFG = {                                         \
                    .pchPoolName = (const uint8_t *)#__NAME,                    \
                    __VA_ARGS__                                                 \
                };                                                              \
                __NAME##_pool_init_ex((__VSF_POOL), (__ALIGN), &tCFG);          \
            } while(0)

#define VSF_POOL_ADD_BUFFER(__NAME,     /* the name of the pool */              \
                            __VSF_POOL, /* the address of the pool */           \
                            __BUFFER,   /* the address of the buffer */         \
                            __SIZE)     /* the size of the buffer */            \
            __NAME##_pool_add_buffer((__VSF_POOL), (__BUFFER), (__SIZE))

#define VSF_POOL_ADD_BUFFER_EX(__NAME,  /* the name of the pool */              \
                            __VSF_POOL, /* the address of the pool */           \
                            __BUFFER,   /* the address of the buffer */         \
                            __SIZE,     /* the size of the buffer */            \
                            __HANDLER)  /* the block initialisation routine */  \
            __NAME##_pool_add_buffer_ex((__VSF_POOL), (__BUFFER), (__SIZE),     \
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
#define VSF_POOL_GET_TAG(__NAME,        /* the name of the pool */              \
                         __VSF_POOL)    /* the address of the pool */           \
            __NAME##_pool_get_target((__VSF_POOL))

#define VSF_POOL_SET_TAG(__NAME,        /* the name of the pool */              \
                         __VSF_POOL,    /* the address of the pool */           \
                         __TARGET)      /* the address of the target */         \
            __NAME##_pool_set_target((__VSF_POOL), (__TARGET))
#endif
//! @}


/*! \note Enable Statistic mode by default */
#ifndef VSF_POOL_CFG_STATISTIC_MODE
#   define  VSF_POOL_CFG_STATISTIC_MODE     ENABLED
#endif

#ifndef VSF_POOL_CFG_FEED_ON_HEAP
#   define VSF_POOL_CFG_FEED_ON_HEAP        ENABLED
#endif

#ifndef VSF_POOL_CFG_SUPPORT_USER_OBJECT
#   define VSF_POOL_CFG_SUPPORT_USER_OBJECT ENABLED
#endif

/*============================ TYPES =========================================*/

declare_class(vsf_pool_t)

typedef
void
vsf_pool_item_init_evt_handler_t(   void *pTarget, 
                                    void *pItem, 
                                    uint_fast32_t wItemSize);


#if     VSF_POOL_CFG_STATISTIC_MODE == ENABLED                                  \
    ||  VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED 
typedef struct vsf_pool_info_t{
#   if  VSF_POOL_CFG_STATISTIC_MODE == ENABLED
    implement(vsf_slist_node_t)
    const uint8_t *pchPoolName;
#   endif
    uint32_t wItemSize;
    uint16_t u15Align           : 15;
    uint16_t IsNoFeedOnHeap     : 1;
}vsf_pool_info_t;
#endif

//! \name pool
//! @{

def_class(vsf_pool_t,,

    private_member(
        vsf_slist_t tFreeList;       /*!< free list */
        uint16_t hwFree;             /*!< the number of free blocks */
        uint16_t hwUsed;
#if     VSF_POOL_CFG_STATISTIC_MODE == ENABLED                                  \
    ||  VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED 
        implement_ex(vsf_pool_info_t, Statistic);
#endif

#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
        vsf_pool_item_init_evt_handler_t *fnItemInit;
#endif

#if !defined(VSF_POOL_CFG_ATOM_ACCESS)
        code_region_t *ptRegion;     /*!< protection region defined by user */
#endif
    )

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    protected_member(
        void *pTarget;     /*!< you can use it to carry mutex or other target */
    )
#endif
)
end_def_class(vsf_pool_t)
//! @}


typedef struct {
    void *pTarget;
    code_region_t *ptRegion;
    const uint8_t *pchPoolName;
    vsf_pool_item_init_evt_handler_t *fnItemInit;
} vsf_pool_cfg_t;


//! \name vsf pool interface
//! @{
def_interface(i_pool_t)
    void (*Init)            (   vsf_pool_t *pObj, 
                                uint32_t wItemSize, 
                                uint_fast16_t hwAlign, 
                                vsf_pool_cfg_t *ptCFG);
    struct {
        bool (*AddEx)       (vsf_pool_t *pObj,
                             void *pBuffer,
                             uint32_t wBufferSize,
                             uint32_t wItemSize,
                             vsf_pool_item_init_evt_handler_t *fnItemInit);
        bool (*Add)         (vsf_pool_t *pObj,
                             void *pBuffer,
                             uint32_t wBufferSize,
                             uint32_t wItemSize);
    }Buffer;

    void *(*Allocate)       (vsf_pool_t *pObj);
    void (*Free)            (vsf_pool_t *pObj, void *pItem);
    uint_fast16_t  (*Count) (vsf_pool_t *pObj);
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    struct {
        void *(*Get)        (vsf_pool_t *pObj);
        void *(*Set)        (vsf_pool_t *pObj, void *pTarget);
    }Tag;
#endif
end_def_interface(i_pool_t)
//! @}
/*============================ GLOBAL VARIABLES ==============================*/
extern const i_pool_t VSF_POOL;

/*============================ PROTOTYPES ====================================*/

/*! \brief initialise target pool
 *! \param pObj address of the target pool
 *! \param wItemSize memory item size
 *! \param hwAlign Item Alignment
 *! \param ptCFG configurations
 *! \return none
 */
extern void vsf_pool_init(  vsf_pool_t *pObj, 
                            uint32_t wItemSize, 
                            uint_fast16_t hwAlign, 
                            vsf_pool_cfg_t *ptCFG);

/*! \brief add memory to pool
 *! \param pObj             address of the target pool
 *! \param pBuffer          address of the target memory
 *! \param wBufferSize      the size of the target memory
 *! \param wItemSize        memory block size of the pool
 *! \param fnItemInit       block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added
 */
extern bool
vsf_pool_add_buffer_ex( vsf_pool_t *pObj,
                        void *pBuffer,
                        uint32_t wBufferSize,
                        uint32_t wItemSize,
                        vsf_pool_item_init_evt_handler_t *fnItemInit);

/*! \brief add memory to pool
 *! \param pObj        address of the target pool
 *! \param pBuffer      address of the target memory
 *! \param wBufferSize  the size of the target memory
 *! \param wItemSize    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added
 */
extern bool vsf_pool_add_buffer(vsf_pool_t *pObj,
                                void *pBuffer,
                                uint32_t wBufferSize,
                                uint32_t wItemSize);

/*! \brief try to fetch a memory block from the target pool
 *! \param pObj    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
extern void *vsf_pool_alloc(vsf_pool_t *pObj);

/*! \brief return a memory block to the target pool
 *! \param pObj     address of the target pool
 *! \param pItem    target memory block
 *! \return none
 */
extern void vsf_pool_free(vsf_pool_t *pObj, void *pItem);

SECTION("text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param pObj    address of the target pool
 *! \return the number of memory blocks
 */
extern uint_fast16_t vsf_pool_get_count(vsf_pool_t *pObj);


SECTION("text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param pObj    address of the target pool
 *! \return the address of the object
 */
extern void *vsf_pool_get_tag(vsf_pool_t *pObj);

SECTION("text.vsf.utilities.vsf_pool_set_tag")
/*! \brief set the address of the object which is attached to the pool
 *! \param pObj    address of the target pool
 *! \return the address of the object
 */
extern void *vsf_pool_set_tag(vsf_pool_t *pObj, void *pTarget);


SECTION("text.vsf.utilities.vsf_pool_get_region")
/*! \brief get the address of the code region used by this pool
 *! \param pObj    address of the target pool
 *! \return the address of the code region
 */
extern code_region_t *vsf_pool_get_region(vsf_pool_t *pObj);

#endif
