#include "./btstack_oo_cfg.h"

#if BTSTACK_OO_USE_HOST == ENABLED || BTSTACK_OO_USE_DEVICE == ENABLED

#define __BTSTACK_CLASS_IMPLEMENT
#include "./btstack_oo.h"

typedef struct btstack_t {
    btstack_packet_callback_registration_t  hci_event_callback_registration;
    btstack_linked_list_t dev_list;
    btstack_dev_t *dev_as_device;
    btstack_dev_t *dev_connecting;
    btstack_timer_source_t timer;
    bd_addr_t mac;
} btstack_t;

static void btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static NO_INIT btstack_t btstack;

#ifndef WEAK_BTSTACK_EVTHANDLER
WEAK(btstack_evthandler)
int btstack_evthandler(btstack_evt_t evt, void *param)
{
    BTSTACK_ASSERT(false);
    return 0;
}
#endif

static void btstack_on_timer(btstack_timer_source_t *timer)
{
    btstack_evthandler(BTSTACK_ON_TIMER, NULL);
}

void btstack_remove_timer(void)
{
    btstack_run_loop_remove_timer(&btstack.timer);
}

void btstack_set_timer(uint_fast32_t timeout_ms)
{
    btstack_run_loop_set_timer_handler(&btstack.timer, btstack_on_timer);
    btstack_run_loop_set_timer(&btstack.timer, timeout_ms);
    btstack_run_loop_add_timer(&btstack.timer);
}

static btstack_dev_t * btstack_get_dev_by_con_handle(hci_con_handle_t con_handle)
{
    btstack_dev_t *dev;
    btstack_linked_list_iterator_t it;
    btstack_linked_list_iterator_init(&it, &btstack.dev_list);

    while (btstack_linked_list_iterator_has_next(&it)) {
        dev = (btstack_dev_t *)btstack_linked_list_iterator_next(&it);
        if (dev->con_handle == con_handle) {
            return dev;
        }
    }
    return NULL;
}

static void btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;

#if BTSTACK_OO_USE_HOST
    extern void btstack_host_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
    btstack_host_packet_handler(packet_type, channel, packet, size);
#endif

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (HCI_STATE_WORKING == btstack_event_state_get_state(packet)) {
                btstack_evthandler(BTSTACK_ON_INITIALIZED, NULL);
            }
            break;
        case HCI_EVENT_CONNECTION_COMPLETE:
            if (!hci_event_connection_complete_get_status(packet)) {
                con_handle = hci_event_connection_complete_get_connection_handle(packet);
                hci_connection_t *conn = hci_connection_for_handle(con_handle);
                btstack_dev_t *dev = NULL;

                if (HCI_ROLE_SLAVE == conn->role) {
                    dev = btstack.dev_as_device;
                } else {
                    if (btstack.dev_connecting != NULL) {
                        dev = btstack.dev_connecting;
                        btstack.dev_connecting = NULL;
                    }
                }

                if (dev != NULL) {
                    dev->con_handle = con_handle;
                    BTSTACK_TRACE("connected %s: %d\r\n", dev->op->name, con_handle);
                    btstack_evthandler(BTSTACK_ON_CONNECTION_COMPLETE, dev);
                }
            }
            break;
        case L2CAP_EVENT_INCOMING_CONNECTION:
            con_handle = l2cap_event_incoming_connection_get_handle(packet);
            break;
        case L2CAP_EVENT_CHANNEL_OPENED:
            con_handle = l2cap_event_channel_opened_get_handle(packet);
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = hci_event_disconnection_complete_get_connection_handle(packet);
            break;
        case HCI_EVENT_PIN_CODE_REQUEST: {
                bd_addr_t addr;
                hci_event_pin_code_request_get_bd_addr(packet, addr);
                gap_pin_code_response(addr, "0000");
            }
            break;
        }
        break;
    case L2CAP_DATA_PACKET: {
            extern l2cap_channel_t * l2cap_get_channel_for_local_cid(uint16_t local_cid);
            l2cap_channel_t * l2cap_channel = l2cap_get_channel_for_local_cid(channel);
            if (l2cap_channel != NULL) {
                con_handle = l2cap_channel->con_handle;
            }
        }
        break;
    }

    if (con_handle != HCI_CON_HANDLE_INVALID) {
        btstack_dev_t *dev = btstack_get_dev_by_con_handle(con_handle);
        if (dev != NULL) {
            if (dev->op->packet_handler != NULL) {
                dev->op->packet_handler(dev, packet_type, channel, packet, size);
            }

            switch (packet_type) {
            case HCI_EVENT_PACKET:
                switch (hci_event_packet_get_type(packet)) {
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    dev->con_handle = HCI_CON_HANDLE_INVALID;
                    BTSTACK_TRACE("disconnected %s: %d\r\n", dev->op->name, con_handle);
                    btstack_evthandler(BTSTACK_ON_DISCONNECTION_COMPLETE, dev);

                    if (dev->is_device) {
#if BTSTACK_OO_USE_DEVICE == ENABLED
#else
                        BTSTACK_ASSERT(false);
#endif
                    } else {
#if BTSTACK_OO_USE_HOST == ENABLED
                        btstack_host_remove_dev((btstack_host_dev_t *)dev, NULL);
#else
                        BTSTACK_ASSERT(false);
#endif
                    }
                    break;
                }
                break;
            }
        }
    }
}

