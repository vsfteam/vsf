#define __BTSTACK_HOST_HID_BASE_CLASS_IMPLEMENT
#define __BTSTACK_HOST_CLASS_INHERIT__
#define __BTSTACK_CLASS_INHERIT__
#include "./btstack_host_hid_base.h"

#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_HOST_HID == ENABLED

void btstack_host_hid_base_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    btstack_host_hid_base_t *hid_dev = (btstack_host_hid_base_t *)btstack_host_get_priv_dev(dev);

    if (HCI_EVENT_PACKET == packet_type) {
        switch (hci_event_packet_get_type(packet)) {
        case L2CAP_EVENT_INCOMING_CONNECTION:
            l2cap_accept_connection(l2cap_event_incoming_connection_get_local_cid(packet));
            break;
        case L2CAP_EVENT_CHANNEL_OPENED:
            if (l2cap_event_channel_opened_get_status(packet) != 0) {
                break;
            }

            uint_fast16_t cid = l2cap_event_channel_opened_get_local_cid(packet);
            uint_fast16_t psm = l2cap_event_channel_opened_get_psm(packet);
            if (PSM_SDP == psm) {
                btstack_host_hid_base_connect(dev);
            } else if (PSM_HID_CONTROL == psm) {
                BTSTACK_HOST_HID_TRACE("%d: control channel connected\r\n", dev->con_handle);
                hid_dev->l2cap_control_cid = cid;
                btstack_l2cap_create_channel(dev, PSM_HID_INTERRUPT, l2cap_max_mtu(), &hid_dev->l2cap_interrupt_cid);
                hid_dev->l2cap_control_cid = cid;
            } else if (PSM_HID_INTERRUPT == psm) {
                BTSTACK_HOST_HID_TRACE("%d: interrupt channel connected\r\n", dev->con_handle);
                hid_dev->l2cap_interrupt_cid = cid;
                btstack_evthandler(BTSTACK_ON_CHANNELS_OPEN_COMPLETE, dev);
            }
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            BTSTACK_HOST_HID_TRACE("%d: disconnected\r\n", dev->con_handle);
            break;
        }
    }
}

int btstack_host_hid_base_connect(btstack_dev_t *dev)
{
    btstack_host_hid_base_t *hid_dev = (btstack_host_hid_base_t *)btstack_host_get_priv_dev(dev);

    uint_fast16_t mtu = l2cap_max_mtu();
    btstack_l2cap_register_service(PSM_SDP, mtu, LEVEL_0);
    btstack_l2cap_register_service(PSM_HID_CONTROL, mtu, LEVEL_0);
    btstack_l2cap_register_service(PSM_HID_INTERRUPT, mtu, LEVEL_0);

    return btstack_l2cap_create_channel(dev, PSM_HID_CONTROL, l2cap_max_mtu(), &hid_dev->l2cap_control_cid);
}

int btstack_host_hid_base_disconnect(btstack_dev_t *dev)
{
    btstack_host_hid_base_t *hid_dev = (btstack_host_hid_base_t *)btstack_host_get_priv_dev(dev);
    if (hid_dev->l2cap_interrupt_cid) {
        l2cap_disconnect(hid_dev->l2cap_interrupt_cid, 0);
    }
    if (hid_dev->l2cap_control_cid) {
        l2cap_disconnect(hid_dev->l2cap_control_cid, 0);
    }
    return 0;
}

#endif      // BTSTACK_OO_USE_HOST && BTSTACK_OO_USE_HOST_HID
