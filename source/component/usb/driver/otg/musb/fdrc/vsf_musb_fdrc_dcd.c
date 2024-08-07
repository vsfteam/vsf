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

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED

#define __VSF_MUSB_FDRC_DCD_CLASS_IMPLEMENT

#include "./vsf_musb_fdrc_dcd.h"
#include "./__vsf_musb_fdrc_common.h"

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
//  for vsf_trace.h
#   include "service/vsf_service.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
#   define __vsf_musb_fdrc_bgtrace_ep0_isr_enter(__csr, __state)                \
        do {                                                                    \
            __vsf_musb_fdrc_bgtrace_ele_t ele = {                               \
                .type       = __MUSBFDRC_BGTRACE_EP0_ISR_ENTER,                 \
                .isr.csr    = (__csr),                                          \
                .isr.state  = (__state),                                        \
            };                                                                  \
            vsf_bgtrace_append(&__vsf_musb_fdrc_bgtrace, &ele);                 \
        } while (0)
#   define __vsf_musb_fdrc_bgtrace_ep0_isr_leave()                              \
        do {                                                                    \
            __vsf_musb_fdrc_bgtrace_ele_t ele = {                               \
                .type       = __MUSBFDRC_BGTRACE_EP0_ISR_LEAVE,                 \
            };                                                                  \
            vsf_bgtrace_append(&__vsf_musb_fdrc_bgtrace, &ele);                 \
        } while (0)
#   define __vsf_musb_fdrc_bgtrace_setup(__ptr)                                 \
        do {                                                                    \
            __vsf_musb_fdrc_bgtrace_ele_t ele = {                               \
                .type       = __MUSBFDRC_BGTRACE_SETUP,                         \
            };                                                                  \
            memcpy(ele.setup, (__ptr), 8);                                      \
            vsf_bgtrace_append(&__vsf_musb_fdrc_bgtrace, &ele);                 \
        } while (0)
#   define __vsf_musb_fdrc_bgtrace_evt(__evt, __value)                          \
        do {                                                                    \
            __vsf_musb_fdrc_bgtrace_ele_t ele = {                               \
                .type       = __MUSBFDRC_BGTRACE_EVT,                           \
                .evt.evt    = (__evt),                                          \
                .evt.value  = (__value),                                        \
            };                                                                  \
            vsf_bgtrace_append(&__vsf_musb_fdrc_bgtrace, &ele);                 \
        } while (0)
#else
#   define __vsf_musb_fdrc_bgtrace_ep0_isr_enter(__csr, __state)
#   define __vsf_musb_fdrc_bgtrace_ep0_isr_leave()
#   define __vsf_musb_fdrc_bgtrace_setup(__ptr)
#   define __vsf_musb_fdrc_bgtrace_evt(__evt, __value)
#endif

// define VSF_USB_ASSERT as below to print trace when asserted
//#define VSF_USB_ASSERT(...)                                                     \
        if (!(__VA_ARGS__)) {                                                   \
            vsf_trace_error("assert failed in %s, %s %d\n", __FILE__, __FUNCTION__, __LINE__);\
            extern void __vsf_musb_fdrc_bgtrace_print(void);                    \
            __vsf_musb_fdrc_bgtrace_print();                                    \
            while(1);                                                           \
        }

/*============================ TYPES =========================================*/

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
typedef enum __vsf_musb_fdrc_bgtrace_type_t {
    __MUSBFDRC_BGTRACE_EP0_ISR_ENTER,
    __MUSBFDRC_BGTRACE_EP0_ISR_LEAVE,
    __MUSBFDRC_BGTRACE_EVT,
    __MUSBFDRC_BGTRACE_SETUP,
} __vsf_musb_fdrc_bgtrace_type_t;

