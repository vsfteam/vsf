/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_EVM == ENABLED

#include "evm_module.h"
#include "evm_oo.h"

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if !defined(VSF_EVM_MODULE_HAL_USE_DEVFS) && !defined(VSF_EVM_MODULE_HAL_USE_VSF)
#   define VSF_EVM_MODULE_HAL_USE_VSF           ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct _evm_hal_gpio_dev_t {
    int pin;
    int direction;
    int mode;
    int edge;
} _evm_hal_gpio_dev_t;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_EVM_MODULE_HAL_USE_VSF == ENABLED
#   include "../template/modules/hal/evm_module_hal_vsf.inc"
#elif VSF_EVM_MODULE_HAL_USE_DEVFS == ENABLED
#   include "../template/modules/hal/evm_module_hal_devfs.inc"
#else
#   error SHOULD NOT error here, becasue default hal proting is based on vsf
#endif

#if EVM_MODULE_HAL_SUPPORT_GPIO
/*============================ TYPES =========================================*/

enum {
    EVM_MODULE_GPIO_ERR_NONE    = 0,
    EVM_MODULE_GPIO_ERR_MEM,
    EVM_MODULE_GPIO_ERR_PIN,
    EVM_MODULE_GPIO_ERR_DIR,
    EVM_MODULE_GPIO_ERR_MODE,
    EVM_MODULE_GPIO_ERR_HW,
    EVM_MODULE_GPIO_ERR_OBJ,
    EVM_MODULE_GPIO_ERR_NUM,
};

/*============================ PROTOTYPES ====================================*/

static evm_val_t evm_module_gpio_class_setDirectionSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t evm_module_gpio_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

/*============================ LOCAL VARIABLES ===============================*/

static const evm_class_vt_t __evm_module_gpio_class_vt[] = {
    EVM_CLASS_VT_METHOD("setDirectionSync",         evm_module_gpio_class_setDirectionSync),
    EVM_CLASS_VT_METHOD("write",                    evm_module_gpio_class_write),
    EVM_CLASS_VT_METHOD("writeSync",                evm_module_gpio_class_writeSync),
    EVM_CLASS_VT_METHOD("read",                     evm_module_gpio_class_read),
    EVM_CLASS_VT_METHOD("readSync",                 evm_module_gpio_class_readSync),
    EVM_CLASS_VT_METHOD("close",                    evm_module_gpio_class_close),
    EVM_CLASS_VT_METHOD("closeSync",                evm_module_gpio_class_closeSync),
};

static const evm_class_t __evm_module_gpio_class = {
    .name       = "gpio.pin",
    .vt         = __evm_module_gpio_class_vt,
    .vt_len     = dimof(__evm_module_gpio_class_vt),
};

static const char * __evm_module_gpio_errstring[EVM_MODULE_GPIO_ERR_NUM] = {
    [EVM_MODULE_GPIO_ERR_MEM]   = "Insufficient external memory",
    [EVM_MODULE_GPIO_ERR_PIN]   = "Configuration has no 'pin' membe",
    [EVM_MODULE_GPIO_ERR_DIR]   = "Configuration has no 'direction' member",
    [EVM_MODULE_GPIO_ERR_MODE]  = "Configuration has no 'mode' member",
    [EVM_MODULE_GPIO_ERR_HW]    = "Hardware error while opening gpio",
    [EVM_MODULE_GPIO_ERR_OBJ]   = "Failed to instantiate",
};

/*============================ IMPLEMENTATION ================================*/

