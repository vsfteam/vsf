#ifndef __BTSTACK_EXT_H__
#define __BTSTACK_EXT_H__

#include "./btstack_oo_cfg.h"

#if BTSTACK_OO_USE_HOST == ENABLED || BTSTACK_OO_USE_DEVICE == ENABLED

#include "btstack.h"

#if     defined(__BTSTACK_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT
#elif   defined(__BTSTACK_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

vsf_declare_class(btstack_dev_t)

enum btstack_evt_t {
    BTSTACK_ON_TIMER,
    BTSTACK_ON_INITIALIZED,
    BTSTACK_ON_CONNECTION_COMPLETE,
    BTSTACK_ON_DISCONNECTION_COMPLETE,
    BTSTACK_ON_CHANNELS_OPEN_COMPLETE,

    BTSTACK_HOST_ON_INQUIRY_RESULT,
    BTSTACK_HOST_ON_INQUIRY_COMPLETE,

    // TODO: add btstack device events
    BTSTACK_DEVICE_ON_XXX,
};
typedef enum btstack_evt_t btstack_evt_t;

struct btstack_drv_op_t {
    uint32_t cod;
    char *name;

    int (*init)(btstack_dev_t *dev);
    int (*connect)(btstack_dev_t *dev);
    int (*disconnect)(btstack_dev_t *dev);
    void (*packet_handler)(btstack_dev_t *dev, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
};
typedef struct btstack_drv_op_t btstack_drv_op_t;

#undef PUBLIC_CONST
#if defined(__BTSTACK_CLASS_INHERIT__) || defined(__BTSTACK_CLASS_IMPLEMENT)
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST             const
#endif

vsf_class(btstack_dev_t) {
    public_member(
        PUBLIC_CONST btstack_linked_item_t dev_node;
        PUBLIC_CONST bd_addr_t remote_addr;
        PUBLIC_CONST bool is_device;
        const btstack_drv_op_t * PUBLIC_CONST op;
    )
    protected_member(
        hci_con_handle_t con_handle;
    )
};

extern void btstack_init(bd_addr_t mac);
extern bd_addr_t * btstack_get_mac(void);
extern void btstack_start(void);
extern void btstack_get_devs(btstack_linked_list_iterator_t *it);
extern bool btstack_is_dev_connected(btstack_dev_t *dev);

extern int btstack_init_dev(btstack_dev_t *dev);
extern int btstack_connect_dev(btstack_dev_t *dev);
extern int btstack_disconnect_dev(btstack_dev_t *dev);
extern int btstack_add_host_dev(btstack_dev_t *dev);
extern int btstack_add_device_dev(btstack_dev_t *dev);
extern int btstack_remove_dev(btstack_dev_t *dev, btstack_linked_list_iterator_t *it);

extern int btstack_evthandler(btstack_evt_t evt, void *param);

extern void btstack_remove_timer(void);
extern void btstack_set_timer(uint_fast32_t timeout_ms);

#if defined(__BTSTACK_CLASS_INHERIT__) || defined(__BTSTACK_CLASS_IMPLEMENT)
extern int btstack_l2cap_create_channel(btstack_dev_t *dev, uint_fast16_t psm, uint_fast16_t mtu, uint16_t *cid);
extern int btstack_l2cap_register_service(uint_fast16_t psm, uint_fast16_t mtu, gap_security_level_t security_level);
#endif

#undef __BTSTACK_CLASS_INHERIT__
#undef __BTSTACK_CLASS_IMPLEMENT

#if BTSTACK_OO_USE_HOST == ENABLED
#   include "./host/btstack_host.h"
#endif
#if BTSTACK_OO_USE_DEVICE == ENABLED
#   include "./device/btstack_dev.h"
#endif

#endif      // BTSTACK_OO_USE_HOST || BTSTACK_OO_USE_DEVICE
#endif      // __BTSTACK_EXT_H__
