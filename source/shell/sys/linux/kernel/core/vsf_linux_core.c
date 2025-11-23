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
#include <fcntl.h>
#include <linux/types.h>
#include <linux/kobject.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/device.h>

#include <pthread.h>
#include <stdio.h>

/*============================ MACROS ========================================*/

#if VSF_SYNC_CFG_SUPPORT_ISR != ENABLED
#   error VSF_SYNC_CFG_SUPPORT_ISR is necessary for queuing work in isr
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

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
    struct kobject *kobj = vsf_container_of(kref, struct kobject, kref);
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

int add_uevent_var(struct kobj_uevent_env *env, const char *format, ...)
{
    return -1;
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

    bool                            is_to_exit;
};

struct workqueue_struct *system_wq;

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

static void * __workqueue_thread(void *arg)
{
    struct workqueue_struct *wq = (struct workqueue_struct *)arg;
    union {
        struct delayed_work *dwork;
        struct work_struct *work;
    } u;
    vsf_timeout_tick_t timeout;
    vsf_protect_t orig;
    vsf_eda_t *pending_eda;

    while (!wq->is_to_exit) {
        orig = vsf_protect_int();
        vsf_dlist_peek_head(struct work_struct, entry, &wq->dwork_list, u.work);
        if (NULL == u.dwork) {
            timeout = vsf_dlist_is_empty(&wq->work_list) ? -1 : 0;
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

        if (timeout < 0) {
            while (true) {
                vsf_slist_remove_head(vsf_eda_t, pending_snode, &wq->flush_list, pending_eda);
                vsf_unprotect_int(orig);
                if (NULL == pending_eda) {
                    break;
                }

                vsf_eda_post_evt(pending_eda, VSF_EVT_USER);
                orig = vsf_protect_int();
            }
        } else {
            vsf_unprotect_int(orig);
        }

        if (timeout != 0) {
            vsf_thread_sem_pend(&wq->sem, timeout);
        }

        while (true) {
            orig = vsf_protect_int();
            vsf_dlist_remove_head(struct work_struct, entry, &wq->work_list, u.work);
            if (NULL == u.work) {
                vsf_unprotect_int(orig);
                break;
            }

            u.work->__is_running = true;
            vsf_unprotect_int(orig);

            u.work->func(u.work);

            orig = vsf_protect_int();
            u.work->__is_running = false;
            pending_eda = u.work->__pending_eda;
            vsf_unprotect_int(orig);

            if (pending_eda != NULL) {
                vsf_eda_post_evt(pending_eda, VSF_EVT_USER);
            }
        }
    }
    return NULL;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

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

    if (pthread_create(NULL, NULL, __workqueue_thread, wq) != 0) {
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
    VSF_LINUX_ASSERT((wq != NULL) && (work != NULL));
    work->__wq = wq;
    vsf_protect_t orig = vsf_protect_int();
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->work_list, work)) {
        vsf_unprotect_int(orig);
        return false;
    } else {
        vsf_dlist_add_to_tail(struct work_struct, entry, &wq->work_list, work);
        vsf_unprotect_int(orig);
        vsf_eda_sem_post_isr(&wq->sem);
        return true;
    }
}

bool queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay)
{
    VSF_LINUX_ASSERT((wq != NULL) && (dwork != NULL));
    if (0 == delay) {
        return queue_work(wq, &dwork->work);
    }

    dwork->work.__wq = wq;
    vsf_protect_t orig = vsf_protect_int();
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->dwork_list, &dwork->work)) {
        vsf_unprotect_int(orig);
        return false;
    } else {
        vsf_dlist_insert(struct work_struct, entry, &wq->dwork_list, &dwork->work,
                ((struct delayed_work *)_)->start_tick >= dwork->start_tick);
        vsf_unprotect_int(orig);
        dwork->start_tick = vsf_systimer_get_tick() + delay;
        vsf_eda_sem_post_isr(&wq->sem);
        return true;
    }
}

bool mod_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay)
{
    bool result;
    vsf_protect_t orig = vsf_protect_int();
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->dwork_list, &dwork->work)) {
        vsf_dlist_remove(struct work_struct, entry, &wq->dwork_list, &dwork->work);
        vsf_unprotect_int(orig);
        result = false;
    } else if (vsf_dlist_is_in(struct work_struct, entry, &wq->work_list, &dwork->work)) {
        vsf_dlist_remove(struct work_struct, entry, &wq->work_list, &dwork->work);
        vsf_unprotect_int(orig);
        result = true;
    } else {
        result = false;
    }

    queue_delayed_work(wq, dwork, delay);
    return result;
}

