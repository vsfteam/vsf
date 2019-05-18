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
#include <stdio.h>
#include <stdarg.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*
struct usrapp_const_t {
    struct {
        vsf_ohci_param_t ohci_param;
    } usbh;

    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[75 + 66];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        uint8_t str_cdc[14];
        uint8_t str_cdc2[16];
        vsf_usbd_desc_t std_desc[7];
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;
*/
struct usrapp_t {
    vsf_callback_timer_t poll_timer;
    vsf_callback_timer_t one_tick_timer;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void usrapp_on_timer(vsf_callback_timer_t *param)
{
    printf( "heartbeat: [%lld]" VSF_TRACE_CFG_LINEEND, vsf_timer_get_tick());
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 1000);
}

static void one_tick_timer_callback(vsf_callback_timer_t *p)
{
    vsf_callback_timer_add(&usrapp.one_tick_timer, 1);
}

int main(void)
{
    vsf_stdio_init();

    usrapp.poll_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 1000);

    usrapp.one_tick_timer.on_timer = one_tick_timer_callback;
    vsf_callback_timer_add(&usrapp.one_tick_timer, 1);
    //while(1);
    return 0;
}

/* EOF */
