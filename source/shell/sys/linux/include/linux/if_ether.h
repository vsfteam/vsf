#ifndef __VSF_LINUX_IF_ETHER_H__
#define __VSF_LINUX_IF_ETHER_H__

#include <linux/skbuff.h>
// for CAL(Compiler Abstraction Layer) VSF_CAL_PACKED
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETH_ALEN                6
#define ETH_TLEN                2
#define ETH_HLEN                14
#define ETH_ZLEN                60
#define ETH_DATA_LEN            1500
#define ETH_FRAME_LEN           1514
#define ETH_FCS_LEN             4

#define ETH_MIN_MTU             68
#define ETH_MAX_MTU             0xFFFFU

struct ethhdr {
    unsigned char               h_dest[ETH_ALEN];
    unsigned char               h_source[ETH_ALEN];
    __be16                      h_proto;
} VSF_CAL_PACKED;

#ifdef __cplusplus
}
#endif

#endif
