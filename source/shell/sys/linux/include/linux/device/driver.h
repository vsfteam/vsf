#ifndef __VSF_LINUX_DEVICE_DRIVER_H__
#define __VSF_LINUX_DEVICE_DRIVER_H__

#include <linux/types.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/device/bus.h>

#ifdef __cplusplus
extern "C" {
#endif

#define module_driver(__driver, __register, __unregister, ...)                  \
static void __init __driver##_init(void)                                        \
{                                                                               \
    __register(&(__driver) , ##__VA_ARGS__);                                    \
}                                                                               \
module_init(__driver##_init);                                                   \
static void __exit __driver##_exit(void)                                        \
{                                                                               \
    __unregister(&(__driver) , ##__VA_ARGS__);                                  \
}                                                                               \
module_exit(__driver##_exit);

#define module_driver_init(__driver)                                            \
module_run_init(__driver##_init)

struct device_driver {
    const char              *name;
    struct bus_type         *bus;

    struct module           *owner;
    const char              *mod_name;

    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
    int (*suspend)(struct device *dev);
    int (*resume)(struct device *dev);
};

static inline int driver_match_device(const struct device_driver *drv, struct device *dev)
{
    return drv->bus->match ? drv->bus->match(dev, (struct device_driver *)drv) : 1;
}

extern int driver_register(struct device_driver *drv);
extern void driver_unregister(struct device_driver *drv);
extern int driver_probe_device(const struct device_driver *drv, struct device *dev);

#ifdef __cplusplus
}
#endif

#endif
