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

#ifndef __VSF_LINUX_TELNETD_H__
#define __VSF_LINUX_TELNETD_H__

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/sys/socket.h"
#   include "../../include/netinet/in.h"
#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#endif

#undef PUBLIC_CONST
#if     defined(__VSF_LINUX_TELNETD_CLASS_IMPLEMENT)
#   undef __VSF_LINUX_TELNETD_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#elif   defined(__VSF_LINUX_TELNETD_CLASS_INHERIT__)
#   undef __VSF_LINUX_TELNETD_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST                             const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_linux_telnetd_t) {
    public_member(
        PUBLIC_CONST uint16_t port;
        PUBLIC_CONST uint8_t backlog;
    )

    private_member(
        int listener_sock;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_linux_telnetd_thread(vsf_linux_telnetd_t *telnetd);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
