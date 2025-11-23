#ifndef __VSF_LINUX_DEVICE_BUS_H__
#define __VSF_LINUX_DEVICE_BUS_H__

#include <linux/types.h>
#include <linux/kobject.h>
#include <linux/pm.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device;
struct device_driver;

struct bus_type {
    const char                      *name;
    const char                      *dev_name;
    struct device                   *dev_root;

    int (*match)(struct device *dev, struct device_driver *drv);
    int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    int (*shutdown)(struct device *dev);

    int (*suspend)(struct device *dev, pm_message_t state);
    int (*resume)(struct device *dev);

    const struct dev_pm_ops         *pm;
};

extern int bus_register(struct bus_type *bus);
extern void bus_unregister(struct bus_type *bus);
extern void bus_probe_device(struct device *dev);
extern int bus_rescan_devices(struct bus_type *bus);

#ifdef __cplusplus
}
#endif

#endif
