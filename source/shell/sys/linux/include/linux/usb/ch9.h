#ifndef __VSF_LINUX_USB_CH9_H__
#define __VSF_LINUX_USB_CH9_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USB_ENDPOINT_DIR_MASK       USB_DIR_MASK
#define USB_ENDPOINT_MAXP_MASK      0x07FF
#define USB_ENDPOINT_NUMBER_MASK    0x7F

#define USB_STATUS_TYPE_STANDARD    0
#define USB_STATUS_TYPE_PTM         1

struct usb_descriptor_header {
    __u8  bLength;
    __u8  bDescriptorType;
} __packed;

struct usb_device_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __le16 bcdUSB;
    __u8  bDeviceClass;
    __u8  bDeviceSubClass;
    __u8  bDeviceProtocol;
    __u8  bMaxPacketSize0;
    __le16 idVendor;
    __le16 idProduct;
    __le16 bcdDevice;
    __u8  iManufacturer;
    __u8  iProduct;
    __u8  iSerialNumber;
    __u8  bNumConfigurations;
} __packed;

struct usb_config_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __le16 wTotalLength;
    __u8  bNumInterfaces;
    __u8  bConfigurationValue;
    __u8  iConfiguration;
    __u8  bmAttributes;
    __u8  bMaxPower;
} __packed;

struct usb_interface_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __u8  bInterfaceNumber;
    __u8  bAlternateSetting;
    __u8  bNumEndpoints;
    __u8  bInterfaceClass;
    __u8  bInterfaceSubClass;
    __u8  bInterfaceProtocol;
    __u8  iInterface;
} __packed;

struct usb_endpoint_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;

    __u8  bEndpointAddress;
    __u8  bmAttributes;
    __le16 wMaxPacketSize;
    __u8  bInterval;
    __u8  bRefresh;
    __u8  bSynchAddress;
} __packed;

static inline int usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
    return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}
static inline int usb_endpoint_type(const struct usb_endpoint_descriptor *epd)
{
    return epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
}
static inline int usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}
static inline int usb_endpoint_dir_out(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT);
}
static inline int usb_endpoint_xfer_bulk(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK);
}
static inline int usb_endpoint_xfer_control(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_CONTROL);
}
static inline int usb_endpoint_xfer_int( const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT);
}
static inline int usb_endpoint_xfer_isoc(const struct usb_endpoint_descriptor *epd)
{
    return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_ISOC);
}

static inline int usb_endpoint_is_bulk_in(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_in(epd);
}
static inline int usb_endpoint_is_bulk_out(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_out(epd);
}
static inline int usb_endpoint_is_int_in(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_int(epd) && usb_endpoint_dir_in(epd);
}
static inline int usb_endpoint_is_int_out(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_int(epd) && usb_endpoint_dir_out(epd);
}
static inline int usb_endpoint_is_isoc_in(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_in(epd);
}
static inline int usb_endpoint_is_isoc_out(const struct usb_endpoint_descriptor *epd)
{
    return usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_out(epd);
}

static inline int usb_endpoint_maxp(const struct usb_endpoint_descriptor *epd)
{
    return le16_to_cpu(epd->wMaxPacketSize) & USB_ENDPOINT_MAXP_MASK;
}

#ifdef __cplusplus
}
#endif

#endif
