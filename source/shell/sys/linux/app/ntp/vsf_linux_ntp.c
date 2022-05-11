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

/*============================ INCLUDES ======================================*/

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED

#define __VSF_LINUX_NTP_CLASS_IMPLEMENT
#include "./vsf_linux_ntp.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
$   include "../../include/unistd.h"
#   include "../../include/sys/time.h"
#else
#   include <unistd.h>
#   include <sys/time.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// RFC5905, Figure 8: Packet Header Format
//     0                   1                   2                   3
//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |LI | VN  |Mode |    Stratum     |     Poll      |  Precision   |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                         Root Delay                            |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                         Root Dispersion                       |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                          Reference ID                         |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    +                     Reference Timestamp (64)                  +
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    +                      Origin Timestamp (64)                    +
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    +                      Receive Timestamp (64)                   +
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    +                      Transmit Timestamp (64)                  +
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    .                                                               .
//    .                    Extension Field 1 (variable)               .
//    .                                                               .
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    .                                                               .
//    .                    Extension Field 2 (variable)               .
//    .                                                               .
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                          Key Identifier                       |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                                                               |
//    |                            dgst (128)                         |
//    |                                                               |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct ntp_packet_header_t {
    uint8_t LI_VN_Mode;
    uint8_t Stratum;
    uint8_t Poll;
    uint8_t Precision;

    uint32_t RootDelay;
    uint32_t RootDispersion;
    uint32_t ReferenceID;

    uint32_t ReferenceTimestampSeconds;
    uint32_t ReferenceTimestampFraction;

    uint32_t OriginTimestampSeconds;
    uint32_t OriginTimestampFraction;

    uint32_t ReceiveTimestampSeconds;
    uint32_t ReceiveTimestampFraction;

    uint32_t TransmitTimestampSeconds;
    uint32_t TransmitTimestampFraction;
} PACKED ntp_packet_header_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_linux_ntp_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr)
{
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_linux_ntp_rtc_enable(vsf_rtc_t *rtc_ptr)
{
    return fsm_rt_cpl;
}

vsf_err_t vsf_linux_ntp_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm)
{
    vsf_linux_ntp_rtc_t *ntp_rtc_ptr = (vsf_linux_ntp_rtc_t *)rtc_ptr;

    ntp_rtc_ptr->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ntp_rtc_ptr->sock < 0) {
        return VSF_ERR_FAIL;
    }

    if (ntp_rtc_ptr->timeout_ms > 0) {
        struct timeval timeout = {
            .tv_sec     = ntp_rtc_ptr->timeout_ms / 1000,
            .tv_usec    = (ntp_rtc_ptr->timeout_ms % 1000) * 1000,
        };
        setsockopt(ntp_rtc_ptr->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }

    ssize_t ret;
    ntp_packet_header_t ntp_packet_header = {
        .LI_VN_Mode =   (2 << 6)    // LI:1 - last minute of the day has 59 seconds
                    |   (4 << 3)    // VN:4 - ntp version 4
                    |   (3 << 0),   // Mode:5 - client
    };
    socklen_t addrlen = sizeof(ntp_rtc_ptr->host_addr);

    ret = sendto(ntp_rtc_ptr->sock, &ntp_packet_header, sizeof(ntp_packet_header),
            0, (const struct sockaddr *)&ntp_rtc_ptr->host_addr, addrlen);
    if (ret != sizeof(ntp_packet_header)) {
        goto close_and_fail;
    }

    ret = recvfrom(ntp_rtc_ptr->sock, &ntp_packet_header, sizeof(ntp_packet_header),
            0, (struct sockaddr *)&ntp_rtc_ptr->host_addr, &addrlen);
    if (ret != sizeof(ntp_packet_header)) {
        goto close_and_fail;
    }
    close(ntp_rtc_ptr->sock);

    // 32-bit seconds, will overflow in 2036
    // ntp time starts from 1900, while time_t starts from 1970
    time_t time = ntohl(ntp_packet_header.TransmitTimestampSeconds) - 2208988800ULL;
    if (rtc_tm != NULL) {
        struct tm *t = localtime(&time);

        rtc_tm->tm_sec = t->tm_sec;
        rtc_tm->tm_min = t->tm_min;
        rtc_tm->tm_hour = t->tm_hour;
        rtc_tm->tm_mday = t->tm_mday;
        rtc_tm->tm_wday = t->tm_wday;
        rtc_tm->tm_mon = t->tm_mon;
        rtc_tm->tm_year = t->tm_year + 1900;
    }
    return VSF_ERR_NONE;

close_and_fail:
    close(ntp_rtc_ptr->sock);
    return VSF_ERR_FAIL;
}

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SOCKET && VSF_LINUX_SOCKET_USE_INET
