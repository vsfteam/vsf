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


/******************************************************************************
 * IMPORTANT!!!!!!                                                            *
 * This Header File Could only be used in target module directly. E.g.        *
 ******************************************************************************

//!Add following content to your module header file, e.g. xxxxxx.h

#include "utilities\ooc_class.h"                                            
//! Include module common class definition header file                      
#include ".\__class_xxxxxx.h"       //! xxxxx is the class or module name  


//! Add following content to your module source file, e.g. xxxxx.c

#include "xxxxx.h"                  //! xxxxx.h is the module header file
#define __VSF_CLASS_IMPLEMENT                                          
#include ".\__class_xxxxxx.h"       //! xxxxx is the class or module name  


//! Add following content to the source file of the module which inherit the class, 
//! e.g. xxxxx.c
#include "xxxxx.h"                  //! xxxxx.h is the module header file
#define __VSF_CLASS_INHERIT                                          
#include ".\__class_xxxxxx.h"       //! xxxxx is the class or module name  

 */



/*============================ INCLUDES ======================================*/
#include <stdint.h>

/*! \NOTE the uint_fast8_t used in this header file is defined in stdint.h 
          if you don't have stdint.h supported in your toolchain, you should
          define uint_fast8_t all by yourself with following rule:
          a. if the target processor is 8 bits, define it as uint8_t
          b. if the target processor is 16 bits, define it as uint16_t 
          c. if the target processor is 32 bits, define it as uint32_t
          d. if the target processor is 64 bits, define it as either uint32_t or uint64_t
 */

#include "./preprocessor/mrepeat.h"


/*============================ MACROS ========================================*/
#undef private_member
#undef protected_member
#undef public_member
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef __OOC_CLASS_SIMPLE_H__
#define __OOC_CLASS_SIMPLE_H__

#   define __def_simple_class(__NAME)       struct __NAME
#   define def_simple_class(__NAME)         __def_simple_class(__NAME)
#   define declare_simple_class(__NAME)     typedef struct __NAME __NAME;

#   define __visible_member(...)                                                \
        struct{                                                                 \
            __VA_ARGS__                                                         \
        };

#   if  !defined (__OOC_CLASS_USE_NO_STRUCT_MASK__)
#       define __invisible_member(...)                                          \
            uint8_t ATPASTE2(zzz_, __LINE__)[sizeof(struct{                     \
                __VA_ARGS__                                                     \
            })];
#   else
#       define __invisible_member(...)                                          \
            struct{                                                             \
                __VA_ARGS__                                                     \
            } ATPASTE2(zzz_, __LINE__);
#   endif   /* __ooc_class_i_dont_like_mask_struct */
#endif  /* __OOC_CLASS_SIMPLE_H__ */

#if     defined(__OOC_DEBUG__)

#   define private_member(...)              __VA_ARGS__
#   define protected_member(...)            __VA_ARGS__
#   define public_member(...)               __VA_ARGS__

#elif   defined(__VSF_CLASS_IMPLEMENT)

#   define private_member(...)              __visible_member(__VA_ARGS__)
#   define protected_member(...)            __visible_member(__VA_ARGS__)
#   define public_member(...)               __visible_member(__VA_ARGS__)

#elif   defined(__VSF_CLASS_INHERIT)

#   define private_member(...)              __invisible_member(__VA_ARGS__)
#   define protected_member(...)            __visible_member(__VA_ARGS__)
#   define public_member(...)               __visible_member(__VA_ARGS__)

#else   /* __VSF_CLASS_EXTERN */

#   define private_member(...)              __invisible_member(__VA_ARGS__)
#   define protected_member(...)            __invisible_member(__VA_ARGS__)
#   define public_member(...)               __visible_member(__VA_ARGS__)

#endif

// code below is just try to be compatible with ooc_class_strict
#undef def_class
#undef __def_class
#undef end_def_class
#undef __end_def_class

#define __def_class(__NAME, ...)                                                \
    struct __NAME {                                                             \
        __VA_ARGS__
          
#define __end_def_class(__NAME, ...)                                            \
    };

#if     defined(__VSF_CLASS_IMPLEMENT)

