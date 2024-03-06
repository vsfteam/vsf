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

#ifndef __VSF_MUSB_FDRC_HW_H__
#define __VSF_MUSB_FDRC_HW_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED)

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// Common.Power
#define MUSB_POWER_ISOUPDATE            ((uint8_t)(1 << 7))
#define MUSB_POWER_RESET                ((uint8_t)(1 << 3))
#define MSUB_POWER_RESUME               ((uint8_t)(1 << 2))
#define MSUB_POWER_SUSPENDM             ((uint8_t)(1 << 1))
#define MUSB_POWER_ENABLESUSPEND        ((uint8_t)(1 << 0))

// Common.INTRUSB
#define MUSB_INTRUSB_VBUSERROR          ((uint8_t)(1 << 7))
#define MUSB_INTRUSB_SESSREQ            ((uint8_t)(1 << 6))
#define MUSB_INTRUSB_DISCON             ((uint8_t)(1 << 5))
#define MUSB_INTRUSB_CONN               ((uint8_t)(1 << 4))
#define MUSB_INTRUSB_SOF                ((uint8_t)(1 << 3))
#define MUSB_INTRUSB_RESET              ((uint8_t)(1 << 2))
#define MUSB_INTRUSB_RESUME             ((uint8_t)(1 << 1))
#define MUSB_INTRUSB_SUSPEND            ((uint8_t)(1 << 0))

// Common.INTRUSBE
#define MUSB_INTRUSBE_DISCON            ((uint8_t)(1 << 5))
#define MUSB_INTRUSBE_CONN              ((uint8_t)(1 << 4))
#define MUSB_INTRUSBE_SOF               ((uint8_t)(1 << 3))
#define MUSB_INTRUSBE_RESET             ((uint8_t)(1 << 2))
#define MUSB_INTRUSBE_RESUME            ((uint8_t)(1 << 1))
#define MUSB_INTRUSBE_SUSPEND           ((uint8_t)(1 << 0))

// Common.DevCtl
#define MUSB_DEVCTL_BDEVICE             ((uint8_t)(1 << 7))
#define MUSB_DEVCTL_FSDEV               ((uint8_t)(1 << 6))
#define MUSB_DEVCTL_LSDEV               ((uint8_t)(1 << 5))
#define MUSB_DEVCTL_HOSTMODE            ((uint8_t)(1 << 2))
#define MUSB_DEVCTL_HOSTREQ             ((uint8_t)(1 << 1))
#define MUSB_DEVCTL_SESSION             ((uint8_t)(1 << 0))

// EP0.CSR0
#define MUSBD_CSR0_SERVICEDSETUPEND     ((uint8_t)(1 << 7))
#define MUSBD_CSR0_SERVICEDRXPKGRDY     ((uint8_t)(1 << 6))
#define MUSBD_CSR0_SENDSTALL            ((uint8_t)(1 << 5))
#define MUSBD_CSR0_SETUPEND             ((uint8_t)(1 << 4))
#define MUSBD_CSR0_DATAEND              ((uint8_t)(1 << 3))
#define MUSBD_CSR0_SENTSTALL            ((uint8_t)(1 << 2))
#define MUSBD_CSR0_TXPKTRDY             ((uint8_t)(1 << 1))
#define MUSBD_CSR0_RXPKTRDY             ((uint8_t)(1 << 0))

#define MUSBH_CSR0_NAKTIMEOUT           ((uint8_t)(1 << 7))
#define MUSBH_CSR0_STATUSPKT            ((uint8_t)(1 << 6))
#define MUSBH_CSR0_REQPKT               ((uint8_t)(1 << 5))
#define MUSBH_CSR0_ERROR                ((uint8_t)(1 << 4))
#define MUSBH_CSR0_SETUPPKT             ((uint8_t)(1 << 3))
#define MUSBH_CSR0_RXSTALL              ((uint8_t)(1 << 2))
#define MUSBH_CSR0_TXPKTRDY             ((uint8_t)(1 << 1))
#define MUSBH_CSR0_RXPKTRDY             ((uint8_t)(1 << 0))
#define MUSBH_CSR0_ERRMASK              (MUSBH_CSR0_NAKTIMEOUT | MUSBH_CSR0_ERROR | MUSBH_CSR0_RXSTALL)

