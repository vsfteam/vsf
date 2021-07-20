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

#include "hal/vsf_hal_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

// for vsf_pool
#include "service/vsf_service.h"

// for __VSF_OS_SWI_NUM
#include "kernel/vsf_kernel.h"

#include "hal/vsf_hal.h"

#include "bt_user_driver.h"
#include "bt_hci.h"

#include "btstack_config.h"
#include "btstack_debug.h"
#include "hci.h"
#include "hci_transport.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct hci_transport_aic8800_buffer_t {
    uint8_t buffer[256];
} hci_transport_aic8800_buffer_t;

declare_vsf_pool(hci_transport_aic8800_buffer_pool)
def_vsf_pool(hci_transport_aic8800_buffer_pool, hci_transport_aic8800_buffer_t)

typedef struct hci_transport_aic8800_param_t {
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);
    vsf_pool(hci_transport_aic8800_buffer_pool) buffer_pool;

    uint8_t init_script_offset;
} hci_transport_aic8800_param_t;

/*============================ PROTOTYPES ====================================*/

// hci_transport
static void __hci_transport_aic8800_init(const void *transport_config);
static int __hci_transport_aic8800_open(void);
static int __hci_transport_aic8800_close(void);
static void __hci_transport_aic8800_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
static int __hci_transport_aic8800_can_send_packet_now(uint8_t packet_type);
static int __hci_transport_aic8800_send_packet(uint8_t packet_type, uint8_t *packet, int size);

// chipset
static void __btstack_chipset_aic8800_init(const void * config);
static btstack_chipset_result_t __btstack_chipset_aic8800_next_command(uint8_t * hci_cmd_buffer);
static void __btstack_chipset_aic8800_set_bd_addr_command(bd_addr_t addr, uint8_t *hci_cmd_buffer);
static void __btstack_chipset_aic8800_set_ble_addr_command(bd_addr_t addr, uint8_t *hci_cmd_buffer);

/*============================ GLOBAL VARIABLES ==============================*/

static const hci_transport_t __hci_transport_aic8800 = {
    .name                       = "AIC8800_VSF",
    .init                       = __hci_transport_aic8800_init,
    .open                       = __hci_transport_aic8800_open,
    .close                      = __hci_transport_aic8800_close,
    .register_packet_handler    = __hci_transport_aic8800_register_packet_handler,
    .can_send_packet_now        = __hci_transport_aic8800_can_send_packet_now,
    .send_packet                = __hci_transport_aic8800_send_packet,
};

static btstack_chipset_t __btstack_chipset_aic8800 = {
    .name                       = "AIC8800",
    .init                       = __btstack_chipset_aic8800_init,
    .next_command               = __btstack_chipset_aic8800_next_command,
    .set_bd_addr_command        = __btstack_chipset_aic8800_set_bd_addr_command,
//    .set_ble_addr_command       = __btstack_chipset_aic8800_set_ble_addr_command,
};

static const uint32_t __aic8800_rf_mdm_regs_table_bt_only[][2] = {
    {0x40580104, 0x000923fb},
    {0x4062201c, 0x0008d000},
    {0x40622028, 0x48912020},
    {0x40622014, 0x00018983},
    {0x40622054, 0x00008f34},
    {0x40620748, 0x021a01a0},
    {0x40620728, 0x00010020},
    {0x40620738, 0x04800fd4},
    {0x4062073c, 0x00c80064},
    {0x4062202c, 0x000cb220},
    {0x4062200c, 0xe9ad2b45},
    {0x40622030, 0x143c30d2},
    {0x40622034, 0x00001602},
    {0x40620754, 0x214220fd},
    {0x40620758, 0x0007f01e},
    {0x4062071c, 0x00000a33},
    {0x40622018, 0x00124124},
    {0x4062000c, 0x04040000},
    {0x40620090, 0x00069082},
    {0x40621034, 0x02003080},
    {0x40621014, 0x0445117a},
    {0x40622024, 0x00001100},
    {0x40622004, 0x0001a9c0},
    {0x4060048c, 0x00500834},
    {0x40600110, 0x027e0058},
    {0x40600880, 0x00500834},
    {0x40600884, 0x00500834},
    {0x40600888, 0x00500834},
    {0x4060088c, 0x00000834},
    {0x4062050c, 0x20202013},
    {0x406205a0, 0x181c0000},
    {0x406205a4, 0x36363636},
    {0x406205f0, 0x0000ff00},
    {0x40620508, 0x54553132},
    {0x40620530, 0x140f0b00},
    {0x406205b0, 0x00005355},
    {0x4062051c, 0x964b5766},
};

