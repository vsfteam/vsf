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

#include "vsf_cfg.h"

#if APP_USE_BTSTACK_DEMO == ENABLED

#include <unistd.h>
#include "btstack_event.h"
#include "hci.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static btstack_packet_callback_registration_t __hci_event_callback_registration;
static vsf_eda_t *__eda;

/*============================ PROTOTYPES ====================================*/

// avoid to include unistd, because esp32 platform conflicts with stdlib in linux subsystem
//  and btstack headers will include these conflicted headers
typedef int (*vsf_linux_main_entry_t)(int, char **);
extern int busybox_bind(char *path, vsf_linux_main_entry_t entry);

/*============================ IMPLEMENTATION ================================*/

static void __btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    bd_addr_t addr;

    switch (event) {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
#if APP_USE_LINUX_DEMO != ENABLED
            vsf_trace_info("Starting inquiry scan.." VSF_TRACE_CFG_LINEEND);
            gap_inquiry_start(5);
#else
            vsf_trace_info("btstack started...", VSF_TRACE_CFG_LINEEND);
#endif
        }
        break;
    case GAP_EVENT_INQUIRY_RESULT:
        gap_event_inquiry_result_get_bd_addr(packet, addr);

        vsf_trace_info("Device found: %s ",  bd_addr_to_str(addr));
        vsf_trace_info("with COD: 0x%06x, ", (unsigned int)gap_event_inquiry_result_get_class_of_device(packet));
        vsf_trace_info("pageScan %d, ",      gap_event_inquiry_result_get_page_scan_repetition_mode(packet));
        vsf_trace_info("clock offset 0x%04x",gap_event_inquiry_result_get_clock_offset(packet));

        if (gap_event_inquiry_result_get_rssi_available(packet)) {
            vsf_trace_info(", rssi %d dBm", (int8_t) gap_event_inquiry_result_get_rssi(packet));
        }
        if (gap_event_inquiry_result_get_name_available(packet)) {
            char name_buffer[240];
            int name_len = gap_event_inquiry_result_get_name_len(packet);
            memcpy(name_buffer, gap_event_inquiry_result_get_name(packet), name_len);
            name_buffer[name_len] = 0;
            vsf_trace_info(", name '%s'", name_buffer);
        }
        vsf_trace_info(VSF_TRACE_CFG_LINEEND);
        break;
    case GAP_EVENT_INQUIRY_COMPLETE:
        VSF_ASSERT(__eda != NULL);
        vsf_eda_post_evt(__eda, VSF_EVT_USER);
        break;
    default:
        break;
    }
}

int btstack_main(int argc, const char * argv[])
{
    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    __hci_event_callback_registration.callback = &__btstack_packet_handler;
    hci_add_event_handler(&__hci_event_callback_registration);
    hci_power_control(HCI_POWER_ON);
    return 0;
}

#if APP_USE_LINUX_DEMO == ENABLED
int btstack_scan_main(int argc, char *argv[])
{
    if (HCI_STATE_WORKING == hci_get_state()) {
        vsf_trace_info("Starting inquiry scan.." VSF_TRACE_CFG_LINEEND);
        gap_inquiry_start(5);

        __eda = vsf_eda_get_cur();
        vsf_thread_wfe(VSF_EVT_USER);
        __eda = NULL;
    } else {
        vsf_trace_info("bluetooth is not available.." VSF_TRACE_CFG_LINEEND);
    }
    return 0;
}

void btstack_install(void)
{
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/bt_scan", btstack_scan_main);
}
#endif

#endif
