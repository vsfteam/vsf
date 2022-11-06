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

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#include <unistd.h>

#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/kobject.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/device.h>

#include <linux/skbuff.h>

#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

/*******************************************************************************
* linux/atomic                                                                 *
*******************************************************************************/

WEAK(atomic_inc)
void atomic_inc(atomic_t *a)
{
    vsf_protect_t orig = vsf_protect_int();
    a->counter++;
    vsf_unprotect_int(orig);
}

WEAK(atomic_dec_and_test)
int atomic_dec_and_test(atomic_t *a)
{
    int value;

    vsf_protect_t orig = vsf_protect_int();
    value = --a->counter;
    vsf_unprotect_int(orig);
    return !value;
}

/*******************************************************************************
* kernel object                                                                *
*******************************************************************************/

int kobject_set_name_vargs(struct kobject *kobj, const char *fmt, va_list ap)
{
    int size = vsnprintf(NULL, 0, fmt, ap);
    if (size > 0) {
        if (kobj->name != NULL) {
            vsf_heap_free((void *)kobj->name);
        }
        kobj->name = vsf_heap_malloc(size);
        if (NULL == kobj->name) {
            return -ENOMEM;
        }
        return 0;
    }
    return -EINVAL;
}

void kobject_init(struct kobject *kobj, const struct kobj_type *ktype)
{
    INIT_LIST_HEAD(&kobj->entry);
    kref_init(&kobj->kref);
    kobj->ktype = ktype;
}

static void kobject_cleanup(struct kobject *kobj)
{
    struct kobject *parent = kobj->parent;
    const struct kobj_type *t = kobj->ktype;

    if (t && t->release) {
        t->release(kobj);
    }

    if (kobj->name != NULL) {
        kfree(kobj->name);
    }
    kobject_put(parent);
}

static void kobject_release(struct kref *kref)
{
    struct kobject *kobj = container_of(kref, struct kobject, kref);
    kobject_cleanup(kobj);
}

struct kobject * kobject_get(struct kobject *kobj)
{
    if (kobj) {
        kref_get(&kobj->kref);
    }
    return kobj;
}

void kobject_put(struct kobject *kobj)
{
    if (kobj) {
        kref_put(&kobj->kref, kobject_release);
    }
}

void kobject_del(struct kobject *kobj)
{
    if (kobj != NULL) {
        struct kobject *parent = kobj->parent;
        kobject_put(parent);
    }
}

/*******************************************************************************
* linux/workqueue.h                                                            *
*******************************************************************************/

struct workqueue_struct {
    vsf_dlist_t                     work_list;
    vsf_dlist_t                     dwork_list;
    vsf_slist_t                     flush_list;

    struct mutex                    mutex;
    vsf_sem_t                       sem;

    char                            name[16];

    vsf_linux_process_t             *process;
    bool                            is_to_exit;
};

static int __workqueue_thread(int argc, char **argv)
{
    VSF_LINUX_ASSERT(1 == argc);
    struct workqueue_struct *wq = (struct workqueue_struct *)argv[0];
    union {
        struct delayed_work *dwork;
        struct work_struct *work;
    } u;
    vsf_timeout_tick_t timeout;

    while (!wq->is_to_exit) {
        mutex_lock(&wq->mutex);
            vsf_dlist_peek_head(struct work_struct, entry, &wq->dwork_list, u.work);
            if (NULL == u.dwork) {
                timeout = -1;
            } else {
                vsf_systimer_tick_t now = vsf_systimer_get_tick();
                if (vsf_systimer_is_due(u.dwork->start_tick)) {
                    vsf_dlist_remove_head(struct work_struct, entry, &wq->dwork_list, u.work);
                    vsf_dlist_add_to_tail(struct work_struct, entry, &wq->work_list, u.work);
                    timeout = 0;
                } else {
                    timeout = vsf_systimer_get_duration(now, u.dwork->start_tick);
                }
            }
        mutex_unlock(&wq->mutex);

        if (timeout != 0) {
            vsf_thread_sem_pend(&wq->sem, timeout);
        }

        while (true) {
            vsf_dlist_remove_head(struct work_struct, entry, &wq->work_list, u.work);
            if (NULL == u.work) {
                break;
            }

            u.work->func(u.work);
        }
    }
    return 0;
}

struct workqueue_struct * alloc_workqueue(const char *fmt, unsigned int flags, int max_active, ...)
{
    struct workqueue_struct *wq = kzalloc(sizeof(*wq), GFP_KERNEL);
    va_list ap;

    va_start(ap, max_active);
    vsnprintf(wq->name, sizeof(wq->name), fmt, ap);
    va_end(ap);

    vsf_dlist_init(&wq->work_list);
    vsf_dlist_init(&wq->dwork_list);
    vsf_slist_init(&wq->flush_list);
    mutex_init(&wq->mutex);
    vsf_eda_sem_init(&wq->sem);
    wq->is_to_exit = false;

