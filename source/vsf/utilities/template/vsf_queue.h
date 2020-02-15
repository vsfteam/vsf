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

#ifndef __VSF_RING_BUFFER_H__
#define __VSF_RING_BUFFER_H__

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Declare the ring buffer
    declare_vsf_rng_buf(<ring buffer name>);

    // 2. Defining your ring buffer variable with specific type
    def_vsf_rng_buf(<ring buffer name>, <item type>)

    // 3. Implement your ring buffer with specific atom access protection macro:
    implement_vsf_rng_buf(<ring buffer name>, <item type>, <atom access macro>)
    
    NOTE: You can use __SAFE_ATOM_CODE for interrupt, use __vsf_sched_safe for
          scheduler, use NO_RNG_BUF_PROTECT for no protection

    // 4. Defining your ring buffer variable
    NO_INIT static <ring buffer name> <ring buffer var>;

    // 5. Initialise a ring buffer with specific ring buffer buffer 
    vsf_rng_buf_init( <ring buffer name>, <item type>, <address of ring buffer var>, <item count>);

    // 6. If you want to initialise a ring buffer with a given buffer, use vsf_rng_buf_prepare
          rather than vsf_rng_buf_init above:
    vsf_rng_buf_prepare(  <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of buffer>, 
                        <size of ring buffer buffer> );

    // 7. Use following macro for enqueue, dequeue and peek one item:
    vsf_rng_buf_send(  <ring buffer name>, 
                        <address of ring buffer var>, 
                        <Item>);
    vsf_rng_buf_get(  <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of item buffer>);
    vsf_rng_buf_peek(     <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of item pointer>);

    NOTE: Peek returns a refrence to existing data in the ring buffer, there is no copy access

    // 8. Use following macro for enqueue, dequeue and peek multile items:
    vsf_rng_buf_send(  <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of item buffer>, 
                        <number of Items>);
    vsf_rng_buf_get(  <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of item buffer>, 
                        <number of Items>);
    vsf_rng_buf_peek(     <ring buffer name>, 
                        <address of ring buffer var>, 
                        <address of item pointer>, 
                        <number of Items>);

    NOTE: Peek returns a refrence to existing data in the ring buffer, there is no copy access

    // 9. You can get the number of items within a ring buffer:
    vsf_rng_buf_count( <ring buffer name>, <address of ring buffer var> )

    // 10. You can get the number of peekable items withi a ring buffer
    vsf_rng_buf_peekable_count( <ring buffer name>, <address of ring buffer var> )

    // 11. You can reset the peek access
    vsf_rng_buf_reset_peek( <ring buffer name>, <address of ring buffer var> )

    // 12. You can also remove all peeked items from the queue
    vsf_rng_buf_get_all_peeked( <ring buffer name>, <address of ring buffer var> )

    Example:


    declare_vsf_rng_buf(my_hword_queue_t)
    def_vsf_rng_buf(my_hword_queue_t, uint16_t)

    implement_vsf_rng_buf(my_hword_queue_t, uint16_t, NO_RNG_BUF_PROTECT)


    void vsf_ring_buffer_example(void)
    {
        NO_INIT static my_hword_queue_t s_tQueue;

        static uint16_t s_hwTemp[] = { 0x1234, 0x5678 };
        static uint16_t s_hwItem;

        //NO_INIT static uint8_t s_chQueueBuffer[1024];
        //vsf_rng_buf_prepare(my_hword_queue_t, &s_tQueue, &s_chQueueBuffer, sizeof(s_chQueueBuffer));

        vsf_rng_buf_init(my_hword_queue_t, uint16_t, &s_tQueue, 32);

        vsf_rng_buf_send(my_hword_queue_t, &s_tQueue, 0x1234);


        vsf_rng_buf_send(my_hword_queue_t, &s_tQueue, s_hwTemp, UBOUND(s_hwTemp));

        vsf_rng_buf_get(my_hword_queue_t, &s_tQueue, s_hwTemp, UBOUND(s_hwTemp));
        vsf_rng_buf_get(my_hword_queue_t, &s_tQueue, &s_hwItem);

        const uint16_t* phwItemRef = NULL;
        vsf_rng_buf_peek(my_hword_queue_t, &s_tQueue, &phwItemRef, 2);    
        vsf_rng_buf_peek(my_hword_queue_t, &s_tQueue, &phwItemRef);
    }

 */

