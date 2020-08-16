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

#ifndef __CODE_REGION_H__
#define __CODE_REGION_H__

/*============================ INCLUDES ======================================*/
#include "../compiler/compiler.h"

/*! \brief How To Define and Use your own CODE_REGION
 *!        Example:

    static void __code_region_example_on_enter(void *obj_ptr, void *local_ptr)
    {
        printf("-------enter-------\r\n");
    }

    static void __code_region_example_on_leave(void *obj_ptr,void *local_ptr)
    {
        printf("-------leave-------\r\n");
    }

    const static i_code_region_t __example_code_region = {
        .OnEnter = __code_region_example_on_enter,
        .OnLeave = __code_region_example_on_leave,
    };


    void main(void)
    {
        ...
        code_region(&__example_code_region, NULL){
            printf("\tbody\r\n");
        }
        ...
    }

Output:

-------enter-------
        body
-------leave-------


 *! \note How to use code_region()
 *!       Syntax:
 *!             code_region(<Address of i_code_region_t obj>, <Object Address>) {
 *!                 //! put your code here
 *!             }
 *!
 *! \note <Address of i_code_region_t obj>: this can be NULL, if so, 
 *!         DEFAULT_CODE_REGION_NONE will be used.
 *! 
 *! \note <Object Address>: it is the address of the object you want to pass to 
 *!         your OnEnter and OnLeave functions. It can be NULL
 *!
 *! \note A local object will be generated from users' stack, the size is specified
 *!         by i_code_region_t.chLocalSize. The address of this local object will
 *!         be passed to your OnEnter and OnLeave functions. You can use it to
 *!         store some local status.
 *! 
 *! \name   List of Default Code Regions
 *! @{
 *!         DEFAULT_CODE_REGION_ATOM_CODE           //!< interrupt-safe region
 *!         DEFAULT_CODE_REGION_NONE                //!< do nothing
 *! @}
 */


#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#ifndef COMPILER_PATCH_CODE_REGION_LOCAL_SIZE
#   define COMPILER_PATCH_CODE_REGION_LOCAL_SIZE     4
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if __IS_COMPILER_IAR__
#   define __CODE_REGION(__REGION_ADDR)                                         \
    for(code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);         \
        NULL != code_region_ptr;                                                   \
        code_region_ptr = NULL)                                                    \
        for(uint8_t local[COMPILER_PATCH_CODE_REGION_LOCAL_SIZE],               \
                __CONNECT2(__code_region_, __LINE__) = 1;                          \
            __CONNECT2(__code_region_, __LINE__)-- ?                               \
                (code_region_ptr->methods_ptr->OnEnter(  code_region_ptr->target_ptr, local)\
                    ,1)                                                         \
                : 0;                                                            \
            code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local))

#   define __CODE_REGION_START(__REGION_ADDR)   __CODE_REGION(__REGION_ADDR) {
#   define __CODE_REGION_END()                  }

#   define __CODE_REGION_SIMPLE(__REGION_ADDR, ...)                             \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);         \
        uint8_t local[COMPILER_PATCH_CODE_REGION_LOCAL_SIZE];                   \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local);          \
        __VA_ARGS__;                                                            \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local);          \
    } } while(0);

#   define __CODE_REGION_SIMPLE_START(__REGION_ADDR, ...)                       \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);         \
        uint8_t local[COMPILER_PATCH_CODE_REGION_LOCAL_SIZE];                   \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local);          

#   define __CODE_REGION_SIMPLE_END(__REGION_ADDR, ...)                         \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local);          \
    } } while(0);


#   define EXIT_CODE_REGION()                                                   \
            code_region_ptr->ptMethods->OnLeave(code_region_ptr->target_ptr, local)
#   define exit_code_region()  EXIT_CODE_REGION()

#   define CODE_REGION(__REGION_ADDR)          __CODE_REGION((__REGION_ADDR))
#   define code_region(__region_addr)          __CODE_REGION((__region_addr))

#else