// EPN.TXCSR1
#define MUSBD_TXCSR1_CLRDATATOG         ((uint8_t)(1 << 6))
#define MUSBD_TXCSR1_SENTSTALL          ((uint8_t)(1 << 5))
#define MUSBD_TXCSR1_SENDSTALL          ((uint8_t)(1 << 4))
#define MUSBD_TXCSR1_FLUSHFIFO          ((uint8_t)(1 << 3))
#define MUSBD_TXCSR1_UNDERRUN           ((uint8_t)(1 << 2))
#define MUSBD_TXCSR1_FIFONOTEMPTY       ((uint8_t)(1 << 1))
#define MUSBD_TXCSR1_TXPKTRDY           ((uint8_t)(1 << 0))

#define MUSBH_TXCSR1_NAKTIMEOUT         ((uint8_t)(1 << 7))
#define MUSBH_TXCSR1_CLRDATATOG         ((uint8_t)(1 << 6))
#define MUSBH_TXCSR1_RXSTALL            ((uint8_t)(1 << 5))
#define MUSBH_TXCSR1_FLUSHFIFO          ((uint8_t)(1 << 3))
#define MUSBH_TXCSR1_ERROR              ((uint8_t)(1 << 2))
#define MUSBH_TXCSR1_FIFONOTEMPTY       ((uint8_t)(1 << 1))
#define MUSBH_TXCSR1_TXPKTRDY           ((uint8_t)(1 << 0))
#define MUSBH_TXCSR1_ERRMASK            (MUSBH_TXCSR1_NAKTIMEOUT | MUSBH_TXCSR1_RXSTALL | MUSBH_TXCSR1_ERROR)

// EPN.TXCSR2
#define MUSB_TXCSR2_AUTOSET             ((uint8_t)(1 << 7))
#define MUSBD_TXCSR2_ISO                ((uint8_t)(1 << 6))
#define MUSB_TXCSR2_MODE                ((uint8_t)(1 << 5))
#define MUSB_TXCSR2_DMAEN               ((uint8_t)(1 << 4))
#define MUSB_TXCSR2_FRCDATATOG          ((uint8_t)(1 << 3))
#define MUSB_TXCSR2_DMAMODE             ((uint8_t)(1 << 2))

// EPN.RXCSR1
#define MUSBD_RXCSR1_CLRDATATOG         ((uint8_t)(1 << 7))
#define MUSBD_RXCSR1_SENTSTALL          ((uint8_t)(1 << 6))
#define MUSBD_RXCSR1_SENDSTALL          ((uint8_t)(1 << 5))
#define MUSBD_RXCSR1_FLUSHFIFO          ((uint8_t)(1 << 4))
#define MUSBD_RXCSR1_DATAERROR          ((uint8_t)(1 << 3))
#define MUSBD_RXCSR1_OVERRUN            ((uint8_t)(1 << 2))
#define MUSBD_RXCSR1_FIFOFULL           ((uint8_t)(1 << 1))
#define MUSBD_RXCSR1_RXPKTRDY           ((uint8_t)(1 << 0))

#define MUSBH_RXCSR1_CLRDATATOG         ((uint8_t)(1 << 7))
#define MUSBH_RXCSR1_RXSTALL            ((uint8_t)(1 << 6))
#define MUSBH_RXCSR1_REQPKT             ((uint8_t)(1 << 5))
#define MUSBH_RXCSR1_FLUSHFIFO          ((uint8_t)(1 << 4))
#define MUSBH_RXCSR1_DATAERROR          ((uint8_t)(1 << 3))
#define MUSBH_RXCSR1_NAKTIMEOUT         ((uint8_t)(1 << 3))
#define MUSBH_RXCSR1_ERROR              ((uint8_t)(1 << 2))
#define MUSBH_RXCSR1_FIFOFULL           ((uint8_t)(1 << 1))
#define MUSBH_RXCSR1_RXPKTRDY           ((uint8_t)(1 << 0))
#define MUSBH_RXCSR1_ERRMASK            (MUSBH_RXCSR1_RXSTALL | MUSBH_RXCSR1_DATAERROR | MUSBH_RXCSR1_NAKTIMEOUT | MUSBH_RXCSR1_ERROR)

