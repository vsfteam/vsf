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

#if     APP_USE_STREAM_HAL_DEMO == ENABLED && APP_USE_STREAM_USART_DEMO == ENABLED\
    &&  VSF_USE_STREAM_HAL == ENABLED && VSF_USE_STREAM_USART == ENABLED        \
    &&  VSF_HAL_USE_USART == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/

#ifndef APP_STREAM_USART_DEMO_CFG_USART
#   define APP_STREAM_USART_DEMO_CFG_USART              vsf_usart0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

describe_block_stream(__stream_usart_tx, 2, 64)
describe_block_stream(__stream_usart_rx, 2, 64)

describe_usbd(__usbd, APP_CFG_USBD_VID, APP_CFG_USBD_PID, USB_DC_SPEED_FULL)
    usbd_common_desc(__usbd, u"VSF-USBD-Simplest", u"SimonQian", u"1.0.0", 64, USB_DESC_CDC_ACM_IAD_LEN, USB_CDC_ACM_IFS_NUM, USB_CONFIG_ATT_WAKEUP, 100)
        cdc_acm_desc(__usbd, 0 * USB_CDC_ACM_IFS_NUM, 0, 1, 2, 2, 64, 16)
    usbd_func_desc(__usbd)
        usbd_func_str_desc(__usbd, 0, u"VSF-CDC0")
    usbd_std_desc_table(__usbd)
        usbd_func_str_desc_table(__usbd, 0)
    usbd_func(__usbd)
        cdc_acm_func(__usbd, 0, 1, 2, 2, &__stream_usart_tx, &__stream_usart_rx, USB_CDC_ACM_LINECODE_115200_8N1)
    usbd_ifs(__usbd)
        cdc_acm_ifs(__usbd, 0)
end_describe_usbd(__usbd, VSF_USB_DC0)

static vsf_stream_usart_t __stream_usart = {
    .usart = &APP_STREAM_USART_DEMO_CFG_USART,
    .stream_tx = &__stream_usart_tx.use_as__vsf_stream_t,
    .stream_rx = &__stream_usart_rx.use_as__vsf_stream_t,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int stream_usart_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

#ifdef __WIN__
    static uint8_t ret;
    vsf_usart_get_can_used_port(&ret);
#endif

    VSF_STREAM_INIT(&__stream_usart_tx);
    VSF_STREAM_INIT(&__stream_usart_rx);
    vk_usbd_init(&__usbd);
    vk_usbd_connect(&__usbd);

    {
        usart_cfg_t cfg = {
            .mode       = USART_8_BIT_LENGTH | USART_NO_PARITY | USART_1_STOPBIT,
            .baudrate   = 115200,
        };
        vsf_usart_init(__stream_usart.usart, &cfg);
        vsf_usart_enable(__stream_usart.usart);
    }
    vsf_stream_usart_init(&__stream_usart);
    return 0;
}

#endif
