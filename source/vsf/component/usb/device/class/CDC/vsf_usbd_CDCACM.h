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

#ifndef __VSF_USBD_CDCACM_H__
#define __VSF_USBD_CDCACM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_CDCACM == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDCACM.h"

#if     defined(VSF_USBD_CDCACM_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_CDCACM_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

// __IFS_START + 0:     Conrol interface
// __IFS_START + 1:     Data interface
// __INT_EP_INTERVAL:   HS 0x10, otherwise 0xFF
#define USB_DESC_CDC_ACM(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL)\
            USB_DESC_IAD((__IFS_START), 2, USB_CLASS_COMM, 0x02, 0x01, (__I_FUNC))\
            USB_DESC_IFS((__IFS_START), 0, 1, USB_CLASS_COMM, 0x02, 0x01, (__I_FUNC))\
                                                /* Header Functional Descriptor */\
            0x05,                               /* bLength: Endpoint Descriptor size */\
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x00,                               /* bDescriptorSubtype: Header Func Desc */\
            USB_DESC_WORD(0x0110),              /* bcdCDC: spec release number */\
                                                /* Call Managment Functional Descriptor */\
            0x05,                               /* bFunctionLength */\
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x01,                               /* bDescriptorSubtype: Call Management Func Desc */\
            0x00,                               /* bmCapabilities: D0+D1 */     \
            0x01,                               /* bDataInterface: 1 */         \
                                                /* ACM Functional Descriptor */ \
            0x04,                               /* bFunctionLength */           \
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x02,                               /* bDescriptorSubtype: Abstract Control Management desc */\
            0x02,                               /* bmCapabilities */\
                                                /* Union Functional Descriptor */\
            0x05,                               /* bFunctionLength */           \
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x06,                               /* bDescriptorSubtype: Union func desc */\
            (__IFS_START),                      /* bMasterInterface: Communication class interface */\
            1 + (__IFS_START),                  /* bSlaveInterface0: Data Class Interface */\
                                                                                \
            USB_DESC_EP(USB_DIR_IN | (__INT_IN_EP), USB_ENDPOINT_XFER_INT, 8, __INT_EP_INTERVAL)\
            USB_DESC_IFS((__IFS_START) + 1, 0, 2, USB_CLASS_CDC_DATA, 0x00, 0x00, (__I_FUNC))\
            USB_DESC_EP(USB_DIR_IN | (__BULK_IN_EP), USB_ENDPOINT_XFER_BULK, __BULK_EP_SIZE, 0x00)\
            USB_DESC_EP(USB_DIR_OUT | (__BULK_OUT_EP), USB_ENDPOINT_XFER_BULK, __BULK_EP_SIZE, 0x00)

#define USB_DESC_CDC_ACM_LEN                                                    \
            (   USB_DT_INTERFACE_ASSOCIATION_SIZE +                             \
                USB_DT_INTERFACE_SIZE + 5 + 5 + 4 + 5 + USB_DT_ENDPOINT_SIZE +  \
                USB_DT_INTERFACE_SIZE + 2 * USB_DT_ENDPOINT_SIZE)

#define USB_DESC_CDC_UART_HS(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP)\
            USB_DESC_CDC_ACM((__IFS_START), (__I_FUNC), (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), 512, 0x10)
#define USB_DESC_CDC_UART_FS(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP)\
            USB_DESC_CDC_ACM((__IFS_START), (__I_FUNC), (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), 64, 0xFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbd_CDCACM_t)

def_simple_class(vsf_usbd_CDCACM_t) {

    public_member(
        implement(vsf_usbd_CDC_t)

        usb_CDCACM_line_coding_t line_coding;
        uint8_t control_line;

        struct {
            vsf_err_t (*set_line_coding)(usb_CDCACM_line_coding_t *line_coding);
            vsf_err_t (*set_control_line)(uint8_t control_line);
            vsf_err_t (*get_control_line)(uint8_t *control_line);
            vsf_err_t (*send_break)(void);
        } callback;
    )
};

typedef struct {
    implement_ex(vsf_usbd_ep_cfg_t, ep)
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_stream_t *tx_stream;
    vsf_stream_t *rx_stream;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
    implement_ex(vsf_stream_usr_cfg_t, stream_usr);
    implement_ex(vsf_stream_src_cfg_t, stream_src);
#endif
}vsf_usbd_CDCACM_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern const vsf_usbd_class_op_t vsf_usbd_CDCACM_control;
extern const vsf_usbd_class_op_t vsf_usbd_CDCACM_data;

/*============================ PROTOTYPES ====================================*/
extern 
vsf_err_t vsf_usbd_CMDACM_init( vsf_usbd_CDCACM_t *obj, 
                                const vsf_usbd_CDCACM_cfg_t *cfg);


#endif  // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_CDCACM
#endif	// __VSF_USBD_CDCACM_H__
