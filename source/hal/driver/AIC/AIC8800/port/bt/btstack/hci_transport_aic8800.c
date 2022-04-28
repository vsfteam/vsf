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

#include "hal/vsf_hal_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

// for vsf_pool
#include "service/vsf_service.h"

#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"

#include "hal/vsf_hal.h"

#include "bt_user_driver.h"
#include "bt_hci.h"

#include "btstack_config.h"
#include "btstack_debug.h"
#include "hci.h"
#include "hci_transport.h"

/*============================ MACROS ========================================*/

#ifndef VSF_AIC8800_BTSTACK_CFG_PRIORITY
#   define VSF_AIC8800_BTSTACK_CFG_PRIORITY             vsf_prio_inherit
#endif

#ifndef VSF_AIC8800_BTSTACK_CFG_MAX_PACKET_SIZE
#   define VSF_AIC8800_BTSTACK_CFG_MAX_PACKET_SIZE      (8 + 256)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct hci_transport_aic8800_buffer_t {
    uint8_t buffer[VSF_AIC8800_BTSTACK_CFG_MAX_PACKET_SIZE];
} hci_transport_aic8800_buffer_t;

declare_vsf_pool(hci_transport_aic8800_buffer_pool)
def_vsf_pool(hci_transport_aic8800_buffer_pool, hci_transport_aic8800_buffer_t)