/*============================ INCLUDES ======================================*/

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__VSF_QUEUE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_QUEUE_CLASS_IMPLEMENT
#elif   defined(__VSF_QUEUE_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_QUEUE_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/



#define __declare_vsf_rng_buf(__name)                                           \
    typedef struct __name __name;                                               \
    typedef struct __name##_cfg_t __name##_cfg_t;
#define declare_vsf_rng_buf(__name)       __declare_vsf_rng_buf(__name)


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

#   define NO_RNG_BUF_PROTECT(__CODE)               __CODE
#   define no_rng_buf_protect(__code)               __code

#   define __def_vsf_rng_buf(__name, __type)                                    \
    struct __name {                                                             \
        implement(vsf_rng_buf_t)                                                \
        __type *ptBuffer;                                                       \
    };                                                                          \
                                                                                \
    struct __name##_cfg_t {                                                     \
        __type *ptBuffer;                                                       \
        uint16_t hwSize;                                                        \
        bool     bInitAsFull;                                                   \
    };                                                                          \
                                                                                \
extern                                                                          \
void __name##_init(__name* ptQ, __name##_cfg_t* ptCFG);                         \
                                                                                \
extern                                                                          \
bool __name##_send_one(__name* ptQ, __type tItem);                              \
                                                                                \
extern                                                                          \
bool __name##_get_one(__name* ptQ, __type* ptItem);                             \
                                                                                \