typedef struct __vsf_musb_fdrc_bgtrace_ele_t {
    __vsf_musb_fdrc_bgtrace_type_t type;
    union {
        struct {
            uint8_t csr;
            uint8_t state;
        } isr;
        struct {
            uint8_t evt;
            uint8_t value;
        } evt;
        uint8_t setup[8];
    };
} __vsf_musb_fdrc_bgtrace_ele_t;
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
static void __vsf_musb_fdrc_bgtrace_print_ele(uint16_t pos, void *element);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
static __vsf_musb_fdrc_bgtrace_ele_t __vsf_musb_fdrc_bgtrace_ele[VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE];
static vsf_bgtrace_t __vsf_musb_fdrc_bgtrace = {
    .ele_num        = dimof(__vsf_musb_fdrc_bgtrace_ele),
    .ele_size       = sizeof(__vsf_musb_fdrc_bgtrace_ele[0]),
    .elements       = __vsf_musb_fdrc_bgtrace_ele,
    .print_element  = __vsf_musb_fdrc_bgtrace_print_ele,
};
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
static void __vsf_musb_fdrc_bgtrace_print_ele(uint16_t pos, void *element)
{
    __vsf_musb_fdrc_bgtrace_ele_t *ele = element;
    switch (ele->type) {
    case __MUSBFDRC_BGTRACE_EP0_ISR_ENTER:
        vsf_trace_debug("I0: %02X %d\n", ele->isr.csr, ele->isr.state);
        break;
    case __MUSBFDRC_BGTRACE_EP0_ISR_LEAVE:
        vsf_trace_debug("\n");
        break;
    case __MUSBFDRC_BGTRACE_EVT:
        vsf_trace_debug("E: %d %02X\n", ele->evt.evt, ele->evt.value);
        break;
    case __MUSBFDRC_BGTRACE_SETUP:
        vsf_trace_debug("S: ");
        vsf_trace_buffer(VSF_TRACE_DEBUG, ele->setup, 8, VSF_TRACE_DF_NEWLINE);
        break;
    }
}

void __vsf_musb_fdrc_bgtrace_print(void)
{
    vsf_bgtrace_print(&__vsf_musb_fdrc_bgtrace, 0);
}
#endif

static void __vk_musb_fdrc_usbd_notify(vk_musb_fdrc_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    __vsf_musb_fdrc_bgtrace_evt(evt, value);
    if (usbd->callback.evthandler != NULL) {
        usbd->callback.evthandler(usbd->callback.param, evt, value);
    }
}

static void __vk_musb_fdrc_usbd_reset_do(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;

    reg->Common->Power = 0;
    vk_musb_fdrc_interrupt_init(reg);
    reg->Common->IntrUSBE = MUSB_INTRUSBE_RESET;
    usbd->ep_buf_ptr = 0;
    usbd->out_mask = usbd->in_mask = 0;
    usbd->ep0_state = MUSB_FDRC_USBD_EP0_WAIT_SETUP;
    usbd->is_status_notified = true;
    usbd->is_to_notify_status_in_next_isr = false;
    vk_musb_fdrc_fifo_init(reg);
#if VSF_MUSB_FDRC_DCD_CFG_BGTRACE_SIZE > 0
    vsf_bgtrace_clear(&__vsf_musb_fdrc_bgtrace);
#endif
}

vsf_err_t vk_musb_fdrc_usbd_init(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));
    VSF_USB_ASSERT((usbd->param != NULL) && (usbd->param->op != NULL));

    vk_musb_fdrc_dc_ip_info_t info;
    usbd->param->op->GetInfo(&info.use_as__usb_dc_ip_info_t);
#if defined(VSF_MUSB_FDRC_NO_EP_IDX) || defined(VSF_MUSB_FDRC_NO_HWFIFO)
    usbd->__reg.info = info.use_as__vk_musb_fdrc_reg_info_t;
    usbd->__reg.__cur_ep = 0;
    usbd->reg = &usbd->__reg;
#else
    usbd->reg = info.regbase;
#endif
    usbd->ep_num = info.ep_num;
    usbd->is_dma = info.is_dma;

    usbd->callback.evthandler = cfg->evthandler;
    usbd->callback.param = cfg->param;

    {
        usb_dc_ip_cfg_t ip_cfg = {
            .priority       = cfg->priority,
            .irqhandler     = (usb_ip_irqhandler_t)vk_musb_fdrc_usbd_irq,
            .param          = usbd,
        };
        usbd->param->op->Init(&ip_cfg);
    }

    usbd->is_resetting = false;
    __vk_musb_fdrc_usbd_reset_do(usbd);
    return VSF_ERR_NONE;
}

void vk_musb_fdrc_usbd_fini(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common->IntrUSBE = 0;
    usbd->param->op->Fini();
}

void vk_musb_fdrc_usbd_reset(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    vk_musb_fdrc_usbd_init(usbd, cfg);
}

void vk_musb_fdrc_usbd_connect(vk_musb_fdrc_dcd_t *usbd)
{
    usbd->param->op->Connect();
}

