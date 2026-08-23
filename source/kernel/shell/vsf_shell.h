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

/**
 * @file
 * \~english
 * @brief VSF kernel shell aggregation header.
 *
 * This header exposes the simple kernel shell APIs by including the
 * corresponding sub-module headers. It is the intended entry point for
 * applications that use the VSF kernel shell layer.
 *
 * \~chinese
 * @brief VSF 内核 shell 聚合头文件。
 *
 * 此头文件通过包含对应的子模块头文件来暴露简单的内核 shell API。
 * 它是使用 VSF 内核 shell 层的应用程序的推荐入口点。
 */
#ifndef __VSF_KERNEL_SHELL_H__
#define __VSF_KERNEL_SHELL_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_kernel_cfg.h"

/**
 * \~english
 * @brief Include the simple kernel shell API.
 * \~chinese
 * @brief 包含简单的内核 shell API。
 */
#include "./simple/vsf_simple.h"

/**
 * \~english
 * @brief POSIX-compatible kernel shell API (currently disabled).
 * \~chinese
 * @brief POSIX 兼容的内核 shell API（当前未启用）。
 */
//#include "./posix/vsf_posix.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