void flush_workqueue(struct workqueue_struct *wq)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);

    vsf_protect_t orig = vsf_protect_int();
        if (vsf_dlist_is_empty(&wq->work_list) && vsf_dlist_is_empty(&wq->dwork_list)) {
            eda = NULL;
        } else {
            vsf_slist_add_to_head(vsf_eda_t, pending_snode, &wq->flush_list, eda);
        }
    vsf_unprotect_int(orig);

    if (eda != NULL) {
        vsf_thread_wfe(VSF_EVT_USER);
    }
}

bool flush_work(struct work_struct *work)
{
    struct workqueue_struct *wq = work->__wq;
    if (NULL == wq) {
        return false;
    }

    vsf_eda_t *eda_cur = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda_cur != NULL);
    vsf_protect_t orig = vsf_protect_int();
    if (    vsf_dlist_is_in(struct work_struct, entry, &wq->work_list, work)
        ||  vsf_dlist_is_in(struct work_struct, entry, &wq->dwork_list, work)
        ||  work->__is_running) {
        work->__pending_eda = eda_cur;
        vsf_unprotect_int(orig);
        vsf_thread_wfe(VSF_EVT_USER);
        return true;
    } else {
        vsf_unprotect_int(orig);
        return false;
    }
}

// __queue_cancel_work MUST be called locked
static bool __queue_cancel_work(struct workqueue_struct *wq, struct work_struct *work, bool is_to_wait, vsf_protect_t orig)
{
    vsf_eda_t *eda_cur = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda_cur != NULL);
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->work_list, work)) {
        vsf_dlist_remove(struct work_struct, entry, &wq->work_list, work);
        vsf_unprotect_int(orig);
        return true;
    } else if (is_to_wait && work->__is_running) {
        work->__pending_eda = eda_cur;
        vsf_unprotect_int(orig);
        vsf_thread_wfe(VSF_EVT_USER);
        return true;
    } else {
        vsf_unprotect_int(orig);
        return false;
    }
}

static bool __queue_cancel_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, bool is_to_wait, vsf_protect_t orig)
{
    if (vsf_dlist_is_in(struct work_struct, entry, &wq->dwork_list, &dwork->work)) {
        vsf_dlist_remove(struct work_struct, entry, &wq->dwork_list, &dwork->work);
        vsf_unprotect_int(orig);
        return true;
    } else {
        return __queue_cancel_work(wq, &dwork->work, is_to_wait, orig);
    }
}

bool cancel_work(struct work_struct *work)
{
    struct workqueue_struct *wq = work->__wq;
    if (NULL == wq) {
        return false;
    }

    return __queue_cancel_work(wq, work, false, vsf_protect_int());
}

bool cancel_work_sync(struct work_struct *work)
{
    struct workqueue_struct *wq = work->__wq;
    if (NULL == wq) {
        return false;
    }

    return __queue_cancel_work(wq, work, true, vsf_protect_int());
}

bool flush_delayed_work(struct delayed_work *dwork)
{
    return flush_work(&dwork->work);
}

bool cancel_delayed_work(struct delayed_work *dwork)
{
    struct workqueue_struct *wq = dwork->work.__wq;
    if (NULL == wq) {
        return false;
    }

    return __queue_cancel_delayed_work(wq, dwork, false, vsf_protect_int());
}

bool cancel_delayed_work_sync(struct delayed_work *dwork)
{
    struct workqueue_struct *wq = dwork->work.__wq;
    if (NULL == wq) {
        return false;
    }

    return __queue_cancel_delayed_work(wq, dwork, true, vsf_protect_int());
}

void workqueue_init_early(void)
{
    system_wq = alloc_workqueue("events", 0, 0);
}

/*******************************************************************************
* linux/wait.h                                                                 *
*******************************************************************************/

int autoremove_wake_function(struct wait_queue_entry *wqe, unsigned mode, int sync, void *key)
{
    list_del_init(&wqe->entry);
    return 1;
}

