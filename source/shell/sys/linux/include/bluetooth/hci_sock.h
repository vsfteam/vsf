#ifndef __VSF_LINUX_BLUETOOTH_HCI_SOCK_H__
#define __VSF_LINUX_BLUETOOTH_HCI_SOCK_H__

#include <sys/socket.h>
#include <sys/ioctl.h>

struct sockaddr_hci {
    sa_family_t             hci_family;
    unsigned short          hci_dev;
    unsigned short          hci_channel;
};
#define HCI_DEV_NONE        0xffff

#define HCI_CHANNEL_RAW     0
#define HCI_CHANNEL_USER    1

#define HCIDEVUP            _IOW('H', 201, int)
#define HCIDEVDOWN          _IOW('H', 202, int)
#define HCIDEVRESET         _IOW('H', 203, int)
#define HCIDEVRESTAT        _IOW('H', 204, int)

#define HCIGETDEVLIST       _IOR('H', 210, int)
#define HCIGETDEVINFO       _IOR('H', 211, int)

#endif      // __VSF_LINUX_BLUETOOTH_HCI_SOCK_H__
