#ifndef __VSF_LINUX_LLIST_H__
#define __VSF_LINUX_LLIST_H__

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

struct llist_node {
    struct llist_node *next;
};

struct llist_head {
    struct llist_node *first;
};

#define LLIST_HEAD_INIT(name)           { NULL }
#define LLIST_HEAD(name)                struct llist_head name = LLIST_HEAD_INIT(name)

static inline void init_llist_head(struct llist_head *list)
{
    list->first = NULL;
}

#define llist_entry(p, t, m)            vsf_container_of(p, t, m)
#define llist_for_each(pos, node)                                               \
    for ((pos) = (node); pos; (pos) = (pos)->next)
#define llist_for_each_safe(pos, n, node)                                       \
    for ((pos) = (node); (pos) && ((n) = (pos)->next, true); (pos) = (n))
#define llist_for_each_entry(pos, node, member)                                 \
    for ((pos) = llist_entry((node), typeof(*(pos)), member);                   \
         member_address_is_nonnull(pos, member);                                \
         (pos) = llist_entry((pos)->member.next, typeof(*(pos)), member))
#define llist_for_each_entry_safe(pos, n, node, member)                         \
    for (pos = llist_entry((node), typeof(*pos), member);                       \
         member_address_is_nonnull(pos, member) &&                              \
            (n = llist_entry(pos->member.next, typeof(*n), member), true);      \
         pos = n)

static inline bool llist_empty(const struct llist_head *head)
{
    return head->first == NULL;
}

static inline struct llist_node * llist_next(struct llist_node *node)
{
    return node->next;
}

#ifdef __cplusplus
}
#endif

#endif