SECTION(".text." #__name "_item_count")                                         \
extern                                                                          \
uint_fast16_t __name##_item_count(__name* ptQ);                                 \
                                                                                \
SECTION(".text." #__name "_send_multiple")                                      \
extern                                                                          \
int32_t __name##_send_multiple(__name* ptQ,                                     \
                                        const __type* ptItem,                   \
                                        uint16_t hwCount);                      \
                                                                                \
SECTION(".text." #__name "_get_multiple")                                       \
extern                                                                          \
int32_t __name##_get_multiple(  __name* ptQ,                                    \
                                    __type* ptItem,                             \
                                    uint16_t hwCount);                          \
                                                                                \
SECTION(".text." #__name "_peek_one")                                           \
extern                                                                          \
bool __name##_peek_one(__name* ptQ, const __type** pptItem);                    \
                                                                                \
SECTION(".text." #__name "_reset_peek")                                         \
extern                                                                          \
void __name##_reset_peek(__name *ptQ);                                          \
                                                                                \
SECTION(".text." #__name "_get_all_peeked")                                     \
extern                                                                          \
void __name##_get_all_peeked(__name *ptQ);                                      \
                                                                                \
SECTION(".text." #__name "_item_count_peekable")                                \
extern                                                                          \
uint_fast16_t __name##_item_count_peekable(__name* ptQ);                        \
                                                                                \
SECTION(".text." #__name "_peek_multiple")                                      \
extern                                                                          \
int32_t __name##_peek_multiple( __name* ptQ,                                    \
                                const __type** pptItem,                         \
                                uint16_t hwCount);

#   define def_vsf_rng_buf(__name, __type)                                      \
            __def_vsf_rng_buf(__name, __type) 

#else
#   define NO_RNG_BUF_PROTECT(...)               __VA_ARGS__
#   define no_rng_buf_protect(...)               __VA_ARGS__

#   define __def_vsf_rng_buf(__name, __type, ...)                               \
    struct __name {                                                             \
        implement(vsf_rng_buf_t)                                                \
        __type *ptBuffer;                                                       \
        __VA_ARGS__                                                             \
    };                                                                          \
                                                                                \
    struct __name##_cfg_t {                                                     \
        __type *ptBuffer;                                                       \
        uint16_t hwSize;                                                        \
        bool     bInitAsFull;                                                   \
    };                                                                          \
                                                                                \
extern                                                                          \
void __name##_init(__name* ptQ, __name##_cfg_t* ptCFG);                         \
                                                                                \
extern                                                                          \
bool __name##_send_one(__name* ptQ, __type tItem);                              \
                                                                                \
extern                                                                          \
bool __name##_get_one(__name* ptQ, __type* ptItem);                             \
                                                                                \
SECTION(".text." #__name "_item_count")                                         \
extern                                                                          \
uint_fast16_t __name##_item_count(__name* ptQ);                                 \
                                                                                \
SECTION(".text." #__name "_send_multiple")                                      \
extern                                                                          \
int32_t __name##_send_multiple(__name* ptQ,                                     \
                                        const __type* ptItem,                   \
                                        uint16_t hwCount);                      \
                                                                                \
SECTION(".text." #__name "_get_multiple")                                       \
extern                                                                          \
int32_t __name##_get_multiple(  __name* ptQ,                                    \
                                    __type* ptItem,                             \
                                    uint16_t hwCount);                          \
                                                                                \
SECTION(".text." #__name "_peek_one")                                           \
extern                                                                          \
bool __name##_peek_one(__name* ptQ, const __type** pptItem);                    \
                                                                                \
SECTION(".text." #__name "_reset_peek")                                         \
extern                                                                          \
void __name##_reset_peek(__name *ptQ);                                          \
                                                                                \
SECTION(".text." #__name "_get_all_peeked")                                     \
extern                                                                          \
void __name##_get_all_peeked(__name *ptQ);                                      \
                                                                                \
SECTION(".text." #__name "_item_count_peekable")                                \
extern                                                                          \
uint_fast16_t __name##_item_count_peekable(__name* ptQ);                        \
                                                                                \
SECTION(".text." #__name "_peek_multiple")                                      \
extern                                                                          \
int32_t __name##_peek_multiple( __name* ptQ,                                    \
                                const __type** pptItem,                         \
                                uint16_t hwCount);

#   define def_vsf_rng_buf(__name, __type, ...)                                 \
            __def_vsf_rng_buf(__name, __type, __VA_ARGS__) 

#endif

#define __implement_vsf_rng_buf(__name, __type, __queue_protect)                \
void __name##_init(__name* ptQ, __name##_cfg_t* ptCFG)                          \
{                                                                               \
    ASSERT(NULL != ptQ && NULL != ptCFG);                                       \
    ASSERT(NULL != ptCFG->ptBuffer);                                            \
    ASSERT(ptCFG->hwSize >= sizeof(__type));                                    \
    ptQ->ptBuffer = ptCFG->ptBuffer;                                            \
    __vsf_rng_buf_init_ex(&(ptQ->use_as__vsf_rng_buf_t),                        \
        ptCFG->hwSize / sizeof(__type),                                         \
        ptCFG->bInitAsFull);                                                    \
}                                                                               \
                                                                                \
bool __name##_send_one(__name *ptQ, __type tItem)                               \
{                                                                               \
    bool bResult = false;                                                       \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            int32_t nIndex =                                                    \
                __vsf_rng_buf_send_one(&(ptQ->use_as__vsf_rng_buf_t));          \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
            ptQ->ptBuffer[nIndex] = tItem;                                      \
            bResult = true;                                                     \
        } while(0);                                                             \
    )                                                                           \
                                                                                \
    return bResult;                                                             \
}                                                                               \
                                                                                \
bool __name##_get_one(__name * ptQ, __type *ptItem)                             \
{                                                                               \
    bool bResult = false;                                                       \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            int32_t nIndex =                                                    \
                __vsf_rng_buf_get_one(&(ptQ->use_as__vsf_rng_buf_t));           \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
            if (NULL != ptItem) {                                               \
                *ptItem = ptQ->ptBuffer[nIndex];                                \
            }                                                                   \
            bResult = true;                                                     \
        } while (0);                                                            \
    )                                                                           \
                                                                                \
    return bResult;                                                             \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_item_count")                                         \
uint_fast16_t __name##_item_count(__name * ptQ)                                 \
{                                                                               \
    ASSERT(NULL != ptQ);                                                        \
    return __vsf_rng_buf_item_count(&(ptQ->use_as__vsf_rng_buf_t));             \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_send_multiple")                                      \
int32_t __name##_send_multiple(  __name * ptQ,                                  \
                                    const __type*ptItem,                        \
                                    uint16_t hwCount)                           \
{                                                                               \
    int32_t nResult = -1, nIndex = 0;                                           \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            if (NULL == ptItem || 0 == hwCount) {                               \
                break;                                                          \
            }                                                                   \
            nIndex =                                                            \
                __vsf_rng_buf_send_multiple(   &(ptQ->use_as__vsf_rng_buf_t),   \
                                                &hwCount);                      \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
                                                                                \
            memcpy(&(ptQ->ptBuffer[nIndex]), ptItem, hwCount * sizeof(__type)); \
            nResult = hwCount;                                                  \
        } while(0);                                                             \
    )                                                                           \
    return nResult;                                                             \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_get_multiple")                                       \
int32_t __name##_get_multiple(  __name * ptQ,                                   \
                                    __type* ptItem,                             \
                                    uint16_t hwCount)                           \
{                                                                               \
    int32_t nResult = -1, nIndex = 0;                                           \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            if (NULL == ptItem || 0 == hwCount) {                               \
                break;                                                          \
            }                                                                   \
            nIndex =                                                            \
                __vsf_rng_buf_get_multiple(   &(ptQ->use_as__vsf_rng_buf_t),    \
                                                &hwCount);                      \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
                                                                                \
            memcpy(ptItem, &(ptQ->ptBuffer[nIndex]), hwCount * sizeof(__type)); \
            nResult = hwCount;                                                  \
        } while(0);                                                             \
    )                                                                           \
    return nResult;                                                             \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_peek_one")                                           \
bool __name##_peek_one(__name *ptQ, const __type** pptItem)                     \
{                                                                               \
    bool bResult = false;                                                       \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            int32_t nIndex = __vsf_rng_buf_peek_one(                            \
                                &(ptQ->use_as__vsf_rng_buf_t));                 \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
            if (NULL != pptItem) {                                              \
                (*pptItem) = (const __type*)&ptQ->ptBuffer[nIndex];             \
            }                                                                   \
            bResult = true;                                                     \
        } while (0);                                                            \
    )                                                                           \
                                                                                \
    return bResult;                                                             \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_reset_peek")                                         \
void __name##_reset_peek(__name *ptQ)                                           \
{                                                                               \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        __vsf_rng_buf_reset_peek(&(ptQ->use_as__vsf_rng_buf_t));                \
    )                                                                           \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_get_all_peeked")                                     \
