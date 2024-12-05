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

#ifndef __VSF_USB_CDCNCM_H__
#define __VSF_USB_CDCNCM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_CDCNCM_CAP_SetEthernetPacketFilter  (1 << 0)
#define USB_CDCNCM_CAP_NetAddress               (1 << 1)
#define USB_CDCNCM_CAP_EncapsulatedCmdResp      (1 << 2)
#define USB_CDCNCM_CAP_MaxDatagramSize          (1 << 3)
#define USB_CDCNCM_CAP_CrcMode                  (1 << 4)
#define USB_CDCNCM_CAP_NtbInputSize8            (1 << 5)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_cdcncm_ntb_param_t usb_cdcncm_ntb_param_t;
struct usb_cdcncm_ntb_param_t {
    uint16_t wLength;
    uint16_t bmNtbFormatsSupported;
    uint32_t dwNtbInMaxSize;
    uint16_t wNdpInDivisor;
    uint16_t wNdpInPayloadRemainder;
    uint16_t wNdpInAlignment;
    uint16_t wReservedZero;
    uint32_t dwNtbOutMaxSize;
    uint16_t wNdpOutDivisor;
    uint16_t wNdpOutPayloadRemainder;
    uint16_t wNdpOutAlignment;
    uint16_t wNtbOutMaxDatagrams;
} VSF_CAL_PACKED;

typedef struct usb_cdcncm_ntb_input_size_t usb_cdcncm_ntb_input_size_t;
struct usb_cdcncm_ntb_input_size_t {
    uint32_t dwNtbInMaxSize;
    uint16_t wNtbInMaxDataframs;
    uint16_t reserved;
} VSF_CAL_PACKED;

#define USB_CDCNCM_NTB_PARAM_NTB16              (1 << 0)
#define USB_CDCNCM_NTB_PARAM_NTB32              (1 << 1)

typedef enum usb_cdcncm_req_t {
    USB_CDCNCM_REQ_SET_ETHERNET_MULTICAST_FILTERS               = 0x40,
    USB_CDCNCM_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x41,
    USB_CDCNCM_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x42,
    USB_CDCNCM_REQ_SET_ETHERNET_PACKET_FILTER                   = 0x43,
    USB_CDCNCM_REQ_GET_ETHERNET_STATIC                          = 0x44,
    USB_CDCNCM_REQ_GET_NTB_PARAMETERS                           = 0x80,
    USB_CDCNCM_REQ_GET_NET_ADDRESS                              = 0x81,
    USB_CDCNCM_REQ_SET_NET_ADDRESS                              = 0x82,
    USB_CDCNCM_REQ_GET_NTB_FORMAT                               = 0x83,
    USB_CDCNCM_REQ_SET_NTB_FORMAT                               = 0x84,
    USB_CDCNCM_REQ_GET_NTB_INPUT_SIZE                           = 0x85,
    USB_CDCNCM_REQ_SET_NTB_INPUT_SIZE                           = 0x86,
    USB_CDCNCM_REQ_GET_MAX_DATAGRAM_SIZE                        = 0x87,
    USB_CDCNCM_REQ_SET_MAX_DATAGRAM_SIZE                        = 0x88,
    USB_CDCNCM_REQ_GET_CRC_MODE                                 = 0x89,
    USB_CDCNCM_REQ_SET_CRC_MODE                                 = 0x8A,
} usb_cdcncm_req_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USB_CDCNCM_H__
