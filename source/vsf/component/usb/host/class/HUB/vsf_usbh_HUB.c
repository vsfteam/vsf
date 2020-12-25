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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HUB == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_HUB.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_usbh_hub_t {
    vsf_teda_t teda;

    vk_usbh_t *usbh;
    vk_usbh_dev_t *dev;
    vk_usbh_ifs_t *ifs;

    struct usb_hub_desc_t desc_hub;
    //struct usb_hub_status_t hub_status;
    struct usb_port_status_t hub_portsts;

    enum {
        HUB_STAT_ENUM_START,
        HUB_STAT_ENUM_WAIT_HUB_DESC_LEN,
        HUB_STAT_ENUM_WAIT_HUB_DESC,
        HUB_STAT_ENUM_WAIT_PORT_POWER_ON,
        HUB_STAT_ENUM_WAIT_PORT_POWER_ON_STABLE,
        HUB_STAT_ENUM_END,
        HUB_STAT_SCAN_START = HUB_STAT_ENUM_END,
        HUB_STAT_SCAN_WAIT_GET_PORT_STATUS,
        HUB_STAT_SCAN_WAIT_NEXT_ROUND,
        HUB_STAT_SCAN_END,
        HUB_STAT_RESET_CHILD_START = HUB_STAT_SCAN_END,
        HUB_STAT_RESET_CHILD_WAIT_PORT_RESET,
        HUB_STAT_RESET_CHILD_WAIT_PORT_RESET_STABLE,
        HUB_STAT_RESET_CHILD_WAIT_PORT_CLEAR_RESET,
        HUB_STAT_RESET_CHILD_WAIT_PORT_CLEAR_RESET_STABLE,
        HUB_STAT_RESET_CHILD_WAIT_GET_PORT_STATUS,
        HUB_STAT_RESET_CHILD_WAIT_NEXT_RETRY,
        HUB_STAT_CONNECT_START,
        HUB_STAT_CONNECT_WAIT_PORT_CLEAR_CONNECT_CHANGE,
        HUB_STAT_CONNECT_WAIT_PORT_STABLE,
        HUB_STAT_CONNECT_WAIT_PORT_RESET_STABLE,

        // standalone state
        HUB_STAT_SCAN_DELAY_FOR_RESCAN_CURRENT_PORT,
    } state;

    uint8_t is_running              : 1;
    uint8_t is_child_connecting     : 1;
    uint8_t is_waiting_next_round   : 1;
    uint8_t is_go_on_next_round     : 1;
    uint8_t retry                   : 4;
    uint8_t cur_dev_idx;            /* start from 1 */
    uint16_t reset_mask;
} vk_usbh_hub_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_hub_dev_id[] = {
    {
        .match_ifs_class = 1,
        .bInterfaceClass = USB_CLASS_HUB,
    },
};

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_hub_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_hub_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_hub_drv = {
    .name       = "hub",
    .dev_id_num = dimof(__vk_usbh_hub_dev_id),
    .dev_ids    = __vk_usbh_hub_dev_id,
    .probe      = __vk_usbh_hub_probe,
    .disconnect = __vk_usbh_hub_disconnect,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vk_usbh_hub_t *__vk_usbh_hub_dev_gethub(vk_usbh_dev_t *dev_hub)
{
    vk_usbh_hub_t *hub = NULL;
    vk_usbh_ifs_t *ifs = dev_hub->ifs;

    for (int i = 0; i < dev_hub->num_of_ifs; i++, ifs++) {
        if (ifs->drv == &vk_usbh_hub_drv) {
            hub = ifs->param;
            break;
        }
    }
    return hub;
}

static vk_usbh_dev_t *__vk_usbh_hub_getdev(vk_usbh_hub_t *hub, uint_fast8_t index)
{
    vk_usbh_dev_t *dev = hub->dev->children_list.head;
    do {
        if (dev->index == index) {
            return dev;
        }
        vsf_slist_peek_next(vk_usbh_dev_t, child_node, &dev->child_node, dev);
    } while (dev != NULL);
    return NULL;
}

static vsf_err_t __vk_usbh_hub_set_port_feature(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        uint_fast16_t port, uint_fast16_t feature)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_OTHER | USB_DIR_OUT,
        .bRequest        =  USB_REQ_SET_FEATURE,
        .wValue          =  feature,
        .wIndex          =  port,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
static vsf_err_t __vk_usbh_hub_get_port_status(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        uint_fast16_t port)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_OTHER | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_STATUS,
        .wValue          =  0,
        .wIndex          =  port,
        .wLength         =  sizeof(struct usb_port_status_t),
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
static vsf_err_t __vk_usbh_hub_clear_port_feature(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        uint_fast16_t port, uint_fast16_t feature)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_OTHER | USB_DIR_OUT,
        .bRequest        =  USB_REQ_CLEAR_FEATURE,
        .wValue          =  feature,
        .wIndex          =  port,
        .wLength         =  0,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}