void __name##_get_all_peeked(__name *ptQ)                                       \
{                                                                               \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        __vsf_rng_buf_get_all_peeked(&(ptQ->use_as__vsf_rng_buf_t));            \
    )                                                                           \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_item_count_peekable")                                \
uint_fast16_t __name##_item_count_peekable(__name *ptQ)                         \
{                                                                               \
    ASSERT(NULL != ptQ);                                                        \
    return __vsf_rng_buf_item_count_peekable(&ptQ->use_as__vsf_rng_buf_t);      \
}                                                                               \
                                                                                \
SECTION(".text." #__name "_peek_multiple")                                      \
int32_t __name##_peek_multiple( __name * ptQ,                                   \
                                const __type** pptItem,                         \
                                uint16_t hwCount)                               \
{                                                                               \
    int32_t nResult = -1, nIndex = 0;                                           \
    ASSERT(NULL != ptQ);                                                        \
    __queue_protect(                                                            \
        do {                                                                    \
            if (NULL == pptItem || 0 == hwCount) {                              \
                break;                                                          \
            }                                                                   \
            nIndex =                                                            \
                __vsf_rng_buf_peek_multiple(  &(ptQ->use_as__vsf_rng_buf_t),    \
                                            &hwCount);                          \
            if (nIndex < 0) {                                                   \
                break;                                                          \
            }                                                                   \
                                                                                \
/*memcpy(ptItem, &(ptQ->ptBuffer[nIndex]), hwCount * sizeof(__type));*/         \
            (*pptItem) = (const __type*)&(ptQ->ptBuffer[nIndex]);               \
            nResult = hwCount;                                                  \
        } while(0);                                                             \
    )                                                                           \
    return nResult;                                                             \
}


