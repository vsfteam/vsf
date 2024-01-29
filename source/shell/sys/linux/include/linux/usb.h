#ifndef __VSF_LINUX_USB_H__
#define __VSF_LINUX_USB_H__

#include "component/usb/vsf_usb.h"
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/pm.h>
#include <linux/jiffies.h>

#include <linux/usb/ch9.h>

#ifdef __cplusplus
extern "C" {
#endif

#define module_usb_driver(__usb_driver)                                         \
            module_driver(__usb_driver, usb_register, usb_deregister)
#define module_usb_driver_init(__usb_driver)                                    \
            module_driver_init(__usb_driver)

#define USB_DEVICE_ID_MATCH_VENDOR          VSF_USBH_MATCH_FLAGS_VENDOR
#define USB_DEVICE_ID_MATCH_PRODUCT         VSF_USBH_MATCH_FLAGS_PRODUCT
#define USB_DEVICE_ID_MATCH_DEVICE          (USB_DEVICE_ID_MATCH_VENDOR | USB_DEVICE_ID_MATCH_PRODUCT)
#define USB_DEVICE_ID_MATCH_DEV_CLASS       VSF_USBH_MATCH_FLAGS_DEV_CLASS
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS    VSF_USBH_MATCH_FLAGS_DEV_SUBCLASS
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL    VSF_USBH_MATCH_FLAGS_DEV_PROTOCOL
#define USB_DEVICE_ID_MATCH_DEV_INFO        (USB_DEVICE_ID_MATCH_DEV_CLASS | USB_DEVICE_ID_MATCH_DEV_SUBCLASS | USB_DEVICE_ID_MATCH_DEV_PROTOCOL)
#define USB_DEVICE_ID_MATCH_INT_CLASS       VSF_USBH_MATCH_FLAGS_IFS_CLASS
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS    VSF_USBH_MATCH_FLAGS_IFS_SUBCLASS
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL    VSF_USBH_MATCH_FLAGS_IFS_PROTOCOL
#define USB_DEVICE_ID_MATCH_INT_INFO        (USB_DEVICE_ID_MATCH_INT_CLASS | USB_DEVICE_ID_MATCH_INT_SUBCLASS | USB_DEVICE_ID_MATCH_INT_PROTOCOL)
#define USB_DEVICE_ID_MATCH_INT_NUMBER      VSF_USBH_MATCH_FLAGS_IFS_NUM
#define usb_device_id               vk_usbh_dev_id_t
#define USB_DEVICE(__vid, __pid)                                                \
            .match_flags            = USB_DEVICE_ID_MATCH_DEVICE,               \
            .idVendor               = (__vid),                                  \
            .idProduct              = (__pid),

#define URB_SHORT_NOT_OK            0x0001
#define URB_ISO_ASAP                0x0002
#define URB_NO_TRANSFER_DMA_MAP     0x0004
#define URB_ZERO_PACKET             0x0040
#define URB_NO_INTERRUPT            0x0080
#define URB_FREE_BUFFER             0x0100

struct usb_host_endpoint {
    struct usb_endpoint_descriptor  desc;

    unsigned char                   *extra;
    int                             extralen;
};

struct usb_host_interface {
    struct usb_interface_descriptor desc;
    int                             extralen;
    unsigned char                   *extra;
    struct usb_host_endpoint        *endpoint;
    char                            *string;
};

struct usb_interface_cache {
    unsigned num_altsetting;
    struct usb_host_interface altsetting[];
};

struct usb_interface {
    struct usb_host_interface       *altsetting;
    struct usb_host_interface       *cur_altsetting;
    unsigned                        num_altsetting;

    struct device                   dev;
    struct device                   *usb_dev;

    unsigned needs_remote_wakeup    : 1;
};
#define    to_usb_interface(__d)       vsf_container_of(__d, struct usb_interface, dev)
static inline void *usb_get_intfdata(struct usb_interface *intf)
{
    return dev_get_drvdata(&intf->dev);
}

static inline void usb_set_intfdata(struct usb_interface *intf, void *data)
{
    dev_set_drvdata(&intf->dev, data);
}

#define USB_MAXENDPOINTS            30
#define USB_MAXINTERFACES           32
#define USB_MAXIADS                 (USB_MAXINTERFACES / 2)
struct usb_host_config {
    struct usb_config_descriptor    desc;
    char                            *string;
    struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];
    struct usb_interface            *interface[USB_MAXINTERFACES];
    struct usb_interface_cache      *intf_cache[USB_MAXINTERFACES];
    unsigned char                   *extra;
    int extralen;
};

struct usb_anchor {
    struct list_head                urb_list;
    wait_queue_head_t               wait;
    atomic_t                        suspend_wakeups;
    bool                            poisoned;
};

