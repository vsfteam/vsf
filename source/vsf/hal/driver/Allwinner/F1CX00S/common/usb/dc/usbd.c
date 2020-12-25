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

//#include "../../common.h"
#include "../usb.h"
#include "./usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED

/*============================ MACROS ========================================*/

#define F1CX00S_USBD_TRACE_EN               ENABLED
#   define F1CX00S_USBD_TRACE_BUFFER_EN     DISABLED

/*============================ INCLUDES ======================================*/

#if F1CX00S_USBD_TRACE_EN == ENABLED
#   include "service/vsf_service.h"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_usb_dcd_trans_t {
    uint8_t *buffer;
    uint32_t remain;
    uint32_t size;
    uint8_t zlp;
} f1cx00s_usb_dcd_trans_t;

struct f1cx00s_usb_dcd_t {
    f1cx00s_usb_otg_t *otg;

    struct {
#if VSF_USE_USB_DEVICE == ENABLED
        usb_dc_evt_handler_t evthandler;
#endif
        void *param;
    } callback;

    uint16_t fifo_pos;
    uint16_t out_enable;
    bool has_data_stage;
    enum {
        MUSB_USBD_EP0_IDLE,
        MUSB_USBD_EP0_SETUP,
        MUSB_USBD_EP0_DATA_IN,
        MUSB_USBD_EP0_DATA_OUT,
        MUSB_USBD_EP0_STATUS,
    } ep0_state;
    // 4 eps include ep0, both IN and OUT
    f1cx00s_usb_dcd_trans_t trans[2 * 4];
};

/*============================ GLOBAL VARIABLES ==============================*/