#define implement_vsf_rng_buf(__name, __type, __queue_protect)                  \
            __implement_vsf_rng_buf(__name, __type, __queue_protect) 


#define __vsf_rng_buf_init(__name, __type, __qaddr, __item_count)               \
    do {                                                                        \
        NO_INIT static uint16_t s_hwBuffer[(__item_count)];                     \
        __name##_cfg_t tCFG = {                                                 \
            s_hwBuffer,                                                         \
            sizeof(s_hwBuffer),                                                 \
        };                                                                      \
        __name##_init((__qaddr), & tCFG);                                       \
    } while(0)

#define vsf_rng_buf_init(__name, __type, __qaddr, __item_count)                 \
        __vsf_rng_buf_init(__name, __type, (__qaddr), (__item_count))



#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L

#define __vsf_rng_buf_prepare(__name, __qaddr, __buffer, __size)                \
    do {                                                                        \
        __name##_cfg_t tCFG = {0};                                              \
        tCFG.ptBuffer = (__buffer);                                             \
        tCFG.hwSize = (__size);                                                 \
        __name##_init((__qaddr), & tCFG);                                       \
    } while(0)

#   define vsf_rng_buf_prepare(__name, __qaddr, __buffer, __size)               \
                __vsf_rng_buf_prepare(__name, (__qaddr), (__buffer), (__size))
#else

#   define __vsf_rng_buf_prepare(__name, __qaddr, __buffer, __size, ...)        \
    do {                                                                        \
        __name##_cfg_t tCFG = {                                                 \
            (__buffer),                                                         \
            (__size),                                                           \
            __VA_ARGS__                                                         \
        };                                                                      \
        __name##_init((__qaddr), & tCFG);                                       \
    } while(0)

#   define vsf_rng_buf_prepare(__name, __qaddr, __buffer, __size, ...)          \
        __vsf_rng_buf_prepare(__name, (__qaddr), (__buffer), (__size), __VA_ARGS__)
#endif

#define __vsf_rng_buf_send_1(__name, __qaddr, __item)                           \
            __name##_send_one((__qaddr), __item)

#define __vsf_rng_buf_send_2(__name, __qaddr, __buffer, __size)                 \
            __name##_send_multiple((__qaddr), (__buffer), (__size))

#define __vsf_rng_buf_get_1(__name, __qaddr, __item)                            \
            __name##_get_one((__qaddr), __item)

#define __vsf_rng_buf_get_2(__name, __qaddr, __buffer, __size)                  \
            __name##_get_multiple((__qaddr), (__buffer), (__size))

#define __vsf_rng_buf_peek_1(__name, __qaddr, __item)                           \
            __name##_peek_one((__qaddr), __item)

#define __vsf_rng_buf_peek_2(__name, __qaddr, __buffer, __size)                 \
            __name##_peek_multiple((__qaddr), (__buffer), (__size))

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#   define vsf_rng_buf_send(__name, __qaddr, ...)                               \
            __PLOOC_EVAL(__VSF_RNG_BUF_SEND_, __VA_ARGS__)                      \
                (__name, __qaddr, __VA_ARGS__)

