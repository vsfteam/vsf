#ifndef __VSF_LINUX_LIBUSB_H__
#define __VSF_LINUX_LIBUSB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./poll.h"
#   include "./sys/time.h"
#   include "./unistd.h"
#else
#   include <poll.h>
#   include <sys/time.h>
#   include <unistd.h>
#endif

// for USB constants
#include "component/usb/common/usb_common.h"
// for endian APIs
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_LIBUSB_CFG_WRAPPER == ENABLED
#define VSF_LINUX_LIBUSB_WRAPPER(__api)                 VSF_SHELL_WRAPPER(vsf_linux_libusb, __api)

#define libusb_get_version                              VSF_LINUX_LIBUSB_WRAPPER(libusb_get_version)
#define libusb_init                                     VSF_LINUX_LIBUSB_WRAPPER(libusb_init)
#define libusb_exit                                     VSF_LINUX_LIBUSB_WRAPPER(libusb_exit)
#define libusb_set_debug                                VSF_LINUX_LIBUSB_WRAPPER(libusb_set_debug)
#define libusb_error_name                               VSF_LINUX_LIBUSB_WRAPPER(libusb_error_name)
#define libusb_get_device_list                          VSF_LINUX_LIBUSB_WRAPPER(libusb_get_device_list)
#define libusb_free_device_list                         VSF_LINUX_LIBUSB_WRAPPER(libusb_free_device_list)
#define libusb_open                                     VSF_LINUX_LIBUSB_WRAPPER(libusb_open)
#define libusb_open_device_with_vid_pid                 VSF_LINUX_LIBUSB_WRAPPER(libusb_open_device_with_vid_pid)
#define libusb_get_device                               VSF_LINUX_LIBUSB_WRAPPER(libusb_get_device)
#define libusb_close                                    VSF_LINUX_LIBUSB_WRAPPER(libusb_close)
#define libusb_get_device_descriptor                    VSF_LINUX_LIBUSB_WRAPPER(libusb_get_device_descriptor)
#define libusb_get_device_address                       VSF_LINUX_LIBUSB_WRAPPER(libusb_get_device_address)
#define libusb_get_bus_number                           VSF_LINUX_LIBUSB_WRAPPER(libusb_get_bus_number)
#define libusb_alloc_transfer                           VSF_LINUX_LIBUSB_WRAPPER(libusb_alloc_transfer)
#define libusb_fill_control_transfer                    VSF_LINUX_LIBUSB_WRAPPER(libusb_fill_control_transfer)
#define libusb_fill_bulk_transfer                       VSF_LINUX_LIBUSB_WRAPPER(libusb_fill_bulk_transfer)
#define libusb_fill_interrupt_transfer                  VSF_LINUX_LIBUSB_WRAPPER(libusb_fill_interrupt_transfer)
#define libusb_fill_iso_transfer                        VSF_LINUX_LIBUSB_WRAPPER(libusb_fill_iso_transfer)
#define libusb_get_iso_packet_buffer_simple             VSF_LINUX_LIBUSB_WRAPPER(libusb_get_iso_packet_buffer_simple)
#define libusb_set_iso_packet_lengths                   VSF_LINUX_LIBUSB_WRAPPER(libusb_set_iso_packet_lengths)
#define libusb_free_transfer                            VSF_LINUX_LIBUSB_WRAPPER(libusb_free_transfer)
#define libusb_submit_transfer                          VSF_LINUX_LIBUSB_WRAPPER(libusb_submit_transfer)
#define libusb_cancel_transfer                          VSF_LINUX_LIBUSB_WRAPPER(libusb_cancel_transfer)
#define libusb_control_transfer                         VSF_LINUX_LIBUSB_WRAPPER(libusb_control_transfer)
#define libusb_bulk_transfer                            VSF_LINUX_LIBUSB_WRAPPER(libusb_bulk_transfer)
#define libusb_interrupt_transfer                       VSF_LINUX_LIBUSB_WRAPPER(libusb_interrupt_transfer)
#define libusb_get_string_descriptor_ascii              VSF_LINUX_LIBUSB_WRAPPER(libusb_get_string_descriptor_ascii)
#define libusb_get_config_descriptor                    VSF_LINUX_LIBUSB_WRAPPER(libusb_get_config_descriptor)
#define libusb_get_config_descriptor_by_value           VSF_LINUX_LIBUSB_WRAPPER(libusb_get_config_descriptor_by_value)
#define libusb_get_active_config_descriptor             VSF_LINUX_LIBUSB_WRAPPER(libusb_get_active_config_descriptor)
#define libusb_free_config_descriptor                   VSF_LINUX_LIBUSB_WRAPPER(libusb_free_config_descriptor)
#define libusb_get_descriptor                           VSF_LINUX_LIBUSB_WRAPPER(libusb_get_descriptor)
#define libusb_get_string_descriptor                    VSF_LINUX_LIBUSB_WRAPPER(libusb_get_string_descriptor)
#define libusb_set_interface_alt_setting                VSF_LINUX_LIBUSB_WRAPPER(libusb_set_interface_alt_setting)
#define libusb_handle_events_timeout_completed          VSF_LINUX_LIBUSB_WRAPPER(libusb_handle_events_timeout_completed)
#define libusb_handle_events_completed                  VSF_LINUX_LIBUSB_WRAPPER(libusb_handle_events_completed)
#define libusb_handle_events_timeout                    VSF_LINUX_LIBUSB_WRAPPER(libusb_handle_events_timeout)
#define libusb_handle_events                            VSF_LINUX_LIBUSB_WRAPPER(libusb_handle_events)
#define libusb_get_pollfds                              VSF_LINUX_LIBUSB_WRAPPER(libusb_get_pollfds)
#define libusb_free_pollfds                             VSF_LINUX_LIBUSB_WRAPPER(libusb_free_pollfds)
#define libusb_get_ss_endpoint_companion_descriptor     VSF_LINUX_LIBUSB_WRAPPER(libusb_get_ss_endpoint_companion_descriptor)
#define libusb_free_ss_endpoint_companion_descriptor    VSF_LINUX_LIBUSB_WRAPPER(libusb_free_ss_endpoint_companion_descriptor)
#define libusb_claim_interface                          VSF_LINUX_LIBUSB_WRAPPER(libusb_claim_interface)
#define libusb_release_interface                        VSF_LINUX_LIBUSB_WRAPPER(libusb_release_interface)
#define libusb_hotplug_register_callback                VSF_LINUX_LIBUSB_WRAPPER(libusb_hotplug_register_callback)
#define libusb_hotplug_deregister_callback              VSF_LINUX_LIBUSB_WRAPPER(libusb_hotplug_deregister_callback)
#define libusb_get_next_timeout                         VSF_LINUX_LIBUSB_WRAPPER(libusb_get_next_timeout)
#define libusb_has_capability                           VSF_LINUX_LIBUSB_WRAPPER(libusb_has_capability)
#define libusb_attach_kernel_driver                     VSF_LINUX_LIBUSB_WRAPPER(libusb_attach_kernel_driver)
#define libusb_detach_kernel_driver                     VSF_LINUX_LIBUSB_WRAPPER(libusb_detach_kernel_driver)
#define libusb_kernel_driver_active                     VSF_LINUX_LIBUSB_WRAPPER(libusb_kernel_driver_active)
#define libusb_ref_device                               VSF_LINUX_LIBUSB_WRAPPER(libusb_ref_device)
#define libusb_unref_device                             VSF_LINUX_LIBUSB_WRAPPER(libusb_unref_device)
#define libusb_get_max_packet_size                      VSF_LINUX_LIBUSB_WRAPPER(libusb_get_max_packet_size)
#define libusb_get_device_speed                         VSF_LINUX_LIBUSB_WRAPPER(libusb_get_device_speed)
#define libusb_set_configuration                        VSF_LINUX_LIBUSB_WRAPPER(libusb_set_configuration)
#define libusb_get_configuration                        VSF_LINUX_LIBUSB_WRAPPER(libusb_get_configuration)
#endif

