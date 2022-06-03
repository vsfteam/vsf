#ifndef __VSF_LINUX_NETINET_TCP_H__
#define __VSF_LINUX_NETINET_TCP_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#else
#   include <sys/types.h>
#endif
// for stdint.h
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__LITTLE_ENDIAN_BITFIELD) && !defined(__BIG_ENDIAN_BITFIELD)
#   define __LITTLE_ENDIAN_BITFIELD
#endif

#define SOL_TCP                     IPPROTO_TCP
// tcp options
// refer: https://code.woboq.org/gtk/include/netinet/tcp.h.html
#define TCP_NODELAY                 1
#define TCP_MAXSEG                  2
//#define TCP_CORK                    3
//#define TCP_KEEPIDLE                4
//#define TCP_KEEPINTVL               5
//#define TCP_KEEPCNT                 6
//#define TCP_SYNCNT                  7
//#define TCP_LINGER2                 8
//#define TCP_DEFER_ACCEPT            9
//#define TCP_WINDOW_CLAMP            10
//#define TCP_INFO                    11
//#define TCP_QUICKACK                12
//#define TCP_CONGESTION              13
//#define TCP_MD5SIG                  14
//#define TCP_COOKIE_TRANSACTIONS     15
//#define TCP_THIN_LINEAR_TIMEOUTS    16
//#define TCP_THIN_DUPACK             17
//#define TCP_USER_TIMEOUT            18
//#define TCP_REPAIR                  19
//#define TCP_REPAIR_QUEUE            20
//#define TCP_QUEUE_SEQ               21
//#define TCP_REPAIR_OPTIONS          22
//#define TCP_FASTOPEN                23
//#define TCP_TIMESTAMP               24
//#define TCP_NOTSENT_LOWAT           25
//#define TCP_CC_INFO                 26
//#define TCP_SAVE_SYN                27
//#define TCP_SAVED_SYN               28
#define TCP_KEEPALIVE               100

#if defined(__FAVOR_BSD) || defined(_BSD_SOURCE)
typedef uint32_t            tcp_seq;
struct tcphdr {
    uint16_t th_sport;
    uint16_t th_dport;
    tcp_seq th_seq;
    tcp_seq th_ack;

#   if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t th_x2:4;
    uint8_t th_off:4;
#   elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t th_off:4;
    uint8_t th_x2:4;
#   else
#       error "invalid __BYTE_ORDER"
#   endif

    uint8_t th_flags;
#   define TH_FIN           0x01
#   define TH_SYN           0x02
#   define TH_RST           0x04
#   define TH_PUSH          0x08
#   define TH_ACK           0x10
#   define TH_URG           0x20
    uint16_t th_win;
    uint16_t th_sum;
    uint16_t th_urp;
};

#else
struct tcphdr {
    uint16_t    source;
    uint16_t    dest;
    uint32_t    seq;
    uint32_t    ack_seq;
#   if __BYTE_ORDER == __LITTLE_ENDIAN
    uint16_t    res1:4,
                doff:4,
                fin:1,
                syn:1,
                rst:1,
                psh:1,
                ack:1,
                urg:1,
                ece:1,
                cwr:1;
#   elif __BYTE_ORDER == __BIG_ENDIAN
    uint16_t    doff:4,
                res1:4,
                cwr:1,
                ece:1,
                urg:1,
                ack:1,
                psh:1,
                rst:1,
                syn:1,
                fin:1;
#   else
#       error "invalid __BYTE_ORDER"
#   endif
    uint16_t    window;
    uint16_t    check;
    uint16_t    urg_ptr;
};
#endif

#ifdef __cplusplus
}
#endif

#endif
