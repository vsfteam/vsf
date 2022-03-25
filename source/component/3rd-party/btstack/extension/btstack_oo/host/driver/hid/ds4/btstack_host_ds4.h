#ifndef __BTSTACK_HOST_DS4_H__
#define __BTSTACK_HOST_DS4_H__

#include "../../../btstack_host_cfg.h"

#if BTSTACK_OO_USE_HOST == ENABLED && BTSTACK_OO_USE_HOST_HID == ENABLED && BTSTACK_OO_USE_HOST_DS4 == ENABLED

#include "../btstack_host_hid_base.h"

#if     defined(__BTSTACK_HOST_DS4_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_HOST_DS4_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

vsf_class(btstack_host_ds4_t) {
    public_member(
        implement(btstack_host_hid_base_t)
    )
};

extern const btstack_host_drv_op_t btstack_host_ds4_drv;

#   undef __BTSTACK_HOST_DS4_CLASS_IMPLEMENT
#   undef __BTSTACK_HOST_DS4_CLASS_INHERIT__
#endif      // BTSTACK_OO_USE_HOST && BTSTACK_OO_USE_HOST_HID && BTSTACK_OO_USE_HOST_DS4
#endif      // __BTSTACK_HOST_DS4_H__
