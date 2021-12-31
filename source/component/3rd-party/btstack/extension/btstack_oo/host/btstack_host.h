#ifndef __BTSTACK_HOST_H__
#define __BTSTACK_HOST_H__

#include "./btstack_host_cfg.h"

#if BTSTACK_OO_USE_HOST == ENABLED

#include "../btstack_oo.h"
#include "btstack.h"

#if     defined(__BTSTACK_HOST_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_HOST_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

#define BTSTACK_HOST_TRACE(...)                     BTSTACK_TRACE("host: " __VA_ARGS__)

#if BTSTACK_HOST_CFG_DEV_INHERIT != ENABLED
#   error not supported currently!
#   define btstack_host_get_priv_dev(__dev)         (__dev->priv_dev)
#else
#   define btstack_host_get_priv_dev(__dev)         (__dev)
#endif

struct btstack_host_drv_op_t {
    implement(btstack_drv_op_t)
};
typedef struct btstack_host_drv_op_t btstack_host_drv_op_t;

struct btstack_host_drv_t {
    btstack_linked_item_t drv_node;
    const btstack_host_drv_op_t *op;
};
typedef struct btstack_host_drv_t btstack_host_drv_t;

#undef PUBLIC_CONST
#if defined(__BTSTACK_HOST_CLASS_INHERIT__) || defined(__BTSTACK_HOST_CLASS_IMPLEMENT)
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST             const
#endif

vsf_class(btstack_host_dev_t) {
    public_member(
        implement(btstack_dev_t)

        // info from inquiry
        PUBLIC_CONST uint32_t cod;
        PUBLIC_CONST char name[32];
        PUBLIC_CONST uint16_t clock_offset;
        PUBLIC_CONST uint8_t page_scan_repetition_mode;
    )
    protected_member(
#if BTSTACK_HOST_CFG_DEV_INHERIT != ENABLED
        void *priv_dev;
#endif

        // remote name request state
        uint8_t request_name    : 1;
    )
};

extern void btstack_host_init(void);
extern void btstack_host_register_drv(btstack_host_drv_t *drv);
extern bool btstack_host_is_dev_supported(btstack_host_dev_t *dev);
extern int btstack_host_connect_dev(btstack_host_dev_t *dev);
extern int btstack_host_remove_dev(btstack_host_dev_t *dev, btstack_linked_list_iterator_t *it);

extern int btstack_host_scan(uint_fast8_t duration_in_1280ms_units);

#undef __BTSTACK_HOST_CLASS_IMPLEMENT
#undef __BTSTACK_HOST_CLASS_INHERIT__

#if BTSTACK_OO_USE_HOST_HID == ENABLED
#   include "./driver/hid/btstack_host_hid_base.h"
#endif

#endif      // BTSTACK_OO_USE_HOST
#endif      // __BTSTACK_HOST_H__
