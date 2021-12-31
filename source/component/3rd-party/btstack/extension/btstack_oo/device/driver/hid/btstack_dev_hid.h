#ifndef __BTSTACK_DEV_HID_H__
#define __BTSTACK_DEV_HID_H__

#include "../../btstack_dev_cfg.h"

#if BTSTACK_USE_DEVICE && BTSTACK_USE_DEVICE_HID == ENABLED

#include "../../btstack_dev.h"

#if     defined(__BTSTACK_DEVICE_HID_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_DEVICE_HID_CLASS_INHERIT)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

declare_simple_class(btstack_device_hid_t)

struct btstack_device_hid_drv_op_t {
    implement(btstack_device_drv_op_t)
    int (*report)(btstack_dev_t *dev);

    const uint8_t               *sdp_hid;
};
typedef struct btstack_device_hid_drv_op_t btstack_device_hid_drv_op_t;

def_simple_class(btstack_device_hid_t) {

    public_member(
        implement(btstack_device_dev_t)
        btstack_dev_callback_t      *callback;
    )

	protected_member(
	    uint16_t                    l2cap_control_cid;
        uint16_t                    l2cap_interrupt_cid;
    )

    private_member(
        btstack_timer_source_t      timer;
        void (*on_timer)(btstack_device_hid_t *dev_hid);
    )
};

#if     defined(__BTSTACK_DEVICE_HID_CLASS_INHERIT) || defined(__BTSTACK_DEVICE_HID_CLASS_IMPLEMENT)
// private/protected APIs
extern btstack_device_hid_t * btstack_device_hid_get(uint8_t cid);
extern int btstack_device_hid_init(btstack_dev_t *dev);
extern int btstack_device_hid_connect(btstack_dev_t *dev);
extern int btstack_device_hid_disconnect(btstack_dev_t *dev);

extern void btstack_device_hid_packet_handler(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel,
        uint8_t *packet, uint16_t packet_size);
#else
// public APIs
extern void btstack_dev_hid_send_report(btstack_device_hid_t *dev_hid);

extern void btstack_dev_hid_set_timer(btstack_device_hid_t *dev_hid, uint_fast32_t timeout_ms, void (*on_timer)(btstack_device_hid_t *dev_hid));
extern void btstack_dev_hid_remove_timer(btstack_device_hid_t *dev_hid);
#endif

#undef __BTSTACK_DEVICE_HID_CLASS_INHERIT
#undef __BTSTACK_DEVICE_HID_CLASS_IMPLEMENT

#if BTSTACK_USE_DEVICE_NSPRO == ENABLED
#   include "./nspro/btstack_dev_nspro.h"
#endif
#if BTSTACK_USE_DEVICE_DS4 == ENABLED
#   include "./ds4/btstack_dev_ds4.h"
#endif

#endif      // BTSTACK_USE_DEVICE && BTSTACK_USE_DEVICE_HID
#endif	    // __BTSTACK_DEV_HID_H__
