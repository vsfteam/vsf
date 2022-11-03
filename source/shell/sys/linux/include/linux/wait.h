#ifndef __VSF_LINUX_WAIT_H__
#define __VSF_LINUX_WAIT_H__

#include <linux/spinlock.h>
#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wait_queue_head {
    spinlock_t              lock;
    struct list_head        head;
} wait_queue_head_t;

#ifdef __cplusplus
}
#endif

#endif
