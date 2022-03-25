#define __BTSTACK_HOST_DS4_CLASS_IMPLEMENT
#define __BTSTACK_HOST_HID_BASE_CLASS_INHERIT__
#define __BTSTACK_HOST_CLASS_INHERIT__
#define __BTSTACK_CLASS_INHERIT__
#include "./btstack_host_ds4.h"

#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_HOST_HID == ENABLED && BTSTACK_OO_USE_HOST_DS4 == ENABLED

#define LEFT_DS4_NAME                       "Wireless Controller"

#define BTSTACK_HOST_DS4_TRACE(...)         BTSTACK_HOST_HID_TRACE("ds4: " __VA_ARGS__)

static void btstack_host_ds4_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    btstack_host_dev_t *host_dev = (btstack_host_dev_t *)dev;
    btstack_host_ds4_t *ds4_dev = (btstack_host_ds4_t *)btstack_host_get_priv_dev(host_dev);

    if (HCI_EVENT_PACKET == packet_type) {
        btstack_host_hid_base_packet_handler(dev, packet_type, channel, packet, size);
    } else if (L2CAP_DATA_PACKET == packet_type) {
        BTSTACK_HOST_DS4_TRACE("%d: 0x%p\r\n", dev->con_handle, ds4_dev);
        vsf_trace_buffer(VSF_TRACE_DEBUG, packet, size);
    }
}

const btstack_host_drv_op_t btstack_host_ds4_drv = {
    .cod            = 0x002508,
    .name           = LEFT_DS4_NAME,
    .connect        = btstack_host_hid_base_connect,
    .disconnect     = btstack_host_hid_base_disconnect,
    .packet_handler = btstack_host_ds4_packet_handler,
};

#endif      // BTSTACK_OO_USE_HOST && BTSTACK_OO_USE_HOST_HID && BTSTACK_OO_USE_HOST_DS4