f1cx00s_usb_dcd_t USB_DC0 = {
    .otg = &USB_OTG0,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __f1cx00s_usb_init(f1cx00s_usb_otg_t *usb, vsf_arch_prio_t priority);
extern uint_fast8_t __f1cx00s_usb_set_ep(f1cx00s_usb_otg_t *usb, uint_fast8_t ep);
extern void __f1cx00s_usb_clear_interrupt(f1cx00s_usb_otg_t *usb);
extern uint_fast16_t __f1cx00s_usb_rxfifo_size(f1cx00s_usb_otg_t *usb, uint_fast8_t ep);
extern void __f1cx00s_usb_read_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern void __f1cx00s_usb_write_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern void __f1cx00s_usb_register_irq(f1cx00s_usb_otg_t *usb, void (*irqhandler)(void *param), void *param);

extern uint_fast16_t vsf_usbd_get_fifo_size(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBD_GET_FIFO_SIZE
WEAK(vsf_usbd_get_fifo_size)
uint_fast16_t vsf_usbd_get_fifo_size(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    return size;
}
#endif

static void __f1cx00s_usbd_notify(f1cx00s_usb_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evthandler != NULL) {
        usbd->callback.evthandler(usbd->callback.param, evt, value);
    }
}

static f1cx00s_usb_dcd_trans_t * __f1cx00s_usbd_get_trans(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t half_trans_num = dimof(usbd->trans) / 2;
    uint_fast8_t is_in = ep & 0x80;
    ep &= 0x0F;

    VSF_HAL_ASSERT(ep < half_trans_num);
    return &usbd->trans[(is_in ? half_trans_num : 0) + ep];
}

vsf_err_t f1cx00s_usbd_init(f1cx00s_usb_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    uint_fast32_t reg_tmp;

    usbd->callback.evthandler = cfg->evt_handler;
    usbd->callback.param = cfg->param;

    __f1cx00s_usb_register_irq(usbd->otg, (void (*)(void *))f1cx00s_usbd_irq, usbd);
    __f1cx00s_usb_init(usbd->otg, cfg->priority);

    // pullup on DPDM and ID, FORCE_ID as device, FORCE_VBUS to high
    reg_tmp = MUSB_BASE->Vendor.ISCR;
    reg_tmp &= ~(   MUSB_ISCR_VBUS_CHANGE_DETECT | MUSB_ISCR_ID_CHANGE_DETECT | MUSB_ISCR_DPDM_CHANGE_DETECT
                |   MUSB_ISCR_FORCE_ID | MUSB_ISCR_FORCE_VBUS_VALID);
    reg_tmp |=      MUSB_ISCR_DPDM_PULLUP_EN | MUSB_ISCR_ID_PULLUP_EN
                |   MUSB_ISCR_FORCE_ID_DEVICE | MUSB_ISCR_FORCE_VBUS_VALID_HIGH;
    MUSB_BASE->Vendor.ISCR = reg_tmp;

    // TODO: ep0 in pio mode

    f1cx00s_usbd_reset(usbd, cfg);

    MUSB_BASE->Common.Power &= ~MUSBD_Power_ISOUpdate;
    switch (cfg->speed) {
    case USB_DC_SPEED_HIGH:
        MUSB_BASE->Common.Power |= MUSBD_Power_HSEnab;
        break;
    default:
        MUSB_BASE->Common.Power &= ~MUSBD_Power_HSEnab;
        break;
    }
    return VSF_ERR_NONE;
}

void f1cx00s_usbd_fini(f1cx00s_usb_dcd_t *usbd)
{
}

void f1cx00s_usbd_reset(f1cx00s_usb_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    // Reserve the 64bytes EP0 size
    usbd->fifo_pos = 64;
    __f1cx00s_usb_clear_interrupt(usbd->otg);

    // TODO: enable interrupt after arm9 arch is ready
    MUSB_BASE->Common.IntrUSBE = MUSBD_IntrUSBE_Reset | MUSB_IntrUSBE_Discon;
    MUSB_BASE->Common.IntrTxE |= 1 << 0;
}

void f1cx00s_usbd_connect(f1cx00s_usb_dcd_t *usbd)
{
    MUSB_BASE->Common.Power |= MUSBD_Power_SoftConn;
}

void f1cx00s_usbd_disconnect(f1cx00s_usb_dcd_t *usbd)
{
    MUSB_BASE->Common.Power &= ~MUSBD_Power_SoftConn;
}

void f1cx00s_usbd_wakeup(f1cx00s_usb_dcd_t *usbd)
{
}

void f1cx00s_usbd_set_address(f1cx00s_usb_dcd_t *usbd, uint_fast8_t addr)
{
    MUSB_BASE->Index.TxFuncAddr = addr;
}

uint_fast8_t f1cx00s_usbd_get_address(f1cx00s_usb_dcd_t *usbd)
{
    return MUSB_BASE->Index.TxFuncAddr;
}

uint_fast16_t f1cx00s_usbd_get_frame_number(f1cx00s_usb_dcd_t *usbd)
{
    return MUSB_BASE->Common.Frame;
}

uint_fast8_t f1cx00s_usbd_get_mframe_number(f1cx00s_usb_dcd_t *usbd)
{
    return 0;
}

void f1cx00s_usbd_get_setup(f1cx00s_usb_dcd_t *usbd, uint8_t *buffer)
{
    __f1cx00s_usb_read_fifo(usbd->otg, 0, buffer, 8);
#if F1CX00S_USBD_TRACE_EN == ENABLED
    vsf_trace_debug("Setup:\r\n");
    vsf_trace_buffer(VSF_TRACE_DEBUG, buffer, 8);
#endif
    uint_fast8_t ep_orig = __f1cx00s_usb_set_ep(usbd->otg, 0);
        MUSB_BASE->Index.DC.EP0.CSR0 |= MUSBD_CSR0_ServicedRxPktRdy;
#if F1CX00S_USBD_TRACE_EN == ENABLED
        vsf_trace_debug("get_setup CSR0: %02X\r\n", MUSB_BASE->Index.DC.EP0.CSR0);
#endif
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);

    VSF_HAL_ASSERT(MUSB_USBD_EP0_IDLE == usbd->ep0_state);
    usbd->has_data_stage = false;
    if (!(buffer[0] & 0x80)) {
        usbd->ep0_state = MUSB_USBD_EP0_DATA_OUT;
    }
}

void f1cx00s_usbd_status_stage(f1cx00s_usb_dcd_t *usbd, bool is_in)
{
#if F1CX00S_USBD_TRACE_EN == ENABLED
    vsf_trace_debug("Status_%s\r\n", is_in ? "IN" : "OUT");
#endif
    usbd->ep0_state = MUSB_USBD_EP0_STATUS;
}

uint_fast8_t f1cx00s_usbd_ep_get_feature(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature)
{
    return USB_DC_FEATURE_TRANSFER;
}

