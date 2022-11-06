#ifndef __VSF_LINUX_DEVICE_H__
#define __VSF_LINUX_DEVICE_H__

#include <linux/kobject.h>
#include <linux/types.h>
#include <linux/mutex.h>
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
};

static inline struct device * kobj_to_dev(struct kobject *kobj)
{
    return container_of(kobj, struct device, kobj);
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

extern int dev_set_name(struct device *dev, const char *fmt, ...);

static inline int device_is_registered(struct device *dev)
{
    return dev->kobj.state_in_sysfs;
}
extern int device_register(struct device *dev);
extern void device_unregister(struct device *dev);
extern void device_initialize(struct device *dev);
extern int device_add(struct device *dev);
extern void device_del(struct device *dev);

extern char * devm_kvasprintf(struct device *dev, gfp_t gfp, const char *fmt, va_list ap);
extern char * devm_kasprintf(struct device *dev, gfp_t gfp, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
