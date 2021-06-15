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

#if VSF_EVM_USE_BLUETOOTH == ENABLED && VSF_USE_EVM == ENABLED

#include "evm_module.h"

#include "kernel/vsf_kernel.h"
#include "btstack_event.h"
#include "hci.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum evm_module_bluetooth_result_t {
    EVM_BT_OK                   = 0,
    EVM_BT_NOT_INIT             = 10000,
    EVM_BT_NOT_AVAIL            = 10001,
    EVM_BT_NO_DEVICE            = 10002,
    EVM_BT_CONNECTION_FAIL      = 10003,
    EVM_BT_NO_SERVICE           = 10004,
    EVM_BT_NO_CHARACTERISTIC    = 10005,
    EVM_BT_NO_CONNECTION        = 10006,
    EVM_BT_PROPERTY_NOT_SUPPORT = 10007,
    EVM_BT_SYSTEM_NOT_SUPPORT   = 10008,
} evm_module_bluetooth_result_t;

typedef struct evm_module_bluetooth_t {
    btstack_packet_callback_registration_t reg;
    vsf_eda_t *eda;

    enum {
        EVM_BT_FUNC_openBluetoothAdapter,
    } func;
    union {
        struct {
            // parameter
            // result
            HCI_STATE state;
        } openBluetoothAdapter;
    } ctx;
} evm_module_bluetooth_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT evm_module_bluetooth_t __evm_module_bt;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __evm_module_bluetooth_notify_eda(vsf_evt_t evt)
{
    vsf_eda_t *eda = __evm_module_bt.eda;
    __evm_module_bt.eda = NULL;
    vsf_eda_post_evt(eda, evt);
}

static void __evm_module_bluetooth_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    uint8_t event = hci_event_packet_get_type(packet);
    bd_addr_t addr;

    switch (event) {
    case BTSTACK_EVENT_STATE:
        if (EVM_BT_FUNC_openBluetoothAdapter == __evm_module_bt.func) {
            __evm_module_bt.ctx.openBluetoothAdapter.state =  btstack_event_state_get_state(packet);
            if (__evm_module_bt.ctx.openBluetoothAdapter.state == HCI_STATE_WORKING) {
                vsf_trace_info("btstack started...\n");
                if (__evm_module_bt.eda != NULL) {
                    __evm_module_bluetooth_notify_eda(VSF_EVT_USER);
                }
            }
        }
        break;
    case GAP_EVENT_INQUIRY_RESULT:
        break;
    case GAP_EVENT_INQUIRY_COMPLETE:
        break;
    default:
        break;
    }
}

static evm_val_t __evm_module_bluetooth_open_adapter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1) {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *param = &v[0];

    VSF_ASSERT(NULL == __evm_module_bt.eda);
    __evm_module_bt.func = EVM_BT_FUNC_openBluetoothAdapter;
    __evm_module_bt.eda = vsf_eda_get_cur();

    hci_set_inquiry_mode(INQUIRY_MODE_RSSI_AND_EIR);
    __evm_module_bt.reg.callback = &__evm_module_bluetooth_packet_handler;
    hci_add_event_handler(&__evm_module_bt.reg);
    hci_power_control(HCI_POWER_ON);

    vsf_thread_wfe(VSF_EVT_USER);

    evm_val_t *cb_func, args[1];
    switch (__evm_module_bt.ctx.openBluetoothAdapter.state) {
    case HCI_STATE_WORKING:
        cb_func = evm_prop_get(e, param, "success", 0);
        if ((cb_func != NULL) && evm_is_function(cb_func)) {
            args[0] = evm_mk_number(EVM_BT_OK);
            evm_run_callback(e, cb_func, p, args, 1);
        }
        break;
    default:
        cb_func = evm_prop_get(e, param, "fail", 0);
        if ((cb_func != NULL) && evm_is_function(cb_func)) {
            args[0] = evm_mk_number(EVM_BT_NOT_INIT);
            evm_run_callback(e, cb_func, p, args, 1);
        }
        break;
    }
    cb_func = evm_prop_get(e, param, "complete", 0);
    if ((cb_func != NULL) && evm_is_function(cb_func)) {
        evm_run_callback(e, cb_func, p, args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

static evm_val_t __evm_module_bluetooth(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    __evm_module_bt.eda = NULL;
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_bluetooth(evm_t * e)
{
    evm_builtin_t class_bluetooth[] = {
        {"openBluetoothAdapter", evm_mk_native((intptr_t)__evm_module_bluetooth_open_adapter)},
        {NULL, EVM_VAL_UNDEFINED},
    };
    return *evm_class_create(e, (evm_native_fn)__evm_module_bluetooth, class_bluetooth, NULL);
}

#endif