#define LIBUSB_CALL
#define LIBUSB_HOTPLUG_MATCH_ANY        -1

#define libusb_device_descriptor        usb_device_desc_t

enum libusb_error {
    LIBUSB_SUCCESS                      = 0,
    LIBUSB_ERROR_IO                     = -1,
    LIBUSB_ERROR_INVALID_PARAM          = -2,
    LIBUSB_ERROR_ACCESS                 = -3,
    LIBUSB_ERROR_NO_DEVICE              = -4,
    LIBUSB_ERROR_NOT_FOUND              = -5,
    LIBUSB_ERROR_BUSY                   = -6,
    LIBUSB_ERROR_TIMEOUT                = -7,
    LIBUSB_ERROR_OVERFLOW               = -8,
    LIBUSB_ERROR_PIPE                   = -9,
    LIBUSB_ERROR_INTERRUPTED            = -10,
    LIBUSB_ERROR_NO_MEM                 = -11,
    LIBUSB_ERROR_NOT_SUPPORTED          = -12,
    LIBUSB_ERROR_OTHER                  = -99,
};

enum libusb_capability {
    LIBUSB_CAP_HAS_CAPABILITY,
    LIBUSB_CAP_HAS_HOTPLUG,
    LIBUSB_CAP_HAS_HID_ACCESS,
    LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER,
};

