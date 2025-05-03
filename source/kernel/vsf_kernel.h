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

#ifndef __VSF_KERNEL_H__
#define __VSF_KERNEL_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_eda_slist_queue.h"
#include "./vsf_os.h"
#include "./shell/vsf_shell.h"
#include "./task/vsf_task.h"
#include "./task/vsf_thread.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if (VSF_USE_APPLET == ENABLED || VSF_LINUX_USE_APPLET == ENABLED) && VSF_APPLET_USE_KERNEL == ENABLED
// TODO: add more kernel APIs
typedef struct vsf_kernel_vplt_t {
    vsf_vplt_info_t info;

    // eda
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_eda_get_cur);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_eda_post_evt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_eda_post_msg);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_eda_post_evt_msg);

    // thread
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_thread_wait_for_evt);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_thread_wait_for_msg);
} vsf_kernel_vplt_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif
/* EOF */
