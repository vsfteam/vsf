/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __VSF_USB_MSC_H__
#define __VSF_USB_MSC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_MSC_CBW_SIGNATURE           0x43425355
#define USB_MSC_CSW_SIGNATURE           0x53425355

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_msc_cbw_t usb_msc_cbw_t;
struct usb_msc_cbw_t {
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
} PACKED;

typedef struct usb_msc_csw_t usb_msc_csw_t;
struct usb_msc_csw_t {
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t dCSWStatus;
} PACKED;

typedef enum usb_msc_req_t {
    USB_MSC_REQ_GET_MAX_LUN = 0xFE,
    USB_MSC_REQ_RESET       = 0xFF,
} usb_msc_req_t;

typedef enum usb_msc_csw_status_t {
    USB_MSC_CSW_OK          = 0,
    USB_MSC_CSW_FAIL        = 1,
    USB_MSC_CSW_PHASE_ERROR = 2,
} usb_msc_csw_status_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_MSC_H__
