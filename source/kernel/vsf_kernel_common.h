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

#ifndef __VSF_KERNEL_COMMON_H__
#define __VSF_KERNEL_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "./vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_SWI_PRIO_INIT(__index, __unused)                                  \
    vsf_prio_##__index,

/*============================ TYPES =========================================*/

#define MFUNC_IN_U8_DEC_VALUE                    VSF_OS_CFG_PRIORITY_NUM
#   include "utilities/preprocessor/mf_u8_dec2str.h"

/**
 * \~english
 * @brief Kernel priority enumeration.
 *
 * Defines the priority levels used by the kernel scheduler and event queues.
 * According to VSF_OS_CFG_PRIORITY_NUM (N), the concrete values vsf_prio_0 ..
 * vsf_prio_{N-1} are generated; vsf_prio_highest is a separate alias equal to
 * the last generated value (so the enum contains a duplicate value).
 *
 * \~chinese
 * @brief 内核优先级枚举。
 *
 * 定义内核调度器和事件队列使用的优先级级别。
 * 根据 VSF_OS_CFG_PRIORITY_NUM(N)，具体的优先级值生成为
 * vsf_prio_0 .. vsf_prio_{N-1}；vsf_prio_highest 是单独定义的别名，
 * 与最后一个生成的值相同（枚举中存在重复值）。
 */
typedef enum vsf_prio_t {
    vsf_prio_inherit                    = -1,                       //!< \~english @brief Inherit the priority of the current event queue. \~chinese @brief 继承当前事件队列的优先级。
    VSF_MREPEAT(MFUNC_OUT_DEC_STR, __VSF_SWI_PRIO_INIT, NULL)
    vsf_prio_highest                    = MFUNC_OUT_DEC_STR - 1,    //!< \~english @brief Highest valid priority level. \~chinese @brief 最高有效优先级级别。
} vsf_prio_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
