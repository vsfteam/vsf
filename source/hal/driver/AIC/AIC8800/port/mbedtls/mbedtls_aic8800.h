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

#ifndef __MBEDTLS_AIC8800_H__
#define __MBEDTLS_AIC8800_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_MBEDTLS == ENABLED && VSF_AIC8800_USE_MBEDTLS_ACCELERATOR == ENABLED

/*============================ MACROS ========================================*/

#ifdef CONFIG_RWNX
//#   define MBEDTLS_AES_SETKEY_ENC_ALT
//#   define MBEDTLS_AES_SETKEY_DEC_ALT
//#   define MBEDTLS_AES_ENCRYPT_ALT
//#   define MBEDTLS_AES_DECRYPT_ALT
//#   define MBEDTLS_ECC_MULCPMB_ALT

//#   define MBEDTLS_AES_ENCRYPT_ECB_ALT
//#   define MBEDTLS_AES_ENCRYPT_CBC_ALT
//#   define MBEDTLS_AES_ENCRYPT_GCM_ALT
//#   define MBEDTLS_SHA256_COMPUTE_ALT
//#   define MBEDTLS_RSA_EXPMOD_ALT
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_MBEDTLS
#endif      // __MBEDTLS_AIC8800_H__
