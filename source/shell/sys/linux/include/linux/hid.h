#ifndef __VSF_LINUX_HID_H__
#define __VSF_LINUX_HID_H__

#include <linux/types.h>
#include <linux/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HID_INPUT_REPORT                0
#define HID_OUTPUT_REPORT               1
#define HID_FEATURE_REPORT              2

struct hid_class_descriptor {
    __u8  bDescriptorType;
    __le16 wDescriptorLength;
} VSF_CAL_PACKED;

struct hid_descriptor {
    __u8  bLength;
    __u8  bDescriptorType;
    __le16 bcdHID;
    __u8  bCountryCode;
    __u8  bNumDescriptors;

    struct hid_class_descriptor desc[1];
} VSF_CAL_PACKED;

struct hid_device;
struct hid_ll_driver;

struct hid_report {
    unsigned int                        id;
    unsigned int                        type;
    unsigned int                        application;
    struct hid_device                   *device;
};

struct hid_device {
    __u16                               bus;
    __u16                               group;
    __u32                               vendor;
    __u32                               product;
    __u32                               version;
    unsigned                            country;

    struct device                       dev;
    struct hid_ll_driver                *ll_driver;

    char                                name[128];
    char                                phys[64];
    char                                uniq[64];

    void                                *driver_data;
};

struct hid_ll_driver {
    int (*start)(struct hid_device *hdev);
    void (*stop)(struct hid_device *hdev);

    int (*open)(struct hid_device *hdev);
    void (*close)(struct hid_device *hdev);

    int (*parse)(struct hid_device *hdev);
    void (*request)(struct hid_device *hdev, struct hid_report *report, int reqtype);
    int (*raw_request)(struct hid_device *hdev, unsigned char reportnum, __u8 *buf, size_t len, unsigned char rtype, int reqtype);
    int (*output_report)(struct hid_device *hdev, __u8 *buf, size_t len);
};

extern struct hid_device *hid_allocate_device(void);
extern void hid_destroy_device(struct hid_device *hdev);
extern int hid_input_report(struct hid_device *hdev, int type, u8 *data, u32 size, int interrupt);

extern bool hid_ignore(struct hid_device *hdev);
extern int hid_add_device(struct hid_device *hdev);
extern void hid_destroy_device(struct hid_device *hdev);

extern int hid_parse_report(struct hid_device *hdev, __u8 *start, unsigned size);

#ifdef __cplusplus
}
#endif

#endif
