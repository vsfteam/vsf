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

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "./vsfip_dhcp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// buf MUST be large enough for all the data
void vsfip_dhcp_append_opt(vsfip_netbuf_t * netbuf, uint32_t *optlen,
                        uint_fast8_t option, uint_fast8_t len, uint8_t *data)
{
    vsfip_dhcp_head_t *head = netbuf->app.obj_ptr;
    
    head->options[(*optlen)++] = option;
    head->options[(*optlen)++] = len;
    memcpy(&head->options[*optlen], data, len);
    *optlen += len;
}

void vsfip_dhcp_end_opt(vsfip_netbuf_t *netbuf, uint32_t *optlen)
{
    vsfip_dhcp_head_t *head = netbuf->app.obj_ptr;
    
    head->options[(*optlen)++] = DHCP_OPT_END;
    while ((*optlen < DHCP_OPT_MINLEN) || (*optlen & 3)) {
        head->options[(*optlen)++] = 0;
    }
    // tweak options length
    netbuf->app.size -= sizeof(head->options);
    netbuf->app.size += *optlen;
}

uint_fast8_t vsfip_dhcp_get_opt(vsfip_netbuf_t * netbuf, uint_fast8_t option,
                        uint8_t **data)
{
    vsfip_dhcp_head_t *head = netbuf->app.obj_ptr;
    uint8_t *ptr = head->options;

    while ((ptr[0] != DHCP_OPT_END) && ((ptr - netbuf->app.buffer) < netbuf->app.size)) {
        if (ptr[0] == option) {
            if (data != NULL) {
                *data = &ptr[2];
            }
            return ptr[1];
        }
        ptr += 2 + ptr[1];
    }
    return 0;
}

#endif      // VSF_USE_TCPIP