vsf_err_t f1cx00s_usbd_ep_add(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    uint_fast16_t fifo_size = (size + 511) & ~511;
    int_fast8_t size_log2;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    VSF_HAL_ASSERT(size <= 1024);

    // log2(size_align_to_512) - 3
    size_log2 = vsf_ffz(~vsf_usbd_get_fifo_size(ep, type, fifo_size)) - 3;

    ep &= 0x0F;
    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);

    if (!ep) {
        VSF_HAL_ASSERT((64 == size) && (USB_EP_TYPE_CONTROL == type));
        MUSB_BASE->Index.DC.EP0.CSR0 = MUSBD_CSR0_FlushFIFO;
    } else if (is_in) {
        VSF_HAL_ASSERT((size_log2 <= 15) && (type != USB_EP_TYPE_CONTROL));

        MUSB_BASE->Index.DC.EPN.TxCSRH = MUSBD_TxCSRH_Mode_Tx;
        MUSB_BASE->Index.DC.EPN.TxCSRL = MUSBD_TxCSRL_ClrDataTog | MUSBD_TxCSRL_FlushFIFO;
        MUSB_BASE->Index.DC.EPN.TxMaxP = size;
        if (USB_EP_TYPE_ISO == type) {
            MUSB_BASE->Index.DC.EPN.TxCSRH |= MUSBD_TxCSRH_ISO;
        } else {
            MUSB_BASE->Index.DC.EPN.TxCSRH &= ~MUSBD_TxCSRH_ISO;
        }

        MUSB_BASE->Index.TxFIFOadd = usbd->fifo_pos >> 3;
        MUSB_BASE->Index.TxFIFOsz = size_log2;
        usbd->fifo_pos += size;

        MUSB_BASE->Common.IntrTxE |= 1 << ep;
    } else {
        VSF_HAL_ASSERT((size_log2 <= 15) && (type != USB_EP_TYPE_CONTROL));

        MUSB_BASE->Index.DC.EPN.RxCSRL = MUSBD_RxCSRL_ClrDataTog | MUSBD_RxCSRL_FlushFIFO;
        MUSB_BASE->Index.DC.EPN.RxMaxP = size;
        if (USB_EP_TYPE_ISO == type) {
            MUSB_BASE->Index.DC.EPN.RxCSRH |= MUSBD_RxCSRH_ISO;
        } else {
            MUSB_BASE->Index.DC.EPN.RxCSRH &= ~MUSBD_RxCSRH_ISO;
        }

        MUSB_BASE->Index.RxFIFOadd = usbd->fifo_pos >> 3;
        MUSB_BASE->Index.RxFIFOsz = size_log2;
        usbd->fifo_pos += size;

        MUSB_BASE->Common.IntrRxE |= 1 << ep;
    }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return VSF_ERR_NONE;
}

uint_fast16_t f1cx00s_usbd_ep_get_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast16_t result;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    if (!ep) {
        return 64;
    }
    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);
        result = MUSB_BASE->Index.DC.EPN.TxMaxP;
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return result;
}

vsf_err_t f1cx00s_usbd_ep_set_stall(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);
        if (!ep) {
            MUSB_BASE->Index.DC.EP0.CSR0 |= MUSBD_CSR0_SendStall;
        } else {
            if (is_in) {
                MUSB_BASE->Index.DC.EPN.TxCSRL |= MUSBD_TxCSRL_SendStall;
            } else {
                MUSB_BASE->Index.DC.EPN.RxCSRL |= MUSBD_RxCSRL_SendStall;
            }
        }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return VSF_ERR_NONE;
}

bool f1cx00s_usbd_ep_is_stalled(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80, is_stall;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);
        if (!ep) {
            is_stall = MUSB_BASE->Index.DC.EP0.CSR0 & MUSBD_CSR0_SendStall;
        } else {
            if (is_in) {
                is_stall = MUSB_BASE->Index.DC.EPN.TxCSRL & MUSBD_TxCSRL_SendStall;
            } else {
                is_stall = MUSB_BASE->Index.DC.EPN.RxCSRL & MUSBD_RxCSRL_SendStall;
            }
        }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return is_stall > 0;
}

vsf_err_t f1cx00s_usbd_ep_clear_stall(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);
        if (!ep) {
            MUSB_BASE->Index.DC.EP0.CSR0 &= ~(MUSBD_CSR0_SentStall | MUSBD_CSR0_SendStall);
        } else {
            if (is_in) {
                MUSB_BASE->Index.DC.EPN.TxCSRL &= ~(MUSBD_TxCSRL_SentStall | MUSBD_TxCSRL_SendStall);
                MUSB_BASE->Index.DC.EPN.TxCSRL |= MUSBD_TxCSRL_ClrDataTog | MUSBD_TxCSRL_FlushFIFO;
            } else {
                MUSB_BASE->Index.DC.EPN.RxCSRL &= ~(MUSBD_RxCSRL_SentStall | MUSBD_RxCSRL_SendStall);
                MUSB_BASE->Index.DC.EPN.RxCSRL |= MUSBD_RxCSRL_ClrDataTog | MUSBD_RxCSRL_FlushFIFO;
            }
        }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return VSF_ERR_NONE;
}

