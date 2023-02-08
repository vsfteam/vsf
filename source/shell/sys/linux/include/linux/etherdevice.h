#ifndef __VSF_LINUX_ETHERDEVICE_H__
#define __VSF_LINUX_ETHERDEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void eth_broadcast_addr(u8 *addr)
{
    memset(addr, 0xFF, ETH_ALEN);
}

static inline bool is_local_ether_addr(const u8 *addr)
{
    return 0x02 & addr[0];
}

static inline bool is_broadcast_ether_addr(const u8 *addr)
{
    return (*(const u16 *)(addr + 0) &
            *(const u16 *)(addr + 2) &
            *(const u16 *)(addr + 4)) == 0xffff;
}

#ifdef __cplusplus
}
#endif

#endif
