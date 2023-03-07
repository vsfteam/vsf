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

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static inline bool is_broadcast_ether_addr(const u8 *addr)
{
    return (*(const u16 *)(addr + 0) &
            *(const u16 *)(addr + 2) &
            *(const u16 *)(addr + 4)) == 0xffff;
}

static inline bool is_zero_ether_addr(const u8 *addr)
{
    return (*(const u16 *)(addr + 0) |
            *(const u16 *)(addr + 2) |
            *(const u16 *)(addr + 4)) == 0;
}

static inline bool is_multicast_ether_addr(const u8 *addr)
{
    u16 a = *(const u16 *)addr;
#if __BYTE_ORDER == __BIG_ENDIAN
    return 0x01 & (a >> ((sizeof(a) * 8) - 8));
#else
    return 0x01 & a;
#endif
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

static inline bool is_unicast_ether_addr(const u8 *addr)
{
    return !is_multicast_ether_addr(addr);
}

static inline bool is_valid_ether_addr(const u8 *addr)
{
    return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

#ifdef __cplusplus
}
#endif

#endif