long prepare_to_wait_event(struct wait_queue_head *wqh, struct wait_queue_entry *wqe, int state)
{
    unsigned long flags;
    spin_lock_irqsave(&wqh->lock, flags);
    if (list_empty(&wqe->entry)) {
        list_add_tail(&wqe->entry, &wqh->head);
    }
    spin_unlock_irqrestore(&wqh->lock, flags);
    return 0;
}

void init_wait_entry(struct wait_queue_entry *wqe, int flags)
{
    wqe->__trig = NULL;
    wqe->flags = flags;
    wqe->func = autoremove_wake_function;
    INIT_LIST_HEAD(&wqe->entry);
}

void finish_wait(struct wait_queue_head *wqh, struct wait_queue_entry *wqe)
{
    unsigned long flags;
    spin_lock_irqsave(&wqh->lock, flags);
    if (!list_empty(&wqe->entry)) {
        list_del_init(&wqe->entry);
    }
    spin_unlock_irqrestore(&wqh->lock, flags);
}

void wake_up_nr(struct wait_queue_head *wqh, int nr)
{
    struct list_head tmp, *p, *n;
    struct wait_queue_entry *wqe;
    unsigned long flags;

    INIT_LIST_HEAD(&tmp);

    spin_lock_irqsave(&wqh->lock, flags);
    list_for_each_safe(p, n, &wqh->head) {
        if (!nr) break;
        wqe = list_entry(p, struct wait_queue_entry, entry);
        if (wqe->__trig != NULL) {
            list_del_init(p);
            list_add_tail(p, &tmp);
            if (nr > 0) {
                --nr;
            }
        }
    }
    spin_unlock_irqrestore(&wqh->lock, flags);

    while (!list_empty(&tmp)) {
        wqe = list_first_entry(&tmp, struct wait_queue_entry, entry);
        list_del_init(&wqe->entry);
        vsf_eda_trig_set(wqe->__trig);
    }
}

void init_wait_queue_head(struct wait_queue_head *wqh)
{
    spin_lock_init(&wqh->lock);
    INIT_LIST_HEAD(&wqh->head);
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

int driver_register(struct device_driver *drv)
{
    return 0;
}

void driver_unregister(struct device_driver *drv)
{
}

int bus_register(struct bus_type *bus)
{
    return 0;
}

void bus_unregister(struct bus_type *bus)
{
}

int bus_rescan_devices(struct bus_type *bus)
{
    return 0;
}

/*******************************************************************************
* linux/firmware.h                                                             *
*******************************************************************************/

#include <linux/firmware.h>

VSF_CAL_WEAK(vsf_linux_firmware_read)
int vsf_linux_firmware_read(struct firmware *fw, const char *name)
{
    int dirfd = open(VSF_LINUX_CFG_FW_PATH, 0);
    if (dirfd < 0) {
        return -1;
    }

    int fd = openat(dirfd, name, 0);
    close(dirfd);
    if (fd < 0) {
        return -1;
    }

    fw->size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    fw->data = kmalloc(fw->size, GFP_KERNEL);
    if (NULL == fw->data) {
        goto close_and_fail;
    }
    if (fw->size == read(fd, (void *)fw->data, fw->size)) {
        return 0;
    }

    kfree(fw->data);
    fw->data = NULL;
close_and_fail:
    close(fd);
    return -1;
}

VSF_CAL_WEAK(vsf_linux_firmware_release)
void vsf_linux_firmware_release(struct firmware *fw)
{
    if (fw->data != NULL) {
        kfree(fw->data);
        fw->data = NULL;
    }
}

int request_firmware(const struct firmware **fw, const char *name, struct device *device)
{
    VSF_LINUX_ASSERT(fw != NULL);
    *fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);
    if (NULL == *fw) {
        return -1;
    }

    return vsf_linux_firmware_read((struct firmware *)*fw, name);
}

void release_firmware(const struct firmware *fw)
{
    vsf_linux_firmware_release((struct firmware *)fw);
    kfree(fw);
}

/*******************************************************************************
* linux/power_supply.h                                                         *
*******************************************************************************/

#include <linux/power_supply.h>

struct power_supply * power_supply_register(struct device *parent,
                const struct power_supply_desc *desc,
                const struct power_supply_config *cfg)
{
    return NULL;
}

