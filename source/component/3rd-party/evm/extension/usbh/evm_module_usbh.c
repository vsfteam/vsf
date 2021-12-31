/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "vsf_cfg.h"

#if VSF_EVM_USE_USBH == ENABLED && VSF_USE_EVM == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/libusb.h"
#else
#   include <unistd.h>
#   include <libusb.h>
#endif

#include "evm_module.h"
#include "evm_oo.h"

/*============================ MACROS ========================================*/

#if VSF_USE_LINUX != ENABLED
#   error currently eusb depends on libusb in linux subsystem
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static evm_val_t __evm_class_usbh_dev_claim_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_release_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_is_kernel_driver_active(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_detach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_attach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

static evm_val_t __evm_class_usbh_dev_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_control_transfer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_set_configuration(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
static evm_val_t __evm_class_usbh_dev_get_string_descriptor(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);

/*============================ LOCAL VARIABLES ===============================*/

static const evm_class_vt_t __evm_class_usbh_dev_vt[] = {
    EVM_CLASS_VT_METHOD("claimInterface",           __evm_class_usbh_dev_claim_interface),
    EVM_CLASS_VT_METHOD("releaseInterface",         __evm_class_usbh_dev_release_interface),
    EVM_CLASS_VT_METHOD("isKernelDriverActive",     __evm_class_usbh_dev_is_kernel_driver_active),
    EVM_CLASS_VT_METHOD("detachKernelDriver",       __evm_class_usbh_dev_detach_kernel_driver),
    EVM_CLASS_VT_METHOD("attachKernelDriver",       __evm_class_usbh_dev_attach_kernel_driver),
    EVM_CLASS_VT_METHOD("open",                     __evm_class_usbh_dev_open),
    EVM_CLASS_VT_METHOD("close",                    __evm_class_usbh_dev_close),
    EVM_CLASS_VT_METHOD("controlTransfer",          __evm_class_usbh_dev_control_transfer),
    EVM_CLASS_VT_METHOD("setConfiguration",         __evm_class_usbh_dev_set_configuration),
    EVM_CLASS_VT_METHOD("getStringDescriptor",      __evm_class_usbh_dev_get_string_descriptor),
    EVM_CLASS_VT_INTEGER("busNumber",               0),
    EVM_CLASS_VT_INTEGER("deviceAddress",           0),
    EVM_CLASS_VT_INTEGER("portNumbers",             0),
    EVM_CLASS_VT_INTEGER("parent",                  0),
    EVM_CLASS_VT_STRUCT("deviceDescriptor"),
    EVM_CLASS_VT_STRUCT("configDescriptor"),
    EVM_CLASS_VT_STRUCT("allConfigDescriptors"),
    EVM_CLASS_VT_STRUCT("interfaces"),
};

/*============================ GLOBAL VARIABLES ==============================*/

const evm_class_t evm_class_usbh_dev = {
    .name       = "usbh_dev",
    .vt         = __evm_class_usbh_dev_vt,
    .vt_len     = dimof(__evm_class_usbh_dev_vt),
};

/*============================ IMPLEMENTATION ================================*/

static evm_val_t __evm_class_usbh_dev_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // for libusb in VSF, libusb_device is same as libusb_device_handle
    //  so, do nothing here
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // refer to __evm_module_usbh_device_open
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_control_transfer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_set_configuration(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc > 0) {
        libusb_device_handle *dev_handle = (libusb_device_handle *)evm_object_get_ext_data(p);
        libusb_set_configuration(dev_handle, evm_2_integer(&v[0]));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_get_string_descriptor(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc > 0) {
        libusb_device_handle *dev_handle = (libusb_device_handle *)evm_object_get_ext_data(p);
        char desc_buffer[256];

        if (0 < libusb_get_string_descriptor_ascii(dev_handle, evm_2_integer(&v[0]), (unsigned char *)desc_buffer, sizeof(desc_buffer))) {
            evm_val_t *result = evm_heap_string_create(e, desc_buffer, strlen(desc_buffer) + 1);
            if (result != NULL) {
                return *result;
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_claim_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_release_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_class_usbh_dev_is_kernel_driver_active(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_FALSE;
}
static evm_val_t __evm_class_usbh_dev_detach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}
static evm_val_t __evm_class_usbh_dev_attach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t * __evm_module_usbh_endpoint_object_create(evm_t *e, struct libusb_endpoint_descriptor *ep_desc)
{
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 1, 0);
    if (obj != NULL) {
        // "descriptor"
        evm_val_t *desc_obj = evm_struct_create(e, ep_desc, 1,
            bLength, bDescriptorType, bEndpointAddress, bmAttributes, wMaxPacketSize,
            bInterval, bRefresh, bSynchAddress
        );
        if (desc_obj != NULL) {
            evm_val_t *extra = evm_buffer_create(e, ep_desc->extra_length);
            if (extra != NULL) {
                memcpy(evm_buffer_addr(extra), ep_desc->extra, ep_desc->extra_length);
                evm_prop_append(e, desc_obj, "extra", *extra);
            }

            evm_prop_append(e, obj, "descriptor", *desc_obj);
        }
    }
    return obj;
}

static evm_val_t * __evm_module_usbh_alt_object_create(evm_t *e, struct libusb_interface_descriptor *alt_desc)
{
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 2, 0);
    if (obj != NULL) {
        // "descriptor"
        evm_val_t *desc_obj = evm_struct_create(e, alt_desc, 1,
            bLength, bDescriptorType, bInterfaceNumber, bAlternateSetting, bNumEndpoints,
            bInterfaceClass, bInterfaceSubClass, bInterfaceProtocol, iInterface
        );
        if (desc_obj != NULL) {
            evm_val_t *extra = evm_buffer_create(e, alt_desc->extra_length);
            if (extra != NULL) {
                memcpy(evm_buffer_addr(extra), alt_desc->extra, alt_desc->extra_length);
                evm_prop_append(e, desc_obj, "extra", *extra);
            }

            evm_prop_append(e, obj, "descriptor", *desc_obj);
        }

        // "endpoints"
        evm_val_t *ep_list = evm_list_create(e, GC_LIST, (uint16_t)alt_desc->bNumEndpoints), *ep_obj;
        struct libusb_endpoint_descriptor *ep_desc;
        for (uint_fast8_t ep_num = 0; ep_num < alt_desc->bNumEndpoints; ep_num++) {
            ep_desc = (struct libusb_endpoint_descriptor *)&alt_desc->endpoint[ep_num];
            ep_obj = __evm_module_usbh_endpoint_object_create(e, ep_desc);
            evm_prop_set_value_by_index(e, ep_list, ep_num, *ep_obj);
        }
        evm_prop_append(e, obj, "endpoints", *ep_list);
    }
    return obj;
}

static evm_val_t * __evm_module_usbh_config_desc_object_create(evm_t *e, struct libusb_config_descriptor *cfg_desc)
{
    evm_val_t *desc_obj = evm_struct_create(e, cfg_desc, 1,
        bLength, bDescriptorType, wTotalLength, bNumInterfaces, bConfigurationValue,
        iConfiguration, bmAttributes, bMaxPower
    );
    if (desc_obj != NULL) {
        evm_val_t *extra = evm_buffer_create(e, cfg_desc->extra_length);
        if (extra != NULL) {
            memcpy(evm_buffer_addr(extra), cfg_desc->extra, cfg_desc->extra_length);
            evm_prop_append(e, desc_obj, "extra", *extra);
        }
    }
    return desc_obj;
}

static evm_val_t * __evm_module_usbh_device_object_create(evm_t *e, libusb_device *dev)
{
    evm_val_t *obj = evm_instance_create(e, &evm_class_usbh_dev);
    if (obj != NULL) {
        evm_object_set_ext_data(obj, (intptr_t)dev);

        evm_prop_set_value(e, obj, "busNumber", evm_mk_number(libusb_get_bus_number(dev)));
        evm_prop_set_value(e, obj, "deviceAddress", evm_mk_number(libusb_get_device_address(dev)));

        // "deviceDescriptor"
        struct libusb_device_descriptor dev_desc;
        if (LIBUSB_SUCCESS == libusb_get_device_descriptor(dev, &dev_desc)) {
            evm_val_t *desc_obj = evm_struct_create(e, &dev_desc, 0,
                bLength, bDescriptorType, bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol,
                bMaxPacketSize0, idVendor, idProduct, bcdDevice, iManufacturer, iProduct,
                iSerialNumber, bNumConfigurations
            );
            if (desc_obj != NULL) {
                evm_prop_set_value(e, obj, "deviceDescriptor", *desc_obj);
            }
        }

        // "configDescriptor"
        struct libusb_config_descriptor *cfg_desc;
        if (LIBUSB_SUCCESS == libusb_get_active_config_descriptor(dev, &cfg_desc)) {
            evm_val_t * desc_obj = __evm_module_usbh_config_desc_object_create(e, cfg_desc);
            if (desc_obj != NULL) {
                evm_prop_set_value(e, obj, "configDescriptor", *desc_obj);
            }
            libusb_free_config_descriptor(cfg_desc);
        }

        // "allConfigDescriptors"
        evm_val_t *cfg_list = evm_list_create(e, GC_LIST, (uint16_t)dev_desc.bNumConfigurations);
        if (cfg_list != NULL) {
            for (ssize_t j = 0; j < dev_desc.bNumConfigurations; j++) {
                if (LIBUSB_SUCCESS == libusb_get_config_descriptor(dev, j, &cfg_desc)) {
                    evm_val_t *desc_obj = __evm_module_usbh_config_desc_object_create(e, cfg_desc);
                    if (desc_obj != NULL) {
                        evm_prop_set_value_by_index(e, cfg_list, j, *desc_obj);
                    }
                }
            }
            evm_prop_set_value(e, obj, "allConfigDescriptors", *cfg_list);
        }

        // TODO: add "portNumbers"

        // "interfaces"
        evm_val_t *ifs_list = evm_list_create(e, GC_LIST, (uint16_t)cfg_desc->bNumInterfaces), *alt_list, *alt_obj;
        struct libusb_interface_descriptor *alt_desc;
        struct libusb_interface *ifs;
        for (uint_fast8_t ifs_num = 0; ifs_num < cfg_desc->bNumInterfaces; ifs_num++) {
            ifs = &cfg_desc->interface[ifs_num];
            alt_list = evm_list_create(e, GC_LIST, (uint16_t)cfg_desc->bNumInterfaces);
            if (alt_list != NULL) {
                for (uint_fast8_t alt_num = 0; alt_num < ifs->num_altsetting; alt_num++) {
                    alt_desc = (struct libusb_interface_descriptor *)&ifs->altsetting[alt_num];
                    alt_obj = __evm_module_usbh_alt_object_create(e, alt_desc);
                    evm_prop_set_value_by_index(e, alt_list, alt_num, *alt_obj);
                }
            }
            evm_prop_set_value_by_index(e, ifs_list, ifs_num, *alt_list);
        }
        evm_prop_set_value(e, obj, "interfaces", *ifs_list);
    }
    return obj;
}

static evm_val_t __evm_module_usbh_get_device_list(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t *dev_list;
    libusb_device **devs;
    ssize_t cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0) {
        evm_set_err(e, ec_type, "Can't get libusb devices");
        return EVM_VAL_UNDEFINED;
    }

    dev_list = evm_list_create(e, GC_LIST, (uint16_t)cnt);
    for (ssize_t i = 0; i < cnt; i++) {
        evm_prop_set_value_by_index(e, dev_list, i, *__evm_module_usbh_device_object_create(e, devs[i]));
    }
    libusb_free_device_list(devs, true);
    return *dev_list;
}

static evm_val_t __evm_module_usbh_set_debug_level(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc > 0) {
        libusb_set_debug(NULL, evm_2_integer(&v[0]));
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_module_usbh(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"getDeviceList", evm_mk_native((intptr_t)__evm_module_usbh_get_device_list)},
        {"setDebugLevel", evm_mk_native((intptr_t)__evm_module_usbh_set_debug_level)},
        {NULL, EVM_VAL_UNDEFINED},
    };

    if (libusb_init(NULL) < 0) {
        evm_set_err(e, ec_type, "Can't initialize libusb");
        return EVM_VAL_UNDEFINED;
    }

    evm_module_create(e, "usbh", builtin);
    return e->err;
}

#endif