enum libusb_speed {
    LIBUSB_SPEED_UNKNOWN                = USB_SPEED_UNKNOWN,
    LIBUSB_SPEED_LOW                    = USB_SPEED_LOW,
    LIBUSB_SPEED_FULL                   = USB_SPEED_FULL,
    LIBUSB_SPEED_HIGH                   = USB_SPEED_HIGH,
    LIBUSB_SPEED_SUPER                  = USB_SPEED_SUPER,
};

enum libusb_endpoint_direction {
    LIBUSB_ENDPOINT_IN                  = USB_DIR_IN,
    LIBUSB_ENDPOINT_OUT                 = USB_DIR_OUT,
    LIBUSB_ENDPOINT_DIR_MASK            = USB_DIR_MASK,
};

enum libusb_request_type {
    LIBUSB_REQUEST_TYPE_STANDARD        = USB_TYPE_STANDARD,
    LIBUSB_REQUEST_TYPE_CLASS           = USB_TYPE_CLASS,
    LIBUSB_REQUEST_TYPE_VENDOR          = USB_TYPE_VENDOR,
    LIBUSB_REQUEST_TYPE_RESERVED        = USB_TYPE_RESERVED,
};

enum libusb_request_recipient {
    LIBUSB_RECIPIENT_DEVICE             = USB_RECIP_DEVICE,
    LIBUSB_RECIPIENT_INTERFACE          = USB_RECIP_INTERFACE,
    LIBUSB_RECIPIENT_ENDPOINT           = USB_RECIP_ENDPOINT,
    LIBUSB_RECIPIENT_OTHER              = USB_RECIP_OTHER,
};

enum libusb_standard_request {
    LIBUSB_REQUEST_GET_STATUS           = USB_REQ_GET_STATUS,
    LIBUSB_REQUEST_CLEAR_FEATURE        = USB_REQ_CLEAR_FEATURE,
    LIBUSB_REQUEST_SET_FEATURE          = USB_REQ_SET_FEATURE,
    LIBUSB_REQUEST_SET_ADDRESS          = USB_REQ_SET_ADDRESS,
    LIBUSB_REQUEST_GET_DESCRIPTOR       = USB_REQ_GET_DESCRIPTOR,
    LIBUSB_REQUEST_SET_DESCRIPTOR       = USB_REQ_SET_DESCRIPTOR,
    LIBUSB_REQUEST_GET_CONFIGURATION    = USB_REQ_GET_CONFIGURATION,
    LIBUSB_REQUEST_SET_CONFIGURATION    = USB_REQ_SET_CONFIGURATION,
    LIBUSB_REQUEST_GET_INTERFACE        = USB_REQ_GET_INTERFACE,
    LIBUSB_REQUEST_SET_INTERFACE        = USB_REQ_SET_INTERFACE,
    LIBUSB_REQUEST_SYNCH_FRAME          = USB_REQ_SYNCH_FRAME,
};

 enum libusb_class_code {
    LIBUSB_CLASS_PER_INTERFACE          = 0x00,
    LIBUSB_CLASS_AUDIO                  = 0x01,
    LIBUSB_CLASS_COMM                   = 0x02,
    LIBUSB_CLASS_HID                    = 0x03,
    LIBUSB_CLASS_PHYSICAL               = 0x05,
    LIBUSB_CLASS_IMAGE                  = 0x06,
    LIBUSB_CLASS_PTP                    = 0x06,
    LIBUSB_CLASS_PRINTER                = 0x07,
    LIBUSB_CLASS_MASS_STORAGE           = 0x08,
    LIBUSB_CLASS_HUB                    = 0x09,
    LIBUSB_CLASS_DATA                   = 0x0A,
    LIBUSB_CLASS_SMART_CARD             = 0x0B,
    LIBUSB_CLASS_CONTENT_SECURITY       = 0x0D,
    LIBUSB_CLASS_VIDEO                  = 0x0E,
    LIBUSB_CLASS_PERSONAL_HEALTHCARE    = 0x0F,
    LIBUSB_CLASS_DIAGNOSTIC_DEVICE      = 0xDC,
    LIBUSB_CLASS_WIRELESS               = 0xE0,
    LIBUSB_CLASS_MISCELLANEOUS          = 0xEF,
    LIBUSB_CLASS_APPLICATION            = 0xFE,
    LIBUSB_CLASS_VENDOR_SPEC            = 0xFF,
};