/*
static vsf_err_t __vk_usbh_hub_get_status(vk_usbh_t *usbh, vk_usbh_dev_t *dev)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_STATUS,
        .wValue          =  0,
        .wIndex          =  0,
        .wLength         =  sizeof(struct usb_hub_status_t),
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}
*/

static vsf_err_t __vk_usbh_hub_get_descriptor(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
        uint_fast16_t size)
{
    struct usb_ctrlrequest_t req = {
        .bRequestType    =  USB_TYPE_CLASS | USB_RECIP_DEVICE | USB_DIR_IN,
        .bRequest        =  USB_REQ_GET_DESCRIPTOR,
        .wValue          =  USB_DT_HUB << 8,
        .wIndex          =  0,
        .wLength         =  size,
    };
    return vk_usbh_control_msg(usbh, dev, &req);
}

static void __vk_usbh_hub_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_hub_t *hub = (vk_usbh_hub_t *)eda;
    vk_usbh_dev_t *dev = hub->dev;
    vk_usbh_urb_t *urb = &dev->ep0.urb;
    vsf_err_t err = VSF_ERR_NONE;
    bool retain_state = false;

    switch (evt) {
    case VSF_EVT_INIT:
        hub->state = HUB_STAT_ENUM_START;
        vk_usbh_urb_set_buffer(urb, &hub->desc_hub, 4);
        err = __vk_usbh_hub_get_descriptor(hub->usbh, dev, 4);
        break;
    case VSF_EVT_MESSAGE:
        if (vk_usbh_urb_get_status(urb) != URB_OK) { goto fail; }

        switch (hub->state) {
        default:
            VSF_USB_ASSERT(false);
            break;
        case HUB_STAT_ENUM_WAIT_HUB_DESC_LEN:
            vk_usbh_urb_set_buffer(urb, &hub->desc_hub, hub->desc_hub.bDescLength);
            if (hub->desc_hub.bDescLength > sizeof(hub->desc_hub)) {
                goto fail;
            }
            err = __vk_usbh_hub_get_descriptor(hub->usbh, dev, hub->desc_hub.bDescLength);
            break;
        case HUB_STAT_ENUM_WAIT_HUB_DESC:
            dev->maxchild = hub->desc_hub.bNbrPorts;
            hub->cur_dev_idx = 0;

        enum_set_next_power_on:
            hub->cur_dev_idx++;
            err = __vk_usbh_hub_set_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                    USB_PORT_FEAT_POWER);
            break;
        case HUB_STAT_ENUM_WAIT_PORT_POWER_ON:
            if (0 == hub->desc_hub.bPwrOn2PwrGood) {
                hub->desc_hub.bPwrOn2PwrGood = 1;   // use a minimum delay
            }
            vsf_teda_set_timer_ms(hub->desc_hub.bPwrOn2PwrGood * 2);
            break;
        case HUB_STAT_RESET_CHILD_WAIT_PORT_RESET:
            // delay 20ms after port reset
        case HUB_STAT_RESET_CHILD_WAIT_PORT_CLEAR_RESET:
            vsf_teda_set_timer_ms(20);
            break;
        case HUB_STAT_RESET_CHILD_WAIT_GET_PORT_STATUS:
            // check port status after reset
            if (hub->hub_portsts.wPortStatus & USB_PORT_STAT_ENABLE) {
                hub->reset_mask &= ~(1 << (hub->cur_dev_idx - 1));

                // reset ready
                if (hub->is_child_connecting) {
                    // reset child while connecting
                    hub->state = HUB_STAT_CONNECT_WAIT_PORT_RESET_STABLE;
                    vsf_teda_set_timer_ms(200);
                    retain_state = true;
                } else {
                    // reset child while scanning
                    hub->state = HUB_STAT_SCAN_WAIT_GET_PORT_STATUS;
                    goto scan_check_port_status;
                }
            } else if (++hub->retry > 3) {
                // todo: reset failed
                
            } else {
                // delay 200ms for next retry
                vsf_teda_set_timer_ms(200);
            }
            break;
        case HUB_STAT_SCAN_WAIT_GET_PORT_STATUS:
        scan_check_port_status:
            if (hub->hub_portsts.wPortChange & USB_PORT_STAT_C_CONNECTION) {
                // try to connect new child
                hub->state = HUB_STAT_CONNECT_START;
                // clear the cnnection change state
                err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                        USB_PORT_FEAT_C_CONNECTION);
            } else if (hub->hub_portsts.wPortChange & USB_PORT_STAT_C_ENABLE) {
                hub->hub_portsts.wPortChange &= ~USB_PORT_STAT_C_ENABLE;
                err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                        USB_PORT_FEAT_C_ENABLE);
                retain_state = true;
            } else if (hub->hub_portsts.wPortChange & USB_PORT_STAT_C_SUSPEND) {
                hub->hub_portsts.wPortChange &= ~USB_PORT_STAT_C_SUSPEND;
                err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                        USB_PORT_FEAT_C_SUSPEND);
                retain_state = true;
            } else if (hub->hub_portsts.wPortChange & USB_PORT_STAT_C_OVERCURRENT) {
                hub->hub_portsts.wPortChange &= ~USB_PORT_FEAT_C_OVER_CURRENT;
                err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                        USB_PORT_FEAT_C_OVER_CURRENT);
                retain_state = true;
            } else if (hub->hub_portsts.wPortChange & USB_PORT_STAT_C_RESET) {
                hub->hub_portsts.wPortChange &= ~USB_PORT_FEAT_C_RESET;
                err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                        USB_PORT_FEAT_C_RESET);
                retain_state = true;
            } else if (hub->usbh->dev_new != NULL) {
                vsf_teda_set_timer_ms(10);
                hub->state = HUB_STAT_SCAN_DELAY_FOR_RESCAN_CURRENT_PORT;
                retain_state = true;
            } else if (hub->cur_dev_idx++ < dev->maxchild) {
            scan_current_child:
                hub->state = HUB_STAT_SCAN_START;
                goto scan_next_child;
            } else {
                if (hub->is_go_on_next_round) {
                    hub->is_go_on_next_round = false;
                    hub->state = HUB_STAT_SCAN_START;
                    goto scan_next_round;
                } else {
                    // sleed 200ms for next scan round
                    hub->is_waiting_next_round = true;
                    vsf_teda_set_timer_ms(200);
                }
            }
            break;
        case HUB_STAT_CONNECT_WAIT_PORT_CLEAR_CONNECT_CHANGE:
            if (!(hub->hub_portsts.wPortStatus & USB_PORT_STAT_CONNECTION)) {
                // child removed
                vk_usbh_dev_t *dev = __vk_usbh_hub_getdev(hub, hub->cur_dev_idx - 1);

                if (dev != NULL) {
                    vk_usbh_disconnect_device(hub->usbh, dev);
                }

                hub->state = HUB_STAT_SCAN_WAIT_GET_PORT_STATUS;
                if (hub->hub_portsts.wPortStatus & USB_PORT_STAT_ENABLE) {
                    err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                            USB_PORT_FEAT_ENABLE);
                    retain_state = true;
                } else {
                    hub->hub_portsts.wPortChange &= ~USB_PORT_STAT_C_CONNECTION;
                    goto scan_check_port_status;
                }
            } else {
                // child connected
                hub->is_child_connecting = true;
                if (hub->hub_portsts.wPortStatus & USB_PORT_STAT_LOW_SPEED) {
                    vsf_teda_set_timer_ms(200);
                } else {
                    vsf_teda_set_timer_ms(10);
                }
            }
            break;
        }
        break;
    case VSF_EVT_USER:
        if (!hub->is_waiting_next_round) {
            break;
        }

        // cancel timer and force to polling next
        vsf_teda_cancel_timer();
    case VSF_EVT_TIMER:
        switch (hub->state) {
        default:
            VSF_USB_ASSERT(false);
            break;
        case HUB_STAT_ENUM_WAIT_PORT_POWER_ON_STABLE:
            if (hub->cur_dev_idx < dev->maxchild) {
                hub->state = HUB_STAT_ENUM_WAIT_HUB_DESC;
                goto enum_set_next_power_on;
            } else {
                hub->state = HUB_STAT_SCAN_START;
                hub->is_running = true;

            scan_next_round:
                hub->cur_dev_idx = 1;
            scan_next_child:
                if (hub->reset_mask & (1 << (hub->cur_dev_idx - 1))) {
                reset_child:
                    hub->state = HUB_STAT_RESET_CHILD_START;
                    hub->retry = 0;
                    // send command to reset port
                    err = __vk_usbh_hub_set_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                            USB_PORT_FEAT_RESET);
                } else {
                    goto get_port_status;
                }
            }
            break;
        case HUB_STAT_RESET_CHILD_WAIT_PORT_RESET_STABLE:
            // clear reset
            err = __vk_usbh_hub_clear_port_feature(hub->usbh, dev, hub->cur_dev_idx,
                    USB_PORT_FEAT_C_RESET);
            break;
        case HUB_STAT_RESET_CHILD_WAIT_PORT_CLEAR_RESET_STABLE:
        get_port_status:
            // get port status
            vk_usbh_urb_set_buffer(urb, &hub->hub_portsts, sizeof(hub->hub_portsts));
            err = __vk_usbh_hub_get_port_status(hub->usbh, dev, hub->cur_dev_idx);
            break;
        case HUB_STAT_RESET_CHILD_WAIT_NEXT_RETRY:
            goto reset_child;
        case HUB_STAT_SCAN_WAIT_NEXT_ROUND:
            hub->is_waiting_next_round = false;
            hub->state = HUB_STAT_SCAN_START;
            goto scan_next_round;
        case HUB_STAT_CONNECT_WAIT_PORT_STABLE:
            goto reset_child;
        case HUB_STAT_CONNECT_WAIT_PORT_RESET_STABLE:
            if (hub->usbh->dev_new != NULL) {
                // should not run to here, because after dev_new is set,
                //  hub driver will poll current port only
                ASSERT(false);
            } else {
                enum usb_device_speed_t speed =
                        (hub->hub_portsts.wPortStatus & USB_PORT_STAT_LOW_SPEED) ? USB_SPEED_LOW :
                        (hub->hub_portsts.wPortStatus & USB_PORT_STAT_HIGH_SPEED) ? USB_SPEED_HIGH : USB_SPEED_FULL;
                vk_usbh_dev_t *dev_new = vk_usbh_new_device(hub->usbh, speed, dev, hub->cur_dev_idx - 1);
                if (NULL == dev_new) {
                    // TODO: process if fail to allocate new device
                }

                hub->is_child_connecting = false;
                hub->state = HUB_STAT_SCAN_WAIT_GET_PORT_STATUS;
                goto scan_check_port_status;
            }
            break;
        case HUB_STAT_SCAN_DELAY_FOR_RESCAN_CURRENT_PORT:
            goto scan_current_child;
        }
        break;
    }
    if (err < 0) {
        goto fail;
    }
    if (!retain_state) {
        hub->state++;
    }
    return;