void btstack_init(bd_addr_t mac)
{
    bd_addr_copy(btstack.mac, mac);
    btstack.dev_connecting = NULL;
    btstack.dev_list = NULL;
    btstack.dev_as_device = NULL;

    btstack.hci_event_callback_registration.callback = btstack_packet_handler;
    hci_add_event_handler(&btstack.hci_event_callback_registration);

    l2cap_init();
    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);

    // link_policy
    //  bit0: Park mode
    //  bit1: sniff mode
    //  bit2: hold mode
    //  bit3: master/slave switch mode
#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_DEVICE == ENABLED
    gap_set_default_link_policy_settings(0x0007);
#elif BTSTACK_OO_USE_HOST == ENABLED
    gap_set_default_link_policy_settings(0x0007);
#elif BTSTACK_OO_USE_DEVICE == ENABLED
    gap_set_default_link_policy_settings(0x0005);
#endif
}

bd_addr_t * btstack_get_mac(void)
{
    return &btstack.mac;
}

void btstack_start(void)
{
    hci_power_control(HCI_POWER_ON);
}

void btstack_get_devs(btstack_linked_list_iterator_t *it)
{
    BTSTACK_ASSERT(it != NULL);
    btstack_linked_list_iterator_init(it, &btstack.dev_list);
}

bool btstack_is_dev_connected(btstack_dev_t *dev)
{
    BTSTACK_ASSERT(dev != NULL);
    return dev->con_handle != HCI_CON_HANDLE_INVALID;
}

int btstack_init_dev(btstack_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (dev->op != NULL));
    if (dev->op->init != NULL) {
        return dev->op->init(dev);
    }
    return 0;
}

int btstack_connect_dev(btstack_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (dev->op != NULL));
    if (btstack.dev_connecting != NULL) {
        return -1;
    }

    btstack.dev_connecting = dev;
    BTSTACK_TRACE("connecting %s\r\n", dev->op->name);
    return dev->op->connect(dev);
}

int btstack_disconnect_dev(btstack_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (dev->op != NULL));
    BTSTACK_TRACE("disconnecting %s\r\n", dev->op->name);
    return dev->op->disconnect(dev);
}

int btstack_add_host_dev(btstack_dev_t *dev)
{
    BTSTACK_ASSERT(dev != NULL);
    btstack_linked_list_add(&btstack.dev_list, &dev->dev_node);
    return 0;
}

int btstack_add_device_dev(btstack_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (NULL == btstack.dev_as_device));
    btstack.dev_as_device = dev;
    return btstack_add_host_dev(dev);
}

int btstack_remove_dev(btstack_dev_t *dev, btstack_linked_list_iterator_t *it)
{
    BTSTACK_ASSERT(dev != NULL);
    if (btstack_is_dev_connected(dev)) {
        return -1;
    }

    if (it != NULL) {
        btstack_linked_list_iterator_remove(it);
    } else {
        btstack_linked_list_remove(&btstack.dev_list, &dev->dev_node);
    }
    return 0;
}

int btstack_l2cap_create_channel(btstack_dev_t *dev, uint_fast16_t psm, uint_fast16_t mtu, uint16_t *cid)
{
    BTSTACK_ASSERT(dev != NULL);
    return l2cap_create_channel(btstack_packet_handler, dev->remote_addr, psm, mtu, cid);
}

int btstack_l2cap_register_service(uint_fast16_t psm, uint_fast16_t mtu, gap_security_level_t security_level)
{
    return l2cap_register_service(btstack_packet_handler, psm, mtu, security_level);
}

#endif      // BTSTACK_OO_USE_HOST || BTSTACK_OO_USE_DEVICE
