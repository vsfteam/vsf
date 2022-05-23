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

#ifndef __VSF_LINUX_NTP_H__
#define __VSF_LINUX_NTP_H__

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/sys/socket.h"
#   include "../../include/netinet/in.h"
#else
#   include <sys/socket.h>
#   include <netinet/in.h>
#endif

// for rtc
#include "hal/vsf_hal.h"

#if     defined(__VSF_LINUX_NTP_CLASS_IMPLEMENT)
#   undef __VSF_LINUX_NTP_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_linux_ntp_rtc_t) {
    public_member(
#if VSF_SPI_CFG_MULTI_CLASS == ENABLED
        vsf_rtc_t vsf_rtc;
#endif
        struct sockaddr_in host_addr;
        uint16_t timeout_ms;
    )

    private_member(
        int sock;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_linux_ntp_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr);
extern fsm_rt_t vsf_linux_ntp_rtc_enable(vsf_rtc_t *rtc_ptr);
extern vsf_err_t vsf_linux_ntp_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