// EPN.RXCSR1
#define MUSB_RXCSR2_AUTOCLEAR           ((uint8_t)(1 << 7))
#define MUSBD_RXCSR2_ISO                ((uint8_t)(1 << 6))
#define MUSBH_RXCSR2_AUTOREQ            ((uint8_t)(1 << 6))
#define MUSB_RXCSR2_DMAEN               ((uint8_t)(1 << 5))
#define MUSB_RXCSR2_DMAMODE             ((uint8_t)(1 << 4))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if     VSF_MUSB_FDRC_REG_WIDTH == 32
typedef uint32_t                        vk_musb_reg_t;
#elif   VSF_MUSB_FDRC_REG_WIDTH == 16
typedef uint16_t                        vk_musb_reg_t;
#else
typedef uint8_t                         vk_musb_reg_t;
#endif

#ifndef VSF_MUSB_FDRC_PRIV_REG_T
typedef struct vk_musb_fdrc_common_reg_t {
    volatile vk_musb_reg_t FAddr;
    volatile vk_musb_reg_t Power;
    volatile vk_musb_reg_t IntrTx1;
    volatile vk_musb_reg_t IntrTx2;
    volatile vk_musb_reg_t IntrRx1;
    volatile vk_musb_reg_t IntrRx2;
    volatile vk_musb_reg_t IntrUSB;
    volatile vk_musb_reg_t IntrTx1E;
    volatile vk_musb_reg_t IntrTx2E;
    volatile vk_musb_reg_t IntrRx1E;
    volatile vk_musb_reg_t IntrRx2E;
    volatile vk_musb_reg_t IntrUSBE;
    volatile vk_musb_reg_t Frame1;
    volatile vk_musb_reg_t Frame2;
    volatile vk_musb_reg_t Index;
    volatile vk_musb_reg_t DevCtl;
} vk_musb_fdrc_common_reg_t;
#endif

typedef struct vk_musb_fdrc_ep0_reg_t {
    volatile vk_musb_reg_t Reserved0;
    volatile vk_musb_reg_t CSR0;
    volatile vk_musb_reg_t CSR02;
    volatile vk_musb_reg_t Reserved1[3];
    volatile vk_musb_reg_t Count0;
} vk_musb_fdrc_ep0_reg_t;

typedef struct vk_musb_fdrc_epn_reg_t {
    volatile vk_musb_reg_t TxMAXP;
    volatile vk_musb_reg_t TxCSR1;
    volatile vk_musb_reg_t TxCSR2;
    volatile vk_musb_reg_t RxMAXP;
    volatile vk_musb_reg_t RxCSR1;
    volatile vk_musb_reg_t RxCSR2;
    volatile vk_musb_reg_t RxCount1;
    volatile vk_musb_reg_t RxCount2;
    volatile vk_musb_reg_t TxType;        // for host only
    volatile vk_musb_reg_t TxInterval;    // for host only
    volatile vk_musb_reg_t RxType;        // for host only
    volatile vk_musb_reg_t RxInterval;    // for host only
} vk_musb_fdrc_epn_reg_t;

#if     defined(VSF_MUSB_FDRC_DYNAMIC_FIFO)
// Indexed area,    TxFIFO1, TxFIFO2, RxFIFO1, RxFIFO2 with vk_musb_reg_t width
typedef struct vk_musb_fdrc_epn_dynfifo_reg_t {
    volatile vk_musb_reg_t TxFIFO1;
    volatile vk_musb_reg_t TxFIFO2;
    volatile vk_musb_reg_t RxFIFO1;
    volatile vk_musb_reg_t RxFIFO2;
} vk_musb_fdrc_epn_dynfifo_reg_t;
#elif   defined(VSF_MUSB_FDRC_DYNAMIC_FIFO2)
// Indexed area,    TxFIFOSz, RxFIFOSz with vk_musb_reg_t width
//                  TxFIFOAdd, RxFIFOAdd with 16-bit width
typedef struct vk_musb_fdrc_epn_dynfifo_reg_t {
    volatile vk_musb_reg_t TxFIFOSz;
    volatile vk_musb_reg_t RxFIFOSz;
    volatile uint16_t TxFIFOAdd;
    volatile uint16_t RxFIFOAdd;
} VSF_CAL_PACKED vk_musb_fdrc_epn_dynfifo_reg_t;
#endif