void power_supply_unregister(struct power_supply *psy)
{
}

int power_supply_powers(struct power_supply *psy, struct device *dev)
{
    return -1;
}

void * power_supply_get_drvdata(struct power_supply *psy)
{
    return NULL;
}

void power_supply_changed(struct power_supply *psy)
{
}

/*******************************************************************************
* linux/uuid.h                                                                 *
*******************************************************************************/

#include <linux/uuid.h>

const guid_t guid_null;
const uuid_t uuid_null;

/*******************************************************************************
* linux/idr.h                                                                  *
*******************************************************************************/

#include <linux/idr.h>

int ida_alloc_range(struct ida *ida, unsigned int __min_to_avoid_conflict,
                    unsigned int __max_to_avoid_conflict, gfp_t gfp)
{
    const int bits_in_value = sizeof(uintalu_t) << 3;
    uintalu_t value;
    unsigned int pos_in_map, pos, mask_bits_high;
    vsf_protect_t orig;
    int id;

    pos = __min_to_avoid_conflict % bits_in_value;
    pos_in_map = __min_to_avoid_conflict / bits_in_value;
    if (pos) {
        __min_to_avoid_conflict = roundup(__min_to_avoid_conflict, bits_in_value);

        value = (1ULL << pos) - 1;
        if (__min_to_avoid_conflict > __max_to_avoid_conflict) {
            mask_bits_high = __min_to_avoid_conflict - __max_to_avoid_conflict - 1;
            value |= ((1ULL << mask_bits_high) - 1) << (bits_in_value - mask_bits_high);
        }

        orig = vsf_protect_int();
        value = ida->bitmap[pos_in_map] | value;
        extern int_fast8_t __vsf_arch_ffz(uintalu_t);
        id = __vsf_arch_ffz(value);
        if (id >= 0) {
            ida->bitmap[pos_in_map] |= 1 << id;
            vsf_unprotect_int(orig);
            return pos_in_map * bits_in_value + id;
        }
        vsf_unprotect_int(orig);
        pos_in_map++;
    }

    if (__min_to_avoid_conflict < __max_to_avoid_conflict) {
        __max_to_avoid_conflict -= __min_to_avoid_conflict;
        __min_to_avoid_conflict = VSF_LINUX_CFG_IDA_MAX - __min_to_avoid_conflict;
        __max_to_avoid_conflict = vsf_min(__max_to_avoid_conflict, __min_to_avoid_conflict);
        __max_to_avoid_conflict++;      // include min

        orig = vsf_protect_int();
        id = vsf_bitmap_ffz(&ida->bitmap[pos_in_map], __max_to_avoid_conflict);
        vsf_unprotect_int(orig);
        return id;
    }

    return -1;
}

/*******************************************************************************
* linux/kstrtox.h                                                              *
*******************************************************************************/

#include <linux/kstrtox.h>

int kstrtou16(const char *s, unsigned int base, u16 *res)
{
    unsigned long long val = strtoull(s, NULL, base);
    if (val != (u16)val) {
        return -ERANGE;
    }
    *res = val;
    return 0;
}

int kstrtos16(const char *s, unsigned int base, s16 *res)
{
    long long val = strtoll(s, NULL, base);
    if (val != (s16)val) {
        return -ERANGE;
    }
    *res = val;
    return 0;
}

int kstrtou8(const char *s, unsigned int base, u8 *res)
{
    unsigned long long val = strtoull(s, NULL, base);
    if (val != (u8)val) {
        return -ERANGE;
    }
    *res = val;
    return 0;
}

int kstrtos8(const char *s, unsigned int base, s8 *res)
{
    long long val = strtoll(s, NULL, base);
    if (val != (s8)val) {
        return -ERANGE;
    }
    *res = val;
    return 0;
}

/*******************************************************************************
* linux/timer.h                                                                *
*******************************************************************************/

#include <linux/timer.h>

void add_timer(struct timer_list *timer)
{
}

int del_timer(struct timer_list *timer)
{
    return 0;
}

int del_timer_sync(struct timer_list *timer)
{
    return 0;
}

int mod_timer(struct timer_list *timer, unsigned long expires)
{
    return 0;
}

int timer_pending(const struct timer_list *timer)
{
    return -1;
}

void timer_setup(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags)
{
}

