#ifndef __VSF_LINUX_ETHERDEVICE_H__
#define __VSF_LINUX_ETHERDEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void eth_broadcast_addr(u8 *addr)
{
    memset(addr, 0xFF, ETH_ALEN);
}

#ifdef __cplusplus
}
#endif

#endif