typedef struct hci_transport_aic8800_param_t {
    void (*packet_handler)(uint8_t packet_type, uint8_t *packet, uint16_t size);
    vsf_pool(hci_transport_aic8800_buffer_pool) buffer_pool;

    vsf_teda_t task;
    uint8_t *tx_buffer;
    uint16_t tx_size;
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

/*============================ LOCAL VARIABLES ===============================*/

static hci_transport_aic8800_param_t __hci_transport_aic8800_param;

/*============================ IMPLEMENTATION ================================*/

/*******************************************************************************
 * code copied from bt_aic8800_driver.c in vendor SDK
*******************************************************************************/

#include "compiler.h"
#include "bt_patch_table.h"
#include "dbg.h"

#define BT_POWERON            1
#define BT_POWEROFF           0

typedef enum {
    DRV_RF_MODE_NULL          = 0x00,
    DRV_RF_MODE_BT_ONLY       = 0x01,
    DRV_RF_MODE_BT_COMBO      = 0x02,
    DRV_RF_MODE_BTWIFI_COMBO  = 0x03,
    DRV_RF_MODE_NUM           = 0x04
}BtDrvRfModeEnum;

enum {
    BT_LP_LEVEL_POWER_OFF  = 0x00,//BT CORE power off, CPUSYS power off, VCORE power off
    BT_LP_LEVEL_CLOCK_GATE = 0x01,//BT CORE clock gate, CPUSYS clock gate, VCORE clock_gate
};

enum {
    BT_LP_LEVEL_ACTIVE      = 0x00,//BT CORE active, CPUSYS active, VCORE active
    BT_LP_LEVEL_CLOCK_GATE1 = 0x01,//BT CORE clock gate, CPUSYS active, VCORE active
    BT_LP_LEVEL_CLOCK_GATE2 = 0x02,//BT CORE clock gate, CPUSYS clock gate, VCORE active
    BT_LP_LEVEL_CLOCK_GATE3 = 0x03,//BT CORE clock gate, CPUSYS clock gate, VCORE clock_gate
    BT_LP_LEVEL_POWER_OFF1  = 0x04,//BT CORE power off, CPUSYS active, VCORE active
    BT_LP_LEVEL_POWER_OFF2  = 0x05,//BT CORE power off, CPUSYS clock gate, VCORE active
    BT_LP_LEVEL_POWER_OFF3  = 0x06,//BT CORE power off, CPUSYS power off, VCORE active
    BT_LP_LEVEL_HIBERNATE   = 0x07,//BT CORE power off, CPUSYS power off, VCORE power off
    BT_LP_LEVEL_NUM         = 0x08,
};

enum {
    BT_PRV_SLP_TIMER       = (0x01UL << 0),
    BT_STACK_SLP_DISALLOW  = (0x01UL << 1),
    BLE_STACK_SLP_DISALLOW = (0x01UL << 2),
};

typedef struct
{
    /// Em save start address
    uint32_t em_save_start_addr;
    /// Em save end address
    uint32_t em_save_end_addr;
    /// Minimum time that allow power off (in hs)
    int32_t aon_min_power_off_duration;
    /// Maximum aon params
    uint16_t aon_max_nb_params;
    /// RF config const time on cpus side (in hus)
    int16_t aon_rf_config_time_cpus;
    /// RF config const time on aon side (in hus)
    int16_t aon_rf_config_time_aon;
    /// Maximum active acl link supported by aon
    uint16_t aon_max_nb_active_acl;
    /// Maximum ble activity supported by aon
    uint16_t aon_ble_activity_max;
    /// Maximum bt rxdesc field supported by aon
    uint16_t aon_max_bt_rxdesc_field;
    /// Maximum ble rxdesc field supported by aon
    uint16_t aon_max_ble_rxdesc_field;
    /// Maximum regs supported by aon
    uint16_t aon_max_nb_regs;
    /// Maximum length of ke_env supported by aon
    uint16_t aon_max_ke_env_len;
    /// Maximum elements of sch_arb_env supported by aon
    uint16_t aon_max_nb_sch_arb_elt;
    /// Maximun elements of sch_plan_env supported by aon
    uint16_t aon_max_nb_sch_plan_elt;
    /// Maximun elements of sch_alarm_env supported by aon
    uint16_t aon_max_nb_sch_alarm_elt;
    /// Minimum advertising interval in slots(625 us) supported by aon
    uint32_t aon_min_ble_adv_intv;
    /// Minimum connection interval in 2-slots(1.25 ms) supported by aon
    uint32_t aon_min_ble_con_intv;
    /// Extra sleep duration for cpus(in hs), may be negative
    int32_t aon_extra_sleep_duration_cpus;
    /// Extra sleep duration for aon cpu(in hs), may be negative
    int32_t aon_extra_sleep_duration_aon;
    /// Minimum time that allow host to power off (in us)
    int32_t aon_min_power_off_duration_cpup;
    /// aon debug level for cpus
    uint32_t aon_debug_level;
    /// aon debug level for aon cpu
    uint32_t aon_debug_level_aon;
    /// Power on delay of bt core on when cpu_sys alive on cpus side(in lp cycles)
    uint16_t aon_bt_pwr_on_dly1;
    /// Power on delay of bt core when cpu_sys clock gate on cpus side(in lp cycles)
    uint16_t aon_bt_pwr_on_dly2;
    /// Power on delay of bt core when cpu_sys power off on cpus side(in lp cycles)
    uint16_t aon_bt_pwr_on_dly3;
    /// Power on delay of bt core on aon side(in lp cycles)
    uint16_t aon_bt_pwr_on_dly_aon;
    /// Time to cancel sch arbiter elements in advance when switching to cpus (in hus)
    uint16_t aon_sch_arb_cancel_in_advance_time;
    /// Duration of sleep and wake-up algorithm (depends on CPU speed) expressed in half us on cpus side
    /// should also contian deep_sleep_on rising edge to finecnt halt (max 4 lp cycles) and finecnt resume to dm_slp_irq (0.5 lp cycles)
    uint16_t aon_sleep_algo_dur_cpus;
    /// Duration of sleep and wake-up algorithm (depends on CPU speed) expressed in half us on aon side
    /// should also contian deep_sleep_on rising edge to finecnt halt (max 4 lp cycles) and finecnt resume to dm_slp_irq (0.5 lp cycles)
    uint16_t aon_sleep_algo_dur_aon;
    /// Threshold that treat fractional part of restore time (in hus) as 1hs on cpus side
    uint16_t aon_restore_time_ceil_cpus;
    /// Threshold that treat fractional part of restore time (in hus) as 1hs on aon side
    uint16_t aon_restore_time_ceil_aon;
    /// Minimum time that allow deep sleep on cpus side (in hs)
    uint16_t aon_min_sleep_duration_cpus;
    /// Minimum time that allow deep sleep on aon side (in hs)
    uint16_t aon_min_sleep_duration_aon;
    /// Difference of restore time and save time on cpus side (in hus)
    int16_t aon_restore_save_time_diff_cpus;
    /// Difference of restore time and save time on aon side (in hus)
    int16_t aon_restore_save_time_diff_aon;
    /// Difference of restore time on aon side and save time on cpus side (in hus)
    int16_t aon_restore_save_time_diff_cpus_aon;
    /// Minimum time that allow clock gate (in hs)
    int32_t aon_min_clock_gate_duration;
    /// Minimum time that allow host to clock gate (in us)
    int32_t aon_min_clock_gate_duration_cpup;
    /// Maximum rf & mdm regs supported by aon
    uint16_t aon_max_nb_rf_mdm_regs;
}bt_drv_wr_aon_param;

/// Buffer structure
struct bt_buffer_tag
{
    /// length of buffer
    uint8_t length;
    /// data of 128 bytes length
    uint8_t data[128];
};

typedef struct
{
    uint8_t calib_type;
    uint16_t offset;
    struct bt_buffer_tag buf;
}bt_drv_rf_calib_req_cmd;

extern bool bt_get_fw_init_complete(void);
extern void bt_drv_poweron(uint8_t en);

const uint32_t rf_mdm_regs_table_bt_only[][2] =
{
#if (PLF_HW_ASIC == 1)
    //@ btrf
#if (PLF_WIFI_STACK == 0)
    {0x40580104, 0x000923FB},//for product bt only
#else
    {0x40580104, 0x000933FB},//for product bt+wifi
#endif
    {0x4062201C, 0x0008D000},
    {0x40622028, 0x48912020},//dvdd=1.18v
    {0x40622014, 0x00018983},
    {0x40622054, 0x00008F34},
    {0x40620748, 0x021A01A0},
    {0x40620728, 0x00010020},
    {0x40620738, 0x04800FD4},
    {0x4062073C, 0x00C80064},
    {0x4062202C, 0x000CB220},
    {0x4062200C, 0xE9AD2B45},
    #if (APP_SUPPORT_TWS == 1)
    // dpll for rx = 208m, (tws slave must use this config)
    {0x40622030, 0x143C30D2},
    #else
    // dpll for rx = 52m
    {0x40622030, 0x140C30D2},
    #endif
    //{0x40622000, 0x00000000},//default value
    {0x40622034, 0x00001602},
    {0x40620754, 0x214220FD},
    {0x40620758, 0x0007F01E},
    {0x4062071C, 0x00000A33},
    {0x40622018, 0x00124124},
    {0x4062000C, 0x04040000},
    //@ btrf tx
    {0x40620090, 0x00069082},
    {0x40621034, 0x02003080},
    {0x40621014, 0x0445117A},
    {0x40622024, 0x00001100},
    {0x40622004, 0x0001A9C0},

    //@ pwr up time
    {0x4060048C, 0x00500834},
    {0x40600110, 0x027E0058},
    {0x40600880, 0x00500834},
    {0x40600884, 0x00500834},
    {0x40600888, 0x00500834},
    {0x4060088C, 0x00000834},

#if GWB_AGC
    //@ wb agc
    {0x40620518, 0x3090880A},
    {0x40620514, 0xA80C1A10},
    {0x40620510, 0x1466FF0A},
    {0x406205B8, 0x00000000},
    {0x4062050C, 0x0A202013},

    {0x406205A0, 0x12140505},
    {0x406205A4, 0x42302E24},
#else
    //@ nb agc
    {0x40620518, 0x3692880A},// rsten_srrc
    {0x40620514, 0xA80C1A10},// dagc=0
    {0x4062052C, 0x9C0C1403},// dagc=3, lr
    {0x4062050C, 0x20202013},
    {0x406205A0, 0x14150C00},
    {0x406205A4, 0x362D3624},
    {0x406205F0, 0x0000FF00},
#endif
    //@ cm agc
    {0x40620508, 0x54553132},
    {0x40620530, 0x13171200},
    {0x40620534, 0x00000074},
    {0x406205B0, 0x00005355},
    //@ dc
    {0x4062051C, 0x964B5766},
#if GEN_ACI
    {0x40621878, 0x00000002},
    {0x4062157C, 0x00000040},
    {0x40621580, 0x00000040},
    {0x40621560, 0x051B1200},
    {0x40621564, 0x051B2200},
    {0x40621560, 0x09145640},
    {0x40621564, 0x051B1201},
#endif
#endif
};

const uint32_t rf_mdm_regs_table_bt_combo[][2] =
{
#if (PLF_HW_ASIC == 1)
    //@ btrf
#if (PLF_WIFI_STACK == 0)
    {0x40580104, 0x000923FB},//for product bt only
#else
    {0x40580104, 0x000933FB},//for product bt+wifi
#endif
    {0x40344020, 0x00000B77},
    {0x40344024, 0x006EC594},
    {0x40344028, 0x00009402},
    {0x4034402C, 0x56201884},
    {0x40344030, 0x1A2E5168},

    //@ pwr up time
    {0x4060048C, 0x00500834},
    {0x40600110, 0x027E0058},
    {0x40600880, 0x00500834},
    {0x40600884, 0x00500834},
    {0x40600888, 0x00500834},
    {0x4060088C, 0x00000834},

#if GWB_AGC
    //@ wb agc
    {0x40620518, 0x3090880A},
    {0x40620514, 0xA80C1A10},
    {0x40620510, 0x1466FF0A},
    {0x406205B8, 0x00000000},
    {0x4062050C, 0x0A202013},

    {0x40620508, 0x54553032},
    {0x406205A0, 0x1810120F},
    {0x406205A4, 0x372E2F2E},
    {0x406205F0, 0x00000077},
#else
    //@ nb agc
    {0x40620518, 0x3692880A},// rsten_srrc
    {0x40620514, 0xA80C1A10},// dagc=0
    {0x4062052C, 0x9C0C1403},// dagc=3, lr
    {0x4062050C, 0x20202013},
    {0x40620508, 0x54553132},
    {0x406205A0, 0x0F171600},
    {0x406205A4, 0x36283636},
    {0x406205F0, 0x0000FF00},
#endif
    //@ cm agc
    {0x40620530, 0x13171A00},
    {0x40620534, 0x00000076},
    {0x406205B0, 0x00005355},
    //@ dc
    {0x4062051C, 0x964B5766},
    //@ 26m cic
    {0x40620090, 0x00050032},
    //@ srrc rolloff = 0.305
    {0x40621010, 0x12000143},
#if GEN_ACI
    {0x40621878, 0x00000002},
    {0x4062157C, 0x00000040},
    {0x40621580, 0x00000040},
    {0x40621560, 0x051B1200},
    {0x40621564, 0x051B2200},
    {0x40621560, 0x09145640},
    {0x40621564, 0x051B1201},
#endif
#endif
};

#ifdef CFG_BTDM_RAM_VER
#if PLF_PMIC
#define AON_BT_PWR_ON_DLY1       (1 + 5)//+5 for safe(not necessary)
#define AON_BT_PWR_ON_DLY2       (10 + 48 + 5)//+48 for dp_open_delay, +5 for safe
#define AON_BT_PWR_ON_DLY3       (12 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe
#define AON_BT_PWR_ON_DLY_AON    (11 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe
#else
#define AON_BT_PWR_ON_DLY1       (1)
#define AON_BT_PWR_ON_DLY2       (10)
#define AON_BT_PWR_ON_DLY3       (12)
#define AON_BT_PWR_ON_DLY_AON    (11)
#endif
const bt_drv_wr_aon_param wr_aon_param =
{
    0x18D700, 0x18F700, 64, 40, 400, 400, 3, 2,
    3, 2, 40, 512, 20, 21, 20, 32,
    8, 0, 0, 20000, 0x101, 0x20067302, AON_BT_PWR_ON_DLY1, AON_BT_PWR_ON_DLY2,
    AON_BT_PWR_ON_DLY3, AON_BT_PWR_ON_DLY_AON, 32, 360, 420, 100, 100, 8,
    24, 40, 140, 0, 64, 20000, 50
};
#else
#if PLF_PMIC
#define AON_BT_PWR_ON_DLY1_U02      (1 + 5)//+5 for safe(not necessary)
#define AON_BT_PWR_ON_DLY2_U02      (10 + 48 + 5)//+48 for dp_open_delay, +5 for safe
#define AON_BT_PWR_ON_DLY3_U02      (12 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe
#define AON_BT_PWR_ON_DLY_AON_U02   (11 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe
#define AON_BT_PWR_ON_DLY1_U03      (1 + 5)//+5 for safe(not necessary)
#define AON_BT_PWR_ON_DLY2_U03      (10 + 48 + 5)//+48 for dp_open_delay, +5 for safe
#define AON_BT_PWR_ON_DLY3_U03      (12 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe
#define AON_BT_PWR_ON_DLY_AON_U03   (11 + 48 + 8 + 5)//+8 for more dp_open_delay than AON_BT_PWR_ON_DLY2, +5 for safe

#define AON_BT_PWR_ON_DLY1x(v)      MCAT(AON_BT_PWR_ON_DLY1_U0, v)
#define AON_BT_PWR_ON_DLY1          AON_BT_PWR_ON_DLY1x(CFG_ROM_VER)
#define AON_BT_PWR_ON_DLY2x(v)      MCAT(AON_BT_PWR_ON_DLY2_U0, v)
#define AON_BT_PWR_ON_DLY2          AON_BT_PWR_ON_DLY2x(CFG_ROM_VER)
#define AON_BT_PWR_ON_DLY3x(v)      MCAT(AON_BT_PWR_ON_DLY3_U0, v)
#define AON_BT_PWR_ON_DLY3          AON_BT_PWR_ON_DLY3x(CFG_ROM_VER)
#define AON_BT_PWR_ON_DLY_AONx(v)   MCAT(AON_BT_PWR_ON_DLY_AON_U0, v)
#define AON_BT_PWR_ON_DLY_AON       AON_BT_PWR_ON_DLY_AONx(CFG_ROM_VER)
#else
#define AON_BT_PWR_ON_DLY1          (1)
#define AON_BT_PWR_ON_DLY1x(v)      AON_BT_PWR_ON_DLY1
#define AON_BT_PWR_ON_DLY2          (10)
#define AON_BT_PWR_ON_DLY2x(v)      AON_BT_PWR_ON_DLY2
#define AON_BT_PWR_ON_DLY3          (12)
#define AON_BT_PWR_ON_DLY2x(v)      AON_BT_PWR_ON_DLY3
#define AON_BT_PWR_ON_DLY_AON       (11)
#define AON_BT_PWR_ON_DLY_AONx(v)   AON_BT_PWR_ON_DLY_AON
#endif

const bt_drv_wr_aon_param VAR_WITH_VERx(wr_aon_param, 2) =
{
    0x16D700, 0x16F680, 64, 40, 400, 400, 3, 2,
    3, 2, 40, 512, 20, 21, 20, 32,
    8, -2, 0, 20000, 0x101, 0x20067302, AON_BT_PWR_ON_DLY1x(2), AON_BT_PWR_ON_DLY2x(2),
    AON_BT_PWR_ON_DLY3x(2), AON_BT_PWR_ON_DLY_AONx(2), 32, 360, 420, 100, 100, 8,
    24, 40, 140, 0, 64, 20000, 50
};

const bt_drv_wr_aon_param VAR_WITH_VERx(wr_aon_param, 3) =
{
    0x16D700, 0x16F680, 64, 40, 400, 400, 3, 2,
    3, 2, 40, 512, 20, 21, 20, 32,
    8, -2, 0, 20000, 0x101, 0x20067302, AON_BT_PWR_ON_DLY1x(3), AON_BT_PWR_ON_DLY2x(3),
    AON_BT_PWR_ON_DLY3x(3), AON_BT_PWR_ON_DLY_AONx(3), 32, 360, 420, 100, 100, 8,
    24, 40, 140, 0, 64, 20000, 50
};
#endif

#if PLF_RF_MODE_BTWIFI_COMBO || PLF_RF_MODE_BT_COMBO
const BtDrvRfModeEnum bt_rf_mode = DRV_RF_MODE_BTWIFI_COMBO;
const bt_drv_rf_calib_req_cmd rf_calib_req = {DRV_RF_MODE_BTWIFI_COMBO, 0x0000, {0x04, {0x03,0x42,0x26,0x00}}};
#else
const BtDrvRfModeEnum bt_rf_mode = DRV_RF_MODE_BT_ONLY;
const bt_drv_rf_calib_req_cmd rf_calib_req = {DRV_RF_MODE_BT_ONLY, 0x0000, {0x08, {0x03,0x42,0x26,0x00,0x0f,0x30,0x02,0x00}}};
#endif

const bt_drv_wr_aon_param *wr_aon_param_ptr;
uint32_t aon_debug_level;
uint8_t pwr_ctrl_slave = 1;
// one of BT_LP_LEVEL_ACTIVE/BT_LP_LEVEL_CLOCK_GATE2/BT_LP_LEVEL_POWER_OFF3/BT_LP_LEVEL_HIBERNATE
uint8_t bt_lp_level = BT_LP_LEVEL_ACTIVE;
uint8_t bt_sleep_debug_level = 0xFF;

void aic_bt_start(void)
{
    if (!bt_get_fw_init_complete()) {
        #ifdef CFG_BTDM_RAM_VER
        wr_aon_param_ptr = &wr_aon_param;
        #else
        #if (CFG_ROM_VER == 255)
        uint8_t chip_id = ChipIdGet(0);
        if (chip_id == 0x03) {
            wr_aon_param_ptr = &VAR_WITH_VERx(wr_aon_param, 2);
        } else if (chip_id == 0x07) {
            wr_aon_param_ptr = &VAR_WITH_VERx(wr_aon_param, 3);
        }
        #else
        wr_aon_param_ptr = &VAR_WITH_VER(wr_aon_param);
        #endif
        bt_patch_prepare();
        #endif
        aon_debug_level = wr_aon_param_ptr->aon_debug_level;
        bt_drv_poweron(BT_POWERON);
    }
}

void bt_drv_lp_level_set(uint8_t level)
{
    // TODO
}

uint32_t host_power_on_mode(void)
{
    uint32_t value = *(uint32_t *)0x4050605C;
    dbg_test_print("AON:host_pwr_on_mode = %d\n", value & 3);
    return (value >> 1) & 1;
}

/*******************************************************************************
 * end of code copied from bt_aic8800_driver.c in vendor SDK
*******************************************************************************/

//implement_vsf_pool(hci_transport_aic8800_buffer_pool, hci_transport_aic8800_buffer_t)
#define __name hci_transport_aic8800_buffer_pool
#define __type hci_transport_aic8800_buffer_t
#include "service/pool/impl_vsf_pool.inc"

static uint32_t __hci_transport_aic8800_rx_handler(const uint8_t *data, uint32_t len)
{
    uint8_t *buffer = (uint8_t *)VSF_POOL_ALLOC(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool);
    VSF_HAL_ASSERT((buffer != NULL) && (len + 4 <= VSF_AIC8800_BTSTACK_CFG_MAX_PACKET_SIZE));

    *(uint32_t *)buffer = len;
    memcpy(&buffer[4], data, len);
    vsf_eda_post_msg(&__hci_transport_aic8800_param.task.use_as__vsf_eda_t, buffer);
    return len;
}

static void __hci_transport_aic8800_tx_handler(const uint8_t *data, uint32_t len)
{
    VSF_POOL_FREE(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool,
                    (hci_transport_aic8800_buffer_t *)data);
    vsf_eda_post_evt(&__hci_transport_aic8800_param.task.use_as__vsf_eda_t, VSF_EVT_USER + 0);
}

static void __hci_transport_aic8800_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    extern void aic_bt_start(void);

    switch (evt) {
    case VSF_EVT_TIMER:
        bt_hci_tx(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART, __hci_transport_aic8800_param.tx_buffer, __hci_transport_aic8800_param.tx_size);
        __hci_transport_aic8800_param.tx_buffer = NULL;
        break;
    case VSF_EVT_MESSAGE: {     // EVT_RX
            const uint8_t *msg = vsf_eda_get_cur_msg();
            const uint8_t *data = msg + 4;
            uint32_t len = *(uint32_t *)msg;

            VSF_HAL_ASSERT((__hci_transport_aic8800_param.packet_handler != NULL) && (len > 0));
            __hci_transport_aic8800_param.packet_handler(data[0], (uint8_t *)&data[1], len - 1);

            VSF_POOL_FREE(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool,
                    (hci_transport_aic8800_buffer_t *)msg);
        }
        break;
    case VSF_EVT_USER + 0: {    // EVT_RX
            const uint8_t event[] = { HCI_EVENT_TRANSPORT_PACKET_SENT, 0 };
            VSF_HAL_ASSERT(__hci_transport_aic8800_param.packet_handler != NULL);
            __hci_transport_aic8800_param.packet_handler(HCI_EVENT_PACKET, (uint8_t *)event, sizeof(event));
        }
        break;
    }
}

