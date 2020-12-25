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

#ifndef __VSF_MUSB_FDRC_COMMON_H__
#define __VSF_MUSB_FDRC_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     (   (VSF_USE_USB_DEVICE == ENABLED)                                     \
        && (VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED))                             \
    ||  (   (VSF_USE_USB_HOST == ENABLED)                                       \
        && (VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED))

#include "hal/vsf_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// Common.Power
#define MUSB_POWER_RESET                ((uint8_t)(1 << 3))
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
#define MUSB_DEVCTL_FSDEV               ((uint8_t)(1 << 6))
#define MUSB_DEVCTL_LSDEV               ((uint8_t)(1 << 5))
#define MUSB_DEVCTL_HOSTMODE            ((uint8_t)(1 << 2))
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

typedef struct vk_musb_fdrc_reg_t {
    struct {
        volatile uint8_t FAddr;
        volatile uint8_t Power;
        volatile uint8_t IntrTx1;
        volatile uint8_t IntrTx2;
        volatile uint8_t IntrRx1;
        volatile uint8_t IntrRx2;
        volatile uint8_t IntrUSB;
        volatile uint8_t IntrTx1E;
        volatile uint8_t IntrTx2E;
        volatile uint8_t IntrRx1E;
        volatile uint8_t IntrRx2E;
        volatile uint8_t IntrUSBE;
        volatile uint8_t Frame1;
        volatile uint8_t Frame2;
        volatile uint8_t Index;
        volatile uint8_t DevCtl;
    } Common;

    union {
        struct {
            volatile uint8_t Reserved0;
            volatile uint8_t CSR0;
            volatile uint8_t CSR02;
            volatile uint8_t Reserved1[3];
            volatile uint8_t Count0;
        } EP0;
        struct {
            volatile uint8_t TxMAXP;
            volatile uint8_t TxCSR1;
            volatile uint8_t TxCSR2;
            volatile uint8_t RxMAXP;
            volatile uint8_t RxCSR1;
            volatile uint8_t RxCSR2;
            volatile uint8_t RxCount1;
            volatile uint8_t RxCount2;
            volatile uint8_t TxType;        // for host only
            volatile uint8_t TxInterval;    // for host only
            volatile uint8_t RxType;        // for host only
            volatile uint8_t RxInterval;    // for host only
            volatile uint8_t TxFIFO1;
            volatile uint8_t TxFIFO2;
            volatile uint8_t RxFIFO1;
            volatile uint8_t RxFIFO2;
        } EPN;
    };

    union {
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
    };
} vk_musb_fdrc_reg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast8_t vk_musb_fdrc_set_ep(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep);

extern void vk_musb_fdrc_set_mask(volatile uint8_t *reg, uint_fast8_t ep);
extern void vk_musb_fdrc_clear_mask(volatile uint8_t *reg, uint_fast8_t ep);
extern uint16_t vk_musb_fdrc_get_mask(volatile uint8_t *reg);

extern void vk_musb_fdrc_interrupt_init(vk_musb_fdrc_reg_t *reg);
extern uint_fast16_t vk_musb_fdrc_rx_fifo_size(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep);
extern void vk_musb_fdrc_read_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern void vk_musb_fdrc_write_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

#ifdef __cplusplus
}
#endif

#endif
#endif      // __VSF_MUSB_FDRC_COMMON_H__
/* EOF */