/*============================ LOCAL VARIABLES ===============================*/

static hci_transport_aic8800_param_t __hci_transport_aic8800_param;

/*============================ IMPLEMENTATION ================================*/

//implement_vsf_pool(hci_transport_aic8800_buffer_pool, hci_transport_aic8800_buffer_t)
#define __name hci_transport_aic8800_buffer_pool
#define __type hci_transport_aic8800_buffer_t
#include "service/pool/impl_vsf_pool.inc"

static uint32_t __hci_transport_aic8800_rx_handler(const uint8_t *data, uint32_t len)
{
    VSF_HAL_ASSERT((__hci_transport_aic8800_param.packet_handler != NULL) && (len > 0));
    __hci_transport_aic8800_param.packet_handler(data[0], (uint8_t *)&data[1], len - 1);

    bt_hci_rx_done(BT_HCI_CH_0);
    return len;
}

static void __hci_transport_aic8800_tx_handler(const uint8_t *data, uint32_t len)
{
    VSF_POOL_FREE(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool,
                    (hci_transport_aic8800_buffer_t *)data);

    const uint8_t event[] = { HCI_EVENT_TRANSPORT_PACKET_SENT, 0 };
    VSF_HAL_ASSERT(__hci_transport_aic8800_param.packet_handler != NULL);
    __hci_transport_aic8800_param.packet_handler(HCI_EVENT_PACKET, (uint8_t *)event, sizeof(event));
}

static int __hci_transport_aic8800_open(void)
{
    bt_hci_open(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART,
            __hci_transport_aic8800_rx_handler, __hci_transport_aic8800_tx_handler, true);
    bt_hci_rx_start(BT_HCI_CH_0);
    return 0;
}

static int __hci_transport_aic8800_close(void)
{
    bt_hci_rx_stop(BT_HCI_CH_0);
    bt_hci_close(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART);
    return 0;
}

static void __hci_transport_aic8800_register_packet_handler(
        void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size))
{
    __hci_transport_aic8800_param.packet_handler = handler;
}

static int __hci_transport_aic8800_can_send_packet_now(uint8_t packet_type)
{
    return bt_hci_tx_available(BT_HCI_CH_0);
}

static int __hci_transport_aic8800_send_packet(uint8_t packet_type, uint8_t *packet, int size)
{
    uint8_t *buffer = (uint8_t *)VSF_POOL_ALLOC(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool);
    VSF_HAL_ASSERT((buffer != NULL) && (size < 256));
    buffer[0] = packet_type;
    memcpy(&buffer[1], packet, size);
    size++;

    return bt_hci_tx(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART, buffer, size);
}

static void __hci_transport_aic8800_init(const void *transport_config)
{
    VSF_POOL_INIT(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool, 8,
        .region_ptr     = (vsf_protect_region_t *)&vsf_protect_region_int,
    );
    bt_launch();

    hci_set_chipset(&__btstack_chipset_aic8800);
}

const hci_transport_t * hci_transport_aic8800_instance(void)
{
    return &__hci_transport_aic8800;
}

// chipset driver implementation
static void __btstack_chipset_aic8800_init(const void * config)
{
    __hci_transport_aic8800_param.init_script_offset = 0;
}

