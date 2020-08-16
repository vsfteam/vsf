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

#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
// ch32f10x_usbhd has no roothub, so need __VSF_USBH_CLASS_IMPLEMENT_HUB__ for
//  vsf_usbh_new_device and vsf_usbh_disconnect_device
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define CH32F10X_USBHD_HCD_CLASS_IMPLEMENT
#include "./usbhd_hcd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef USB_HDC_REG_BASE
#   define __hc32f10x_usbhd_get_reg(__usbhd)        ((hc32f10x_usbhd_reg_t *)USB_HDC_REG_BASE)
#endif

#ifdef USB_HDC_IRQN
#   define __hc32f10x_usbhd_get_irqn(__usbhd)       ((IRQn_Type)USB_HDC_IRQN)
#endif

#ifndef __IO
#   define __IO                                     volatile
#endif

// R8_USB_CTRL
#define RB_UC_HOST_MODE     0x80        // enable USB host mode: 0=device mode, 1=host mode
#define RB_UC_LOW_SPEED     0x40        // enable USB low speed: 0=12Mbps, 1=1.5Mbps
#define RB_UC_INT_BUSY      0x08        // enable automatic responding busy for device mode or automatic pause for host mode during interrupt flag UIF_TRANSFER valid
#define RB_UC_RESET_SIE     0x04        // force reset USB SIE, need software clear
#define RB_UC_CLR_ALL       0x02        // force clear FIFO and count of USB
#define RB_UC_DMA_EN        0x01        // DMA enable and DMA interrupt enable for USB

// R8_UHOST_CTRL
#define RB_UH_LOW_SPEED     0x04        // enable USB port low speed: 0=full speed, 1=low speed
#define RB_UH_BUS_RESET     0x02        // control USB bus reset: 0=normal, 1=force bus reset
#define RB_UH_PORT_EN       0x01        // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached

// R8_UH_EP_MOD
#define RB_UH_EP_TX_EN      0x40        // enable USB host OUT endpoint transmittal
#define RB_UH_EP_TBUF_MOD   0x10        // buffer mode of USB host OUT endpoint
// bUH_EP_TX_EN & bUH_EP_TBUF_MOD: USB host OUT endpoint buffer mode, buffer start address is UH_TX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for transmittal (OUT endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_T_TOG selection for transmittal (OUT endpoint), total=128bytes
#define RB_UH_EP_RX_EN      0x08        // enable USB host IN endpoint receiving
#define RB_UH_EP_RBUF_MOD   0x01        // buffer mode of USB host IN endpoint
// bUH_EP_RX_EN & bUH_EP_RBUF_MOD: USB host IN endpoint buffer mode, buffer start address is UH_RX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (IN endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_R_TOG selection for receiving (IN endpoint), total=128bytes

// R8_UH_SETUP
#define RB_UH_PRE_PID_EN    0x80        // USB host PRE PID enable for low speed device via hub
#define RB_UH_SOF_EN        0x40        // USB host automatic SOF enable

// R8_USB_INT_EN
#define RB_UIE_FIFO_OV      0x10        // enable interrupt for FIFO overflow
#define RB_UIE_HST_SOF      0x08        // enable interrupt for host SOF timer action for USB host mode
#define RB_UIE_SUSPEND      0x04        // enable interrupt for USB suspend or resume event
#define RB_UIE_TRANSFER     0x02        // enable interrupt for USB transfer completion
#define RB_UIE_DETECT       0x01        // enable interrupt for USB device detected event for USB host mode

// R8_USB_INT_FG
#define RB_U_IS_NAK         0x80        // RO, indicate current USB transfer is NAK received
#define RB_U_TOG_OK         0x40        // RO, indicate current USB transfer toggle is OK
#define RB_U_SIE_FREE       0x20        // RO, indicate USB SIE free status
#define RB_UIF_FIFO_OV      0x10        // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
#define RB_UIF_HST_SOF      0x08        // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
#define RB_UIF_SUSPEND      0x04        // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
#define RB_UIF_TRANSFER     0x02        // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
#define RB_UIF_DETECT       0x01        // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear

