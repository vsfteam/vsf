#ifndef __VSF_LINUX_LIST_H__
#define __VSF_LINUX_LIST_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIST_HEAD_INIT(__NAME)                  { &(__NAME), &(__NAME) }
#define LIST_HEAD(__NAME)                       struct list_head __NAME = LIST_HEAD_INIT(__NAME)

struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
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
