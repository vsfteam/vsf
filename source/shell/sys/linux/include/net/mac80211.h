#ifndef __VSF_LINUX_MAC80211_H__
#define __VSF_LINUX_MAC80211_H__

#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/ieee80211.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ieee80211_sta {
    u8 addr[ETH_ALEN];
};

struct ieee80211_vif {
    bool p2p;
};

struct ieee80211_tx_control {
    struct ieee80211_sta *sta;
};

struct ieee80211_conf {
    u32 flags;
};

struct ieee80211_ops;
struct ieee80211_hw {
    struct ieee80211_conf conf;
    void *priv;
};

struct ieee80211_local {
    struct ieee80211_hw hw;
    const struct ieee80211_ops *ops;

    struct work_struct restart_work;
};

enum ieee80211_filter_flags {
    FIF_ALLMULTI            = 1<<1,
    FIF_FCSFAIL             = 1<<2,
    FIF_PLCPFAIL            = 1<<3,
    FIF_BCN_PRBRESP_PROMISC = 1<<4,
    FIF_CONTROL             = 1<<5,
    FIF_OTHER_BSS           = 1<<6,
    FIF_PSPOLL              = 1<<7,
    FIF_PROBE_REQ           = 1<<8,
    FIF_MCAST_ACTION        = 1<<9,
};

struct ieee80211_channel_switch {
    int channel;
};

struct ieee80211_ops {
    void (*tx)(struct ieee80211_hw *hw, struct ieee80211_tx_control *control, struct sk_buff *skb);
    int (*start)(struct ieee80211_hw *hw);
    void (*stop)(struct ieee80211_hw *hw);
    int (*config)(struct ieee80211_hw *hw, u32 changed);
    void (*configure_filter)(struct ieee80211_hw *hw, unsigned int changed_flags, unsigned int *total_flags, u64 multicast);
    void (*channel_switch)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_channel_switch *ch_switch);

    int (*add_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
    void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
};

extern struct ieee80211_hw * ieee80211_alloc_hw(size_t priv_data_len, const struct ieee80211_ops *ops);
extern int ieee80211_register_hw(struct ieee80211_hw *hw);
extern void ieee80211_unregister_hw(struct ieee80211_hw *hw);
extern void ieee80211_free_hw(struct ieee80211_hw *hw);

extern void ieee80211_restart_hw(struct ieee80211_hw *hw);

#ifdef __cplusplus
}
#endif

#endif