static inline void init_usb_anchor(struct usb_anchor *anchor)
{
    memset(anchor, 0, sizeof(*anchor));
    INIT_LIST_HEAD(&anchor->urb_list);
    init_waitqueue_head(&anchor->wait);
}

static inline int usb_anchor_empty(struct usb_anchor *anchor)
{
    return list_empty(&anchor->urb_list);
}

struct usb_iso_packet_descriptor {
    unsigned int                    offset;
    unsigned int                    length;
    unsigned int                    actual_length;
    int                             status;
};

struct urb;
struct usb_device {
    vk_usbh_t                       *__host;
    vk_usbh_dev_t                   *__dev;

    int                             devnum;
    char                            devpath[16];
    enum usb_device_speed           speed;

    struct usb_host_config          *actconfig;
    struct usb_host_endpoint        *ep_in[16];
    struct usb_host_endpoint        *ep_out[16];

    struct device                   dev;
};
#define to_usb_device(__d)          vsf_container_of(__d, struct usb_device, dev)
static inline struct usb_device * interface_to_usbdev(struct usb_interface *intf)
{
    return to_usb_device(intf->dev.parent);
}

typedef void (*usb_complete_t)(struct urb *);
struct urb {
    vk_usbh_urb_t                   __urb;

    struct kref                     kref;
    struct list_head                urb_list;
    struct list_head                anchor_list;
    struct usb_anchor               *anchor;
    struct usb_device               *dev;

    unsigned int                    pipe;
    int                             status;
    unsigned int                    transfer_flags;
    void                            *transfer_buffer;
    u32                             transfer_buffer_length;
    u32                             actual_length;
    unsigned char                   *setup_packet;

    int                             start_frame;
    int                             number_of_packets;
    int                             interval;

    dma_addr_t                      transfer_dma;

    short                           error_count;
    bool                            is_submitted;
    vsf_eda_t                       *eda_pending;
    void                            *context;
    usb_complete_t                  complete;
    struct work_struct              done_work;

    struct usb_iso_packet_descriptor    iso_frame_desc[];
};

struct usbdrv_wrap {
    struct device_driver            driver;
};

struct usb_driver {
    const char                      *name;

    int (*probe)(struct usb_interface *intf, const struct usb_device_id *id);
    void (*disconnect)(struct usb_interface *intf);
    int (*suspend)(struct usb_interface *intf, pm_message_t message);
    int (*resume)(struct usb_interface *intf);

    struct usbdrv_wrap              drvwrap;

    const struct usb_device_id      *id_table;
    bool                            no_dynamic_id;
    bool                            supports_autosuspend;
    bool                            disable_hub_initiated_lpm;
    bool                            soft_unbind;
};

#define PIPE_ISOCHRONOUS            USB_ENDPOINT_XFER_ISOC
#define PIPE_INTERRUPT              USB_ENDPOINT_XFER_INT
#define PIPE_CONTROL                USB_ENDPOINT_XFER_CONTROL
#define PIPE_BULK                   USB_ENDPOINT_XFER_BULK

#define usb_pipein(__pipe)          ({                                          \
            vk_usbh_pipe_flag_t flag;                                           \
            flag.value = (__pipe);                                              \
            flag.dir_in1out0;                                                   \
        })
#define usb_pipeout(__pipe)         (!usb_pipein(__pipe))
#define usb_pipedevice(__pipe)      ({                                          \
            vk_usbh_pipe_flag_t flag;                                           \
            flag.value = (__pipe);                                              \
            flag.address;                                                       \
        })
#define usb_pipeendpoint(__pipe)    ({                                          \
            vk_usbh_pipe_flag_t flag;                                           \
            flag.value = (__pipe);                                              \
            flag.endpoint;                                                      \
        })
#define usb_pipetype(__pipe)        ({                                          \
            vk_usbh_pipe_flag_t flag;                                           \
            flag.value = (__pipe);                                              \
            flag.type;                                                          \
        })
#define usb_pipeisoc(__pipe)        (usb_pipetype(__pipe) == PIPE_ISOCHRONOUS)
#define usb_pipeint(__pipe)         (usb_pipetype(__pipe) == PIPE_INTERRUPT)
#define usb_pipeCONTROL(__pipe)     (usb_pipetype(__pipe) == PIPE_CONTROL)
#define usb_pipebulk(__pipe)        (usb_pipetype(__pipe) == PIPE_BULK)

