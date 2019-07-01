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

#ifndef __VSF_MUSB_FDRC_H__
#define __VSF_MUSB_FDRC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     (   (VSF_USE_USB_DEVICE == ENABLED)                                     \
        && (VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED))                       \
    ||  (   (VSF_USE_USB_HOST == ENABLED)                                       \
        && (VSF_USE_USB_HOST_HCD_MUSB_FDRC == ENABLED))

#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_musb_fdrc_param_t {
    union {
        const i_usb_dc_ip_t *dc_op;
        const i_usb_hc_ip_t *hc_op;
    };
    vsf_arch_priority_t priority;
};
typedef struct vsf_musb_fdrc_param_t vsf_musb_fdrc_param_t;

struct vsf_musb_fdrc_reg_t {
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
            volatile uint8_t Reserved[4];
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
};
typedef struct vsf_musb_fdrc_reg_t vsf_musb_fdrc_reg_t;

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED

struct vsf_musb_fdrc_usbd_t {
    vsf_musb_fdrc_reg_t *reg;
    struct {
        usb_dc_evt_handler_t evt_handler;
        void *param;
    } callback;
    uint16_t ep_buf_ptr;
    enum {
        MUSB_FDRC_USBD_EP0_IDLE,
        MUSB_FDRC_USBD_EP0_DATA_IN,
        MUSB_FDRC_USBD_EP0_DATA_OUT,
    } ep0_state;
    bool has_data_stage;
    uint8_t ep_num;
    bool is_dma;
    const vsf_musb_fdrc_param_t *param;
};
typedef struct vsf_musb_fdrc_usbd_t vsf_musb_fdrc_usbd_t;

#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED
extern vsf_err_t vsf_musb_fdrc_usbd_init(vsf_musb_fdrc_usbd_t *usbd, usb_dc_cfg_t *cfg);
extern void vsf_musb_fdrc_usbd_fini(vsf_musb_fdrc_usbd_t *usbd);
extern void vsf_musb_fdrc_usbd_reset(vsf_musb_fdrc_usbd_t *usbd);

extern void vsf_musb_fdrc_usbd_connect(vsf_musb_fdrc_usbd_t *usbd);
extern void vsf_musb_fdrc_usbd_disconnect(vsf_musb_fdrc_usbd_t *usbd);
extern void vsf_musb_fdrc_usbd_wakeup(vsf_musb_fdrc_usbd_t *usbd);

extern void vsf_musb_fdrc_usbd_set_address(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vsf_musb_fdrc_usbd_get_address(vsf_musb_fdrc_usbd_t *usbd);

extern uint_fast16_t vsf_musb_fdrc_usbd_get_frame_number(vsf_musb_fdrc_usbd_t *usbd);
extern uint_fast8_t vsf_musb_fdrc_usbd_get_mframe_number(vsf_musb_fdrc_usbd_t *usbd);

extern void vsf_musb_fdrc_usbd_get_setup(vsf_musb_fdrc_usbd_t *usbd, uint8_t *buffer);
extern void vsf_musb_fdrc_usbd_status_stage(vsf_musb_fdrc_usbd_t *usbd, bool is_in);

extern bool vsf_musb_fdrc_usbd_ep_is_dma(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_add(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_musb_fdrc_usbd_ep_get_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_musb_fdrc_usbd_ep_set_stall(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);
extern bool vsf_musb_fdrc_usbd_ep_is_stalled(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_clear_stall(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);

extern uint_fast16_t vsf_musb_fdrc_usbd_ep_get_data_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_read_buffer(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_enable_OUT(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_musb_fdrc_usbd_ep_set_data_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_musb_fdrc_usbd_ep_write_buffer(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern void vsf_musb_fdrc_usbd_irq(vsf_musb_fdrc_usbd_t *usbd);
#endif      // VSF_USE_USB_DEVICE_DCD_MUSB_FDRC

#endif
#endif
/* EOF */