// R8_USB_INT_ST
#define RB_UIS_TOG_OK       0x40        // RO, indicate current USB transfer toggle is OK
#define MASK_UIS_H_RES      0x0F        // RO, bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received

// R8_UH_RX_CTRL
#define RB_UH_R_TOG         0x80        // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define RB_UH_R_AUTO_TOG    0x10        // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle

// R8_UH_TX_CTRL
#define RB_UH_T_TOG         0x40        // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define RB_UH_T_AUTO_TOG    0x10        // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle

// R8_USB_MIS_ST
#define RB_UMS_DM_LEVEL     0x02        // RO, indicate UDM level saved at device attached to USB host
#define RB_UMS_DEV_ATTACH   0x01        // RO, indicate device attached status on USB host

/*============================ TYPES =========================================*/

typedef struct hc32f10x_usbhd_reg_t {
    __IO uint8_t    R8_USB_CTRL;        /* offset 0x00 */
    __IO uint8_t    R8_UHOST_CTRL;      /* offset 0x01 */
    __IO uint8_t    R8_USB_INT_EN;      /* offset 0x02 */
    __IO uint8_t    R8_USB_DEV_AD;      /* offset 0x03 */
    __IO uint8_t    __DUMMY0;
    __IO uint8_t    R8_USB_MIS_ST;      /* offset 0x05 */
    __IO uint8_t    R8_USB_INT_FG;      /* offset 0x06 */
    __IO uint8_t    R8_USB_INT_ST;      /* offset 0x07 */
    __IO uint8_t    R8_USB_RX_LEN;      /* offset 0x08 */
    __IO uint8_t    __DUMMY1[4];
    __IO uint8_t    R8_UH_EP_MOD;       /* offset 0x0D */
    __IO uint8_t    __DUMMY2[10];
    __IO uint16_t   R16_UH_RX_DMA;      /* offset 0x18 */
    __IO uint8_t    __DUMMY3[2];
    __IO uint16_t   R16_UH_TX_DMA;      /* offset 0x1C */
    __IO uint8_t    __DUMMY4[8];
    __IO uint8_t    R8_UH_SETUP;        /* offset 0x26 */
    __IO uint8_t    __DUMMY5[1];
    __IO uint8_t    R8_UH_EP_PID;       /* offset 0x28 */
    __IO uint8_t    __DUMMY6[1];
    __IO uint8_t    R8_UH_RX_CTRL;      /* offset 0x2A */
    __IO uint8_t    __DUMMY7[1];
    __IO uint8_t    R8_UH_TX_LEN;       /* offset 0x2C */
    __IO uint8_t    __DUMMY8[1];
    __IO uint8_t    R8_UH_TX_CTRL;      /* offset 0x2E */
    __IO uint8_t    __DUMMY9[9];
    __IO uint8_t    R8_USB_TYPE_C_CTRL; /* offset 0x38 */
} hc32f10x_usbhd_reg_t;

struct hc32f10x_usbhd_urb_t {
    implement(vk_usbh_hcd_urb_t)
    vsf_dlist_node_t urb_node;
    enum {
        HC32F10X_USBHD_URB_STATE_START_SUBMITTING,
        HC32F10X_USBHD_URB_STATE_SETUP,
        HC32F10X_USBHD_URB_STATE_SETUP_DATA,
        HC32F10X_USBHD_URB_STATE_SETUP_STATUS,
        HC32F10X_USBHD_URB_STATE_TO_FREE,
    } state;
};
typedef struct hc32f10x_usbhd_urb_t hc32f10x_usbhd_urb_t;

enum {
    VSF_HC32F10X_HCD_EVT_CONN_CHANGE= VSF_EVT_USER + 0,
    VSF_HC32F10X_HCD_EVT_TRANSFER   = VSF_EVT_USER + 1,
};

