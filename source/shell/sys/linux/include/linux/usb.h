#ifndef __VSF_LINUX_USB_H__
#define __VSF_LINUX_USB_H__

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#include "component/usb/vsf_usb.h"
#include <linux/types.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/device.h>
#include <linux/pm.h>
#include <linux/module.h>

#include <linux/usb/ch9.h>

#ifdef __cplusplus
extern "C" {
#endif

#define module_usb_driver(__usb_driver)

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

struct usb_interface {
    struct usb_host_interface       *altsetting;
    struct usb_host_interface       *cur_altsetting;
    unsigned                        num_altsetting;

    struct device                   dev;
    struct device                   *usb_dev;

    unsigned needs_remote_wakeup    : 1;
};

struct usb_anchor {
    struct list_head                urb_list;
    wait_queue_head_t               wait;
};

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

    struct usb_host_config          *actconfig;
    struct usb_host_endpoint        *ep_in[16];
    struct usb_host_endpoint        *ep_out[16];

    struct device                   dev;
};

typedef void (*usb_complete_t)(struct urb *);
struct urb {
    vk_usbh_urb_t                   __urb;

    struct list_head                urb_list;
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

    int                             error_count;
    void                            *context;
    usb_complete_t                  complete;

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

#define usb_pipein(__pipe)          (((vk_usbh_pipe_flag_t)(__pipe)).dir_in1out0)
#define usb_pipeout(__pipe)         (!usb_pipein(__pipe))
#define usb_pipedevice(__pipe)      (((vk_usbh_pipe_flag_t)(__pipe)).address)
#define usb_pipeendpoint(__pipe)    (((vk_usbh_pipe_flag_t)(__pipe)).endpoint)
#define usb_pipetype(__pipe)        (((vk_usbh_pipe_flag_t)(__pipe)).type)
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

extern void usb_init_urb(struct urb *urb);
extern struct urb * usb_alloc_urb(int iso_packets, gfp_t flags);
extern void usb_free_urb(struct urb *urb);
extern int usb_submit_urb(struct urb *urb, gfp_t flags);
extern int usb_unlink_urb(struct urb *urb);
static inline int usb_urb_dir_in(struct urb *urb)
{
    return vk_usbh_urb_get_pipe(&urb->__urb).dir_in1out0;
}
static inline int usb_urb_dir_out(struct urb *urb)
{
    return !vk_usbh_urb_get_pipe(&urb->__urb).dir_in1out0;
}

extern int usb_control_msg(struct usb_device *udev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout);
extern int usb_interrupt_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout);
extern int usb_bulk_msg(struct usb_device *udev, unsigned int pipe, void *data, int len, int *actual_length, int timeout);

extern int usb_get_descriptor(struct usb_device *udev, unsigned char desctype, unsigned char descindex, void *buf, int size);
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

#ifdef __cplusplus
}
#endif

#endif
