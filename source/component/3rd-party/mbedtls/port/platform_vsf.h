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

#ifndef __MBEDTLS_PLATFORM_VSF_H__
#define __MBEDTLS_PLATFORM_VSF_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED

#include "vsf.h"

/*============================ MACROS ========================================*/

#ifndef MBEDTLS_PLATFORM_C
#   define MBEDTLS_PLATFORM_C
#endif

#ifndef MBEDTLS_PLATFORM_MEMORY
#   define MBEDTLS_PLATFORM_MEMORY
#endif

#undef MBEDTLS_PLATFORM_CALLOC_MACRO
#define MBEDTLS_PLATFORM_CALLOC_MACRO           __vsf_mbedtls_calloc

#undef MBEDTLS_PLATFORM_FREE_MACRO
#define MBEDTLS_PLATFORM_FREE_MACRO             __vsf_mbedtls_free

#undef MBEDTLS_PLATFORM_EXIT_MACRO
#define MBEDTLS_PLATFORM_EXIT_MACRO             __vsf_mbedtls_exit
#undef MBEDTLS_PLATFORM_STD_EXIT_SUCCESS
#define MBEDTLS_PLATFORM_STD_EXIT_SUCCESS       VSF_ERR_NONE
#undef MBEDTLS_PLATFORM_STD_EXIT_FAILURE
#define MBEDTLS_PLATFORM_STD_EXIT_FAILURE       VSF_ERR_FAIL

#undef MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_PLATFORM_ENTROPY
#if VSF_HW_RNG_COUNT > 0
#   define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void * __vsf_mbedtls_calloc(size_t n, size_t size);
extern void __vsf_mbedtls_free(void *ptr);

extern void __vsf_mbedtls_exit(int status);

#endif      // VSF_USE_MBEDTLS
#endif      // __MBEDTLS_PLATFORM_VSF_H__
