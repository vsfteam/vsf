#ifndef __BTSTACK_DEV_H__
#define __BTSTACK_DEV_H__

#include "./btstack_dev_cfg.h"

#if BTSTACK_OO_USE_DEVICE == ENABLED

#include "../btstack_oo.h"

#if     defined(__BTSTACK_DEVICE_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_DEVICE_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

#define BTSTACK_DEVICE_TRACE(...)                   BTSTACK_TRACE("device: " __VA_ARGS__)

enum btstack_dev_state_t {
    BTSTACK_DEV_STATE_DISCONNECT = 0,
    BTSTACK_DEV_STATE_TRY_PAIR,
    BTSTACK_DEV_STATE_TRY_CONNECT,
    BTSTACK_DEV_STATE_CONNECTED,
};
typedef enum btstack_dev_state_t btstack_dev_state_t;

struct btstack_device_drv_op_t {
    implement(btstack_drv_op_t)

    const uint8_t *sdp_pnp;
    const uint8_t *eir;
    union {
        struct {
            uint32_t ssp_enabled : 1;
        };
        uint32_t feature;
    };
};
typedef struct btstack_device_drv_op_t btstack_device_drv_op_t;

struct btstack_dev_callback_t {
    void (*on_connected)(btstack_dev_t *dev);
    void (*on_disconnected)(btstack_dev_t *dev);
};
typedef struct btstack_dev_callback_t btstack_dev_callback_t;

vsf_class(btstack_device_dev_t) {
    public_member(
        implement(btstack_dev_t)
        bd_addr_t mac;
    )

    protected_member(
        btstack_dev_state_t dev_state;
    )
};

extern int btstack_device_init(btstack_device_dev_t *dev);
extern int btstack_device_try_pair(btstack_device_dev_t *dev);
extern int btstack_device_connect(btstack_device_dev_t *dev, bd_addr_t remote_addr);

#undef __BTSTACK_DEVICE_CLASS_IMPLEMENT
#undef __BTSTACK_DEVICE_CLASS_INHERIT__

#if BTSTACK_OO_USE_DEVICE_HID == ENABLED
#   include "./driver/hid/btstack_dev_hid.h"
#endif

#endif      // BTSTACK_OO_USE_DEVICE
#endif      // __BTSTACK_DEV_H__