static evm_val_t __evm_module_gpio_open_imp(evm_t *e, evm_val_t *p, evm_val_t conf, int *errcode)
{
    int _errcode;
    evm_val_t *val;

    evm_hal_gpio_dev_t *dev = evm_malloc(sizeof(evm_hal_gpio_dev_t));
    if (!dev) {
        _errcode = -EVM_MODULE_GPIO_ERR_MEM;
        goto fail;
    }

    val = evm_prop_get(e, &conf, "pin", 0);
    if (NULL == val || !evm_is_integer(val)) {
        _errcode = -EVM_MODULE_GPIO_ERR_PIN;
        goto fail;
    }
    dev->pin = evm_2_integer(val);

    val = evm_prop_get(e, &conf, "direction", 0);
    if (NULL == val || !evm_is_integer(val)) {
        _errcode = -EVM_MODULE_GPIO_ERR_DIR;
        goto fail;
    }
    dev->direction = evm_2_integer(val);

    val = evm_prop_get(e, &conf, "mode", 0);
    if (NULL == val || !evm_is_integer(val)) {
        _errcode = -EVM_MODULE_GPIO_ERR_MODE;
        goto fail;
    }
    dev->mode = evm_2_integer(val);

    if (evm_hal_gpio_open(dev) < 0) {
        _errcode = -EVM_MODULE_GPIO_ERR_HW;
        goto fail;
    }

    {
        evm_val_t *gpio_obj = evm_instance_create(e, &__evm_module_gpio_class);
        if (NULL == gpio_obj) {
            _errcode = -EVM_MODULE_GPIO_ERR_OBJ;
            goto fail;
        }

        evm_object_set_ext_data(gpio_obj, (intptr_t)dev);
        return *gpio_obj;
    }

fail:
    if (errcode != NULL) {
        *errcode = _errcode;
    }
    if (dev != NULL) {
        evm_free(dev);
    }
    return EVM_VAL_UNDEFINED;
}

//gpio.openSync(configuration)
static evm_val_t evm_module_gpio_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc != 1) {
        return EVM_VAL_UNDEFINED;
    }

    int errcode;
    evm_val_t result = __evm_module_gpio_open_imp(e, p, v[0], &errcode);
    if (EVM_VAL_UNDEFINED == result) {
        evm_set_err(e, ec_type, __evm_module_gpio_errstring[-errcode]);
    }
    return result;
}

//gpio.open(configuration, callback)
static evm_val_t evm_module_gpio_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if ((argc != 2) && evm_is_script(v + 1)) {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t result = evm_module_gpio_openSync(e, p, argc - 1, v);
    evm_val_t args[2] = {
        [0] = result == EVM_VAL_UNDEFINED ? evm_mk_foreign_string(e->err_arg) : EVM_VAL_NULL,
        [1] = result == EVM_VAL_UNDEFINED ? EVM_VAL_NULL : result,
    };
    evm_run_callback(e, v + 1, result == EVM_VAL_UNDEFINED ? NULL : &e->scope, args, 2);
    return result;
}

//gpiopin.setDirectionSync(direction)
static evm_val_t evm_module_gpio_class_setDirectionSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_hal_gpio_dev_t *dev = (evm_hal_gpio_dev_t *)evm_object_get_ext_data(p);
    if (NULL == dev || argc < 1 || !evm_is_integer(v)) {
        return EVM_VAL_UNDEFINED;
    }

    int new_direction = evm_2_integer(v);
    if (new_direction != dev->direction) {
        dev->direction = evm_2_integer(v);
        evm_hal_gpio_direction(dev);
    }
    return EVM_VAL_UNDEFINED;
}

//gpiopin.writeSync(value)
static evm_val_t evm_module_gpio_class_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_hal_gpio_dev_t *dev = (evm_hal_gpio_dev_t *)evm_object_get_ext_data(p);
    if (NULL == dev || argc < 1 || !evm_is_integer(v)) {
        return EVM_VAL_UNDEFINED;
    }

    evm_hal_gpio_write(dev, evm_2_integer(v) ? 1 : 0);
    return EVM_VAL_UNDEFINED;
}

//gpiopin.write(value[, callback])
static evm_val_t evm_module_gpio_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_gpio_class_writeSync(e, p, argc, v);
    if (argc > 1 && evm_is_script(v + 1)) {
        evm_val_t args = evm_mk_null();
        evm_run_callback(e, v + 1, NULL, &args, 1);
    }
    return ret;
}