enum libusb_descriptor_type {
    LIBUSB_DT_DEVICE                    = USB_DT_DEVICE,
    LIBUSB_DT_CONFIG                    = USB_DT_CONFIG,
    LIBUSB_DT_STRING                    = USB_DT_STRING,
    LIBUSB_DT_INTERFACE                 = USB_DT_INTERFACE,
    LIBUSB_DT_ENDPOINT                  = USB_DT_ENDPOINT,
    LIBUSB_DT_BOS                       = USB_DT_BOS,
    LIBUSB_DT_DEVICE_CAPABILITY         = USB_DT_DEVICE_CAPABILITY,
    LIBUSB_DT_SS_ENDPOINT_COMPANION     = USB_DT_SS_ENDPOINT_COMP,
};

enum libusb_descriptor_size {
    LIBUSB_DT_DEVICE_SIZE               = USB_DT_DEVICE_SIZE,
    LIBUSB_DT_CONFIG_SIZE               = USB_DT_CONFIG_SIZE,
    LIBUSB_DT_INTERFACE_SIZE            = USB_DT_INTERFACE_SIZE,
    LIBUSB_DT_ENDPOINT_SIZE             = USB_DT_ENDPOINT_SIZE,
    LIBUSB_DT_ENDPOINT_AUDIO_SIZE       = USB_DT_ENDPOINT_AUDIO_SIZE,
};

enum libusb_log_level {
    LIBUSB_LOG_LEVEL_NONE = 0,
    LIBUSB_LOG_LEVEL_ERROR = 1,
    LIBUSB_LOG_LEVEL_WARNING = 2,
    LIBUSB_LOG_LEVEL_INFO = 3,
    LIBUSB_LOG_LEVEL_DEBUG = 4
};

struct libusb_version {
    const uint16_t major;
    const uint16_t minor;
    const uint16_t micro;
    const uint16_t nano;
    const char *rc;
    const char *describe;
};

struct libusb_endpoint_descriptor {
    implement(usb_endpoint_desc_t)

    const unsigned char *extra;
    int extra_length;
};

struct libusb_interface_descriptor {
    implement(usb_interface_desc_t)

    const struct libusb_endpoint_descriptor *endpoint;
    const unsigned char *extra;
    int extra_length;
};

struct libusb_interface {
    const struct libusb_interface_descriptor *altsetting;
    int num_altsetting;
};

struct libusb_config_descriptor {
    implement(usb_config_desc_t)
    const unsigned char *desc;

    struct libusb_interface *interface;
    const unsigned char *extra;
    int extra_length;
};

struct libusb_control_setup {
    implement(usb_ctrlrequest_t)
};

#define LIBUSB_CONTROL_SETUP_SIZE       (sizeof(struct libusb_control_setup))

typedef int libusb_hotplug_callback_handle;