enum {
    USB_ERR_HCD_NAK                 = VSF_ERR_NOT_READY + 1,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __hc32f10x_usbhd_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __hc32f10x_usbhd_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __hc32f10x_usbhd_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __hc32f10x_usbhd_hcd_resume(vk_usbh_hcd_t *hcd);
static vk_usbh_hcd_urb_t * __hc32f10x_usbhd_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __hc32f10x_usbhd_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __hc32f10x_usbhd_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __hc32f10x_usbhd_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __hc32f10x_usbhd_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __hc32f10x_usbhd_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t hc32f10x_usbhd_hcd_drv = {
    .init_evthandler    = __hc32f10x_usbhd_hcd_init_evthandler,
    .fini               = __hc32f10x_usbhd_hcd_fini,
    .suspend            = __hc32f10x_usbhd_hcd_suspend,
    .resume             = __hc32f10x_usbhd_hcd_resume,
    .alloc_urb          = __hc32f10x_usbhd_hcd_alloc_urb,
    .free_urb           = __hc32f10x_usbhd_hcd_free_urb,
    .submit_urb         = __hc32f10x_usbhd_hcd_submit_urb,
    .relink_urb         = __hc32f10x_usbhd_hcd_relink_urb,

    .reset_dev          = __hc32f10x_usbhd_hcd_reset_dev,
    .is_dev_reset       = __hc32f10x_usbhd_hcd_is_dev_reset,
};

#ifdef USB_HDC_IRQHandler
static hc32f10x_usbhd_hcd_t *__hc32f10x_usbhd_hcd;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef USB_HDC_REG_BASE
static hc32f10x_usbhd_reg_t * __hc32f10x_usbhd_get_reg(hc32f10x_usbhd_const_t *param)
{
    return (hc32f10x_usbhd_reg_t *)param->reg;
}
#endif

#ifndef USB_HDC_IRQN
static IRQn_Type __hc32f10x_usbhd_get_irqn(hc32f10x_usbhd_const_t *param)
{
    return param->irq;
}
#endif

static vsf_err_t __hc32f10x_usbhd_hcd_urb_fsm(hc32f10x_usbhd_hcd_t *usbhd_hcd, hc32f10x_usbhd_urb_t *urb)
{
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd->param);
    vk_usbh_pipe_t pipe = urb->pipe;
    uint_fast8_t is_in = pipe.dir_in1out0;
    uint_fast8_t pid = is_in ? USB_PID_IN : USB_PID_OUT;
    uint8_t *buffer;
    uint_fast8_t len, rxlen;

