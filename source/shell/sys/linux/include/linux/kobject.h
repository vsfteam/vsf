#ifndef __VSF_LINUX_KOBJECT_H__
#define __VSF_LINUX_KOBJECT_H__

#include <linux/list.h>
#include <linux/kref.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kobj_type;
struct kobject {
    const char                  *name;
    struct list_head            entry;
    struct kobject              *parent;
    const struct kobj_type      *ktype;
    struct kref                 kref;
    bool                        state_in_sysfs;
};

struct kobj_type {
    void (*release)(struct kobject *kobj);
};

struct kobj_uevent_env {
    char                        *argv[3];
};

static inline const char * kobject_name(const struct kobject *kobj)
{
    return kobj->name;
}

extern struct kobject * kobject_get(struct kobject *kobj);
extern void kobject_put(struct kobject *kobj);
extern void kobject_del(struct kobject *kobj);

#ifdef __cplusplus
}
#endif

#endif