struct libusb_context {
    int dummy;
};
typedef struct libusb_context libusb_context;

struct libusb_device;
typedef struct libusb_device libusb_device;
struct libusb_device_handle;
typedef struct libusb_device_handle libusb_device_handle;

typedef enum {
    LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED = 1 << 1,
    LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT    = 1 << 2,
} libusb_hotplug_event;

typedef enum {
    LIBUSB_HOTPLUG_NO_FLAGS = 0,
    LIBUSB_HOTPLUG_ENUMERATE,
} libusb_hotplug_flag;

typedef int (*libusb_hotplug_callback_fn)(libusb_context *ctx,
        libusb_device *device, libusb_hotplug_event event, void *user_data);


struct libusb_pollfd {
    struct pollfd;
    vsf_linux_fd_t *sfd;
};

enum libusb_transfer_status {
    LIBUSB_TRANSFER_COMPLETED,
    LIBUSB_TRANSFER_ERROR,
    LIBUSB_TRANSFER_CANCELLED,
    LIBUSB_TRANSFER_NO_DEVICE,
    LIBUSB_TRANSFER_TIMED_OUT,
    LIBUSB_TRANSFER_STALL,
    LIBUSB_TRANSFER_OVERFLOW,
};

enum libusb_transfer_type {
    LIBUSB_TRANSFER_TYPE_CONTROL        = USB_ENDPOINT_XFER_CONTROL,
    LIBUSB_TRANSFER_TYPE_ISOCHRONOUS    = USB_ENDPOINT_XFER_ISOC,
    LIBUSB_TRANSFER_TYPE_BULK           = USB_ENDPOINT_XFER_BULK,
    LIBUSB_TRANSFER_TYPE_INTERRUPT      = USB_ENDPOINT_XFER_INT,
    LIBUSB_TRANSFER_TYPE_MASK           = USB_ENDPOINT_XFERTYPE_MASK,
};

enum libusb_transfer_flags {
    LIBUSB_TRANSFER_SHORT_NOT_OK = (1U << 0),
    LIBUSB_TRANSFER_FREE_BUFFER = (1U << 1),
    LIBUSB_TRANSFER_FREE_TRANSFER = (1U << 2),
    LIBUSB_TRANSFER_ADD_ZERO_PACKET = (1U << 3),
};

struct libusb_iso_packet_descriptor {
    unsigned int length;
    unsigned int actual_length;
    enum libusb_transfer_status status;
};

struct libusb_transfer;
typedef void (*libusb_transfer_cb_fn)(struct libusb_transfer *transfer);
struct libusb_transfer {
    libusb_device_handle *dev_handle;
    uint8_t flags;
    unsigned char endpoint;
    unsigned char type;
    unsigned int timeout;
    enum libusb_transfer_status status;
    int length;
    int actual_length;
    libusb_transfer_cb_fn callback;
    void *user_data;
    unsigned char *buffer;
    int num_iso_packets;
    struct libusb_iso_packet_descriptor iso_packet_desc[0];
};

const char * libusb_strerror(int code);
const struct libusb_version * libusb_get_version(void);
int libusb_init(libusb_context **context);
void libusb_exit(libusb_context *ctx);
void libusb_set_debug(libusb_context *ctx, int level);
const char * libusb_error_name(int errcode);
ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device *** list);
void libusb_free_device_list(libusb_device **list, int unref_devices);
int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc);
int libusb_open(libusb_device *dev, libusb_device_handle **dev_handle);
libusb_device * libusb_get_device(libusb_device_handle *dev_handle);
libusb_device_handle * libusb_open_device_with_vid_pid(libusb_context *ctx,
        uint16_t vendor_id, uint16_t product_id);
void libusb_close(libusb_device_handle *dev_handle);
uint8_t libusb_get_device_address(libusb_device *dev);
uint8_t libusb_get_bus_number(libusb_device *dev);
int libusb_release_interface(libusb_device_handle *dev_handle, int interface_number);
int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number);

struct libusb_transfer *libusb_alloc_transfer(int iso_packets);
void libusb_free_transfer(struct libusb_transfer *transfer);
int libusb_submit_transfer(struct libusb_transfer *transfer);
int libusb_cancel_transfer(struct libusb_transfer *transfer);
int libusb_control_transfer(libusb_device_handle *dev_handle,
    uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
    unsigned char *data, uint16_t wLength, unsigned int timeout);
