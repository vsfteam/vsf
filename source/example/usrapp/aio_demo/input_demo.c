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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void input_demo_trace_hid(vsf_hid_event_t *hid_evt)
{
    if (hid_evt->id != 0) {
        uint_fast16_t generic_usage, usage_page, usage_id;

        generic_usage = HID_GET_GENERIC_USAGE(hid_evt->id);
        usage_page = HID_GET_USAGE_PAGE(hid_evt->id);
        usage_id = HID_GET_USAGE_ID(hid_evt->id);

        vsf_trace(VSF_TRACE_DEBUG, "hid(%d): page=%d, id=%d, pre=%d, cur=%d" VSF_TRACE_CFG_LINEEND,
            generic_usage, usage_page, usage_id, hid_evt->pre, hid_evt->cur);
    }
}

#if 1
void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *event)
{
    switch (type) {
    case VSF_INPUT_TYPE_HID:
        input_demo_trace_hid((vsf_hid_event_t *)event);
        break;
    case VSF_INPUT_TYPE_SENSOR:
        break;
    default:
        break;
    }
}
#else
void vsf_hid_on_report_input(vsf_hid_event_t *hid_evt)
{
    input_demo_trace_hid(hid_evt);
}
#endif

void input_demo_start(void)
{
}