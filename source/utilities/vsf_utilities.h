/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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


/*! \note Top Level Configuration */

/*============================ MACROS ========================================*/
/*============================ INCLUDES ======================================*/
/* do not modify this */
#include "./vsf_utilities_cfg.h"

/* minimal OO support for interface definie only, no class support */
#include "./3rd-party/PLOOC/raw/plooc.h"

/* compiler abstraction, supports GCC, IAR, Arm Compiler 5, Arm Compiler 6 */
#include "./compiler/compiler.h"

/* template for abstraction data type */
#include "./template/template.h"

/* other high level language externsion for OOPC */
#include "./language_extension/language_extension.h"

#ifndef __VSF_UTILITIES_H__
#define __VSF_UTILITIES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(__cplusplus)
typedef struct vsf_mem_t vsf_mem_t;
struct vsf_mem_t {
    union {
        // implement linux-style variable
        uint8_t *buffer;
        uint8_t *src;
        void *obj;

        uint8_t *buffer_ptr;
        uint8_t *src_ptr;
        void *obj_ptr;
    } ptr;
    int32_t size;
};
#else
typedef struct vsf_mem_t vsf_mem_t;
struct vsf_mem_t {
    union {
        union {
            // implement linux-style variable
            uint8_t *buffer;
            uint8_t *src;
            void *obj;

            uint8_t *buffer_ptr;
            uint8_t *src_ptr;
            void *obj_ptr;
        };
        union {
            uint8_t *buffer_ptr;
            uint8_t *src_ptr;
            void *obj_ptr;
        } ptr;
    };
    union {
        // implement linux-style variable
        int32_t size;
        int32_t s32_size;
    };
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */