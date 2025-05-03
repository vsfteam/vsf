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

#include "component/vsf_component_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

#include "btstack_config.h"

#include "hci_dump.h"
#include "hci.h"

// for vsf_trace
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __hcp_dump_vsf_log_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len);
static void __hci_dump_vsf_log_message(int log_level, const char * format, va_list argptr);

/*============================ GLOBAL VARIABLES ==============================*/

const hci_dump_t hci_dump_vsf_impl = {
    .log_packet         = __hcp_dump_vsf_log_packet,
    .log_message        = __hci_dump_vsf_log_message,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __hcp_dump_vsf_log_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len)
{
    switch (packet_type) {
    case HCI_COMMAND_DATA_PACKET:
        vsf_trace_debug("CMD => " VSF_TRACE_CFG_LINEEND);
        break;
    case HCI_EVENT_PACKET:
        vsf_trace_debug("EVT <= " VSF_TRACE_CFG_LINEEND);
        break;
    case HCI_ACL_DATA_PACKET:
        if (in) {
            vsf_trace_debug("ACL <= " VSF_TRACE_CFG_LINEEND);
        } else {
            vsf_trace_debug("ACL => " VSF_TRACE_CFG_LINEEND);
        }
        break;
    case HCI_SCO_DATA_PACKET:
        if (in) {
            vsf_trace_debug("SCO <= " VSF_TRACE_CFG_LINEEND);
        } else {
            vsf_trace_debug("SCO => " VSF_TRACE_CFG_LINEEND);
        }
        break;
    case LOG_MESSAGE_PACKET:
        vsf_trace_debug("LOG -- %s\n\r", (char*) packet);
        return;
    default:
        return;
    }
    vsf_trace_buffer(VSF_TRACE_DEBUG, packet, len, VSF_TRACE_DF_DEFAULT);
}

static void __hci_dump_vsf_log_message(int log_level, const char * format, va_list argptr)
{
    vsf_trace_arg(VSF_TRACE_DEBUG, format, argptr);
}

#endif      // VSF_USE_BTSTACK
