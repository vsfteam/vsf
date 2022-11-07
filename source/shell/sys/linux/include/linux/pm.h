#ifndef __VSF_LINUX_PM_H__
#define __VSF_LINUX_PM_H__

#ifdef __cplusplus
extern "C" {
#endif

struct device;

struct dev_pm_ops {
    int (*prepare)(struct device *dev);
    int (*complete)(struct device *dev);
    int (*suspend)(struct device *dev);
    int (*resume)(struct device *dev);
    int (*freeze)(struct device *dev);
};

typedef struct pm_message {
    int event;
} pm_message_t;

#define pm_runtime_set_autosuspend_delay(__dev, __delay)

#ifdef __cplusplus
}
#endif

#endif
