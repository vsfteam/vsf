#define __BTSTACK_DEVICE_HID_CLASS_IMPLEMENT
#define __BTSTACK_DEVICE_CLASS_INHERIT
#define __BTSTACK_CLASS_INHERIT
#include "./btstack_dev_hid.h"

#if BTSTACK_USE_DEVICE == ENABLED && BTSTACK_USE_DEVICE_HID == ENABLED

static void btstack_device_hid_delayed_connected(btstack_timer_source_t *timer)
{
    btstack_device_hid_t *dev_hid = container_of(timer, btstack_device_hid_t, timer);
    if ((dev_hid->callback != NULL) && (dev_hid->callback->on_connected != NULL)) {
        dev_hid->callback->on_connected(&dev_hid->use_as__btstack_dev_t);
    }
}

static void btstack_device_hid_timer(btstack_timer_source_t *timer)
{
    btstack_device_hid_t *dev_hid = container_of(timer, btstack_device_hid_t, timer);
    if (dev_hid->on_timer != NULL) {
        dev_hid->on_timer(dev_hid);
    }
}

void btstack_device_hid_remove_timer(btstack_device_hid_t *dev_hid)
{
    btstack_run_loop_remove_timer(&dev_hid->timer);
}

void btstack_device_hid_set_timer(btstack_device_hid_t *dev_hid, uint_fast32_t timeout_ms, void (*on_timer)(btstack_device_hid_t *dev_hid))
{
    if (on_timer != NULL) {
        dev_hid->on_timer = on_timer;
    }
    btstack_run_loop_set_timer_handler(&dev_hid->timer, btstack_device_hid_timer);
    btstack_run_loop_set_timer(&dev_hid->timer, timeout_ms);
    btstack_run_loop_add_timer(&dev_hid->timer);
}

void btstack_device_hid_send_report(btstack_device_hid_t *dev_hid)
{
    btstack_device_hid_drv_op_t *hid_op = (btstack_device_hid_drv_op_t *)dev_hid->op;
    hid_op->report(&dev_hid->use_as__btstack_dev_t);
}

void btstack_device_hid_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel,
        uint8_t *packet, uint16_t packet_size)
{
    btstack_device_hid_t *dev_hid = (btstack_device_hid_t *)dev;
    BTSTACK_ASSERT(dev_hid != NULL);
    bool connected_before = dev_hid->l2cap_control_cid && dev_hid->l2cap_interrupt_cid;

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (packet[0]) {
        case L2CAP_EVENT_INCOMING_CONNECTION:
            switch (l2cap_event_incoming_connection_get_psm(packet)) {
            case PSM_HID_CONTROL:
            case PSM_HID_INTERRUPT:
                if (!dev_hid->con_handle || (dev_hid->con_handle == l2cap_event_incoming_connection_get_handle(packet))) {
                    BTSTACK_DEVICE_TRACE("hid: connect\r\n");
                    dev_hid->con_handle = l2cap_event_incoming_connection_get_handle(packet);
                    l2cap_accept_connection(channel);
                    break;
                }
            default:
                l2cap_decline_connection(channel);
                break;
            }
            break;
        case L2CAP_EVENT_CHANNEL_OPENED:
            if (l2cap_event_channel_opened_get_status(packet)) {
                return;
            }
            
            switch (l2cap_event_channel_opened_get_psm(packet)) {
            case PSM_HID_CONTROL:
                dev_hid->l2cap_control_cid = l2cap_event_channel_opened_get_local_cid(packet);
                break;
            case PSM_HID_INTERRUPT:
                dev_hid->l2cap_interrupt_cid = l2cap_event_channel_opened_get_local_cid(packet);
                break;
            }
            if (dev_hid->l2cap_control_cid && dev_hid->l2cap_interrupt_cid) {
                BTSTACK_DEVICE_TRACE("hid: opened\r\n");

                btstack_run_loop_set_timer_handler(&dev_hid->timer, btstack_device_hid_delayed_connected);
                btstack_run_loop_set_timer(&dev_hid->timer, 100);
                btstack_run_loop_add_timer(&dev_hid->timer);
            } else if (!connected_before && dev_hid->l2cap_control_cid && !dev_hid->l2cap_interrupt_cid) {
                btstack_l2cap_create_channel(dev, PSM_HID_INTERRUPT, l2cap_max_mtu(), &dev_hid->l2cap_interrupt_cid);
            }
            break;
        case L2CAP_EVENT_CHANNEL_CLOSED:
            if (l2cap_event_channel_closed_get_local_cid(packet) == dev_hid->l2cap_control_cid) {
                dev_hid->l2cap_control_cid = 0;
            }
            if (l2cap_event_channel_closed_get_local_cid(packet) == dev_hid->l2cap_interrupt_cid) {
                dev_hid->l2cap_interrupt_cid = 0;
            }
            if (connected_before && !(dev_hid->l2cap_control_cid && dev_hid->l2cap_interrupt_cid)) {
                BTSTACK_DEVICE_TRACE("hid: closed\r\n");
                dev_hid->con_handle = 0;

                if ((dev_hid->callback != NULL) && (dev_hid->callback->on_disconnected != NULL)) {
                    dev_hid->callback->on_disconnected(&dev_hid->use_as__btstack_dev_t);
                }
            }
            break;
        case L2CAP_EVENT_CAN_SEND_NOW:
            BTSTACK_DEVICE_TRACE("hid: can send now\r\n");
            break;
        }
        break;
    case L2CAP_DATA_PACKET:
        break;
    }
}

int btstack_device_hid_init(btstack_dev_t *dev)
{
    btstack_device_hid_t *dev_hid = (btstack_device_hid_t *)dev;
    btstack_device_hid_drv_op_t *op = (btstack_device_hid_drv_op_t *)dev->op;

    // TODO: use btstack_hci_register_sco_packet_handler(dev);
//    hci_register_sco_packet_handler(&btstack_dev_hid_packet_handler);

    // sdp
    if (op->sdp_hid != NULL) {
        sdp_register_service(op->sdp_hid);
    }

    btstack_l2cap_register_service(PSM_HID_INTERRUPT, l2cap_max_mtu(), LEVEL_2);
    btstack_l2cap_register_service(PSM_HID_CONTROL, l2cap_max_mtu(), LEVEL_2);
    return 0;
}

int btstack_device_hid_connect(btstack_dev_t *dev)
{
    btstack_device_hid_t *dev_hid = (btstack_device_hid_t *)dev;
    btstack_l2cap_create_channel(dev, PSM_HID_CONTROL, l2cap_max_mtu(), &dev_hid->l2cap_control_cid);
    return 0;
}

int btstack_device_hid_disconnect(btstack_dev_t *dev)
{
    btstack_device_hid_t *dev_hid = (btstack_device_hid_t *)dev;
    if (dev_hid->l2cap_interrupt_cid) {
        l2cap_disconnect(dev_hid->l2cap_interrupt_cid, 0);
    }
    if (dev_hid->l2cap_control_cid) {
        l2cap_disconnect(dev_hid->l2cap_control_cid, 0);
    }
    return 0;
}

#endif      // BTSTACK_USE_DEVICE && BTSTACK_USE_DEVICE_HID