#   undef  __class
#   define __class(__NAME)                  __NAME

#   undef  class
#   define class(__NAME)                    __class(__NAME)

#   undef __class_internal
#   define __class_internal(__SRC, __DES, __TYPE, ...)                          \
            class(__TYPE) *(__DES) = (class(__TYPE) *)(__SRC);                  \
            __with_class(__TYPE, (__SRC), __VA_ARGS__)
            
#   undef class_internal
#   define class_internal(__SRC, __DES, __TYPE,...)                             \
            __class_internal(__SRC, __DES, __TYPE, __VA_ARGS__)
            
#   undef __with_class
#   define __with_class(__TYPE, __SRC, ...)                                     \
        {                                                                       \
            class(__TYPE)*_ =(class(__TYPE) *)(__SRC);                          \
            UNUSED_PARAM(_);                                                    \
            __VA_ARGS__;                                                        \
        }                                                                       \
        for (class(__TYPE)*_ =(class(__TYPE) *)(__SRC); NULL != _; _ = NULL)

#   undef with_class
#   define with_class(__TYPE, __SRC, ...)                                       \
            __with_class(__TYPE, __SRC, __VA_ARGS__)

#elif   defined(__OOC_DEBUG__) && defined(__VSF_CLASS_INHERIT)

#   undef  __class_protected
#   define __class_protected(__NAME)        __NAME

#   undef  class_protected
#   define class_protected(__NAME)          __class_protected(__NAME)

#   undef __protected_internal
#   define __protected_internal(__SRC, __DES, __TYPE,...)                       \
            class_protected(__TYPE)*(__DES)=(class_protected(__TYPE) *)(__SRC); \
            __with_protected(__TYPE, (__SRC), __VA_ARGS__)

#   undef protected_internal            
#   define protected_internal(__SRC, __DES, __TYPE,...)                         \
            __protected_internal(__SRC, __DES, __TYPE, __VA_ARGS__)    

#   undef __with_protected
#   define __with_protected(__TYPE, __SRC, ...)                                 \
        {                                                                       \
            class_protected(__TYPE)*_ =(class_protected(__TYPE) *)(__SRC);      \
            __VA_ARGS__;                                                        \
        }                                                                       \
        for (   class_protected(__TYPE)*_ =(class_protected(__TYPE) *)(__SRC);  \
                NULL != _;                                                      \
                _ = NULL)

#   undef with_protected
#   define with_protected(__TYPE, __SRC, ...)                                   \
            __with_protected(__TYPE, __SRC, __VA_ARGS__)
        
#elif   defined(__VSF_CLASS_INHERIT)

#   undef  __class_protected
#   define __class_protected(__NAME)            __NAME

#   undef  class_protected
#   define class_protected(__NAME)              __class_protected(__NAME)

#   undef __protected_internal
#   define __protected_internal(__SRC, __DES, __TYPE)                           \
            class_protected(__TYPE) *(__DES) = (class_protected(__TYPE) *)(__SRC) 

#   undef protected_internal            
#   define protected_internal(__SRC, __DES, __TYPE)                             \
            __protected_internal(__SRC, __DES, __TYPE)    

#   undef __with_protected
#   define __with_protected(__TYPE, __SRC, ...)                                 \
        {                                                                       \
            class_protected(__TYPE)*_ =(class_protected(__TYPE) *)(__SRC);      \
            __VA_ARGS__;                                                        \
        }

#   undef with_protected
#   define with_protected(__TYPE, __SRC, ...)                                   \
            __with_protected(__TYPE, __SRC, __VA_ARGS__)            

#elif defined(__OOC_DEBUG__)

#else  /* __VSF_CLASS_EXTERN */

#endif 




#define def_class(__NAME, ...)          __def_class(__NAME, __VA_ARGS__)
#define end_def_class(__NAME, ...)      __end_def_class(__NAME, __VA_ARGS__)

#undef declare_class
#define declare_class(__NAME)           typedef struct __NAME __NAME;


#undef __VSF_CLASS_IMPLEMENT
#undef __VSF_CLASS_INHERIT
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/