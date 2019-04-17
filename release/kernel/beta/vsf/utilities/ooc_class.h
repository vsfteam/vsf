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
 
//#ifndef __OOC_CLASS_H__           /* deliberately comment this out! */
//#define __OOC_CLASS_H__           /* deliberately comment this out! */

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

#if     defined(__VSF_CLASS_USE_STRICT_TEMPLATE__)
#   include "./ooc_class_strict.h"
#elif   defined(__VSF_CLASS_USE_SIMPLE_TEMPLATE__)
#   include "./ooc_class_simple.h"
#else
#   include "./ooc_class_simple.h"
#endif



/*============================ MACROS ========================================*/
#ifndef this
#   define this         (*ptThis)
#endif
#ifndef base
#   define base         (*ptBase)
#endif
#ifndef target
#   define target       (*ptTarget)
#endif


#undef __VSF_CLASS_USE_STRICT_TEMPLATE__
#undef __VSF_CLASS_USE_SIMPLE_TEMPLATE__
#undef __VSF_CLASS_IMPLEMENT
#undef __VSF_CLASS_INHERIT

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

//#endif                                /* deliberately comment this out! */