void vk_musb_fdrc_usbd_disconnect(vk_musb_fdrc_dcd_t *usbd)
{
    usbd->param->op->Disconnect();
}

void vk_musb_fdrc_usbd_wakeup(vk_musb_fdrc_dcd_t *usbd)
{
}

void vk_musb_fdrc_usbd_set_address(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t addr)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common->FAddr = addr;
}

uint_fast8_t vk_musb_fdrc_usbd_get_address(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    return reg->Common->FAddr;
}

uint_fast16_t vk_musb_fdrc_usbd_get_frame_number(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    return ((reg->Common->Frame2 & 0x07) << 8) | reg->Common->Frame1;
}

uint_fast8_t vk_musb_fdrc_usbd_get_mframe_number(vk_musb_fdrc_dcd_t *usbd)
{
    return 0;
}

void vk_musb_fdrc_usbd_get_setup(vk_musb_fdrc_dcd_t *usbd, uint8_t *buffer)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    VSF_USB_ASSERT(8 == vk_musb_fdrc_usbd_ep_get_data_size(usbd, 0));
    vk_musb_fdrc_read_fifo(reg, 0, buffer, 8);
    reg->EP->EP0.CSR0 |= MUSBD_CSR0_SERVICEDRXPKGRDY;
    usbd->control_size = buffer[6] + (buffer[7] << 8);
    usbd->is_control_in = !!(buffer[0] & 0x80);
    usbd->is_last_control_in = false;
    __vsf_musb_fdrc_bgtrace_setup(buffer);
}

void vk_musb_fdrc_usbd_status_stage(vk_musb_fdrc_dcd_t *usbd, bool is_in)
{
}

uint_fast8_t vk_musb_fdrc_usbd_ep_get_feature(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature)
{
    return usbd->is_dma ? USB_DC_FEATURE_TRANSFER : 0;
}

vsf_err_t vk_musb_fdrc_usbd_ep_add(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t size_msk;
    uint_fast8_t ep_orig;

    if (size == 1023) { size = 1024; }
    VSF_USB_ASSERT(!(size & 7) && (size >= 8) && (size <= 1024));

    size_msk = vsf_msb32(size);
    if (size & ~(1 << size_msk)) {
        size_msk++;
    }
    size_msk -= 3;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep & 0x0F);
    if (ep != 0) {  // skip EP0_OUT(0x00), should share same fifo as EP0_IN(0x80)
        usbd->ep_buf_ptr += vk_musb_fdrc_set_fifo(reg, ep, type, usbd->ep_buf_ptr, size, size_msk);
    }
    ep &= 0x0F;

    if (is_in) {
        if (!ep) {
            VSF_USB_ASSERT(64 == size);
        } else {
            reg->EP->EPN.TxMAXP = size >> 3;
            reg->EP->EPN.TxCSR2 |= MUSB_TXCSR2_MODE;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                VSF_USB_ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EP->EPN.TxCSR2 |= MUSBD_TXCSR2_ISO;
                reg->EP->EPN.TxCSR2 &= ~MUSB_TXCSR2_FRCDATATOG;
                break;
            case USB_EP_TYPE_BULK:
                reg->EP->EPN.TxCSR2 &= ~(MUSBD_TXCSR2_ISO | MUSB_TXCSR2_FRCDATATOG);
                break;
            case USB_EP_TYPE_INTERRUPT:
                reg->EP->EPN.TxCSR2 &= ~MUSBD_TXCSR2_ISO;
                reg->EP->EPN.TxCSR2 |= MUSB_TXCSR2_FRCDATATOG;
                break;
            }
        }
        vk_musb_fdrc_set_mask(&reg->Common->IntrTx1E, ep);
    } else {
        if (ep != 0) {
            reg->EP->EPN.RxMAXP = size >> 3;
            vk_musb_fdrc_set_mask(&reg->Common->IntrRx1E, ep);
            reg->EP->EPN.RxCSR1 |= MUSBD_RXCSR1_CLRDATATOG;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                VSF_USB_ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EP->EPN.RxCSR2 |= MUSBD_RXCSR2_ISO;
                break;
            default:
                reg->EP->EPN.RxCSR2 &= ~MUSBD_RXCSR2_ISO;
                break;
            }
            reg->EP->EPN.RxCSR1 |= MUSBD_RXCSR1_FLUSHFIFO;
        }
    }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

