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

#include <unistd.h>

#include <linux/types.h>
#include <linux/kobject.h>
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
* kernel object                                                                *
*******************************************************************************/

int kobject_set_name_vargs(struct kobject *kobj, const char *fmt, va_list ap)
{
    int size = vsnprintf(NULL, 0, fmt, ap);
    if (size > 0) {
        if (kobj->name != NULL) {
            vsf_heap_free(kobj->name);
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