    buffer = usbhd_hcd->cur_buffer;
    len = urb->transfer_length;
    switch (urb->state) {
    case HC32F10X_USBHD_URB_STATE_START_SUBMITTING:
        buffer = usbhd_hcd->cur_buffer = urb->buffer;
        urb->actual_length = 0;
        urb->status = VSF_ERR_NONE;

        reg->R8_USB_DEV_AD = pipe.address;
        if (0 == pipe.endpoint) {
            is_in = false;
            pid = USB_PID_SETUP;
            len = sizeof(urb->setup_packet);
            buffer = (uint8_t *)&urb->setup_packet;
            reg->R8_UH_TX_CTRL = 0;
            urb->state = HC32F10X_USBHD_URB_STATE_SETUP;
            break;
        }
        // fall through
    case HC32F10X_USBHD_URB_STATE_SETUP:
    setup_data:
        if ((0 == pipe.endpoint) && (0 == len)) {
            goto setup_status;
        }
        if (urb->buffer == usbhd_hcd->cur_buffer) {
            if (0 == pipe.endpoint) {
                // EP0 start from DATA1 every time
                reg->R8_UH_TX_CTRL = reg->R8_UH_RX_CTRL = RB_UH_R_TOG | RB_UH_R_AUTO_TOG | RB_UH_T_TOG | RB_UH_T_AUTO_TOG;
            } else {
                uint_fast8_t ep_mask = 1 << pipe.endpoint;
                if (pipe.dir_in1out0) {
                    reg->R8_UH_RX_CTRL = RB_UH_R_AUTO_TOG | ((usbhd_hcd->dev->toggle[1] & ep_mask) ? RB_UH_R_TOG : 0);
                } else {
                    reg->R8_UH_TX_CTRL = RB_UH_T_AUTO_TOG | ((usbhd_hcd->dev->toggle[0] & ep_mask) ? RB_UH_T_TOG : 0);
                }
            }
        }
        urb->state = HC32F10X_USBHD_URB_STATE_SETUP_DATA;
        if (len > pipe.size) {
            len = pipe.size;
        }
        usbhd_hcd->cur_size = len;
        break;
    case HC32F10X_USBHD_URB_STATE_SETUP_DATA:
        if (!(reg->R8_USB_INT_ST & RB_UIS_TOG_OK)) {
            switch (reg->R8_USB_INT_ST & MASK_UIS_H_RES) {
            case 0x00:
            case (USB_PID_STALL & 0x0F):
                return VSF_ERR_FAIL;
            case (USB_PID_NAK & 0x0F):
                if (pipe.endpoint != 0) {
                    return USB_ERR_HCD_NAK;
                }
                goto setup_data;
                break;
            default:
                VSF_USB_ASSERT(false);
            }
        }

        if (is_in) {
            rxlen = reg->R8_USB_RX_LEN;
            if (rxlen > usbhd_hcd->cur_size) {
                VSF_USB_ASSERT(false);
                // TODO: process overflow
                rxlen = urb->transfer_length;
            }
            memcpy(usbhd_hcd->cur_buffer, usbhd_hcd->ep_buffer, rxlen);
            usbhd_hcd->cur_buffer = (uint8_t *)usbhd_hcd->cur_buffer + rxlen;
            urb->transfer_length -= rxlen;
            urb->actual_length += rxlen;
        } else {
            usbhd_hcd->cur_buffer = (uint8_t *)usbhd_hcd->cur_buffer + usbhd_hcd->cur_size;
            urb->transfer_length -= usbhd_hcd->cur_size;
            urb->actual_length += usbhd_hcd->cur_size;
        }
        len = urb->transfer_length;

    setup_status:
        if ((len > 0) && (!is_in || (rxlen >= pipe.size))) {
            goto setup_data;
        } else if (!is_in && (urb->transfer_flags & URB_ZERO_PACKET)) {
            urb->transfer_flags &= ~URB_ZERO_PACKET;
            break;
        } else if (0 == pipe.endpoint) {
            pid = is_in ? USB_PID_OUT : USB_PID_IN;
            reg->R8_UH_TX_CTRL = reg->R8_UH_RX_CTRL = RB_UH_R_TOG | RB_UH_T_TOG;
            len = 0;
            is_in = false;
            urb->state = HC32F10X_USBHD_URB_STATE_SETUP_STATUS;
            break;
        }
        // fall through
    case HC32F10X_USBHD_URB_STATE_SETUP_STATUS:
        if (pipe.endpoint != 0) {
            uint_fast8_t ep_mask = 1 << pipe.endpoint;
            if (pipe.dir_in1out0) {
                usbhd_hcd->dev->toggle[1] &= ~ep_mask;
                usbhd_hcd->dev->toggle[1] |= (reg->R8_UH_RX_CTRL & 0x80) ? ep_mask : 0;
            } else {
                usbhd_hcd->dev->toggle[0] &= ~ep_mask;
                usbhd_hcd->dev->toggle[0] |= (reg->R8_UH_TX_CTRL & 0x80) ? ep_mask : 0;
            }
        }
        return VSF_ERR_NONE;
    }

    if (is_in) {
    } else {
        memcpy(usbhd_hcd->ep_buffer, buffer, len);
        reg->R8_UH_TX_LEN = len;
    }
    reg->R8_UH_EP_PID = (pid << 4) | pipe.endpoint;
    reg->R8_USB_INT_FG = RB_UIF_TRANSFER;
    usbhd_hcd->is_transacting = true;
    return VSF_ERR_NOT_READY;
}

static void __hc32f10x_usbhd_hcd_execute_urb(hc32f10x_usbhd_hcd_t *usbhd, hc32f10x_usbhd_urb_t *urb)
{
    urb->state = HC32F10X_USBHD_URB_STATE_START_SUBMITTING;
    __hc32f10x_usbhd_hcd_urb_fsm(usbhd, urb);
}