fail:
    if (hub->state < HUB_STAT_ENUM_END) {
        vk_usbh_remove_interface(hub->usbh, dev, hub->ifs);
    } else {
        // hub running, ignore errors and reset
        vk_usbh_urb_free_buffer(urb);
        hub->is_child_connecting = false;
        hub->state = HUB_STAT_SCAN_START;
        goto scan_next_round;
    }
}

static void __vk_usbh_hub_on_eda_terminate(vsf_eda_t *eda)
{
    vk_usbh_hub_t *hub = container_of(eda, vk_usbh_hub_t, teda);
    vsf_usbh_free(hub);
}

static void *__vk_usbh_hub_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_hub_t *hub;

    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    struct usb_interface_desc_t *desc_ifs = parser_ifs->parser_alt[ifs->cur_alt].desc_ifs;
    struct usb_endpoint_desc_t *desc_ep = parser_ifs->parser_alt[ifs->cur_alt].desc_ep;
    if (    (desc_ifs->bInterfaceSubClass > 1) /* some hubs has subclass 1 */
        ||  (desc_ifs->bNumEndpoints != 1)
        ||  ((desc_ep->bEndpointAddress & USB_DIR_MASK) != USB_DIR_IN)
        ||  ((desc_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) != USB_ENDPOINT_XFER_INT)
        ||  (NULL == (hub = vsf_usbh_malloc(sizeof(vk_usbh_hub_t))))) {
        return NULL;
    }

    memset(hub, 0, sizeof(vk_usbh_hub_t));
    hub->usbh = usbh;
    hub->dev = dev;
    hub->ifs = parser_ifs->ifs;

    hub->teda.fn.evthandler = __vk_usbh_hub_evthandler;
    hub->teda.on_terminate = __vk_usbh_hub_on_eda_terminate;
    vsf_teda_init(&hub->teda, vsf_prio_inherit, false);

    return hub;
}