extern uint32_t __vk_usbh_get_pipe_value(vk_usbh_dev_t *dev, uint8_t endpoint, uint8_t type, uint16_t size);
#define usb_sndctrlpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint, PIPE_CONTROL, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_rcvctrlpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint | USB_DIR_IN, PIPE_CONTROL, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_sndisocpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint, PIPE_ISOCHRONOUS, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_rcvisocpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint | USB_DIR_IN, PIPE_ISOCHRONOUS, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_sndbulkpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint, PIPE_BULK, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_rcvbulkpipe(__udev, __endpoint)         __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint | USB_DIR_IN, PIPE_BULK, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_sndintpipe(__udev, __endpoint)          __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint, PIPE_INTERRUPT, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
#define usb_rcvintpipe(__udev, __endpoint)          __vk_usbh_get_pipe_value((__udev)->__dev, __endpoint | USB_DIR_IN, PIPE_INTERRUPT, (__udev)->ep_out[__endpoint]->desc.wMaxPacketSize)
static inline struct usb_host_endpoint * usb_pipe_endpoint(struct usb_device *udev, unsigned int pipe)
{
    struct usb_host_endpoint **eps = usb_pipein(pipe) ? udev->ep_in : udev->ep_out;
    return eps[usb_pipeendpoint(pipe)];
}

static inline void usb_fill_control_urb(struct urb *urb,
                        struct usb_device *dev,
                        unsigned int pipe,
                        unsigned char *setup_packet,
                        void *transfer_buffer,
                        int buffer_length,
                        usb_complete_t complete_fn,
                        void *context)
{
    urb->dev                    = dev;
    urb->pipe                   = pipe;
    urb->setup_packet           = setup_packet;
    urb->transfer_buffer        = transfer_buffer;
    urb->transfer_buffer_length = buffer_length;
    urb->complete               = complete_fn;
    urb->context                = context;
}

static inline void usb_fill_bulk_urb(struct urb *urb,
                        struct usb_device *dev,
                        unsigned int pipe,
                        void *transfer_buffer,
                        int buffer_length,
                        usb_complete_t complete_fn,
                        void *context)
{
    urb->dev                    = dev;
    urb->pipe                   = pipe;
    urb->transfer_buffer        = transfer_buffer;
    urb->transfer_buffer_length = buffer_length;
    urb->complete               = complete_fn;
    urb->context                = context;
}

static inline void usb_fill_int_urb(struct urb *urb,
                        struct usb_device *dev,
                        unsigned int pipe,
                        void *transfer_buffer,
                        int buffer_length,
                        usb_complete_t complete_fn,
                        void *context,
                        int interval)
{
    urb->dev                    = dev;
    urb->pipe                   = pipe;
    urb->transfer_buffer        = transfer_buffer;
    urb->transfer_buffer_length = buffer_length;
    urb->complete               = complete_fn;
    urb->context                = context;

    if (dev->speed == USB_SPEED_HIGH || dev->speed >= USB_SPEED_SUPER) {
        interval = vsf_min(vsf_max(interval, 1), 16);
        urb->interval = 1 << (interval - 1);
    } else {
        urb->interval = interval;
    }

    urb->start_frame = -1;
}

extern int usb_reset_device(struct usb_device *udev);
#define usb_enable_autosuspend(__udev)
#define usb_disable_autosuspend(__udev)
#define usb_autopm_get_interface(__udev)            0
#define usb_autopm_put_interface(__udev)            0

static inline void * usb_alloc_coherent(struct usb_device *dev, size_t size, gfp_t mem_flags, dma_addr_t *dma)
{
    void *buffer = vsf_usbh_malloc(size);
    if (dma) {
        *dma = (dma_addr_t)buffer;
    }
    return buffer;
}
static inline void usb_free_coherent(struct usb_device *dev, size_t size, void *addr, dma_addr_t dma)
{
    vsf_usbh_free(addr);
}


extern void usb_init_urb(struct urb *urb);
extern struct urb * usb_alloc_urb(int iso_packets, gfp_t mem_flags);
extern void usb_free_urb(struct urb *urb);
#define usb_put_urb usb_free_urb
extern struct urb * usb_get_urb(struct urb *urb);
extern int usb_submit_urb(struct urb *urb, gfp_t mem_flags);
extern int usb_unlink_urb(struct urb *urb);
extern void usb_kill_urb(struct urb *urb);
extern void usb_poison_urb(struct urb *urb);
extern void usb_unpoison_urb(struct urb *urb);
extern void usb_block_urb(struct urb *urb);
extern void usb_kill_anchored_urbs(struct usb_anchor *anchor);
extern void usb_poison_anchored_urbs(struct usb_anchor *anchor);
extern void usb_unpoison_anchored_urbs(struct usb_anchor *anchor);
extern void usb_unlink_anchored_urbs(struct usb_anchor *anchor);
extern void usb_anchor_suspend_wakeups(struct usb_anchor *anchor);
extern void usb_anchor_resume_wakeups(struct usb_anchor *anchor);
extern void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor);
extern void usb_unanchor_urb(struct urb *urb);
extern int usb_wait_anchor_empty_timeout(struct usb_anchor *anchor,
                     unsigned int timeout);
