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

#include "vsf_cfg.h"

#if VSF_EVM_USE_USBH == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "shell/sys/linux/include/unistd.h"
#   include "shell/sys/linux/include/libusb.h"
#else
#   include <unistd.h>
#   include <libusb.h>
#endif

#include "evm_module.h"

/*============================ MACROS ========================================*/

#if VSF_USE_LINUX != ENABLED
#   error currently eusb depends on libusb in linux subsystem
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static evm_val_t __evm_module_usbh_device_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // for libusb in VSF, libusb_device is same as libusb_device_handle
    //  so, do nothing here
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // refer to __evm_module_usbh_device_open
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_control_transfer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_set_configuration(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc > 0) {
        libusb_device_handle *dev_handle = (libusb_device_handle *)evm_object_get_ext_data(p);
        libusb_set_configuration(dev_handle, evm_2_integer(&v[0]));
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_get_string_descriptor(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc > 0) {
        libusb_device_handle *dev_handle = (libusb_device_handle *)evm_object_get_ext_data(p);
        char desc_buffer[256];

        if (0 < libusb_get_string_descriptor_ascii(dev_handle, evm_2_integer(&v[0]), (unsigned char *)desc_buffer, sizeof(desc_buffer))) {
            evm_val_t * result = evm_heap_string_create(e, desc_buffer, strlen(desc_buffer) + 1);
            if (result != NULL) {
                return *result;
            }
        }
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_claim_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_release_interface(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_usbh_device_is_kernel_driver_active(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_FALSE;
}
static evm_val_t __evm_module_usbh_device_detach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}
static evm_val_t __evm_module_usbh_device_attach_kernel_driver(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // libusb in vsf has specific driver
    return EVM_VAL_UNDEFINED;
}

static evm_val_t * __evm_module_usbh_endpoint_object_create(evm_t *e, struct libusb_endpoint_descriptor *ep_desc)
{
    evm_val_t * obj = evm_object_create(e, GC_OBJECT, 1, 0);
    int i = 0;

    if (obj != NULL) {
        // "descriptor"
        evm_val_t * desc_obj = evm_object_create(e, GC_OBJECT, 9, 0);
        if (desc_obj != NULL) {
            int j = 0;
            evm_prop_set(e, desc_obj, j++, "bLength", evm_mk_number(ep_desc->bLength));
            evm_prop_set(e, desc_obj, j++, "bDescriptorType", evm_mk_number(ep_desc->bDescriptorType));
            evm_prop_set(e, desc_obj, j++, "bEndpointAddress", evm_mk_number(ep_desc->bEndpointAddress));
            evm_prop_set(e, desc_obj, j++, "bmAttributes", evm_mk_number(ep_desc->bmAttributes));
            evm_prop_set(e, desc_obj, j++, "bmAttributes", evm_mk_number(ep_desc->bmAttributes));
            evm_prop_set(e, desc_obj, j++, "bInterval", evm_mk_number(ep_desc->bInterval));
            evm_prop_set(e, desc_obj, j++, "bRefresh", evm_mk_number(ep_desc->bRefresh));
            evm_prop_set(e, desc_obj, j++, "bSynchAddress", evm_mk_number(ep_desc->bSynchAddress));

            evm_val_t *extra = evm_buffer_create(e, ep_desc->extra_length);
            if (extra != NULL) {
                memcpy(evm_buffer_addr(extra), ep_desc->extra, ep_desc->extra_length);
                evm_prop_set(e, desc_obj, j++, "extra", *extra);
            }

            evm_prop_set(e, obj, i++, "descriptor", *desc_obj);
        }
    }
    return obj;
}

static evm_val_t * __evm_module_usbh_alt_object_create(evm_t *e, struct libusb_interface_descriptor *alt_desc)
{
    evm_val_t * obj = evm_object_create(e, GC_OBJECT, 2, 0);
    int i = 0;

    if (obj != NULL) {
        // "descriptor"
        evm_val_t * desc_obj = evm_object_create(e, GC_OBJECT, 10, 0);
        if (desc_obj != NULL) {
            int j = 0;
            evm_prop_set(e, desc_obj, j++, "bLength", evm_mk_number(alt_desc->bLength));
            evm_prop_set(e, desc_obj, j++, "bDescriptorType", evm_mk_number(alt_desc->bDescriptorType));
            evm_prop_set(e, desc_obj, j++, "bInterfaceNumber", evm_mk_number(alt_desc->bInterfaceNumber));
            evm_prop_set(e, desc_obj, j++, "bAlternateSetting", evm_mk_number(alt_desc->bAlternateSetting));
            evm_prop_set(e, desc_obj, j++, "bNumEndpoints", evm_mk_number(alt_desc->bNumEndpoints));
            evm_prop_set(e, desc_obj, j++, "bInterfaceClass", evm_mk_number(alt_desc->bInterfaceClass));
            evm_prop_set(e, desc_obj, j++, "bInterfaceSubClass", evm_mk_number(alt_desc->bInterfaceSubClass));
            evm_prop_set(e, desc_obj, j++, "bInterfaceProtocol", evm_mk_number(alt_desc->bInterfaceProtocol));
            evm_prop_set(e, desc_obj, j++, "iInterface", evm_mk_number(alt_desc->iInterface));

            evm_val_t *extra = evm_buffer_create(e, alt_desc->extra_length);
            if (extra != NULL) {
                memcpy(evm_buffer_addr(extra), alt_desc->extra, alt_desc->extra_length);
                evm_prop_set(e, desc_obj, j++, "extra", *extra);
            }

            evm_prop_set(e, obj, i++, "descriptor", *desc_obj);
        }

        // "endpoints"
        evm_val_t *ep_list = evm_list_create(e, GC_LIST, (uint16_t)alt_desc->bNumEndpoints), *ep_obj;
        struct libusb_endpoint_descriptor *ep_desc;
        for (uint_fast8_t ep_num = 0; ep_num < alt_desc->bNumEndpoints; ep_num++) {
            ep_desc = (struct libusb_endpoint_descriptor *)&alt_desc->endpoint[ep_num];
            ep_obj = __evm_module_usbh_endpoint_object_create(e, ep_desc);
            evm_prop_set_value_by_index(e, ep_list, ep_num, *ep_obj);
        }
        evm_prop_set(e, obj, i++, "endpoints", *ep_list);
    }
    return obj;
}

static evm_val_t * __evm_module_usbh_config_desc_object_create(evm_t *e, struct libusb_config_descriptor *cfg_desc)
{
    evm_val_t *desc_obj = evm_object_create(e, GC_OBJECT, 9, 0);
    if (desc_obj != NULL) {
        int i = 0;
        evm_prop_set(e, desc_obj, i++, "bLength", evm_mk_number(cfg_desc->bLength));
        evm_prop_set(e, desc_obj, i++, "bDescriptorType", evm_mk_number(cfg_desc->bDescriptorType));
        evm_prop_set(e, desc_obj, i++, "wTotalLength", evm_mk_number(cfg_desc->wTotalLength));
        evm_prop_set(e, desc_obj, i++, "bNumInterfaces", evm_mk_number(cfg_desc->bNumInterfaces));
        evm_prop_set(e, desc_obj, i++, "bConfigurationValue", evm_mk_number(cfg_desc->bConfigurationValue));
        evm_prop_set(e, desc_obj, i++, "iConfiguration", evm_mk_number(cfg_desc->iConfiguration));
        evm_prop_set(e, desc_obj, i++, "bmAttributes", evm_mk_number(cfg_desc->bmAttributes));
        evm_prop_set(e, desc_obj, i++, "bMaxPower", evm_mk_number(cfg_desc->bMaxPower));

        evm_val_t *extra = evm_buffer_create(e, cfg_desc->extra_length);
        if (extra != NULL) {
            memcpy(evm_buffer_addr(extra), cfg_desc->extra, cfg_desc->extra_length);
            evm_prop_set(e, desc_obj, i++, "extra", *extra);
        }
    }
    return desc_obj;
}

static evm_val_t * __evm_module_usbh_device_object_create(evm_t *e, libusb_device *dev)
{
    evm_val_t * obj = evm_object_create(e, GC_OBJECT, 19, 0);
    int i = 0;

    if (obj != NULL) {
        evm_object_set_ext_data(obj, (intptr_t)dev);

        evm_prop_set(e, obj, i++, "claimInterface", evm_mk_native((intptr_t)__evm_module_usbh_device_claim_interface));
        evm_prop_set(e, obj, i++, "releaseInterface", evm_mk_native((intptr_t)__evm_module_usbh_device_release_interface));
        evm_prop_set(e, obj, i++, "isKernelDriverActive", evm_mk_native((intptr_t)__evm_module_usbh_device_is_kernel_driver_active));
        evm_prop_set(e, obj, i++, "detachKernelDriver", evm_mk_native((intptr_t)__evm_module_usbh_device_detach_kernel_driver));
        evm_prop_set(e, obj, i++, "attachKernelDriver", evm_mk_native((intptr_t)__evm_module_usbh_device_attach_kernel_driver));

        evm_prop_set(e, obj, i++, "open", evm_mk_native((intptr_t)__evm_module_usbh_device_open));
        evm_prop_set(e, obj, i++, "close", evm_mk_native((intptr_t)__evm_module_usbh_device_close));
        evm_prop_set(e, obj, i++, "controlTransfer", evm_mk_native((intptr_t)__evm_module_usbh_device_control_transfer));
        evm_prop_set(e, obj, i++, "setConfiguration", evm_mk_native((intptr_t)__evm_module_usbh_device_set_configuration));
        evm_prop_set(e, obj, i++, "getStringDescriptor", evm_mk_native((intptr_t)__evm_module_usbh_device_get_string_descriptor));

        evm_prop_set(e, obj, i++, "busNumber", evm_mk_number(libusb_get_bus_number(dev)));
        evm_prop_set(e, obj, i++, "deviceAddress", evm_mk_number(libusb_get_device_address(dev)));
        evm_prop_set(e, obj, i++, "portNumbers", EVM_VAL_UNDEFINED);
        evm_prop_set(e, obj, i++, "parent", evm_mk_null());

        // "deviceDescriptor"
        struct libusb_device_descriptor dev_desc;
        if (LIBUSB_SUCCESS == libusb_get_device_descriptor(dev, &dev_desc)) {
            evm_val_t * desc_obj = evm_object_create(e, GC_OBJECT, 14, 0);
            if (desc_obj != NULL) {
                int j = 0;
                evm_prop_set(e, desc_obj, j++, "bLength", evm_mk_number(dev_desc.bLength));
                evm_prop_set(e, desc_obj, j++, "bDescriptorType", evm_mk_number(dev_desc.bDescriptorType));
                evm_prop_set(e, desc_obj, j++, "bcdUSB", evm_mk_number(dev_desc.bcdUSB));
                evm_prop_set(e, desc_obj, j++, "bDeviceClass", evm_mk_number(dev_desc.bDeviceClass));
                evm_prop_set(e, desc_obj, j++, "bDeviceSubClass", evm_mk_number(dev_desc.bDeviceSubClass));
                evm_prop_set(e, desc_obj, j++, "bDeviceProtocol", evm_mk_number(dev_desc.bDeviceProtocol));
                evm_prop_set(e, desc_obj, j++, "bMaxPacketSize0", evm_mk_number(dev_desc.bMaxPacketSize0));
                evm_prop_set(e, desc_obj, j++, "idVendor", evm_mk_number(dev_desc.idVendor));
                evm_prop_set(e, desc_obj, j++, "idProduct", evm_mk_number(dev_desc.idProduct));
                evm_prop_set(e, desc_obj, j++, "bcdDevice", evm_mk_number(dev_desc.bcdDevice));
                evm_prop_set(e, desc_obj, j++, "iManufacturer", evm_mk_number(dev_desc.iManufacturer));
                evm_prop_set(e, desc_obj, j++, "iProduct", evm_mk_number(dev_desc.iProduct));
                evm_prop_set(e, desc_obj, j++, "iSerialNumber", evm_mk_number(dev_desc.iSerialNumber));
                evm_prop_set(e, desc_obj, j++, "bNumConfigurations", evm_mk_number(dev_desc.bNumConfigurations));

                evm_prop_set(e, obj, i++, "deviceDescriptor", *desc_obj);
            }
        }

        // "configDescriptor"
        struct libusb_config_descriptor * cfg_desc;
        if (LIBUSB_SUCCESS == libusb_get_active_config_descriptor(dev, &cfg_desc)) {
            evm_val_t * desc_obj = __evm_module_usbh_config_desc_object_create(e, cfg_desc);
            if (desc_obj != NULL) {
                evm_prop_set(e, obj, i++, "configDescriptor", *desc_obj);
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
            evm_prop_set(e, obj, i++, "allConfigDescriptors", *cfg_list);
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
        evm_prop_set(e, obj, i++, "interfaces", *ifs_list);
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

static evm_val_t __evm_module_usbh(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (libusb_init(NULL) < 0) {
        evm_set_err(e, ec_type, "Can't initialize libusb");
        return EVM_VAL_UNDEFINED;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_usbh(evm_t * e)
{
    evm_builtin_t class_usbh[] = {
        {"getDeviceList", evm_mk_native((intptr_t)__evm_module_usbh_get_device_list)},
        {"setDebugLevel", evm_mk_native((intptr_t)__evm_module_usbh_set_debug_level)},
        {NULL, EVM_VAL_UNDEFINED},
    };
    return *evm_class_create(e, (evm_native_fn)__evm_module_usbh, class_usbh, NULL);
}

#endif
