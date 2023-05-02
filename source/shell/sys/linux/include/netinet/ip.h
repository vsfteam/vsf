#ifndef __VSF_LINUX_NETINET_IP_H__
#define __VSF_LINUX_NETINET_IP_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "./in.h"
#else
#   include <sys/types.h>
#   include <netinet/in.h>
#endif
// for byte order in arch
#include "hal/arch/vsf_arch.h"
// for stdint.h
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

struct timestamp {
    uint8_t                 len;
    uint8_t                 ptr;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int            flags : 4;
    unsigned int            overflow : 4;
#else
    unsigned int            overflow : 4;
    unsigned int            flags : 4;
#endif
    uint32_t                data[9];
};

struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int            ihl : 4;
    unsigned int            version : 4;
#else
    unsigned int            version : 4;
    unsigned int            ihl : 4;
#endif
    uint8_t                 tos;
    uint16_t                tot_len;
    uint16_t                id;
    uint16_t                frag_off;
    uint8_t                 ttl;
    uint8_t                 procotol;
    uint16_t                check;
    uint32_t                saddr;
    uint32_t                daddr;
};

struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int            ip_hl : 4;
    unsigned int            ip_v : 4;
#else
    unsigned int            ip_v : 4;
    unsigned int            ip_hl : 4;
#endif
    uint8_t                 ip_tos;
    uint16_t                ip_len;
    uint16_t                ip_id;
    uint16_t                ip_off;
#define IP_RF               0x8000
#define IP_DF               0x4000
#define IP_MF               0x2000
#define IP_OFFMASK          0x1FFF
    uint8_t                 ip_ttl;
    uint8_t                 ip_p;
    uint16_t                ip_sum;
    struct in_addr          ip_src, ip_dst;
};

struct ip_timestamp {
    uint8_t                 ipt_code;
    uint8_t                 ipt_len;
    uint8_t                 ipt_ptr;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int            ipt_flg : 4;
    unsigned int            ipt_oflw : 4;
#else
    unsigned int            ipt_oflw : 4;
    unsigned int            ipt_flg : 4;
#endif
    uint32_t                data[9];
};

#define IPVERSION           4
#define IP_MAXPACKET        65535

#define IPTOS_ECN_MASK      0x03
#define IPTOS_ECN(__X)      ((__X) & IPTOS_ECN_MASK)
#define IPTOS_ECN_NOT_ECT   0x00
#define IPTOS_ECN_ECT1      0x01
#define IPTOS_ECN_ECT0      0x02
#define IPTOS_ECN_CE        0x03

#define IPTOS_DSCP_MASK     0xfc
#define IPTOS_DSCP(__X)     ((__X) & IPTOS_DSCP_MASK)
#define IPTOS_DSCP_AF11     0x28
#define IPTOS_DSCP_AF12     0x30
#define IPTOS_DSCP_AF13     0x38
#define IPTOS_DSCP_AF21     0x48
#define IPTOS_DSCP_AF22     0x50
#define IPTOS_DSCP_AF23     0x58
#define IPTOS_DSCP_AF31     0x68
#define IPTOS_DSCP_AF32     0x70
#define IPTOS_DSCP_AF33     0x78
#define IPTOS_DSCP_AF41     0x88
#define IPTOS_DSCP_AF42     0x90
#define IPTOS_DSCP_AF43     0x98
#define IPTOS_DSCP_EF       0xb8

#define IPTOS_TOS_MASK      0x1E
#define IPTOS_TOS(__TOS)    ((__TOS) & IPTOS_TOS_MASK)
#define IPTOS_LOWDELAY      0x10
#define IPTOS_THROUGHPUT    0x08
#define IPTOS_RELIABILITY   0x04
#define IPTOS_MINCOST       0x02

#define IPTOS_PREC_MASK             0xe0
#define IPTOS_PREC(__TOS)           ((__TOS) & IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL       0xe0
#define IPTOS_PREC_INTERNETCONTROL  0xc0
#define IPTOS_PREC_CRITIC_ECP       0xa0
#define IPTOS_PREC_FLASHOVERRIDE    0x80
#define IPTOS_PREC_FLASH            0x60
#define IPTOS_PREC_IMMEDIATE        0x40
#define IPTOS_PREC_PRIORITY         0x20
#define IPTOS_PREC_ROUTINE          0x00

#define IPOPT_COPY          0x80
#define IPOPT_CLASS_MASK    0x60
#define IPOPT_NUMBER_MASK   0x1F

#define IPOPT_COPIED(__O)   ((__O) & IPOPT_COPY)
#define IPOPT_CLASS(__O)    ((__O) & IPOPT_CLASS_MASK)
#define IPOPT_NUMBER(__O)   ((__O) & IPOPT_NUMBER_MASK)

#define IPOPT_CONTROL       0x00
#define IPOPT_RESERVED1     0x20
#define IPOPT_DEBMEAS       0x40
#define IPOPT_MEASUREMENT   IPOPT_DEBMEAS
#define IPOPT_RESERVED2     0x60

#define IPOPT_EOL           0
#define IPOPT_END           IPOPT_EOL
#define IPOPT_NOP           1
#define IPOPT_NOOP          IPOPT_NOP

#define IPOPT_RR            7
#define IPOPT_TS            68
#define IPOPT_TIMESTAMP     IPOPT_TS
#define IPOPT_SECURITY      130
#define IPOPT_SEC           IPOPT_SECURITY
#define IPOPT_LSRR          131
#define IPOPT_SATID         136
#define IPOPT_SID           IPOPT_SATID
#define IPOPT_SSRR          137
#define IPOPT_RA            148

#define IPOPT_OPTVAL        0
#define IPOPT_OLEN          1
#define IPOPT_OFFSET        2
#define IPOPT_MINOFF        4

#define MAX_IPOPTLEN        40

#define IPOPT_TS_TSONLY     0
#define IPOPT_TS_TSANDADDR  1
#define IPOPT_TS_PRESPEC    3

#define IPOPT_SECUR_UNCLASS     0x0000
#define IPOPT_SECUR_CONFID      0xf135
#define IPOPT_SECUR_EFTO        0x789a
#define IPOPT_SECUR_MMMM        0xbc4d
#define IPOPT_SECUR_RESTR       0xaf13
#define IPOPT_SECUR_SECRET      0xd788
#define IPOPT_SECUR_TOPSECRET   0x6bc5

#define MAXTTL              255
#define IPDEFTTL            64
#define IPFRAGTTL           60
#define IPTTLDEC            1

#define IP_MSS              576

#ifdef __cplusplus
}
#endif

#endif