extern struct urb * usb_get_from_anchor(struct usb_anchor *anchor);
extern void usb_scuttle_anchored_urbs(struct usb_anchor *anchor);
extern int usb_anchor_empty(struct usb_anchor *anchor);

extern int usb_urb_dir_in(struct urb *urb);
static inline int usb_urb_dir_out(struct urb *urb)
{
    return !usb_urb_dir_in(urb);
}

extern int usb_control_msg(struct usb_device *udev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout);
extern int usb_control_msg_send(struct usb_device *udev, __u8 endpoint, __u8 request, __u8 requesttype, __u16 value, __u16 index, const void *data, __u16 size, int timeout, gfp_t memflags);
extern int usb_control_msg_recv(struct usb_device *udev, __u8 endpoint, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout, gfp_t memflags);
extern int usb_interrupt_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout);
extern int usb_bulk_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout);

extern int usb_get_descriptor(struct usb_device *udev, unsigned char type, unsigned char index, void *buf, int size);
extern int usb_get_status(struct usb_device *udev, int recip, int type, int target, void *data);
static inline int usb_get_std_status(struct usb_device *udev, int recip, int target, void *data)
{
    return usb_get_status(udev, recip, 0, target, data);
}
extern int usb_string(struct usb_device *udev, int index, char *buf, size_t size);
extern int usb_clear_halt(struct usb_device *udev, int pipe);
extern int usb_reset_configuration(struct usb_device *udev);
extern int usb_set_interface(struct usb_device *udev, int ifnum, int alternate);
extern int usb_reset_endpoint(struct usb_device *udev, unsigned int epaddr);
extern int usb_set_configuration(struct usb_device *dev, int configuration);

extern void usb_register_driver(struct usb_driver *drv, struct module *mod, const char *name);
extern void usb_deregister_driver(struct usb_driver *drv);
#define usb_register(__drv)                         usb_register_driver((__drv), THIS_MODULE, KBUILD_MODNAME)
#define usb_deregister(__drv)                       usb_deregister_driver((__drv))

extern int usb_driver_claim_interface(struct usb_driver *driver, struct usb_interface *iface, void *data);
extern void usb_driver_release_interface(struct usb_driver *driver, struct usb_interface *iface);
static inline int usb_interface_claimed(struct usb_interface *iface)
{
    return (iface->dev.driver != NULL);
}

extern struct usb_interface * usb_find_interface(struct usb_driver *drv, int minor);
extern struct usb_interface * usb_ifnum_to_if(const struct usb_device *dev, unsigned ifnum);
extern struct usb_host_interface * usb_altnum_to_altsetting(const struct usb_interface *intf, unsigned int altnum);
extern struct usb_host_interface * usb_find_alt_setting(struct usb_host_config *config, unsigned int iface_num, unsigned int alt_num);

extern int usb_find_common_endpoints(struct usb_host_interface *alt,
        struct usb_endpoint_descriptor **bulk_in,
        struct usb_endpoint_descriptor **bulk_out,
        struct usb_endpoint_descriptor **int_in,
        struct usb_endpoint_descriptor **int_out);

extern int usb_find_common_endpoints_reverse(struct usb_host_interface *alt,
        struct usb_endpoint_descriptor **bulk_in,
        struct usb_endpoint_descriptor **bulk_out,
        struct usb_endpoint_descriptor **int_in,
        struct usb_endpoint_descriptor **int_out);
static inline int usb_find_bulk_in_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **bulk_in)
{
    return usb_find_common_endpoints(alt, bulk_in, NULL, NULL, NULL);
}

static inline int usb_find_bulk_out_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **bulk_out)
{
    return usb_find_common_endpoints(alt, NULL, bulk_out, NULL, NULL);
}

static inline int usb_find_int_in_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **int_in)
{
    return usb_find_common_endpoints(alt, NULL, NULL, int_in, NULL);
}

static inline int usb_find_int_out_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **int_out)
{
    return usb_find_common_endpoints(alt, NULL, NULL, NULL, int_out);
}

static inline int usb_find_last_bulk_in_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **bulk_in)
{
    return usb_find_common_endpoints_reverse(alt, bulk_in, NULL, NULL, NULL);
}

static inline int usb_find_last_bulk_out_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **bulk_out)
{
    return usb_find_common_endpoints_reverse(alt, NULL, bulk_out, NULL, NULL);
}

static inline int usb_find_last_int_in_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **int_in)
{
    return usb_find_common_endpoints_reverse(alt, NULL, NULL, int_in, NULL);
}

static inline int usb_find_last_int_out_endpoint(struct usb_host_interface *alt, struct usb_endpoint_descriptor **int_out)
{
    return usb_find_common_endpoints_reverse(alt, NULL, NULL, NULL, int_out);
}

extern void vsf_linux_usb_init(vk_usbh_t *usbh);

#ifdef __cplusplus
}
#endif

#endif