    char * argv[] = {
        (char *)wq,
        NULL,
    };
    wq->process = vsf_linux_start_process_internal(__workqueue_thread, argv);
    if (NULL == wq->process) {
        kfree(wq);
        return NULL;
    }

    return wq;
}

void destroy_workqueue(struct workqueue_struct *wq)
{
    wq->is_to_exit = true;
    flush_workqueue(wq);
    kfree(wq);
}

bool queue_work(struct workqueue_struct *wq, struct work_struct *work)
{
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->work_list, work)) {
        return false;
    } else {
        mutex_lock(&wq->mutex);
            vsf_dlist_add_to_tail(struct work_struct, entry, &wq->work_list, work);
        mutex_unlock(&wq->mutex);
        vsf_eda_sem_post(&wq->sem);
        return true;
    }
}

bool queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay)
{
    if (0 == delay) {
        return queue_work(wq, &dwork->work);
    } else if (vsf_dlist_is_in(struct work_struct, entry, &wq->dwork_list, &dwork->work)) {
        return false;
    } else {
        dwork->start_tick = vsf_systimer_get_tick() + delay;
        mutex_lock(&wq->mutex);
            vsf_dlist_insert(struct work_struct, entry, &wq->dwork_list, &dwork->work,
                ((struct delayed_work *)_)->start_tick >= dwork->start_tick);
        mutex_unlock(&wq->mutex);
        vsf_eda_sem_post(&wq->sem);
        return true;
    }
}

void flush_workqueue(struct workqueue_struct *wq)
{
    vsf_eda_t *eda = vsf_eda_get_cur();

    mutex_lock(&wq->mutex);
        if (vsf_dlist_is_empty(&wq->work_list) && vsf_dlist_is_empty(&wq->dwork_list)) {
            eda = NULL;
        } else {
            vsf_slist_add_to_head(vsf_eda_t, pending_snode, &wq->flush_list, eda);
        }
    mutex_unlock(&wq->mutex);

    if (eda != NULL) {
        vsf_thread_wfe(VSF_EVT_USER);
    }
}

/*******************************************************************************
* linux/device.h                                                               *
*******************************************************************************/

struct device * get_device(struct device *dev)
{
    if (dev) {
        kobject_get(&dev->kobj);
    }
    return dev;
}

void put_device(struct device *dev)
{
    if (dev) {
        kobject_put(&dev->kobj);
    }
}

int dev_set_name(struct device *dev, const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = kobject_set_name_vargs(&dev->kobj, fmt, ap);
    va_end(ap);
    return err;
}

void device_initialize(struct device *dev)
{
    kobject_init(&dev->kobj, NULL);
}

int device_add(struct device *dev)
{
    struct device *parent;
    struct kobject *kobj;
    VSF_LINUX_ASSERT(dev != NULL);

    dev = get_device(dev);

    if (!dev_name(dev)) {
        dev_set_name(dev, "%s%d", dev->bus->dev_name, dev->id);
    }
    if (!dev_name(dev)) {
        return -EINVAL;
    }

    parent = get_device(dev->parent);
    if (parent != NULL) {
        dev->kobj.parent = &parent->kobj;
    }

    put_device(dev);
    return 0;
}

void device_del(struct device *dev)
{
    struct device *parent = dev->parent;

    kobject_del(&dev->kobj);
    put_device(parent);
}

int device_register(struct device *dev)
{
    device_initialize(dev);
    return device_add(dev);
}

void device_unregister(struct device *dev)
{
    device_del(dev);
    put_device(dev);
}

/*******************************************************************************
* linux/skbuff.h                                                               *
*******************************************************************************/

#define skb_shinfo(__skb)           ((struct skb_shared_info *)(skb_end_pointer(__skb)))

struct sk_buff * alloc_skb(unsigned int size, gfp_t flags)
{
    struct sk_buff *skb = kmalloc(sizeof(*skb), 0);
    if (NULL == skb) {
        return NULL;
    }

    u8 *data = kmalloc(size + sizeof(struct skb_shared_info), flags);
    if (NULL == skb->data) {
        kfree(skb);
        return NULL;
    }

    memset(skb, 0, sizeof(*skb));
    refcount_set(&skb->users, 1);
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
    skb_set_end_offset(skb, size);

    struct skb_shared_info *shinfo = skb_shinfo(skb);
    memset(shinfo, 0, sizeof(*shinfo));
    atomic_set(&shinfo->dataref, 1);

    return skb;
}

void kfree_skb(struct sk_buff *skb)
{
    if (skb_unref(skb)) {
        if (skb->data) {
            kfree(skb->data);
        }
        kfree(skb);
    }
}

void consume_skb(struct sk_buff *skb)
{
    kfree_skb(skb);
}

#endif