static void __hc32f10x_usbhd_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    hc32f10x_usbhd_hcd_t *usbhd_hcd = container_of(eda, hc32f10x_usbhd_hcd_t, teda);
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd->param);

    switch (evt) {
    case VSF_EVT_INIT:
        usbhd_hcd->state = HC32F10X_HCD_STATE_WAIT_CONNECT;
        break;
    case VSF_EVT_TIMER:
        switch (usbhd_hcd->state) {
        case HC32F10X_HCD_STATE_WAIT_RESET:
            reg->R8_UHOST_CTRL &= ~RB_UH_BUS_RESET;
            usbhd_hcd->state = HC32F10X_HCD_STATE_WAIT_RESET_CLEAR;
            vsf_teda_set_timer_ms(20);
            break;
        case HC32F10X_HCD_STATE_WAIT_RESET_CLEAR:
            reg->R8_UHOST_CTRL |= RB_UH_PORT_EN;
            usbhd_hcd->state = HC32F10X_HCD_STATE_CONNECTED;
            usbhd_hcd->dev = vk_usbh_new_device((vk_usbh_t *)usbhd_hcd->hcd,
                    reg->R8_USB_MIS_ST & RB_UMS_DM_LEVEL ? USB_SPEED_LOW : USB_SPEED_FULL,
                    NULL, 0);
            reg->R8_USB_INT_EN = RB_UIE_DETECT;
            break;
        }
        break;
    case VSF_EVT_MESSAGE:
        vsf_usbh_free(vsf_eda_get_cur_msg());
        break;
    case VSF_HC32F10X_HCD_EVT_CONN_CHANGE: {
            bool is_connected = reg->R8_USB_MIS_ST & RB_UMS_DEV_ATTACH;
            if ((HC32F10X_HCD_STATE_WAIT_CONNECT == usbhd_hcd->state) && is_connected) {
                reg->R8_UHOST_CTRL &= ~RB_UH_PORT_EN;
                if (reg->R8_USB_MIS_ST & RB_UMS_DM_LEVEL) {
                    // LowSpeed
                    reg->R8_USB_CTRL |= RB_UC_LOW_SPEED;
                    reg->R8_UHOST_CTRL |= RB_UH_LOW_SPEED;
                } else {
                    // FullSpeed
                    reg->R8_USB_CTRL &= ~RB_UC_LOW_SPEED;
                    reg->R8_UHOST_CTRL &= ~RB_UH_LOW_SPEED;
                }
                reg->R8_UHOST_CTRL |= RB_UH_BUS_RESET;
                usbhd_hcd->state = HC32F10X_HCD_STATE_WAIT_RESET;
                vsf_teda_set_timer_ms(20);
            } else if ((HC32F10X_HCD_STATE_CONNECTED == usbhd_hcd->state) && !is_connected) {
                vk_usbh_disconnect_device((vk_usbh_t *)usbhd_hcd->hcd, usbhd_hcd->dev);
                usbhd_hcd->dev = NULL;
                usbhd_hcd->state = HC32F10X_HCD_STATE_WAIT_CONNECT;
                reg->R8_USB_INT_EN = RB_UIE_DETECT;
            } else {
                // VSF_HC32F10X_HCD_EVT_CONN_CHANGE will not be triggered on startup
                //  but will be triggered later(when the first transaction done)
                reg->R8_USB_INT_EN |= RB_UIE_DETECT;
            }
        }
        break;
    case VSF_HC32F10X_HCD_EVT_TRANSFER: {
            hc32f10x_usbhd_urb_t *urb = usbhd_hcd->urb_cur;
            if (HC32F10X_USBHD_URB_STATE_TO_FREE == urb->state) {
                vsf_dlist_peek_next(hc32f10x_usbhd_urb_t, urb_node, urb, usbhd_hcd->urb_cur);
                vsf_dlist_remove(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, urb);
                vsf_usbh_free(urb);
            } else {
                vsf_err_t err = __hc32f10x_usbhd_hcd_urb_fsm(usbhd_hcd, urb);
                if (VSF_ERR_NOT_READY == err) {
                    break;
                } else if (USB_ERR_HCD_NAK == err) {
                    vsf_dlist_peek_next(hc32f10x_usbhd_urb_t, urb_node, urb, usbhd_hcd->urb_cur);
                } else {
                    urb->status = err;
                    vsf_dlist_peek_next(hc32f10x_usbhd_urb_t, urb_node, urb, usbhd_hcd->urb_cur);
                    vsf_dlist_remove(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, urb);
                    vsf_eda_post_msg(urb->eda_caller, urb);
                }
            }
            if (usbhd_hcd->urb_cur != NULL) {
                __hc32f10x_usbhd_hcd_execute_urb(usbhd_hcd, (hc32f10x_usbhd_urb_t *)usbhd_hcd->urb_cur);
            } else {
                reg->R8_USB_INT_EN &= ~RB_UIE_TRANSFER;
            }
        }
        break;
    }
}