typedef union vk_musb_fdrc_fifo_reg_t {
    struct {
        volatile uint32_t FIFO0;
        volatile uint32_t FIFO1;
        volatile uint32_t FIFO2;
        volatile uint32_t FIFO3;
        volatile uint32_t FIFO4;
        volatile uint32_t FIFO5;
        volatile uint32_t FIFO6;
        volatile uint32_t FIFO7;
    };
    volatile uint32_t FIFO[8];
} vk_musb_fdrc_fifo_reg_t;

typedef union vk_musb_fdrc_ep_reg_t {
    vk_musb_fdrc_ep0_reg_t EP0;
    vk_musb_fdrc_epn_reg_t EPN;
} vk_musb_fdrc_ep_reg_t;

typedef struct vk_musb_fdrc_reg_info_t {
    vk_musb_fdrc_common_reg_t *Common;
    vk_musb_fdrc_ep_reg_t *EP;
    vk_musb_fdrc_fifo_reg_t *FIFO;
#if defined(VSF_MUSB_FDRC_DYNAMIC_FIFO) || defined(VSF_MUSB_FDRC_DYNAMIC_FIFO2)
    vk_musb_fdrc_epn_dynfifo_reg_t *DynFIFO;
#endif

#if defined(VSF_MUSB_FDRC_NO_EP_IDX) || defined(VSF_MUSB_FDRC_NO_HWFIFO)
    void *param;
#endif
#if defined(VSF_MUSB_FDRC_NO_EP_IDX)
    // some musb instance does not support COMMON->Index register, so need get_ep_reg to get ep registers
    void * (*get_ep_reg)(void *param, uint8_t ep);
#endif
#if defined(VSF_MUSB_FDRC_NO_HWFIFO)
    void (*set_ep_fifo)(void *param, uint8_t ep, void *buffer, uint_fast16_t size);
    void (*write_ep_fifo)(void *param, uint8_t ep, uint_fast16_t size);
#endif
} vk_musb_fdrc_reg_info_t;

typedef struct vk_musb_fdrc_reg_t {
    union {
        vk_musb_fdrc_reg_info_t;
        vk_musb_fdrc_reg_info_t info;
    };

#if defined(VSF_MUSB_FDRC_NO_EP_IDX)
    uint8_t __cur_ep;
#endif
#if defined(VSF_MUSB_FDRC_NO_HWFIFO)
#   ifndef VSF_MUSB_FDRC_FIFO_SIZE
#       warning no hw fifo, VSF_MUSB_FDRC_FIFO_SIZE(not defined by user) is used to defined the size of user fifo, default 1K
#       define VSF_MUSB_FDRC_FIFO_SIZE          (1024)
#   endif
    // some musb instance have no hw fifo, so need fifos from user(driver)
    // granularity of musb fifo is 8-byte
    uint64_t __fifo[VSF_MUSB_FDRC_FIFO_SIZE >> 3] VSF_CAL_ALIGN(8);
    vk_musb_fdrc_fifo_reg_t __fifo_reg[2];
#endif
} vk_musb_fdrc_reg_t;

#if (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED)
typedef struct vk_musb_fdrc_dc_ip_info_t {
    implement(usb_dc_ip_info_t)
    implement(vk_musb_fdrc_reg_info_t)
} vk_musb_fdrc_dc_ip_info_t;
#endif

#if (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED)
typedef struct vk_musb_fdrc_hc_ip_info_t {
    implement(usb_hc_ip_info_t)
    implement(vk_musb_fdrc_reg_info_t)
} vk_musb_fdrc_hc_ip_info_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