/*******************************************************************************
* linux/skbuff.h                                                               *
*******************************************************************************/

#include <linux/skbuff.h>

#if defined(VSF_LINUX_CFG_SKB_SIZE) && defined(VSF_LINUX_CFG_SKB_NUM)
#   define __VSF_LINUX_CFG_SKB_POOL ENABLED

typedef struct vsf_ieee80211_skb_t {
    struct sk_buff skb;
    uint8_t buffer[VSF_LINUX_CFG_SKB_SIZE];
} vsf_ieee80211_skb_t;
dcl_vsf_pool(vsf_ieee80211_skb_pool)
def_vsf_pool(vsf_ieee80211_skb_pool, vsf_ieee80211_skb_t)
imp_vsf_pool(vsf_ieee80211_skb_pool, vsf_ieee80211_skb_t)

static vsf_pool(vsf_ieee80211_skb_pool) __vsf_ieee80211_skb_pool;
static vsf_pool_item(vsf_ieee80211_skb_pool) __vsf_ieee80211_skb_pool_items[VSF_LINUX_CFG_SKB_NUM];
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#define skb_shinfo(__skb)           ((struct skb_shared_info *)(skb_end_pointer(__skb)))

struct sk_buff * alloc_skb(unsigned int size, gfp_t flags)
{
    struct sk_buff *skb;
    u8 *data;
#if __VSF_LINUX_CFG_SKB_POOL == ENABLED
    vsf_protect_t orig = vsf_protect_int();
        vsf_ieee80211_skb_t *ieee80211_skb = VSF_POOL_ALLOC(vsf_ieee80211_skb_pool, &__vsf_ieee80211_skb_pool);
    vsf_unprotect_int(orig);
    if (NULL == ieee80211_skb) {
        return NULL;
    }
    skb = &ieee80211_skb->skb;
    data = (u8 *)&skb[1];
#else
    skb = kmalloc(sizeof(*skb), 0);
    if (NULL == skb) {
        return NULL;
    }

    data = kmalloc(size + sizeof(struct skb_shared_info), flags);
    if (NULL == data) {
        kfree(skb);
        return NULL;
    }
#endif

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

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

void kfree_skb(struct sk_buff *skb)
{
    if (skb_unref(skb)) {
#if __VSF_LINUX_CFG_SKB_POOL == ENABLED
        vsf_ieee80211_skb_t *ieee80211_skb = vsf_container_of(skb, vsf_ieee80211_skb_t, skb);
        vsf_protect_t orig = vsf_protect_int();
            VSF_POOL_FREE(vsf_ieee80211_skb_pool, &__vsf_ieee80211_skb_pool, ieee80211_skb);
        vsf_unprotect_int(orig);
#else
        if (skb->head) {
            kfree(skb->head);
        }
        kfree(skb);
#endif
    }
}

void skb_init(void)
{
#if __VSF_LINUX_CFG_SKB_POOL == ENABLED
    VSF_POOL_PREPARE(vsf_ieee80211_skb_pool, &__vsf_ieee80211_skb_pool);
    VSF_POOL_ADD_BUFFER(vsf_ieee80211_skb_pool, &__vsf_ieee80211_skb_pool,
        __vsf_ieee80211_skb_pool_items, sizeof(__vsf_ieee80211_skb_pool_items));
#endif
}

/*******************************************************************************
* devm                                                                         *
*******************************************************************************/

char * devm_kvasprintf(struct device *dev, gfp_t gfp, const char *fmt, va_list ap)
{
    va_list aq;
    int len;
    char *p;

    va_copy(aq, ap);
    len = vsnprintf(NULL, 0, fmt, aq);
    va_end(aq);

    p = kmalloc(len + 1, gfp);
    if (NULL == p) {
        return NULL;
    }

    vsnprintf(p, len + 1, fmt, ap);
    return p;
}

char * devm_kasprintf(struct device *dev, gfp_t gfp, const char *fmt, ...)
{
    va_list ap;
    char *p;

    va_start(ap, fmt);
    p = devm_kvasprintf(dev, gfp, fmt, ap);
    va_end(ap);

    return p;
}

struct power_supply * devm_power_supply_register(struct device *parent,
                const struct power_supply_desc *desc,
                const struct power_supply_config *cfg)
{
    return NULL;
}

#endif
