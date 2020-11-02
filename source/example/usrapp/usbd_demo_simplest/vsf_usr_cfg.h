#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

#define ASSERT(...)                     if (!(__VA_ARGS__)) {while(1);};
//#define ASSERT(...)

#define VSF_SYSTIMER_FREQ               (192000000ul)

#define VSF_USE_USB_DEVICE              ENABLED
#   define VSF_USBD_CFG_STREAM_EN       ENABLED
#   define VSF_USBD_USE_CDCACM          ENABLED
#   define VSF_USBD_USE_MSC             ENABLED

#define VSF_USE_SCSI                    ENABLED
#define VSF_USE_SIMPLE_STREAM           ENABLED

#endif
