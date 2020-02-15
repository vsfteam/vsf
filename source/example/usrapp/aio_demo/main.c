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

extern void bt_demo_start(void);
extern void usbh_demo_start(void);
extern void usbd_demo_start(void);
extern void tcpip_demo_start(void);
extern void input_demo_start(void);
extern void ui_demo_start(void);
extern void ai_demo_start(void);

// remove later
void ai_demo_init(void);
extern void ai_demo_mnist(uint_fast8_t index);

/*============================ IMPLEMENTATION ================================*/

void main(void)
{
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
    vsf_stdio_init();

    usbh_demo_start();
    tcpip_demo_start();
    usbd_demo_start();
    input_demo_start();

    // bt_demo uses usbh_demo, support USB bt dongle CSR8510
    bt_demo_start();

    ai_demo_init();
    ai_demo_mnist(1);

    // ui_demo will not return, so call last
    // and VSF_OS_CFG_MAIN_MODE MUST be VSF_OS_CFG_MAIN_MODE_IDLE
    // ui_demo uses usbd_demo, ui is implemented over UVC
    ui_demo_start();
}

/* EOF */
