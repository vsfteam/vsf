#ifndef __VSF_LINUX_MODULE_H__
#define __VSF_LINUX_MODULE_H__

#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __init
#   define __init
#endif
#ifndef __exit
#   define __exit
#endif

#define MODULE_DEVICE_TABLE(__DEV, __TABLE)
#define MODULE_ALIAS(__ALIAS_STR)
#define MODULE_AUTHOR(__AUTHOR_STR)
#define MODULE_DESCRIPTION(__DESC_STR)
#define MODULE_VERSION(__VERSION_STR)
#define MODULE_LICENSE(__LICENSE_STR)

#define module_driver(__drv, __register, __unregister)
#define module_init(__init_func)
#define module_exit(__exit_func)

#define MODULE_NAME_LEN                 32

struct module {
    struct list_head list;
    char name[MODULE_NAME_LEN];
};

#ifdef __cplusplus
}
#endif

#endif