static int __hci_transport_aic8800_open(void)
{
    extern void aic_bt_start(void);

    __hci_transport_aic8800_param.task.fn.evthandler = __hci_transport_aic8800_evthandler;
    vsf_teda_init(&__hci_transport_aic8800_param.task, VSF_AIC8800_BTSTACK_CFG_PRIORITY);

    bt_hci_open(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART,
            __hci_transport_aic8800_rx_handler, __hci_transport_aic8800_tx_handler, false);
#ifdef VSF_AIC8800_BTSTACK_CFG_HW_PRIORITY
    NVIC_SetPriority(WCN2MCU0_IRQn, VSF_AIC8800_BTSTACK_CFG_HW_PRIORITY);
#endif
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
    return bt_hci_tx_available(BT_HCI_CH_0) && (NULL == __hci_transport_aic8800_param.tx_buffer);
}

WEAK(__hci_transport_aic8800_get_cmd_delay_ms)
uint_fast8_t __hci_transport_aic8800_get_cmd_delay_ms(uint8_t *packet, int size)
{
    return 0;
}

static int __hci_transport_aic8800_send_packet(uint8_t packet_type, uint8_t *packet, int size)
{
    uint8_t *buffer = (uint8_t *)VSF_POOL_ALLOC(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool);
    VSF_HAL_ASSERT((buffer != NULL) && (size < VSF_AIC8800_BTSTACK_CFG_MAX_PACKET_SIZE));
    buffer[0] = packet_type;
    memcpy(&buffer[1], packet, size);
    size++;

    uint_fast8_t ms = __hci_transport_aic8800_get_cmd_delay_ms(buffer, size);
    if (ms > 0) {
        __hci_transport_aic8800_param.tx_buffer = buffer;
        __hci_transport_aic8800_param.tx_size = size;
        vsf_teda_set_timer_ex(&__hci_transport_aic8800_param.task, vsf_systimer_ms_to_tick(ms));
        return 0;
    } else {
        return bt_hci_tx(BT_HCI_CH_0, BT_HCI_PKT_TYPE_HCI_UART, buffer, size);
    }
}

