#ifndef __VSF_LINUX_DEVICE_DRIVER_H__
#define __VSF_LINUX_DEVICE_DRIVER_H__

#include <linux/types.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/device/bus.h>

#ifdef __cplusplus
extern "C" {
#endif

struct device_driver {
    const char              *name;
    struct bus_type         *bus;

    struct module           *owner;
    const char              *mod_name;

    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    int (*shutdown)(struct device *dev);
    int (*suspend)(struct device *dev);
    int (*resume)(struct device *dev);
};

#ifdef __cplusplus
}
#endif

#endif