#   define vsf_rng_buf_get(__name, __qaddr, ...)                                \
            __PLOOC_EVAL(__vsf_rng_buf_get_, __VA_ARGS__)                       \
                (__name, __qaddr, __VA_ARGS__)

#   define vsf_rng_buf_peek(__name, __qaddr, ...)                               \
            __PLOOC_EVAL(__VSF_QUEUE_PEEK_, __VA_ARGS__)                        \
                (__name, __qaddr, __VA_ARGS__)
#endif

#define vsf_rng_buf_send_one(__name, __qaddr, __item)                           \
            __vsf_rng_buf_send_1(__name, (__qaddr), (__item))
            
#define vsf_rng_buf_send_buf(__name, __qaddr, __buffer, __size)                 \
            __vsf_rng_buf_send_2(__name, (__qaddr), (__buffer), (__size))
            
#define vsf_rng_buf_get_one(__name, __qaddr, __item)                            \
            __vsf_rng_buf_get_1(__name, (__qaddr), (__item))
            
#define vsf_rng_buf_get_buf(__name, __qaddr, __buffer, __size)                  \
            __vsf_rng_buf_get_2(__name, (__qaddr), (__buffer), (__size))

#define __vsf_rng_buf_count(__name, __qaddr)                                    \
            __name##_item_count((__qaddr))
#define vsf_rng_buf_count(__name, __qaddr)                                      \
            __vsf_rng_buf_count(__name, __qaddr)

#define ____vsf_rng_buf_reset_peek(__name, __qaddr)                             \
            __name##_reset_peek((__qaddr))
#define vsf_rng_buf_reset_peek(__name, __qaddr)                                 \
            ____vsf_rng_buf_reset_peek(__name, __qaddr)

#define ____vsf_rng_buf_get_all_peeked(__name, __qaddr)                         \
            __name##_get_all_peeked((__qaddr))
#define vsf_rng_buf_get_all_peeked(__name, __qaddr)                             \
            ____vsf_rng_buf_get_all_peeked(__name, __qaddr)

#define __vsf_rng_buf_peekable_count(__name, __qaddr)                           \
            __name##_item_count_peekable((__qaddr))
#define vsf_rng_buf_peekable_count(__name, __qaddr)                             \
            __vsf_rng_buf_peekable_count(__name, __qaddr)

/*============================ TYPES =========================================*/

declare_class(vsf_rng_buf_t)

def_class(vsf_rng_buf_t,
    
    protected_member(
        uint16_t hwBufferItemCount;
        uint16_t hwTail;
        uint16_t hwLength;
        uint16_t hwHead;
        uint16_t hwPeekCount;
        uint16_t hwPeek;
    )
)
end_def_class(vsf_rng_buf_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern 
void __vsf_rng_buf_init_ex(  vsf_rng_buf_t* ptObj, 
                        uint_fast16_t hwBufferItemCount, 
                        bool bInitAsFull);

extern 
int32_t __vsf_rng_buf_send_one(vsf_rng_buf_t* ptObj);

extern 
int32_t __vsf_rng_buf_get_one(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_send_multiple")
extern 
int32_t __vsf_rng_buf_send_multiple(vsf_rng_buf_t* ptObj, uint16_t* phwItemCount);

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_multiple")
extern 
int32_t __vsf_rng_buf_get_multiple(vsf_rng_buf_t* ptObj, uint16_t* phwItemCount);

SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count")
extern 
uint_fast16_t __vsf_rng_buf_item_count(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_one")
extern 
int32_t __vsf_rng_buf_peek_one(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_all_peeked")
extern 
void __vsf_rng_buf_get_all_peeked(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_reset_peek")
extern
void __vsf_rng_buf_reset_peek(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count_peekable")
extern
uint_fast16_t __vsf_rng_buf_item_count_peekable(vsf_rng_buf_t* ptObj);

SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_multiple")
extern
int32_t __vsf_rng_buf_peek_multiple(vsf_rng_buf_t* ptObj, uint16_t* phwItemCount);
#endif
