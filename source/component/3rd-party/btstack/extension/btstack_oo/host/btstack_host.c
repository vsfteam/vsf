#define __BTSTACK_HOST_CLASS_IMPLEMENT
#define __BTSTACK_CLASS_INHERIT__
#include "./btstack_host.h"

#include "service/heap/vsf_heap.h"

#if BTSTACK_OO_USE_HOST == ENABLED
typedef struct btstack_host_t {
    btstack_linked_list_t drv_list;
} btstack_host_t;

static NO_INIT btstack_host_t btstack_host;

WEAK(btstack_host_malloc_dev)
btstack_host_dev_t * btstack_host_malloc_dev(void)
{
    return vsf_heap_malloc(sizeof(btstack_host_dev_t));
}

WEAK(btstack_host_free_dev)
void btstack_host_free_dev(btstack_host_dev_t *dev)
{
    vsf_heap_free(dev);
}

void btstack_host_trace_dev(btstack_host_dev_t *dev)
{
    BTSTACK_HOST_TRACE(
            "btstack device(%s-%06X):\r\n"
            "    name: %s\r\n",
            bd_addr_to_str(dev->remote_addr), dev->cod,
                '\0' == dev->name[0] ? "unknown" : dev->name);
}

static const btstack_host_drv_op_t * btstack_host_get_drv_for_dev(btstack_host_dev_t *dev)
{
    btstack_host_drv_t *drv;
    btstack_linked_list_iterator_t it;
    btstack_linked_list_iterator_init(&it, &btstack_host.drv_list);

    while (btstack_linked_list_iterator_has_next(&it)) {
        drv = (btstack_host_drv_t *)btstack_linked_list_iterator_next(&it);
        if (    ((drv->op->cod != 0) && (drv->op->cod != dev->cod))
            ||  ((drv->op->name != NULL) && strcmp(drv->op->name, dev->name))) {
            continue;
        }
        return drv->op;
    }
    return NULL;
}

void btstack_host_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    btstack_linked_list_iterator_t it;
    btstack_host_dev_t *dev = NULL;
    bd_addr_t addr;

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {
        case GAP_EVENT_INQUIRY_RESULT:
            gap_event_inquiry_result_get_bd_addr((const uint8_t *)packet, addr);
            btstack_get_devs(&it);
            while (btstack_linked_list_iterator_has_next(&it)) {
                dev = (btstack_host_dev_t *)btstack_linked_list_iterator_next(&it);
                if (!bd_addr_cmp(addr, dev->remote_addr)) {
                    return;
                }
            }

            dev = btstack_host_malloc_dev();
            if (NULL == dev) {
                break;
            }
            memset(dev, 0, sizeof(*dev));
            dev->con_handle = HCI_CON_HANDLE_INVALID;
            dev->is_device = false;

            bd_addr_copy(dev->remote_addr, addr);
            dev->page_scan_repetition_mode = gap_event_inquiry_result_get_page_scan_repetition_mode((const uint8_t *)packet);
            dev->cod = gap_event_inquiry_result_get_class_of_device((const uint8_t *)packet);
            dev->clock_offset = gap_event_inquiry_result_get_clock_offset((const uint8_t *)packet);

            if (gap_event_inquiry_result_get_name_available((const uint8_t *)packet)) {
                strncpy(dev->name, (const char *)gap_event_inquiry_result_get_name((const uint8_t *)packet), sizeof(dev->name) - 1);
                if (btstack_host_is_dev_supported(dev)) {
                    btstack_host_trace_dev(dev);
                }
                if (!btstack_evthandler(BTSTACK_HOST_ON_INQUIRY_RESULT, dev)) {
                    btstack_host_free_dev(dev);
                    return;
                }
            } else {
                dev->request_name = true;
            }

            btstack_add_host_dev(&dev->use_as__btstack_dev_t);
            break;
        case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
            hci_event_remote_name_request_complete_get_bd_addr((const uint8_t *)packet, addr);
            btstack_get_devs(&it);
            while (btstack_linked_list_iterator_has_next(&it)) {
                dev = (btstack_host_dev_t *)btstack_linked_list_iterator_next(&it);
                if (dev->request_name && !bd_addr_cmp(addr, dev->remote_addr)) {
                    dev->request_name = false;
                    if (!hci_event_remote_name_request_complete_get_status((const uint8_t *)packet)) {
                        strncpy(dev->name, hci_event_remote_name_request_complete_get_remote_name((const uint8_t *)packet), sizeof(dev->name) - 1);
                    } else {
                    remove_dev:
                        btstack_remove_dev(&dev->use_as__btstack_dev_t, &it);
                        btstack_host_free_dev(dev);
                        break;
                    }

                    if (btstack_host_is_dev_supported(dev)) {
                        btstack_host_trace_dev(dev);
                    }
                    if (!btstack_evthandler(BTSTACK_HOST_ON_INQUIRY_RESULT, dev)) {
                        goto remove_dev;
                    }
                    break;
                }
            }
            // fall through
        case GAP_EVENT_INQUIRY_COMPLETE:
            btstack_get_devs(&it);
            while (btstack_linked_list_iterator_has_next(&it)) {
                dev = (btstack_host_dev_t *)btstack_linked_list_iterator_next(&it);
                if (dev->request_name) {
                    gap_remote_name_request(dev->remote_addr, dev->page_scan_repetition_mode, dev->clock_offset | 0x8000);
                    return;
                }
            }

            btstack_evthandler(BTSTACK_HOST_ON_INQUIRY_COMPLETE, NULL);
            break;
        }
        break;
    }
}

void btstack_host_init(void)
{
    btstack_host.drv_list = NULL;
}

void btstack_host_register_drv(btstack_host_drv_t *drv)
{
    btstack_linked_list_add(&btstack_host.drv_list, &drv->drv_node);
}

bool btstack_host_is_dev_supported(btstack_host_dev_t *dev)
{
    return btstack_host_get_drv_for_dev(dev) != NULL;
}

int btstack_host_remove_dev(btstack_host_dev_t *dev, btstack_linked_list_iterator_t *it)
{
    int ret = btstack_remove_dev(&dev->use_as__btstack_dev_t, it);
    if (!ret) {
        btstack_host_free_dev(dev);
    }
    return ret;
}

int btstack_host_connect_dev(btstack_host_dev_t *dev)
{
    const btstack_host_drv_op_t *op = btstack_host_get_drv_for_dev(dev);
    if (op != NULL) {
        dev->op = &op->use_as__btstack_drv_op_t;
        btstack_init_dev(&dev->use_as__btstack_dev_t);
        return btstack_connect_dev(&dev->use_as__btstack_dev_t);
    }
    return -1;
}

int btstack_host_scan(uint_fast8_t duration_in_1280ms_units)
{
    gap_inquiry_start(duration_in_1280ms_units);
    return 0;
}

#endif      // BTSTACK_OO_USE_HOST