static void __vk_usbh_hub_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vk_usbh_hub_t *hub = param;

    vsf_eda_fini(&hub->teda.use_as__vsf_eda_t);
}

bool vk_usbh_hub_is_dev_resetting(vk_usbh_dev_t *dev)
{
    vk_usbh_dev_t *dev_hub;

    VSF_USB_ASSERT(dev != NULL);
    dev_hub = dev->dev_parent;
    if (dev_hub != NULL) {
        vk_usbh_hub_t *hub = __vk_usbh_hub_dev_gethub(dev_hub);
        return (hub->reset_mask & (1 << dev->index)) != 0;
    }
    return false;
}

vsf_err_t vk_usbh_hub_reset_dev(vk_usbh_dev_t *dev)
{
    vk_usbh_dev_t *dev_hub;

    VSF_USB_ASSERT(dev != NULL);
    dev_hub = dev->dev_parent;
    if (dev_hub != NULL) {
        vk_usbh_hub_t *hub = __vk_usbh_hub_dev_gethub(dev_hub);
        uint_fast8_t index = dev->index;

        if (!(hub->reset_mask & (1 << index))) {
            hub->reset_mask |= (1 << index);
            __vsf_sched_safe(
                if (hub->is_waiting_next_round) {
                    vsf_eda_post_evt(&hub->teda.use_as__vsf_eda_t, VSF_EVT_USER);
                } else {
                    hub->is_go_on_next_round = true;
                }
            )
        }
    }
    return VSF_ERR_NONE;
}

#endif
