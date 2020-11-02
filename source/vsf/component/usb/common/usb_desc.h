#ifndef __USB_DESC_H__
#define __USB_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DESC_WORD(__VALUE)                                                  \
            ((__VALUE) >> 0) & 0xFF, ((__VALUE) >> 8) & 0xFF

#define USB_DESC_DWORD(__VALUE)                                                 \
            ((__VALUE) >> 0) & 0xFF, ((__VALUE) >> 8) & 0xFF,                   \
            ((__VALUE) >> 16) & 0xFF, ((__VALUE) >> 24) & 0xFF

#define USB_DESC_DEV_LEN                        USB_DT_DEVICE_SIZE
#define USB_DESC_DEV(__VERSION, __CLASS, __SUBCLASS, __PROTOCOL, __EP0_SIZE, __VID, __PID, __BCD_VER, __I_MANUFACTURER, __I_PRODUCT, __I_SERIAL_NUM, __CONFIG_NUM)\
            USB_DESC_DEV_LEN,                                                   \
            USB_DT_DEVICE,                                                      \
            USB_DESC_WORD(__VERSION),           /* bcdUSB */                    \
            (__CLASS),                          /* device class: IAD */         \
            (__SUBCLASS),                       /* device sub class */          \
            (__PROTOCOL),                       /* device protocol */           \
            (__EP0_SIZE),                       /* max packet size */           \
            USB_DESC_WORD(__VID),               /* vendor */                    \
            USB_DESC_WORD(__PID),               /* product */                   \
            USB_DESC_WORD(__BCD_VER),           /* bcdDevice */                 \
            (__I_MANUFACTURER),                 /* manufacturer */              \
            (__I_PRODUCT),                      /* product */                   \
            (__I_SERIAL_NUM),                   /* serial number */             \
            (__CONFIG_NUM),                     /* number of configuration */

#define USB_DESC_DEV_IAD_LEN                    (USB_DESC_IAD_LEN + USB_DESC_DEV_LEN)
#define USB_DESC_DEV_IAD(__VERSION, __EP0_SIZE, __VID, __PID, __BCD_VER, __I_MANUFACTURER, __I_PRODUCT, __I_SERIAL_NUM, __CONFIG_NUM)\
            USB_DESC_DEV((__VERSION), 0xEF, 0x02, 0x01, (__EP0_SIZE), (__VID), (__PID), (__BCD_VER), (__I_MANUFACTURER), (__I_PRODUCT), (__I_SERIAL_NUM), (__CONFIG_NUM))

#define USB_DESC_CFG_LEN                        USB_DT_CONFIG_SIZE
#define USB_DESC_CFG(__CFG_SIZE, __IFS_NUM, __CONFIG_VALUE, __I_CONFIG, __ATTR, __MAX_POWER)\
            USB_DESC_CFG_LEN,                                                   \
            USB_DT_CONFIG,                                                      \
            USB_DESC_WORD(__CFG_SIZE),          /* wTotalLength */              \
            (__IFS_NUM),                        /* bNumInterfaces: */           \
            (__CONFIG_VALUE),                   /* bConfigurationValue: Configuration value */\
            (__I_CONFIG),                       /* iConfiguration: Index of string descriptor describing the configuration */\
            (__ATTR),                           /* bmAttributes: bus powered */ \
            (__MAX_POWER),                      /* MaxPower */

#define USB_DESC_IFS_LEN                        USB_DT_INTERFACE_SIZE
#define USB_DESC_IFS(__IFS_NUM, __ALT_SETTING, __EP_NUM, __CLASS, __SUBCLASS, __PROTOCOL, __I_IFS)\
            USB_DESC_IFS_LEN,                                                   \
            USB_DT_INTERFACE,                                                   \
            (__IFS_NUM),                        /* bInterfaceNumber: Number of Interface */\
            (__ALT_SETTING),                    /* bAlternateSetting: Alternate setting */\
            (__EP_NUM),                         /* bNumEndpoints */             \
            (__CLASS),                          /* bInterfaceClass */           \
            (__SUBCLASS),                       /* bInterfaceSubClass */        \
            (__PROTOCOL),                       /* nInterfaceProtocol */        \
            (__I_IFS),                          /* iInterface: */

#define USB_DESC_EP_LEN                        USB_DT_ENDPOINT_SIZE
#define USB_DESC_EP(__EP_NUM, __EP_ATTR, __EP_SIZE, __EP_INTERVAL)              \
            USB_DESC_EP_LEN,                                                    \
            USB_DT_ENDPOINT,                                                    \
            (__EP_NUM),                         /* bEndpointAddress: */         \
            (__EP_ATTR),                        /* bmAttributes: */             \
            USB_DESC_WORD(__EP_SIZE),           /* wMaxPacketSize: */           \
            (__EP_INTERVAL),                    /* bInterval: */

#define USB_DESC_QUALIFIER_LEN                  USB_DT_DEVICE_QUALIFIER_SIZE
#define USB_DESC_QUALIFIER(__VERSION, __CLASS, __SUB_CLASS, __PROTOCOL, __EP0_SIZE, __CONFIG_NUM)\
            USB_DESC_QUALIFIER_LEN,                                             \
            USB_DT_DEVICE_QUALIFIER,                                            \
            USB_DESC_WORD(__VERSION),           /* bcdUSB */                    \
            (__CLASS),                          /* bDeviceClass */              \
            (__SUB_CLASS),                      /* bDeviceSubClass */           \
            (__PROTOCOL),                       /* bDeviceProtocol */           \
            (__EP0_SIZE),                       /* bMaxPacketSize0 */           \
            (__CONFIG_NUM),                     /* bNumConfigurations */        \
            0,                                  /* bReserved */

#define USB_DESC_IAD_LEN                        USB_DT_INTERFACE_ASSOCIATION_SIZE
#define USB_DESC_IAD(__IFS_START, __IFS_NUM, __CLASS, __SUBCLASS, __PROTOCOL, __I_FUNC)\
            USB_DESC_IAD_LEN                 ,  /* IDA */                       \
            USB_DT_INTERFACE_ASSOCIATION,                                       \
            (__IFS_START),                      /* bFirstInterface */           \
            (__IFS_NUM),                        /* bInterfaceCount */           \
            (__CLASS),                          /* bFunctionClass */            \
            (__SUBCLASS),                       /* bFunctionSubClass */         \
            (__PROTOCOL),                       /* bFunctionProtocol */         \
            (__I_FUNC),                         /* iFunction */

#define USB_DESC_STRING_LEN(__STR_LEN)          ((__STR_LEN) + 2)
#define USB_DESC_STRING(__STR_LEN, ...)                                         \
            USB_DESC_STRING_LEN(__STR_LEN),                                     \
            USB_DT_STRING,                                                      \
            __VA_ARGS__,

#ifdef __cplusplus
}
#endif

#endif      // __USB_DESC_H__