int libusb_bulk_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout);
int libusb_interrupt_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout);

int libusb_hotplug_register_callback(libusb_context *ctx,
        libusb_hotplug_event events,
        libusb_hotplug_flag flags,
        int vendor_id,
        int product_id,
        int dev_class,
        libusb_hotplug_callback_fn cb_fn,
        void *user_data,
        libusb_hotplug_callback_handle *callback_handle);
void libusb_hotplug_deregister_callback(libusb_context *ctx,
        libusb_hotplug_callback_handle callback_handle);

static inline void libusb_fill_control_transfer(
    struct libusb_transfer *transfer, libusb_device_handle *dev_handle,
    unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
    unsigned int timeout)
{
    // no idea why libusb_control_setup can not be used for c++ here
    struct usb_ctrlrequest_t *setup = (struct usb_ctrlrequest_t *)(void *)buffer;
    transfer->dev_handle = dev_handle;
    transfer->endpoint = 0;
    transfer->type = LIBUSB_TRANSFER_TYPE_CONTROL;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    if (setup)
        transfer->length = (int) (LIBUSB_CONTROL_SETUP_SIZE + le16_to_cpu(setup->wLength));
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_control_setup(unsigned char *buffer,
	uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	uint16_t wLength)
{
	struct libusb_control_setup *setup = (struct libusb_control_setup *)(void *)buffer;
	setup->bRequestType = bRequestType;
	setup->bRequest = bRequest;
	setup->wValue = cpu_to_le16(wValue);
	setup->wIndex = cpu_to_le16(wIndex);
	setup->wLength = cpu_to_le16(wLength);
}

static inline unsigned char *libusb_control_transfer_get_data(
	struct libusb_transfer *transfer)
{
	return transfer->buffer + LIBUSB_CONTROL_SETUP_SIZE;
}

static inline void libusb_fill_bulk_transfer(struct libusb_transfer *transfer,
    libusb_device_handle *dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
    void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_BULK;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_interrupt_transfer(
    struct libusb_transfer *transfer, libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *buffer, int length,
    libusb_transfer_cb_fn callback, void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_INTERRUPT;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_iso_transfer(struct libusb_transfer *transfer,
    libusb_device_handle *dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, int num_iso_packets,
    libusb_transfer_cb_fn callback, void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_ISOCHRONOUS;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->num_iso_packets = num_iso_packets;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

int libusb_get_string_descriptor_ascii(libusb_device_handle *dev_handle,
    uint8_t desc_index, unsigned char *data, int length);
int libusb_get_config_descriptor(libusb_device *dev, uint8_t config_index,
        struct libusb_config_descriptor **config);
int libusb_get_config_descriptor_by_value(libusb_device *dev, uint8_t value,
        struct libusb_config_descriptor **config);
int libusb_get_active_config_descriptor(libusb_device *dev,
        struct libusb_config_descriptor **config);
void libusb_free_config_descriptor(struct libusb_config_descriptor *config);
int libusb_get_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_type, uint8_t desc_index, unsigned char *data, int length);
int libusb_get_string_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_index, uint16_t langid, unsigned char *data, int length);
int libusb_set_interface_alt_setting(libusb_device_handle *dev_handle,
        int interface_number, int alternate_setting);
void libusb_free_pollfds(const struct libusb_pollfd **pollfds);

struct libusb_ss_endpoint_companion_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
} PACKED;

int libusb_get_ss_endpoint_companion_descriptor(
    struct libusb_context *ctx,
    const struct libusb_endpoint_descriptor *endpoint,
    struct libusb_ss_endpoint_companion_descriptor **ep_comp);
void libusb_free_ss_endpoint_companion_descriptor(
    struct libusb_ss_endpoint_companion_descriptor *ep_comp);

static inline unsigned char *libusb_get_iso_packet_buffer_simple(
    struct libusb_transfer *transfer, unsigned int packet)
{
    int _packet;
    if (packet > 0x80000000)
        return NULL;
    _packet = (int) packet;

    if (_packet >= transfer->num_iso_packets)
        return NULL;

    return transfer->buffer + ((int) transfer->iso_packet_desc[0].length * _packet);
}

static inline void libusb_set_iso_packet_lengths(
    struct libusb_transfer *transfer, unsigned int length)
{
    int i;
    for (i = 0; i < transfer->num_iso_packets; i++)
        transfer->iso_packet_desc[i].length = length;
}

int libusb_get_next_timeout(libusb_context *ctx, struct timeval *tv);
int libusb_has_capability(uint32_t capability);

int libusb_attach_kernel_driver(libusb_device_handle *dev_handle, int interface_number);
int libusb_detach_kernel_driver(libusb_device_handle *dev_handle, int interface_number);
int libusb_kernel_driver_active(libusb_device_handle *dev_handle, int interface_number);

libusb_device * libusb_ref_device(libusb_device *dev);
void libusb_unref_device(libusb_device *dev);

int libusb_get_max_packet_size(libusb_device *dev, unsigned char endpoint);
int libusb_get_device_speed(libusb_device *dev);

int libusb_set_configuration(libusb_device_handle *dev_handle, int configuration);
int libusb_get_configuration(libusb_device_handle *dev_handle, int *config);

int libusb_handle_events_timeout_completed(libusb_context *ctx,
    struct timeval *tv, int *completed);
int libusb_handle_events_completed(libusb_context *ctx, int *completed);
int libusb_handle_events_timeout(libusb_context *ctx, struct timeval *tv);
int libusb_handle_events(libusb_context *ctx);
const struct libusb_pollfd** libusb_get_pollfds(libusb_context *ctx);
void libusb_free_pollfds(const struct libusb_pollfd **pollfds);

void vsf_linux_libusb_startup(void);

// libusb 0.1 compatibility
#if VSF_LINUX_LIBUSB_CFG_01_COMPATIBLE == ENABLED
#define usb_dev_handle                  libusb_device_handle
#define USB_ENDPOINT_IN                 LIBUSB_ENDPOINT_IN
#define USB_ENDPOINT_OUT                LIBUSB_ENDPOINT_OUT

#define usb_init()                      libusb_init(NULL)
#define usb_set_debug(__level)          libusb_set_debug(NULL, (__level))
#define usb_device(__handle)            (struct usb_device *)(__handle)
#define usb_get_string                  libusb_get_string_descriptor
#define usb_get_string_simple           libusb_get_string_descriptor_ascii
#define usb_control_msg                 libusb_control_transfer
#define usb_set_configuration           libusb_set_configuration
#define usb_claim_interface             libusb_claim_interface
#define usb_release_interface           libusb_release_interface
#define usb_get_descriptor              libusb_get_descriptor
#define usb_find_busses()               1
#define usb_strerror()                  "usb_strerror not_supported!!!"

// TODO: implement later
//#define usb_get_descriptor_by_endpoint
//#define usb_set_altinterface
//#define usb_resetep
//#define usb_clear_halt
//#define usb_reset

struct usb_device;
struct usb_bus {
    struct usb_bus                      *next, *prev;
    struct usb_device                   *devices;
    struct usb_device                   *root_dev;
};
struct usb_device {
    struct usb_device                   *next, *prev;
    struct libusb_device_descriptor     descriptor;
    struct libusb_config_descriptor     *config;

    // actually libusb_device
    void                                *dev;
    uint8_t                             devnum;
    uint8_t                             num_children;
    struct usb_device                   **children;

    // private
    struct libusb_config_descriptor     __config;
};

int usb_find_devices(void);
struct usb_bus *usb_get_busses(void);

int usb_get_driver_np(usb_dev_handle *dev, int interface, char *name, unsigned int namelen);
int usb_detach_kernel_driver_np(usb_dev_handle *dev, int interface);

usb_dev_handle *usb_open(struct usb_device *dev);
int usb_close(usb_dev_handle *dev);

int usb_bulk_write(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
int usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);

int usb_interrupt_write(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
int usb_interrupt_read(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
#endif      // VSF_LINUX_LIBUSB_CFG_01_COMPATIBLE

#ifdef __cplusplus
}
#endif

#endif
