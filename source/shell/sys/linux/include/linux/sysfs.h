#ifndef __VSF_LINUX_SYSFS_H__
#define __VSF_LINUX_SYSFS_H__

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

struct attribute {
    const char                      *name;
    umode_t                         mode;
};

struct attribute_group {
    const char                      *name;
    struct attribute                **attrs;
};

#define __ATTR(__NAME, __MODE, __SHOW, __STORE) {                               \
        .attr = { .name = VSF_STR(__NAME), .mode = (__MODE), },                 \
        .show = (__SHOW),                                                       \
        .store = (__STORE),                                                     \
}

#define __ATTRIBUTE_GROUPS(__NAME)                                              \
static const struct attribute_group * __NAME##_groups[] = {                     \
    &__NAME##_group, NULL,                                                      \
}

#define ATTRIBUTE_GROUPS(__NAME)                                                \
static const struct attribute_group __NAME##_group = {                          \
    .attrs = __NAME##_attrs,                                                    \
};                                                                              \
__ATTRIBUTE_GROUPS(__NAME)

#ifdef __cplusplus
}
#endif

#endif
