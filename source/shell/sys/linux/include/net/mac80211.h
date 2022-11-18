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

struct ieee80211_ops {
    void (*tx)(struct ieee80211_hw *hw, struct ieee80211_tx_control *control, struct sk_buff *skb);
    int (*start)(struct ieee80211_hw *hw);
    void (*stop)(struct ieee80211_hw *hw);
    int (*config)(struct ieee80211_hw *hw, u32 changed);
    void (*configure_filter)(struct ieee80211_hw *hw, unsigned int changed_flags, unsigned int *total_flags, u64 multicast);

    int (*add_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
    void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
};

extern struct ieee80211_hw * ieee80211_alloc_hw(size_t priv_data_len, const struct ieee80211_ops *ops);
extern void ieee80211_free_hw(struct ieee80211_hw *hw);

extern void ieee80211_restart_hw(struct ieee80211_hw *hw);
extern void ieee80211_rx_list(struct ieee80211_hw *hw, struct ieee80211_sta *sta, struct sk_buff *skb, struct list_head *list);
extern void ieee80211_rx(struct ieee80211_hw *hw, struct sk_buff *skb);
extern void ieee80211_tx_status(struct ieee80211_hw *hw, struct sk_buff *skb);

#ifdef __cplusplus
}
#endif

#endif
