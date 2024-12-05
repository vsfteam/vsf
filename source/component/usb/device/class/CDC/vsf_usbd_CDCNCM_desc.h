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

#ifndef __VSF_USBD_CDCNCM_DESC_H__
#define __VSF_USBD_CDCNCM_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// __IFS_START + 0:     Control interface
// __IFS_START + 1:     Data interface
// __INT_EP_INTERVAL:   HS 0x10, otherwise 0xFF

#define USB_DESC_CDC_NCM3(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES, __MC_FITER_NUM, __PWR_FILTER_NUM)\
            USB_DESC_IFS((__IFS_START), 0, 1, USB_CLASS_COMM, 0x0D, 0x00, (__I_FUNC))\
                                                /* Header Functional Descriptor */\
            0x05,                               /* bLength: Endpoint Descriptor size */\
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x00,                               /* bDescriptorSubtype: Header Func Desc */\
            USB_DESC_WORD(0x0110),              /* bcdCDC: spec release number */\
                                                /* Union Functional Descriptor */\
            0x05,                               /* bFunctionLength */           \
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x06,                               /* bDescriptorSubtype: Union func desc */\
            (__IFS_START),                      /* bMasterInterface: Communication class interface */\
            1 + (__IFS_START),                  /* bSlaveInterface0: Data Class Interface */\
                                                /* CDC Ethernet Networking Functional Descriptor */\
            0x0D,                               /* bFunctionLength */           \
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x0F,                               /* bDescriptorSubtype: Ethernet Networking functional descriptor */\
            (__I_MAC),                          /* iMACAddress */               \
            USB_DESC_DWORD(0),                  /* bmEthernetStatistics  */     \
            USB_DESC_WORD(__MAX_SEG_SIZE),      /* WmAXsEGMENTsIZE  */          \
            USB_DESC_WORD(__MC_FITER_NUM),      /* WnUMBERmcfILTERS  */         \
            (__PWR_FILTER_NUM),                 /* BnUMBERpOWERfILTERS  */      \
                                                /* NCM Functional Descriptor */ \
            0x06,                               /* bFunctionLength */           \
            0x24,                               /* bDescriptorType: CS_INTERFACE */\
            0x1A,                               /* bDescriptorSubtype: NCM Function Descriptor */\
            USB_DESC_WORD(0x0100),              /* bcdNcmVersion: 1.00 */       \
            (__NETWORK_CAPABILITIES),           /* bmNetworkCapabilities */     \
                                                                                \
            USB_DESC_EP(USB_DIR_IN | (__INT_IN_EP), USB_ENDPOINT_XFER_INT, 16, (__INT_EP_INTERVAL))\
            USB_DESC_IFS((__IFS_START) + 1, 0, 0, USB_CLASS_CDC_DATA, 0x00, 0x01, (__I_FUNC))\
            USB_DESC_IFS((__IFS_START) + 1, 1, 2, USB_CLASS_CDC_DATA, 0x00, 0x01, (__I_FUNC))\
            USB_DESC_EP(USB_DIR_IN | (__BULK_IN_EP), USB_ENDPOINT_XFER_BULK, (__BULK_EP_SIZE), 0x00)\
            USB_DESC_EP(USB_DIR_OUT | (__BULK_OUT_EP), USB_ENDPOINT_XFER_BULK, (__BULK_EP_SIZE), 0x00)
#define USB_DESC_CDC_NCM2(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES, __MC_FITER_NUM)        \
            USB_DESC_CDC_NCM3(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES, __MC_FITER_NUM, 0)
#define USB_DESC_CDC_NCM1(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES)                        \
            USB_DESC_CDC_NCM2(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES, 0)
#define USB_DESC_CDC_NCM0(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE)                                                \
            USB_DESC_CDC_NCM1(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, 0)

// prototype: USB_DESC_CDC_NCM(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,
//                        __I_MAC, __MAX_SEG_SIZE, __NETWORK_CAPABILITIES = 0, __MC_FITER_NUM = 0, __PWR_FILTER_NUM = 0)
#define USB_DESC_CDC_NCM(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, __MAX_SEG_SIZE, ...)                           \
            __PLOOC_EVAL(USB_DESC_CDC_NCM, __VA_ARGS__)                         \
                (__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                __I_MAC, __MAX_SEG_SIZE, ##__VA_ARGS__)
#define USB_DESC_CDC_NCM_LEN                                                    \
            (   USB_DT_INTERFACE_SIZE + 5 + 5 + 13 + 6 + USB_DT_ENDPOINT_SIZE + \
                2 * USB_DT_INTERFACE_SIZE + 2 * USB_DT_ENDPOINT_SIZE)

#define USB_DESC_CDC_NCM_IAD(__IFS_START, __I_FUNC, __INT_IN_EP, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE, __INT_EP_INTERVAL,\
                        __I_MAC, ...)                                           \
            USB_DESC_IAD((__IFS_START), 2, USB_CLASS_COMM, 0x0D, 0x00, (__I_FUNC))\
            USB_DESC_CDC_NCM((__IFS_START), (__I_FUNC), (__INT_IN_EP), (__BULK_IN_EP), (__BULK_OUT_EP), (__BULK_EP_SIZE), (__INT_EP_INTERVAL),\
                        __I_MAC, ##__VA_ARGS__)
#define USB_DESC_CDC_NCM_IAD_LEN                (USB_DESC_IAD_LEN + USB_DESC_CDC_NCM_LEN)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USBD_CDCNCM_DESC_H__
