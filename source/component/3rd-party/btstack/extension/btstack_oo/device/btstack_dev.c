#define __BTSTACK_DEVICE_CLASS_IMPLEMENT
#define __BTSTACK_CLASS_INHERIT__
#include "./btstack_dev.h"

#if BTSTACK_OO_USE_DEVICE == ENABLED

int btstack_device_init(btstack_device_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (dev->op != NULL));
    const btstack_device_drv_op_t *op = (const btstack_device_drv_op_t *)dev->op;

    dev->con_handle = HCI_CON_HANDLE_INVALID;
    dev->is_device = true;
    btstack_add_device_dev(&dev->use_as__btstack_dev_t);

    if (op->cod != 0) {
        gap_set_class_of_device(op->cod);
    }
    if (op->name != NULL) {
        gap_set_local_name(op->name);
    }

    sdp_init();
    if (op->sdp_pnp != NULL) {
        sdp_register_service(op->sdp_pnp);
    }
    if (op->eir != NULL) {
        gap_set_extended_inquiry_response(op->eir);
    }

    return dev->op->init(&dev->use_as__btstack_dev_t);
}

int btstack_device_try_pair(btstack_device_dev_t *dev)
{
    BTSTACK_ASSERT((dev != NULL) && (dev->op != NULL));
    const btstack_device_drv_op_t *op = (const btstack_device_drv_op_t *)dev->op;

    if (op->ssp_enabled) {
        gap_ssp_set_enable(1);
    }
    gap_discoverable_control(1);
    return 0;
}

int btstack_device_connect(btstack_device_dev_t *dev, bd_addr_t remote_addr)
{
    BTSTACK_ASSERT(dev != NULL);

    gap_connectable_control(0);
    gap_ssp_set_enable(0);

    bd_addr_copy(dev->remote_addr, remote_addr);
    return btstack_connect_dev(&dev->use_as__btstack_dev_t);
}

#endif      // BTSTACK_OO_USE_DEVICE