//gpiopin.readSync()
static evm_val_t evm_module_gpio_class_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_hal_gpio_dev_t *dev = (evm_hal_gpio_dev_t *)evm_object_get_ext_data(p);
    if (NULL == dev) {
        return EVM_VAL_UNDEFINED;
    }

    int status = evm_hal_gpio_read(dev);
    return status ? EVM_VAL_TRUE : EVM_VAL_FALSE;
}

//gpiopin.read([callback])
static evm_val_t evm_module_gpio_class_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_gpio_class_readSync(e, p, argc, v);
    if (argc > 0 && evm_is_script(v)) {
        evm_val_t args[2] = {
            [0] = evm_mk_null(),
            [1] = ret,
        };
        evm_run_callback(e, v, NULL, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//gpiopin.closeSync()
static evm_val_t evm_module_gpio_class_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_hal_gpio_dev_t *dev = (evm_hal_gpio_dev_t *)evm_object_get_ext_data(p);
    if (NULL == dev) {
        return EVM_VAL_UNDEFINED;
    }

    evm_hal_gpio_close(dev);
    return EVM_VAL_UNDEFINED;
}

//gpiopin.close([callback])
static evm_val_t evm_module_gpio_class_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_gpio_class_closeSync(e, p, argc, v);
    if (argc > 0 && evm_is_script(v)) {
        evm_val_t args = evm_mk_null();
        evm_run_callback(e, v, NULL, &args, 1);
    }
    return ret;
}

evm_err_t evm_module_gpio(evm_t *e) {
    evm_val_t *dir_prop = evm_object_create(e, GC_OBJECT, 2, 0);
    if (dir_prop) {
        evm_prop_append(e, dir_prop, "IN", evm_mk_number(EVM_GPIO_DIRECTION_IN));
        evm_prop_append(e, dir_prop, "OUT", evm_mk_number(EVM_GPIO_DIRECTION_OUT));
    } else {
        return e->err;
    }

    evm_val_t *mode_prop = evm_object_create(e, GC_OBJECT, 6, 0);
    if (mode_prop) {
        evm_prop_append(e, mode_prop, "NONE", evm_mk_number(EVM_GPIO_MODE_NONE));
        evm_prop_append(e, mode_prop, "PULLUP", evm_mk_number(EVM_GPIO_MODE_PULLUP));
        evm_prop_append(e, mode_prop, "PULLDOWN", evm_mk_number(EVM_GPIO_MODE_PULLDOWN));
        evm_prop_append(e, mode_prop, "FLOAT", evm_mk_number(EVM_GPIO_MODE_FLOAT));
        evm_prop_append(e, mode_prop, "PUSHPULL", evm_mk_number(EVM_GPIO_MODE_PUSHPULL));
        evm_prop_append(e, mode_prop, "OPENDRAIN", evm_mk_number(EVM_GPIO_MODE_OPENDRAIN));
    } else {
        return e->err;
    }

    evm_val_t *edge_prop = evm_object_create(e, GC_OBJECT, 4, 0);
    if (edge_prop) {
        evm_prop_append(e, edge_prop, "NONE", evm_mk_number(EVM_GPIO_EDGE_NONE));
        evm_prop_append(e, edge_prop, "RISING", evm_mk_number(EVM_GPIO_EDGE_RISING));
        evm_prop_append(e, edge_prop, "FALLING", evm_mk_number(EVM_GPIO_EDGE_FALLING));
        evm_prop_append(e, edge_prop, "BOTH", evm_mk_number(EVM_GPIO_EDGE_BOTH));
    } else {
        return e->err;
    }

    evm_builtin_t builtin[] = {
        {"open", evm_mk_native((intptr_t)evm_module_gpio_open)},
        {"openSync", evm_mk_native((intptr_t)evm_module_gpio_openSync)},
        {"DIRECTION", *dir_prop},
        {"MODE", *mode_prop},
        {"EDGE", *edge_prop},
        {NULL, NULL}
    };
    evm_module_create(e, "gpio", builtin);
    evm_pop(e);
    evm_pop(e);
    evm_pop(e);
    return e->err;
}
#endif      // VSF_EVM_MODULE_HAL_SUPPORT_GPIO

#endif      // VSF_USE_EVM
