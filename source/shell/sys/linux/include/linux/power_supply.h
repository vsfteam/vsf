#ifndef __VSF_LINUX_POWER_SUPPLY_H__
#define __VSF_LINUX_POWER_SUPPLY_H__

#ifdef __cplusplus
extern "C" {
#endif

enum power_supply_type {
    POWER_SUPPLY_TYPE_UNKNOWN = 0,
    POWER_SUPPLY_TYPE_BATTERY,
    POWER_SUPPLY_TYPE_UPS,
    POWER_SUPPLY_TYPE_MAINS,
    POWER_SUPPLY_TYPE_USB,
    POWER_SUPPLY_TYPE_USB_DCP,
    POWER_SUPPLY_TYPE_USB_CDP,
    POWER_SUPPLY_TYPE_USB_ACA,
    POWER_SUPPLY_TYPE_USB_TYPE_C,
    POWER_SUPPLY_TYPE_USB_PD,
    POWER_SUPPLY_TYPE_USB_PD_DRP,
    POWER_SUPPLY_TYPE_APPLE_BRICK_ID,
    POWER_SUPPLY_TYPE_WIRELESS,
};

enum power_supply_property {
    POWER_SUPPLY_PROP_STATUS = 0,
    POWER_SUPPLY_PROP_CHARGE_TYPE,
    POWER_SUPPLY_PROP_CAPACITY_LEVEL,
    POWER_SUPPLY_PROP_SCOPE,
    POWER_SUPPLY_PROP_MODEL_NAME,
};

enum {
    POWER_SUPPLY_SCOPE_UNKNOWN = 0,
    POWER_SUPPLY_SCOPE_SYSTEM,
    POWER_SUPPLY_SCOPE_DEVICE,
};

enum {
    POWER_SUPPLY_STATUS_UNKNOWN = 0,
    POWER_SUPPLY_STATUS_CHARGING,
    POWER_SUPPLY_STATUS_DISCHARGING,
    POWER_SUPPLY_STATUS_NOT_CHARGING,
    POWER_SUPPLY_STATUS_FULL,
};

enum {
    POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN = 0,
    POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL,
    POWER_SUPPLY_CAPACITY_LEVEL_LOW,
    POWER_SUPPLY_CAPACITY_LEVEL_NORMAL,
    POWER_SUPPLY_CAPACITY_LEVEL_HIGH,
    POWER_SUPPLY_CAPACITY_LEVEL_FULL,
};

union power_supply_propval {
    int                                 intval;
    const char                          *strval;
};

struct power_supply;
struct power_supply_desc {
    const char                          *name;
    enum power_supply_type              type;
    const enum power_supply_property    *properties;
    size_t                              num_properties;

    int (*get_property)(struct power_supply *psy,
                enum power_supply_property psp,
                union power_supply_propval *val);
    int (*set_property)(struct power_supply *psy,
                enum power_supply_property psp,
                const union power_supply_propval *val);
};

struct power_supply_config {
    void                                *drv_data;
};

struct power_supply {
    const struct power_supply_desc      *desc;
};

#ifdef __cplusplus
}
#endif

#endif
