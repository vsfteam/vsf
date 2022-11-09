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

#define usb_device_speed            usb_device_speed_t
#define usb_descriptor_header       usb_descriptor_header_t
#define usb_device_descriptor       usb_device_desc_t
#define usb_config_descriptor       usb_config_desc_t
#define usb_interface_descriptor    usb_interface_desc_t
#define usb_endpoint_descriptor     usb_endpoint_desc_t

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
