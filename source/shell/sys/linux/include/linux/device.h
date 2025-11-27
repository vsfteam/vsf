#ifndef __VSF_LINUX_DEVICE_H__
#define __VSF_LINUX_DEVICE_H__

#include <linux/kobject.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#include <linux/device/bus.h>
#include <linux/device/class.h>
#include <linux/device/driver.h>

#include <linux/gfp.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device_type {
    const char                  *name;
    int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
    void (*release)(struct device *dev);
    const struct dev_pm_ops     *pm;
};

struct device_attribute {
    struct attribute            attr;
    ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
};

struct device {
    struct kobject              kobj;
    struct device               *parent;

    u32                         id;
    const struct device_type    *type;
    struct bus_type             *bus;
    struct device_driver        *driver;
    void                        *driver_data;

    struct vsf_dlist_t          devres_head;
};

static inline struct device * kobj_to_dev(struct kobject *kobj)
{
    return vsf_container_of(kobj, struct device, kobj);
}

static inline const char * dev_bus_name(const struct device *dev)
{
    return dev->bus ? dev->bus->name : NULL;
}

static inline const char * dev_name(const struct device *dev)
{
    return kobject_name(&dev->kobj);
}

static inline void * dev_get_drvdata(const struct device *dev)
{
    return dev->driver_data;
}

static inline void dev_set_drvdata(struct device *dev, void *data)
{
    dev->driver_data = data;
}

#define dev_dbg(__dev, __fmt, ...)          pr_debug(__fmt, ##__VA_ARGS__)
#define dev_emerg(__dev, __fmt, ...)        pr_emerg(__fmt, ##__VA_ARGS__)
#define dev_crit(__dev, __fmt, ...)         pr_crit(__fmt, ##__VA_ARGS__)
#define dev_alert(__dev, __fmt, ...)        pr_alert(__fmt, ##__VA_ARGS__)
#define dev_err(__dev, __fmt, ...)          pr_err(__fmt, ##__VA_ARGS__)
#define dev_warn(__dev, __fmt, ...)         pr_warn(__fmt, ##__VA_ARGS__)
#define dev_notice(__dev, __fmt, ...)       pr_notice(__fmt, ##__VA_ARGS__)
#define dev_info(__dev, __fmt, ...)         pr_info(__fmt, ##__VA_ARGS__)

#define dev_level_once(__level, __dev, __fmt, ...)                              \
        do {                                                                    \
            static bool __printed = false;                                      \
            if (!__printed) {                                                   \
                __printed = true;                                               \
                __level(__dev, __fmt, ##__VA_ARGS__);                           \
            }                                                                   \
        } while (0)
#define dev_dbg_once(__dev, __fmt, ...)     dev_level_once(dev_dbg, __dev, __fmt, ##__VA_ARGS__)
#define dev_emerg_once(__dev, __fmt, ...)   dev_level_once(dev_emerg, __dev, __fmt, ##__VA_ARGS__)
#define dev_crit_once(__dev, __fmt, ...)    dev_level_once(dev_crit, __dev, __fmt, ##__VA_ARGS__)
#define dev_alert_once(__dev, __fmt, ...)   dev_level_once(dev_alert, __dev, __fmt, ##__VA_ARGS__)
#define dev_err_once(__dev, __fmt, ...)     dev_level_once(dev_err, __dev, __fmt, ##__VA_ARGS__)
#define dev_warn_once(__dev, __fmt, ...)    dev_level_once(dev_warn, __dev, __fmt, ##__VA_ARGS__)
#define dev_notice_once(__dev, __fmt, ...)  dev_level_once(dev_notice, __dev, __fmt, ##__VA_ARGS__)
#define dev_info_once(__dev, __fmt, ...)    dev_level_once(dev_info, __dev, __fmt, ##__VA_ARGS__)

extern struct device *get_device(struct device *dev);
extern void put_device(struct device *dev);
extern bool kill_device(struct device *dev);

extern int dev_set_name(struct device *dev, const char *fmt, ...);

#define device_wakeup_enable(__dev)
#define device_wakeup_disable(__dev)

static inline int device_is_registered(struct device *dev)
{
    return dev->kobj.state_in_sysfs;
}
extern int device_register(struct device *dev);
extern void device_unregister(struct device *dev);
extern void device_initialize(struct device *dev);
extern int device_add(struct device *dev);
extern void device_del(struct device *dev);

// devres and devm

typedef void (*dr_release_t)(struct device *dev, void *res);

extern void devres_add(struct device *dev, void *res);
extern int devres_release_all(struct device *dev);

static inline void * devm_kmalloc(struct device *dev, size_t size, gfp_t gfp)
{
    return kmalloc(size, gfp);
}
static inline void * devm_krealloc(struct device *dev, void *ptr, size_t size, gfp_t gfp)
{
    return krealloc(ptr, size, gfp);
}
static inline void *devm_kzalloc(struct device *dev, size_t size, gfp_t gfp)
{
    return devm_kmalloc(dev, size, gfp | __GFP_ZERO);
}
static inline void devm_kfree(struct device *dev, const void *p)
{
    kfree(p);
}

extern char * devm_kvasprintf(struct device *dev, gfp_t gfp, const char *fmt, va_list ap);
extern char * devm_kasprintf(struct device *dev, gfp_t gfp, const char *fmt, ...);

int __devm_add_action(struct device *dev, void (*action)(void *), void *data, const char *name);
#define devm_add_action(dev, action, data)          __devm_add_action(dev, action, data, #action)

static inline int __devm_add_action_or_reset(struct device *dev, void (*action)(void *), void *data, const char *name)
{
    int ret = __devm_add_action(dev, action, data, name);
    if (ret) { action(data); }
    return ret;
}
#define devm_add_action_or_reset(dev, action, data) __devm_add_action_or_reset(dev, action, data, #action)

#ifdef __cplusplus
}
#endif

#endif