static btstack_chipset_result_t __btstack_chipset_aic8800_next_command(uint8_t * hci_cmd_buffer)
{
    uint16_t init_script_offset = __hci_transport_aic8800_param.init_script_offset;

    if (init_script_offset < dimof(__aic8800_rf_mdm_regs_table_bt_only)) {
        // wr_rf_mdm_regs
        uint32_t reg_addr = __aic8800_rf_mdm_regs_table_bt_only[init_script_offset][0];
        uint32_t reg_val = __aic8800_rf_mdm_regs_table_bt_only[init_script_offset][1];

        hci_cmd_buffer[0] = 0x53;
        hci_cmd_buffer[1] = 0xFC;
        hci_cmd_buffer[2] = 4 + 8;

        init_script_offset <<= 3;
        hci_cmd_buffer[3] = (init_script_offset >> 0) & 0xFF;
        hci_cmd_buffer[4] = (init_script_offset >> 8) & 0xFF;
        hci_cmd_buffer[5] = 0;
        hci_cmd_buffer[6] = 8;

        hci_cmd_buffer[7] = (reg_addr >> 0) & 0xFF;
        hci_cmd_buffer[8] = (reg_addr >> 8) & 0xFF;
        hci_cmd_buffer[9] = (reg_addr >> 16) & 0xFF;
        hci_cmd_buffer[10] = (reg_addr >> 24) & 0xFF;

        hci_cmd_buffer[11] = (reg_val >> 0) & 0xFF;
        hci_cmd_buffer[12] = (reg_val >> 8) & 0xFF;
        hci_cmd_buffer[13] = (reg_val >> 16) & 0xFF;
        hci_cmd_buffer[14] = (reg_val >> 24) & 0xFF;
    } else if (init_script_offset == (0 + dimof(__aic8800_rf_mdm_regs_table_bt_only))) {
        // set_rf_mode
        const uint8_t hci_cmd_set_rf_mode[] = {
            0x48, 0xFC, 0x01, 0x01,
        };
        memcpy(hci_cmd_buffer, hci_cmd_set_rf_mode, sizeof(hci_cmd_set_rf_mode));
    } else if (init_script_offset == (1 + dimof(__aic8800_rf_mdm_regs_table_bt_only))) {
        // rf_calib_req
        const uint8_t hci_cmd_rf_calib_req[] = {
            0x4B, 0xFC, 0x0C, 0x01, 0x00, 0x00, 0x08, 0x13, 0x42, 0x26, 0x00, 0x0F, 0x30, 0x02, 0x00,
        };
        memcpy(hci_cmd_buffer, hci_cmd_rf_calib_req, sizeof(hci_cmd_rf_calib_req));
    } else {
        hci_cmd_buffer = NULL;
    }

    if (hci_cmd_buffer != NULL) {
        __hci_transport_aic8800_param.init_script_offset++;
        return BTSTACK_CHIPSET_VALID_COMMAND;
    }
    return BTSTACK_CHIPSET_DONE;
}

static void __btstack_chipset_aic8800_set_bd_addr_command(bd_addr_t addr, uint8_t *hci_cmd_buffer)
{
    hci_cmd_buffer[0] = 0x70;
    hci_cmd_buffer[1] = 0xFC;
    hci_cmd_buffer[2] = 0x06;
    reverse_bd_addr(addr, &hci_cmd_buffer[3]);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe177
#endif
// actually, btstack_chipset_t doesn't have set_ble_addr_command
//  but aic8800 need this to set ble command
//  TODO: how to add it to btstack?
static void __btstack_chipset_aic8800_set_ble_addr_command(bd_addr_t addr, uint8_t *hci_cmd_buffer)
{
    hci_cmd_buffer[0] = 0x32;
    hci_cmd_buffer[1] = 0xFC;
    hci_cmd_buffer[2] = 0x06;
    reverse_bd_addr(addr, &hci_cmd_buffer[3]);
}

#endif      // VSF_USE_BTSTACK