static vsf_err_t __hc32f10x_usbhd_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    hc32f10x_usbhd_hcd_t *usbhd_hcd = (hc32f10x_usbhd_hcd_t *)hcd->param;
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd);

    switch (evt) {
    case VSF_EVT_INIT:
        __hc32f10x_usbhd_hcd= usbhd_hcd;
        usbhd_hcd->hcd      = hcd;

        EXTEN->EXTEN_CTR    |= EXTEN_USBHD_IO_EN;
        switch (CH32F10X_SYS_FREQ_HZ) {
        case 72UL * 1000 * 1000:
            RCC->CFGR0 &= ~RCC_USBPRE;
            break;
        case 48UL * 1000 * 1000:
            RCC->CFGR0 |= RCC_USBPRE;
            break;
        default:
            VSF_HAL_ASSERT(false);
            return VSF_ERR_INVALID_PARAMETER;
        }
        RCC->AHBPCENR |= RCC_USBHD;

        reg->R8_USB_CTRL    = RB_UC_HOST_MODE;
        reg->R8_UHOST_CTRL  = 0;
        reg->R8_UH_EP_MOD   = RB_UH_EP_TX_EN | RB_UH_EP_RX_EN;
        reg->R16_UH_RX_DMA  = (uint16_t)&usbhd_hcd->ep_buffer;
        reg->R16_UH_TX_DMA  = (uint16_t)&usbhd_hcd->ep_buffer;

        reg->R8_UH_RX_CTRL  = 0x00;
        reg->R8_UH_TX_CTRL  = 0x00;
        reg->R8_USB_CTRL    = RB_UC_HOST_MODE | RB_UC_INT_BUSY | RB_UC_DMA_EN;
        reg->R8_UH_SETUP    = RB_UH_SOF_EN;
        reg->R8_USB_INT_FG  = 0xFF;
        reg->R8_USB_INT_EN  = 0;

        // TODO: enable nvic
        // priotiry of hw interrupt MUST be >= priority of current task
        NVIC_SetPriority(__hc32f10x_usbhd_get_irqn(usbd), usbhd_hcd->priority);
        NVIC_EnableIRQ(__hc32f10x_usbhd_get_irqn(usbd));

        usbhd_hcd->teda.fn.evthandler = __hc32f10x_usbhd_hcd_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
        usbhd_hcd->teda.on_terminate = NULL;
#endif
        vsf_teda_init(&usbhd_hcd->teda, vsf_prio_inherit, false);

        // is device is already connected, send event
        if (reg->R8_USB_MIS_ST & RB_UMS_DEV_ATTACH) {
            vsf_eda_post_evt(&usbhd_hcd->teda.use_as__vsf_eda_t, VSF_HC32F10X_HCD_EVT_CONN_CHANGE);
        } else {
            reg->R8_USB_INT_EN = RB_UIE_DETECT;
        }
        return VSF_ERR_NONE;
//        break;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __hc32f10x_usbhd_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __hc32f10x_usbhd_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __hc32f10x_usbhd_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vk_usbh_hcd_urb_t * __hc32f10x_usbhd_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    vk_usbh_hcd_urb_t *usbhd_urb = vsf_usbh_malloc(sizeof(hc32f10x_usbhd_urb_t));
    if (usbhd_urb != NULL) {
        memset(usbhd_urb, 0, sizeof(hc32f10x_usbhd_urb_t));
    }
    return usbhd_urb;
}

