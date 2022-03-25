#ifndef __BTSTACK_HOST_HID_H__
#define __BTSTACK_HOST_HID_H__

#include "../../btstack_host_cfg.h"

#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_HOST_HID == ENABLED

#include "../../btstack_host.h"

#if     defined(__BTSTACK_HOST_HID_BASE_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_HOST_HID_BASE_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

#define BTSTACK_HOST_HID_TRACE(...)         BTSTACK_HOST_TRACE("hid: " __VA_ARGS__)

vsf_class(btstack_host_hid_base_t) {
#if BTSTACK_HOST_CFG_DEV_INHERIT == ENABLED
    public_member(
        implement(btstack_host_dev_t)
    )
#endif
    protected_member(
#if BTSTACK_HOST_CFG_DEV_INHERIT != ENABLED
        btstack_host_dev_t *dev;
#endif
        uint16_t l2cap_control_cid;
        uint16_t l2cap_interrupt_cid;
    )
};

#if defined(__BTSTACK_HOST_HID_BASE_CLASS_IMPLEMENT) || defined(__BTSTACK_HOST_HID_BASE_CLASS_INHERIT__)
extern void btstack_host_hid_base_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
extern int btstack_host_hid_base_connect(btstack_dev_t *dev);
extern int btstack_host_hid_base_disconnect(btstack_dev_t *dev);
#endif

#undef __BTSTACK_HOST_HID_BASE_CLASS_IMPLEMENT
#undef __BTSTACK_HOST_HID_BASE_CLASS_INHERIT__

#if BTSTACK_OO_USE_HOST_JOYCON == ENABLED
#   include "./joycon/btstack_host_joycon.h"
#endif
#if BTSTACK_OO_USE_HOST_DS4 == ENABLED
#   include "./ds4/btstack_host_ds4.h"
#endif

#endif      // BTSTACK_OO_USE_HOST && BTSTACK_OO_USE_HOST_HID
#endif      // __BTSTACK_HOST_HID_H__
