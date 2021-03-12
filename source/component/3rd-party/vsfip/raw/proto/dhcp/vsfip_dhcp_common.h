/*****************************************************************************
 *   Copyright(C)2009-2019 by SimonQian                                      *
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

#ifndef __VSFIP_DHCP_COMMON_H__
#define __VSFIP_DHCP_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "../../vsfip.h"

/*============================ MACROS ========================================*/

#define DHCP_CLIENT_PORT            68
#define DHCP_SERVER_PORT            67
#define DHCP_MAGIC                  0x63825363

#define DHCP_OPT_MSGTYPE_LEN        1
#define DHCP_OPT_MAXMSGSIZE_LEN     2
#define DHCP_OPT_MINLEN             (0x40 - 4)    // 64bytes - 4byte magic

#define DHCP_TOSERVER               1
#define DHCP_TOCLIENT               2

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum dhcp_opt_t {
    DHCP_OPT_PAD                    = 0,
    DHCP_OPT_SUBNETMASK             = 1,
    DHCP_OPT_ROUTER                 = 3,
    DHCP_OPT_DNSSERVER              = 6,
    DHCP_OPT_HOSTNAME               = 12,
    DHCP_OPT_DOMAIN                 = 15,
    DHCP_OPT_MTU                    = 26,
    DHCP_OPT_BROADCAST              = 28,
    DHCP_OPT_ROUTERDISCOVERY        = 31,
    DHCP_OPT_REQIP                  = 50,
    DHCP_OPT_LEASE_TIME             = 51,
    DHCP_OPT_MSGTYPE                = 53,
    DHCP_OPT_SERVERID               = 54,
    DHCP_OPT_PARAMLIST              = 55,
    DHCP_OPT_MAXMSGSIZE             = 57,
    DHCP_OPT_RENEW_TIME             = 58,
    DHCP_OPT_REBINDING_TIME         = 59,
    DHCP_OPT_CLASSID                = 60,
    DHCP_OPT_END                    = 255,
} dhcp_opt_t;

typedef enum dhcp_op_t {
    DHCP_OP_DISCOVER                = 1,
    DHCP_OP_OFFER                   = 2,
    DHCP_OP_REQUEST                 = 3,
    DHCP_OP_DECLINE                 = 4,
    DHCP_OP_ACK                     = 5,
    DHCP_OP_NAK                     = 6,
    DHCP_OP_RELEASE                 = 7,
    DHCP_OP_INFORM                  = 8,
} dhcp_op_t;

typedef struct vsfip_dhcp_head_t {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    char sname[64];
    char file[128];
    uint32_t magic;
    uint8_t options[DHCP_OPT_MINLEN];    // min option size
} PACKED vsfip_dhcp_head_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsfip_dhcp_append_opt(vsfip_netbuf_t * netbuf, uint_fast32_t *optlen,
                        uint_fast8_t option, uint_fast8_t len, uint8_t *data);
extern void vsfip_dhcp_end_opt(vsfip_netbuf_t *netbuf, uint32_t *optlen);
extern uint_fast8_t vsfip_dhcp_get_opt(vsfip_netbuf_t *netbuf, uint_fast8_t option,
                        uint8_t **data);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_DHCP_COMMON_H__
