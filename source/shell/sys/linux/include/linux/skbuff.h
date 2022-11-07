#ifndef __VSF_LINUX_SKBUFF_H__
#define __VSF_LINUX_SKBUFF_H__

#include <linux/types.h>
#include <linux/gfp.h>
#include <linux/llist.h>
#include <linux/refcount.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int                sk_buff_data_t;

struct skb_shared_info {
    __u8                            flags;
    atomic_t                        dataref;
};

struct sk_buff {
    union {
        struct {
            struct sk_buff          *next;
            struct sk_buff          *prev;
        };
        struct list_head            list;
        struct llist_node           ll_node;
    };

    char                            cb[48];

    unsigned int                    len, data_len;
    sk_buff_data_t                  tail;
    sk_buff_data_t                  end;
    unsigned char                   *head, *data;
    refcount_t                      users;
};

struct sk_buff * alloc_skb(unsigned int size, gfp_t priority);

static inline bool skb_unref(struct sk_buff *skb)
{
    return refcount_dec_and_test(&skb->users);
}

static inline void skb_reserve(struct sk_buff *skb, int len)
{
    skb->data += len;
    skb->tail += len;
}

static inline unsigned char * skb_tail_pointer(const struct sk_buff *skb)
{
    return skb->head + skb->tail;
}

static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
    skb->tail = skb->data - skb->head;
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
    skb_reset_tail_pointer(skb);
    skb->tail += offset;
}

static inline unsigned char * skb_end_pointer(const struct sk_buff *skb)
{
    return skb->head + skb->end;
}

static inline unsigned int skb_end_offset(const struct sk_buff *skb)
{
    return skb->end;
}

static inline void skb_set_end_offset(struct sk_buff *skb, unsigned int offset)
{
    skb->end = offset;
}

static inline void * skb_put(struct sk_buff *skb, int len)
{
    void *tmp = skb_tail_pointer(skb);
    skb->tail += len;
    skb->len += len;
    return tmp;
}

static inline void * skb_put_zero(struct sk_buff *skb, unsigned int len)
{
    void *tmp = skb_put(skb, len);
    memset(tmp, 0, len);
    return tmp;
}

static inline void * skb_put_data(struct sk_buff *skb, const void *data, unsigned int len)
{
    void *tmp = skb_put(skb, len);
    memcpy(tmp, data, len);
    return tmp;
}

static inline void skb_put_u8(struct sk_buff *skb, u8 val)
{
    *(u8 *)skb_put(skb, 1) = val;
}

static inline void * skb_push(struct sk_buff *skb, unsigned int len)
{
    skb->data -= len;
    skb->len += len;
    return skb->data;
}

static inline void * skb_pull(struct sk_buff *skb, unsigned int len)
{
    skb->len -= len;
    return skb->data += len;
}

static inline struct sk_buff * skb_get(struct sk_buff *skb)
{
    refcount_inc(&skb->users);
    return skb;
}

static inline unsigned int skb_headroom(const struct sk_buff *skb)
{
    return skb->data - skb->head;
}

static inline int skb_tailroom(const struct sk_buff *skb)
{
    return skb->end - skb->tail;
}

static inline void skb_trim(struct sk_buff *skb, unsigned int len)
{
    if (skb->len > len) {
        skb->len = len;
        skb_set_tail_pointer(skb, len);
    }
}

static inline struct sk_buff * netdev_alloc_skb(struct net_device *dev, unsigned int length)
{
    return alloc_skb(length, GFP_KERNEL);
}

#define dev_kfree_skb(__skb)        consume_skb(__skb)
#define dev_consume_skb_any(__skb)  consume_skb(__skb)
static inline struct sk_buff * dev_alloc_skb(unsigned int length)
{
    return netdev_alloc_skb(NULL, length);
}

#ifdef __cplusplus
}
#endif

#endif