static void __hc32f10x_usbhd_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    hc32f10x_usbhd_hcd_t *usbhd_hcd = (hc32f10x_usbhd_hcd_t *)hcd->param;
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd);

    uint_fast8_t orig = reg->R8_USB_INT_EN & RB_UIE_TRANSFER;
    reg->R8_USB_INT_EN &= ~RB_UIE_TRANSFER;
        hc32f10x_usbhd_urb_t *usbhd_urb = (hc32f10x_usbhd_urb_t *)usbhd_hcd->urb_cur;
        if (&usbhd_urb->use_as__vk_usbh_hcd_urb_t == urb) {
            usbhd_urb->state = HC32F10X_USBHD_URB_STATE_TO_FREE;
        } else {
            vsf_dlist_remove(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, (hc32f10x_usbhd_urb_t *)urb);
            vsf_eda_post_msg(&usbhd_hcd->teda.use_as__vsf_eda_t, urb);
        }
    reg->R8_USB_INT_EN |= orig;
}

static vsf_err_t __hc32f10x_usbhd_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    hc32f10x_usbhd_hcd_t *usbhd_hcd = (hc32f10x_usbhd_hcd_t *)hcd->param;
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd);

    uint_fast8_t orig = reg->R8_USB_INT_EN & RB_UIE_TRANSFER;
    reg->R8_USB_INT_EN &= ~RB_UIE_TRANSFER;
        vsf_dlist_add_to_tail(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, (hc32f10x_usbhd_urb_t *)urb);
    reg->R8_USB_INT_EN |= RB_UIE_TRANSFER;

    if (!orig) {
        vsf_dlist_peek_head(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, usbhd_hcd->urb_cur);
        __hc32f10x_usbhd_hcd_execute_urb(usbhd_hcd, (hc32f10x_usbhd_urb_t *)usbhd_hcd->urb_cur);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __hc32f10x_usbhd_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __hc32f10x_usbhd_hcd_submit_urb(hcd, urb);
}

static vsf_err_t __hc32f10x_usbhd_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    // TODO:
    return VSF_ERR_NONE;
}

static bool __hc32f10x_usbhd_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    // TODO:
    return false;
}

static void __hc32f10x_usbhd_hcd_isr(void *param)
{
    hc32f10x_usbhd_hcd_t *usbhd_hcd = param;
    hc32f10x_usbhd_reg_t *reg = __hc32f10x_usbhd_get_reg(usbhd_hcd);

    uint_fast8_t status = reg->R8_USB_INT_EN & reg->R8_USB_INT_FG;
    if (status & RB_UIF_DETECT) {
        reg->R8_USB_INT_FG = RB_UIF_DETECT;
        reg->R8_USB_INT_EN &= ~RB_UIE_DETECT;
        vsf_eda_post_evt(&usbhd_hcd->teda.use_as__vsf_eda_t, VSF_HC32F10X_HCD_EVT_CONN_CHANGE);
    }
    if (status & RB_UIF_TRANSFER) {
        // seems RB_UIF_TRANSFER will be triggered twice in the first run
        //  is_transacting is used to skip unwanted interrupt
        if (usbhd_hcd->is_transacting) {
            usbhd_hcd->is_transacting = false;
            vsf_eda_post_evt(&usbhd_hcd->teda.use_as__vsf_eda_t, VSF_HC32F10X_HCD_EVT_TRANSFER);
        }
    }
    if (status & RB_UIF_HST_SOF) {
        VSF_USB_ASSERT(NULL == usbhd_hcd->urb_cur);
        reg->R8_USB_INT_EN |= RB_UIE_TRANSFER;
        vsf_dlist_peek_head(hc32f10x_usbhd_urb_t, urb_node, &usbhd_hcd->urb_list, usbhd_hcd->urb_cur);
        __hc32f10x_usbhd_hcd_execute_urb(usbhd_hcd, (hc32f10x_usbhd_urb_t *)usbhd_hcd->urb_cur);
    }
}

#ifdef USB_HDC_IRQHandler
ROOT void USB_HDC_IRQHandler(void)
{
    __hc32f10x_usbhd_hcd_isr(__hc32f10x_usbhd_hcd);
}
#endif

#endif      // VSF_USE_USB_HOST
