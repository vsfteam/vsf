#define __BTSTACK_HOST_JOYCON_CLASS_IMPLEMENT
#define __BTSTACK_HOST_HID_BASE_CLASS_INHERIT__
#define __BTSTACK_HOST_CLASS_INHERIT__
#define __BTSTACK_CLASS_INHERIT__
#include "./btstack_host_joycon.h"

#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_HOST_HID == ENABLED && BTSTACK_OO_USE_HOST_JOYCON == ENABLED

#define LEFT_JOYCON_NAME                    "Joy-Con (L)"
#define RIGHT_JOYCON_NAME                   "Joy-Con (R)"

#define BTSTACK_HOST_JOYCON_TRACE(...)      BTSTACK_HOST_HID_TRACE("joycon: " __VA_ARGS__)

static void btstack_host_joycon_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    btstack_host_dev_t *host_dev = (btstack_host_dev_t *)dev;
    btstack_host_joycon_t *joycon_dev = (btstack_host_joycon_t *)btstack_host_get_priv_dev(host_dev);
    bool is_left = !strcmp(host_dev->name, LEFT_JOYCON_NAME);

    if (HCI_EVENT_PACKET == packet_type) {
        btstack_host_hid_base_packet_handler(dev, packet_type, channel, packet, size);
    } else if (L2CAP_DATA_PACKET == packet_type) {
        BTSTACK_HOST_JOYCON_TRACE("%d: 0x%p %s\r\n", dev->con_handle, joycon_dev, is_left? "left" : "right");
        vsf_trace_buffer(VSF_TRACE_DEBUG, packet, size);
    }
}

const btstack_host_drv_op_t btstack_host_joycon_left_drv = {
    .cod            = 0x002508,
    .name           = LEFT_JOYCON_NAME,
    .connect        = btstack_host_hid_base_connect,
    .disconnect     = btstack_host_hid_base_disconnect,
    .packet_handler = btstack_host_joycon_packet_handler,
};

const btstack_host_drv_op_t btstack_host_joycon_right_drv = {
    .cod            = 0x002508,
    .name           = RIGHT_JOYCON_NAME,
    .connect        = btstack_host_hid_base_connect,
    .disconnect     = btstack_host_hid_base_disconnect,
    .packet_handler = btstack_host_joycon_packet_handler,
};

#endif      // BTSTACK_OO_USE_HOST && BTSTACK_OO_USE_HOST_HID && BTSTACK_OO_USE_HOST_JOYCON