#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define __CODE_REGION_SIMPLE(__REGION_ADDR, __CODE)                      \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);      \
        uint8_t local[COMPILER_PATCH_CODE_REGION_LOCAL_SIZE];                   \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local); \
        __CODE;                                                                 \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local); \
    } }while(0);

#       define __CODE_REGION_SIMPLE_START(__REGION_ADDR)                        \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);      \
        uint8_t local[COMPILER_PATCH_CODE_REGION_LOCAL_SIZE];                   \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local);          

#       define __CODE_REGION_SIMPLE_END(__REGION_ADDR)                          \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local); \
    } } while(0);
#   else

/* code region require C99 and above */
#       define __CODE_REGION(__REGION_ADDR)                                     \
    for(code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);      \
        NULL != code_region_ptr;                                                \
        code_region_ptr = NULL)                                                 \
        for(uint8_t local[code_region_ptr->methods_ptr->local_obj_size],        \
                __CONNECT2(__code_region_, __LINE__) = 1;                          \
            __CONNECT2(__code_region_, __LINE__)-- ?                               \
                (code_region_ptr->methods_ptr->OnEnter(                         \
                    code_region_ptr->target_ptr, local)                            \
                    ,1)                                                         \
                : 0;                                                            \
            code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local))

#       define __CODE_REGION_START(__REGION_ADDR) __CODE_REGION(__REGION_ADDR) {
#       define __CODE_REGION_END()                  }


#       define __CODE_REGION_SIMPLE(__REGION_ADDR, ...)                         \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);      \
        uint8_t local[code_region_ptr->methods_ptr->local_obj_size];            \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local); \
        __VA_ARGS__;                                                            \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local); \
    } }while(0);
    
#       define __CODE_REGION_SIMPLE_START(__REGION_ADDR, ...)                   \
    do {if (NULL != (__REGION_ADDR)) {                                          \
        code_region_t *code_region_ptr = (code_region_t *)(__REGION_ADDR);      \
        uint8_t local[code_region_ptr->methods_ptr->local_obj_size];            \
        code_region_ptr->methods_ptr->OnEnter(code_region_ptr->target_ptr, local);          

#       define __CODE_REGION_SIMPLE_END(__REGION_ADDR, ...)                     \
        code_region_ptr->methods_ptr->OnLeave(code_region_ptr->target_ptr, local); \
    } } while(0);
#   endif

#   define EXIT_CODE_REGION()                                                   \
            code_region_ptr->ptMethods->OnLeave(code_region_ptr->target_ptr, local)
#   define exit_code_region()  EXIT_CODE_REGION()

#   define CODE_REGION(__REGION_ADDR)          __CODE_REGION((__REGION_ADDR))
#   define code_region(__region_addr)          __CODE_REGION((__region_addr))

#endif


#define CODE_REGION_START(__REGION_ADDR)    __CODE_REGION_START((__REGION_ADDR))
#define CODE_REGION_END()                   __CODE_REGION_END()

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define CODE_REGION_SIMPLE(__REGION_ADDR, __CODE)                               \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __CODE)
#define code_region_simple(__REGION_ADDR, __CODE)                               \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __CODE)
#else
#define CODE_REGION_SIMPLE(__REGION_ADDR, ...)                                  \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __VA_ARGS__)
#define code_region_simple(__REGION_ADDR, ...)                                  \
            __CODE_REGION_SIMPLE((__REGION_ADDR), __VA_ARGS__)
#endif
    
#define CODE_REGION_SIMPLE_START(__REGION_ADDR)                                 \
            __CODE_REGION_SIMPLE_START((__REGION_ADDR))
#define CODE_REGION_SIMPLE_END()                                                \
            __CODE_REGION_SIMPLE_END()



/*============================ TYPES =========================================*/
typedef struct {
    uint_fast8_t    local_obj_size;
    void (*OnEnter)(void *obj_ptr, void *local_ptr);
    void (*OnLeave)(void *obj_ptr, void *local_ptr);
}i_code_region_t;

typedef struct {
    void *target_ptr;
    i_code_region_t *methods_ptr;
} code_region_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern const code_region_t DEFAULT_CODE_REGION_NONE;
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