uint_fast32_t f1cx00s_usbd_ep_get_data_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    f1cx00s_usb_dcd_trans_t *trans = __f1cx00s_usbd_get_trans(usbd, ep);
    return trans->size - trans->remain;
}

vsf_err_t f1cx00s_usbd_ep_transaction_read_buffer(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t f1cx00s_usbd_ep_transaction_enable_out(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t f1cx00s_usbd_ep_transaction_set_data_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t f1cx00s_usbd_ep_transaction_write_buffer(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __f1cx00s_usbd_ep_transfer_recv(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep_idx)
{
    uint_fast8_t ep = ep_idx;
    f1cx00s_usb_dcd_trans_t *trans = __f1cx00s_usbd_get_trans(usbd, ep);
    uint_fast16_t ep_size = f1cx00s_usbd_ep_get_size(usbd, ep);
    uint_fast16_t size = __f1cx00s_usb_rxfifo_size(usbd->otg, ep);
    uint_fast8_t ep_orig;

    VSF_HAL_ASSERT((trans->buffer != NULL) && (size <= trans->remain));
    __f1cx00s_usb_read_fifo(usbd->otg, ep, trans->buffer, size);
#if F1CX00S_USBD_TRACE_EN == ENABLED
    vsf_trace_debug("Read EP%d: %d\r\n", ep & 0x0F, size);
#   if F1CX00S_USBD_TRACE_BUFFER_EN == ENABLED
    vsf_trace_buffer(VSF_TRACE_DEBUG, trans->buffer, size);
#   endif
#endif
    trans->remain -= size;
    trans->buffer += size;
    if ((size < ep_size) || !trans->remain) {
        trans->zlp = true;
    }

    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep);
        if (!ep) {
            MUSB_BASE->Index.DC.EP0.CSR0 |= MUSBD_CSR0_ServicedRxPktRdy | (trans->zlp ? MUSBD_CSR0_DataEnd : 0);
#if F1CX00S_USBD_TRACE_EN == ENABLED
            vsf_trace_debug("read_buffer CSR0: %02X\r\n", MUSB_BASE->Index.DC.EP0.CSR0);
#endif
        } else {
            MUSB_BASE->Index.DC.EPN.RxCSRL &= ~MUSBD_RxCSRL_RxPktRdy;
        }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    return VSF_ERR_NONE;
}

vsf_err_t f1cx00s_usbd_ep_transfer_recv(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    VSF_HAL_ASSERT(!(ep & 0x80));

    f1cx00s_usb_dcd_trans_t *trans = __f1cx00s_usbd_get_trans(usbd, ep);
    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = false;

    // TODO: protect out_enable against usb interrupt
    usbd->out_enable |= 1 << (ep & 0x0F);
    return VSF_ERR_NONE;
}

static vsf_err_t __f1cx00s_usbd_ep_transfer_send(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep_idx)
{
    uint_fast8_t ep = ep_idx | 0x80;
    f1cx00s_usb_dcd_trans_t *trans = __f1cx00s_usbd_get_trans(usbd, ep);
    uint_fast16_t ep_size = f1cx00s_usbd_ep_get_size(usbd, ep);
    uint_fast16_t size = min(ep_size, trans->remain);
    uint_fast8_t ep_orig;

    if (size > 0) {
        VSF_HAL_ASSERT(trans->buffer != NULL);
#if F1CX00S_USBD_TRACE_EN == ENABLED
        vsf_trace_debug("Write EP%d: %d\r\n", ep & 0x0F, size);
#   if F1CX00S_USBD_TRACE_BUFFER_EN == ENABLED
        vsf_trace_buffer(VSF_TRACE_DEBUG, trans->buffer, size);
#   endif
#endif
        __f1cx00s_usb_write_fifo(usbd->otg, ep, trans->buffer, size);
        trans->remain -= size;
        trans->buffer += size;
    }

    ep_orig = __f1cx00s_usb_set_ep(usbd->otg, ep_idx);
        if (!ep_idx) {
            uint_fast8_t ep0_int_en = MUSB_BASE->Common.IntrTxE & 1;
            MUSB_BASE->Common.IntrTxE &= ~1;
            usbd->ep0_state = MUSB_USBD_EP0_DATA_IN;
            MUSB_BASE->Index.DC.EP0.CSR0 |= MUSBD_CSR0_TxPktRdy | (size < ep_size ? MUSBD_CSR0_DataEnd : 0);
#if F1CX00S_USBD_TRACE_EN == ENABLED
            vsf_trace_debug("set_data_size CSR0: %02X\r\n", MUSB_BASE->Index.DC.EP0.CSR0);
#endif
            MUSB_BASE->Common.IntrTxE |= ep0_int_en;
        } else {
            MUSB_BASE->Index.DC.EPN.TxCSRL |= MUSBD_TxCSRL_TxPktRdy;
        }
    __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    if (size < ep_size) {
        trans->zlp = false;
    }
    return VSF_ERR_NONE;
}

vsf_err_t f1cx00s_usbd_ep_transfer_send(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    VSF_HAL_ASSERT(ep & 0x80);

    f1cx00s_usb_dcd_trans_t *trans = __f1cx00s_usbd_get_trans(usbd, ep);
    ep &= 0x0F;

    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = zlp;
    return __f1cx00s_usbd_ep_transfer_send(usbd, ep);
}

void f1cx00s_usbd_irq(f1cx00s_usb_dcd_t *usbd)
{
    uint_fast8_t status;
    uint_fast16_t status_rx, status_tx, csr;
    f1cx00s_usb_dcd_trans_t *trans;

    status = MUSB_BASE->Common.IntrUSB;
    status &= MUSB_BASE->Common.IntrUSBE;

    if (status & MUSBD_IntrUSB_Reset) {
        __f1cx00s_usbd_notify(usbd, USB_ON_RESET, 0);
        MUSB_BASE->Common.IntrUSB = MUSBD_IntrUSB_Reset;
    }
//    if (status & MUSB_IntrUSB_SOF) {
//        __f1cx00s_usbd_notify(usb, USB_ON_SOF, 0);
//        MUSB_BASE->Common.IntrUSB = MUSB_IntrUSB_SOF;
//    }
    if (status & MUSB_IntrUSB_Resume) {
        __f1cx00s_usbd_notify(usbd, USB_ON_RESUME, 0);
        MUSB_BASE->Common.IntrUSB = MUSB_IntrUSB_Resume;
    }
    if (status & MUSBD_IntrUSB_Suspend) {
        __f1cx00s_usbd_notify(usbd, USB_ON_SUSPEND, 0);
        MUSB_BASE->Common.IntrUSB = MUSBD_IntrUSB_Suspend;
    }
    if (status & MUSB_IntrUSB_Discon) {
        __f1cx00s_usbd_notify(usbd, USB_ON_DETACH, 0);
        MUSB_BASE->Common.IntrUSB = MUSB_IntrUSB_Discon;
    }

    status_rx = MUSB_BASE->Common.IntrRx;
    status_rx &= MUSB_BASE->Common.IntrRxE;
    status_tx = MUSB_BASE->Common.IntrTx;
    status_tx &= MUSB_BASE->Common.IntrTxE;

    // ep0
    if (status_tx & 1) {
        uint_fast8_t ep_orig = __f1cx00s_usb_set_ep(usbd->otg, 0);

        MUSB_BASE->Common.IntrTx = 1;
        status_tx &= ~1;

        csr = MUSB_BASE->Index.DC.EP0.CSR0;
#if F1CX00S_USBD_TRACE_EN == ENABLED
        vsf_trace_debug("interrupt CSR0: %02X\r\n", csr);
#endif

        switch (usbd->ep0_state) {
        case MUSB_USBD_EP0_IDLE:
            if (csr & MUSBD_CSR0_RxPktRdy) {
            on_setup:
#if F1CX00S_USBD_TRACE_EN == ENABLED
                vsf_trace_debug("ON_SETUP\r\n");
#endif
                __f1cx00s_usbd_notify(usbd, USB_ON_SETUP, 0);
            } else {
                VSF_HAL_ASSERT(false);
            }
            break;
        case MUSB_USBD_EP0_DATA_OUT:
            if (csr & MUSBD_CSR0_RxPktRdy) {
                usbd->has_data_stage = true;
                trans = __f1cx00s_usbd_get_trans(usbd, 0);

                VSF_HAL_ASSERT((trans != NULL) && (trans->buffer != NULL) && !trans->zlp);
                __f1cx00s_usbd_ep_transfer_recv(usbd, 0);
                if (trans->zlp) {
#if F1CX00S_USBD_TRACE_EN == ENABLED
                    vsf_trace_debug("ON_OUT0\r\n");
#endif
                    trans->buffer = NULL;
                    usbd->out_enable &= ~(1 << 0);
                    __f1cx00s_usbd_notify(usbd, USB_ON_OUT, 0);
                }
            } else {
                VSF_HAL_ASSERT(false);
            }
            break;
        case MUSB_USBD_EP0_DATA_IN:
            usbd->has_data_stage = true;
            trans = __f1cx00s_usbd_get_trans(usbd, 0x80);

            if (trans->remain || trans->zlp) {
                __f1cx00s_usbd_ep_transfer_send(usbd, 0);
            } else {
#if F1CX00S_USBD_TRACE_EN == ENABLED
                vsf_trace_debug("ON_IN0\r\n");
#endif
                trans->buffer = NULL;
                __f1cx00s_usbd_notify(usbd, USB_ON_IN, 0);
            }
            break;
        case MUSB_USBD_EP0_STATUS:
            if (csr & MUSBD_CSR0_SetupEnd) {
                // SetupEnd is set when control transact ends before DataEnd has been set
                // so it will run here if transact has no data phase
#if F1CX00S_USBD_TRACE_EN == ENABLED
                vsf_trace_debug("EP0_SetupEnd\r\n");
#endif
                MUSB_BASE->Index.DC.EP0.CSR0 |= MUSBD_CSR0_ServicedSetupEnd;
            }

#if F1CX00S_USBD_TRACE_EN == ENABLED
            vsf_trace_debug("ON_STATUS\r\n");
#endif
            usbd->ep0_state = MUSB_USBD_EP0_IDLE;
            __f1cx00s_usbd_notify(usbd, USB_ON_STATUS, 0);

            // if RxPktRdy is set, next setup has been received
            if (csr & MUSBD_CSR0_RxPktRdy) {
                goto on_setup;
            }
        }

        if (csr & MUSBD_CSR0_SentStall) {
            MUSB_BASE->Index.DC.EP0.CSR0 &= ~MUSBD_CSR0_SentStall;
            usbd->ep0_state = MUSB_USBD_EP0_IDLE;
        }
        __f1cx00s_usb_set_ep(usbd->otg, ep_orig);
    }

    // EPN interrupt
    status_rx &= ~1;
    status_rx &= usbd->out_enable;
    while (status_rx) {
        uint_fast8_t ep_idx = ffz(~status_rx);
        status_rx &= ~(1 << ep_idx);
        MUSB_BASE->Common.IntrRx = 1 << ep_idx;

        // TODO: check csr for SentStall or TxPktRdy
#if F1CX00S_USBD_TRACE_EN == ENABLED
        vsf_trace_debug("interrupt OUT%d\r\n", ep_idx);
#endif

        trans = __f1cx00s_usbd_get_trans(usbd, ep_idx);
        VSF_HAL_ASSERT((trans != NULL) && (trans->buffer != NULL) && !trans->zlp);
        __f1cx00s_usbd_ep_transfer_recv(usbd, ep_idx);
        if (trans->zlp) {
#if F1CX00S_USBD_TRACE_EN == ENABLED
            vsf_trace_debug("ON_OUT%d\r\n", ep_idx);
#endif
            trans->buffer = NULL;
            usbd->out_enable &= ~(1 << ep_idx);
            __f1cx00s_usbd_notify(usbd, USB_ON_OUT, ep_idx);
        }
    }

    while (status_tx) {
        uint_fast8_t ep_idx = ffz(~status_tx);
        status_tx &= ~(1 << ep_idx);
        MUSB_BASE->Common.IntrTx = 1 << ep_idx;

        // TODO: check csr for SentStall or RxPktRdy
#if F1CX00S_USBD_TRACE_EN == ENABLED
        vsf_trace_debug("interrupt IN%d\r\n", ep_idx);
#endif

        trans = __f1cx00s_usbd_get_trans(usbd, ep_idx | 0x80);
        // trans->buffer can be NULL for ZLP
        VSF_HAL_ASSERT(trans != NULL);
        if (trans->remain || trans->zlp) {
            __f1cx00s_usbd_ep_transfer_send(usbd, ep_idx);
        } else {
#if F1CX00S_USBD_TRACE_EN == ENABLED
            vsf_trace_debug("ON_IN%d\r\n", ep_idx);
#endif
            trans->buffer = NULL;
            __f1cx00s_usbd_notify(usbd, USB_ON_IN, ep_idx);
        }
    }
}

#endif      // VSF_USE_USB_DEVICE
