#ifndef __VSF_LINUX_LIST_H__
#define __VSF_LINUX_LIST_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIST_HEAD_INIT(__NAME)                  { &(__NAME), &(__NAME) }
#define LIST_HEAD(__NAME)                       struct list_head __NAME = LIST_HEAD_INIT(__NAME)

#define list_entry(ptr, type, member)           vsf_container_of(ptr, type, member)
#define list_first_entry(ptr, type, member)     list_entry((ptr)->next, type, member)
#define list_last_entry(ptr, type, member)      list_entry((ptr)->prev, type, member)
#define list_next_entry(pos, member)            list_entry((pos)->member.next, typeof(*(pos)), member)
#define list_next_entry_circular(pos, head, member)                             \
    (list_is_last(&(pos)->member, head) ? list_first_entry(head, typeof(*(pos)), member) : list_next_entry(pos, member))
#define list_prev_entry(pos, member)            list_entry((pos)->member.prev, typeof(*(pos)), member)
#define list_prev_entry_circular(pos, head, member)                             \
    (list_is_first(&(pos)->member, head) ? list_last_entry(head, typeof(*(pos)), member) : list_prev_entry(pos, member))
#define list_first_entry_or_null(ptr, type, member) ({                          \
    struct list_head *head__ = (ptr);                                           \
    struct list_head *pos__ = READ_ONCE(head__->next);                          \
    pos__ != head__ ? list_entry(pos__, type, member) : NULL;                   \
})
#define list_entry_is_head(pos, head, member)   (&pos->member == (head))

#define list_prepare_entry(pos, head, member)   ((pos) ? : list_entry(head, typeof(*pos), member))

#define list_for_each(pos, head)                for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)
#define list_for_each_continue(pos, head)       for (pos = pos->next; !list_is_head(pos, (head)); pos = pos->next)
#define list_for_each_prev(pos, head)           for (pos = (head)->prev; !list_is_head(pos, (head)); pos = pos->prev)
#define list_for_each_safe(pos, n, head)                                        \
    for (pos = (head)->next, n = pos->next; !list_is_head(pos, (head)); pos = n, n = pos->next)
#define list_for_each_prev_safe(pos, n, head)                                   \
    for (pos = (head)->prev, n = pos->prev; !list_is_head(pos, (head)); pos = n, n = pos->prev)
#define list_for_each_entry(pos, head, member)                                  \
    for (pos = list_first_entry(head, typeof(*pos), member);                    \
        !list_entry_is_head(pos, head, member);                                 \
        pos = list_next_entry(pos, member))
#define list_for_each_entry_reverse(pos, head, member)                          \
    for (pos = list_last_entry(head, typeof(*pos), member);                     \
        !list_entry_is_head(pos, head, member);                                 \
        pos = list_prev_entry(pos, member))
#define list_for_each_entry_continue(pos, head, member)                         \
    for (pos = list_next_entry(pos, member);                                    \
        !list_entry_is_head(pos, head, member);                                 \
        pos = list_next_entry(pos, member))
#define list_for_each_entry_continue_reverse(pos, head, member)                 \
    for (pos = list_prev_entry(pos, member);                                    \
        !list_entry_is_head(pos, head, member);                                 \
        pos = list_prev_entry(pos, member))
#define list_for_each_entry_from(pos, head, member)                             \
    for (; !list_entry_is_head(pos, head, member); pos = list_next_entry(pos, member))
#define list_for_each_entry_from_reverse(pos, head, member)                     \
    for (; !list_entry_is_head(pos, head, member); pos = list_prev_entry(pos, member))
#define list_for_each_entry_safe(pos, n, head, member)                          \
    for (pos = list_first_entry(head, typeof(*pos), member),                    \
            n = list_next_entry(pos, member);                                   \
        !list_entry_is_head(pos, head, member);                                 \
        pos = n, n = list_next_entry(n, member))
#define list_for_each_entry_safe_continue(pos, n, head, member)                 \
    for (pos = list_next_entry(pos, member), n = list_next_entry(pos, member);  \
        !list_entry_is_head(pos, head, member);                                 \
        pos = n, n = list_next_entry(n, member))
#define list_for_each_entry_safe_from(pos, n, head, member)                     \
    for (n = list_next_entry(pos, member);                                      \
        !list_entry_is_head(pos, head, member);                                 \
        pos = n, n = list_next_entry(n, member))
#define list_for_each_entry_safe_reverse(pos, n, head, member)                  \
    for (pos = list_last_entry(head, typeof(*pos), member),                     \
            n = list_prev_entry(pos, member);                                   \
        !list_entry_is_head(pos, head, member);                                 \
        pos = n, n = list_prev_entry(n, member))

struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline int list_is_head(const struct list_head *list, const struct list_head *head)
{
    return list == head;
}

static inline int list_is_last(const struct list_head *list, const struct list_head *head)
{
    return list->next == head;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

static inline int list_is_singular(const struct list_head *head)
{
    return !list_empty(head) && (head->next == head->prev);
}

static inline void list_insert(struct list_head *node, struct list_head *prev, struct list_head *next)
{
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void list_add(struct list_head *node, struct list_head *head)
{
    list_insert(node, head, head->next);
}

static inline void list_add_tail(struct list_head *node, struct list_head *head)
{
    list_insert(node, head->prev, head);
}

static inline void list_del(struct list_head *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

static inline void list_del_init(struct list_head *entry)
{
    list_del(entry);
    INIT_LIST_HEAD(entry);
}

#ifdef __cplusplus
}
#endif

#endif
