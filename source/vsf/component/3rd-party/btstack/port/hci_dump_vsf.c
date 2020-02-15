/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
#include "hci_transport.h"
#include "hci_cmd.h"
#include "btstack_run_loop.h"

// TODO: use dedicated include
#include "vsf.h"

#include <stdarg.h>
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR) || defined(ENABLE_LOG_DEBUG)
static void printf_timestamp(void)
{
    uint32_t time_ms = btstack_run_loop_get_time_ms();
    vsf_trace(VSF_TRACE_DEBUG, "[%04ums] ", time_ms);
}

static void printf_packet(uint8_t packet_type, uint8_t in, uint8_t * packet, uint16_t len)
{
    switch (packet_type) {
    case HCI_COMMAND_DATA_PACKET:
        vsf_trace(VSF_TRACE_DEBUG, "CMD => " VSF_TRACE_CFG_LINEEND);
        break;
    case HCI_EVENT_PACKET:
        vsf_trace(VSF_TRACE_DEBUG, "EVT <= " VSF_TRACE_CFG_LINEEND);
        break;
    case HCI_ACL_DATA_PACKET:
        if (in) {
            vsf_trace(VSF_TRACE_DEBUG, "ACL <= " VSF_TRACE_CFG_LINEEND);
        } else {
            vsf_trace(VSF_TRACE_DEBUG, "ACL => " VSF_TRACE_CFG_LINEEND);
        }
        break;
    case HCI_SCO_DATA_PACKET:
        if (in) {
            vsf_trace(VSF_TRACE_DEBUG, "SCO <= " VSF_TRACE_CFG_LINEEND);
        } else {
            vsf_trace(VSF_TRACE_DEBUG, "SCO => " VSF_TRACE_CFG_LINEEND);
        }
        break;
    case LOG_MESSAGE_PACKET:
        vsf_trace(VSF_TRACE_DEBUG, "LOG -- %s\n\r", (char*) packet);
        return;
    default:
        return;
    }
    vsf_trace_buffer(VSF_TRACE_DEBUG, packet, len, VSF_TRACE_DF_DEFAULT);
}

void hci_dump_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len)
{
    printf_timestamp();
    printf_packet(packet_type, in, packet, len);
}

void hci_dump_log(int log_level, const char * format, ...)
{
    char log_message_buffer[256];

    va_list argptr;
    va_start(argptr, format);
    int len = vsnprintf(log_message_buffer, sizeof(log_message_buffer), format, argptr);
    va_end(argptr);

    hci_dump_packet(LOG_MESSAGE_PACKET, 0, (uint8_t*)log_message_buffer, len);
}
#else
void hci_dump_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len)
{

}

void hci_dump_log(int log_level, const char * format, ...)
{

}
#endif

#endif      // VSF_USE_BTSTACK