uint_fast16_t vk_musb_fdrc_usbd_ep_get_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast16_t result;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    if (!ep) {
        return 64;
    }
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        result = is_in ? reg->EP->EPN.TxMAXP : reg->EP->EPN.RxMAXP;
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return result << 3;
}

vsf_err_t vk_musb_fdrc_usbd_ep_set_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            // there seems to be cases that MUSBD_CSR0_SENTSTALL will not be triggered,
            //  so reset status here for next setup
            usbd->is_status_notified = true;
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_WAIT_SETUP;
            reg->EP->EP0.CSR0 |= MUSBD_CSR0_SENDSTALL | MUSBD_CSR0_SERVICEDRXPKGRDY;
        } else {
            if (is_in) {
                reg->EP->EPN.TxCSR1 |= MUSBD_TXCSR1_SENDSTALL;
            } else {
                reg->EP->EPN.RxCSR1 |= MUSBD_RXCSR1_SENDSTALL;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

bool vk_musb_fdrc_usbd_ep_is_stalled(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80, is_stall;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            is_stall = reg->EP->EP0.CSR0 & MUSBD_CSR0_SENDSTALL;
        } else {
            if (is_in) {
                is_stall = reg->EP->EPN.TxCSR1 & MUSBD_TXCSR1_SENDSTALL;
            } else {
                is_stall = reg->EP->EPN.RxCSR1 & MUSBD_RXCSR1_SENDSTALL;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return is_stall > 0;
}

vsf_err_t vk_musb_fdrc_usbd_ep_clear_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            reg->EP->EP0.CSR0 &= ~(MUSBD_CSR0_SENTSTALL | MUSBD_CSR0_SENDSTALL);
        } else {
            if (is_in) {
                reg->EP->EPN.TxCSR1 &= ~(MUSBD_TXCSR1_SENTSTALL | MUSBD_TXCSR1_SENDSTALL);
                reg->EP->EPN.TxCSR1 |= MUSBD_TXCSR1_CLRDATATOG | MUSBD_TXCSR1_FLUSHFIFO;
            } else {
                reg->EP->EPN.RxCSR1 &= ~(MUSBD_RXCSR1_SENTSTALL | MUSBD_RXCSR1_SENDSTALL);
                reg->EP->EPN.RxCSR1 |= MUSBD_RXCSR1_CLRDATATOG | MUSBD_RXCSR1_FLUSHFIFO;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

uint_fast32_t vk_musb_fdrc_usbd_ep_get_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;

    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    return vk_musb_fdrc_rx_fifo_size(reg, ep);
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_read_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg;
    uint_fast8_t ep_orig;

    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    reg = usbd->reg;
    vk_musb_fdrc_read_fifo(reg, ep, buffer, size);

    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            VSF_USB_ASSERT(usbd->control_size >= size);
            usbd->control_size -= size;

            if (!usbd->control_size || (size < 64)) {
                usbd->ep0_state = MUSB_FDRC_USBD_EP0_STATUS;
                usbd->is_to_notify_status_in_next_isr = true;
                reg->EP->EP0.CSR0 |= MUSBD_CSR0_SERVICEDRXPKGRDY | MUSBD_CSR0_DATAEND;
            } else {
                reg->EP->EP0.CSR0 |= MUSBD_CSR0_SERVICEDRXPKGRDY;
            }
        } else {
            reg->EP->EPN.RxCSR1 &= ~MUSBD_RXCSR1_RXPKTRDY;
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_enable_out(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    ep &= 0x0F;

    vsf_protect_t orig = vsf_protect_int();
    if (usbd->out_mask & (1 << ep)) {
        // if ep in out_mask is set, means data is received, simply notify user
        usbd->out_mask &= ~(1 << ep);
        vsf_unprotect_int(orig);
        __vk_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, ep);
    } else {
        usbd->out_mask |= (1 << ep);
        vsf_unprotect_int(orig);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_set_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t ep_orig;

    VSF_USB_ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            VSF_USB_ASSERT(usbd->control_size >= size);
            usbd->control_size -= size;

            usbd->ep0_state = MUSB_FDRC_USBD_EP0_DATA_IN;
            if (!usbd->control_size || (size < 64)) {
                usbd->is_last_control_in = true;
                reg->EP->EP0.CSR0 |= MUSBD_CSR0_TXPKTRDY | MUSBD_CSR0_DATAEND;
            } else {
                reg->EP->EP0.CSR0 |= MUSBD_CSR0_TXPKTRDY;
            }
        } else {
            vsf_protect_t orig = vsf_protect_int();
                reg->EP->EPN.TxCSR1 |= MUSBD_TXCSR1_TXPKTRDY;
                usbd->in_mask |= 1 << ep;
            vsf_unprotect_int(orig);
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_write_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg;

    VSF_USB_ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    reg = usbd->reg;
    vk_musb_fdrc_write_fifo(reg, ep, buffer, size);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transfer_recv(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transfer_send(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

static void __vk_musb_fdrc_usbd_notify_status(vk_musb_fdrc_dcd_t *usbd)
{
    usbd->is_to_notify_status_in_next_isr = false;
    if (!usbd->is_status_notified) {
        usbd->is_status_notified = true;
        __vk_musb_fdrc_usbd_notify(usbd, USB_ON_STATUS, 0);
        usbd->ep0_state = MUSB_FDRC_USBD_EP0_WAIT_SETUP;
    }
}

void vk_musb_fdrc_usbd_irq(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t status, csr1;
    uint_fast16_t status_rx, status_tx;
    vsf_protect_t orig;
    uint_fast8_t ep_orig;

    status = reg->Common->IntrUSB;
    status &= reg->Common->IntrUSBE;

    // USB interrupt

    // Some musb device will trigger reset event many times while D+ and D- are low.
    //  If the last reset event is too close to the first setup event, there maybe hw related problem.
    //  Add is_ressetting so that reset event is only triggered once.
    if (status & MUSB_INTRUSB_RESET) {
        if (!usbd->is_resetting) {
            usbd->is_resetting = true;
            __vk_musb_fdrc_usbd_notify(usbd, USB_ON_RESET, 0);
        }
        reg->Common->Power |= MUSB_POWER_ENABLESUSPEND;
    } else if (status) {
        usbd->is_resetting = false;
    }

    if (status & MUSB_INTRUSB_SOF) {
        __vk_musb_fdrc_usbd_notify(usbd, USB_ON_SOF, 0);
    }
    if (status & MUSB_INTRUSB_RESUME) {
        reg->Common->Power |= MUSB_POWER_ENABLESUSPEND;
        __vk_musb_fdrc_usbd_notify(usbd, USB_ON_RESUME, 0);
    }
    if (status & MUSB_INTRUSB_SUSPEND) {
        __vk_musb_fdrc_usbd_notify(usbd, USB_ON_SUSPEND, 0);
    }

    status_rx = vk_musb_fdrc_get_mask(&reg->Common->IntrRx1);
    status_rx &= vk_musb_fdrc_get_mask(&reg->Common->IntrRx1E);
    status_tx = vk_musb_fdrc_get_mask(&reg->Common->IntrTx1);
    status_tx &= vk_musb_fdrc_get_mask(&reg->Common->IntrTx1E);

    // EP0 interrupt
    if (status_tx & 1) {
        status_tx &= ~1;
        ep_orig = vk_musb_fdrc_set_ep(reg, 0);
        csr1 = reg->EP->EP0.CSR0;

        __vsf_musb_fdrc_bgtrace_ep0_isr_enter(csr1, usbd->ep0_state);
        if (csr1 & MUSBD_CSR0_SENTSTALL) {
            reg->EP->EP0.CSR0 &= ~MUSBD_CSR0_SENTSTALL;
            usbd->is_status_notified = true;
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_WAIT_SETUP;
        }

        if (csr1 & MUSBD_CSR0_SETUPEND) {
            reg->EP->EP0.CSR0 |= MUSBD_CSR0_SERVICEDSETUPEND;
            __vk_musb_fdrc_usbd_notify_status(usbd);
        }
        if (usbd->is_to_notify_status_in_next_isr) {
            __vk_musb_fdrc_usbd_notify_status(usbd);
        }

        vk_musb_fdrc_set_ep(reg, ep_orig);

        // IMPORTANT:
        //         the IN/STATUS_OUT/SETUP of musb has no flow control, so the order of
        //         checking the interrupt flag MUST be as follows:
        //         IN0 -->> STATUS -->> SETUP
        // consider this:
        //         SETUP -->> IN0 -->> STATUS -->> SETUP
        //                    --------------------------
        //        in some condition, the under line interrupt MAYBE in one routine

        // MUSBD_CSR0_TXPKTRDY is cleared by hardware
        if (    (MUSB_FDRC_USBD_EP0_DATA_IN == usbd->ep0_state)
            &&  !(csr1 & MUSBD_CSR0_TXPKTRDY)) {

            __vk_musb_fdrc_usbd_notify(usbd, USB_ON_IN, 0);
            if (usbd->is_last_control_in) {
                // interrupt will be generated to indicate status got
                usbd->is_to_notify_status_in_next_isr = true;
            } else {
                usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
            }
        }

        if (csr1 & MUSBD_CSR0_RXPKTRDY) {
            switch (usbd->ep0_state) {
            case MUSB_FDRC_USBD_EP0_WAIT_SETUP:
                if (!usbd->is_status_notified) {
                    // setup notified, but not processed yet.
                    //  next possible interrupt is data out
                    goto on_data_out;
                }
            on_setup:
                usbd->is_status_notified = false;
                __vk_musb_fdrc_usbd_notify(usbd, USB_ON_SETUP, 0);
                break;
            case MUSB_FDRC_USBD_EP0_DATA_OUT:
            on_data_out:
                orig = vsf_protect_int();
                if (usbd->out_mask & 1) {
                    usbd->out_mask &= ~1;
                    vsf_unprotect_int(orig);
                    __vk_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, 0);
                } else {
                    usbd->out_mask |= 1;
                    vsf_unprotect_int(orig);
                }
                break;
            // if RxPktRdy in DataIn/Status state, means that next setup is arrived
            case MUSB_FDRC_USBD_EP0_DATA_IN:
            case MUSB_FDRC_USBD_EP0_STATUS:
                __vk_musb_fdrc_usbd_notify_status(usbd);
                goto on_setup;
            default:
                VSF_USB_ASSERT(false);
            }
        }
        __vsf_musb_fdrc_bgtrace_ep0_isr_leave();
    }

    // EPN interrupt
    status_rx &= ~1;
    while (status_rx) {
        uint_fast8_t ep = vsf_ffz32(~status_rx);
        status_rx &= ~(1 << ep);

        ep_orig = vk_musb_fdrc_set_ep(reg, ep);
            csr1 = reg->EP->EPN.RxCSR1;
            if (csr1 & MUSBD_RXCSR1_SENTSTALL) {
                reg->EP->EPN.RxCSR1 &= ~MUSBD_RXCSR1_SENDSTALL;
                reg->EP->EPN.RxCSR1 |= MUSBD_RXCSR1_CLRDATATOG;
            }
            if (csr1 & MUSBD_RXCSR1_DATAERROR) {
            }
            if (csr1 & MUSBD_RXCSR1_OVERRUN) {
                reg->EP->EPN.RxCSR1 &= ~MUSBD_RXCSR1_OVERRUN;
            }
        vk_musb_fdrc_set_ep(reg, ep_orig);

        orig = vsf_protect_int();
        if (usbd->out_mask & (1 << ep)) {
            usbd->out_mask &= ~(1 << ep);
            vsf_unprotect_int(orig);
            __vk_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, ep);
        } else {
            usbd->out_mask |= 1 << ep;
            vsf_unprotect_int(orig);
        }
    }

    while (status_tx) {
        uint_fast8_t ep = vsf_ffz32(~status_tx);
        status_tx &= ~(1 << ep);

        ep_orig = vk_musb_fdrc_set_ep(reg, ep);
            csr1 = reg->EP->EPN.TxCSR1;
            if (csr1 & MUSBD_TXCSR1_SENTSTALL) {
                reg->EP->EPN.TxCSR1 &= ~MUSBD_TXCSR1_SENDSTALL;
                reg->EP->EPN.TxCSR1 |= MUSBD_TXCSR1_CLRDATATOG;
            }
            if (csr1 & MUSBD_TXCSR1_UNDERRUN) {
                reg->EP->EPN.TxCSR1 &= ~MUSBD_TXCSR1_UNDERRUN;
            }
        vk_musb_fdrc_set_ep(reg, ep_orig);

        orig = vsf_protect_int();
        if ((usbd->in_mask & (1 << ep)) && !(csr1 & MUSBD_TXCSR1_TXPKTRDY)) {
            usbd->in_mask &= ~(1 << ep);
            vsf_unprotect_int(orig);
            __vk_musb_fdrc_usbd_notify(usbd, USB_ON_IN, ep);
        } else {
            vsf_unprotect_int(orig);
        }
    }
}

#endif
