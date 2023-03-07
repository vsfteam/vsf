/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#include <linux/slab.h>
#include <net/mac80211.h>
#include <net/cfg80211.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

unsigned int ieee80211_hdrlen(__le16 fc)
{
    unsigned int hdrlen = 24;

    if (ieee80211_is_ext(fc)) {
        return 4;
    }

    if (ieee80211_is_data(fc)) {
        if (ieee80211_has_a4(fc)) {
            hdrlen = 30;
        }
        if (ieee80211_is_data_qos(fc)) {
            hdrlen += IEEE80211_QOS_CTL_LEN;
            if (ieee80211_has_order(fc))
                hdrlen += IEEE80211_HT_CTL_LEN;
        }
        return hdrlen;
    }

    if (ieee80211_is_mgmt(fc)) {
        if (ieee80211_has_order(fc)) {
            hdrlen += IEEE80211_HT_CTL_LEN;
        }
        return hdrlen;
    }

    if (ieee80211_is_ctl(fc)) {
        if ((fc & cpu_to_le16(0x00E0)) == cpu_to_le16(0x00C0)) {
            return 10;
        } else {
            return 16;
        }
    }

    return hdrlen;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

unsigned int ieee80211_get_hdrlen_from_skb(const struct sk_buff *skb)
{
    const struct ieee80211_hdr *hdr = (const struct ieee80211_hdr *)skb->data;
    unsigned int hdrlen;

    if (skb->len < 10) {
        return 0;
    }
    hdrlen = ieee80211_hdrlen(hdr->frame_control);
    if (hdrlen > skb->len) {
        return 0;
    }
    return hdrlen;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

struct ieee80211_hw * ieee80211_alloc_hw(size_t priv_data_len, const struct ieee80211_ops *ops)
{
    struct ieee80211_local *local = kzalloc(sizeof(struct ieee80211_local) + priv_data_len, GFP_KERNEL);
    if (NULL == local) { return NULL; }

    local->ops = ops;
    local->hw.priv = (void *)&local[1];
    return &local->hw;
}

#endif