static void __hci_transport_aic8800_init(const void *transport_config)
{
    VSF_POOL_INIT(hci_transport_aic8800_buffer_pool, &__hci_transport_aic8800_param.buffer_pool, 8,
        .region_ptr     = (vsf_protect_region_t *)&vsf_protect_region_int,
    );

    aic_bt_start();
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

    const uint32_t (*rf_mdm_regs_table)[2];
    uint32_t rf_mdm_regs_table_len;
    if (bt_rf_mode == DRV_RF_MODE_BT_ONLY) {
        rf_mdm_regs_table_len = dimof(rf_mdm_regs_table_bt_only);
        rf_mdm_regs_table = rf_mdm_regs_table_bt_only;
    } else if ((bt_rf_mode == DRV_RF_MODE_BT_COMBO) || (bt_rf_mode == DRV_RF_MODE_BTWIFI_COMBO)) {
        rf_mdm_regs_table_len = dimof(rf_mdm_regs_table_bt_combo);
        rf_mdm_regs_table = rf_mdm_regs_table_bt_combo;
    }

    if (0 == init_script_offset) {
        // bt_drv_wr_aon_param
        hci_cmd_buffer[0] = 0x4D;
        hci_cmd_buffer[1] = 0xFC;
        hci_cmd_buffer[2] = sizeof(bt_drv_wr_aon_param) - 2;
        memcpy(&hci_cmd_buffer[3], wr_aon_param_ptr, hci_cmd_buffer[2]);
    } else if (1 == init_script_offset) {
        // aon_debug_level
        hci_cmd_buffer[0] = 0x4F;
        hci_cmd_buffer[1] = 0xFC;
        hci_cmd_buffer[2] = sizeof(aon_debug_level);
        memcpy(&hci_cmd_buffer[3], &aon_debug_level, sizeof(aon_debug_level));
    } else if (init_script_offset < 2 + rf_mdm_regs_table_len) {
        init_script_offset -= 2;

        // wr_rf_mdm_regs
        uint32_t reg_addr = rf_mdm_regs_table[init_script_offset][0];
        uint32_t reg_val = rf_mdm_regs_table[init_script_offset][1];

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
    } else if (init_script_offset == (2 + rf_mdm_regs_table_len)) {
        // set_rf_mode
        const uint8_t hci_cmd_set_rf_mode[] = {
            0x48, 0xFC, 0x01, (uint8_t)bt_rf_mode,
        };
        memcpy(hci_cmd_buffer, hci_cmd_set_rf_mode, sizeof(hci_cmd_set_rf_mode));
    } else if (init_script_offset == (3 + rf_mdm_regs_table_len)) {
        // rf_calib_req
        hci_cmd_buffer[0] = 0x4B;
        hci_cmd_buffer[1] = 0xFC;
        hci_cmd_buffer[2] = 132;

        hci_cmd_buffer[3] = rf_calib_req.calib_type;
        hci_cmd_buffer[4] = (uint8_t)(rf_calib_req.offset >> 0);
        hci_cmd_buffer[5] = (uint8_t)(rf_calib_req.offset >> 8);
        hci_cmd_buffer[6] = rf_calib_req.buf.length;
        memcpy(&hci_cmd_buffer[7], rf_calib_req.buf.data, rf_calib_req.buf.length);
    } else if (init_script_offset == (4 + rf_mdm_regs_table_len)) {
        // pwr_ctrl_slave
        const uint8_t hci_cmd_set_pwr_ctrl_slave[] = {
            0x51, 0xFC, 0x01, pwr_ctrl_slave,
        };
        memcpy(hci_cmd_buffer, hci_cmd_set_pwr_ctrl_slave, sizeof(hci_cmd_set_pwr_ctrl_slave));
    } else if (init_script_offset == (5 + rf_mdm_regs_table_len)) {
        // bt_lp_level
        const uint8_t hci_cmd_set_bt_lp_level[] = {
            0x50, 0xFC, 0x01, (uint8_t)bt_lp_level,
        };
        memcpy(hci_cmd_buffer, hci_cmd_set_bt_lp_level, sizeof(hci_cmd_set_bt_lp_level));
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
