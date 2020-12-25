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

#include "vsf.h"

#if APP_USE_BTSTACK_DEMO == ENABLED

#include "btstack_event.h"
#include "btstack_run_loop.h"
#include "btstack_memory.h"
#include "hci.h"
#include "csr/btstack_chipset_csr.h"
#include "bcm/btstack_chipset_bcm.h"
#include "component/3rd-party/btstack/port/btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static btstack_packet_callback_registration_t __hci_event_callback_registration;
static vsf_eda_t *__eda;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    bd_addr_t addr;

    switch (event) {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
#if APP_USE_LINUX_DEMO != ENABLED
            vsf_trace_info("Starting inquiry scan..\n");
            gap_inquiry_start(5);
#else
            vsf_trace_info("btstack started...\n");
#endif
        }
        break;
    case GAP_EVENT_INQUIRY_RESULT:
        gap_event_inquiry_result_get_bd_addr(packet, addr);

        vsf_trace_info("Device found: %s ",  bd_addr_to_str(addr));
        vsf_trace_info("with COD: 0x%06x, ", (unsigned int)gap_event_inquiry_result_get_class_of_device(packet));
        vsf_trace_info("pageScan %d, ",      gap_event_inquiry_result_get_page_scan_repetition_mode(packet));
        vsf_trace_info("clock offset 0x%04x",gap_event_inquiry_result_get_clock_offset(packet));

        if (gap_event_inquiry_result_get_rssi_available(packet)){
            vsf_trace_info(", rssi %d dBm", (int8_t) gap_event_inquiry_result_get_rssi(packet));
        }
        if (gap_event_inquiry_result_get_name_available(packet)) {
            char name_buffer[240];
            int name_len = gap_event_inquiry_result_get_name_len(packet);
            memcpy(name_buffer, gap_event_inquiry_result_get_name(packet), name_len);
            name_buffer[name_len] = 0;
            vsf_trace_info(", name '%s'", name_buffer);
        }
        vsf_trace_info("\n");
        break;
    case GAP_EVENT_INQUIRY_COMPLETE:
        ASSERT(__eda != NULL);
        vsf_eda_post_evt(__eda, VSF_EVT_USER);
        break;
    default:
        break;
    }
}

static int btstack_main(int argc, const char * argv[])
{
    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    __hci_event_callback_registration.callback = &__btstack_packet_handler;
    hci_add_event_handler(&__hci_event_callback_registration);
    hci_power_control(HCI_POWER_ON);
    return 0;
}

vsf_err_t vsf_bluetooth_h2_on_new(void *dev, vk_usbh_dev_id_t *id)
{
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_vsf_get_instance());
    hci_init(hci_transport_usb_instance(), dev);

    if ((id->idVendor == 0x0A12) && (id->idProduct == 0x0001)) {
        hci_set_chipset(btstack_chipset_csr_instance());
    } else if ((id->idVendor == 0x0A5C) && (id->idProduct == 0x21E8)) {
        hci_set_chipset(btstack_chipset_bcm_instance());
    } else {
        return VSF_ERR_FAIL;
    }

    btstack_main(0, NULL);
    return VSF_ERR_NONE;
}

#if APP_USE_LINUX_DEMO == ENABLED
int btstack_scan_main(int argc, char *argv[])
{
    if (HCI_STATE_WORKING == hci_get_state()) {
        vsf_trace_info("Starting inquiry scan..\n");
        gap_inquiry_start(5);

        __eda = vsf_eda_get_cur();
        vsf_thread_wfe(VSF_EVT_USER);
        __eda = NULL;
    } else {
        vsf_trace_info("bluetooth is not available..\n");
    }
    return 0;
}
#endif

#endif
